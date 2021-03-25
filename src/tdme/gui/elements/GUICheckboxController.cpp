#include <tdme/gui/elements/GUICheckboxController.h>

#include <tdme/gui/events/GUIKeyboardEvent.h>
#include <tdme/gui/events/GUIMouseEvent.h>
#include <tdme/gui/nodes/GUIElementController.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUINodeConditions.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/gui/GUI.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/MutableString.h>

using tdme::gui::elements::GUICheckboxController;
using tdme::gui::events::GUIKeyboardEvent;
using tdme::gui::events::GUIMouseEvent;
using tdme::gui::nodes::GUIElementController;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeConditions;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::GUI;
using tdme::utilities::Console;
using tdme::utilities::MutableString;

string GUICheckboxController::CONDITION_CHECKED = "checked";
string GUICheckboxController::CONDITION_UNCHECKED = "unchecked";
string GUICheckboxController::CONDITION_DISABLED = "disabled";
string GUICheckboxController::CONDITION_ENABLED = "enabled";

GUICheckboxController::GUICheckboxController(GUINode* node)
	: GUIElementController(node)
{
	this->checked = required_dynamic_cast<GUIElementNode*>(node)->isSelected();
	this->disabled = required_dynamic_cast<GUIElementNode*>(node)->isDisabled();
}

bool GUICheckboxController::isChecked()
{
	return checked;
}

void GUICheckboxController::setChecked(bool checked)
{
	auto& nodeConditions = required_dynamic_cast<GUIElementNode*>(node)->getActiveConditions();
	nodeConditions.remove(this->checked == true?CONDITION_CHECKED:CONDITION_UNCHECKED);
	this->checked = checked;
	nodeConditions.add(this->checked == true?CONDITION_CHECKED:CONDITION_UNCHECKED);
}

bool GUICheckboxController::isDisabled()
{
	return disabled;
}

void GUICheckboxController::setDisabled(bool disabled)
{
	auto& nodeConditions = required_dynamic_cast<GUIElementNode*>(node)->getActiveConditions();
	nodeConditions.remove(this->disabled == true?CONDITION_DISABLED:CONDITION_ENABLED);
	this->disabled = disabled;
	nodeConditions.add(this->disabled == true?CONDITION_DISABLED:CONDITION_ENABLED);
}

void GUICheckboxController::initialize()
{
	//
	setChecked(checked);

	//
	GUIElementController::initialize();
}

void GUICheckboxController::dispose()
{
	GUIElementController::dispose();
}

void GUICheckboxController::postLayout()
{
}

void GUICheckboxController::handleMouseEvent(GUINode* node, GUIMouseEvent* event)
{
	GUIElementController::handleMouseEvent(node, event);
	if (disabled == false && node == this->node && node->isEventBelongingToNode(event) && event->getButton() == MOUSE_BUTTON_LEFT) {
		event->setProcessed(true);
		if (event->getType() == GUIMouseEvent::MOUSEEVENT_RELEASED) {
			setChecked(checked == true ? false : true);
			node->getScreenNode()->getGUI()->setFoccussedNode(required_dynamic_cast<GUIElementNode*>(node));
			node->getScreenNode()->delegateValueChanged(required_dynamic_cast<GUIElementNode*>(node));
		}
	}
}

void GUICheckboxController::handleKeyboardEvent(GUIKeyboardEvent* event)
{
	GUIElementController::handleKeyboardEvent(event);
	if (disabled == false) {
		switch (event->getKeyCode()) {
		case GUIKeyboardEvent::KEYCODE_SPACE: {
				event->setProcessed(true);
				if (event->getType() == GUIKeyboardEvent::KEYBOARDEVENT_KEY_PRESSED) {
					setChecked(checked == true ? false : true);
					node->getScreenNode()->delegateValueChanged(required_dynamic_cast<GUIElementNode*>(node));
				}
			}
			break;
		default: {
				break;
			}
		}

	}
}

void GUICheckboxController::tick()
{
	GUIElementController::tick();
}

void GUICheckboxController::onFocusGained()
{
}

void GUICheckboxController::onFocusLost()
{
}

bool GUICheckboxController::hasValue()
{
	return true;
}

const MutableString& GUICheckboxController::getValue()
{
	value.reset();
	if (checked == true) {
		value.append(required_dynamic_cast<GUIElementNode*>(node)->getValue());
	}
	return value;
}

void GUICheckboxController::setValue(const MutableString& value)
{
	setChecked(value.equals(required_dynamic_cast<GUIElementNode*>(node)->getValue()));
}
