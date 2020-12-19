#pragma once

#include <tdme/os/threading/Queue.h>
#include <tdme/os/threading/Thread.h>

using tdme::os::threading::Queue;
using tdme::os::threading::Thread;

class ConsumerThread: public Thread {
public:
	ConsumerThread(int id, Queue<int> *queue);
	void run();

private:
	int id;
	Queue<int> *queue;

};
