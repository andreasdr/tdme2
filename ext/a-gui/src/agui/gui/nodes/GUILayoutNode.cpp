#include <agui/gui/nodes/GUILayoutNode.h>

#include <agui/agui.h>
#include <agui/gui/nodes/GUILayoutNode_Alignment.h>
#include <agui/gui/nodes/GUINode.h>
#include <agui/gui/nodes/GUINode_Alignments.h>
#include <agui/gui/nodes/GUINode_AlignmentHorizontal.h>
#include <agui/gui/nodes/GUINode_AlignmentVertical.h>
#include <agui/gui/nodes/GUINode_Border.h>
#include <agui/gui/nodes/GUINode_ComputedConstraints.h>
#include <agui/gui/nodes/GUINode_Flow.h>
#include <agui/gui/nodes/GUINode_Padding.h>
#include <agui/gui/nodes/GUINode_RequestedConstraints.h>
#include <agui/gui/nodes/GUINode_RequestedConstraints_RequestedConstraintsType.h>
#include <agui/gui/nodes/GUINode_Scale9Grid.h>
#include <agui/gui/nodes/GUIParentNode.h>
#include <agui/gui/nodes/GUIScreenNode.h>
#include <agui/gui/GUI.h>
#include <agui/math/Math.h>
#include <agui/utilities/StringTools.h>

using agui::gui::nodes::GUILayoutNode;

using agui::gui::nodes::GUILayoutNode_Alignment;
using agui::gui::nodes::GUINode;
using agui::gui::nodes::GUINode_Alignments;
using agui::gui::nodes::GUINode_AlignmentHorizontal;
using agui::gui::nodes::GUINode_AlignmentVertical;
using agui::gui::nodes::GUINode_Border;
using agui::gui::nodes::GUINode_ComputedConstraints;
using agui::gui::nodes::GUINode_Flow;
using agui::gui::nodes::GUINode_Padding;
using agui::gui::nodes::GUINode_RequestedConstraints;
using agui::gui::nodes::GUINode_RequestedConstraints_RequestedConstraintsType;
using agui::gui::nodes::GUINode_Scale9Grid;
using agui::gui::nodes::GUIParentNode;
using agui::gui::nodes::GUIScreenNode;
using agui::gui::GUI;
using agui::math::Math;
using agui::utilities::StringTools;

GUILayoutNode::GUILayoutNode(
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
	const GUINodeConditions& hideOn,
	const string& tooltip,
	GUILayoutNode_Alignment* alignment
):
	GUIParentNode(screenNode, parentNode, id, flow, overflowX, overflowY, alignments, requestedConstraints, backgroundColor, backgroundImage, backgroundImageScale9Grid, backgroundImageEffectColorMul, backgroundImageEffectColorAdd, border, padding, showOn, hideOn, tooltip)
{
	this->alignment = alignment;
}

const string GUILayoutNode::getNodeType()
{
	return "layout";
}

bool GUILayoutNode::isContentNode()
{
	return false;
}

int GUILayoutNode::getContentWidth()
{
	auto width = 0;
	if (alignment == GUILayoutNode_Alignment::HORIZONTAL) {
		for (auto i = 0; i < subNodes.size(); i++) {
			auto guiSubNode = subNodes[i];
			if (guiSubNode->conditionsMet == false) continue;
			if (guiSubNode->flow == GUINode_Flow::FLOATING) {
				continue;
			}
			width += guiSubNode->getAutoWidth();
		}
	} else {
		for (auto i = 0; i < subNodes.size(); i++) {
			auto guiSubNode = subNodes[i];
			if (guiSubNode->conditionsMet == false) continue;
			if (guiSubNode->flow == GUINode_Flow::FLOATING) {
				continue;
			}
			auto contentWidth = guiSubNode->getAutoWidth();
			if (contentWidth > width) {
				width = contentWidth;
			}
		}
	}
	width += border.left + border.right;
	width += padding.left + padding.right;
	return width;
}

int GUILayoutNode::getContentHeight()
{
	auto height = 0;
	if (alignment == GUILayoutNode_Alignment::VERTICAL) {
		for (auto i = 0; i < subNodes.size(); i++) {
			auto guiSubNode = subNodes[i];
			if (guiSubNode->conditionsMet == false) continue;
			if (guiSubNode->flow == GUINode_Flow::FLOATING) {
				continue;
			}
			height += guiSubNode->getAutoHeight();
		}
	} else {
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
	}
	height += border.top + border.bottom;
	height += padding.top + padding.bottom;
	return height;
}

