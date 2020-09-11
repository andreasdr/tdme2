#include <tdme/utilities/ReferenceCounter.h>

#include <tdme/os/threading/AtomicOperations.h>

using tdme::utilities::ReferenceCounter;

using tdme::os::threading::AtomicOperations;

ReferenceCounter::ReferenceCounter() : referenceCounter(0) {
}

ReferenceCounter::~ReferenceCounter() {
}

void ReferenceCounter::acquireReference() {
	AtomicOperations::increment(referenceCounter);
}

void ReferenceCounter::releaseReference() {
	if (AtomicOperations::decrement(referenceCounter) == 0) {
		onDelete();
		delete this;
	}
}

void ReferenceCounter::onDelete() {
}
