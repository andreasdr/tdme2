#pragma once

#include <tdme/tdme.h>
#include <tdme/os/threading/fwd-tdme.h>

#if defined(_WIN32) && defined(_MSC_VER)
	#include <windows.h>
#endif

/**
 * Atomic operations
 * @author Andreas Drewke
 */
class tdme::os::threading::AtomicOperations {
public:

	/**
	 * Increment uint32 value and return its value
	 * @param value reference to value
	 * @param byValue value you like to add
	 * @return incremented value
	 */
	inline static uint32_t increment(volatile uint32_t& value, uint32_t byValue = 1) {
		#if defined(_WIN32) && defined(_MSC_VER)
			return InterlockedAdd((volatile long*)&value, (long)byValue);
		#else
			return __sync_add_and_fetch(&value, byValue);
		#endif
	}

	/**
	 * Decrement uint32 value and return its value
	 * @param value reference to value
	 * @param byValue value you like to subtract
	 * @return decremented value
	 */
	inline static uint32_t decrement(volatile uint32_t& value, uint32_t byValue = 1) {
		#if defined(_WIN32) && defined(_MSC_VER)
			return InterlockedAdd((volatile long*)&value, (long)-byValue);
		#else
			return __sync_sub_and_fetch(&value, byValue);
		#endif
	}

};
