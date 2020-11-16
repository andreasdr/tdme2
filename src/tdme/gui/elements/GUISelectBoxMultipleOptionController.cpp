#include <tdme/gui/elements/GUISelectBoxMultipleOptionController.h>

#include <tdme/gui/GUI.h>
#include <tdme/gui/elements/GUISelectBoxMultipleController.h>
#include <tdme/gui/events/GUIMouseEvent.h>
#include <tdme/gui/nodes/GUIElementController.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUINodeConditions.h>
#include <tdme/gui/nodes/GUIParentNode.h>
#include <tdme/gui/nodes/GUIScreenNode.h>

using tdme::gui::elements::GUISelectBoxMultipleOptionController;
using tdme::gui::GUI;
using tdme::gui::elements::GUISelectBoxMultipleController;
using tdme::gui::events::GUIMouseEvent;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeConditions;
using tdme::gui::nodes::GUINodeController;
using tdme::gui::nodes::GUIParentNode;
using tdme::gui::nodes::GUIScreenNode;

string GUISelectBoxMultipleOptionController::CONDITION_SELECTED = "selected";
string GUISelectBoxMultipleOptionController::CONDITION_UNSELECTED = "unselected";
string GUISelectBoxMultipleOptionController::CONDITION_FOCUSSED = "focussed";
string GUISelectBoxMultipleOptionController::CONDITION_UNFOCUSSED = "unfocussed";
string GUISelectBoxMultipleOptionController::CONDITION_DISABLED = "disabled";
string GUISelectBoxMultipleOptionController::CONDITION_ENABLED = "enabled";

GUISelectBoxMultipleOptionController::GUISelectBoxMultipleOptionController(GUINode* node)
	: GUIElementController(node)
{
	this->initialPostLayout = true;
	this->selected = (dynamic_cast< GUIElementNode* >(node))->isSelected();
	this->focussed = false;
}

bool GUISelectBoxMultipleOptionController::isDisabled()
{
	return false;
}

void GUISelectBoxMultipleOptionController::setDisabled(bool disabled)
{
}

bool GUISelectBoxMultipleOptionController::isSelected()
{
	return selected;
}

void GUISelectBoxMultipleOptionController::select()
{
	auto& nodeConditions = (dynamic_cast< GUIElementNode* >(node))->getActiveConditions();
	nodeConditions.remove(this->selected == true ? CONDITION_SELECTED : CONDITION_UNSELECTED);
	this->selected = true;
	nodeConditions.add(this->selected == true ? CONDITION_SELECTED : CONDITION_UNSELECTED);
	auto disabled = (dynamic_cast< GUISelectBoxMultipleController* >(selectBoxMultipleNode->getController()))->isDisabled();
	nodeConditions.remove(CONDITION_DISABLED);
	nodeConditions.remove(CONDITION_ENABLED);
	nodeConditions.add(disabled == true ? CONDITION_DISABLED : CONDITION_ENABLED);
}

void GUISelectBoxMultipleOptionController::unselect()
{
	auto& nodeConditions = (dynamic_cast< GUIElementNode* >(node))->getActiveConditions();
	nodeConditions.remove(this->selected == true ? CONDITION_SELECTED : CONDITION_UNSELECTED);
	this->selected = false;
	nodeConditions.add(this->selected == true ? CONDITION_SELECTED : CONDITION_UNSELECTED);
	auto disabled = (dynamic_cast< GUISelectBoxMultipleController* >(selectBoxMultipleNode->getController()))->isDisabled();
	nodeConditions.remove(CONDITION_DISABLED);
	nodeConditions.remove(CONDITION_ENABLED);
	nodeConditions.add(disabled == true ? CONDITION_DISABLED : CONDITION_ENABLED);
}

void GUISelectBoxMultipleOptionController::toggle()
{
	if (selected == true) {
		unselect();
	} else {
		select();
	}
}

bool GUISelectBoxMultipleOptionController::isFocussed()
{
	return focussed;
}

