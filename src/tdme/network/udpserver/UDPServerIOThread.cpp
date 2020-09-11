#include <string.h>

#include <iostream>
#include <map>
#include <string>
#include <typeinfo>

#include <tdme/os/network/KernelEventMechanism.h>
#include <tdme/os/network/NIOInterest.h>
#include <tdme/os/threading/Thread.h>
#include <tdme/os/threading/Mutex.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>
#include <tdme/utilities/RTTI.h>
#include <tdme/utilities/Time.h>
#include <tdme/network/udpserver/UDPServerIOThread.h>
#include <tdme/network/udpserver/ServerRequest.h>

using std::pair;
using std::ios_base;
using std::map;
using std::string;
using std::to_string;

using tdme::os::network::KernelEventMechanism;
using tdme::os::network::NIOInterest;
using tdme::os::network::NIO_INTEREST_NONE;
using tdme::os::network::NIO_INTEREST_READ;
using tdme::os::network::NIO_INTEREST_WRITE;
using tdme::os::threading::Mutex;
using tdme::os::threading::Thread;
using tdme::utilities::Console;
using tdme::utilities::Exception;
using tdme::utilities::RTTI;
using tdme::utilities::Time;
using tdme::network::udpserver::UDPServerIOThread;
using tdme::network::udpserver::ServerRequest;

const uint64_t UDPServerIOThread::MESSAGEACK_RESENDTIMES[UDPServerIOThread::MESSAGEACK_RESENDTIMES_TRIES] = {125L, 250L, 500L, 750L, 1000L, 2000L, 5000L};

UDPServerIOThread::UDPServerIOThread(const unsigned int id, UDPServer *server, const unsigned int maxCCU) :
	Thread("nioudpserveriothread"),
	id(id),
	server(server),
	maxCCU(maxCCU),
	messageQueueMutex("nioupserveriothread_messagequeue"),
	messageMapAckMutex("nioupserveriothread_messagequeueack") {
	//
}

