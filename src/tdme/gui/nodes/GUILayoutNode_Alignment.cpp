#include <tdme/gui/nodes/GUILayoutNode_Alignment.h>

#include <string>

#include <tdme/utilities/Enum.h>

using std::string;

using tdme::gui::nodes::GUILayoutNode_Alignment;
using tdme::utilities::Enum;

GUILayoutNode_Alignment::GUILayoutNode_Alignment(const string& name, int ordinal)
	: Enum(name, ordinal)
{
}

GUILayoutNode_Alignment* tdme::gui::nodes::GUILayoutNode_Alignment::VERTICAL = new GUILayoutNode_Alignment("VERTICAL", 0);
GUILayoutNode_Alignment* tdme::gui::nodes::GUILayoutNode_Alignment::HORIZONTAL = new GUILayoutNode_Alignment("HORIZONTAL", 1);
GUILayoutNode_Alignment* tdme::gui::nodes::GUILayoutNode_Alignment::NONE = new GUILayoutNode_Alignment("NONE", 2);

GUILayoutNode_Alignment* GUILayoutNode_Alignment::valueOf(const string& a0)
{
	if (HORIZONTAL->getName() == a0) return HORIZONTAL;
	if (NONE->getName() == a0) return NONE;
	if (VERTICAL->getName() == a0) return VERTICAL;
	// TODO: throw exception here maybe
	return nullptr;
}
