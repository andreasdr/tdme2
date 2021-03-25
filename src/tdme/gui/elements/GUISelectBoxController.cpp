#include <tdme/gui/elements/GUISelectBoxController.h>

#include <tdme/gui/elements/GUISelectBoxOptionController.h>
#include <tdme/gui/elements/GUISelectBoxParentOptionController.h>
#include <tdme/gui/events/GUIKeyboardEvent.h>
#include <tdme/gui/events/GUIMouseEvent.h>
#include <tdme/gui/nodes/GUIElementController.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUINodeConditions.h>
#include <tdme/gui/nodes/GUIParentNode.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/gui/GUI.h>
#include <tdme/utilities/MutableString.h>

using tdme::gui::elements::GUISelectBoxController;
using tdme::gui::elements::GUISelectBoxOptionController;
using tdme::gui::elements::GUISelectBoxParentOptionController;
using tdme::gui::events::GUIKeyboardEvent;
using tdme::gui::events::GUIMouseEvent;
using tdme::gui::nodes::GUIElementController;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeConditions;
using tdme::gui::nodes::GUIParentNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::GUI;
using tdme::utilities::MutableString;

constexpr char GUISelectBoxController::VALUE_DELIMITER;

string GUISelectBoxController::CONDITION_DISABLED = "disabled";
string GUISelectBoxController::CONDITION_ENABLED = "enabled";

GUISelectBoxController::GUISelectBoxController(GUINode* node)
	: GUIElementController(node)
{
	this->disabled = required_dynamic_cast<GUIElementNode*>(node)->isDisabled();
	this->multipleSelection = required_dynamic_cast<GUIElementNode*>(node)->hasOption("multiple");
	this->keyControl = false;
}

bool GUISelectBoxController::isMultipleSelection() {
	return multipleSelection;
}

bool GUISelectBoxController::isKeyControlDown() {
	return keyControl;
}

bool GUISelectBoxController::isDisabled()
{
	return disabled;
}

void GUISelectBoxController::setDisabled(bool disabled)
{
	auto& nodeConditions = required_dynamic_cast<GUIElementNode*>(node)->getActiveConditions();
	nodeConditions.remove(this->disabled == true?CONDITION_DISABLED:CONDITION_ENABLED);
	this->disabled = disabled;
	nodeConditions.add(this->disabled == true ?CONDITION_DISABLED:CONDITION_ENABLED);
	selectCurrent();
}

void GUISelectBoxController::initialize()
{
	if (getFocussedOptionIdx() == -1) {
		auto value = getValue();
		setValue(value);
	}
	GUIElementController::initialize();
}

void GUISelectBoxController::dispose()
{
	GUIElementController::dispose();
}

void GUISelectBoxController::postLayout()
{
}

void GUISelectBoxController::unselect()
{
	required_dynamic_cast<GUIParentNode*>(node)->getChildControllerNodes(childControllerNodes);
	for (auto i = 0; i < childControllerNodes.size(); i++) {
		auto childControllerNode = childControllerNodes[i];
		auto childController = childControllerNode->getController();
		auto selectBoxMultipleOptionController = dynamic_cast<GUISelectBoxOptionController*>(childController);
		if (selectBoxMultipleOptionController != nullptr) selectBoxMultipleOptionController->unselect();
	}
}

void GUISelectBoxController::unfocus()
{
	required_dynamic_cast<GUIParentNode*>(node)->getChildControllerNodes(childControllerNodes);
	for (auto i = 0; i < childControllerNodes.size(); i++) {
		auto childControllerNode = childControllerNodes[i];
		auto childController = childControllerNode->getController();
		auto selectBoxMultipleOptionController = dynamic_cast<GUISelectBoxOptionController*>(childController);
		if (selectBoxMultipleOptionController != nullptr) selectBoxMultipleOptionController->unfocus();
	}
}

void GUISelectBoxController::determineSelectBoxMultipleOptionControllers()
{
	selectBoxMultipleOptionControllers.clear();
	required_dynamic_cast<GUIParentNode*>(node)->getChildControllerNodes(childControllerNodes);
	for (auto i = 0; i < childControllerNodes.size(); i++) {
		auto childControllerNode = childControllerNodes[i];
		auto childController = childControllerNode->getController();
		auto selectBoxMultipleOptionController = dynamic_cast<GUISelectBoxOptionController*>(childController);
		if (selectBoxMultipleOptionController != nullptr && selectBoxMultipleOptionController->isCollapsed() == false) {
			selectBoxMultipleOptionControllers.push_back(selectBoxMultipleOptionController);
		}
	}
}

