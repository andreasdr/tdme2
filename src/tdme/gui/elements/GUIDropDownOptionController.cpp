#include <tdme/gui/elements/GUIDropDownOptionController.h>

#include <string>

#include <tdme/gui/GUI.h>
#include <tdme/gui/elements/GUIDropDownController.h>
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

using tdme::gui::elements::GUIDropDownOptionController;

using tdme::gui::GUI;
using tdme::gui::elements::GUIDropDownController;
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

string GUIDropDownOptionController::CONDITION_SELECTED = "selected";
string GUIDropDownOptionController::CONDITION_UNSELECTED = "unselected";
string GUIDropDownOptionController::CONDITION_HIDDEN = "hidden";

GUIDropDownOptionController::GUIDropDownOptionController(GUINode* node)
	: GUIElementController(node)
{
	this->initialPostLayout = true;
	this->selected = required_dynamic_cast<GUIElementNode*>(node)->isSelected();
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
	auto& nodeConditions = required_dynamic_cast<GUIElementNode*>(node)->getActiveConditions();
	nodeConditions.remove(this->selected == true?CONDITION_SELECTED:CONDITION_UNSELECTED);
	this->selected = true;
	nodeConditions.add(this->selected == true?CONDITION_SELECTED:CONDITION_UNSELECTED);
	auto dropDownOptionTextNode = required_dynamic_cast<GUITextNode*>(node->getScreenNode()->getNodeById(node->getId() + "_unselected"));
	auto dropDownTextNodeEnabled = required_dynamic_cast<GUITextNode*>(node->getScreenNode()->getNodeById(dropDownNode->getId() + "_text_enabled"));
	dropDownTextNodeEnabled->setText(dropDownOptionTextNode->getText());
	auto dropDownTextNodeDisabled = required_dynamic_cast<GUITextNode*>(node->getScreenNode()->getNodeById(dropDownNode->getId() + "_text_disabled"));
	dropDownTextNodeDisabled->setText(dropDownOptionTextNode->getText());
}

void GUIDropDownOptionController::unselect()
{
	auto& nodeConditions = required_dynamic_cast<GUIElementNode*>(node)->getActiveConditions();
	nodeConditions.remove(this->selected == true?CONDITION_SELECTED:CONDITION_UNSELECTED);
	this->selected = false;
	nodeConditions.add(this->selected == true?CONDITION_SELECTED:CONDITION_UNSELECTED);
}

bool GUIDropDownOptionController::search(const string& value) {
	auto& nodeConditions = required_dynamic_cast<GUIElementNode*>(node)->getActiveConditions();
	if (value.empty() == true) {
		nodeConditions.remove(CONDITION_HIDDEN);
		return true;
	}
	auto dropDownOptionTextNode = required_dynamic_cast<GUITextNode*>(node->getScreenNode()->getNodeById(node->getId() + "_unselected"));
	auto nodeTextLowerCase = StringTools::toLowerCase(dropDownOptionTextNode->getText().getString());
	if (nodeTextLowerCase.find(value) == string::npos) {
		nodeConditions.add(CONDITION_HIDDEN);
		return false;
	} else {
		nodeConditions.remove(CONDITION_HIDDEN);
		return true;
	}
}

void GUIDropDownOptionController::initialize()
{
	//
	dropDownNode = node->getParentControllerNode();
	while (true == true) {
		if (dynamic_cast<GUIDropDownController*>(dropDownNode->getController()) != nullptr) {
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
		if (event->getType() == GUIMouseEvent::MOUSEEVENT_RELEASED) {
			required_dynamic_cast<GUIDropDownController*>(dropDownNode->getController())->unselect();
			select();
			required_dynamic_cast<GUIDropDownController*>(dropDownNode->getController())->toggleOpenState();
			node->scrollToNodeX(dropDownNode);
			node->scrollToNodeY(dropDownNode);
			node->getScreenNode()->delegateValueChanged(required_dynamic_cast<GUIElementNode*>(dropDownNode));
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
