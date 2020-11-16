#pragma once

#include <tdme/tdme.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/gui/nodes/GUIColor.h>

using tdme::gui::nodes::GUIColor;
using tdme::gui::nodes::GUINode_AlignmentHorizontal;
using tdme::gui::nodes::GUINode_AlignmentVertical;
using tdme::gui::nodes::GUINode_Alignments;
using tdme::gui::nodes::GUINode_ComputedConstraints;
using tdme::gui::nodes::GUINode_Flow;
using tdme::gui::nodes::GUINode_Padding;
using tdme::gui::nodes::GUINode_RequestedConstraints_RequestedConstraintsType;
using tdme::gui::nodes::GUINode_RequestedConstraints;
using tdme::gui::nodes::GUINode;

/**
 * GUI node border entity
 * @author Andreas Drewke
 * @version $Id$
 */
struct tdme::gui::nodes::GUINode_Border
{

	GUIColor leftColor;
	GUIColor topColor;
	GUIColor rightColor;
	GUIColor bottomColor;
	int32_t left { 0 };
	int32_t top { 0 };
	int32_t right { 0 };
	int32_t bottom { 0 };
};
