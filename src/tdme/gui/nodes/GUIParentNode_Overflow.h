#pragma once

#include <string>

#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/utilities/Enum.h>

using std::string;

using tdme::utilities::Enum;
using tdme::gui::nodes::GUIParentNode;
using tdme::gui::nodes::GUIParentNode_Overflow;

/**
 * GUI parent node overflow enum
 * @author Andreas Drewke
 */
class tdme::gui::nodes::GUIParentNode_Overflow final: public Enum
{
public:
	static GUIParentNode_Overflow* HIDDEN;
	static GUIParentNode_Overflow* DOWNSIZE_CHILDREN;
	static GUIParentNode_Overflow* SCROLL;

	/**
	 * Public constructor
	 * @param name name
	 * @param ordinal ordinal
	 */
	GUIParentNode_Overflow(const string& name, int ordinal);

	/**
	 * Returns enum object given by name
	 * @param name name
	 * @return enum object
	 */
	static GUIParentNode_Overflow* valueOf(const string& name);
};
