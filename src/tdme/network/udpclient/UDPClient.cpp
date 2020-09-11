#include <string.h>

#include <iostream>
#include <sstream>
#include <map>
#include <string>
#include <typeinfo>

#include <tdme/os/network/KernelEventMechanism.h>
#include <tdme/os/network/NIOInterest.h>
#include <tdme/os/network/NetworkSocket.h>
#include <tdme/os/threading/Thread.h>
#include <tdme/os/threading/Mutex.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>
#include <tdme/utilities/RTTI.h>
#include <tdme/utilities/Time.h>
#include <tdme/network/udpclient/UDPClient.h>
#include <tdme/network/udpclient/UDPClientMessage.h>
#include <tdme/network/udpclient/NetworkClientException.h>

using std::pair;
using std::ios;
using std::map;
using std::stringstream;
using std::string;
using std::string;
using std::to_string;

using tdme::os::network::KernelEventMechanism;
using tdme::os::network::NIOInterest;
using tdme::os::network::NetworkSocket;
using tdme::os::network::NIO_INTEREST_NONE;
using tdme::os::network::NIO_INTEREST_READ;
using tdme::os::network::NIO_INTEREST_WRITE;
using tdme::os::threading::Mutex;
using tdme::os::threading::Thread;
using tdme::utilities::Console;
using tdme::utilities::Exception;
using tdme::utilities::RTTI;
using tdme::utilities::Time;
using tdme::network::udpclient::UDPClient;
using tdme::network::udpclient::UDPClientMessage;
using tdme::network::udpclient::NetworkClientException;

const uint64_t UDPClient::MESSAGEACK_RESENDTIMES[UDPClient::MESSAGEACK_RESENDTIMES_TRIES] = {125L, 250L, 500L, 750L, 1000L, 2000L, 5000L};

UDPClient::UDPClient(const string& ip, const unsigned int port) :
	Thread("nioudpclientthread"),
	messageQueueMutex("nioupclientthread_messagequeue"),
	messageMapAckMutex("nioupclientthread_messagequeueack"),
	recvMessageQueueMutex("nioupclientthread_recvmessagequeuemutex"),
	messageMapSafeMutex("nioupclientthread_messagemasafemutex"),
	ip(ip),
	port(port),
	clientId(0),
	messageCount(0),
	initialized(false),
	connected(false) {
	//
}

bool UDPClient::isInitialized() {
	return initialized;
}

bool UDPClient::isConnected() {
	return connected;
}

const string& UDPClient::getIp() {
	return ip;
}

const unsigned int UDPClient::getPort() {
	return port;
}

const string& UDPClient::getClientKey() {
	return clientKey;
}

void UDPClient::setClientKey(const string& clientKey) {
	this->clientKey = clientKey;
}

