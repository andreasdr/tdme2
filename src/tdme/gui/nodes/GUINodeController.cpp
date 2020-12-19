#include <tdme/gui/nodes/GUINodeController.h>

#include <tdme/gui/nodes/GUINode.h>

using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeController;

GUINodeController::GUINodeController(GUINode* node)
{
	this->node = node;
}

GUINodeController::~GUINodeController() {
}

GUINode* GUINodeController::getNode()
{
	return node;
}

