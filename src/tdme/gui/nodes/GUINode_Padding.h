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

	int left { 0 };
	int top { 0 };
	int right { 0 };
	int bottom { 0 };
};
