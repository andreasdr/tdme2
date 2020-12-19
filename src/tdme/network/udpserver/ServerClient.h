#pragma once

#include <stdint.h>

#include <exception>
#include <sstream>
#include <string>

#include <tdme/tdme.h>
#include <tdme/network/udpserver/fwd-tdme.h>
#include <tdme/network/udpserver/ServerRequest.h>
#include <tdme/utilities/Exception.h>
#include <tdme/utilities/Reference.h>

using std::string;
using std::stringstream;

using tdme::utilities::Exception;
using tdme::utilities::Reference;

namespace tdme {
namespace network {
namespace udpserver {
class ServerRequest;
};
};
};

/**
 * Base class for network server clients
 * @author Andreas Drewke
 */
class tdme::network::udpserver::ServerClient : public Reference {
friend class ServerWorkerThread;

public:
	static const char* KEY_PREFIX_UNNAMED;

	/**
	 * @brief returns client's ip
	 * @return client ip
	 */
	virtual const string& getIp() const = 0;

	/**
	 * @brief returns client port
	 * @return client port
	 */
	virtual const unsigned int getPort() const = 0;

	/**
	 * @brief Client identification key
	 * @return client key
	 */
	virtual const string& getKey() const = 0;

	/**
	 * @brief sets the clients identification key
	 * @param &key client identification key
	 * @return if setting the key was succesful
	 */
	virtual const bool setKey(const string &key) = 0;

	/**
	 * @brief Creates a frame to be used with send
	 * @return frame to be send
	 */
	stringstream* createFrame();

	/**
	 * @brief Shuts down this network client
	 */
	virtual void shutdown() = 0;

protected:
	/**
	 * @brief To be overwritten with a request handler, will be called from worker
	 * @param frame frame
	 * @param messageId message id (udp server only)
	 * @param retries retries (udp server only)
	 */
	virtual void onRequest(stringstream* frame, const uint32_t messageId, const uint8_t retries) = 0;

	/*
	 * @brief event method called if client will be initiated, will be called from worker
	 */
	virtual void onInit() = 0;

	/*
	 * @brief event method called if client will be closed, will be called from worker
	 */
	virtual void onClose() = 0;

	/*
	 * @brief custom event method called if fired, will be called from worker
	 */
	virtual void onCustom(const string& type) = 0;

	/**
	 * @brief Event, which will be called if frame has been received, defaults to worker thread pool
	 * @param frame frame
	 * @param messageId message id (upd server only)
	 * @param retries retries (udp server only)
	 */
	virtual void onFrameReceived(stringstream* frame, const uint32_t messageId = 0, const uint8_t retries = 0) = 0;

	/**
	 * @brief Shuts down this network client
	 */
	virtual void close() = 0;

	//
	std::string key;
};

