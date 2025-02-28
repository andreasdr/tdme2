#include <agui/gui/elements/GUIDropDownController.h>

#include <string>

#include <agui/agui.h>
#include <agui/gui/elements/GUIDropDownOptionController.h>
#include <agui/gui/events/GUIKeyboardEvent.h>
#include <agui/gui/events/GUIMouseEvent.h>
#include <agui/gui/nodes/GUIElementController.h>
#include <agui/gui/nodes/GUIElementNode.h>
#include <agui/gui/nodes/GUINode.h>
#include <agui/gui/nodes/GUINodeConditions.h>
#include <agui/gui/nodes/GUIParentNode.h>
#include <agui/gui/nodes/GUIScreenNode.h>
#include <agui/gui/nodes/GUITextNode.h>
#include <agui/gui/GUI.h>
#include <agui/utilities/MutableString.h>
#include <agui/utilities/StringTools.h>

using std::string;
using std::to_string;

using agui::gui::elements::GUIDropDownController;
using agui::gui::elements::GUIDropDownOptionController;
using agui::gui::events::GUIKeyboardEvent;
using agui::gui::events::GUIMouseEvent;
using agui::gui::nodes::GUIElementController;
using agui::gui::nodes::GUIElementNode;
using agui::gui::nodes::GUINode;
using agui::gui::nodes::GUINodeConditions;
using agui::gui::nodes::GUIParentNode;
using agui::gui::nodes::GUIScreenNode;
using agui::gui::nodes::GUITextNode;
using agui::gui::GUI;
using agui::utilities::MutableString;
using agui::utilities::StringTools;

string GUIDropDownController::CONDITION_DISABLED = "disabled";
string GUIDropDownController::CONDITION_ENABLED = "enabled";
string GUIDropDownController::CONDITION_OPENED = "opened";
string GUIDropDownController::CONDITION_CLOSED = "closed";

GUIDropDownController::GUIDropDownController(GUINode* node)
	: GUIElementController(node)
{
	open = false;
	this->disabled = required_dynamic_cast<GUIElementNode*>(node)->isDisabled();
}

bool GUIDropDownController::isDisabled()
{
	return disabled;
}

void GUIDropDownController::setDisabled(bool disabled)
{
	auto& nodeConditions = required_dynamic_cast<GUIElementNode*>(node)->getActiveConditions();
	auto& nodeConditionsTextElement = textElementNode->getActiveConditions();
	nodeConditions.remove(this->disabled == true?CONDITION_DISABLED:CONDITION_ENABLED);
	nodeConditionsTextElement.remove(this->disabled == true?CONDITION_DISABLED:CONDITION_ENABLED);
	this->disabled = disabled;
	nodeConditions.add(this->disabled == true?CONDITION_DISABLED:CONDITION_ENABLED);
	nodeConditionsTextElement.add(this->disabled == true ?CONDITION_DISABLED:CONDITION_ENABLED);
	if (disabled == true && isOpen() == true) {
		toggleOpenState();
	}
}

void GUIDropDownController::initialize()
{
	dropDownContainerNode = required_dynamic_cast<GUIParentNode*>(node->getScreenNode()->getInnerNodeById(node->getId()));
	arrowNode = required_dynamic_cast<GUIElementNode*>(node->getScreenNode()->getNodeById(node->getId() + "_arrow"));
	textElementNode = required_dynamic_cast<GUIElementNode*>(node->getScreenNode()->getNodeById(node->getId() + "_layout_horizontal_element"));
	required_dynamic_cast<GUIElementNode*>(node)->getActiveConditions().add(open == true?CONDITION_OPENED:CONDITION_CLOSED);
	arrowNode->getActiveConditions().add(open == true?CONDITION_OPENED:CONDITION_CLOSED);

	//
	GUIElementController::initialize();

	//
	determineDropDownOptionControllers();
}

void GUIDropDownController::dispose()
{
	GUIElementController::dispose();
}

void GUIDropDownController::postLayout()
{
}

bool GUIDropDownController::isOpen()
{
	return open;
}

