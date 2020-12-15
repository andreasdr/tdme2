#include <tdme/gui/elements/GUIDropDownController.h>

#include <string>

#include <tdme/gui/GUI.h>
#include <tdme/gui/elements/GUIDropDownOptionController.h>
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
using std::to_string;

using tdme::gui::elements::GUIDropDownController;
using tdme::gui::GUI;
using tdme::gui::elements::GUIDropDownOptionController;
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

string GUIDropDownController::CONDITION_DISABLED = "disabled";
string GUIDropDownController::CONDITION_ENABLED = "enabled";
string GUIDropDownController::CONDITION_OPENED = "opened";
string GUIDropDownController::CONDITION_CLOSED = "closed";

GUIDropDownController::GUIDropDownController(GUINode* node)
	: GUIElementController(node)
{
	open = false;
	this->disabled = (dynamic_cast< GUIElementNode* >(node))->isDisabled();
}

bool GUIDropDownController::isDisabled()
{
	return disabled;
}

void GUIDropDownController::setDisabled(bool disabled)
{
	auto& nodeConditions = (dynamic_cast< GUIElementNode* >(node))->getActiveConditions();
	auto& nodeConditionsTextElement = textElementNode->getActiveConditions();
	nodeConditions.remove(this->disabled == true ? CONDITION_DISABLED : CONDITION_ENABLED);
	nodeConditionsTextElement.remove(this->disabled == true ? CONDITION_DISABLED : CONDITION_ENABLED);
	this->disabled = disabled;
	nodeConditions.add(this->disabled == true ? CONDITION_DISABLED : CONDITION_ENABLED);
	nodeConditionsTextElement.add(this->disabled == true ? CONDITION_DISABLED : CONDITION_ENABLED);
	if (disabled == true && isOpen() == true) {
		toggleOpenState();
	}
}

void GUIDropDownController::initialize()
{
	dropDownNode = dynamic_cast< GUIParentNode* >(node->getScreenNode()->getNodeById(node->getId() + "_layout_horizontal"));
	arrowNode = dynamic_cast< GUIElementNode* >(node->getScreenNode()->getNodeById(node->getId() + "_arrow"));
	textElementNode = dynamic_cast< GUIElementNode* >(node->getScreenNode()->getNodeById(node->getId() + "_layout_horizontal_element"));
	(dynamic_cast< GUIElementNode* >(node))->getActiveConditions().add(open == true ? CONDITION_OPENED : CONDITION_CLOSED);
	arrowNode->getActiveConditions().add(open == true ? CONDITION_OPENED : CONDITION_CLOSED);

	//
	GUIElementController::initialize();
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
	(dynamic_cast< GUIParentNode* >(node))->getChildControllerNodes(childControllerNodes);
	for (auto i = 0; i < childControllerNodes.size(); i++) {
		auto childControllerNode = childControllerNodes[i];
		auto childController = childControllerNode->getController();
		if (dynamic_cast< GUIDropDownOptionController* >(childController) != nullptr) {
			(dynamic_cast< GUIDropDownOptionController* >(childController))->unselect();
		}
	}
}

void GUIDropDownController::toggleOpenState()
{
	(dynamic_cast< GUIElementNode* >(node))->getActiveConditions().remove(open == true ? CONDITION_OPENED : CONDITION_CLOSED);
	arrowNode->getActiveConditions().remove(open == true ? CONDITION_OPENED : CONDITION_CLOSED);
	open = open == true ? false : true;
	(dynamic_cast< GUIElementNode* >(node))->getActiveConditions().add(open == true ? CONDITION_OPENED : CONDITION_CLOSED);
	arrowNode->getActiveConditions().add(open == true ? CONDITION_OPENED : CONDITION_CLOSED);
	search.clear();
	doSearch();
}

void GUIDropDownController::determineDropDownOptionControllers()
{
	dropDownOptionControllers.clear();
	(dynamic_cast< GUIParentNode* >(node))->getChildControllerNodes(childControllerNodes);
	for (auto i = 0; i < childControllerNodes.size(); i++) {
		auto childControllerNode = childControllerNodes[i];
		auto childController = childControllerNode->getController();
		if (dynamic_cast< GUIDropDownOptionController* >(childController) != nullptr) {
			dropDownOptionControllers.push_back(dynamic_cast< GUIDropDownOptionController* >(childController));
		}
	}
}

int GUIDropDownController::getSelectedOptionIdx()
{
	auto selectedDropDownOptionControlerIdx = -1;
	for (auto i = 0; i < dropDownOptionControllers.size(); i++) {
		auto selectBoxOptionController = dropDownOptionControllers[i];
		if (selectBoxOptionController->isSelected() == true) {
			selectedDropDownOptionControlerIdx = i;
			break;
		}
	}
	return selectedDropDownOptionControlerIdx;
}

void GUIDropDownController::selectNext()
{
	determineDropDownOptionControllers();
	auto selectedDropDownOptionControlerIdx = getSelectedOptionIdx();
	unselect();
	if (dropDownOptionControllers.size() == 0)
		return;

	selectedDropDownOptionControlerIdx = (selectedDropDownOptionControlerIdx + 1) % dropDownOptionControllers.size();
	if (selectedDropDownOptionControlerIdx < 0)
		selectedDropDownOptionControlerIdx += dropDownOptionControllers.size();

	dropDownOptionControllers[selectedDropDownOptionControlerIdx]->select();
	dropDownOptionControllers[selectedDropDownOptionControlerIdx]->getNode()->scrollToNodeX(dropDownNode);
	dropDownOptionControllers[selectedDropDownOptionControlerIdx]->getNode()->scrollToNodeY(dropDownNode);
}

