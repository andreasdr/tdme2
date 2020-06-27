#include <tdme/gui/nodes/GUIElementNode.h>

#include <set>

#include <tdme/gui/events/GUIMouseEvent_Type.h>
#include <tdme/gui/events/GUIMouseEvent.h>
#include <tdme/gui/nodes/GUIElementController.h>
#include <tdme/gui/nodes/GUIElementIgnoreEventsController.h>
#include <tdme/gui/nodes/GUIImageNode.h>
#include <tdme/gui/nodes/GUINode_Alignments.h>
#include <tdme/gui/nodes/GUINode_Border.h>
#include <tdme/gui/nodes/GUINode_ComputedConstraints.h>
#include <tdme/gui/nodes/GUINode_Padding.h>
#include <tdme/gui/nodes/GUINode_RequestedConstraints_RequestedConstraintsType.h>
#include <tdme/gui/nodes/GUINode_RequestedConstraints.h>
#include <tdme/gui/nodes/GUINode_Scale9Grid.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUINodeConditions.h>
#include <tdme/gui/nodes/GUINodeController.h>
#include <tdme/gui/nodes/GUIParentNode_Overflow.h>
#include <tdme/gui/nodes/GUIParentNode.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/utils/Console.h>
#include <tdme/utils/Float.h>
#include <tdme/utils/StringTokenizer.h>
#include <tdme/utils/StringUtils.h>

using std::set;

using tdme::gui::events::GUIMouseEvent_Type;
using tdme::gui::events::GUIMouseEvent;
using tdme::gui::nodes::GUIElementController;
using tdme::gui::nodes::GUIElementIgnoreEventsController;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUIImageNode;
using tdme::gui::nodes::GUINode_Border;
using tdme::gui::nodes::GUINode_ComputedConstraints;
using tdme::gui::nodes::GUINode_Padding;
using tdme::gui::nodes::GUINode_RequestedConstraints_RequestedConstraintsType;
using tdme::gui::nodes::GUINode_RequestedConstraints;
using tdme::gui::nodes::GUINode_Scale9Grid;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeConditions;
using tdme::gui::nodes::GUINodeController;
using tdme::gui::nodes::GUIParentNode_Overflow;
using tdme::gui::nodes::GUIParentNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::utils::Console;
using tdme::utils::Float;
using tdme::utils::StringTokenizer;
using tdme::utils::StringUtils;

GUIElementNode::GUIElementNode(
	GUIScreenNode* screenNode,
	GUIParentNode* parentNode,
	const string& id,
	GUINode_Flow* flow,
	GUIParentNode_Overflow* overflowX,
	GUIParentNode_Overflow* overflowY,
	const GUINode_Alignments& alignments,
	const GUINode_RequestedConstraints& requestedConstraints,
	const GUIColor& backgroundColor,
	const string& backgroundImage,
	const GUINode_Scale9Grid& backgroundImageScaleGrid,
	const GUIColor& backgroundImageEffectColorMul,
	const GUIColor& backgroundImageEffectColorAdd,
	const GUINode_Border& border,
	const GUINode_Padding& padding,
	const GUINodeConditions& showOn,
	const GUINodeConditions& hideOn,
	const string& name,
	const string& value,
	bool selected,
	bool disabled,
	bool focusable,
	bool ignoreEvents,
	const string& onInitializeExpression,
	const string& onMouseClickExpression,
	const string& onMouseDoubleClickExpression,
	const string& onMouseOverExpression,
	const string& onMouseOutExpression,
	const string& onChangeExpression
	) :
	GUIParentNode(screenNode, parentNode, id, flow, overflowX, overflowY, alignments, requestedConstraints, backgroundColor, backgroundImage, backgroundImageScaleGrid, backgroundImageEffectColorMul, backgroundImageEffectColorAdd, border, padding, showOn, hideOn),
	activeConditions(this)
{
	init();
	this->name = name;
	this->value = value;
	this->selected = selected;
	this->disabled = disabled;
	this->focusable = focusable;
	this->ignoreEvents = ignoreEvents;
	this->onInitializeExpression = onInitializeExpression;
	this->onMouseClickExpression = onMouseClickExpression;
	this->onMouseDoubleClickExpression = onMouseDoubleClickExpression;
	this->onMouseOverExpression = onMouseOverExpression;
	this->onMouseOutExpression = onMouseOutExpression;
	this->onChangeExpression = onChangeExpression;
	this->controller = ignoreEvents == true ? static_cast< GUINodeController* >(new GUIElementIgnoreEventsController(this)) : static_cast< GUINodeController* >(new GUIElementController(this));
	this->controller->initialize();
}

void GUIElementNode::init()
{
}