void GUILayoutNode::layoutSubNodes()
{
	if (conditionsMet == false) return;
	GUIParentNode::layoutSubNodes();

	//
	if (alignment == GUILayoutNode_Alignment::VERTICAL) {
		auto starCount = 0;
		auto width = computedConstraints.width - border.left - border.right - padding.left - padding.right;
		auto height = computedConstraints.height - border.top - border.bottom - padding.top - padding.bottom;
		auto nodesHeight = 0;
		auto finalNodesHeight = 0;
		for (auto i = 0; i < subNodes.size(); i++) {
			auto guiSubNode = subNodes[i];
			if (guiSubNode->conditionsMet == false) continue;
			if (guiSubNode->flow == GUINode_Flow::FLOATING) {
				continue;
			}
			if (guiSubNode->requestedConstraints.heightType == GUINode_RequestedConstraints_RequestedConstraintsType::STAR) {
				starCount++;
			} else {
				nodesHeight += guiSubNode->computedConstraints.height;
				finalNodesHeight += guiSubNode->computedConstraints.height;
			}
		}
		auto verticalStarPixelRest = 0.0f;
		for (auto i = 0; i < subNodes.size(); i++) {
			auto guiSubNode = subNodes[i];
			if (guiSubNode->conditionsMet == false) continue;
			if (guiSubNode->requestedConstraints.widthType == GUINode_RequestedConstraints_RequestedConstraintsType::STAR) {
				guiSubNode->requestedConstraints.width = width;
				guiSubNode->computedConstraints.width = width;
			}
			if (guiSubNode->requestedConstraints.heightType == GUINode_RequestedConstraints_RequestedConstraintsType::STAR) {
				auto nodeStarHeight = (static_cast<float>(height) - static_cast<float>(nodesHeight)) / static_cast<float>(starCount);
				auto nodeStarHeightInt = static_cast<int>(nodeStarHeight);
				verticalStarPixelRest += nodeStarHeight - nodeStarHeightInt;
				if (static_cast<int>(verticalStarPixelRest) > 0) {
					nodeStarHeightInt += static_cast<int>(verticalStarPixelRest);
					verticalStarPixelRest -= static_cast<int>(verticalStarPixelRest);
				}
				guiSubNode->requestedConstraints.height = nodeStarHeightInt;
				guiSubNode->computedConstraints.height = nodeStarHeightInt;
				if (guiSubNode->computedConstraints.height < 0) {
					guiSubNode->computedConstraints.height = 0;
				}
				finalNodesHeight += guiSubNode->computedConstraints.height;
				if (dynamic_cast<GUIParentNode*>(guiSubNode) != nullptr) {
					required_dynamic_cast<GUIParentNode*>(guiSubNode)->layoutSubNodes();
				}
			}
		}
		if (alignments.vertical == GUINode_AlignmentVertical::TOP) {
			for (auto i = 0; i < subNodes.size(); i++) {
				auto guiSubNode = subNodes[i];
				if (guiSubNode->conditionsMet == false) continue;
				guiSubNode->computedConstraints.alignmentTop = Math::max(0, border.top + padding.top);
			}
		} else
		if (alignments.vertical == GUINode_AlignmentVertical::CENTER) {
			for (auto i = 0; i < subNodes.size(); i++) {
				auto guiSubNode = subNodes[i];
				if (guiSubNode->conditionsMet == false) continue;
				guiSubNode->computedConstraints.alignmentTop = Math::max(0, border.top + padding.top + ((height - finalNodesHeight) / 2));
			}
		} else
		if (alignments.vertical == GUINode_AlignmentVertical::BOTTOM) {
			for (auto i = 0; i < subNodes.size(); i++) {
				auto guiSubNode = subNodes[i];
				if (guiSubNode->conditionsMet == false) continue;
				guiSubNode->computedConstraints.alignmentTop = Math::max(0, height - finalNodesHeight); // TODO: take bottom padding into account
			}
		}

		computeHorizontalChildrenAlignment();
	} else
	if (alignment == GUILayoutNode_Alignment::HORIZONTAL) {
		auto starCount = 0;
		auto width = computedConstraints.width - border.left - border.right - padding.left - padding.right;
		auto height = computedConstraints.height - border.top - border.bottom - padding.top - padding.bottom;
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
			if (guiSubNode->requestedConstraints.heightType == GUINode_RequestedConstraints_RequestedConstraintsType::STAR) {
				guiSubNode->requestedConstraints.height = height;
				guiSubNode->computedConstraints.height = height;
			}
			if (guiSubNode->requestedConstraints.widthType == GUINode_RequestedConstraints_RequestedConstraintsType::STAR) {
				auto nodeStarWidth = (static_cast<float>(width) - static_cast<float>(nodesWidth)) / static_cast<float>(starCount);
				auto nodeStarWidthInt = static_cast<int>(nodeStarWidth);
				horizontalStarPixelRest += nodeStarWidth - nodeStarWidthInt;
				if (static_cast<int>(horizontalStarPixelRest) > 0) {
					nodeStarWidthInt += static_cast<int>(horizontalStarPixelRest);
					horizontalStarPixelRest -= static_cast<int>(horizontalStarPixelRest);
				}
				guiSubNode->requestedConstraints.width = nodeStarWidthInt;
				guiSubNode->computedConstraints.width = nodeStarWidthInt;
				if (guiSubNode->computedConstraints.width < 0) {
					guiSubNode->computedConstraints.width = 0;
				}
				finalNodesWidth += guiSubNode->computedConstraints.width;
				if (dynamic_cast<GUIParentNode*>(guiSubNode) != nullptr) {
					required_dynamic_cast<GUIParentNode*>(guiSubNode)->layoutSubNodes();
				}
			}
		}

		if (alignments.horizontal == GUINode_AlignmentHorizontal::LEFT) {
			for (auto i = 0; i < subNodes.size(); i++) {
				auto guiSubNode = subNodes[i];
				if (guiSubNode->conditionsMet == false) continue;
				guiSubNode->computedConstraints.alignmentLeft = Math::max(0, border.left + padding.left);
			}
		} else
		if (alignments.horizontal == GUINode_AlignmentHorizontal::CENTER) {
			for (auto i = 0; i < subNodes.size(); i++) {
				auto guiSubNode = subNodes[i];
				if (guiSubNode->conditionsMet == false) continue;
				guiSubNode->computedConstraints.alignmentLeft = Math::max(0, border.left + padding.left + ((width - finalNodesWidth) / 2));
			}
		} else
		if (alignments.horizontal == GUINode_AlignmentHorizontal::RIGHT) {
			for (auto i = 0; i < subNodes.size(); i++) {
				auto guiSubNode = subNodes[i];
				if (guiSubNode->conditionsMet == false) continue;
				guiSubNode->computedConstraints.alignmentLeft = Math::max(0, width - finalNodesWidth); // TODO: take right padding into account
			}
		}

		computeVerticalChildrenAlignment();
	} else {
		computeHorizontalChildrenAlignment();
		computeVerticalChildrenAlignment();
	}

	for (auto i = 0; i < subNodes.size(); i++) {
		auto guiSubNode = subNodes[i];
		guiSubNode->computeContentAlignment();
	}
	setTop(computedConstraints.top);
	setLeft(computedConstraints.left);
}

