#pragma once

#include <tdme/engine/fileio/fwd-tdme.h>

/**
 * Progress Callback
 * @author Andreas Drewke
 * @version $Id$
 */
struct tdme::engine::fileio::ProgressCallback
{
	/**
	 * Destructor
	 */
	virtual ~ProgressCallback() {}

	/**
	 * Perform action
	 */
	virtual void progress(float value) = 0;

};
