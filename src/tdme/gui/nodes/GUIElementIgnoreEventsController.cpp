#include <tdme/gui/nodes/GUIElementIgnoreEventsController.h>

#include <tdme/gui/events/GUIMouseEvent.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUINodeConditions.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/gui/GUI.h>

using tdme::gui::events::GUIMouseEvent;
using tdme::gui::nodes::GUIElementIgnoreEventsController;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeConditions;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::GUI;

GUIElementIgnoreEventsController::GUIElementIgnoreEventsController(GUINode* node)
	: GUINodeController(node)
{
	this->disabled = (dynamic_cast< GUIElementNode* >(node))->isDisabled();
}

string GUIElementIgnoreEventsController::CONDITION_DISABLED= "disabled";
string GUIElementIgnoreEventsController::CONDITION_ENABLED = "enabled";

bool GUIElementIgnoreEventsController::isDisabled()
{
	return disabled;
}

void GUIElementIgnoreEventsController::setDisabled(bool disabled)
{
	auto& nodeConditions = (dynamic_cast< GUIElementNode* >(node))->getActiveConditions();
	nodeConditions.remove(this->disabled == true ? CONDITION_DISABLED : CONDITION_ENABLED);
	this->disabled = disabled;
	nodeConditions.add(this->disabled == true ? CONDITION_DISABLED : CONDITION_ENABLED);
}

void GUIElementIgnoreEventsController::initialize()
{
	setDisabled(disabled);
}

void GUIElementIgnoreEventsController::dispose()
{
}

void GUIElementIgnoreEventsController::postLayout()
{
}

void GUIElementIgnoreEventsController::handleMouseEvent(GUINode* node, GUIMouseEvent* event)
{
	if (disabled == false && node == this->node && node->isEventBelongingToNode(event) && event->getButton() == MOUSE_BUTTON_LEFT) {
		if (event->getType() == GUIMouseEvent::MOUSEEVENT_PRESSED) {
			if ((dynamic_cast< GUIElementNode* >(node))->isFocusable() == true) {
				node->getScreenNode()->getGUI()->setFoccussedNode(dynamic_cast< GUIElementNode* >(node));
			}
		}
	}
}

void GUIElementIgnoreEventsController::handleKeyboardEvent(GUINode* node, GUIKeyboardEvent* event)
{
}

void GUIElementIgnoreEventsController::tick()
{
}

void GUIElementIgnoreEventsController::onFocusGained()
{
}

void GUIElementIgnoreEventsController::onFocusLost()
{
}

bool GUIElementIgnoreEventsController::hasValue()
{
	return false;
}

const MutableString& GUIElementIgnoreEventsController::getValue()
{
	return value;
}

void GUIElementIgnoreEventsController::setValue(const MutableString& value)
{
}

