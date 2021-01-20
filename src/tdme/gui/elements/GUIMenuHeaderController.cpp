#include <tdme/gui/elements/GUIMenuHeaderController.h>

#include <tdme/gui/GUI.h>
#include <tdme/gui/elements/GUIMenuHeaderItemController.h>
#include <tdme/gui/elements/GUITabController.h>
#include <tdme/gui/elements/GUITabsController.h>
#include <tdme/gui/events/GUIKeyboardEvent.h>
#include <tdme/gui/events/GUIMouseEvent.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUINodeController.h>
#include <tdme/gui/nodes/GUIParentNode.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/utilities/Console.h>

using tdme::gui::elements::GUIMenuHeaderController;
using tdme::gui::GUI;
using tdme::gui::elements::GUIMenuHeaderItemController;
using tdme::gui::events::GUIKeyboardEvent;
using tdme::gui::events::GUIMouseEvent;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeController;
using tdme::gui::nodes::GUIParentNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::utilities::Console;

GUIMenuHeaderController::GUIMenuHeaderController(GUINode* node)
	: GUINodeController(node)
{
	this->focus = false;
}

bool GUIMenuHeaderController::isDisabled()
{
	return false;
}

void GUIMenuHeaderController::setDisabled(bool disabled)
{
}

void GUIMenuHeaderController::initialize()
{
}

void GUIMenuHeaderController::dispose()
{
}

void GUIMenuHeaderController::postLayout()
{
}

bool GUIMenuHeaderController::hasFocus()
{
	return focus;
}

void GUIMenuHeaderController::determineMenuHeaderItemControllers()
{
	menuHeaderItemControllers.clear();
	required_dynamic_cast<GUIParentNode*>(node)->getChildControllerNodes(childControllerNodes);
	for (auto i = 0; i < childControllerNodes.size(); i++) {
		auto childControllerNode = childControllerNodes[i];
		auto childController = childControllerNode->getController();
		if (dynamic_cast<GUIMenuHeaderItemController*>(childController) != nullptr) {
			auto menuHeaderItemController = required_dynamic_cast< GUIMenuHeaderItemController* >(childController);
			if (menuHeaderItemController->isDisabled() == true)
				continue;
			menuHeaderItemControllers.push_back(menuHeaderItemController);
		}
	}
}

int GUIMenuHeaderController::getSelectedHeaderItemIdx()
{
	auto menuHeaderItemControllerIdx = -1;
	for (auto i = 0; i < menuHeaderItemControllers.size(); i++) {
		auto menuHeaderItemController = menuHeaderItemControllers[i];
		if (menuHeaderItemController->isSelected() == true) {
			menuHeaderItemControllerIdx = i;
			break;
		}
	}
	return menuHeaderItemControllerIdx;
}

bool GUIMenuHeaderController::isOpen() {
	determineMenuHeaderItemControllers();
	for (auto menuHeaderItemController: menuHeaderItemControllers) {
		if (menuHeaderItemController->isOpen() == true) return true;
	}
	return false;
}

void GUIMenuHeaderController::unselect() {
	determineMenuHeaderItemControllers();
	for (auto menuHeaderItemController: menuHeaderItemControllers) {
		menuHeaderItemController->unselect();
		if (menuHeaderItemController->isOpen() == true) menuHeaderItemController->toggleOpenState();
	}
}

void GUIMenuHeaderController::selectNext()
{
	determineMenuHeaderItemControllers();
	auto menuHeaderItemControllerIdx = getSelectedHeaderItemIdx();
	menuHeaderItemControllerIdx = (menuHeaderItemControllerIdx + 1) % (int)menuHeaderItemControllers.size();
	if (menuHeaderItemControllerIdx < 0)
		menuHeaderItemControllerIdx += menuHeaderItemControllers.size();

	for (auto menuHeaderItemController: menuHeaderItemControllers) {
		menuHeaderItemController->unselect();
		if (menuHeaderItemController->isOpen() == true) menuHeaderItemController->toggleOpenState();
	}
	menuHeaderItemControllers[menuHeaderItemControllerIdx]->select();
	if (menuHeaderItemControllers[menuHeaderItemControllerIdx]->isOpen() == false) menuHeaderItemControllers[menuHeaderItemControllerIdx]->toggleOpenState();
	menuHeaderItemControllers[menuHeaderItemControllerIdx]->selectFirst();
}

void GUIMenuHeaderController::selectPrevious()
{
	determineMenuHeaderItemControllers();
	auto menuHeaderItemControllerIdx = getSelectedHeaderItemIdx();
	menuHeaderItemControllerIdx = (menuHeaderItemControllerIdx - 1) % (int)menuHeaderItemControllers.size();
	if (menuHeaderItemControllerIdx < 0)
		menuHeaderItemControllerIdx += menuHeaderItemControllers.size();

	for (auto menuHeaderItemController: menuHeaderItemControllers) {
		menuHeaderItemController->unselect();
		if (menuHeaderItemController->isOpen() == true) menuHeaderItemController->toggleOpenState();
	}
	menuHeaderItemControllers[menuHeaderItemControllerIdx]->select();
	if (menuHeaderItemControllers[menuHeaderItemControllerIdx]->isOpen() == false) menuHeaderItemControllers[menuHeaderItemControllerIdx]->toggleOpenState();
	menuHeaderItemControllers[menuHeaderItemControllerIdx]->selectFirst();
}

