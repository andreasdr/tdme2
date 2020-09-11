#include <tdme/gui/elements/GUIRadioButtonController.h>

#include <tdme/gui/GUI.h>
#include <tdme/gui/events/GUIKeyboardEvent_Type.h>
#include <tdme/gui/events/GUIKeyboardEvent.h>
#include <tdme/gui/events/GUIMouseEvent_Type.h>
#include <tdme/gui/events/GUIMouseEvent.h>
#include <tdme/gui/nodes/GUIElementController.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUINodeConditions.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/utilities/MutableString.h>

using tdme::gui::elements::GUIRadioButtonController;
using tdme::gui::GUI;
using tdme::gui::events::GUIKeyboardEvent_Type;
using tdme::gui::events::GUIKeyboardEvent;
using tdme::gui::events::GUIMouseEvent_Type;
using tdme::gui::events::GUIMouseEvent;
using tdme::gui::nodes::GUIElementController;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeConditions;
using tdme::gui::nodes::GUIScreenNode;
using tdme::utilities::MutableString;

string GUIRadioButtonController::CONDITION_SELECTED = "selected";
string GUIRadioButtonController::CONDITION_UNSELECTED = "unselected";
string GUIRadioButtonController::CONDITION_DISABLED = "disabled";
string GUIRadioButtonController::CONDITION_ENABLED = "enabled";

map<string, vector<GUIElementNode*>>* GUIRadioButtonController::radioButtonGroupNodesByName = new map<string, vector<GUIElementNode*>>();

GUIRadioButtonController::GUIRadioButtonController(GUINode* node) 
	: GUIElementController(node)
{
	init();
	this->selected = (dynamic_cast< GUIElementNode* >(node))->isSelected();
	this->disabled = (dynamic_cast< GUIElementNode* >(node))->isDisabled();
	(*radioButtonGroupNodesByName)[node->getScreenNode()->getId() + "_radiobuttongroup_"
		+ (dynamic_cast< GUIElementNode* >(node))->getName()].push_back(dynamic_cast< GUIElementNode* >(node));
}

void GUIRadioButtonController::init()
{
}

bool GUIRadioButtonController::isSelected()
{
	return selected;
}

void GUIRadioButtonController::select()
{
	auto radioButtonGroupNodesIt = radioButtonGroupNodesByName->find(this->node->getScreenNode()->getId() + "_radiobuttongroup_" +
		(dynamic_cast< GUIElementNode* >(this->node))->getName());
	if (radioButtonGroupNodesIt != radioButtonGroupNodesByName->end()) {
		for (auto i = 0; i < radioButtonGroupNodesIt->second.size(); i++) {
			auto radioButtonNode = dynamic_cast< GUIElementNode* >(radioButtonGroupNodesIt->second[i]);
			auto& nodeConditions = radioButtonNode->getActiveConditions();
			auto nodeController = dynamic_cast< GUIRadioButtonController* >(radioButtonNode->getController());
			nodeConditions.remove(nodeController->selected == true ? CONDITION_SELECTED : CONDITION_UNSELECTED);
			nodeController->selected = false;
			nodeConditions.add(nodeController->selected == true ? CONDITION_SELECTED : CONDITION_UNSELECTED);
		}
	}
	auto& nodeConditions = (dynamic_cast< GUIElementNode* >(node))->getActiveConditions();
	nodeConditions.remove(this->selected == true ? CONDITION_SELECTED : CONDITION_UNSELECTED);
	this->selected = true;
	nodeConditions.add(this->selected == true ? CONDITION_SELECTED : CONDITION_UNSELECTED);
}

bool GUIRadioButtonController::isDisabled()
{
	return disabled;
}

void GUIRadioButtonController::setDisabled(bool disabled)
{
	auto& nodeConditions = (dynamic_cast< GUIElementNode* >(node))->getActiveConditions();
	nodeConditions.remove(this->disabled == true ? CONDITION_DISABLED : CONDITION_ENABLED);
	this->disabled = disabled;
	nodeConditions.add(this->disabled == true ? CONDITION_DISABLED : CONDITION_ENABLED);
}

void GUIRadioButtonController::initialize()
{
	auto& nodeConditions = (dynamic_cast< GUIElementNode* >(node))->getActiveConditions();
	nodeConditions.add(this->selected == true ? CONDITION_SELECTED : CONDITION_UNSELECTED);

	//
	GUIElementController::initialize();
}

void GUIRadioButtonController::dispose()
{
	GUIElementController::dispose();
	radioButtonGroupNodesByName->erase(
		this->node->getScreenNode()->getId() +
		"_radiobuttongroup_" +
		dynamic_cast<GUIElementNode*>(this->node)->getName()
	);
}

void GUIRadioButtonController::postLayout()
{
}

void GUIRadioButtonController::handleMouseEvent(GUINode* node, GUIMouseEvent* event)
{
	GUIElementController::handleMouseEvent(node, event);
	if (disabled == false && node == this->node && node->isEventBelongingToNode(event) && event->getButton() == MOUSE_BUTTON_LEFT) {
		event->setProcessed(true);
		if (event->getType() == GUIMouseEvent_Type::MOUSEEVENT_RELEASED) {
			select();
			node->getScreenNode()->getGUI()->setFoccussedNode(dynamic_cast< GUIElementNode* >(node));
			node->getScreenNode()->delegateValueChanged(dynamic_cast< GUIElementNode* >(node));
		}
	}
}

void GUIRadioButtonController::handleKeyboardEvent(GUINode* node, GUIKeyboardEvent* event)
{
	GUIElementController::handleKeyboardEvent(node, event);
	if (disabled == false && node == this->node) {
		switch (event->getKeyCode()) {
		case GUIKeyboardEvent::KEYCODE_SPACE: {
				event->setProcessed(true);
				if (event->getType() == GUIKeyboardEvent_Type::KEYBOARDEVENT_KEY_PRESSED) {
					select();
					node->getScreenNode()->delegateValueChanged(dynamic_cast< GUIElementNode* >(node));
				}
			}
			break;
		default: {
				break;
			}
		}

	}
}

void GUIRadioButtonController::tick()
{
	GUIElementController::tick();
}

void GUIRadioButtonController::onFocusGained()
{
}

void GUIRadioButtonController::onFocusLost()
{
}

bool GUIRadioButtonController::hasValue()
{
	return true;
}

const MutableString& GUIRadioButtonController::getValue()
{
	value.reset();
	if (selected == true) {
		value.append((dynamic_cast< GUIElementNode* >(node))->getValue());
	}
	return value;
}

void GUIRadioButtonController::setValue(const MutableString& value)
{
	if (value.equals((dynamic_cast< GUIElementNode* >(node))->getValue()) == true) {
		select();
	}
}

