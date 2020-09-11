#include <tdme/tdme.h>

#if defined(_WIN32)
	#include <winsock2.h>
#else
	#include <sys/select.h>
	#include <sys/time.h>
#endif

#include <tdme/os/network/fwd-tdme.h>
#include <tdme/os/network/NIOInterest.h>
#include <tdme/os/network/KernelEventMechanism.h>
#include <tdme/os/network/platform/fallback/fwd-tdme.h>
#include <tdme/os/network/platform/fallback/KernelEventMechanismPSD.h>

using tdme::os::network::KernelEventMechanism;
using tdme::os::network::NIOInterest;
using tdme::os::network::platform::fallback::KernelEventMechanismPSD;

KernelEventMechanism::KernelEventMechanism() : initialized(false), _psd(NULL) {
	// allocate platform specific data
	_psd = static_cast<void*>(new KernelEventMechanismPSD());

	//
	auto psd = static_cast<KernelEventMechanismPSD*>(_psd);

	// clear fd sets
	FD_ZERO(&psd->rfds);
	FD_ZERO(&psd->wfds);
}

KernelEventMechanism::~KernelEventMechanism() {
	// delete platform specific data
	delete static_cast<KernelEventMechanismPSD*>(_psd);
}

void KernelEventMechanism::setSocketInterest(const NetworkSocket& socket, const NIOInterest lastInterest, const NIOInterest interest, const void* cookie) {
	// skip if not initialized
	if (initialized == false) return;

	//
	auto psd = static_cast<KernelEventMechanismPSD*>(_psd);

	// synchronize fd set access
	psd->fdsMutex.lock();

	// remove fd from fds
	psd->fds.erase(socket.descriptor);

	// remove last read interest
	if ((lastInterest & NIO_INTEREST_READ) == NIO_INTEREST_READ) {
		FD_CLR(socket.descriptor, &psd->rfds);
	}
	// remove last write interest
	if ((lastInterest & NIO_INTEREST_WRITE) == NIO_INTEREST_WRITE) {
		FD_CLR(socket.descriptor, &psd->wfds);
	}

	// have interest?
	bool haveInterest = false;

	// add read interest
	if ((interest & NIO_INTEREST_READ) == NIO_INTEREST_READ) {
		FD_SET(socket.descriptor, &psd->rfds);
		haveInterest = true;
	}
	// add write interest
	if ((interest & NIO_INTEREST_WRITE) == NIO_INTEREST_WRITE) {
		FD_SET(socket.descriptor, &psd->wfds);
		haveInterest = true;
	}

	// add fd to fds
	if (haveInterest == true) {
		if (socket.descriptor > psd->maxFd) psd->maxFd = socket.descriptor;
		psd->fds[socket.descriptor] = (void*)cookie;
	}

	// done synchronize fd set access
	psd->fdsMutex.unlock();
}

void KernelEventMechanism::initKernelEventMechanism(const unsigned int maxCCU)  {
	KernelEventMechanismPSD* psd = static_cast<KernelEventMechanismPSD*>(_psd);

	//
	initialized = true;
}

void KernelEventMechanism::shutdownKernelEventMechanism() {
	// skip if not initialized
	if (initialized == false) return;

	// platform specific data
	auto psd = static_cast<KernelEventMechanismPSD*>(_psd);
}

int KernelEventMechanism::doKernelEventMechanism() {
	// skip if not initialized
	if (initialized == false) return -1;

	// platform specific data
	auto psd = static_cast<KernelEventMechanismPSD*>(_psd);

	// have a timeout of 1ms
	// as we only can delegate interest changes to the kernel by
	// select
	struct timeval timeout = {0, 1L * 1000L};

	// clone fd sets
	psd->fdsMutex.lock();
	fd_set rfds = psd->rfds;
	fd_set wfds = psd->wfds;
	psd->fdsMutex.unlock();

	// run select
	auto result = select(psd->maxFd + 1, &rfds, &wfds, NULL, &timeout);
	if (result == -1) {
		throw NetworkKEMException("select failed");
	}

	// compile list of events
	psd->fdsMutex.lock();
	psd->events.clear();
	for (auto fdIt = psd->fds.begin(); fdIt != psd->fds.end(); ++fdIt) {
		if (FD_ISSET(fdIt->first, &rfds) != 0) {
			KernelEventMechanismPSD::Event event;
			event.descriptor = fdIt->first;
			event.interest = NIO_INTEREST_READ;
			event.cookie = fdIt->second;
			psd->events.push_back(event);
		}
		if (FD_ISSET(fdIt->first, &wfds) != 0) {
			KernelEventMechanismPSD::Event event;
			event.descriptor = fdIt->first;
			event.interest = NIO_INTEREST_WRITE;
			event.cookie = fdIt->second;
			psd->events.push_back(event);
		}
	}
	psd->fdsMutex.unlock();

	//
	return psd->events.size();
}

void KernelEventMechanism::decodeKernelEvent(const unsigned int index, NIOInterest &interest, void*& cookie)  {
	// skip if not initialized
	if (initialized == false) return;

	// platform specific data
	auto psd = static_cast<KernelEventMechanismPSD*>(_psd);

	// read interest and cookie from event
	KernelEventMechanismPSD::Event& event = psd->events[index];
	interest = event.interest;
	cookie = event.cookie;
}
