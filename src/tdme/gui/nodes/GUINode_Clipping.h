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

	int32_t left { 0 };
	int32_t top { 0 };
	int32_t right { 0 };
	int32_t bottom { 0 };
};
