#include <tdme/gui/elements/GUIMenuItemController.h>

#include <string>

#include <tdme/gui/elements/GUIDropDownController.h>
#include <tdme/gui/elements/GUIMenuHeaderItemController.h>
#include <tdme/gui/events/GUIKeyboardEvent.h>
#include <tdme/gui/events/GUIMouseEvent.h>
#include <tdme/gui/nodes/GUIElementController.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUINodeConditions.h>
#include <tdme/gui/nodes/GUIParentNode.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/gui/nodes/GUITextNode.h>
#include <tdme/utilities/MutableString.h>
#include <tdme/utilities/StringTools.h>

using std::string;

using tdme::gui::elements::GUIMenuHeaderItemController;
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
using tdme::utilities::MutableString;
using tdme::utilities::StringTools;

string GUIMenuItemController::CONDITION_SELECTED = "selected";
string GUIMenuItemController::CONDITION_UNSELECTED = "unselected";

GUIMenuItemController::GUIMenuItemController(GUINode* node)
	: GUIElementController(node)
{
	this->selected = (dynamic_cast<GUIElementNode*>(node))->isSelected();
}

bool GUIMenuItemController::isSelected()
{
	return selected;
}

void GUIMenuItemController::select()
{
	auto& nodeConditions = (dynamic_cast< GUIElementNode* >(node))->getActiveConditions();
	nodeConditions.remove(this->selected == true?CONDITION_SELECTED:CONDITION_UNSELECTED);
	this->selected = true;
	nodeConditions.add(this->selected == true?CONDITION_SELECTED:CONDITION_UNSELECTED);
}

void GUIMenuItemController::unselect()
{
	auto& nodeConditions = (dynamic_cast< GUIElementNode* >(node))->getActiveConditions();
	nodeConditions.remove(this->selected == true?CONDITION_SELECTED:CONDITION_UNSELECTED);
	this->selected = false;
	nodeConditions.add(this->selected == true?CONDITION_SELECTED:CONDITION_UNSELECTED);
}

void GUIMenuItemController::initialize()
{
	//
	menuHeaderItemNode = node->getParentControllerNode();
	while (true == true) {
		if (dynamic_cast<GUIMenuHeaderItemController*>(menuHeaderItemNode->getController()) != nullptr) {
			break;
		}
		menuHeaderItemNode = menuHeaderItemNode->getParentControllerNode();
	}
	if (selected == true) {
		select();
	} else {
		unselect();
	}
	setDisabled(isDisabled());

	//
	GUIElementController::initialize();
}

void GUIMenuItemController::postLayout()
{
}

void GUIMenuItemController::dispose()
{
	GUIElementController::dispose();
}

void GUIMenuItemController::handleMouseEvent(GUINode* node, GUIMouseEvent* event)
{
	GUIElementController::handleMouseEvent(node, event);
	if (node == this->node &&
		node->isEventBelongingToNode(event) &&
		event->getType() == GUIMouseEvent::MOUSEEVENT_RELEASED &&
		event->getButton() == MOUSE_BUTTON_LEFT) {
		dynamic_cast<GUIMenuHeaderItemController*>(menuHeaderItemNode->getController())->toggleOpenState();
		event->setProcessed(true);
	}
}

void GUIMenuItemController::handleKeyboardEvent(GUINode* node, GUIKeyboardEvent* event)
{
	GUIElementController::handleKeyboardEvent(node, event);
	if (isDisabled() == false && node == this->node && event->getType() == GUIKeyboardEvent::KEYBOARDEVENT_KEY_PRESSED) {
		switch (event->getKeyCode()) {
			case GUIKeyboardEvent::KEYCODE_SPACE: {
				dynamic_cast<GUIMenuHeaderItemController*>(menuHeaderItemNode->getController())->toggleOpenState();
				event->setProcessed(true);
			}
		}
	}
}

void GUIMenuItemController::tick()
{
	GUIElementController::tick();
}

void GUIMenuItemController::onFocusGained()
{
}

void GUIMenuItemController::onFocusLost()
{
}

bool GUIMenuItemController::hasValue()
{
	return false;
}

const MutableString& GUIMenuItemController::getValue()
{
	return value;
}

void GUIMenuItemController::setValue(const MutableString& value)
{
}
