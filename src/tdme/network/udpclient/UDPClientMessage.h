#pragma once

#include <sstream>
#include <string>

#include <tdme/tdme.h>
#include <tdme/network/udpclient/fwd-tdme.h>

using std::string;
using std::stringstream;

/**
 * UDP client message
 * @author Andreas Drewke
 */
class tdme::network::udpclient::UDPClientMessage {
	friend UDPClient;

public:
	enum MessageType {
		MESSAGETYPE_NONE,
		MESSAGETYPE_CONNECT,
		MESSAGETYPE_MESSAGE,
		MESSAGETYPE_ACKNOWLEDGEMENT
	};

	/**
	 * Parse
	 * @param message[512] message
	 * @param bytes bytes
	 */
	static UDPClientMessage* parse(const char message[512], const size_t bytes);

	/**
	 * Destructor
	 */
	~UDPClientMessage();

	/**
	 * @return time
	 */
	const uint64_t getTime();

	/**
	 * @return message type
	 */
	const MessageType getMessageType();

	/**
	 * @return client id
	 */
	const uint32_t getClientId();

	/**
	 * @return message id
	 */
	const uint32_t getMessageId();

	/**
	 * @return retry count
	 */
	const uint8_t getRetryCount();

	/**
	 * @return retry time
	 */
	const int64_t getRetryTime();

	/**
	 * Mark message to be resend with increased retry count
	 */
	void retry();

	/**
	 * @return frame
	 */
	stringstream* getFrame();

	/**
	 * Generate datagram
	 */
	void generate(char message[512], size_t& bytes);

private:
	/**
	 * Public constructor
	 * @param messageType message type
	 * @param clientId client id
	 * @param messageId message id
	 * @param retries retries
	 * @param frame frame
	 */
	UDPClientMessage(const MessageType messageType, const uint32_t clientId, const uint32_t messageId, const uint8_t retries, stringstream* frame);

	//
	long time;
	uint8_t retries;
	MessageType messageType;
	uint32_t clientId;
	uint32_t messageId;
	stringstream* frame;
};
