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
	GUIParentNode_Overflow(const string& name, int ordinal);
	static GUIParentNode_Overflow* valueOf(const string& a0);
};
