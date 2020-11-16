#pragma once

#include <tdme/gui/events/fwd-tdme.h>
#include <tdme/gui/nodes/fwd-tdme.h>

using tdme::gui::nodes::GUIElementNode;

/**
 * GUI focus listener interface
 * @author Andreas Drewke
 * @version $Id$
 */
struct tdme::gui::events::GUIFocusListener
{

	/**
	 * Destructor
	 */
	virtual ~GUIFocusListener() {}

	/**
	 * On focus
	 * @param node node
	 */
	virtual void onFocus(GUIElementNode* node) = 0;

	/**
	 * On unfocus
	 * @param node node
	 */
	virtual void onUnfocus(GUIElementNode* node) = 0;

};