void UDPServerIOThread::run() {
	Console::println("UDPServerIOThread[" + to_string(id) + "]::run(): start");

	// wait on startup barrier
	server->startUpBarrier->wait();

	// catch kernel event and server socket exceptions
	try {
		// create server socket
		UDPSocket::createServerSocket(socket, server->host, server->port);

		// initialize kernel event mechanismn
		kem.initKernelEventMechanism(1);
		kem.setSocketInterest(socket, NIO_INTEREST_NONE, NIO_INTEREST_READ, NULL);

		// do event loop
		uint64_t lastMessageQueueAckTime = Time::getCurrentMillis();
		while(isStopRequested() == false) {
			uint64_t now = Time::getCurrentMillis();

			// process ack messages every 25ms
			if (now >= lastMessageQueueAckTime + 25L) {
				processAckMessages();
				lastMessageQueueAckTime = now;
			}

			// do kernel event mechanism
			int events = kem.doKernelEventMechanism();

			// iterate the event list
			for(unsigned int i = 0; i < (unsigned int)events; i++) {
				NIOInterest keInterest;
				void* nil;

				// decode kernel event
				kem.decodeKernelEvent(i, keInterest, (void*&)nil);

				// interests
				bool hasReadInterest = (keInterest & NIO_INTEREST_READ) == NIO_INTEREST_READ;
				bool hasWriteInterest = (keInterest & NIO_INTEREST_WRITE) == NIO_INTEREST_WRITE;

				// process read interest
				if (hasReadInterest) {
					ssize_t bytesReceived;
					string ip;
					unsigned int port;
					char message[512];

					// receive datagrams as long as its size > 0 and read would not block
					while ((bytesReceived = socket.read(ip, port, (void*)message, sizeof(message))) > 0) {
						// process event, catch and handle client related exceptions
						UDPServerClient* client = NULL;
						UDPServerClient* clientNew = NULL;
						stringstream* frame = NULL;
						try {
							// transfer buffer to string stream
							frame = new stringstream();
							frame->write(message, bytesReceived);

							// validate datagram
							server->validate(frame);

							// identify datagram
							UDPServer::MessageType messageType;
							uint32_t clientId;
							uint32_t messageId;
							uint8_t retries;
							server->identify(frame, messageType, clientId, messageId, retries);

							// process message depending on messageType
							switch(messageType) {
								case(UDPServer::MESSAGETYPE_CONNECT):
									{
										// check if client is connected already
										client = server->getClientByIp(ip, port);
										if (client != NULL) {
											// delete frame
											delete frame;
											frame = NULL;
											client->sendConnected();
											client->releaseReference();
											// we are done
											break;
										}

										// create client
										clientNew = server->accept(
											server->allocateClientId(),
											ip,
											port
										);

										// assign server
										clientNew->server = server;

										// add client to server
										server->addClient(clientNew);

										// delete frame
										delete frame;
										frame = NULL;

										// switch from client new to client
										client = clientNew;
										clientNew = NULL;

										// send connected ack
										client->sendConnected();

										// set/register client in Server
										if (client->setKey(client->getKey()) == false) {
											throw NetworkServerException("Client key is already in use");
										}

										// fire on init
										client->init();

										// we are done
										break;
									}
								case(UDPServer::MESSAGETYPE_MESSAGE):
									{
										// look up client
										client = server->lookupClient(clientId);
										// check if client ip, port matches datagram ip and prt
										if (client->ip != ip || client->port != port) {
											//
											client->releaseReference();
											throw NetworkServerException("message invalid");
										}
										// delegate
										client->onFrameReceived(frame, messageId, retries);
										break;
									}
								case(UDPServer::MESSAGETYPE_ACKNOWLEDGEMENT):
									{
										client = server->lookupClient(clientId);
										server->processAckReceived(client, messageId);
										delete frame;
										frame = NULL;
										break;
									}
								default:
									throw NetworkServerException("Invalid message type");
							}
						} catch(Exception& exception) {
							// delete frame
							if (frame != NULL) delete frame;

							// log
							Console::println(
								"UDPServerIOThread[" +
								to_string(id) +
								"]::run(): " +
								(RTTI::demangle(typeid(exception).name())) +
								": " +
								(exception.what())
							);

							if (clientNew != NULL) {
								delete clientNew;
							}
							// in case it was a client related exception
							if (client != NULL) {
								// otherwise shut down client
								client->shutdown();
							}
						}
					}
				}

				// process write interest
				while (hasWriteInterest) {
					// fetch batch of messages to be send
					MessageQueue messageQueueBatch;
					messageQueueMutex.lock();
					for (int i = 0; i < MESSAGEQUEUE_SEND_BATCH_SIZE && messageQueue.empty() == false; i++) {
						Message* message = &messageQueue.front();
						messageQueueBatch.push(*message);
						messageQueue.pop();
					}
					messageQueueMutex.unlock();

					// try to send batch
					while (messageQueueBatch.empty() == false) {
						Message* message = &messageQueueBatch.front();
						if (socket.write(message->ip, message->port, (void*)message->message, message->bytes) == -1) {
							// sending would block, stop trying to sendin
							break;
						} else {
							// success, remove message from message queue batch and continue
							messageQueueBatch.pop();
						}
					}

					// re add messages not sent in batch to message queue
					if (messageQueueBatch.empty() == true) {
						messageQueueMutex.lock();
						if (messageQueue.empty() == true) {
							kem.setSocketInterest(
								socket,
								NIO_INTEREST_READ | NIO_INTEREST_WRITE,
								NIO_INTEREST_READ,
								NULL
							);

							// no more data to send, so stop the loop
							hasWriteInterest = false;
						}
						messageQueueMutex.unlock();
					} else {
						messageQueueMutex.lock();
						do {
							Message* message = &messageQueueBatch.front();
							messageQueue.push(*message);
							messageQueueBatch.pop();
						} while (messageQueueBatch.empty() == false);
						messageQueueMutex.unlock();

						// we did not send all batched messages, so stop the loop
						hasWriteInterest = false;
					}
				}
			}
		}

	//
	} catch (Exception &exception) {
		// log
		Console::println(
			"UDPServerIOThread[" +
			to_string(id) +
			"]::run(): " +
			(RTTI::demangle(typeid(exception).name())) +
			": " +
			(exception.what())
		);
	}

	// exit gracefully
	kem.shutdownKernelEventMechanism();
	socket.close();

	// log
	Console::println("UDPServerIOThread[" + to_string(id) + "]::run(): done");
}

