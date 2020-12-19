#include <tdme/network/udpserver/ServerWorkerThreadPool.h>

#include <tdme/utilities/Console.h>

using tdme::network::udpserver::ServerWorkerThreadPool;
using tdme::os::threading::Queue;
using tdme::utilities::Console;

ServerWorkerThreadPool::ServerWorkerThreadPool(Barrier* startUpBarrier, const unsigned int workerCount, const unsigned int maxElements) :
	Queue<ServerRequest>(maxElements),

	startUpBarrier(startUpBarrier),
	workerCount(workerCount),
	worker(NULL) {
	//
}

ServerWorkerThreadPool::~ServerWorkerThreadPool() {
}

void ServerWorkerThreadPool::start() {
	worker = new ServerWorkerThread*[workerCount];
	for(unsigned int i = 0; i < workerCount; i++) {
		worker[i] = new ServerWorkerThread(i, this);
		worker[i]->start();
	}
}

void ServerWorkerThreadPool::stop() {
	// stop queue
	Queue<ServerRequest>::stop();

	// stop worker
	for(unsigned int i = 0; i < workerCount; i++) {
		// wait until worker has finished
		worker[i]->join();
		// delete worker
		delete worker[i];
	}
	delete [] worker;
}