void GUIDropDownController::selectPrevious()
{
	determineDropDownOptionControllers();
	auto selectedDropDownOptionControlerIdx = getSelectedOptionIdx();
	unselect();
	if (dropDownOptionControllers.size() == 0)
		return;

	selectedDropDownOptionControlerIdx = (selectedDropDownOptionControlerIdx - 1) % dropDownOptionControllers.size();
	if (selectedDropDownOptionControlerIdx < 0)
		selectedDropDownOptionControlerIdx += dropDownOptionControllers.size();

	dropDownOptionControllers[selectedDropDownOptionControlerIdx]->select();
	dropDownOptionControllers[selectedDropDownOptionControlerIdx]->getNode()->scrollToNodeX(dropDownNode);
	dropDownOptionControllers[selectedDropDownOptionControlerIdx]->getNode()->scrollToNodeY(dropDownNode);
}

void GUIDropDownController::doSearch() {
	determineDropDownOptionControllers();
	string selectedDropDownOptionText;
	auto selectedDropDownOptionControllerIdx = getSelectedOptionIdx();
	if (selectedDropDownOptionControllerIdx != -1) {
		selectedDropDownOptionText = dynamic_cast< GUITextNode* >(node->getScreenNode()->getNodeById(dropDownOptionControllers[selectedDropDownOptionControllerIdx]->getNode()->getId() + "_unselected"))->getText().getString();
	}
	auto dropDownTextNode = dynamic_cast< GUITextNode* >(node->getScreenNode()->getNodeById(node->getId() + "_unselected"));
	auto dropDownTextNodeEnabled = dynamic_cast< GUITextNode* >(node->getScreenNode()->getNodeById(node->getId() + "_text_enabled"));
	dropDownTextNodeEnabled->setText(search.empty() == false?MutableString(search):MutableString(selectedDropDownOptionText));
	auto dropDownTextNodeDisabled = dynamic_cast< GUITextNode* >(node->getScreenNode()->getNodeById(node->getId() + "_text_disabled"));
	dropDownTextNodeDisabled->setText(search.empty() == false?MutableString(search):MutableString(selectedDropDownOptionText));
	if (dropDownOptionControllers.size() > 0) {
		auto firstVisibleDropDown = -1;
		auto searchLowerCase = StringTools::toLowerCase(search);
		auto i = 0;
		for (auto dropDownOptionController: dropDownOptionControllers) {
			auto match = dropDownOptionController->search(searchLowerCase);
			if (firstVisibleDropDown == -1 && match == true) firstVisibleDropDown = i;
			i++;
		}
		if (firstVisibleDropDown != -1) {
			dropDownOptionControllers[firstVisibleDropDown]->getNode()->scrollToNodeX(dropDownNode);
			dropDownOptionControllers[firstVisibleDropDown]->getNode()->scrollToNodeY(dropDownNode);
		}
	}
}

void GUIDropDownController::handleMouseEvent(GUINode* node, GUIMouseEvent* event)
{
	GUIElementController::handleMouseEvent(node, event);
	if (disabled == true) return;
	auto elementNode  = dynamic_cast< GUIElementNode* >(this->node);
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

void GUIDropDownController::handleKeyboardEvent(GUINode* node, GUIKeyboardEvent* event)
{
	GUIElementController::handleKeyboardEvent(node, event);
	if (disabled == false && node == this->node) {
		auto keyChar = event->getKeyChar();
		if (keyChar >= 32 && keyChar < 127) {
			event->setProcessed(true);
			#if defined(VULKAN) || defined(GLFW3)
				if (event->getType() == GUIKeyboardEvent::KEYBOARDEVENT_KEY_TYPED) {
			#else
				if (event->getType() == GUIKeyboardEvent::KEYBOARDEVENT_KEY_PRESSED) {
			#endif
				search+= event->getKeyChar();
			}
		} else {
			switch (event->getKeyCode()) {
				case GUIKeyboardEvent::KEYCODE_BACKSPACE:
				case GUIKeyboardEvent::KEYCODE_DELETE: {
					if (event->getType() == GUIKeyboardEvent::KEYBOARDEVENT_KEY_PRESSED) {
						search.clear();
					}
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
							node->getScreenNode()->delegateValueChanged(dynamic_cast< GUIElementNode* >(node));
						}
					}
					break;
				}
			}
		}
		if (open == false) toggleOpenState();
		doSearch();
	}
}

void GUIDropDownController::tick()
{
	GUIElementController::tick();
	if (open == true) node->getScreenNode()->getGUI()->addMouseOutClickCandidateElementNode(dynamic_cast< GUIElementNode* >(this->node));
}

void GUIDropDownController::onFocusGained()
{
}

void GUIDropDownController::onFocusLost()
{
}

bool GUIDropDownController::hasValue()
{
	return true;
}

const MutableString& GUIDropDownController::getValue()
{
	value.reset();
	determineDropDownOptionControllers();
	for (auto i = 0; i < dropDownOptionControllers.size(); i++) {
		auto dropDownOptionController = dropDownOptionControllers[i];
		if (dropDownOptionController->isSelected() == true) {
			value.append((dynamic_cast< GUIElementNode* >(dropDownOptionController->getNode()))->getValue());
		}
	}
	return value;
}

void GUIDropDownController::setValue(const MutableString& value)
{
	determineDropDownOptionControllers();
	unselect();
	for (auto i = 0; i < dropDownOptionControllers.size(); i++) {
		auto dropDownOptionController = dropDownOptionControllers[i];
		auto dropDownOptionNode = (dynamic_cast< GUIElementNode* >(dropDownOptionController->getNode()));
		if (value.equals(dropDownOptionNode->getValue())) {
			dropDownOptionController->select();
			dropDownOptionNode->scrollToNodeX(dropDownNode);
			dropDownOptionNode->scrollToNodeY(dropDownNode);
			break;
		}
	}
}

