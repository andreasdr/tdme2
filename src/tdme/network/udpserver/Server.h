#pragma once

#include <map>
#include <set>
#include <string>

#include <tdme/network/udpserver/ServerWorkerThreadPool.h>
#include <tdme/os/threading/Barrier.h>
#include <tdme/os/threading/ReadWriteLock.h>

using tdme::os::threading::Barrier;
using tdme::os::threading::ReadWriteLock;

namespace tdme {
namespace network {
namespace udpserver {

class ServerWorkerThreadPool;

/**
 * Base class for network servers
 * @author Andreas Drewke
 */
template <typename CLIENT, typename GROUP>
class Server {
public:
	typedef std::set<std::string> ClientKeySet;
	typedef std::set<std::string> GroupKeySet;

	/**
	 * @brief Public constructor
	 * @param name server name
	 * @param host host where to bind the server socket
	 * @param port port to listen on
s			 * @param maxCCU max ccu
	 */
	Server(const std::string& name, const std::string& host, const unsigned int port, const unsigned int maxCCU) :
		name(name),
		host(host),
		port(port),
		maxCCU(maxCCU),
		startUpBarrier(NULL),
		clientKeyListsReadWriteLock("nioserver_clientlist"),
		groupKeyListsReadWriteLock("nioserver_grouplist"),
		ioThreadCount(1),
		workerThreadPoolCount(2),
		workerThreadPoolMaxElements(128) {
		//
	}

	/**
	 * @brief Sets up the numbers of threads to handle IO and framing
	 * @param ioThreadCount IO thread count
	 */
	void setIOThreadCount(const unsigned int ioThreadCount) {
		this->ioThreadCount = ioThreadCount;
	}

	/**
	 * @brief Sets up the number of workers that handle requests in thread pool
	 * @param workerThreadCount number of workers in thread pool
	 */
	void setWorkerThreadCount(const unsigned int workerThreadCount) {
		workerThreadPoolCount = workerThreadCount;
	}

	/**
	 * @brief Sets up max number of elements in worker thread pool queue
	 * @param maxElements max elements
	 */
	void setThreadPoolMaxElements(const unsigned int maxElements) {
		workerThreadPoolMaxElements = maxElements;
	}

	/**
	 * @brief destructor
	 */
	virtual ~Server() {
	}

	/**
	 * @brief get a copy of current client keys
	 * @return client list
	 */
	ClientKeySet getClientKeySet() {
		// make a copy of the client key set
		ClientKeySet _clientKeySet;
		clientKeyListsReadWriteLock.readLock();
		_clientKeySet = clientKeySet;
		clientKeyListsReadWriteLock.unlock();
		// return copy
		return _clientKeySet;
	}

	/**
	 * @brief retrieve a client by key, the client reference is acquired, must be released after usage
	 * @param clientKey client identification key
	 * @return client or NULL
	 */
	CLIENT* getClientByKey(const std::string& clientKey) {
		clientKeyListsReadWriteLock.readLock();
		typename ClientKeyMap::iterator it = clientKeyMap.find(clientKey);
		CLIENT* client = it != clientKeyMap.end()?it->second:NULL;
		if (client != NULL) {
			client->acquireReference();
		}
		clientKeyListsReadWriteLock.unlock();
		//
		return client;
	}

	/**
	 * @brief get a copy of current group keys
	 * @return group list
	 */
	ClientKeySet getGroupKeySet() {
		// make a copy of the group key set
		GroupKeySet _groupKeySet;
		groupKeyListsReadWriteLock.readLock();
		_groupKeySet = groupKeySet;
		groupKeyListsReadWriteLock.unlock();
		// return copy
		return _groupKeySet;
	}

	/**
	 * @brief retrieve a group by key, the group reference is acquired, must be released after usage
	 * @param groupKey group identification key
	 * @return group or NULL
	 */
	GROUP* getGroupByKey(const std::string& groupKey) {
		groupKeyListsReadWriteLock.readLock();
		typename GroupKeyMap::iterator it = groupKeyMap.find(groupKey);
		GROUP* group = it != groupKeyMap.end()?it->second:NULL;
		if (group != NULL) {
			group->acquireReference();
		}
		groupKeyListsReadWriteLock.unlock();
		//
		return group;
	}

protected:
	typedef std::map<const std::string, CLIENT*> ClientKeyMap;
	typedef std::map<const std::string, CLIENT*> GroupKeyMap;
	typedef std::set<CLIENT*> ClientSet;

	/**
	 * @brief sets a client identification key
	 * @param client client
	 * @param &clientKey client identification key
	 * @return if setting the key was successful
	 */
	bool setClientKey(CLIENT* client, const std::string &clientKey) {
		clientKeyListsReadWriteLock.writeLock();
		// set new client key association
		typename ClientKeyMap::iterator it = clientKeyMap.find(clientKey);
		if (it != clientKeyMap.end()) {
			clientKeyListsReadWriteLock.unlock();
			// nope, we could't set the key
			return false;
		}
		// remove old client key association if changed
		if (client->getKey() != clientKey) {
			clientKeyMap.erase(client->getKey());
		}
		// set client in map
		clientKeyMap.insert(make_pair(clientKey, client));
		// set client key in set
		clientKeySet.insert(clientKey);
		clientKeyListsReadWriteLock.unlock();
		// success
		return true;
	}

	/**
	 * @brief closes a client connection
	 * @param client client
	 */
	void closeClient(CLIENT* client) {
		clientKeyListsReadWriteLock.writeLock();
		// erase client in client map
		clientKeyMap.erase(client->getKey());
		// erase client key in key set
		clientKeySet.erase(client->getKey());
		//
		clientKeyListsReadWriteLock.unlock();
	}

	/**
	 * @brief sets a group identification key
	 * @param group group
	 * @param &groupKey group identification key
	 * @return if setting the key was successful
	 */
	bool setGroupKey(GROUP* group, const std::string &groupKey) {
		groupKeyListsReadWriteLock.writeLock();
		// set new client key association
		typename GroupKeyMap::iterator it = groupKeyMap.find(groupKey);
		if (it != groupKeyMap.end()) {
			groupKeyListsReadWriteLock.unlock();
			// nope, we could't set the key
			return false;
		}
		// remove old client key association if changed
		if (group->getKey() != groupKey) {
			groupKeyMap.erase(group->getKey());
		}
		// set group in map
		groupKeyMap.insert(make_pair(groupKey, group));
		// set client key in set
		groupKeySet.insert(groupKey);
		groupKeyListsReadWriteLock.unlock();
		// success
		return true;
	}

	/**
	 * @brief closes a group connection
	 * @param group group
	 */
	void closeGroup(GROUP* group) {
		groupKeyListsReadWriteLock.writeLock();
		// erase client in client map
		groupKeyMap.erase(group->getKey());
		// erase client key in key set
		groupKeySet.erase(group->getKey());
		//
		groupKeyListsReadWriteLock.unlock();
	}

	std::string name;
	std::string host;
	unsigned int port;
	unsigned int maxCCU;

	Barrier* startUpBarrier;

	ClientKeyMap clientKeyMap;
	ClientKeySet clientKeySet;

	ReadWriteLock clientKeyListsReadWriteLock;

	GroupKeyMap groupKeyMap;
	GroupKeySet groupKeySet;

	ReadWriteLock groupKeyListsReadWriteLock;

	unsigned int ioThreadCount;
	unsigned int workerThreadPoolCount;
	unsigned int workerThreadPoolMaxElements;
};

};
};
};
