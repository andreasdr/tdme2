#pragma once

#include <tdme/os/threading/Mutex.h>
#include <tdme/os/threading/Thread.h>

using tdme::os::threading::Mutex;
using tdme::os::threading::Thread;

class SharedData {
public:
	SharedData() : m("shareddata"), counter(0) { };
	inline void incrementCounter() { m.lock(); int i = counter; counter = i + 1; m.unlock(); };
	inline int getCounter() { return counter; };
private:
	Mutex m;
	int counter;
};