void GUIDropDownController::unselect()
{
	if (selectedDropDownOptionControllerIdx == -1) return;
	dropDownOptionControllers[selectedDropDownOptionControllerIdx]->unselect();
}

void GUIDropDownController::select(int idx) {
	unselect();
	selectedDropDownOptionControllerIdx = idx;
	if (selectedDropDownOptionControllerIdx == -1) return;
	dropDownOptionControllers[selectedDropDownOptionControllerIdx]->select();
	dropDownOptionControllers[selectedDropDownOptionControllerIdx]->getNode()->scrollToNodeX(dropDownContainerNode);
	dropDownOptionControllers[selectedDropDownOptionControllerIdx]->getNode()->scrollToNodeY(dropDownContainerNode);
}

void GUIDropDownController::select(GUIElementNode* selectedDropDownOptionElementNode) {
	unselect();
	auto selectedDropDownOptionElementController = selectedDropDownOptionElementNode->getController();
	auto i = 0;
	for (auto dropDownOptionController: dropDownOptionControllers) {
		if (dropDownOptionController == selectedDropDownOptionElementController) {
			select(i);
			break;
		}
		i++;
	}
}

void GUIDropDownController::toggleOpenState()
{
	if (open == false) {
		lastSelectedDropDownOptionControllerIdx = selectedDropDownOptionControllerIdx;
	}
	required_dynamic_cast<GUIElementNode*>(node)->getActiveConditions().remove(open == true?CONDITION_OPENED:CONDITION_CLOSED);
	arrowNode->getActiveConditions().remove(open == true?CONDITION_OPENED:CONDITION_CLOSED);
	open = open == true?false:true;
	required_dynamic_cast<GUIElementNode*>(node)->getActiveConditions().add(open == true?CONDITION_OPENED:CONDITION_CLOSED);
	arrowNode->getActiveConditions().add(open == true?CONDITION_OPENED:CONDITION_CLOSED);
	search.clear();
	doSearch();
}

void GUIDropDownController::determineDropDownOptionControllers()
{
	vector<GUINode*> childControllerNodes;
	dropDownOptionControllers.clear();
	required_dynamic_cast<GUIParentNode*>(node)->getChildControllerNodes(childControllerNodes);
	for (auto i = 0; i < childControllerNodes.size(); i++) {
		auto childControllerNode = childControllerNodes[i];
		auto childController = dynamic_cast<GUIDropDownOptionController*>(childControllerNode->getController());
		if (childController != nullptr) dropDownOptionControllers.push_back(childController);
	}
}

void GUIDropDownController::selectNext()
{
	unselect();
	if (dropDownOptionControllers.size() == 0)
		return;

	auto disabledCount = 0;
	while (disabledCount < dropDownOptionControllers.size()) {
		selectedDropDownOptionControllerIdx = (selectedDropDownOptionControllerIdx + 1) % dropDownOptionControllers.size();
		if (dropDownOptionControllers[selectedDropDownOptionControllerIdx]->isDisabled() == false &&
			dropDownOptionControllers[selectedDropDownOptionControllerIdx]->isHidden() == false) break;
		disabledCount++;
	}
	if (disabledCount == dropDownOptionControllers.size()) {
		selectedDropDownOptionControllerIdx = -1;
		return;
	}

	dropDownOptionControllers[selectedDropDownOptionControllerIdx]->select();
	dropDownOptionControllers[selectedDropDownOptionControllerIdx]->getNode()->scrollToNodeX(dropDownContainerNode);
	dropDownOptionControllers[selectedDropDownOptionControllerIdx]->getNode()->scrollToNodeY(dropDownContainerNode);
}

