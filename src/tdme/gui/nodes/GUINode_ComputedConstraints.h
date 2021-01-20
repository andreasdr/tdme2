#pragma once

#include <tdme/tdme.h>
#include <tdme/gui/nodes/fwd-tdme.h>

using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINode_Alignments;
using tdme::gui::nodes::GUINode_AlignmentHorizontal;
using tdme::gui::nodes::GUINode_AlignmentVertical;
using tdme::gui::nodes::GUINode_Border;
using tdme::gui::nodes::GUINode_Flow;
using tdme::gui::nodes::GUINode_Padding;
using tdme::gui::nodes::GUINode_RequestedConstraints;
using tdme::gui::nodes::GUINode_RequestedConstraints_RequestedConstraintsType;

/**
 * GUI node computed constraints
 * @author Andreas Drewke
 * @version $Id$
 */
struct tdme::gui::nodes::GUINode_ComputedConstraints
{
	int32_t left { 0 };
	int32_t top { 0 };
	int32_t width { 0 };
	int32_t height { 0 };
	int32_t alignmentLeft { 0 };
	int32_t alignmentTop { 0 };
	int32_t contentAlignmentLeft { 0 };
	int32_t contentAlignmentTop { 0 };
};
