#include <tdme/gui/nodes/GUIScreenNode.h>

#include <algorithm>
#include <map>
#include <set>
#include <string>

#include <tdme/gui/events/GUIActionListener.h>
#include <tdme/gui/events/GUIChangeListener.h>
#include <tdme/gui/events/GUIFocusListener.h>
#include <tdme/gui/events/GUIInputEventHandler.h>
#include <tdme/gui/events/GUIMouseOverListener.h>
#include <tdme/gui/nodes/GUIElementController.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUINode_ComputedConstraints.h>
#include <tdme/gui/nodes/GUINode_RequestedConstraints.h>
#include <tdme/gui/nodes/GUINode_RequestedConstraints_RequestedConstraintsType.h>
#include <tdme/gui/nodes/GUINode_Scale9Grid.h>
#include <tdme/gui/nodes/GUINodeController.h>
#include <tdme/gui/nodes/GUIParentNode.h>
#include <tdme/gui/nodes/GUIScreenNode_SizeConstraints.h>
#include <tdme/gui/renderer/GUIRenderer.h>
#include <tdme/gui/GUI.h>
#include <tdme/utilities/Integer.h>
#include <tdme/utilities/MutableString.h>

using std::map;
using std::remove;
using std::reverse;
using std::set;
using std::string;
using std::to_string;

using tdme::gui::events::GUIActionListener;
using tdme::gui::events::GUIChangeListener;
using tdme::gui::events::GUIInputEventHandler;
using tdme::gui::events::GUIMouseOverListener;
using tdme::gui::nodes::GUIElementController;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINode_ComputedConstraints;
using tdme::gui::nodes::GUINode_RequestedConstraints;
using tdme::gui::nodes::GUINode_RequestedConstraints_RequestedConstraintsType;
using tdme::gui::nodes::GUINode_Scale9Grid;
using tdme::gui::nodes::GUINodeController;
using tdme::gui::nodes::GUIParentNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::nodes::GUIScreenNode_SizeConstraints;
using tdme::gui::renderer::GUIRenderer;
using tdme::gui::GUI;
using tdme::utilities::Integer;
using tdme::utilities::MutableString;

GUIScreenNode::GUIScreenNode(
	const string& applicationRootPathName,
	const string& applicationSubPathName,
	const string& id,
	GUINode_Flow* flow,
	GUIParentNode_Overflow* overflowX,
	GUIParentNode_Overflow* overflowY,
	const GUINode_Alignments& alignments,
	const GUINode_RequestedConstraints& requestedConstraints,
	const GUIColor& backgroundColor,
	const string& backgroundImage,
	const GUINode_Scale9Grid& backgroundImageScale9Grid,
	const GUIColor& backgroundImageEffectColorMul,
	const GUIColor& backgroundImageEffectColorAdd,
	const GUINode_Border& border,
	const GUINode_Padding& padding,
	const GUIScreenNode_SizeConstraints& sizeConstraints,
	const GUINodeConditions& showOn,
	const GUINodeConditions& hideOn,
	bool scrollable,
	bool popUp
):
	GUIParentNode(this, nullptr, id, flow, overflowX, overflowY, alignments, requestedConstraints, backgroundColor, backgroundImage, backgroundImageScale9Grid, backgroundImageEffectColorMul, backgroundImageEffectColorAdd, border, padding, showOn, hideOn)
{
	this->applicationRootPathName = applicationRootPathName;
	this->applicationSubPathName = applicationSubPathName;
	this->sizeConstraints = sizeConstraints;
	this->gui = nullptr;
	this->nodeCounter = 0;
	this->screenWidth = 0;
	this->screenHeight = 0;
	this->inputEventHandler = nullptr;
	this->screenNode = this;
	this->parentNode = nullptr;
	this->visible = true;
	this->popUp = popUp;
	this->reshapeRequested = false;
}

GUIScreenNode::~GUIScreenNode() {
	// remove sub nodes
	for (auto i = 0; i < subNodes.size(); i++) {
		removeNode(subNodes[i]);
	}
	subNodes.clear();

	// dispose
	GUINode::dispose();
}

GUI* GUIScreenNode::getGUI()
{
	return gui;
}

