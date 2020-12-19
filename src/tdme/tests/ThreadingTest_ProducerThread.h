#pragma once

#include <tdme/os/threading/Queue.h>
#include <tdme/os/threading/Thread.h>

using tdme::os::threading::Queue;
using tdme::os::threading::Thread;

class ProducerThread: public Thread {
public:
	ProducerThread(int id, Queue<int> *queue);
	void run();

private:
	int id;
	Queue<int> *queue;

};
