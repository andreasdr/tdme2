#pragma once

#include <string>

#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/utilities/Enum.h>

using std::string;

using tdme::gui::nodes::GUILayoutNode;
using tdme::gui::nodes::GUILayoutNode_Alignment;
using tdme::utilities::Enum;

/**
 * GUI layout node alignment enum
 * @author Andreas Drewke
 */
class tdme::gui::nodes::GUILayoutNode_Alignment final: public Enum
{
public:
	static GUILayoutNode_Alignment* VERTICAL;
	static GUILayoutNode_Alignment* HORIZONTAL;
	static GUILayoutNode_Alignment* NONE;

	/**
	 * Public constructor
	 * @param name name
	 * @param ordinal ordinal
	 */
	GUILayoutNode_Alignment(const string& name, int ordinal);

	/**
	 * Returns enum object given by name
	 * @param name name
	 * @return enum object
	 */
	static GUILayoutNode_Alignment* valueOf(const string& name);
};
