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
 * GUI node flow enum
 * @author Andreas Drewke
 */
class tdme::gui::nodes::GUINode_Flow final: public Enum
{
public:
	static GUINode_Flow* INTEGRATED;
	static GUINode_Flow* FLOATING;

	/**
	 * Public constructor
	 * @param name name
	 * @param ordinal ordinal
	 */
	GUINode_Flow(const string& name, int ordinal);

	/**
	 * Returns enum object given by name
	 * @param name name
	 * @return enum object
	 */
	static GUINode_Flow* valueOf(const string& name);

};
