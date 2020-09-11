#pragma once

#include <stdint.h>

#include <tdme/os/threading/Thread.h>
#include <tdme/network/udpserver/ServerWorkerThreadPool.h>

using tdme::os::threading::Thread;

namespace tdme {
namespace network {
namespace udpserver {

class ServerWorkerThreadPool;

/**
 * @brief Server worker thread
 * @author Andreas Drewke
 */
class ServerWorkerThread : public Thread {
public:
	/**
	 * @brief Public constructor
	 * @param id id
	 * @param threadPool thread pool
	 */
	ServerWorkerThread(const unsigned int id, ServerWorkerThreadPool* threadPool);

	/**
	 * @brief Public destructor
	 */
	virtual ~ServerWorkerThread();

	/**
	 * @brief Thread run method
	 */
	void run();
private:
	unsigned int id;
	ServerWorkerThreadPool *threadPool;
};

};
};
};