int GUISelectBoxController::getFocussedOptionIdx()
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

void GUISelectBoxController::selectCurrent()
{
	setValue(getValue());
}

void GUISelectBoxController::focusNext()
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
	selectBoxMultipleOptionControllers[selectBoxMultipleOptionControllerIdx]->getNode()->scrollToNodeX(required_dynamic_cast<GUIParentNode*>(node));
	selectBoxMultipleOptionControllers[selectBoxMultipleOptionControllerIdx]->getNode()->scrollToNodeY(required_dynamic_cast<GUIParentNode*>(node));
}

void GUISelectBoxController::focusPrevious()
{
	determineSelectBoxMultipleOptionControllers();
	auto selectBoxMultipleOptionControllerIdx = getFocussedOptionIdx();
	unfocus();
	if (selectBoxMultipleOptionControllers.size() == 0)
		return;

	selectBoxMultipleOptionControllerIdx = (selectBoxMultipleOptionControllerIdx - 1) % static_cast<int>(selectBoxMultipleOptionControllers.size());
	if (selectBoxMultipleOptionControllerIdx < 0)
		selectBoxMultipleOptionControllerIdx += selectBoxMultipleOptionControllers.size();

	selectBoxMultipleOptionControllers[selectBoxMultipleOptionControllerIdx]->focus();
	selectBoxMultipleOptionControllers[selectBoxMultipleOptionControllerIdx]->getNode()->scrollToNodeX(required_dynamic_cast<GUIParentNode*>(node));
	selectBoxMultipleOptionControllers[selectBoxMultipleOptionControllerIdx]->getNode()->scrollToNodeY(required_dynamic_cast<GUIParentNode*>(node));
}

void GUISelectBoxController::toggle()
{
	determineSelectBoxMultipleOptionControllers();
	auto selectBoxMultipleOptionControllerIdx = getFocussedOptionIdx();
	selectBoxMultipleOptionControllers[selectBoxMultipleOptionControllerIdx]->toggle();
	selectBoxMultipleOptionControllers[selectBoxMultipleOptionControllerIdx]->getNode()->scrollToNodeX(required_dynamic_cast<GUIParentNode*>(node));
	selectBoxMultipleOptionControllers[selectBoxMultipleOptionControllerIdx]->getNode()->scrollToNodeY(required_dynamic_cast<GUIParentNode*>(node));
}

void GUISelectBoxController::select()
{
	determineSelectBoxMultipleOptionControllers();
	auto selectBoxMultipleOptionControllerIdx = getFocussedOptionIdx();
	selectBoxMultipleOptionControllers[selectBoxMultipleOptionControllerIdx]->select();
	selectBoxMultipleOptionControllers[selectBoxMultipleOptionControllerIdx]->getNode()->scrollToNodeX(required_dynamic_cast<GUIParentNode*>(node));
	selectBoxMultipleOptionControllers[selectBoxMultipleOptionControllerIdx]->getNode()->scrollToNodeY(required_dynamic_cast<GUIParentNode*>(node));
}

void GUISelectBoxController::toggleOpenState() {
	determineSelectBoxMultipleOptionControllers();
	auto selectBoxMultipleOptionControllerIdx = getFocussedOptionIdx();
	if (selectBoxMultipleOptionControllers.size() == 0)
		return;

	if (selectBoxMultipleOptionControllerIdx == -1) return;

	auto selectBoxMultipleParentOptionController = dynamic_cast<GUISelectBoxParentOptionController*>(selectBoxMultipleOptionControllers[selectBoxMultipleOptionControllerIdx]);
	if (selectBoxMultipleParentOptionController != nullptr) selectBoxMultipleParentOptionController->toggleOpenState();
}
void GUISelectBoxController::handleMouseEvent(GUINode* node, GUIMouseEvent* event)
{
	GUIElementController::handleMouseEvent(node, event);
	auto disabled = required_dynamic_cast<GUISelectBoxController*>(this->node->getController())->isDisabled();
	if (disabled == false && node == this->node && node->isEventBelongingToNode(event) && event->getButton() == MOUSE_BUTTON_LEFT) {
		event->setProcessed(true);
		if (event->getType() == GUIMouseEvent::MOUSEEVENT_PRESSED) {
			node->getScreenNode()->getGUI()->setFoccussedNode(required_dynamic_cast<GUIElementNode*>(node));
		}
	}
}