void GUIScreenNode::setGUI(GUI* gui)
{
	this->gui = gui;
}

void GUIScreenNode::setVisible(bool visible)
{
	this->visible = visible;
	if (gui != nullptr)
		gui->invalidateFocussedNode();

}

void GUIScreenNode::setPopUp(bool popUp)
{
	this->popUp = popUp;
}

const vector<GUINode*>& GUIScreenNode::getFloatingNodes()
{
	return floatingNodes;
}

bool GUIScreenNode::isContentNode()
{
	return false;
}

int GUIScreenNode::getContentWidth()
{
	return -1;
}

int GUIScreenNode::getContentHeight()
{
	return -1;
}

void GUIScreenNode::layout()
{
	for (auto i = 0; i < subNodes.size(); i++) {
		subNodes[i]->layout();
	}
	getChildControllerNodes(childControllerNodes);
	for (auto i = 0; i < childControllerNodes.size(); i++) {
		auto node = childControllerNodes[i];
		auto controller = node->getController();
		if (controller != nullptr && node->layouted == true) {
			controller->postLayout();
		}
	}
	layouted = true;
}

GUINode* GUIScreenNode::forceInvalidateLayout(GUINode* node) {
	// check if parent nodes have conditions met
	{
		auto _node = node;
		while (_node != nullptr) {
			if (_node->conditionsMet == false) {
				return nullptr;
			}
			_node = _node->parentNode;
		}
	}

	// first step, make sure all parents up to screen node are layouted
	auto _node = node;
	auto __node = node;
	while (_node->parentNode != nullptr) {
		if (_node->layouted == false) __node = _node;
		_node = _node->parentNode;
	}
	_node = __node;

	// invalidate all nodes from node to _node
	for (__node = node; __node != _node; __node = __node->getParentNode()) {
		__node->layouted = false;
	}

	// find a node that is a valid base for layouting from
	while (
		_node->parentNode != nullptr &&
		// auto depends on its children dimensions, so do relayout the parent
		((_node->requestedConstraints.leftType == GUINode_RequestedConstraints_RequestedConstraintsType::AUTO ||
		_node->requestedConstraints.topType == GUINode_RequestedConstraints_RequestedConstraintsType::AUTO ||
		_node->requestedConstraints.widthType == GUINode_RequestedConstraints_RequestedConstraintsType::AUTO ||
		_node->requestedConstraints.heightType == GUINode_RequestedConstraints_RequestedConstraintsType::AUTO) ||
		// percent depend on its parent dimensions so make sure its already layouted
		(_node->layouted == false &&
		(_node->requestedConstraints.leftType == GUINode_RequestedConstraints_RequestedConstraintsType::PERCENT ||
		_node->requestedConstraints.topType == GUINode_RequestedConstraints_RequestedConstraintsType::PERCENT ||
		_node->requestedConstraints.widthType == GUINode_RequestedConstraints_RequestedConstraintsType::PERCENT ||
		_node->requestedConstraints.heightType == GUINode_RequestedConstraints_RequestedConstraintsType::PERCENT)) ||
		// star depend on its parent dimensions, so make sure its already layouted
		(_node->layouted == false &&
		(_node->requestedConstraints.leftType == GUINode_RequestedConstraints_RequestedConstraintsType::STAR ||
		_node->requestedConstraints.topType == GUINode_RequestedConstraints_RequestedConstraintsType::STAR ||
		_node->requestedConstraints.widthType == GUINode_RequestedConstraints_RequestedConstraintsType::STAR ||
		_node->requestedConstraints.heightType == GUINode_RequestedConstraints_RequestedConstraintsType::STAR)))) {
		_node->layouted = false;
		_node = _node->parentNode;
	}

	_node->layouted = false;

	//
	auto parentNode = dynamic_cast<GUIParentNode*>(_node);
	if (parentNode != nullptr) parentNode->invalidateRenderCaches();

	//
	return _node;
}

void GUIScreenNode::invalidateLayouts() {
	for (auto& nodeId: invalidateLayoutNodeIds) {
		auto node = getNodeById(nodeId);
		if (node != nullptr) forceInvalidateLayout(node);
	}
	invalidateLayoutNodeIds.clear();
}

