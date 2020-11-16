// kqueue
	#if defined(__FreeBSD__) || defined(__OpenBSD__)
	#include <sys/types.h>
#endif
#include <sys/event.h>
#include <sys/time.h>

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <tdme/tdme.h>
#include <tdme/os/network/fwd-tdme.h>
#include <tdme/os/network/NIOInterest.h>
#include <tdme/os/network/KernelEventMechanism.h>
#include <tdme/os/network/platform/bsd/fwd-tdme.h>
#include <tdme/os/network/platform/bsd/KernelEventMechanismPSD.h>

using tdme::os::network::KernelEventMechanism;
using tdme::os::network::NIOInterest;
using tdme::os::network::platform::bsd::KernelEventMechanismPSD;

#if defined(__NetBSD__)
	#define KEVENT_UDATA_DATATYPE __intptr_t
#else
	#define KEVENT_UDATA_DATATYPE void*
#endif

KernelEventMechanism::KernelEventMechanism() : initialized(false),_psd(NULL) {
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

	psd->kqMutex.lock();
	// check for change list overrun
	if (psd->kqChangeListCurrent + 2 >= psd->kqChangeListMax) {
		// try to enlarge buffers
		bool reallocated = false;
		auto kqChangeList0Resized = (struct kevent*)realloc(psd->kqChangeList[0], sizeof(struct kevent) * (psd->kqChangeListMax << 1));
		auto kqChangeList1Resized = (struct kevent*)realloc(psd->kqChangeList[1], sizeof(struct kevent) * (psd->kqChangeListMax << 1));

		// realloc failed?
		if (kqChangeList0Resized == NULL || kqChangeList1Resized == NULL) {
			// yep
			reallocated = false;
			// only change kq change list if successfully reallocated
			if (kqChangeList0Resized != NULL) psd->kqChangeList[0] = kqChangeList0Resized;
			if (kqChangeList1Resized != NULL) psd->kqChangeList[1] = kqChangeList1Resized;
		} else {
			// success
			reallocated = true;
			// enlarge kq change list max
			psd->kqChangeListMax<<=1;
			// change kq change lists
			psd->kqChangeList[0] = kqChangeList0Resized;
			psd->kqChangeList[1] = kqChangeList1Resized;
		}

		// failed to reallocate
		if (reallocated == false) {
			psd->kqMutex.unlock();
			throw NetworkKEMException("kqueue change list too small");
		}
	}
	// handle read interest
	if ((interest & NIO_INTEREST_READ) == NIO_INTEREST_READ) {
		struct kevent* ke = &psd->kqChangeList[psd->kqChangeListBuffer][psd->kqChangeListCurrent++];
		ke->ident = socket.descriptor;
		ke->filter = EVFILT_READ;
		ke->flags = EV_ADD | EV_ENABLE;
		ke->fflags = 0;
		ke->data = 0;
		ke->udata = (KEVENT_UDATA_DATATYPE)cookie;
	} else {
		struct kevent* ke = &psd->kqChangeList[psd->kqChangeListBuffer][psd->kqChangeListCurrent++];
		ke->ident = socket.descriptor;
		ke->filter = EVFILT_READ;
		ke->flags = EV_ADD | EV_DISABLE;
		ke->fflags = 0;
		ke->data = 0;
		ke->udata = (KEVENT_UDATA_DATATYPE)cookie;
	}
	// handle write interest
	if ((interest & NIO_INTEREST_WRITE) == NIO_INTEREST_WRITE) {
		struct kevent* ke = &psd->kqChangeList[psd->kqChangeListBuffer][psd->kqChangeListCurrent++];
		ke->ident = socket.descriptor;
		ke->filter = EVFILT_WRITE;
		ke->flags = EV_ADD | EV_ENABLE;
		ke->fflags = 0;
		ke->data = 0;
		ke->udata = (KEVENT_UDATA_DATATYPE)cookie;
	} else {
		struct kevent* ke = &psd->kqChangeList[psd->kqChangeListBuffer][psd->kqChangeListCurrent++];
		ke->ident = socket.descriptor;
		ke->filter = EVFILT_WRITE;
		ke->flags = EV_ADD | EV_DISABLE;
		ke->fflags = 0;
		ke->data = 0;
		ke->udata = (KEVENT_UDATA_DATATYPE)cookie;
	}
	psd->kqMutex.unlock();
}

