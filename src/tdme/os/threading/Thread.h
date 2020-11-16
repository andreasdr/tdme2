#pragma once

#include "fwd-tdme.h"

#include <tdme/tdme.h>

#if defined(CPPTHREADS)
	#include <thread>
	using std::thread;
#else
	#if defined(_WIN32)
		#include <windows.h>
	#endif
	#include <pthread.h>
#endif

#if !defined(_MSC_VER)
	#include <time.h>
#endif


#include <string>

using std::string;

/**
 * Base class for threads.
 * @author Andreas Drewke
 */
class tdme::os::threading::Thread {
public:
	/**
	 * @brief Public constructor
	 * @param name name
	 * @param stackSize stack size, defaults to 2MB
	 */
	Thread(const string& name, size_t stackSize = 2 * 1024 * 1024);

	/**
	 * @brief Public destructor
	 */
	virtual ~Thread();

	/**
	 * @return hardware thread count
	 */
	static int getHardwareThreadCount();

	/**
	 * @brief sleeps current thread for given time in milliseconds
	 * @param milliseconds uint64_t milliseconds to wait
	 */
	static void sleep(const uint64_t milliseconds);

	/**
	 * @brief sleeps current thread for given time in nanoseconds
	 * @param nanoseconds uint64_t nanoseconds to wait
	 */
	inline static void nanoSleep(const uint64_t nanoseconds) {
		#if defined(CPPTHREADS)
		#else
			#if defined(_WIN32)
				// see: https://gist.github.com/Youka/4153f12cf2e17a77314c
				HANDLE timer;
				LARGE_INTEGER li;
				if ((timer = CreateWaitableTimer(NULL, TRUE, NULL)) == NULL) {
					return;
				}
				li.QuadPart = -nanoseconds;
				if (SetWaitableTimer(timer, &li, 0, NULL, NULL, FALSE) == 0) {
					CloseHandle(timer);
					return;
				}
				if (WaitForSingleObject(timer, INFINITE) != WAIT_OBJECT_0) {
					// no op
				}
				CloseHandle(timer);
			#else
				struct timespec sleepTime;
				struct timespec returnTime;
				sleepTime.tv_sec = 0;
				sleepTime.tv_nsec = nanoseconds;
				nanosleep(&sleepTime, &returnTime);
			#endif
		#endif
	}

	/**
	 * @brief Blocks caller thread until this thread has been terminated
	 */
	void join();

	/**
	 * @brief Starts this objects thread
	 */
	void start();

	/**
	 * @brief Abstract run() method, should be implemented by subclassed class, will be called after spawn by start()
	 */
	virtual void run() = 0;

	/**
	 * @brief Requests that this thread should be stopped
	 */
	void stop();

	/**
	 * @brief Returns if stop has been requested
	 * @return bool
	 */
	bool isStopRequested();
private:
	static void *pThreadRun(void *thread);
	#if defined(CPPTHREADS)
		thread* thread;
	#else
		pthread_t pThread;
	#endif
	string name;
	bool pThreadCreated;
	volatile bool stopRequested;
	size_t stackSize;
};
