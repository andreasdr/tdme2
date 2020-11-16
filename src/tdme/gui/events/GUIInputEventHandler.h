#pragma once

#include <tdme/gui/events/fwd-tdme.h>

/**
 * GUI input event handler interface
 * @author Andreas Drewke
 * @version $Id$
 */
struct tdme::gui::events::GUIInputEventHandler
{

	/**
	 * Handle input events that have not yet been processed
	 */
	virtual void handleInputEvents() = 0;

	/**
	 * Destructor
	 */
	virtual ~GUIInputEventHandler() {}
};