void UDPClient::run() {
	Console::println("UDPClient::run(): start");

	// catch kernel event and server socket exceptions
	try {
		// create client socket
		UDPSocket::createClientSocket(socket, NetworkSocket::determineIpVersion(ip));

		// initialize kernel event mechanismn
		kem.initKernelEventMechanism(1);
		kem.setSocketInterest(socket, NIO_INTEREST_NONE, NIO_INTEREST_READ, nullptr);

		// initialized
		initialized = true;

		// do event loop
		uint64_t lastMessageQueueAckTime = Time::getCurrentMillis();
		uint64_t lastMessageConnectTime = Time::getCurrentMillis();
		uint64_t lastMessageSafeCleanTime = Time::getCurrentMillis();
		while(isStopRequested() == false) {
			uint64_t now = Time::getCurrentMillis();

			// process connect messages every 25ms
			if (connected == false && now >= lastMessageConnectTime + 25L) {
				// send connection message
				sendMessage(new UDPClientMessage(UDPClientMessage::MESSAGETYPE_CONNECT, 0, 0, 0, nullptr), false);
				lastMessageConnectTime = now;
			}

			// process ack messages every 25ms
			if (now >= lastMessageQueueAckTime + 25L) {
				processAckMessages();
				lastMessageQueueAckTime = now;
			}

			// process save messages clean up every 25ms
			if (now >= lastMessageSafeCleanTime + 25L) {
				cleanUpSafeMessages();
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
					string fromIp;
					unsigned int fromPort;
					char message[512];

					// receive datagrams as long as its size > 0 and read would not block
					while ((bytesReceived = socket.read(fromIp, fromPort, (void*)message, sizeof(message))) > 0) {
						UDPClientMessage* clientMessage = UDPClientMessage::parse(message, bytesReceived);
						try {
							if (clientMessage == nullptr) {
								throw NetworkClientException("invalid message");
							}
							switch(clientMessage->getMessageType()) {
								case UDPClientMessage::MESSAGETYPE_ACKNOWLEDGEMENT:
									{
										processAckReceived(clientMessage->getMessageId());
										delete clientMessage;
										break;
									}
								case UDPClientMessage::MESSAGETYPE_CONNECT:
									{
										sendMessage(
											new UDPClientMessage(
												UDPClientMessage::MESSAGETYPE_ACKNOWLEDGEMENT,
												clientMessage->getClientId(),
												clientMessage->getMessageId(),
												clientMessage->getRetryCount() + 1,
												nullptr
											),
											false
										);
										clientId = clientMessage->getClientId();
										// read client key
										auto frame = clientMessage->getFrame();
										clientKey = "";
										uint8_t clientKeySize;
										char c;
										frame->read((char*)&clientKeySize, 1);
										for (uint8_t i = 0; i < clientKeySize; i++) {
											frame->read(&c, 1);
											clientKey+= c;
										}
										delete clientMessage;
										// we are connected
										connected = true;
										break;
									}
								case UDPClientMessage::MESSAGETYPE_MESSAGE:
									{
										//	check if message queue is full
										recvMessageQueueMutex.lock();
										if (recvMessageQueue.size() > 1000) {
											recvMessageQueueMutex.unlock();
											throw NetworkClientException("recv message queue overflow");
										}
										recvMessageQueue.push(clientMessage);
										recvMessageQueueMutex.unlock();
										break;
									}
								case UDPClientMessage::MESSAGETYPE_NONE:
									{
										break;
									}
							}
						} catch (Exception &exception) {
							if (clientMessage != nullptr) delete clientMessage;

							// log
							Console::println(
								"UDPClient::run(): " +
								(RTTI::demangle(typeid(exception).name())) +
								": " +
								(exception.what())
							);

							// rethrow to quit communication for now
							// TODO: maybe find a better way to handle errors
							//	one layer up should be informed about network client problems somehow
							throw exception;
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
						if (socket.write(ip, port, (void*)message->message, message->bytes) == -1) {
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
			"UDPClient::run(): " +
			(RTTI::demangle(typeid(exception).name())) +
			": " +
			(exception.what())
		);
	}

	// exit gracefully
	kem.shutdownKernelEventMechanism();
	socket.close();

	// log
	Console::println("UDPClient::run(): done");
}

void UDPClient::sendMessage(UDPClientMessage* clientMessage, bool safe) {
	// create message
	Message message;
	message.time = clientMessage->getTime();
	message.messageType = clientMessage->getMessageType();
	message.messageId = clientMessage->getMessageId();
	message.retries = 0;
	clientMessage->generate(message.message, message.bytes);
	delete clientMessage;

	// requires ack and retransmission ?
	if (safe == true) {
		// 	create message ack
		Message messageAck;
		messageAck = message;

		messageMapAckMutex.lock();
		MessageMapAck::iterator it;
		// 	check if message has already be pushed to ack
		it = messageMapAck.find(message.messageId);
		if (it != messageMapAck.end()) {
 			// its on ack queue already, so unlock
			messageMapAckMutex.unlock();
			throw NetworkClientException("message already on message queue ack");
		}
		//	check if message queue is full
		if (messageMapAck.size() > 1000) {
			messageMapAckMutex.unlock();
			throw NetworkClientException("message queue ack overflow");
		}
		// 	push to message queue ack
		messageMapAck.insert(it, pair<uint32_t, Message>(message.messageId, messageAck));
		messageMapAckMutex.unlock();
	}

	// push to message queue
	messageQueueMutex.lock();

	//	check if message queue is full
	if (messageQueue.size() > 1000) {
		messageQueueMutex.unlock();
		throw NetworkClientException("message queue overflow");
	}
	messageQueue.push(message);

	// set nio interest
	if (messageQueue.size() == 1) {
		kem.setSocketInterest(
			socket,
			NIO_INTEREST_READ,
			NIO_INTEREST_READ | NIO_INTEREST_WRITE,
			nullptr
		);
	}

	// done
	messageQueueMutex.unlock();
}

void UDPClient::processAckReceived(const uint32_t messageId) {
	bool messageAckValid = true;
	MessageMapAck::iterator iterator;

	// delete message from message queue ack
	messageMapAckMutex.lock();
	iterator = messageMapAck.find(messageId);
	if (iterator != messageMapAck.end()) {
		// message ack valid?
		messageAckValid = true; //messageAck->ip == client->ip && messageAck->port == client->port;
		// remove if valid
		if (messageAckValid == true) {
			// remove message from message queue ack
			messageMapAck.erase(iterator);
		}
	}
	messageMapAckMutex.unlock();

	// check if message ack was valid
	if (messageAckValid == false) {
		throw NetworkClientException("message ack invalid");
	}
}

void UDPClient::processAckMessages() {
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

			// parse client message from message raw data
			UDPClientMessage* clientMessage = UDPClientMessage::parse(message.message, message.bytes);

			// increase/set retry
			clientMessage->retry();

			// recreate message
			clientMessage->generate(message.message, message.bytes);

			// delete client message
			delete clientMessage;

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

bool UDPClient::processSafeMessage(UDPClientMessage* clientMessage) {
	bool messageProcessed = false;
	MessageMapSafe::iterator it;
	auto messageId = clientMessage->getMessageId();

	//
	messageMapSafeMutex.lock();

	// check if message has been already processed
	it = messageMapSafe.find(messageId);
	if (it != messageMapSafe.end()) {
		// yep, we did
		messageProcessed = true;
		SafeMessage& message = it->second;
		message.receptions++;
	} else {
		// nope, just remember message
		SafeMessage message;
		message.messageId = messageId;
		message.receptions = 1;
		message.time = Time::getCurrentMillis();
		// TODO: check for overflow
		messageMapSafe.insert(it, pair<uint32_t, SafeMessage>(messageId, message));
	}

	//
	messageMapSafeMutex.unlock();

	// always send ack
	sendMessage(
		new UDPClientMessage(
			UDPClientMessage::MESSAGETYPE_ACKNOWLEDGEMENT,
			clientId,
			clientMessage->getMessageId(),
			0,
			nullptr
		),
		false
	);

	// return if message should be processed
	return messageProcessed == true?false:true;
}


void UDPClient::cleanUpSafeMessages() {
	//
	messageMapSafeMutex.lock();

	// check if message has been already processed
	uint64_t now = Time::getCurrentMillis();
	MessageMapSafe::iterator it = messageMapSafe.begin();
	while (it != messageMapSafe.end()) {
		SafeMessage& message = it->second;
		if (message.time < now - MESSAGESSAFE_KEEPTIME) {
			messageMapSafe.erase(it++);
			continue;
		}
		++it;
	}

	//
	messageMapSafeMutex.unlock();
}

uint64_t UDPClient::getRetryTime(const uint8_t retries) {
	if (retries == 0) return 0L;
	if (retries > UDPClient::MESSAGEACK_RESENDTIMES_TRIES) return 0L;
	return UDPClient::MESSAGEACK_RESENDTIMES[retries - 1];
}

UDPClientMessage* UDPClient::receiveMessage() {
	UDPClientMessage* message = nullptr;
	recvMessageQueueMutex.lock();
	if (recvMessageQueue.empty() == false) {
		message = recvMessageQueue.front();
		recvMessageQueue.pop();
	}
	recvMessageQueueMutex.unlock();
	return message;
}

UDPClientMessage* UDPClient::createMessage(stringstream* frame) {
	return new UDPClientMessage(
		UDPClientMessage::MESSAGETYPE_MESSAGE,
		clientId,
		messageCount++,
		0,
		frame
	);
}
