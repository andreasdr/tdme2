#include <tdme/gui/nodes/GUINode_RequestedConstraints_RequestedConstraintsType.h>

#include <string>

#include <tdme/utilities/Enum.h>

using std::string;

using tdme::gui::nodes::GUINode_RequestedConstraints_RequestedConstraintsType;
using tdme::utilities::Enum;


GUINode_RequestedConstraints_RequestedConstraintsType::GUINode_RequestedConstraints_RequestedConstraintsType(const string& name, int ordinal)
	: Enum(name, ordinal)
{
}

GUINode_RequestedConstraints_RequestedConstraintsType* tdme::gui::nodes::GUINode_RequestedConstraints_RequestedConstraintsType::NONE = new GUINode_RequestedConstraints_RequestedConstraintsType("NONE", 0);
GUINode_RequestedConstraints_RequestedConstraintsType* tdme::gui::nodes::GUINode_RequestedConstraints_RequestedConstraintsType::PIXEL = new GUINode_RequestedConstraints_RequestedConstraintsType("PIXEL", 0);
GUINode_RequestedConstraints_RequestedConstraintsType* tdme::gui::nodes::GUINode_RequestedConstraints_RequestedConstraintsType::PERCENT = new GUINode_RequestedConstraints_RequestedConstraintsType("PERCENT", 1);
GUINode_RequestedConstraints_RequestedConstraintsType* tdme::gui::nodes::GUINode_RequestedConstraints_RequestedConstraintsType::STAR = new GUINode_RequestedConstraints_RequestedConstraintsType("STAR", 2);
GUINode_RequestedConstraints_RequestedConstraintsType* tdme::gui::nodes::GUINode_RequestedConstraints_RequestedConstraintsType::AUTO = new GUINode_RequestedConstraints_RequestedConstraintsType("AUTO", 3);

GUINode_RequestedConstraints_RequestedConstraintsType* GUINode_RequestedConstraints_RequestedConstraintsType::valueOf(const string& name)
{
	if (NONE->getName() == name) return NONE;
	if (AUTO->getName() == name) return AUTO;
	if (PERCENT->getName() == name) return PERCENT;
	if (PIXEL->getName() == name) return PIXEL;
	if (STAR->getName() == name) return STAR;
	// TODO: throw exception here maybe
	return nullptr;
}
