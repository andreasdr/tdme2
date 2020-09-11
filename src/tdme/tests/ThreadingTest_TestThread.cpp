#include "ThreadingTest_TestThread.h"

#include <string>

#include <tdme/utilities/Console.h>

using std::string;
using std::to_string;

using tdme::utilities::Console;

TestThread::TestThread(int id, SharedData *data) : Thread("test"), id(id), data(data) {
}

void TestThread::run() {
	Console::println("TestThread::run()");
	for(int i = 0; i < 100; i++) {
		Console::println(to_string(id) + ":" + to_string(data->getCounter()));
		data->incrementCounter();
	}
}
