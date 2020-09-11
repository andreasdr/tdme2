#include <tdme/gui/elements/GUISelectBoxMultipleController.h>

#include <tdme/gui/GUI.h>
#include <tdme/gui/elements/GUISelectBoxMultipleOptionController.h>
#include <tdme/gui/events/GUIKeyboardEvent_Type.h>
#include <tdme/gui/events/GUIKeyboardEvent.h>
#include <tdme/gui/events/GUIMouseEvent_Type.h>
#include <tdme/gui/events/GUIMouseEvent.h>
#include <tdme/gui/nodes/GUIElementController.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUINodeConditions.h>
#include <tdme/gui/nodes/GUIParentNode.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/utilities/MutableString.h>

using tdme::gui::elements::GUISelectBoxMultipleController;
using tdme::gui::GUI;
using tdme::gui::elements::GUISelectBoxMultipleOptionController;
using tdme::gui::events::GUIKeyboardEvent_Type;
using tdme::gui::events::GUIKeyboardEvent;
using tdme::gui::events::GUIMouseEvent_Type;
using tdme::gui::events::GUIMouseEvent;
using tdme::gui::nodes::GUIElementController;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeConditions;
using tdme::gui::nodes::GUIParentNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::utilities::MutableString;

constexpr char GUISelectBoxMultipleController::VALUE_DELIMITER;

string GUISelectBoxMultipleController::CONDITION_DISABLED = "disabled";
string GUISelectBoxMultipleController::CONDITION_ENABLED = "enabled";

GUISelectBoxMultipleController::GUISelectBoxMultipleController(GUINode* node) 
	: GUIElementController(node)
{
	init();
	this->disabled = (dynamic_cast< GUIElementNode* >(node))->isDisabled();
}

void GUISelectBoxMultipleController::init()
{
}

bool GUISelectBoxMultipleController::isDisabled()
{
	return disabled;
}

void GUISelectBoxMultipleController::setDisabled(bool disabled)
{
	auto& nodeConditions = (dynamic_cast< GUIElementNode* >(node))->getActiveConditions();
	nodeConditions.remove(this->disabled == true ? CONDITION_DISABLED : CONDITION_ENABLED);
	this->disabled = disabled;
	nodeConditions.add(this->disabled == true ? CONDITION_DISABLED : CONDITION_ENABLED);
	selectCurrent();
}

void GUISelectBoxMultipleController::initialize()
{
	if (getFocussedOptionIdx() == -1) {
		auto value = getValue();
		setValue(value);
	}
	GUIElementController::initialize();
}

void GUISelectBoxMultipleController::dispose()
{
	GUIElementController::dispose();
}

void GUISelectBoxMultipleController::postLayout()
{
}

void GUISelectBoxMultipleController::unselect()
{
	(dynamic_cast< GUIParentNode* >(node))->getChildControllerNodes(childControllerNodes);
	for (auto i = 0; i < childControllerNodes.size(); i++) {
		auto childControllerNode = childControllerNodes[i];
		auto childController = childControllerNode->getController();
		if (dynamic_cast< GUISelectBoxMultipleOptionController* >(childController) != nullptr) {
			(dynamic_cast< GUISelectBoxMultipleOptionController* >(childController))->unselect();
		}
	}
}

void GUISelectBoxMultipleController::unfocus()
{
	(dynamic_cast< GUIParentNode* >(node))->getChildControllerNodes(childControllerNodes);
	for (auto i = 0; i < childControllerNodes.size(); i++) {
		auto childControllerNode = childControllerNodes[i];
		auto childController = childControllerNode->getController();
		if (dynamic_cast< GUISelectBoxMultipleOptionController* >(childController) != nullptr) {
			(dynamic_cast< GUISelectBoxMultipleOptionController* >(childController))->unfocus();
		}
	}
}

