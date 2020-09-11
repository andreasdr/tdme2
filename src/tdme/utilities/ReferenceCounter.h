#pragma once

#include <tdme/utilities/fwd-tdme.h>

/**
 * Reference counter implementation to be used with inheritance
 * @author Andreas Drewke
 */
class tdme::utilities::ReferenceCounter {
public:
	/**
	 * @brief Public constructor
	 */
	ReferenceCounter();

	/**
	 * @brief destructor
	 */
	virtual ~ReferenceCounter();

	/**
	 * @brief acquires a reference, incrementing the counter
	 */
	void acquireReference();

	/**
	 * @brief releases a reference, thus decrementing the counter and delete it if reference counter is zero
	 */
	void releaseReference();

	/**
	 * Callback method to be overridden, will be called if object will be deleted
	 */
	virtual void onDelete();

private:
	unsigned int referenceCounter;
};
