#include <tdme/gui/nodes/GUINodeConditions.h>

#include <algorithm>
#include <vector>

#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUINode_RequestedConstraints_RequestedConstraintsType.h>
#include <tdme/gui/nodes/GUIScreenNode.h>

using std::find;
using std::vector;

using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINodeConditions;
using tdme::gui::nodes::GUINode_RequestedConstraints_RequestedConstraintsType;
using tdme::gui::nodes::GUIScreenNode;

GUINodeConditions::GUINodeConditions(GUIElementNode* elementNode): elementNode(elementNode)
{
}

const vector<string>& GUINodeConditions::getConditions( ) const
{
	return conditions;
}

bool GUINodeConditions::has(const string& condition) const {
	return find(conditions.begin(), conditions.end(), condition) != conditions.end();
}

void GUINodeConditions::set(const string& condition) {
	this->conditions = {{ condition }};
	updateElementNode(conditions);
}

void GUINodeConditions::set(const vector<string>& conditions) {
	this->conditions = conditions;
	updateElementNode(conditions);
}

bool GUINodeConditions::add(const string& condition)
{
	auto conditionsChanged = has(condition) == false;
	if (conditionsChanged == true) conditions.push_back(condition);
	if (conditionsChanged == true) updateElementNode({condition});
	return conditionsChanged;
}

bool GUINodeConditions::remove(const string& condition)
{
	auto conditionsChanged = has(condition);
	conditions.erase(std::remove(conditions.begin(), conditions.end(), condition), conditions.end());
	if (conditionsChanged == true) updateElementNode({});
	return conditionsChanged;
}

bool GUINodeConditions::removeAll()
{
	auto conditionsChanged = conditions.empty() == false;
	conditions.clear();
	if (conditionsChanged == true) updateElementNode({});
	return conditionsChanged;
}

void GUINodeConditions::updateNode(GUINode* node, const vector<string>& conditions) const {
	node->conditionsMet = node->checkConditions();
	node->onSetConditions(conditions);
	auto parentNode = dynamic_cast<GUIParentNode*>(node);
	if (parentNode != nullptr) {
		for (auto i = 0; i < parentNode->subNodes.size(); i++) {
			auto guiSubNode = parentNode->subNodes[i];
			updateNode(guiSubNode, conditions);
		}
	}
}

void GUINodeConditions::updateElementNode(const vector<string>& conditions) const {
	if (elementNode == nullptr) return;

	// update related element node
	elementNode->onSetConditions(conditions);
	for (auto i = 0; i < elementNode->subNodes.size(); i++) {
		auto guiSubNode = elementNode->subNodes[i];
		updateNode(guiSubNode, conditions);
	}
	auto screenNode = elementNode->getScreenNode();
	screenNode->invalidateLayout(elementNode);

	// also update nodes that belong to this conditions but are not with in sub tree of related element node
	auto elementNodeToNodeMappingIt = screenNode->elementNodeToNodeMapping.find(elementNode->getId());
	if (elementNodeToNodeMappingIt != screenNode->elementNodeToNodeMapping.end()) {
		for (auto& nodeId: elementNodeToNodeMappingIt->second) {
			auto node = screenNode->getNodeById(nodeId);
			if (node == nullptr) continue;
			updateNode(node, conditions);
			screenNode->invalidateLayout(node->getParentNode() != nullptr?node->getParentNode():node);
		}
	}
}