void GUISelectBoxMultipleOptionController::focus()
{
	auto& nodeConditions = (dynamic_cast< GUIElementNode* >(node))->getActiveConditions();
	nodeConditions.remove(this->focussed == true ? CONDITION_FOCUSSED : CONDITION_UNFOCUSSED);
	this->focussed = true;
	nodeConditions.add(this->focussed == true ? CONDITION_FOCUSSED : CONDITION_UNFOCUSSED);
}

void GUISelectBoxMultipleOptionController::unfocus()
{
	auto& nodeConditions = (dynamic_cast< GUIElementNode* >(node))->getActiveConditions();
	nodeConditions.remove(this->focussed == true ? CONDITION_FOCUSSED : CONDITION_UNFOCUSSED);
	this->focussed = false;
	nodeConditions.add(this->focussed == true ? CONDITION_FOCUSSED : CONDITION_UNFOCUSSED);
}

bool GUISelectBoxMultipleOptionController::isCollapsed() {
	auto _node = node->getParentNode();
	while(_node != nullptr && _node != selectBoxMultipleNode) {
		if (_node->isConditionsMet() == false) return true;
		_node = _node->getParentNode();
	}
	return false;
}

void GUISelectBoxMultipleOptionController::initialize()
{
	selectBoxMultipleNode = node->getParentControllerNode();
	while (true == true) {
		if (dynamic_cast< GUISelectBoxMultipleController* >(selectBoxMultipleNode->getController()) != nullptr) {
			break;
		}
		selectBoxMultipleNode = selectBoxMultipleNode->getParentControllerNode();
	}
	if (selected == true) {
		select();
	} else {
		unselect();
	}

	//
	GUIElementController::initialize();
}

void GUISelectBoxMultipleOptionController::dispose()
{
	GUIElementController::dispose();
}

void GUISelectBoxMultipleOptionController::postLayout()
{
	if (initialPostLayout != true) return;
	if (selected == true) {
		node->scrollToNodeX(selectBoxMultipleNode);
		node->scrollToNodeY(selectBoxMultipleNode);
	}
	initialPostLayout = false;
}

void GUISelectBoxMultipleOptionController::handleMouseEvent(GUINode* node, GUIMouseEvent* event)
{
	GUIElementController::handleMouseEvent(node, event);
	auto disabled = (dynamic_cast< GUISelectBoxMultipleController* >(selectBoxMultipleNode->getController()))->isDisabled();
	if (disabled == false && node == this->node && node->isEventBelongingToNode(event) && event->getButton() == MOUSE_BUTTON_LEFT) {
		event->setProcessed(true);
		if (event->getType() == GUIMouseEvent::MOUSEEVENT_PRESSED) {
			(dynamic_cast< GUISelectBoxMultipleController* >(selectBoxMultipleNode->getController()))->unfocus();
			toggle();
			focus();
			node->getScreenNode()->getGUI()->setFoccussedNode(dynamic_cast< GUIElementNode* >(selectBoxMultipleNode));
			node->scrollToNodeX(selectBoxMultipleNode);
			node->scrollToNodeY(selectBoxMultipleNode);
			node->getScreenNode()->delegateValueChanged(dynamic_cast< GUIElementNode* >(selectBoxMultipleNode));
		}
	}
}

void GUISelectBoxMultipleOptionController::handleKeyboardEvent(GUINode* node, GUIKeyboardEvent* event)
{
	GUIElementController::handleKeyboardEvent(node, event);
}

void GUISelectBoxMultipleOptionController::onFocusGained()
{
}

void GUISelectBoxMultipleOptionController::onFocusLost()
{
}

bool GUISelectBoxMultipleOptionController::hasValue()
{
	return false;
}

const MutableString& GUISelectBoxMultipleOptionController::getValue()
{
	return value;
}

void GUISelectBoxMultipleOptionController::setValue(const MutableString& value)
{
}
