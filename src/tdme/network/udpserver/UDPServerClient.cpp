#include <stdio.h>

#include <map>
#include <sstream>
#include <string>
#include <typeinfo>

#include <tdme/network/udpserver/UDPServerClient.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/IntEncDec.h>
#include <tdme/utilities/RTTI.h>
#include <tdme/utilities/Time.h>

using std::ios_base;
using std::map;
using std::pair;
using std::string;
using std::stringstream;

using tdme::network::udpserver::UDPServerClient;
using tdme::utilities::Console;
using tdme::utilities::IntEncDec;
using tdme::utilities::RTTI;
using tdme::utilities::Time;

UDPServerClient::UDPServerClient(const uint32_t clientId, const string& ip, const unsigned int port) :
	server(NULL),
	ioThread(NULL),
	clientId(clientId),
	ip(ip),
	port(port),
	shutdownRequested(false),
	messageMapSafeMutex("nioudpserverclient_messagemapsafe") {
	// key
	ostringstream tmp;
	tmp << KEY_PREFIX_UNNAMED;
	tmp << clientId;
	key = tmp.str();
}

UDPServerClient::~UDPServerClient() {
}

UDPServer* UDPServerClient::getServer() {
	return server;
}

const uint32_t UDPServerClient::getClientId() {
	return clientId;
}

const string& UDPServerClient::getIp() const {
	return ip;
}

const unsigned int UDPServerClient::getPort() const {
	return port;
}

const string& UDPServerClient::getKey() const {
	return key;
}

const bool UDPServerClient::setKey(const string &key) {
	if (key.size() > 0 &&
		key.size() < 256 &&
		server->setClientKey(this, key) == true) {
		this->key = key;
		return true;
	} else {
		return false;
	}
}

stringstream* UDPServerClient::createFrame() {
	stringstream* frame = new stringstream();
	frame->exceptions(std::ios_base::failbit | std::ios_base::badbit);
	UDPServer::initializeHeader(frame);
	return frame;
}

void UDPServerClient::send(stringstream* frame, bool safe, bool deleteFrame) {
	try {
		// seek writing to end of stream
		frame->seekp(0, ios_base::end);

		// check size
		if (frame->tellp() > 512) {
			delete frame;
			throw NetworkServerException("message too big");
		}

		server->sendMessage(this, frame, safe, deleteFrame, UDPServer::MESSAGETYPE_MESSAGE);
	} catch (NetworkServerException &exception) {
		// shut down client
		shutdown();

		// log
		Console::println(
			"UDPServerClient::send(): send failed for client '" +
			(ip) +
			"': " +
			(RTTI::demangle(typeid(exception).name())) +
			": " +
			(exception.what())
		);
	}
}

bool UDPServerClient::processSafeMessage(const uint32_t messageId) {
	bool messageProcessed = false;
	MessageMapSafe::iterator it;

	//
	messageMapSafeMutex.lock();

	// check if message has been already processed
	it = messageMapSafe.find(messageId);
	if (it != messageMapSafe.end()) {
		// yep, we did
		messageProcessed = true;
		Message* message = &it->second;
		message->receptions++;
	} else {
		// nope, just remember message
		Message message;
		message.messageId = messageId;
		message.receptions = 1;
		message.time = Time::getCurrentMillis();
		// TODO: check for overflow
		messageMapSafe.insert(it, pair<uint32_t, Message>(messageId, message));
	}

	//
	messageMapSafeMutex.unlock();

	// always send acknowlegdement to client
	stringstream* frame = createFrame();
	try {
		server->sendMessage(this, frame, false, true, UDPServer::MESSAGETYPE_ACKNOWLEDGEMENT, messageId);
	} catch (NetworkServerException &exception) {
		// shut down client
		shutdown();

		// log
		Console::println(
			"UDPServerClient::sendAcknowledgement(): send failed for client '" +
			(ip) +
			"': " +
			(RTTI::demangle(typeid(exception).name())) +
			": " +
			(exception.what())
		);
	}

	// return if message should be processed
	return messageProcessed == true?false:true;
}