string GUIElementNode::CONDITION_ALWAYS = "always";
string GUIElementNode::CONDITION_ONMOUSEOVER = "mouseover";
string GUIElementNode::CONDITION_CLICK = "click";
string GUIElementNode::CONDITION_FOCUS = "focus";

const string GUIElementNode::getNodeType()
{
	return "element";
}

bool GUIElementNode::isContentNode()
{
	return false;
}

int32_t GUIElementNode::getContentWidth()
{
	auto width = 0;
	for (auto i = 0; i < subNodes.size(); i++) {
		auto guiSubNode = subNodes[i];
		if (guiSubNode->conditionsMet == false) continue;
		auto contentWidth = guiSubNode->getAutoWidth();
		if (contentWidth > width) {
			width = contentWidth;
		}
	}
	width += border.left + border.right;
	width += padding.left + padding.right;
	return width;
}

int32_t GUIElementNode::getContentHeight()
{
	auto height = 0;
	for (auto i = 0; i < subNodes.size(); i++) {
		auto guiSubNode = subNodes[i];
		if (guiSubNode->conditionsMet == false) continue;
		auto contentHeight = guiSubNode->getAutoHeight();
		if (contentHeight > height) {
			height = contentHeight;
		}
	}
	height += border.top + border.bottom;
	height += padding.top + padding.bottom;
	return height;
}

bool GUIElementNode::isFocusable()
{
	return focusable;
}

void GUIElementNode::setTop(int32_t top)
{
	if (requestedConstraints.topType == GUINode_RequestedConstraints_RequestedConstraintsType::PIXEL) top = requestedConstraints.top;
	GUIParentNode::setTop(top);
	top += computedConstraints.alignmentTop;
	for (auto i = 0; i < subNodes.size(); i++) {
		auto guiSubNode = subNodes[i];
		if (guiSubNode->conditionsMet == false) continue;
		guiSubNode->setTop(top);
	}
}

void GUIElementNode::setLeft(int32_t left)
{
	if (requestedConstraints.leftType == GUINode_RequestedConstraints_RequestedConstraintsType::PIXEL) left = requestedConstraints.left;
	GUIParentNode::setLeft(left);
	left += computedConstraints.alignmentLeft;
	for (auto i = 0; i < subNodes.size(); i++) {
		auto guiSubNode = subNodes[i];
		if (guiSubNode->conditionsMet == false) continue;
		guiSubNode->setLeft(left);
	}
}

void GUIElementNode::layoutSubNodes()
{
	if (conditionsMet == false) {
		layouted = false;
		return;
	}
	GUIParentNode::layoutSubNodes();
	auto height = computedConstraints.height - border.top - border.bottom - padding.top - padding.bottom;
	auto width = computedConstraints.width - border.left - border.right - padding.left - padding.right;
	for (auto i = 0; i < subNodes.size(); i++) {
		auto guiSubNode = subNodes[i];
		if (guiSubNode->conditionsMet == false) continue;
		auto doLayoutSubNodes = false;
		if (guiSubNode->requestedConstraints.heightType == GUINode_RequestedConstraints_RequestedConstraintsType::STAR) {
			guiSubNode->computedConstraints.height = height;
			doLayoutSubNodes = true;
		} else
		if (guiSubNode->requestedConstraints.widthType == GUINode_RequestedConstraints_RequestedConstraintsType::STAR) {
			guiSubNode->computedConstraints.width = width;
			doLayoutSubNodes = true;
		}
		if (dynamic_cast< GUIParentNode* >(guiSubNode) != nullptr && doLayoutSubNodes == true) {
			(dynamic_cast< GUIParentNode* >(guiSubNode))->layoutSubNodes();
		}
	}
	setTop(computedConstraints.top);
	setLeft(computedConstraints.left);
}

void GUIElementNode::layout()
{
	if (conditionsMet == false) {
		layouted = false;
		return;
	}
	if (requestedConstraints.heightType == GUINode_RequestedConstraints_RequestedConstraintsType::PIXEL) {
		auto subNodesHeight = requestedConstraints.height - border.top - border.bottom - padding.top - padding.bottom;
		for (auto i = 0; i < subNodes.size(); i++) {
			auto guiSubNode = subNodes[i];
			if (guiSubNode->conditionsMet == false) continue;
			if (overflowY == GUIParentNode_Overflow::DOWNSIZE_CHILDREN && guiSubNode->requestedConstraints.heightType == GUINode_RequestedConstraints_RequestedConstraintsType::PIXEL && guiSubNode->requestedConstraints.height > subNodesHeight) {
				guiSubNode->requestedConstraints.height = subNodesHeight;
			}
		}
	}
	if (requestedConstraints.widthType == GUINode_RequestedConstraints_RequestedConstraintsType::PIXEL) {
		auto subNodesWidth = requestedConstraints.width - border.left - border.right- padding.left - padding.right;
		for (auto i = 0; i < subNodes.size(); i++) {
			auto guiSubNode = subNodes[i];
			if (guiSubNode->conditionsMet == false) continue;
			if (overflowY == GUIParentNode_Overflow::DOWNSIZE_CHILDREN && guiSubNode->requestedConstraints.widthType == GUINode_RequestedConstraints_RequestedConstraintsType::PIXEL && guiSubNode->requestedConstraints.width > subNodesWidth) {
				guiSubNode->requestedConstraints.width = subNodesWidth;
			}
		}
	}
	GUIParentNode::layout();
	setTop(computedConstraints.top);
	setLeft(computedConstraints.left);
	computeHorizontalChildrenAlignment();
	computeVerticalChildrenAlignment();
}