void GUIDropDownController::selectPrevious()
{
	unselect();
	if (dropDownOptionControllers.size() == 0)
		return;

	if (selectedDropDownOptionControllerIdx == -1) selectedDropDownOptionControllerIdx = (int)dropDownOptionControllers.size() - 1;

	auto disabledCount = 0;
	while (disabledCount < dropDownOptionControllers.size()) {
		selectedDropDownOptionControllerIdx = (selectedDropDownOptionControllerIdx - 1) % dropDownOptionControllers.size();
		if (selectedDropDownOptionControllerIdx < 0)
			selectedDropDownOptionControllerIdx += dropDownOptionControllers.size();
		if (dropDownOptionControllers[selectedDropDownOptionControllerIdx]->isDisabled() == false &&
			dropDownOptionControllers[selectedDropDownOptionControllerIdx]->isHidden() == false) break;
		disabledCount++;
	}
	if (disabledCount == dropDownOptionControllers.size()) {
		selectedDropDownOptionControllerIdx = -1;
		return;
	}


	dropDownOptionControllers[selectedDropDownOptionControllerIdx]->select();
	dropDownOptionControllers[selectedDropDownOptionControllerIdx]->getNode()->scrollToNodeX(dropDownContainerNode);
	dropDownOptionControllers[selectedDropDownOptionControllerIdx]->getNode()->scrollToNodeY(dropDownContainerNode);
}

void GUIDropDownController::selectLast()
{
	if (lastSelectedDropDownOptionControllerIdx == -1 || lastSelectedDropDownOptionControllerIdx >= dropDownOptionControllers.size()) {
		lastSelectedDropDownOptionControllerIdx = -1;
		return;
	}
	unselect();

	dropDownOptionControllers[lastSelectedDropDownOptionControllerIdx]->select();
	dropDownOptionControllers[lastSelectedDropDownOptionControllerIdx]->getNode()->scrollToNodeX(dropDownContainerNode);
	dropDownOptionControllers[lastSelectedDropDownOptionControllerIdx]->getNode()->scrollToNodeY(dropDownContainerNode);
}

void GUIDropDownController::doSearch() {
	determineDropDownOptionControllers();
	string selectedDropDownOptionText;
	if (selectedDropDownOptionControllerIdx != -1) {
		selectedDropDownOptionText = required_dynamic_cast<GUITextNode*>(node->getScreenNode()->getNodeById(dropDownOptionControllers[selectedDropDownOptionControllerIdx]->getNode()->getId() + "_unselected"))->getText().getString();
	}
	auto dropDownTextNodeEnabled = required_dynamic_cast<GUITextNode*>(node->getScreenNode()->getNodeById(node->getId() + "_text_enabled"));
	dropDownTextNodeEnabled->setText(search.empty() == false?MutableString(search):MutableString(selectedDropDownOptionText));
	auto dropDownTextNodeDisabled = required_dynamic_cast<GUITextNode*>(node->getScreenNode()->getNodeById(node->getId() + "_text_disabled"));
	dropDownTextNodeDisabled->setText(search.empty() == false?MutableString(search):MutableString(selectedDropDownOptionText));
	if (dropDownOptionControllers.size() > 0) {
		auto scrollToDropDownOption = -1;
		auto searchLowerCase = StringTools::toLowerCase(search);
		auto i = 0;
		for (auto dropDownOptionController: dropDownOptionControllers) {
			auto match = dropDownOptionController->search(searchLowerCase);
			if (match == true && (scrollToDropDownOption == -1 || i == lastSelectedDropDownOptionControllerIdx)) scrollToDropDownOption = i;
			i++;
		}
		if (scrollToDropDownOption != -1) {
			dropDownOptionControllers[scrollToDropDownOption]->getNode()->scrollToNodeX(dropDownContainerNode);
			dropDownOptionControllers[scrollToDropDownOption]->getNode()->scrollToNodeY(dropDownContainerNode);
		}
	}
}

