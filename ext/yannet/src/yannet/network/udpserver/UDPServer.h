#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include <yannet/network/udpserver/fwd-yannet.h>

#include <yannet/yannet.h>
#include <yannet/network/udp/UDPPacket.h>
#include <yannet/network/udpserver/NetworkServerException.h>
#include <yannet/network/udpserver/Server.h>
#include <yannet/network/udpserver/ServerWorkerThreadPool.h>
#include <yannet/network/udpserver/UDPServerClient.h>
#include <yannet/network/udpserver/UDPServerGroup.h>
#include <yannet/network/udpserver/UDPServerIOThread.h>
#include <yannet/os/threading/Barrier.h>
#include <yannet/os/threading/ReadWriteLock.h>
#include <yannet/os/threading/Thread.h>

using std::string;
using std::unique_ptr;
using std::unordered_map;
using std::unordered_set;

// namespaces
namespace yannet {
namespace network {
namespace udpserver {
	using ::yannet::network::udp::UDPPacket;
	using ::yannet::os::threading::Barrier;
	using ::yannet::os::threading::ReadWriteLock;
	using ::yannet::os::threading::Thread;
}
}
}

/**
 * Base class for network UDP servers
 * @author Andreas Drewke
 */
class yannet::network::udpserver::UDPServer: public Thread, public Server<UDPServerClient, UDPServerGroup> {
	friend class UDPServerClient;
	friend class UDPServerIOThread;
	friend class ServerGroup<UDPServer, UDPServerClient, UDPServerGroup>;

public:
	struct UDPServer_Statistics {
		int64_t time { -1LL };
		uint32_t received { 0 };
		uint32_t sent { 0 };
		uint32_t clients { 0 };
		uint32_t accepts { 0 };
		uint32_t errors { 0 };
	};

	// forbid class copy
	FORBID_CLASS_COPY(UDPServer)

	/**
	 * @brief Public constructor
	 * @param name server name
	 * @param host host where to bind the server socket
	 * @param port port to listen on
	 * @param maxCCU max ccu
	 */
	UDPServer(const string& name, const string& host, const unsigned int port, const unsigned int maxCCU);

	/**
	 * @brief destructor
	 */
	virtual ~UDPServer();

	/**
	 * @returns UDP client statistics
	 */
	const UDPServer_Statistics getStatistics();

protected:
	enum MessageType {MESSAGETYPE_CONNECT = 0, MESSAGETYPE_MESSAGE = 1, MESSAGETYPE_ACKNOWLEDGEMENT = 2};

	/**
	 * @brief method to implement for accepting clients
	 * @param clientId client id
	 * @param ip ip
	 * @param port port
	 * @return server client class
	 */
	virtual UDPServerClient* accept(const uint32_t clientId, const string& ip, const uint16_t port);

	/**
	 * Identifies a client message
	 * @param packet packet
	 * @param messageType message type (see UDPServer::MessageType)
	 * @param connectionId connection id
	 * @param messageId message id
	 * @param retries retries
	 * @throws tdme::network::udpserver::NetworkServerException
	 * @return client or nullptr
	 */
	virtual void identify(const UDPPacket* packet, MessageType& messageType, uint32_t& connectionId, uint32_t& messageId, uint8_t& retries);

	/**
	 * Validates a client message
	 * @param packet packet
	 * @throws tdme::network::udpserver::NetworkServerException
	 */
	virtual void validate(const UDPPacket* packet);

	/**
	 * Writes a empty header to packet
	 * @param packet packet
	 * @throws tdme::network::udpserver::NetworkServerException
	 */
	static void initializeHeader(UDPPacket* packet);

	/**
	 * Writes a message header to message
	 * @param packet packet
	 * @param messageType message type
	 * @param clientId client id
	 * @param messageId message id
	 * @param retries retries
	 * @throws tdme::network::udpserver::NetworkServerException
	 */
	virtual void writeHeader(UDPPacket* packet, MessageType messageType, const uint32_t clientId, const uint32_t messageId, const uint8_t retries);
private:
	static const uint64_t CLIENT_CLEANUP_IDLETIME = 120000L;
	struct ClientId {
		uint32_t clientId;
		UDPServerClient* client;
		uint64_t time;
	};
	typedef unordered_map<uint32_t, ClientId*> ClientIdMap;
	typedef unordered_map<string, UDPServerClient*> ClientIpMap;
	typedef unordered_set<UDPServerClient*> ClientSet;
	static const uint32_t MESSAGE_ID_NONE = 0;

	/**
	 * main event loop
	 */
	virtual void run();

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
	UDPServerClient* getClientByIp(const string& ip, const uint16_t port);

	/**
	 * @brief Clean up clients that have been idle for some time or are flagged to be shut down
	 */
	void cleanUpClients();

	/**
	 * @brief pushes a message to be send, takes over ownership of frame
	 * @param client client
	 * @param packet packet to be send
	 * @param safe safe, requires ack and retransmission
	 * @param deleteFrame delete frame
	 * @param messageType message type
	 * @param messageId message id (only for MESSAGETYPE_MESSAGE)
	 * @throws tdme::network::udpserver::NetworkServerException
	 */
	void sendMessage(const UDPServerClient* client, UDPPacket* packet, const bool safe, const bool deleteFrame, const MessageType messageType, const uint32_t messageId = MESSAGE_ID_NONE);

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

	vector<unique_ptr<UDPServerIOThread>> ioThreads;
	unique_ptr<ServerWorkerThreadPool> workerThreadPool;

	uint32_t clientCount;
	uint32_t messageCount;

	UDPServer_Statistics statistics;
};