void GUISelectBoxController::handleKeyboardEvent(GUIKeyboardEvent* event)
{
	GUIElementController::handleKeyboardEvent(event);
	if (disabled == false) {
		if (event->getType() != GUIKeyboardEvent::KEYBOARDEVENT_KEY_TYPED) {
			auto isKeyDown = event->getType() == GUIKeyboardEvent::KEYBOARDEVENT_KEY_PRESSED;
			#if defined(GLFW3) || defined(VUKAN)
				if (event->getKeyCode() == KEYBOARD_KEYCODE_LEFT_CTRL) keyControl = isKeyDown;
			#else
				keyControl = event->isControlDown();
			#endif
		}

		switch (event->getKeyCode()) {
		case GUIKeyboardEvent::KEYCODE_UP: {
				event->setProcessed(true);
				if (event->getType() == GUIKeyboardEvent::KEYBOARDEVENT_KEY_PRESSED) {
					focusPrevious();
					if (multipleSelection == false) {
						auto selectBoxMultipleOptionControllerIdx = getFocussedOptionIdx();
						unselect();
						selectBoxMultipleOptionControllers[selectBoxMultipleOptionControllerIdx]->select();
					}
				}
			}
			break;
		case GUIKeyboardEvent::KEYCODE_DOWN: {
				event->setProcessed(true);
				if (event->getType() == GUIKeyboardEvent::KEYBOARDEVENT_KEY_PRESSED) {
					focusNext();
					if (multipleSelection == false) {
						auto selectBoxMultipleOptionControllerIdx = getFocussedOptionIdx();
						unselect();
						selectBoxMultipleOptionControllers[selectBoxMultipleOptionControllerIdx]->select();
					}
				}
			}
			break;
		case GUIKeyboardEvent::KEYCODE_RIGHT:
		case GUIKeyboardEvent::KEYCODE_LEFT: {
				event->setProcessed(true);
				if (event->getType() == GUIKeyboardEvent::KEYBOARDEVENT_KEY_PRESSED) toggleOpenState();
			}
			break;
		case GUIKeyboardEvent::KEYCODE_SPACE: {
				event->setProcessed(true);
				if (event->getType() == GUIKeyboardEvent::KEYBOARDEVENT_KEY_PRESSED) {
					if (multipleSelection == true) {
						toggle();
					}
				}
			}
			break;
		}
	}
}

void GUISelectBoxController::tick()
{
	GUIElementController::tick();
}

void GUISelectBoxController::onFocusGained()
{
}

void GUISelectBoxController::onFocusLost()
{
	keyControl = false;
}

bool GUISelectBoxController::hasValue()
{
	return true;
}

const MutableString& GUISelectBoxController::getValue()
{
	value.reset();
	determineSelectBoxMultipleOptionControllers();
	for (auto i = 0; i < selectBoxMultipleOptionControllers.size(); i++) {
		auto selectBoxOptionController = selectBoxMultipleOptionControllers[i];
		if (selectBoxOptionController->isSelected() == true) {
			value.append(required_dynamic_cast<GUIElementNode*>(selectBoxOptionController->getNode())->getValue());
			if (multipleSelection == true) value.append(VALUE_DELIMITER);
		}
	}
	if (value.length() > 0) {
		if (multipleSelection == true) value.insert(0, VALUE_DELIMITER);
	}
	return value;
}

void GUISelectBoxController::setValue(const MutableString& value)
{
	determineSelectBoxMultipleOptionControllers();
	unselect();
	unfocus();
	MutableString searchValue;
	GUIElementNode* selectBoxOptionNodeLast = nullptr;
	for (auto i = 0; i < selectBoxMultipleOptionControllers.size(); i++) {
		auto selectBoxOptionController = selectBoxMultipleOptionControllers[i];
		auto selectBoxOptionNode = required_dynamic_cast<GUIElementNode*>(selectBoxOptionController->getNode());
		searchValue.reset();
		if (multipleSelection == true) searchValue.append(VALUE_DELIMITER);
		searchValue.append(selectBoxOptionNode->getValue());
		if (multipleSelection == true) searchValue.append(VALUE_DELIMITER);
		if (value.indexOf(searchValue) != -1) {
			selectBoxOptionController->select();
			selectBoxOptionNode->scrollToNodeX(required_dynamic_cast<GUIParentNode*>(node));
			selectBoxOptionNode->scrollToNodeY(required_dynamic_cast<GUIParentNode*>(node));
			selectBoxOptionNodeLast = selectBoxOptionNode;
		}
	}
	if (selectBoxOptionNodeLast != nullptr) {
		required_dynamic_cast<GUISelectBoxOptionController*>(selectBoxOptionNodeLast->getController())->focus();
	}
}
