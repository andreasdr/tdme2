#include <tdme/gui/nodes/GUINodeController.h>

#include <tdme/gui/nodes/GUINode.h>

using tdme::gui::nodes::GUINodeController;
using tdme::gui::nodes::GUINode;

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