void GUIScreenNode::layout(GUINode* node)
{
	auto _node = forceInvalidateLayout(node);
	if (_node == nullptr) {
		return;
	}

	//
	forceLayout(_node);
}

void GUIScreenNode::forceLayout(GUINode* node)
{
	// do the magic
	if (dynamic_cast<GUIParentNode*>(node) != nullptr) {
		auto parentNode = required_dynamic_cast<GUIParentNode*>(node);
		parentNode->layouted = true;
		parentNode->layoutSubNodes();
		parentNode->layoutSubNodes();
		parentNode->getScreenNode()->getChildControllerNodes(childControllerNodes, true);
		for (auto i = 0; i < childControllerNodes.size(); i++) {
			auto childNode = childControllerNodes[i];
			auto childController = childNode->getController();
			if (childController != nullptr) childController->postLayout();
		}
	} else {
		node->layout();
		node->getScreenNode()->layoutSubNodes();
		node->computeContentAlignment();
		auto nodeController = node->getController();
		if (nodeController != nullptr) nodeController->postLayout();
	}
}

void GUIScreenNode::setScreenSize(int width, int height)
{
	this->screenWidth = width;
	this->screenHeight = height;
	this->requestedConstraints.widthType = GUINode_RequestedConstraints_RequestedConstraintsType::PIXEL;
	this->requestedConstraints.width = width;
	this->requestedConstraints.heightType = GUINode_RequestedConstraints_RequestedConstraintsType::PIXEL;
	this->requestedConstraints.height = height;
	this->computedConstraints.left = 0;
	this->computedConstraints.top = 0;
	this->computedConstraints.width = width;
	this->computedConstraints.height = height;
	this->layouted = false;
}


const string GUIScreenNode::getNodeType()
{
	return "screen";
}

GUINode* GUIScreenNode::getNodeById(const string& nodeId)
{
	auto nodesByIdIt = nodesById.find(nodeId);
	if (nodesByIdIt == nodesById.end()) {
		return nullptr;
	}
	return nodesByIdIt->second;
}

const string GUIScreenNode::allocateNodeId()
{
	return "tdme_gui_anonymous_node_" + to_string(nodeCounter++);
}

bool GUIScreenNode::addNode(GUINode* node)
{
	// if node does exist do not insert it and return
	if (nodesById.find(node->id) != nodesById.end()) {
		return false;
	}
	// otherwise go
	nodesById[node->id] = node;

	// add to floating nodes
	if (node->flow == GUINode_Flow::FLOATING) floatingNodes.push_back(node);

	return true;
}

bool GUIScreenNode::removeNode(GUINode* node)
{
	{
		auto elementNodeToNodeMappingIt = elementNodeToNodeMapping.find(node->getId());
		if (elementNodeToNodeMappingIt != elementNodeToNodeMapping.end()) {
			elementNodeToNodeMapping.erase(elementNodeToNodeMappingIt);
		}
	}
	{
		for (auto& elementNodeToNodeMappingIt: elementNodeToNodeMapping) {
			elementNodeToNodeMappingIt.second.erase(node->getId());
		}
	}
	if (dynamic_cast<GUIParentNode*>(node) != nullptr) {
		auto parentNode = required_dynamic_cast<GUIParentNode*>(node);
		for (auto i = 0; i < parentNode->subNodes.size(); i++) {
			removeNode(parentNode->subNodes[i]);
		}
		parentNode->subNodes.clear();
	}
	nodesById.erase(node->id);
	tickNodesById.erase(node->getId());
	floatingNodes.erase(remove(floatingNodes.begin(), floatingNodes.end(), node), floatingNodes.end());
	node->dispose();
	delete node;
	return true;
}

void GUIScreenNode::render(GUIRenderer* guiRenderer)
{
	guiRenderer->initScreen(this);
	GUIParentNode::layoutOnDemand();
	GUIParentNode::render(guiRenderer);
	guiRenderer->doneScreen();
}

