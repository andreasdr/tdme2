#include <tdme/gui/elements/GUITabController.h>

#include <tdme/gui/GUI.h>
#include <tdme/gui/elements/GUITabsController.h>
#include <tdme/gui/elements/GUITabsHeaderController.h>
#include <tdme/gui/events/GUIMouseEvent.h>
#include <tdme/gui/nodes/GUIColor.h>
#include <tdme/gui/nodes/GUIElementController.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUINode_Border.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUINodeConditions.h>
#include <tdme/gui/nodes/GUINodeController.h>
#include <tdme/gui/nodes/GUIParentNode.h>
#include <tdme/gui/nodes/GUIScreenNode.h>

using tdme::gui::elements::GUITabController;
using tdme::gui::GUI;
using tdme::gui::elements::GUITabsController;
using tdme::gui::elements::GUITabsHeaderController;
using tdme::gui::events::GUIMouseEvent;
using tdme::gui::nodes::GUIColor;
using tdme::gui::nodes::GUIElementController;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINode_Border;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeConditions;
using tdme::gui::nodes::GUINodeController;
using tdme::gui::nodes::GUIParentNode;
using tdme::gui::nodes::GUIScreenNode;

string GUITabController::CONDITION_DISABLED = "disabled";
string GUITabController::CONDITION_ENABLED = "enabled";
string GUITabController::CONDITION_SELECTED = "selected";
string GUITabController::CONDITION_UNSELECTED = "unselected";

GUITabController::GUITabController(GUINode* node)
	: GUIElementController(node)
{
	this->tabsNode = nullptr;
	this->tabsHeaderNode = nullptr;
	this->selected = false;
	this->disabled = required_dynamic_cast<GUIElementNode*>(node)->isDisabled();
}

bool GUITabController::isDisabled()
{
	return disabled;
}

void GUITabController::setDisabled(bool disabled)
{
	auto& nodeConditions = required_dynamic_cast<GUIElementNode*>(node)->getActiveConditions();
	nodeConditions.remove(this->disabled == true?CONDITION_DISABLED:CONDITION_ENABLED);
	this->disabled = disabled;
	nodeConditions.add(this->disabled == true?CONDITION_DISABLED:CONDITION_ENABLED);
}

bool GUITabController::isSelected()
{
	return selected;
}

void GUITabController::setSelected(bool selected)
{
	auto& nodeConditions = required_dynamic_cast<GUIElementNode*>(this->node)->getActiveConditions();
	nodeConditions.remove(this->selected == true?CONDITION_SELECTED:CONDITION_UNSELECTED);
	this->selected = selected;
	nodeConditions.add(this->selected == true?CONDITION_SELECTED:CONDITION_UNSELECTED);
	if (required_dynamic_cast<GUITabsHeaderController*>(tabsHeaderNode->getController())->hasFocus() == true) {
		if (selected == true) {
			auto focussedBorderColor = node->getScreenNode()->getGUI()->getFoccussedBorderColor();
			auto border = node->getBorder();
			border.topColor = focussedBorderColor;
			border.leftColor = focussedBorderColor;
			border.bottomColor = focussedBorderColor;
			border.rightColor = focussedBorderColor;
		} else {
			auto border = node->getBorder();
			border.topColor = unfocussedNodeBorderTopColor;
			border.leftColor = unfocussedNodeBorderLeftColor;
			border.bottomColor = unfocussedNodeBorderBottomColor;
			border.rightColor = unfocussedNodeBorderRightColor;
		}
	} else {
		auto border = node->getBorder();
		border.topColor = unfocussedNodeBorderTopColor;
		border.leftColor = unfocussedNodeBorderLeftColor;
		border.bottomColor = unfocussedNodeBorderBottomColor;
		border.rightColor = unfocussedNodeBorderRightColor;
	}
}

void GUITabController::initialize()
{
	tabsNode = required_dynamic_cast<GUIParentNode*>(node)->getParentControllerNode()->getParentControllerNode();
	tabsHeaderNode = required_dynamic_cast<GUIParentNode*>(node)->getParentControllerNode();
	auto border = node->getBorder();
	unfocussedNodeBorderTopColor = border.topColor;
	unfocussedNodeBorderLeftColor = border.leftColor;
	unfocussedNodeBorderBottomColor = border.bottomColor;
	unfocussedNodeBorderRightColor = border.rightColor;
	setSelected(selected);

	//
	GUIElementController::initialize();
}

void GUITabController::dispose()
{
	GUIElementController::dispose();
}

void GUITabController::postLayout()
{
}

void GUITabController::handleMouseEvent(GUINode* node, GUIMouseEvent* event)
{
	GUIElementController::handleMouseEvent(node, event);
	if (disabled == false && node == this->node && node->isEventBelongingToNode(event) && event->getButton() == MOUSE_BUTTON_LEFT) {
		event->setProcessed(true);
		if (event->getType() == GUIMouseEvent::MOUSEEVENT_RELEASED) {
			auto guiTabsController = required_dynamic_cast<GUITabsController*>(tabsNode->getController());
			guiTabsController->unselect();
			setSelected(selected == true?false:true);
			guiTabsController->setTabContentSelected(node->getId());
		}
	}
}

void GUITabController::handleKeyboardEvent(GUINode* node, GUIKeyboardEvent* event)
{
	GUIElementController::handleKeyboardEvent(node, event);
}

void GUITabController::tick()
{
	GUIElementController::tick();
}

void GUITabController::onFocusGained()
{
}

void GUITabController::onFocusLost()
{
}

bool GUITabController::hasValue()
{
	return false;
}

const MutableString& GUITabController::getValue()
{
	return value;
}

void GUITabController::setValue(const MutableString& value)
{
}

void GUITabController::selectTab()
{
	auto guiTabsController = required_dynamic_cast<GUITabsController*>(tabsNode->getController());
	guiTabsController->unselect();
	setSelected(true);
	guiTabsController->setTabContentSelected(node->getId());
	node->getScreenNode()->getGUI()->invalidateFocussedNode();
}
