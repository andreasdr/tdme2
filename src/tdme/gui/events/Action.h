#pragma once

#include <tdme/gui/events/fwd-tdme.h>

/**
 * Action Interface
 * @author Andreas Drewke
 * @version $Id$
 */
struct tdme::gui::events::Action
{
	/**
	 * Perform action
	 */
	virtual void performAction() = 0;

	/**
	 * Destructor
	 */
	virtual ~Action() {}
};