void GUIScreenNode::renderFloatingNodes(GUIRenderer* guiRenderer)
{
	guiRenderer->initScreen(this);
	for (auto i = 0; i < floatingNodes.size(); i++) {
		guiRenderer->setRenderAreaLeft(GUIRenderer::SCREEN_LEFT);
		guiRenderer->setRenderAreaTop(GUIRenderer::SCREEN_TOP);
		guiRenderer->setRenderAreaRight(GUIRenderer::SCREEN_RIGHT);
		guiRenderer->setRenderAreaBottom(GUIRenderer::SCREEN_BOTTOM);
		floatingNodes[i]->render(guiRenderer);
	}
	guiRenderer->doneScreen();
}

void GUIScreenNode::determineFocussedNodes(GUIParentNode* parentNode, vector<GUIElementNode*>& focusableNodes)
{
	if (parentNode->conditionsMet == false) {
		return;
	}
	if (dynamic_cast<GUIElementNode*>(parentNode) != nullptr) {
		auto parentElementNode = required_dynamic_cast<GUIElementNode*>(parentNode);
		if (parentElementNode->focusable == true && (parentElementNode->getController() == nullptr || parentElementNode->getController()->isDisabled() == false)) {
			focusableNodes.push_back(required_dynamic_cast<GUIElementNode*>(parentNode));
		}
	}
	for (auto i = 0; i < parentNode->subNodes.size(); i++) {
		auto subNode = parentNode->subNodes[i];
		if (dynamic_cast<GUIParentNode*>(subNode) != nullptr) {
			determineFocussedNodes(required_dynamic_cast<GUIParentNode*>(subNode), focusableNodes);
		}
	}
}

void GUIScreenNode::determineMouseEventNodes(GUIMouseEvent* event, bool floatingNode, set<string>& eventNodeIds, set<string>& eventFloatingNodeIds)
{
	for (auto i = 0; i < floatingNodes.size(); i++) {
		floatingNodes[i]->determineMouseEventNodes(event, floatingNode == true || flow == GUINode_Flow::FLOATING, eventNodeIds, eventFloatingNodeIds);
	}
	GUIParentNode::determineMouseEventNodes(event, floatingNode, eventNodeIds, eventFloatingNodeIds);
}

void GUIScreenNode::addActionListener(GUIActionListener* listener)
{
	removeActionListener(listener);
	actionListener.push_back(listener);
}

void GUIScreenNode::removeActionListener(GUIActionListener* listener)
{
	actionListener.erase(std::remove(actionListener.begin(), actionListener.end(), listener), actionListener.end());
}

GUIInputEventHandler* GUIScreenNode::getInputEventHandler()
{
	return inputEventHandler;
}

void GUIScreenNode::setInputEventHandler(GUIInputEventHandler* inputEventHandler)
{
	this->inputEventHandler = inputEventHandler;
}

void GUIScreenNode::delegateActionPerformed(GUIActionListenerType type, GUIElementNode* node)
{
	for (auto i = 0; i < actionListener.size(); i++) {
		actionListener[i]->onActionPerformed(type, node);
	}
}

void GUIScreenNode::addChangeListener(GUIChangeListener* listener)
{
	removeChangeListener(listener);
	changeListener.push_back(listener);
}

void GUIScreenNode::removeChangeListener(GUIChangeListener* listener)
{
	changeListener.erase(std::remove(changeListener.begin(), changeListener.end(), listener), changeListener.end());
}

void GUIScreenNode::delegateValueChanged(GUIElementNode* node)
{
	node->executeOnChangeExpression();
	for (auto i = 0; i < changeListener.size(); i++) {
		changeListener[i]->onValueChanged(node);
	}
}

void GUIScreenNode::addMouseOverListener(GUIMouseOverListener* listener)
{
	removeMouseOverListener(listener);
	mouseOverListener.push_back(listener);
}

void GUIScreenNode::removeMouseOverListener(GUIMouseOverListener* listener)
{
	mouseOverListener.erase(std::remove(mouseOverListener.begin(), mouseOverListener.end(), listener), mouseOverListener.end());
}

void GUIScreenNode::delegateMouseOver(GUIElementNode* node)
{
	for (auto i = 0; i < mouseOverListener.size(); i++) {
		mouseOverListener[i]->onMouseOver(node);
	}
}

void GUIScreenNode::addFocusListener(GUIFocusListener* listener)
{
	removeFocusListener(listener);
	focusListener.push_back(listener);
}

