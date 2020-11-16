/**
 * @version $Id: 1a286d86b29cabf8420205519e524a885c74a8d7 $
 */

#include <errno.h>

#include <atomic>

#include "SpinLock.h"

using tdme::os::threading::SpinLock;

using std::atomic_flag;

SpinLock::SpinLock(const string& name) {
	this->name = name;
}

SpinLock::~SpinLock() {
}

bool SpinLock::tryLock() {
	if (locked.test_and_set(std::memory_order_acquire) == false) return true;
	return false;
}

void SpinLock::lock() {
	while (locked.test_and_set(std::memory_order_acquire));
}

void SpinLock::unlock() {
	locked.clear(std::memory_order_release);
}
