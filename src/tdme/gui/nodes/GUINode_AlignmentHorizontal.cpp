#include <tdme/gui/nodes/GUINode_AlignmentHorizontal.h>

#include <string>

#include <tdme/utilities/Enum.h>

using std::string;

using tdme::gui::nodes::GUINode_AlignmentHorizontal;
using tdme::utilities::Enum;

GUINode_AlignmentHorizontal::GUINode_AlignmentHorizontal(const string& name, int ordinal)
	: Enum(name, ordinal)
{
}

GUINode_AlignmentHorizontal* tdme::gui::nodes::GUINode_AlignmentHorizontal::NONE = new GUINode_AlignmentHorizontal("NONE", 0);
GUINode_AlignmentHorizontal* tdme::gui::nodes::GUINode_AlignmentHorizontal::LEFT = new GUINode_AlignmentHorizontal("LEFT", 1);
GUINode_AlignmentHorizontal* tdme::gui::nodes::GUINode_AlignmentHorizontal::CENTER = new GUINode_AlignmentHorizontal("CENTER", 2);
GUINode_AlignmentHorizontal* tdme::gui::nodes::GUINode_AlignmentHorizontal::RIGHT = new GUINode_AlignmentHorizontal("RIGHT", 3);

GUINode_AlignmentHorizontal* GUINode_AlignmentHorizontal::valueOf(const string& a0)
{
	if (NONE->getName() == a0) return NONE;
	if (LEFT->getName() == a0) return LEFT;
	if (CENTER->getName() == a0) return CENTER;
	if (RIGHT->getName() == a0) return RIGHT;
	// TODO: throw exception here maybe
	return nullptr;
}
