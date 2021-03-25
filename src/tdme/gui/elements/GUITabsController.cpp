#include <tdme/gui/elements/GUITabsController.h>

#include <tdme/gui/elements/GUITabContentController.h>
#include <tdme/gui/elements/GUITabController.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUINodeController.h>
#include <tdme/gui/nodes/GUIParentNode.h>
#include <tdme/gui/GUI.h>
#include <tdme/utilities/MutableString.h>

using tdme::gui::elements::GUITabsController;

using tdme::gui::elements::GUITabContentController;
using tdme::gui::elements::GUITabController;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeController;
using tdme::gui::nodes::GUIParentNode;
using tdme::gui::GUI;
using tdme::utilities::MutableString;

GUITabsController::GUITabsController(GUINode* node)
	: GUINodeController(node)
{
	init();
}

void GUITabsController::init()
{
}

bool GUITabsController::isDisabled()
{
	return false;
}

void GUITabsController::setDisabled(bool disabled)
{
}

void GUITabsController::initialize()
{
	required_dynamic_cast<GUIParentNode*>(node)->getChildControllerNodes(childControllerNodes);
	for (auto i = 0; i < childControllerNodes.size(); i++) {
		auto childControllerNode = childControllerNodes[i];
		auto childController = childControllerNode->getController();
		if (dynamic_cast<GUITabController*>(childController) != nullptr) {
			auto tabController = required_dynamic_cast< GUITabController* >(childController);
			if (static_cast<GUINode*>(tabController->getNode()->getParentControllerNode()->getParentControllerNode()) != node)
				continue;

			tabController->setSelected(true);
			setTabContentSelected(tabController->getNode()->getId());
			break;
		}
	}
}

void GUITabsController::dispose()
{
}

void GUITabsController::postLayout()
{
}

void GUITabsController::unselect()
{
	required_dynamic_cast<GUIParentNode*>(node)->getChildControllerNodes(childControllerNodes);
	for (auto i = 0; i < childControllerNodes.size(); i++) {
		auto childControllerNode = childControllerNodes[i];
		auto childController = childControllerNode->getController();
		if (dynamic_cast<GUITabController*>(childController) != nullptr) {
			auto tabController = required_dynamic_cast<GUITabController*>(childController);
			if (static_cast<GUINode*>(tabController->getNode()->getParentControllerNode()->getParentControllerNode()) != node)
				continue;

			tabController->setSelected(false);
		}
	}
}

void GUITabsController::setTabContentSelected(const string& id)
{
	MutableString tabContentNodeId;
	tabContentNodeId.set(id + "-content");
	required_dynamic_cast<GUIParentNode*>(node)->getChildControllerNodes(childControllerNodes);
	for (auto i = 0; i < childControllerNodes.size(); i++) {
		auto childControllerNode = childControllerNodes[i];
		auto childController = childControllerNode->getController();
		if (dynamic_cast<GUITabContentController*>(childController) != nullptr) {
			auto tabContentController = required_dynamic_cast<GUITabContentController*>(childController);
			if (static_cast<GUINode*>(tabContentController->getNode()->getParentControllerNode()->getParentControllerNode()) != node)
				continue;

			required_dynamic_cast<GUITabContentController*>(childController)->setSelected(tabContentNodeId.equals(childController->getNode()->getId()));
		}
	}
}

void GUITabsController::handleMouseEvent(GUINode* node, GUIMouseEvent* event)
{
}

void GUITabsController::handleKeyboardEvent(GUIKeyboardEvent* event)
{
}

void GUITabsController::tick()
{
}

void GUITabsController::onFocusGained()
{
}

void GUITabsController::onFocusLost()
{
}

bool GUITabsController::hasValue()
{
	return false;
}

const MutableString& GUITabsController::getValue()
{
	return value;
}

void GUITabsController::setValue(const MutableString& value)
{
}

