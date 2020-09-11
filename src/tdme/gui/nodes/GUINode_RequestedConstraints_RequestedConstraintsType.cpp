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

GUINode_RequestedConstraints_RequestedConstraintsType* GUINode_RequestedConstraints_RequestedConstraintsType::valueOf(const string& a0)
{
	if (NONE->getName() == a0) return NONE;
	if (AUTO->getName() == a0) return AUTO;
	if (PERCENT->getName() == a0) return PERCENT;
	if (PIXEL->getName() == a0) return PIXEL;
	if (STAR->getName() == a0) return STAR;
	// TODO: throw exception here maybe
	return nullptr;
}