void UDPServerIOThread::sendMessage(const UDPServerClient* client, const uint8_t messageType, const uint32_t messageId, stringstream* frame, const bool safe, const bool deleteFrame) {
	// FIXME:
	//	We could use lock free queues here
	//	For now, we will go with plain mutexes

	// reset stream for read
	frame->seekg(0, ios_base::beg);
	frame->seekp(0, ios_base::end);

	// create message
	Message message;
	message.ip = client->ip;
	message.port = client->port;
	message.time = Time::getCurrentMillis();
	message.messageType = messageType;
	message.clientId = client->clientId;
	message.messageId = messageId;
	message.retries = 0;
	message.bytes = frame->tellp();
	if (message.bytes > 512) message.bytes = 512;
	frame->read(message.message, message.bytes);

	if (deleteFrame == true) delete frame;

	// requires ack and retransmission ?
	if (safe == true) {
		// 	create message ack
		Message messageAck;
		messageAck = message;

		messageMapAckMutex.lock();
		MessageMapAck::iterator it;
		// 	check if message has already be pushed to ack
		it = messageMapAck.find(messageId);
		if (it != messageMapAck.end()) {
 			// its on ack queue already, so unlock
			messageMapAckMutex.unlock();
			throw NetworkServerException("message already on message queue ack");
		}
		//	check if message queue is full
		if (messageMapAck.size() > maxCCU * 20) {
			messageMapAckMutex.unlock();
			throw NetworkServerException("message queue ack overflow");
		}
		// 	push to message queue ack
		messageMapAck.insert(it, pair<uint32_t, Message>(messageId, messageAck));
		messageMapAckMutex.unlock();
	}

	// push to message queue
	messageQueueMutex.lock();

	//	check if message queue is full
	if (messageQueue.size() > maxCCU * 20) {
		messageQueueMutex.unlock();
		throw NetworkServerException("message queue overflow");
	}
	messageQueue.push(message);

	// set nio interest
	if (messageQueue.size() == 1) {
		kem.setSocketInterest(
			socket,
			NIO_INTEREST_READ,
			NIO_INTEREST_READ | NIO_INTEREST_WRITE,
			NULL
		);
	}

	// done
	messageQueueMutex.unlock();
}

void UDPServerIOThread::processAckReceived(UDPServerClient* client, const uint32_t messageId) {
	bool messageAckValid = true;
	MessageMapAck::iterator iterator;

	// delete message from message queue ack
	messageMapAckMutex.lock();
	iterator = messageMapAck.find(messageId);
	if (iterator != messageMapAck.end()) {
		// message exists
		Message* messageAck = &iterator->second;
		// message ack valid?
		messageAckValid = messageAck->ip == client->ip && messageAck->port == client->port;
		// remove if valid
		if (messageAckValid == true) {
			// remove message from message queue ack
			messageMapAck.erase(iterator);
		}
	}
	messageMapAckMutex.unlock();

	//
	client->releaseReference();

	// check if message ack was valid
	if (messageAckValid == false) {
		throw NetworkServerException("message ack invalid");
	}
}

void UDPServerIOThread::processAckMessages() {
	MessageQueue messageQueueResend;
	uint64_t now = Time::getCurrentMillis();

	messageMapAckMutex.lock();
	MessageMapAck::iterator it = messageMapAck.begin();
	while (it != messageMapAck.end()) {
		Message* messageAck = &it->second;
		// message ack timed out?
		//	most likely the client is gone
		if (messageAck->retries == MESSAGEACK_RESENDTIMES_TRIES) {
			// delete from message map ack
			messageMapAck.erase(it++);
			// skip
			continue;
		} else
		// message should be resend?
		if (now > (messageAck->time + (MESSAGEACK_RESENDTIMES[messageAck->retries]))) {
			// increase tries
			messageAck->retries++;

			// construct message
			Message message = *messageAck;

			// recreate frame header with updated hash and retries
			stringstream frame;
			frame.write(message.message, message.bytes);
			server->writeHeader(&frame, (UDPServer::MessageType)message.messageType, message.clientId, message.messageId, message.retries);
			frame.read(message.message, message.bytes);

			// and push to be resent
			messageQueueResend.push(message);
		}
		++it;
	}
	messageMapAckMutex.unlock();

	// reissue messages to be resent
	if (messageQueueResend.empty() == false) {
		messageQueueMutex.lock();
		do {
			Message* message = &messageQueueResend.front();
			messageQueue.push(*message);
			messageQueueResend.pop();

			// set nio interest
			if (messageQueue.size() == 1) {
				kem.setSocketInterest(
					socket,
					NIO_INTEREST_READ,
					NIO_INTEREST_READ | NIO_INTEREST_WRITE,
					NULL
				);
			}
		} while (messageQueueResend.empty() == false);
		messageQueueMutex.unlock();
	}
}
