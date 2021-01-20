#pragma once

#include <tdme/tdme.h>
#include <tdme/gui/nodes/fwd-tdme.h>

/**
 * GUI node padding entity
 * @author Andreas Drewke
 * @version $Id$
 */
struct tdme::gui::nodes::GUINode_Padding
{

	int32_t left { 0 };
	int32_t top { 0 };
	int32_t right { 0 };
	int32_t bottom { 0 };
};