void KernelEventMechanism::initKernelEventMechanism(const unsigned int maxCCU)  {
	// platform specific data
	KernelEventMechanismPSD* psd = static_cast<KernelEventMechanismPSD*>(_psd);

	// kqueue change list, maxCCU * (read + write change)
	// can still be too less as you could change the filter a lot in a single request
	psd->kqChangeListMax = maxCCU * 2;
	psd->kqChangeListCurrent = 0;
	psd->kqChangeList = (struct kevent**)malloc(sizeof(void*) * 2);
	psd->kqChangeList[0] = (struct kevent*)malloc(sizeof(struct kevent) * psd->kqChangeListMax);
	psd->kqChangeList[1] = (struct kevent*)malloc(sizeof(struct kevent) * psd->kqChangeListMax);
	if (psd->kqChangeList == NULL) {
		throw NetworkKEMException("Could not allocate kqueue change list array");
	}
	if (psd->kqChangeList[0] == NULL) {
		free(psd->kqChangeList);
		throw NetworkKEMException("Could not allocate kqueue change list 0");
	}
	if (psd->kqChangeList[1] == NULL) {
		free(psd->kqChangeList[0]);
		free(psd->kqChangeList);
		throw NetworkKEMException("Could not allocate kqueue change list 1");
	}

	// kqueue event list, maxCCU * (read + write change)
	psd->kqEventListMax = maxCCU * 2;
	psd->kqEventList = (struct kevent*)malloc(sizeof(struct kevent) * psd->kqEventListMax);
	if (psd->kqEventList == NULL) {
		free(psd->kqChangeList[0]);
		free(psd->kqChangeList[1]);
		free(psd->kqChangeList);
		throw NetworkKEMException("Could not allocate kqueue event list");
	}

	// start kqueue and get the descriptor
	psd->kq = kqueue();
	if (psd->kq == -1) {
		shutdownKernelEventMechanism() ;
		std::string msg = "Could not create kqueue: ";
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
	free(psd->kqChangeList[0]);
	free(psd->kqChangeList[1]);
	free(psd->kqChangeList);
	free(psd->kqEventList);
	close(psd->kq);
}

int KernelEventMechanism::doKernelEventMechanism()  {
	// platform specific data
	KernelEventMechanismPSD* psd = (KernelEventMechanismPSD*)_psd;

	// have a timeout of 1ms
	// as we only can delegate interest changes to the kernel by
	// running kevent
	const struct timespec timeout = {0, 1L * 1000L * 1000L};

	//
	while (true == true) {
		// do kqueue change list double buffer logic
		psd->kqMutex.lock();

		// current kevent parameter from current change kqueue list
		auto kqChangeListFilledBuffer = psd->kqChangeListBuffer;
		auto kqChangeListFilledCurrent = psd->kqChangeListCurrent;

		// cycle change list buffer
		psd->kqChangeListBuffer = (psd->kqChangeListBuffer + 1) % 2;

		// reset change list
		psd->kqChangeListCurrent = 0;

		// done
		psd->kqMutex.unlock();

		// kevent
		int events = kevent(
			psd->kq,
			psd->kqChangeList[kqChangeListFilledBuffer],
			kqChangeListFilledCurrent,
			psd->kqEventList,
			psd->kqEventListMax,
			&timeout
		);

		// check for error
		if (events == -1) {
			if (errno == EINTR) {
				// kevent was interrupted by system call, so ignore this and restart
			} else {
				std::string msg = "kevent failed: ";
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

	struct kevent* ke = &psd->kqEventList[index];
	cookie = (void*)ke->udata;
	switch (ke->filter) {
		case(EVFILT_READ):
			interest = NIO_INTEREST_READ;
			break;
		case(EVFILT_WRITE):
			interest = NIO_INTEREST_WRITE;
			break;
		default:
			interest = NIO_INTEREST_NONE;
	}
}
