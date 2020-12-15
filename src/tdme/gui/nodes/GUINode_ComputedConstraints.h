#pragma once

#include <tdme/tdme.h>
#include <tdme/gui/nodes/fwd-tdme.h>

using tdme::gui::nodes::GUINode_AlignmentHorizontal;
using tdme::gui::nodes::GUINode_AlignmentVertical;
using tdme::gui::nodes::GUINode_Alignments;
using tdme::gui::nodes::GUINode_Border;
using tdme::gui::nodes::GUINode_Flow;
using tdme::gui::nodes::GUINode_Padding;
using tdme::gui::nodes::GUINode_RequestedConstraints_RequestedConstraintsType;
using tdme::gui::nodes::GUINode_RequestedConstraints;
using tdme::gui::nodes::GUINode;

/**
 * GUI node computed constraints
 * @author Andreas Drewke
 * @version $Id$
 */
struct tdme::gui::nodes::GUINode_ComputedConstraints
{
	int left { 0 };
	int top { 0 };
	int width { 0 };
	int height { 0 };
	int alignmentLeft { 0 };
	int alignmentTop { 0 };
	int contentAlignmentLeft { 0 };
	int contentAlignmentTop { 0 };
};
