#pragma once

#include <stdint.h>

#include <exception>
#include <string>
#include <sstream>

#include <tdme/tdme.h>

#include <tdme/network/udpserver/Server.h>
#include <tdme/network/udpserver/ServerGroupBase.h>

#include <tdme/os/threading/ReadWriteLock.h>
#include <tdme/utilities/Exception.h>
#include <tdme/utilities/ReferenceCounter.h>

using std::string;
using std::ostringstream;;

using tdme::os::threading::ReadWriteLock;
using tdme::utilities::Exception;
using tdme::utilities::ReferenceCounter;

namespace tdme {
namespace network {
namespace udpserver {

/**
 * Base class for network server groups
 * @author Andreas Drewke
 */
template <typename SERVER, typename CLIENT, typename GROUP>
class ServerGroup : public ServerGroupBase {
public:
	typedef std::set<std::string> ClientKeySet;

	ServerGroup(const uint32_t groupId) :
		server(NULL),
		groupId(groupId),
		clientKeyListsReadWriteLock("nioservergroup_clientlists"){
		//
		ostringstream tmp;
		tmp << "unnamed.";
		tmp << groupId;
		key = tmp.str();
	}

	/**
	 * @brief group identification key
	 * @return group key
	 */
	const string& getKey() {
		return key;
	}

	/**
	 * @brief sets the group identification key
	 * @param &key group identification key
	 * @return if setting the key was succesful
	 */
	const bool setKey(const string &key) {
		if (server->setGroupKey(this, key) == true) {
			this->key = key;
			return true;
		} else {
			return false;
		}
	}

	/**
	 * @brief get a copy of current group client keys
	 * @return group client list
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
	 * @brief Adds a client to this group
	 * @param client client
	 */
	virtual const bool addClient(CLIENT* client) {
		clientKeyListsReadWriteLock.writeLock();
		typename ClientKeySet::iterator it = clientKeySet.find(client->getKey());
		// check if already exists
		if (it != clientKeySet.end()) {
			clientKeyListsReadWriteLock.unlock();
			return false;
		}
		clientKeySet.insert(client->getKey());
		clientKeyListsReadWriteLock.unlock();
		return true;
	}

	/**
	 * @brief Removes a client from this group
	 * @param client client
	 */
	virtual const bool removeClient(CLIENT* client) {
		clientKeyListsReadWriteLock.writeLock();
		typename ClientKeySet::iterator it = clientKeySet.find(client->getKey());
		// check if not exists
		if (it == clientKeySet.end()) {
			clientKeyListsReadWriteLock.unlock();
			return false;
		}
		clientKeySet.erase(client->getKey());
		clientKeyListsReadWriteLock.unlock();
		return true;
	}

	/**
	 * @brief Shuts down this server group
	 */
	virtual void shutdown() = 0;
protected:
	typedef std::map<const std::string, CLIENT*> ClientKeyMap;

	/*
	 * @brief event method called if group will be created, will be called from worker
	 */
	virtual void onCreate() = 0;

	/*
	 * @brief event method called if client will be closed, will be called from worker
	 */
	virtual void onClose() = 0;

	/**
	 * @brief Shuts down this group
	 */
	virtual void close() {
		// acquire reference for worker
		acquireReference();
		// create request
		ServerRequest* request = new ServerRequest(
			ServerRequest::REQUESTTYPE_GROUP_CLOSE,
			this
		);
		// delegate it to thread pool, but make close request not declinable
		server->workerThreadPool->addElement(request, false);
		// server call back
		server->closeGroup(static_cast<GROUP*>(this));
	}

	//
	SERVER* server;
	uint32_t groupId;
	std::string key;

	ClientKeySet clientKeySet;

	ReadWriteLock clientKeyListsReadWriteLock;
};

};
};
};
