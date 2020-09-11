#include "ThreadingTest_ConsumerThread.h"

#include <string>

#include <tdme/os/threading/Thread.h>
#include <tdme/os/threading/Queue.h>
#include <tdme/utilities/Console.h>

using std::string;
using std::to_string;

using tdme::os::threading::Thread;
using tdme::os::threading::Queue;
using tdme::utilities::Console;

ConsumerThread::ConsumerThread(int id, Queue<int>* queue) : Thread("consumer"), id(id), queue(queue) {
}

void ConsumerThread::run() {
	Console::println("ConsumerThread[" + to_string(id) + "]::init()");
	while(isStopRequested() == false) {
		int* element = queue->getElement();
		if (element == NULL) {
			break;
		}
		Console::println("ConsumerThread[" + to_string(id) + "]: got " + to_string(*element) + " from queue");
		delete element;
		Thread::sleep(100);
	}
	Console::println("ConsumerThread[" + to_string(id) + "]::done()");
}
