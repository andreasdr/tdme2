#include <tdme/gui/elements/GUIMenuHeaderItemController.h>

#include <tdme/gui/GUI.h>
#include <tdme/gui/elements/GUIMenuHeaderItemController.h>
#include <tdme/gui/elements/GUIMenuItemController.h>
#include <tdme/gui/events/GUIKeyboardEvent.h>
#include <tdme/gui/events/GUIMouseEvent.h>
#include <tdme/gui/nodes/GUIElementController.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUINodeConditions.h>
#include <tdme/gui/nodes/GUIParentNode.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/gui/nodes/GUITextNode.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/MutableString.h>
#include <tdme/utilities/StringTools.h>

using tdme::gui::elements::GUIMenuHeaderItemController;
using tdme::gui::GUI;
using tdme::gui::elements::GUIMenuItemController;
using tdme::gui::events::GUIKeyboardEvent;
using tdme::gui::events::GUIMouseEvent;
using tdme::gui::nodes::GUIElementController;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeConditions;
using tdme::gui::nodes::GUIParentNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::nodes::GUITextNode;
using tdme::utilities::Console;
using tdme::utilities::MutableString;
using tdme::utilities::StringTools;

string GUIMenuHeaderItemController::CONDITION_OPENED = "opened";
string GUIMenuHeaderItemController::CONDITION_CLOSED = "closed";
string GUIMenuHeaderItemController::CONDITION_SELECTED = "selected";
string GUIMenuHeaderItemController::CONDITION_UNSELECTED = "unselected";


GUIMenuHeaderItemController::GUIMenuHeaderItemController(GUINode* node)
	: GUIElementController(node)
{
	init();
	this->selected = (dynamic_cast< GUIElementNode* >(node))->isSelected();
}

void GUIMenuHeaderItemController::init()
{
	open = false;
}

bool GUIMenuHeaderItemController::isSelected()
{
	return selected;
}

void GUIMenuHeaderItemController::select()
{
	auto& nodeConditions = (dynamic_cast<GUIElementNode*>(node))->getActiveConditions();
	nodeConditions.remove(this->selected == true?CONDITION_SELECTED:CONDITION_UNSELECTED);
	this->selected = true;
	nodeConditions.add(this->selected == true?CONDITION_SELECTED:CONDITION_UNSELECTED);
}

void GUIMenuHeaderItemController::unselect()
{
	auto& nodeConditions = (dynamic_cast<GUIElementNode*>(node))->getActiveConditions();
	nodeConditions.remove(this->selected == true?CONDITION_SELECTED:CONDITION_UNSELECTED);
	this->selected = false;
	nodeConditions.add(this->selected == true?CONDITION_SELECTED:CONDITION_UNSELECTED);
}

void GUIMenuHeaderItemController::initialize()
{
	(dynamic_cast< GUIElementNode* >(node))->getActiveConditions().add(open == true?CONDITION_OPENED:CONDITION_CLOSED);

	//
	GUIElementController::initialize();

	//
	if (this->selected == true) select(); else unselect();
}

void GUIMenuHeaderItemController::dispose()
{
	GUIElementController::dispose();
}

void GUIMenuHeaderItemController::postLayout()
{
}

bool GUIMenuHeaderItemController::isOpen()
{
	return open;
}

void GUIMenuHeaderItemController::toggleOpenState()
{
	(dynamic_cast<GUIElementNode*>(node))->getActiveConditions().remove(open == true?CONDITION_OPENED:CONDITION_CLOSED);
	open = open == true ? false : true;
	(dynamic_cast<GUIElementNode*>(node))->getActiveConditions().add(open == true?CONDITION_OPENED:CONDITION_CLOSED);
	if (open == true) select(); else unselect();
}

void GUIMenuHeaderItemController::determineMenuItemControllers()
{
	menuItemControllers.clear();
	(dynamic_cast< GUIParentNode* >(node))->getChildControllerNodes(childControllerNodes);
	for (auto i = 0; i < childControllerNodes.size(); i++) {
		auto childControllerNode = childControllerNodes[i];
		auto childController = childControllerNode->getController();
		auto menuItemController = dynamic_cast<GUIMenuItemController*>(childController);
		if (menuItemController != nullptr && menuItemController->isDisabled() == false) {
			menuItemControllers.push_back(menuItemController);
		}
	}
}

