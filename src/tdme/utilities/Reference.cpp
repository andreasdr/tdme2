#include <tdme/utilities/Reference.h>

#include <tdme/os/threading/AtomicOperations.h>

using tdme::utilities::Reference;

using tdme::os::threading::AtomicOperations;

Reference::Reference() : referenceCounter(0) {
}

Reference::~Reference() {
}

void Reference::acquireReference() {
	AtomicOperations::increment(referenceCounter);
}

void Reference::releaseReference() {
	if (AtomicOperations::decrement(referenceCounter) == 0) {
		onDelete();
		delete this;
	}
}

void Reference::onDelete() {
}
