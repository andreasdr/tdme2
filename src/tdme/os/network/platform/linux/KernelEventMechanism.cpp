/**
 * @version $Id: 48c210d2bc49b9f43a27196102d30eda9d1ef12d $
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <tdme/os/network/fwd-tdme.h>
#include <tdme/os/network/NIOInterest.h>
#include <tdme/os/network/KernelEventMechanism.h>
#include <tdme/os/network/platform/linux/fwd-tdme.h>
#include <tdme/os/network/platform/linux/KernelEventMechanismPSD.h>

using tdme::os::network::KernelEventMechanism;
using tdme::os::network::NIOInterest;
using tdme::os::network::platform::_linux::KernelEventMechanismPSD;

KernelEventMechanism::KernelEventMechanism() : initialized(false), _psd(NULL) {
	// allocate platform specific data
	_psd = static_cast<void*>(new KernelEventMechanismPSD());
}

KernelEventMechanism::~KernelEventMechanism() {
	// delete platform specific data
	delete static_cast<KernelEventMechanismPSD*>(_psd);
}

void KernelEventMechanism::setSocketInterest(const NetworkSocket& socket, const NIOInterest lastInterest, const NIOInterest interest, const void* cookie) {
	// platform specific data
	auto psd = static_cast<KernelEventMechanismPSD*>(_psd);

	// setup new event
	struct epoll_event event;
	event.events = EPOLLET;
	event.data.ptr = (void*)cookie;

	// handle read interest
	if ((interest & NIO_INTEREST_READ) == NIO_INTEREST_READ) {
		event.events|= EPOLLIN;
	}
	// handle write interest
	if ((interest & NIO_INTEREST_WRITE) == NIO_INTEREST_WRITE) {
		event.events|= EPOLLOUT;
	}

	//
	if (epoll_ctl(
		psd->ep,
		lastInterest == NIO_INTEREST_NONE?EPOLL_CTL_ADD:EPOLL_CTL_MOD,
		socket.descriptor,
		&event) == -1) {
		//
		std::string msg = "Could not add epoll event: ";
		msg+= strerror(errno);
		throw NetworkKEMException(msg);
	}
}

void KernelEventMechanism::initKernelEventMechanism(const unsigned int maxCCU)  {
	// platform specific data
	auto psd = static_cast<KernelEventMechanismPSD*>(_psd);

	// epoll event list, maxCCU
	psd->epEventListMax = maxCCU;
	psd->epEventList = new epoll_event[psd->epEventListMax];

	// start epoll and get the descriptor
	psd->ep = epoll_create1(0);
	if (psd->ep == -1) {
		delete [] psd->epEventList;
		std::string msg = "Could not create epoll: ";
		msg+= strerror(errno);
		throw NetworkKEMException(msg);
	}

	//
	initialized = true;
}

void KernelEventMechanism::shutdownKernelEventMechanism() {
	// skip if not initialized
	if (initialized == false) return;

	// platform specific data
	auto psd = static_cast<KernelEventMechanismPSD*>(_psd);

	//
	close(psd->ep);
	delete [] psd->epEventList;
}

int KernelEventMechanism::doKernelEventMechanism()  {
	// platform specific data
	auto psd = static_cast<KernelEventMechanismPSD*>(_psd);

	while (true == true) {
		//
		auto events = epoll_wait(psd->ep, psd->epEventList, psd->epEventListMax, 5);

		// check for error
		if (events == -1) {
			if (errno == EINTR) {
				// epoll_wait was interrupted by system call, so ignore this and restart
			} else {
				std::string msg = "epoll_wait failed: ";
				msg+= strerror(errno);
				throw NetworkKEMException(msg);
			}
		} else {
			//
			return events;
		}
	}
}

void KernelEventMechanism::decodeKernelEvent(const unsigned int index, NIOInterest &interest, void*& cookie)  {
	// platform specific data
	auto psd = static_cast<KernelEventMechanismPSD*>(_psd);

	//
	auto event = &psd->epEventList[index];

	// we only support user data
	cookie = (void*)event->data.ptr;

	// set up interest
	interest = NIO_INTEREST_NONE;
	if ((event->events & EPOLLIN) == EPOLLIN) {
		interest|= NIO_INTEREST_READ;
	}
	if ((event->events & EPOLLOUT) == EPOLLOUT) {
		interest|= NIO_INTEREST_WRITE;
	}
}