void GUIDropDownController::handleMouseEvent(GUINode* node, GUIMouseEvent* event)
{
	GUIElementController::handleMouseEvent(node, event);
	if (disabled == true) return;
	auto elementNode  = required_dynamic_cast<GUIElementNode*>(this->node);
	if (event->getButton() == MOUSE_BUTTON_LEFT) {
		if (node == this->node && node->isEventBelongingToNode(event) == true) {
			if (event->getType() == GUIMouseEvent::MOUSEEVENT_RELEASED) {
				event->setProcessed(true);
				toggleOpenState();
				node->getScreenNode()->getGUI()->setFoccussedNode(elementNode);
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

void GUIDropDownController::handleKeyboardEvent(GUIKeyboardEvent* event)
{
	GUIElementController::handleKeyboardEvent(event);
	if (disabled == false) {
		auto keyChar = event->getKeyChar();
		if (keyChar > 32 && keyChar < 127 && keyChar >= 32) {
			event->setProcessed(true);
			if (event->getType() == GUIKeyboardEvent::KEYBOARDEVENT_KEY_TYPED) {
				if (isOpen() == false) toggleOpenState();
				search+= event->getKeyChar();
				doSearch();
			}
		} else {
			switch (event->getKeyCode()) {
				case GUIKeyboardEvent::KEYCODE_ESCAPE:
					if (event->getType() == GUIKeyboardEvent::KEYBOARDEVENT_KEY_PRESSED) {
						if (isOpen() == true) {
							selectLast();
							toggleOpenState();
						}
					}
					break;
				case GUIKeyboardEvent::KEYCODE_BACKSPACE:
				case GUIKeyboardEvent::KEYCODE_DELETE: {
					if (event->getType() == GUIKeyboardEvent::KEYBOARDEVENT_KEY_PRESSED) {
						search.clear();
					}
					if (isOpen() == false) toggleOpenState();
					break;
				case GUIKeyboardEvent::KEYCODE_UP: {
						event->setProcessed(true);
						if (event->getType() == GUIKeyboardEvent::KEYBOARDEVENT_KEY_PRESSED) {
							if (isOpen() == false) {
								toggleOpenState();
							} else {
								selectPrevious();
							}
						}
					}
					break;
				case GUIKeyboardEvent::KEYCODE_DOWN: {
						event->setProcessed(true);
						if (event->getType() == GUIKeyboardEvent::KEYBOARDEVENT_KEY_PRESSED) {
							if (isOpen() == false) {
								toggleOpenState();
							} else {
								selectNext();
							}
						}
					}
					break;
				case GUIKeyboardEvent::KEYCODE_SPACE: {
						event->setProcessed(true);
						if (event->getType() == GUIKeyboardEvent::KEYBOARDEVENT_KEY_PRESSED) {
							toggleOpenState();
						}
						if (open == false) {
							node->getScreenNode()->forwardChange(required_dynamic_cast<GUIElementNode*>(node));
						}
					}
					break;
				}
			}
		}
	}
}

void GUIDropDownController::tick()
{
	GUIElementController::tick();
	if (open == true) node->getScreenNode()->getGUI()->addMouseOutClickCandidateNode(required_dynamic_cast<GUIElementNode*>(this->node));
}

void GUIDropDownController::onFocusGained()
{
}

void GUIDropDownController::onFocusLost()
{
	if (isOpen() == true) {
		toggleOpenState();
	}
}

bool GUIDropDownController::hasValue()
{
	return true;
}

const MutableString& GUIDropDownController::getValue()
{
	value.reset();
	if (selectedDropDownOptionControllerIdx != -1) value.append(required_dynamic_cast<GUIElementNode*>(dropDownOptionControllers[selectedDropDownOptionControllerIdx]->getNode())->getValue());
	return value;
}

void GUIDropDownController::setValue(const MutableString& value)
{
	unselect();
	auto i = 0;
	for (auto dropDownOptionController: dropDownOptionControllers) {
		auto dropDownOptionNode = required_dynamic_cast<GUIElementNode*>(dropDownOptionController->getNode());
		if (value.equals(dropDownOptionNode->getValue())) {
			select(i);
			break;
		}
		i++;
	}
}

void GUIDropDownController::onSubTreeChange() {
	determineDropDownOptionControllers();
	selectedDropDownOptionControllerIdx = -1;
	auto i = 0;
	for (auto dropDownOptionController: dropDownOptionControllers) {
		auto dropDownOptionNode = required_dynamic_cast<GUIElementNode*>(dropDownOptionController->getNode());
		dropDownOptionController->unselect();
		if (dropDownOptionNode->isSelected() == true) {
			selectedDropDownOptionControllerIdx = i;
			break;
		}
		i++;
	}
	if (selectedDropDownOptionControllerIdx != -1) {
		select(selectedDropDownOptionControllerIdx);
	}
}