void GUISelectBoxMultipleController::determineSelectBoxMultipleOptionControllers()
{
	selectBoxMultipleOptionControllers.clear();
	(dynamic_cast< GUIParentNode* >(node))->getChildControllerNodes(childControllerNodes);
	for (auto i = 0; i < childControllerNodes.size(); i++) {
		auto childControllerNode = childControllerNodes[i];
		auto childController = childControllerNode->getController();
		if (dynamic_cast< GUISelectBoxMultipleOptionController* >(childController) != nullptr) {
			selectBoxMultipleOptionControllers.push_back(dynamic_cast< GUISelectBoxMultipleOptionController* >(childController));
		}
	}
}

int32_t GUISelectBoxMultipleController::getFocussedOptionIdx()
{
	auto selectBoxOptionControllerIdx = -1;
	for (auto i = 0; i < selectBoxMultipleOptionControllers.size(); i++) {
		auto selectBoxOptionController = selectBoxMultipleOptionControllers[i];
		if (selectBoxOptionController->isFocussed() == true) {
			selectBoxOptionControllerIdx = i;
			break;
		}
	}
	return selectBoxOptionControllerIdx;
}

void GUISelectBoxMultipleController::selectCurrent()
{
	setValue(getValue());
}

void GUISelectBoxMultipleController::focusNext()
{
	determineSelectBoxMultipleOptionControllers();
	auto selectBoxMultipleOptionControllerIdx = getFocussedOptionIdx();
	unfocus();
	if (selectBoxMultipleOptionControllers.size() == 0)
		return;

	selectBoxMultipleOptionControllerIdx = (selectBoxMultipleOptionControllerIdx + 1) % selectBoxMultipleOptionControllers.size();
	if (selectBoxMultipleOptionControllerIdx < 0)
		selectBoxMultipleOptionControllerIdx += selectBoxMultipleOptionControllers.size();

	selectBoxMultipleOptionControllers[selectBoxMultipleOptionControllerIdx]->focus();
	selectBoxMultipleOptionControllers[selectBoxMultipleOptionControllerIdx]->getNode()->scrollToNodeX(dynamic_cast< GUIParentNode* >(node));
	selectBoxMultipleOptionControllers[selectBoxMultipleOptionControllerIdx]->getNode()->scrollToNodeY(dynamic_cast< GUIParentNode* >(node));
}

void GUISelectBoxMultipleController::focusPrevious()
{
	determineSelectBoxMultipleOptionControllers();
	auto selectBoxMultipleOptionControllerIdx = getFocussedOptionIdx();
	unfocus();
	if (selectBoxMultipleOptionControllers.size() == 0)
		return;

	selectBoxMultipleOptionControllerIdx = (selectBoxMultipleOptionControllerIdx - 1) % selectBoxMultipleOptionControllers.size();
	if (selectBoxMultipleOptionControllerIdx < 0)
		selectBoxMultipleOptionControllerIdx += selectBoxMultipleOptionControllers.size();

	selectBoxMultipleOptionControllers[selectBoxMultipleOptionControllerIdx]->focus();
	selectBoxMultipleOptionControllers[selectBoxMultipleOptionControllerIdx]->getNode()->scrollToNodeX(dynamic_cast< GUIParentNode* >(node));
	selectBoxMultipleOptionControllers[selectBoxMultipleOptionControllerIdx]->getNode()->scrollToNodeY(dynamic_cast< GUIParentNode* >(node));
}

void GUISelectBoxMultipleController::toggle()
{
	determineSelectBoxMultipleOptionControllers();
	auto selectBoxMultipleOptionControllerIdx = getFocussedOptionIdx();
	selectBoxMultipleOptionControllers[selectBoxMultipleOptionControllerIdx]->toggle();
	selectBoxMultipleOptionControllers[selectBoxMultipleOptionControllerIdx]->getNode()->scrollToNodeX(dynamic_cast< GUIParentNode* >(node));
	selectBoxMultipleOptionControllers[selectBoxMultipleOptionControllerIdx]->getNode()->scrollToNodeY(dynamic_cast< GUIParentNode* >(node));
}

