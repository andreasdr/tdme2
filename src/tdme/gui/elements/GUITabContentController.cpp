#include <tdme/gui/elements/GUITabContentController.h>

#include <tdme/tdme.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUINodeConditions.h>
#include <tdme/gui/nodes/GUIParentNode.h>
#include <tdme/gui/GUI.h>

using tdme::gui::elements::GUITabContentController;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeConditions;
using tdme::gui::nodes::GUIParentNode;
using tdme::gui::GUI;

GUITabContentController::GUITabContentController(GUINode* node)
	: GUINodeController(node)
{
	this->selected = false;
	CONDITION_SELECTED = node->getId() + "-selected";
	CONDITION_UNSELECTED = node->getId() + "-unselected";
}

bool GUITabContentController::isDisabled()
{
	return false;
}

void GUITabContentController::setDisabled(bool disabled)
{
}

bool GUITabContentController::isSelected()
{
	return selected;
}

void GUITabContentController::setSelected(bool selected)
{
	auto& nodeConditions = parentElementNode->getActiveConditions();
	nodeConditions.remove(this->selected == true?CONDITION_SELECTED:CONDITION_UNSELECTED);
	this->selected = selected;
	nodeConditions.add(this->selected == true ?CONDITION_SELECTED:CONDITION_UNSELECTED);
}

void GUITabContentController::initialize()
{
	auto _parentElementNode = this->node->getParentNode();
	while (dynamic_cast<GUIElementNode*>(_parentElementNode) == nullptr) _parentElementNode = _parentElementNode->getParentNode();
	parentElementNode = required_dynamic_cast<GUIElementNode*>(_parentElementNode);
}

void GUITabContentController::dispose()
{
}

void GUITabContentController::postLayout()
{
}

void GUITabContentController::handleMouseEvent(GUINode* node, GUIMouseEvent* event)
{
}

void GUITabContentController::handleKeyboardEvent(GUIKeyboardEvent* event)
{
}

void GUITabContentController::tick()
{
}

void GUITabContentController::onFocusGained()
{
}

void GUITabContentController::onFocusLost()
{
}

bool GUITabContentController::hasValue()
{
	return false;
}

const MutableString& GUITabContentController::getValue()
{
	return value;
}

void GUITabContentController::setValue(const MutableString& value)
{
}

void GUITabContentController::onSubTreeChange()
{
}
