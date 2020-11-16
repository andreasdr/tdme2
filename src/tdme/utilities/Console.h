
#pragma once

#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/os/threading/fwd-tdme.h>
#include <tdme/os/threading/Mutex.h>
#include <tdme/os/threading/Thread.h>
#include <tdme/utilities/fwd-tdme.h>

using std::string;
using std::vector;

using tdme::os::threading::Mutex;
using tdme::os::threading::Thread;


/**
 * Console class
 * @author Andreas Drewke
 * @versio $Id$
 */
class tdme::utilities::Console
{
public:
	/**
	 * Print given string and trailing newline to console
	 * @param str string
	 */
	static void println(const string& str);

	/**
	 * Print given string
	 * @param str string
	 */
	static void print(const string& str);

	/**
	 * Print new line to console
	 */
	static void println();

	/**
	 * Shutdown console logging and especially writing log to file
	 */
	static void shutdown();

private:
	class LogWriterThread: public Thread {
	public:
		LogWriterThread();
		~LogWriterThread();
		void run();
		void flush();
	};

	/**
	 * Initialize log writer thread
	 */
	static void initLogWriterThread();

	//
	static Mutex* mutex;
	static vector<string>* messages;
	static LogWriterThread logWriterThread;
};
