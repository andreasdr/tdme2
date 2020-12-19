#pragma once

#include <tdme/gui/nodes/fwd-tdme.h>

using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINode_AlignmentHorizontal;
using tdme::gui::nodes::GUINode_AlignmentVertical;
using tdme::gui::nodes::GUINode_Border;
using tdme::gui::nodes::GUINode_ComputedConstraints;
using tdme::gui::nodes::GUINode_Flow;
using tdme::gui::nodes::GUINode_Padding;
using tdme::gui::nodes::GUINode_RequestedConstraints;
using tdme::gui::nodes::GUINode_RequestedConstraints_RequestedConstraintsType;

/**
 * GUI node alignments
 * @author Andreas Drewke
 * @version $Id$
 */
struct tdme::gui::nodes::GUINode_Alignments
{
	GUINode_AlignmentHorizontal* horizontal { nullptr };
	GUINode_AlignmentVertical* vertical { nullptr };
};
