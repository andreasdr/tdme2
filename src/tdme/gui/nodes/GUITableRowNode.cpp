#include <tdme/gui/nodes/GUITableRowNode.h>

#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUINode_Alignments.h>
#include <tdme/gui/nodes/GUINode_AlignmentHorizontal.h>
#include <tdme/gui/nodes/GUINode_AlignmentVertical.h>
#include <tdme/gui/nodes/GUINode_Border.h>
#include <tdme/gui/nodes/GUINode_ComputedConstraints.h>
#include <tdme/gui/nodes/GUINode_Flow.h>
#include <tdme/gui/nodes/GUINode_Padding.h>
#include <tdme/gui/nodes/GUINode_RequestedConstraints.h>
#include <tdme/gui/nodes/GUINode_RequestedConstraints_RequestedConstraintsType.h>
#include <tdme/gui/nodes/GUINode_Scale9Grid.h>
#include <tdme/gui/nodes/GUIParentNode.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/gui/nodes/GUITableNode.h>
#include <tdme/gui/GUI.h>
#include <tdme/utilities/StringTools.h>

using tdme::gui::nodes::GUITableRowNode;

using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINode_Alignments;
using tdme::gui::nodes::GUINode_AlignmentHorizontal;
using tdme::gui::nodes::GUINode_AlignmentVertical;
using tdme::gui::nodes::GUINode_Border;
using tdme::gui::nodes::GUINode_ComputedConstraints;
using tdme::gui::nodes::GUINode_Flow;
using tdme::gui::nodes::GUINode_Padding;
using tdme::gui::nodes::GUINode_RequestedConstraints;
using tdme::gui::nodes::GUINode_RequestedConstraints_RequestedConstraintsType;
using tdme::gui::nodes::GUINode_Scale9Grid;
using tdme::gui::nodes::GUIParentNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::nodes::GUITableNode;
using tdme::gui::GUI;
using tdme::utilities::StringTools;

GUITableRowNode::GUITableRowNode(
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
	const GUINode_Scale9Grid& backgroundImageScale9Grid,
	const GUIColor& backgroundImageEffectColorMul,
	const GUIColor& backgroundImageEffectColorAdd,
	const GUINode_Border& border,
	const GUINode_Padding& padding,
	const GUINodeConditions& showOn,
	const GUINodeConditions& hideOn
):
	GUIParentNode(screenNode, parentNode, id, flow, overflowX, overflowY, alignments, requestedConstraints, backgroundColor, backgroundImage, backgroundImageScale9Grid, backgroundImageEffectColorMul, backgroundImageEffectColorAdd, border, padding, showOn, hideOn)
{
}

const string GUITableRowNode::getNodeType()
{
	return "table-row";
}

bool GUITableRowNode::isContentNode()
{
	return false;
}

int GUITableRowNode::getContentWidth()
{
	auto width = 0;
	for (auto i = 0; i < subNodes.size(); i++) {
		auto guiSubNode = subNodes[i];
		if (guiSubNode->conditionsMet == false) continue;
		if (guiSubNode->flow == GUINode_Flow::FLOATING) {
			continue;
		}
		width += guiSubNode->getAutoWidth();
	}
	width += border.left + border.right;
	width += padding.left + padding.right;
	return width;
}

int GUITableRowNode::getContentHeight()
{
	auto height = 0;
	for (auto i = 0; i < subNodes.size(); i++) {
		auto guiSubNode = subNodes[i];
		if (guiSubNode->conditionsMet == false) continue;
		if (guiSubNode->flow == GUINode_Flow::FLOATING) {
			continue;
		}
		auto contentHeight = guiSubNode->getAutoHeight();
		if (contentHeight > height) {
			height = contentHeight;
		}
	}
	height += border.top + border.bottom;
	height += padding.top + padding.bottom;
	return height;
}