const string& GUIElementNode::getName()
{
	return name;
}

const string& GUIElementNode::getValue()
{
	return value;
}

bool GUIElementNode::isSelected()
{
	return selected;
}

bool GUIElementNode::isDisabled()
{
	return disabled;
}

const string& GUIElementNode::getOnInitializeExpression() {
	return onInitializeExpression;
}

const string& GUIElementNode::getOnMouseClickExpression() {
	return onMouseClickExpression;
}

const string& GUIElementNode::getOnMouseDoubleClickExpression() {
	return onMouseDoubleClickExpression;
}

const string& GUIElementNode::getOnMouseOverExpression() {
	return onMouseOverExpression;
}

const string& GUIElementNode::getOnMouseOutExpression() {
	return onMouseOutExpression;
}

const string& GUIElementNode::getOnChangeExpression() {
	return onChangeExpression;
}

void GUIElementNode::executeExpression(const string& expression) {
	StringTokenizer t1;
	StringTokenizer t2;
	t1.tokenize(expression, ";");
	while (t1.hasMoreTokens()) {
		t2.tokenize(t1.nextToken(), "=");
		string command;
		string value;
		string nodeId;
		string subCommand;
		if (t2.countTokens() > 0) {
			command = StringUtils::trim(t2.nextToken());
			if (t2.countTokens() > 1) value = StringUtils::trim(t2.nextToken());
		}
		t2.tokenize(command, ".");
		if (t2.countTokens() == 2) {
			nodeId = StringUtils::trim(t2.nextToken());
			subCommand = StringUtils::trim(t2.nextToken());
		}
		// element (controller) values
		if (subCommand == "value") {
			auto nodeElementNode = dynamic_cast<GUIElementNode*>(screenNode->getNodeById(nodeId));
			auto nodeController = nodeElementNode != nullptr?nodeElementNode->getController():nullptr;
			if (StringUtils::startsWith(value, "'") == true && StringUtils::endsWith(value, "'") == true) {
				if (nodeController != nullptr) nodeController->setValue(MutableString(StringUtils::substring(value, 1, value.size() - 1)));
			} else
			if (StringUtils::endsWith(value, ".value") == true) {
				auto nodeValueElementNode = dynamic_cast<GUIElementNode*>(screenNode->getNodeById(StringUtils::substring(value, 0, value.length() - string(".value").size())));
				auto nodeValueController = nodeValueElementNode != nullptr?nodeValueElementNode->getController():nullptr;
				if (nodeController != nullptr && nodeValueController != nullptr) nodeController->setValue(nodeValueController->getValue());
			} else {
				Console::println("GUIElementController::executeExpression(): Unknown value in expression: " + value);
			}
		} else
		// element conditions
		if (subCommand == "condition") {
			auto nodeElementNode = dynamic_cast<GUIElementNode*>(screenNode->getNodeById(nodeId));
			if (nodeElementNode != nullptr) {
				if (value.find('?') != string::npos &&
					value.find(':') != string::npos &&
					value.find(':') > value.find('?')) {
					t2.tokenize(value, "?:");
					string testCondition = t2.hasMoreTokens() == true?t2.nextToken():"";
					string setOnTrueCondition = t2.hasMoreTokens() == true?t2.nextToken():"";
					string setOnFalseCondition = t2.hasMoreTokens() == true?t2.nextToken():"";
					if (testCondition.empty() == true ||
						setOnTrueCondition.empty() == true ||
						setOnFalseCondition.empty() == true) {
						Console::println("GUIElementController::executeExpression(): = ternary operator requires the following format 'node.condition=a?b:c'");
					} else {
						auto nodeElementNode = dynamic_cast<GUIElementNode*>(screenNode->getNodeById(nodeId));
						if (nodeElementNode != nullptr) {
							if (nodeElementNode->getActiveConditions().has(testCondition) == true) {
								nodeElementNode->getActiveConditions().removeAll();
								nodeElementNode->getActiveConditions().add(setOnTrueCondition);
							} else {
								nodeElementNode->getActiveConditions().removeAll();
								nodeElementNode->getActiveConditions().add(setOnFalseCondition);
							}
						}
					}
				} else {
					nodeElementNode->getActiveConditions().removeAll();
					nodeElementNode->getActiveConditions().add(value);
				}
			}
		} else
		if (subCommand == "condition-") {
			auto nodeElementNode = dynamic_cast<GUIElementNode*>(screenNode->getNodeById(nodeId));
			if (nodeElementNode != nullptr) nodeElementNode->getActiveConditions().remove(value);
		} else
		if (subCommand == "condition+") {
			auto nodeElementNode = dynamic_cast<GUIElementNode*>(screenNode->getNodeById(nodeId));
			if (nodeElementNode != nullptr) nodeElementNode->getActiveConditions().add(value);
		} else
		if (subCommand == "condition!") {
			auto nodeElementNode = dynamic_cast<GUIElementNode*>(screenNode->getNodeById(nodeId));
			if (nodeElementNode != nullptr) {
				if (nodeElementNode->getActiveConditions().has(value) == true) {
					nodeElementNode->getActiveConditions().remove(value);
				} else {
					nodeElementNode->getActiveConditions().add(value);
				}
			}
		} else
		if (subCommand == "condition?") {
			t2.tokenize(value, "?:");
			string testCondition = t2.hasMoreTokens() == true?t2.nextToken():"";
			string setOnTrueCondition = t2.hasMoreTokens() == true?t2.nextToken():"";
			string setOnFalseCondition = t2.hasMoreTokens() == true?t2.nextToken():"";
			if (value.find('?') == string::npos ||
				value.find(':') == string::npos ||
				value.find(':') < value.find('?') ||
				testCondition.empty() == true ||
				setOnTrueCondition.empty() == true ||
				setOnFalseCondition.empty() == true) {
				Console::println("GUIElementController::executeExpression(): ?= ternary operator requires the following format 'node.condition?=a?b:c'");
			}
			auto nodeElementNode = dynamic_cast<GUIElementNode*>(screenNode->getNodeById(nodeId));
			if (nodeElementNode != nullptr) {
				if (nodeElementNode->getActiveConditions().has(testCondition) == true) {
					nodeElementNode->getActiveConditions().remove(setOnFalseCondition);
					nodeElementNode->getActiveConditions().add(setOnTrueCondition);
				} else {
					nodeElementNode->getActiveConditions().remove(setOnTrueCondition);
					nodeElementNode->getActiveConditions().add(setOnFalseCondition);
				}
			}
		} else
		// image node specific data
		if (subCommand == "maskmaxvalue") {
			auto imageNode = dynamic_cast<GUIImageNode*>(screenNode->getNodeById(nodeId));
			if (StringUtils::endsWith(value, ".value") == true) {
				auto nodeValueElementNode = dynamic_cast<GUIElementNode*>(screenNode->getNodeById(StringUtils::substring(value, 0, value.length() - string(".value").size())));
				auto nodeValueController = nodeValueElementNode != nullptr?nodeValueElementNode->getController():nullptr;
				if (nodeValueController != nullptr) imageNode->setMaskMaxValue(Float::parseFloat(nodeValueController->getValue().getString()));
			} else {
				imageNode->setMaskMaxValue(Float::parseFloat(value));
			}
		} else {
			Console::println("GUIElementController::executeExpression(): Unknown sub command in expression: " + subCommand);
		}
	}
}

void GUIElementNode::executeOnChangeExpression() {
	if (onChangeExpression.size() > 0) executeExpression(onChangeExpression);
}

GUINodeConditions& GUIElementNode::getActiveConditions()
{
	return activeConditions;
}

void GUIElementNode::handleKeyboardEvent(GUIKeyboardEvent* event)
{
	if (ignoreEvents == true)
		return;

	if (conditionsMet == false)
		return;

	for (auto i = 0; i < subNodes.size(); i++) {
		auto subNode = subNodes[i];
		subNode->handleKeyboardEvent(event);
	}
	GUIParentNode::handleKeyboardEvent(event);
}

GUINode_Alignments GUIElementNode::createAlignments(const string& horizontal, const string& vertical)
{
	GUINode_Alignments alignments;
	alignments.horizontal = GUINode_AlignmentHorizontal::valueOf(horizontal.empty() == false && horizontal.length() > 0 ? StringUtils::toUpperCase(horizontal) : "LEFT");
	alignments.vertical = GUINode_AlignmentVertical::valueOf(vertical.empty() == false && vertical.length() > 0 ? StringUtils::toUpperCase(vertical) : "TOP");
	return alignments;
}
