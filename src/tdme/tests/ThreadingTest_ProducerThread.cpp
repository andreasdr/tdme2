#include "ThreadingTest_ProducerThread.h"

#include <string>

#include <tdme/os/threading/Thread.h>
#include <tdme/os/threading/Queue.h>
#include <tdme/utilities/Console.h>

using std::string;
using std::to_string;

using tdme::os::threading::Thread;
using tdme::os::threading::Queue;
using tdme::utilities::Console;

ProducerThread::ProducerThread(int id, Queue<int>* queue) : Thread("producer"), id(id), queue(queue) {
}

void ProducerThread::run() {
	Console::println("ProducerThread[" + to_string(id) + "]::init()");
	for(int i = 0; isStopRequested() == false; i++) {
		int* element = new int;
		*element = i;
		queue->addElement(element, false);
		Console::println("ProducerThread[" + to_string(id) + "]: added " + to_string(i) + " to queue");
		Thread::sleep(50);
	}
	Console::println("ProducerThread[" + to_string(id) + "]::done()");
}
