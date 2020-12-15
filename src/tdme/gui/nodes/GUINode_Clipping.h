#pragma once

#include <tdme/tdme.h>
#include <tdme/gui/nodes/fwd-tdme.h>

using tdme::gui::nodes::GUINode_Clipping;

/**
 * GUI node clipping entity
 * @author Andreas Drewke
 * @version $Id$
 */
struct tdme::gui::nodes::GUINode_Clipping
{

	int left { 0 };
	int top { 0 };
	int right { 0 };
	int bottom { 0 };
};
