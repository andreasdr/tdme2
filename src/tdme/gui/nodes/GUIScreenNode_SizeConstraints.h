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

	int32_t minWidth { -1 };
	int32_t minHeight { -1 };
	int32_t maxWidth { -1 };
	int32_t maxHeight { -1 };

};
