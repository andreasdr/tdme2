#include <tdme/gui/nodes/GUINode_Flow.h>

#include <string>

#include <tdme/utilities/Enum.h>

using std::string;

using tdme::gui::nodes::GUINode_Flow;
using tdme::utilities::Enum;

GUINode_Flow::GUINode_Flow(const string& name, int ordinal)
	: Enum(name, ordinal)
{
}

GUINode_Flow* tdme::gui::nodes::GUINode_Flow::INTEGRATED = new GUINode_Flow("INTEGRATED", 0);
GUINode_Flow* tdme::gui::nodes::GUINode_Flow::FLOATING = new GUINode_Flow("FLOATING", 1);

GUINode_Flow* GUINode_Flow::valueOf(const string& name)
{
	if (FLOATING->getName() == name) return FLOATING;
	if (INTEGRATED->getName() == name) return INTEGRATED;
	// TODO: throw exception here maybe
	return nullptr;
}

