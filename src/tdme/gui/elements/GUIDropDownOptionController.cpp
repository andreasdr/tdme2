#include <tdme/gui/elements/GUIDropDownOptionController.h>

#include <string>

#include <tdme/gui/elements/GUIDropDownController.h>
#include <tdme/gui/events/GUIMouseEvent_Type.h>
#include <tdme/gui/events/GUIMouseEvent.h>
#include <tdme/gui/nodes/GUIElementController.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUINodeConditions.h>
#include <tdme/gui/nodes/GUIParentNode.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/gui/nodes/GUITextNode.h>
#include <tdme/utilities/MutableString.h>

using std::string;

using tdme::gui::elements::GUIDropDownOptionController;
using tdme::gui::elements::GUIDropDownController;
using tdme::gui::events::GUIMouseEvent_Type;
using tdme::gui::events::GUIMouseEvent;
using tdme::gui::nodes::GUIElementController;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeConditions;
using tdme::gui::nodes::GUIParentNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::nodes::GUITextNode;
using tdme::utilities::MutableString;

string GUIDropDownOptionController::CONDITION_SELECTED = "selected";
string GUIDropDownOptionController::CONDITION_UNSELECTED = "unselected";

GUIDropDownOptionController::GUIDropDownOptionController(GUINode* node) 
	: GUIElementController(node)
{
	this->initialPostLayout = true;
	this->selected = (dynamic_cast< GUIElementNode* >(node))->isSelected();
}

bool GUIDropDownOptionController::isSelected()
{
	return selected;
}

bool GUIDropDownOptionController::isDisabled()
{
	return false;
}

void GUIDropDownOptionController::setDisabled(bool disabled)
{
}

void GUIDropDownOptionController::select()
{
	auto& nodeConditions = (dynamic_cast< GUIElementNode* >(node))->getActiveConditions();
	nodeConditions.remove(this->selected == true ? CONDITION_SELECTED : CONDITION_UNSELECTED);
	this->selected = true;
	nodeConditions.add(this->selected == true ? CONDITION_SELECTED : CONDITION_UNSELECTED);
	auto dropDownOptionTextNode = dynamic_cast< GUITextNode* >(node->getScreenNode()->getNodeById(node->getId() + "_unselected"));
	auto dropDownTextNodeEnabled = dynamic_cast< GUITextNode* >(node->getScreenNode()->getNodeById(dropDownNode->getId() + "_text_enabled"));
	dropDownTextNodeEnabled->setText(dropDownOptionTextNode->getText());
	auto dropDownTextNodeDisabled = dynamic_cast< GUITextNode* >(node->getScreenNode()->getNodeById(dropDownNode->getId() + "_text_disabled"));
	dropDownTextNodeDisabled->setText(dropDownOptionTextNode->getText());
}

void GUIDropDownOptionController::unselect()
{
	auto& nodeConditions = (dynamic_cast< GUIElementNode* >(node))->getActiveConditions();
	nodeConditions.remove(this->selected == true ? CONDITION_SELECTED : CONDITION_UNSELECTED);
	this->selected = false;
	nodeConditions.add(this->selected == true ? CONDITION_SELECTED : CONDITION_UNSELECTED);
}

void GUIDropDownOptionController::initialize()
{
	//
	dropDownNode = node->getParentControllerNode();
	while (true == true) {
		if (dynamic_cast< GUIDropDownController* >(dropDownNode->getController()) != nullptr) {
			break;
		}
		dropDownNode = dropDownNode->getParentControllerNode();
	}
	if (selected == true) {
		select();
	} else {
		unselect();
	}

	//
	GUIElementController::initialize();
}

void GUIDropDownOptionController::postLayout()
{
	if (initialPostLayout != true) return;
	if (selected == true) {
		node->scrollToNodeX(dropDownNode);
		node->scrollToNodeY(dropDownNode);
	}
	initialPostLayout = false;
}

void GUIDropDownOptionController::dispose()
{
	GUIElementController::dispose();
}

void GUIDropDownOptionController::handleMouseEvent(GUINode* node, GUIMouseEvent* event)
{
	GUIElementController::handleMouseEvent(node, event);
	if (node == this->node && node->isEventBelongingToNode(event) && event->getButton() == MOUSE_BUTTON_LEFT) {
		event->setProcessed(true);
		if (event->getType() == GUIMouseEvent_Type::MOUSEEVENT_RELEASED) {
			(dynamic_cast< GUIDropDownController* >(dropDownNode->getController()))->unselect();
			select();
			(dynamic_cast< GUIDropDownController* >(dropDownNode->getController()))->toggleOpenState();
			node->scrollToNodeX(dropDownNode);
			node->scrollToNodeY(dropDownNode);
			node->getScreenNode()->delegateValueChanged(dynamic_cast< GUIElementNode* >(dropDownNode));
		}
	}
}

void GUIDropDownOptionController::handleKeyboardEvent(GUINode* node, GUIKeyboardEvent* event)
{
	GUIElementController::handleKeyboardEvent(node, event);
}

void GUIDropDownOptionController::tick()
{
	GUIElementController::tick();
}

void GUIDropDownOptionController::onFocusGained()
{
}

void GUIDropDownOptionController::onFocusLost()
{
}

bool GUIDropDownOptionController::hasValue()
{
	return false;
}

const MutableString& GUIDropDownOptionController::getValue()
{
	return value;
}

void GUIDropDownOptionController::setValue(const MutableString& value)
{
}
