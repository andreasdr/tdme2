#pragma once

#include <tdme/os/threading/Barrier.h>
#include <tdme/os/threading/Queue.h>

#include <tdme/network/udpserver/ServerRequest.h>
#include <tdme/network/udpserver/ServerWorkerThread.h>

using tdme::os::threading::Barrier;
using tdme::os::threading::Queue;

namespace tdme {
namespace network {
namespace udpserver {

class ServerRequest;

/**
 * @brief Simple server worker thread pool class
 * @author Andreas Drewke
 */
class ServerWorkerThreadPool : public Queue<ServerRequest> {
	friend class ServerWorkerThread;

public:

	/**
	 * @brief Public constructor
	 * @param startUpBarrier start up barrier
	 * @param workerCount worker count
	 * @param maxElements max elements
	 */
	ServerWorkerThreadPool(Barrier* startUpBarrier, const unsigned int workerCount, const unsigned int maxElements);

	/**
	 * @brief Public destructor
	 */
	virtual ~ServerWorkerThreadPool();

	/**
	 * @brief Start worker thread pool
	 */
	void start();

	/**
	 * @brief Stop worker thread pool
	 */
	void stop();
private:
	Barrier* startUpBarrier;
	unsigned int workerCount;
	ServerWorkerThread** worker;
};

};
};
};
