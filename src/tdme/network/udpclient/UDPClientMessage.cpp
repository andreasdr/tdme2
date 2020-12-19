#include <tdme/network/udpclient/UDPClientMessage.h>

#include <string.h>

#include <sstream>
#include <string>

#include <tdme/network/udpclient/UDPClient.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/IntEncDec.h>
#include <tdme/utilities/Time.h>

using tdme::network::udpclient::UDPClientMessage;

using std::ios_base;
using std::string;
using std::stringstream;
using std::to_string;

using tdme::network::udpclient::UDPClient;
using tdme::utilities::Console;
using tdme::utilities::IntEncDec;
using tdme::utilities::Time;

UDPClientMessage* UDPClientMessage::parse(const char message[512], const size_t bytes) {
	// decode header
	MessageType messageType = MessageType::MESSAGETYPE_NONE;
	switch(message[0]) {
		case 'A':
			messageType = MessageType::MESSAGETYPE_ACKNOWLEDGEMENT;
			break;
		case 'C':
			messageType = MessageType::MESSAGETYPE_CONNECT;
			break;
		case 'M':
			messageType = MessageType::MESSAGETYPE_MESSAGE;
			break;
		default:
			Console::println("UDPClientMessage::parse(): invalid message type: '" + (string() + message[0]) + "' (" + to_string(message[0]) + ")");
			return nullptr;
	}
	uint32_t clientId;
	uint32_t messageId;
	uint32_t retries;
	IntEncDec::decodeInt(string(&message[1], 6), clientId);
	IntEncDec::decodeInt(string(&message[7], 6), messageId);
	IntEncDec::decodeInt(string(&message[13], 1), retries);
	// decode data
	stringstream* frame = nullptr;
	if (bytes > 14) {
		frame = new stringstream();
		frame->write(&message[14], bytes - 14);
	}
	return new UDPClientMessage(messageType, clientId, messageId, retries, frame);
}

UDPClientMessage::UDPClientMessage(const MessageType messageType, const uint32_t clientId, const uint32_t messageId, const uint8_t retries, stringstream* frame) :
	messageType(messageType),
	clientId(clientId),
	messageId(messageId),
	retries(retries),
	frame(frame)
{
	time = Time::getCurrentMillis();
}

UDPClientMessage::~UDPClientMessage() {
	if (frame != nullptr) delete frame;
}

const uint64_t UDPClientMessage::getTime() {
	return time;
}

const UDPClientMessage::MessageType UDPClientMessage::getMessageType() {
	return messageType;
}

const uint32_t UDPClientMessage::getClientId() {
	return clientId;
}

const uint32_t UDPClientMessage::getMessageId() {
	return messageId;
}

const int64_t UDPClientMessage::getRetryTime() {
	return UDPClient::getRetryTime(retries);
}

const uint8_t UDPClientMessage::getRetryCount() {
	return retries;
}

void UDPClientMessage::retry() {
	retries++;
}

stringstream* UDPClientMessage::getFrame() {
	return frame;
}

void UDPClientMessage::generate(char message[512], size_t& bytes) {
	string datagram;
	switch(messageType) {
		case MESSAGETYPE_ACKNOWLEDGEMENT:
			datagram+= 'A';
			break;
		case MESSAGETYPE_CONNECT:
			datagram+= 'C';
			break;
		case MESSAGETYPE_MESSAGE :
			datagram+= 'M';
			break;
		default:
			// FIXME
			break;
	}
	string retriesEncoded;
	string clientIdEncoded;
	string messageIdEncoded;
	IntEncDec::encodeInt(retries, retriesEncoded);
	IntEncDec::encodeInt(clientId, clientIdEncoded);
	IntEncDec::encodeInt(messageId, messageIdEncoded);
	datagram+= clientIdEncoded;
	datagram+= messageIdEncoded;
	datagram+= retriesEncoded[retriesEncoded.length()];
	if (frame != nullptr) {
		datagram+= frame->str();
	}
	bytes = datagram.size();
	memcpy(message, datagram.c_str(), bytes);
}
