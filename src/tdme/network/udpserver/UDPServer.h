#pragma once

#include <stdint.h>

#include <sstream>
#include <string>
#include <map>
#include <set>

#include <tdme/network/udpserver/fwd-tdme.h>

#include <tdme/tdme.h>
#include <tdme/os/threading/Thread.h>
#include <tdme/os/threading/Barrier.h>
#include <tdme/os/threading/ReadWriteLock.h>
#include <tdme/network/udpserver/NetworkServerException.h>
#include <tdme/network/udpserver/UDPServerIOThread.h>
#include <tdme/network/udpserver/UDPServerClient.h>
#include <tdme/network/udpserver/UDPServerGroup.h>
#include <tdme/network/udpserver/Server.h>
#include <tdme/network/udpserver/ServerWorkerThreadPool.h>

using std::stringstream;

using tdme::os::threading::Thread;
using tdme::os::threading::Barrier;
using tdme::os::threading::ReadWriteLock;
using tdme::network::udpserver::NetworkServerException;
using tdme::network::udpserver::UDPServerIOThread;
using tdme::network::udpserver::UDPServerClient;
using tdme::network::udpserver::UDPServerGroup;
using tdme::network::udpserver::Server;
using tdme::network::udpserver::ServerWorkerThreadPool;

/**
 * Base class for network UDP servers
 * @author Andreas Drewke
 */
class tdme::network::udpserver::UDPServer: public Thread, public Server<UDPServerClient, UDPServerGroup> {
	friend class UDPServerClient;
	friend class UDPServerIOThread;
	friend class ServerGroup<UDPServer, UDPServerClient, UDPServerGroup>;

public:
	/**
	 * @brief Public constructor
	 * @param name server name
	 * @param host host where to bind the server socket
	 * @param port port to listen on
	 * @param maxCCU max ccu
	 */
	UDPServer(const std::string& name, const std::string& host, const unsigned int port, const unsigned int maxCCU);

	/**
	 * @brief destructor
	 */
	virtual ~UDPServer();

	/**
	 * main event loop
	 */
	virtual void run();

protected:
	enum MessageType {MESSAGETYPE_CONNECT = 0, MESSAGETYPE_MESSAGE = 1, MESSAGETYPE_ACKNOWLEDGEMENT = 2};

	/**
	 * @brief method to implement for accepting clients
	 * @param clientId client id
	 * @param ip ip
	 * @param port port
	 * @return server client class
	 */
	virtual UDPServerClient* accept(const uint32_t clientId, const std::string& ip, const unsigned int port);

	/**
	 * Identifies a client message
	 * @param frame frame
	 * @param messageType message type (see UDPServer::MessageType)
	 * @param connectionId connection id
	 * @param messageId message id
	 * @param retries retries
	 * @throws tdme::network::udpserver::NetworkServerException
	 * @return client or NULL
	 */
	virtual void identify(stringstream* frame, MessageType& messageType, uint32_t& connectionId, uint32_t& messageId, uint8_t& retries);

	/**
	 * Validates a client message
	 * @param frame frame
	 * @throws tdme::network::udpserver::NetworkServerException
	 */
	virtual void validate(stringstream* frame);

	/**
	 * Writes a empty header to message
	 * @param frame frame
	 * @throws tdme::network::udpserver::NetworkServerException
	 */
	static void initializeHeader(stringstream* frame);

	/**
	 * Writes a message header to message
	 * @param frame frame
	 * @param messageType message type
	 * @param clientId client id
	 * @param messageId message id
	 * @param retries retries
	 * @throws tdme::network::udpserver::NetworkServerException
	 */
	virtual void writeHeader(stringstream* frame, MessageType messageType, const uint32_t clientId, const uint32_t messageId, const uint8_t retries);
private:
	static const uint64_t CLIENT_CLEANUP_IDLETIME = 120000L;
	struct Client {
		uint32_t clientId;
		UDPServerClient* client;
		uint64_t time;
	};
	typedef std::map<uint32_t, Client> ClientIdMap;
	typedef std::map<string, UDPServerClient*> ClientIpMap;
	typedef std::set<UDPServerClient*> ClientSet;
	static const uint32_t MESSAGE_ID_NONE = 0;

	/**
	 * @brief maps a new client to a given client id
	 * @param client client
	 * @throws tdme::network::udpserver::NetworkServerException if id is already in use
	 */
	void addClient(UDPServerClient* client);

	/**
	 * @brief removes a client
	 * @param client client
	 * @throws tdme::network::udpserver::NetworkServerException if id is not in use
	 */
	void removeClient(UDPServerClient* client);

	/**
	 * @brief Look ups a client by client id
	 * @param clientId client id
	 * @throws tdme::network::udpserver::NetworkServerException if client does not exist
	 * @return client
	 */
	UDPServerClient* lookupClient(const uint32_t clientId);

	/**
	 * @brief Returns client by host name and port
	 * @param ip ip
	 * @param port port
	 * @return client
	 */
	UDPServerClient* getClientByIp(const string& ip, const unsigned int port);

	/**
	 * @brief Clean up clients that have been idle for some time or are flagged to be shut down
	 */
	void cleanUpClients();

	/**
	 * @brief pushes a message to be send, takes over ownership of frame
	 * @param client client
	 * @param frame frame to be send
	 * @param safe safe, requires ack and retransmission
	 * @param deleteFrame delete frame
	 * @param messageType message type
	 * @param messageId message id (only for MESSAGETYPE_MESSAGE)
	 * @throws tdme::network::udpserver::NetworkServerException
	 */
	void sendMessage(const UDPServerClient* client, stringstream* frame, const bool safe, const bool deleteFrame, const MessageType messageType, const uint32_t messageId = MESSAGE_ID_NONE);

	/**
	 * @brief Processes an acknowlegdement reception
	 * @param client client
	 * @param messageId message id
	 * @throws tdme::network::udpserver::NetworkServerException
	 */
	void processAckReceived(UDPServerClient* client, const uint32_t messageId);

	/**
	 * @brief Allocates a client id for a new client
	 * @return client id
	 */
	const uint32_t allocateClientId();

	//
	ClientIdMap clientIdMap;
	ReadWriteLock clientIdMapReadWriteLock;

	ClientIpMap clientIpMap;
	ReadWriteLock clientIpMapReadWriteLock;

	unsigned int ioThreadCurrent;
	UDPServerIOThread** ioThreads;
	ServerWorkerThreadPool* workerThreadPool;

	uint32_t clientCount;
	uint32_t messageCount;
};