void GUIScreenNode::removeFocusListener(GUIFocusListener* listener)
{
	focusListener.erase(std::remove(focusListener.begin(), focusListener.end(), listener), focusListener.end());
}

void GUIScreenNode::delegateFocus(GUIElementNode* node) {
	for (auto i = 0; i < focusListener.size(); i++) {
		focusListener[i]->onFocus(node);
	}
}

void GUIScreenNode::delegateUnfocus(GUIElementNode* node) {
	for (auto i = 0; i < focusListener.size(); i++) {
		focusListener[i]->onUnfocus(node);
	}
}

void GUIScreenNode::addTickNode(GUINode* node) {
	tickNodesById[node->getId()] = node;
}

void GUIScreenNode::removeTickNode(GUINode* node) {
	tickNodesById.erase(node->getId());
}

void GUIScreenNode::tick() {
	auto now = Time::getCurrentMillis();
	vector<int64_t> timedExpressionsToRemove;
	for (auto& timedExpressionIt: timedExpressions) {
		if (now >= timedExpressionIt.first) {
			timedExpressionsToRemove.push_back(timedExpressionIt.first);
			GUIElementNode::executeExpression(this, timedExpressionIt.second);
		}
	}
	for (auto& timedExpressionToRemove: timedExpressionsToRemove) {
		timedExpressions.erase(timedExpressionToRemove);
	}
	auto _tickNodesById = tickNodesById;
	for (auto tickNodesByIdIt: _tickNodesById) {
		auto node = tickNodesByIdIt.second;
		if (node->controller != nullptr) node->controller->tick();
	}
}

void GUIScreenNode::getValues(map<string, MutableString>& values)
{
	values.clear();
	getChildControllerNodes(childControllerNodes);
	for (auto i = 0; i < childControllerNodes.size(); i++) {
		auto childControllerNode = childControllerNodes[i];
		if (dynamic_cast<GUIElementNode*>(childControllerNode) != nullptr == false)
			continue;

		auto guiElementNode = required_dynamic_cast<GUIElementNode*>(childControllerNode);
		auto guiElementNodeController = guiElementNode->getController();
		if (guiElementNodeController->hasValue()) {
			auto& name = guiElementNode->getName();
			auto& value = guiElementNodeController->getValue();
			auto currentValueIt = values.find(name);
			if (currentValueIt == values.end() || currentValueIt->second.length() == 0) {
				values[name] = value;
			}
		}
	}
}

void GUIScreenNode::setValues(const map<string, MutableString>& values)
{
	getChildControllerNodes(childControllerNodes);
	for (auto i = 0; i < childControllerNodes.size(); i++) {
		auto childControllerNode = childControllerNodes[i];
		if (dynamic_cast<GUIElementNode*>(childControllerNode) != nullptr == false)
			continue;

		auto guiElementNode = required_dynamic_cast<GUIElementNode*>(childControllerNode);
		auto guiElementNodeController = guiElementNode->getController();
		if (guiElementNodeController->hasValue()) {
			auto name = guiElementNode->getName();
			auto newValueIt = values.find(name);
			if (newValueIt == values.end())
				continue;
			guiElementNodeController->setValue(newValueIt->second);
		}
	}
}

GUIScreenNode_SizeConstraints GUIScreenNode::createSizeConstraints(const string& minWidth, const string& minHeight, const string& maxWidth, const string& maxHeight)
{
	GUIScreenNode_SizeConstraints constraints;
	constraints.minWidth = minWidth.empty() == true?-1:Integer::parseInt(minWidth);
	constraints.minHeight = minHeight.empty() == true?-1:Integer::parseInt(minHeight);
	constraints.maxWidth = maxWidth.empty() == true?-1:Integer::parseInt(maxWidth);
	constraints.maxHeight = maxHeight.empty() == true?-1:Integer::parseInt(maxHeight);
	return constraints;
}

void GUIScreenNode::addTimedExpression(int64_t time, const string& expression) {
	timedExpressions[time]+= timedExpressions[time].empty() == false?";" + expression:expression;
}

void GUIScreenNode::addNodeElementNodeDependency(const string& elementNodeId, const string& nodeId) {
	elementNodeToNodeMapping[elementNodeId].insert(nodeId);
}
