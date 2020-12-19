#pragma once

#if defined(_WIN32)
	#include <winsock2.h>
#else
	#include <sys/select.h>
#endif

#include <map>
#include <vector>

#include <tdme/os/network/fwd-tdme.h>
#include <tdme/os/network/platform/fallback/fwd-tdme.h>
#include <tdme/os/network/NIOInterest.h>

#include <tdme/os/threading/Mutex.h>

using std::map;
using std::vector;

using tdme::os::threading::Mutex;

/**
 * Fallback/select kernel event mechanism platform specific data
 * @author Andreas Drewke
 */
class tdme::os::network::platform::fallback::KernelEventMechanismPSD {
	friend class tdme::os::network::KernelEventMechanism;

private:
	struct Event {
		int descriptor;
		NIOInterest interest;
		void* cookie;
	};

	/**
	 * @brief Public constructor
	 */
	KernelEventMechanismPSD() :
		maxFd(0),
		fdsMutex("kemfallbackpsdfdmutex")
	{
		//
	}

	int maxFd;
	Mutex fdsMutex;
	fd_set rfds;
	fd_set wfds;
	map<int, void*> fds;
	vector<Event> events;
};
