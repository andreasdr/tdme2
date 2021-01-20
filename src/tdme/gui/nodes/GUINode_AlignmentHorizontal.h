#pragma once

#include <string>

#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/utilities/Enum.h>

using std::string;

using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINode_Alignments;
using tdme::gui::nodes::GUINode_AlignmentHorizontal;
using tdme::gui::nodes::GUINode_AlignmentVertical;
using tdme::gui::nodes::GUINode_Border;
using tdme::gui::nodes::GUINode_ComputedConstraints;
using tdme::gui::nodes::GUINode_Flow;
using tdme::gui::nodes::GUINode_Padding;
using tdme::gui::nodes::GUINode_RequestedConstraints;
using tdme::gui::nodes::GUINode_RequestedConstraints_RequestedConstraintsType;
using tdme::utilities::Enum;

/**
 * GUI node horizontal alignment enum
 * @author Andreas Drewke
 */
class tdme::gui::nodes::GUINode_AlignmentHorizontal final: public Enum
{
public:
	static GUINode_AlignmentHorizontal* NONE;
	static GUINode_AlignmentHorizontal* LEFT;
	static GUINode_AlignmentHorizontal* CENTER;
	static GUINode_AlignmentHorizontal* RIGHT;

	/**
	 * Public constructor
	 * @param name name
	 * @param ordinal ordinal
	 */
	GUINode_AlignmentHorizontal(const string& name, int ordinal);

	/**
	 * Returns enum object given by name
	 * @param name name
	 * @return enum object
	 */
	static GUINode_AlignmentHorizontal* valueOf(const string& name);
};
