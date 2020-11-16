#include <string>

#include <tdme/os/threading/Thread.h>
#include <tdme/os/threading/Queue.h>
#include <tdme/utilities/Console.h>

#include "ThreadingTest_ConsumerThread.h"
#include "ThreadingTest_ProducerThread.h"
#include "ThreadingTest_SharedData.h"
#include "ThreadingTest_TestThread.h"

using std::string;
using std::to_string;

using tdme::os::threading::Thread;
using tdme::os::threading::Queue;
using tdme::utilities::Console;

#define TESTTHREAD_THREADS_COUNT	4

void testthread_test() {
	// basic thread test
	SharedData data;
	Console::println("Hallo World!");
	TestThread* tt[TESTTHREAD_THREADS_COUNT];
	// start threads
	for(int i = 0; i < TESTTHREAD_THREADS_COUNT; i++) {
		tt[i] = new TestThread(i, &data);
		tt[i]->start();
	}
	// wait until threads are finished
	for(int i = 0; i < TESTTHREAD_THREADS_COUNT; i++) {
		tt[i]->join();
	}
}

void pc_test() {
	// producer, consumer test
	Queue<int> queue(128);
	ProducerThread p(0, &queue);
	ConsumerThread c0(0, &queue);
	ConsumerThread c1(1, &queue);

	Console::println("starting consumer threads");
	c0.start();
	c1.start();
	Thread::sleep(500);

	Console::println("starting producer thread");
	p.start();

	Console::println("waiting 10 seconds");
	Thread::sleep(2000);

	Console::println("stopping producer");
	p.stop();
	p.join();

	Thread::sleep(500);
	Console::println("stopping consumer and queue");
	c0.stop();
	c1.stop();
	queue.stop();

	c0.join();
	c1.join();
}

void atomic_test() {
	#if defined(_WIN32) && defined(_MSC_VER)
		// need to create some class or definition for atomic add, dec, otherwise this is not portable
	#else
		int intValue = 0;
		// 5 atomic adds
		for(int i = 0; i < 5; i++) {
			Console::println("atomic add, result " + to_string(__sync_add_and_fetch(&intValue,1)));
		}
		// 5 atomic subs
		for(int i = 0; i < 5; i++) {
			Console::println("atomic sub, result " + to_string(__sync_sub_and_fetch(&intValue,1)));
		}
	#endif
}

int main(int argc, char *argv[]) {
	testthread_test();
	pc_test();
	atomic_test();
}