void GUITableRowNode::layoutSubNodes()
{
	GUIParentNode::layoutSubNodes();
	if (conditionsMet == false) {
		screenNode->forceInvalidateLayout(this);
		return;
	}
	GUITableNode* guiTableNode = required_dynamic_cast<GUITableNode*>(parentNode);
	if (tableRowNodeIdx == -1) {
		tableRowNodeIdx = 0;
		for (auto guiTableNowChild: guiTableNode->subNodes) {
			if (this == guiTableNowChild) break;
			tableRowNodeIdx++;
		}
	}
	auto tableCellMaxHeight = guiTableNode->getTableCellMaxHeight(tableRowNodeIdx);
	{
		auto starCount = 0;
		auto width = computedConstraints.width - border.left - border.right - padding.left - padding.right;
		auto nodesWidth = 0;
		auto finalNodesWidth = 0;
		for (auto i = 0; i < subNodes.size(); i++) {
			auto guiSubNode = subNodes[i];
			if (guiSubNode->conditionsMet == false) continue;
			if (guiSubNode->flow == GUINode_Flow::FLOATING) {
				continue;
			}
			if (guiSubNode->requestedConstraints.widthType == GUINode_RequestedConstraints_RequestedConstraintsType::STAR) {
				starCount++;
			} else {
				nodesWidth += guiSubNode->computedConstraints.width;
				finalNodesWidth += guiSubNode->computedConstraints.width;
			}
		}

		auto horizontalStarPixelRest = 0.0f;
		for (auto i = 0; i < subNodes.size(); i++) {
			auto guiSubNode = subNodes[i];
			if (guiSubNode->conditionsMet == false) continue;

			if (guiSubNode->requestedConstraints.widthType == GUINode_RequestedConstraints_RequestedConstraintsType::TABLECELL) {
				auto tableCellMaxWidth = guiTableNode->getTableCellMaxWidth(i);
				guiSubNode->requestedConstraints.widthType = GUINode_RequestedConstraints_RequestedConstraintsType::TABLECELL;
				guiSubNode->requestedConstraints.width = tableCellMaxWidth;
				guiSubNode->computedConstraints.width = tableCellMaxWidth;
				finalNodesWidth += guiSubNode->computedConstraints.width;
			}

			if (guiSubNode->requestedConstraints.heightType == GUINode_RequestedConstraints_RequestedConstraintsType::TABLECELL) {
				guiSubNode->requestedConstraints.height = tableCellMaxHeight;
				guiSubNode->computedConstraints.height = tableCellMaxHeight;
			}

			if (dynamic_cast<GUIParentNode*>(guiSubNode) != nullptr) {
				required_dynamic_cast<GUIParentNode*>(guiSubNode)->layoutSubNodes();
			}
		}

		auto v2 = alignments.horizontal;
		if (v2 == GUINode_AlignmentHorizontal::LEFT) {
			for (auto i = 0; i < subNodes.size(); i++) {
				auto guiSubNode = subNodes[i];
				if (guiSubNode->conditionsMet == false) continue;
				guiSubNode->computedConstraints.alignmentLeft = border.left + padding.left;
			}
		} else
		if (v2 == GUINode_AlignmentHorizontal::CENTER) {
			for (auto i = 0; i < subNodes.size(); i++) {
				auto guiSubNode = subNodes[i];
				if (guiSubNode->conditionsMet == false) continue;
				guiSubNode->computedConstraints.alignmentLeft = border.left + padding.left + ((width - finalNodesWidth) / 2);
			}
		} else
		if (v2 == GUINode_AlignmentHorizontal::RIGHT) {
			for (auto i = 0; i < subNodes.size(); i++) {
				auto guiSubNode = subNodes[i];
				if (guiSubNode->conditionsMet == false) continue;
				guiSubNode->computedConstraints.alignmentLeft = (width - finalNodesWidth);
			}
		}

		computeVerticalChildrenAlignment();
	}

	for (auto i = 0; i < subNodes.size(); i++) {
		auto guiSubNode = subNodes[i];
		guiSubNode->computeContentAlignment();
	}
	setTop(computedConstraints.top);
	setLeft(computedConstraints.left);
}

void GUITableRowNode::setTop(int top)
{
	if (requestedConstraints.topType == GUINode_RequestedConstraints_RequestedConstraintsType::PIXEL) top = requestedConstraints.top;
	GUIParentNode::setTop(top);
	top += computedConstraints.alignmentTop;
	for (auto i = 0; i < subNodes.size(); i++) {
		auto guiSubNode = subNodes[i];
		if (guiSubNode->conditionsMet == false) continue;
		guiSubNode->setTop(top);
		continue;
		top += guiSubNode->computedConstraints.height;
	}
}

void GUITableRowNode::setLeft(int left)
{
	if (requestedConstraints.leftType == GUINode_RequestedConstraints_RequestedConstraintsType::PIXEL) left = requestedConstraints.left;
	GUIParentNode::setLeft(left);
	left += computedConstraints.alignmentLeft;
	for (auto i = 0; i < subNodes.size(); i++) {
		auto guiSubNode = subNodes[i];
		if (guiSubNode->conditionsMet == false) continue;
		guiSubNode->setLeft(left);
		if (guiSubNode->flow == GUINode_Flow::FLOATING) {
			continue;
		}
		left += guiSubNode->computedConstraints.width;
	}
}


GUINode_RequestedConstraints GUITableRowNode::createRequestedConstraints(const string& left, const string& top, const string& width, const string& height, int factor)
{
	GUINode_RequestedConstraints constraints;
	constraints.leftType = getRequestedConstraintsType(StringTools::trim(left), GUINode_RequestedConstraints_RequestedConstraintsType::NONE);
	constraints.left = getRequestedConstraintsValue(StringTools::trim(left), 0);
	constraints.topType = getRequestedConstraintsType(StringTools::trim(top), GUINode_RequestedConstraints_RequestedConstraintsType::NONE);
	constraints.top = getRequestedConstraintsValue(StringTools::trim(top), 0);
	constraints.widthType = getRequestedConstraintsType(StringTools::trim(width), GUINode_RequestedConstraints_RequestedConstraintsType::AUTO);
	constraints.width = getRequestedConstraintsValue(StringTools::trim(width), -1);
	constraints.heightType = getRequestedConstraintsType(StringTools::trim(height), GUINode_RequestedConstraints_RequestedConstraintsType::AUTO);
	constraints.height = getRequestedConstraintsValue(StringTools::trim(height), -1);
	if (constraints.leftType == GUINode_RequestedConstraints_RequestedConstraintsType::PIXEL) constraints.left*= factor;
	if (constraints.topType == GUINode_RequestedConstraints_RequestedConstraintsType::PIXEL) constraints.top*= factor;
	if (constraints.widthType == GUINode_RequestedConstraints_RequestedConstraintsType::PIXEL) constraints.width*= factor;
	if (constraints.heightType == GUINode_RequestedConstraints_RequestedConstraintsType::PIXEL) constraints.height*= factor;
	return constraints;
}
