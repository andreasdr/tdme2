#include <tdme/gui/elements/GUIInputController.h>

#include <string>

#include <tdme/gui/events/GUIMouseEvent.h>
#include <tdme/gui/nodes/GUIElementController.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUIInputInternalController.h>
#include <tdme/gui/nodes/GUIInputInternalNode.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUINodeConditions.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/gui/GUI.h>
#include <tdme/utilities/MutableString.h>

using std::string;

using tdme::gui::elements::GUIInputController;
using tdme::gui::events::GUIMouseEvent;
using tdme::gui::nodes::GUIElementController;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUIInputInternalController;
using tdme::gui::nodes::GUIInputInternalNode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeConditions;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::GUI;
using tdme::utilities::MutableString;

string GUIInputController::CONDITION_DISABLED = "disabled";
string GUIInputController::CONDITION_ENABLED = "enabled";

GUIInputController::GUIInputController(GUINode* node)
	: GUIElementController(node)
{
	this->disabled = required_dynamic_cast<GUIElementNode*>(node)->isDisabled();
}

void GUIInputController::onValueChange() {
	auto& nodeConditions = required_dynamic_cast<GUIElementNode*>(node)->getActiveConditions();
	if (inputNode->getText().getString().empty() == true) {
		if (nodeConditions.has("hint") == false) nodeConditions.add("hint");
	} else {
		if (nodeConditions.has("hint") == true) nodeConditions.remove("hint");
	}
}

bool GUIInputController::isDisabled()
{
	return disabled;
}

void GUIInputController::setDisabled(bool disabled)
{
	auto& nodeConditions = required_dynamic_cast<GUIElementNode*>(node)->getActiveConditions();
	nodeConditions.remove(this->disabled == true?CONDITION_DISABLED:CONDITION_ENABLED);
	this->disabled = disabled;
	nodeConditions.add(this->disabled == true?CONDITION_DISABLED:CONDITION_ENABLED);
}

void GUIInputController::initialize()
{
	inputNode = required_dynamic_cast<GUIInputInternalNode*>(node->getScreenNode()->getNodeById(node->getId() + "_text-input"));

	//
	GUIElementController::initialize();

	//
	onValueChange();
}

void GUIInputController::dispose()
{
	GUIElementController::dispose();
}

void GUIInputController::postLayout()
{
}

void GUIInputController::handleMouseEvent(GUINode* node, GUIMouseEvent* event)
{
	GUIElementController::handleMouseEvent(node, event);
	if (disabled == false && node == this->node && node->isEventBelongingToNode(event) && event->getButton() == MOUSE_BUTTON_LEFT) {
		node->getScreenNode()->getGUI()->setFoccussedNode(required_dynamic_cast<GUIElementNode*>(node));
		event->setProcessed(true);
	}
}

void GUIInputController::handleKeyboardEvent(GUIKeyboardEvent* event)
{
}

void GUIInputController::tick()
{
	GUIElementController::tick();
}

void GUIInputController::onFocusGained()
{
	inputNode = required_dynamic_cast<GUIInputInternalNode*>(node->getScreenNode()->getNodeById(node->getId() + "_text-input"));
	inputNode->getController()->onFocusGained();
}

void GUIInputController::onFocusLost()
{
	inputNode = required_dynamic_cast<GUIInputInternalNode*>(node->getScreenNode()->getNodeById(node->getId() + "_text-input"));
	inputNode->getController()->onFocusLost();
}

bool GUIInputController::hasValue()
{
	return true;
}

const MutableString& GUIInputController::getValue()
{
	return inputNode->getText();
}

void GUIInputController::setValue(const MutableString& value)
{
	inputNode->getText().set(value);
	required_dynamic_cast<GUIInputInternalController*>(inputNode->getController())->reset();
	onValueChange();
}