void UDPServerClient::sendConnected() {
	stringstream* frame = createFrame();
	try {
		char keySize = key.size();
		frame->write(&keySize, 1);
		frame->write(key.data(), keySize);
		server->sendMessage(this, frame, true, true, UDPServer::MESSAGETYPE_CONNECT);
	} catch (NetworkServerException &exception) {
		// shut down client
		shutdown();

		// log
		Console::println(
			"UDPServerClient::sendConnected(): send failed for client '" +
			(ip) +
			"': " +
			(RTTI::demangle(typeid(exception).name())) +
			": " +
			(exception.what())
		);
	}
}

void UDPServerClient::shutdown() {
	shutdownRequested = true;
}

void UDPServerClient::onFrameReceived(stringstream* frame, const uint32_t messageId, const uint8_t retries) {
	// create request
	ServerRequest* request = new ServerRequest(
		ServerRequest::REQUESTTYPE_CLIENT_REQUEST,
		this,
		ServerRequest::EVENT_CUSTOM_NONE,
		frame,
		messageId,
		retries
	);
	// delegate it to thread pool, but make it declinable
	if (server->workerThreadPool->addElement(request, true) == false) {
		// element was declined
		Console::println("UDPServerClient::onFrameReceived(): client request declined from '" + (ip) + "'. Shutting down client");
		// 	release client reference
		releaseReference();
		// 	delete frame
		delete frame;
		// 	delete request
		delete request;
		// 	shutdown client
		shutdown();
	}
}

void UDPServerClient::close() {
	// create request
	ServerRequest* request = new ServerRequest(
		ServerRequest::REQUESTTYPE_CLIENT_CLOSE,
		this,
		ServerRequest::EVENT_CUSTOM_NONE,
		NULL,
		ServerRequest::MESSAGE_ID_UNSUPPORTED,
		ServerRequest::MESSAGE_RETRIES_NONE
	);
	// delegate it to thread pool, but make close request not declinable
	server->workerThreadPool->addElement(request, false);
	// server call back
	server->closeClient(this);
}

void UDPServerClient::init() {
	// acquire reference for worker
	acquireReference();

	// create request
	ServerRequest* request = new ServerRequest(
		ServerRequest::REQUESTTYPE_CLIENT_INIT,
		this,
		ServerRequest::EVENT_CUSTOM_NONE,
		NULL,
		ServerRequest::MESSAGE_ID_UNSUPPORTED,
		ServerRequest::MESSAGE_RETRIES_NONE
	);
	// delegate it to thread pool, but make close request not declinable
	server->workerThreadPool->addElement(request, false);
}

void UDPServerClient::fireEvent(const string &type) {
	// acquire reference for worker
	acquireReference();

	// create request
	ServerRequest* request = new ServerRequest(
		ServerRequest::REQUESTTYPE_CLIENT_CUSTOM,
		this,
		type,
		NULL,
		ServerRequest::MESSAGE_ID_UNSUPPORTED,
		ServerRequest::MESSAGE_RETRIES_NONE
	);

	// delegate it to thread pool, but make close request not declinable
	server->workerThreadPool->addElement(request, false);
}


void UDPServerClient::cleanUpSafeMessages() {
	//
	messageMapSafeMutex.lock();

	// check if message has been already processed
	uint64_t now = Time::getCurrentMillis();
	MessageMapSafe::iterator it = messageMapSafe.begin();
	while (it != messageMapSafe.end()) {
		Message* message = &it->second;
		if (message->time < now - MESSAGESSAFE_KEEPTIME) {
			messageMapSafe.erase(it++);
			continue;
		}
		++it;
	}

	//
	messageMapSafeMutex.unlock();
}

uint64_t UDPServerClient::getRetryTime(const uint8_t retries) {
	if (retries == 0) return 0L;
	if (retries > UDPServerIOThread::MESSAGEACK_RESENDTIMES_TRIES) return 0L;
	return UDPServerIOThread::MESSAGEACK_RESENDTIMES[retries - 1];
}