void GUIMenuHeaderController::handleMouseEvent(GUINode* node, GUIMouseEvent* event)
{
	if (node == this->node && node->isEventBelongingToNode(event) && event->getButton() == MOUSE_BUTTON_LEFT) {
		event->setProcessed(true);
		if (event->getType() == GUIMouseEvent::MOUSEEVENT_RELEASED) {
			node->getScreenNode()->getGUI()->setFoccussedNode(required_dynamic_cast<GUIElementNode*>(node));
		}
	}
}

void GUIMenuHeaderController::handleKeyboardEvent(GUINode* node, GUIKeyboardEvent* event)
{
	if (node == this->node) {
		switch (event->getKeyCode()) {
			case GUIKeyboardEvent::KEYCODE_ESCAPE: {
				determineMenuHeaderItemControllers();
				auto menuHeaderItemControllerIdx = getSelectedHeaderItemIdx();
				if (menuHeaderItemControllerIdx == -1) return;
				if (menuHeaderItemControllers[menuHeaderItemControllerIdx]->isOpen() == true) menuHeaderItemControllers[menuHeaderItemControllerIdx]->toggleOpenState();
				menuHeaderItemControllers[menuHeaderItemControllerIdx]->unselect();
			}
			break;
		case GUIKeyboardEvent::KEYCODE_LEFT: {
				event->setProcessed(true);
				if (event->getType() == GUIKeyboardEvent::KEYBOARDEVENT_KEY_PRESSED) {
					selectPrevious();
				}
			}
			break;
		case GUIKeyboardEvent::KEYCODE_RIGHT: {
				event->setProcessed(true);
				if (event->getType() == GUIKeyboardEvent::KEYBOARDEVENT_KEY_PRESSED) {
					selectNext();
				}
			}
			break;
		case GUIKeyboardEvent::KEYCODE_UP: {
				event->setProcessed(true);
				if (event->getType() == GUIKeyboardEvent::KEYBOARDEVENT_KEY_PRESSED) {
					determineMenuHeaderItemControllers();
					if (menuHeaderItemControllers.size() == 0) return;
					auto menuHeaderItemControllerIdx = getSelectedHeaderItemIdx();
					if (menuHeaderItemControllerIdx == -1) menuHeaderItemControllerIdx = 0;
					if (menuHeaderItemControllers[menuHeaderItemControllerIdx]->isOpen() == false) {
						menuHeaderItemControllers[menuHeaderItemControllerIdx]->toggleOpenState();
						menuHeaderItemControllers[menuHeaderItemControllerIdx]->selectFirst();
					} else {
						menuHeaderItemControllers[menuHeaderItemControllerIdx]->selectPrevious();
					}
				}
			}
			break;
		case GUIKeyboardEvent::KEYCODE_DOWN: {
				event->setProcessed(true);
				if (event->getType() == GUIKeyboardEvent::KEYBOARDEVENT_KEY_PRESSED) {
					determineMenuHeaderItemControllers();
					if (menuHeaderItemControllers.size() == 0) return;
					auto menuHeaderItemControllerIdx = getSelectedHeaderItemIdx();
					if (menuHeaderItemControllerIdx == -1) menuHeaderItemControllerIdx = 0;
					if (menuHeaderItemControllers[menuHeaderItemControllerIdx]->isOpen() == false) {
						menuHeaderItemControllers[menuHeaderItemControllerIdx]->toggleOpenState();
						menuHeaderItemControllers[menuHeaderItemControllerIdx]->selectFirst();
					} else {
						menuHeaderItemControllers[menuHeaderItemControllerIdx]->selectNext();
					}
				}
			}
			break;
		case GUIKeyboardEvent::KEYCODE_SPACE: {
				determineMenuHeaderItemControllers();
				if (menuHeaderItemControllers.size() == 0) return;
				auto menuHeaderItemControllerIdx = getSelectedHeaderItemIdx();
				if (menuHeaderItemControllerIdx == -1) menuHeaderItemControllerIdx = 0;
				menuHeaderItemControllers[menuHeaderItemControllerIdx]->handleCurrentMenuItemKeyboardEvent(event);
			}
			break;
		}
	}
}

void GUIMenuHeaderController::tick()
{
}

void GUIMenuHeaderController::onFocusGained()
{
	focus = true;
}

void GUIMenuHeaderController::onFocusLost()
{
	focus = false;
}

bool GUIMenuHeaderController::hasValue()
{
	return false;
}

const MutableString& GUIMenuHeaderController::getValue()
{
	return value;
}

void GUIMenuHeaderController::setValue(const MutableString& value)
{
}

