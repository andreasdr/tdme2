/**
 * @version $Id: aa3465de5134eac90f5c600a66a33e052df4613d $
 */
#if defined(_WIN32) && defined(_MSC_VER)
	#include <windows.h>
#else
	#include <unistd.h>
#endif

#include <iostream>
#include <string>
#include <thread>

#include <tdme/utilities/Console.h>

#include "Thread.h"
#include "ThreadingError.h"

using std::string;

using tdme::os::threading::Thread;
using tdme::utilities::Console;

Thread::Thread(const string& name, size_t stackSize): name(name), pThreadCreated(false), stopRequested(false), stackSize(stackSize) {
}

Thread::~Thread() {
}

int Thread::getHardwareThreadCount() {
	return std::thread::hardware_concurrency();
}

void Thread::sleep(const uint64_t milliseconds) {
	#if defined(CPPTHREADS)
	#else
		// TODO: Check nanosleep from time.h
		//	struct timespec time;
		//	time.tv_sec = milliseconds / 1000;
		//	time.tv_nsec = (milliseconds - (time.tv_sec * 1000)) * 1000000;
		//	nanosleep(&time, &time);
		#if defined(_WIN32) && defined(_MSC_VER)
			Sleep(milliseconds);
		#else
			uint64_t secondsWaited = 0L;
			while (milliseconds - (secondsWaited * 1000L) >= 1000L) {
				::sleep(1);
				secondsWaited++;
			}
			usleep((milliseconds - (secondsWaited * 1000L)) * 1000L);
		#endif
	#endif
}

void Thread::join() {
	#if defined(CPPTHREADS)
		thread->join();
	#else
		void* status;
		auto result = pthread_join(pThread, &status);
		PTHREAD_CHECK_ERROR(name, "Could not join pthread", "pthread_join");
	#endif
}

void Thread::start() {
	#if defined(CPPTHREADS)
		thread = new std::thread(pThreadRun, (void*)this);
	#else
		// set up thread attributes
		pthread_attr_t pThreadAttr;
		pthread_attr_init(&pThreadAttr);
		// joinable state
		pthread_attr_setdetachstate(&pThreadAttr, PTHREAD_CREATE_JOINABLE);

		size_t currentStackSize;
		pthread_attr_getstacksize(&pThreadAttr, &currentStackSize);
		if (currentStackSize != stackSize) {
			Console::println("Thread::start(): " + name + ": changing stack size from " + to_string(currentStackSize) + " to " + to_string(stackSize));
			pthread_attr_setstacksize(&pThreadAttr, stackSize);
		}

		// create thread
		auto result = pthread_create(&pThread, &pThreadAttr, &pThreadRun, (void*)this);
		PTHREAD_CHECK_ERROR(name, "Could not create pthread", "pthread_create");
	#endif
}

void* Thread::pThreadRun(void *thread) {
	static_cast<Thread*>(thread)->run();
	pthread_exit(NULL);
	return NULL;
}

void Thread::stop() {
	stopRequested = true;
}

bool Thread::isStopRequested() {
	return stopRequested;
}

