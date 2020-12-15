#pragma once

#include <tdme/tdme.h>
#include <tdme/gui/nodes/fwd-tdme.h>

using tdme::gui::nodes::GUINode_AlignmentHorizontal;
using tdme::gui::nodes::GUINode_AlignmentVertical;
using tdme::gui::nodes::GUINode_Alignments;
using tdme::gui::nodes::GUINode_Border;
using tdme::gui::nodes::GUINode_ComputedConstraints;
using tdme::gui::nodes::GUINode_Flow;
using tdme::gui::nodes::GUINode_Padding;
using tdme::gui::nodes::GUINode_RequestedConstraints_RequestedConstraintsType;
using tdme::gui::nodes::GUINode;

/**
 * GUI node requested constraints entity
 * @author Andreas Drewke
 * @version $Id$
 */
struct tdme::gui::nodes::GUINode_RequestedConstraints
{
	GUINode_RequestedConstraints_RequestedConstraintsType* leftType { nullptr };
	int left { 0 };
	GUINode_RequestedConstraints_RequestedConstraintsType* topType { nullptr };
	int top { 0 };
	GUINode_RequestedConstraints_RequestedConstraintsType* widthType { nullptr };
	int width { 0 };
	GUINode_RequestedConstraints_RequestedConstraintsType* heightType { nullptr };
	int height { 0 };
};
