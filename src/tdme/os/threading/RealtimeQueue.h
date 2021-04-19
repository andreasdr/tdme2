#pragma once

#include "fwd-tdme.h"

#include <queue>

#include "SpinLock.h"

using std::queue;
using tdme::os::threading::SpinLock;

namespace tdme {
namespace os {
namespace threading {

/**
 * Realtime consumer/producer queue.
 * @author Andreas Drewke
 * @version $Id: 160da968398d80de667bab6b825cdeca9dc1dd50 $
 */
template <typename T>
class RealtimeQueue {
public:
	/**
	 * @brief Public constructor
	 */
	RealtimeQueue(const unsigned int maxElements) :
		maxElements(maxElements),
		m("queue"),
		stopRequested(false) {
		//
	}

	/**
	 * @brief Destructor, removes remaining elements from queue
	 */
	virtual ~RealtimeQueue() {
		while (data.size() > 0) {
			T* element = data.front();
			delete element;
			data.pop();
		}
	}

	/**
	 * @brief Requests this queue to be stopped
	 */
	void stop() {
		stopRequested = true;
	}

	/**
	 * @brief Gets an element from this queue, if no element exists it will immediately return NULL
	 * @return T*
	 */
	T* getElement() {
		m.lock();
		if (stopRequested == true) {
			m.unlock();
			return NULL;
		} else
		if (data.size() == 0) {
			m.unlock();
			return NULL;
		} else {
			T* element = data.front();
			data.pop();
			m.unlock();
			return element;
		}
	}

	/**
	 * @brief Adds an element to this queue
	 * @param element T* element
	 * @param declinable bool if element is declinable
	 * @return if element was added
	 */
	bool addElement(T* element, const bool declinable) {
		m.lock();
		if (declinable == true && data.size() > maxElements) {
			m.unlock();
			return false;
		}
		data.push(element);
		m.unlock();
		return true;
	}

protected:
	typedef queue<T*> RealtimeQueueType;
	RealtimeQueueType data;
	unsigned int maxElements;

private:
	SpinLock m;
	volatile bool stopRequested;

};

};
};
};
