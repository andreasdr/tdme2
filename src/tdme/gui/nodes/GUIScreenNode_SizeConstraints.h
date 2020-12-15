#pragma once

#include <tdme/tdme.h>
#include <tdme/gui/nodes/fwd-tdme.h>

/**
 * GUI screen node size constraints entity
 * @author Andreas Drewke
 * @version $Id$
 */
struct tdme::gui::nodes::GUIScreenNode_SizeConstraints
{

	int minWidth { -1 };
	int minHeight { -1 };
	int maxWidth { -1 };
	int maxHeight { -1 };

};