int32_t GUIMenuHeaderItemController::getSelectedMenuItemControllerIdx()
{
	auto selectedMenuItemControllerIdx = 0;
	for (auto i = 0; i < menuItemControllers.size(); i++) {
		auto selectBoxOptionController = menuItemControllers[i];
		if (selectBoxOptionController->isSelected() == true) {
			selectedMenuItemControllerIdx = i;
			break;
		}
	}
	return selectedMenuItemControllerIdx;
}

void GUIMenuHeaderItemController::selectFirst()
{
	determineMenuItemControllers();
	auto selectedMenuItemControllerIdx = getSelectedMenuItemControllerIdx();
	if (menuItemControllers.size() == 0) return;

	for (auto menuItemController: menuItemControllers) menuItemController->unselect();
	menuItemControllers[0]->select();
}

void GUIMenuHeaderItemController::selectNext()
{
	determineMenuItemControllers();
	auto selectedMenuItemControllerIdx = getSelectedMenuItemControllerIdx();
	if (menuItemControllers.size() == 0) return;

	selectedMenuItemControllerIdx = (selectedMenuItemControllerIdx + 1) % (int)menuItemControllers.size();
	if (selectedMenuItemControllerIdx < 0)
		selectedMenuItemControllerIdx += menuItemControllers.size();

	for (auto menuItemController: menuItemControllers) menuItemController->unselect();
	menuItemControllers[selectedMenuItemControllerIdx]->select();
}

void GUIMenuHeaderItemController::selectPrevious()
{
	determineMenuItemControllers();
	auto selectedMenuItemControllerIdx = getSelectedMenuItemControllerIdx();
	if (menuItemControllers.size() == 0) return;

	selectedMenuItemControllerIdx = (selectedMenuItemControllerIdx - 1) % (int)menuItemControllers.size();
	if (selectedMenuItemControllerIdx < 0)
		selectedMenuItemControllerIdx += menuItemControllers.size();

	for (auto menuItemController: menuItemControllers) menuItemController->unselect();
	menuItemControllers[selectedMenuItemControllerIdx]->select();
}

void GUIMenuHeaderItemController::handleMouseEvent(GUINode* node, GUIMouseEvent* event)
{
	GUIElementController::handleMouseEvent(node, event);
	if (isDisabled() == true) return;
	auto elementNode  = dynamic_cast<GUIElementNode*>(this->node);
	if (event->getButton() == MOUSE_BUTTON_LEFT) {
		if (node == this->node && node->isEventBelongingToNode(event) == true) {
			auto& nodeConditions = (dynamic_cast<GUIElementNode*>(node))->getActiveConditions();
			if (event->getType() == GUIMouseEvent::MOUSEEVENT_RELEASED) {
				event->setProcessed(true);
				toggleOpenState();
			}
		} else {
			if (open == true) {
				auto innerNode = this->node->getScreenNode()->getNodeById(this->node->getId() + "_inner");
				if (node == this->node && innerNode->isEventBelongingToNode(event) == false) {
					event->setProcessed(true);
					toggleOpenState();
				}
			}
		}
	}
}

void GUIMenuHeaderItemController::handleKeyboardEvent(GUINode* node, GUIKeyboardEvent* event)
{
	GUIElementController::handleKeyboardEvent(node, event);
}

void GUIMenuHeaderItemController::tick()
{
	GUIElementController::tick();
	if (open == true) node->getScreenNode()->getGUI()->addMouseOutClickCandidateElementNode(dynamic_cast<GUIElementNode*>(this->node));
}

void GUIMenuHeaderItemController::onFocusGained()
{
}

void GUIMenuHeaderItemController::onFocusLost()
{
}

bool GUIMenuHeaderItemController::hasValue()
{
	return true;
}

const MutableString& GUIMenuHeaderItemController::getValue()
{
	value.reset();
	return value;
}

void GUIMenuHeaderItemController::setValue(const MutableString& value)
{
}