void GUILayoutNode::setTop(int top)
{
	if (requestedConstraints.topType == GUINode_RequestedConstraints_RequestedConstraintsType::PIXEL) top = requestedConstraints.top;
	GUIParentNode::setTop(top);
	top += computedConstraints.alignmentTop;
	for (auto i = 0; i < subNodes.size(); i++) {
		auto guiSubNode = subNodes[i];
		if (guiSubNode->conditionsMet == false) continue;
		guiSubNode->setTop(top);
		if (alignment != GUILayoutNode_Alignment::VERTICAL || guiSubNode->flow == GUINode_Flow::FLOATING) {
			continue;
		}
		top += guiSubNode->computedConstraints.height;
	}
}

void GUILayoutNode::setLeft(int left)
{
	if (requestedConstraints.leftType == GUINode_RequestedConstraints_RequestedConstraintsType::PIXEL) left = requestedConstraints.left;
	GUIParentNode::setLeft(left);
	left += computedConstraints.alignmentLeft;
	for (auto i = 0; i < subNodes.size(); i++) {
		auto guiSubNode = subNodes[i];
		if (guiSubNode->conditionsMet == false) continue;
		guiSubNode->setLeft(left);
		if (alignment != GUILayoutNode_Alignment::HORIZONTAL || guiSubNode->flow == GUINode_Flow::FLOATING) {
			continue;
		}
		left += guiSubNode->computedConstraints.width;
	}
}

GUILayoutNode_Alignment* GUILayoutNode::createAlignment(const string& alignment)
{
	return GUILayoutNode_Alignment::valueOf(alignment.empty() == false && alignment.length() > 0 ? StringTools::toUpperCase(alignment) : "NONE");
}