void GUISelectBoxMultipleController::handleMouseEvent(GUINode* node, GUIMouseEvent* event)
{
	GUIElementController::handleMouseEvent(node, event);
	auto disabled = (dynamic_cast< GUISelectBoxMultipleController* >(this->node->getController()))->isDisabled();
	if (disabled == false && node == this->node && node->isEventBelongingToNode(event) && event->getButton() == MOUSE_BUTTON_LEFT) {
		event->setProcessed(true);
		if (event->getType() == GUIMouseEvent_Type::MOUSEEVENT_PRESSED) {
			node->getScreenNode()->getGUI()->setFoccussedNode(dynamic_cast< GUIElementNode* >(node));
		}
	}
}

void GUISelectBoxMultipleController::handleKeyboardEvent(GUINode* node, GUIKeyboardEvent* event)
{
	GUIElementController::handleKeyboardEvent(node, event);
	if (disabled == false && node == this->node) {
		switch (event->getKeyCode()) {
		case GUIKeyboardEvent::KEYCODE_UP: {
				event->setProcessed(true);
				if (event->getType() == GUIKeyboardEvent_Type::KEYBOARDEVENT_KEY_PRESSED) {
					focusPrevious();
				}
			}
			break;
		case GUIKeyboardEvent::KEYCODE_DOWN: {
				event->setProcessed(true);
				if (event->getType() == GUIKeyboardEvent_Type::KEYBOARDEVENT_KEY_PRESSED) {
					focusNext();
				}
			}
			break;
		case GUIKeyboardEvent::KEYCODE_SPACE: {
				event->setProcessed(true);
				if (event->getType() == GUIKeyboardEvent_Type::KEYBOARDEVENT_KEY_PRESSED) {
					toggle();
				}
			}
			break;
		}

	}
}

void GUISelectBoxMultipleController::tick()
{
	GUIElementController::tick();
}

void GUISelectBoxMultipleController::onFocusGained()
{
}

void GUISelectBoxMultipleController::onFocusLost()
{
}

bool GUISelectBoxMultipleController::hasValue()
{
	return true;
}

const MutableString& GUISelectBoxMultipleController::getValue()
{
	value.reset();
	determineSelectBoxMultipleOptionControllers();
	for (auto i = 0; i < selectBoxMultipleOptionControllers.size(); i++) {
		auto selectBoxOptionController = selectBoxMultipleOptionControllers[i];
		if (selectBoxOptionController->isSelected() == true) {
			value.append((dynamic_cast< GUIElementNode* >(selectBoxOptionController->getNode()))->getValue());
			value.append(VALUE_DELIMITER);
		}
	}
	if (value.length() > 0) {
		value.insert(0, VALUE_DELIMITER);
	}
	return value;
}

void GUISelectBoxMultipleController::setValue(const MutableString& value)
{
	determineSelectBoxMultipleOptionControllers();
	unselect();
	unfocus();
	MutableString searchValue;
	GUIElementNode* selectBoxOptionNodeLast = nullptr;
	for (auto i = 0; i < selectBoxMultipleOptionControllers.size(); i++) {
		auto selectBoxOptionController = selectBoxMultipleOptionControllers[i];
		auto selectBoxOptionNode = dynamic_cast< GUIElementNode* >(selectBoxOptionController->getNode());
		searchValue.reset();
		searchValue.append(VALUE_DELIMITER);
		searchValue.append(selectBoxOptionNode->getValue());
		searchValue.append(VALUE_DELIMITER);
		if (value.indexOf(searchValue) != -1) {
			selectBoxOptionController->select();
			selectBoxOptionNode->scrollToNodeX(dynamic_cast< GUIParentNode* >(node));
			selectBoxOptionNode->scrollToNodeY(dynamic_cast< GUIParentNode* >(node));
			selectBoxOptionNodeLast = selectBoxOptionNode;
		}
	}
	if (selectBoxOptionNodeLast != nullptr) {
		(dynamic_cast< GUISelectBoxMultipleOptionController* >(selectBoxOptionNodeLast->getController()))->focus();
	}
}
