#include <tdme/gui/nodes/GUITableCellNode.h>

#include <set>
#include <string>

#include <tdme/gui/events/GUIMouseEvent.h>
#include <tdme/gui/nodes/GUINode_RequestedConstraints.h>
#include <tdme/gui/nodes/GUINode_RequestedConstraints_RequestedConstraintsType.h>
#include <tdme/utilities/StringTools.h>

using std::set;
using std::string;

using tdme::gui::events::GUIMouseEvent;
using tdme::gui::nodes::GUINode_RequestedConstraints;
using tdme::gui::nodes::GUITableCellNode;
using tdme::utilities::StringTools;

GUITableCellNode::GUITableCellNode(
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
	GUILayoutNode_Alignment* alignment
) :
	GUILayoutNode(screenNode, parentNode, id, flow, overflowX, overflowY, alignments, requestedConstraints, backgroundColor, backgroundImage, backgroundImageScale9Grid, backgroundImageEffectColorMul, backgroundImageEffectColorAdd, border, padding, showOn, hideOn, alignment)
{
}

const string GUITableCellNode::getNodeType()
{
	return "table-cell";
}

GUINode_RequestedConstraints GUITableCellNode::createRequestedConstraints(const string& left, const string& top, const string& width, const string& height, int factor)
{
	GUINode_RequestedConstraints constraints;
	constraints.leftType = getRequestedConstraintsType(StringTools::trim(left), GUINode_RequestedConstraints_RequestedConstraintsType::NONE);
	constraints.left = getRequestedConstraintsValue(StringTools::trim(left), 0);
	constraints.topType = getRequestedConstraintsType(StringTools::trim(top), GUINode_RequestedConstraints_RequestedConstraintsType::NONE);
	constraints.top = getRequestedConstraintsValue(StringTools::trim(top), 0);
	constraints.widthType = getRequestedConstraintsType(StringTools::trim(width), GUINode_RequestedConstraints_RequestedConstraintsType::TABLECELL);
	constraints.width = getRequestedConstraintsValue(StringTools::trim(width), -1);
	constraints.heightType = getRequestedConstraintsType(StringTools::trim(height), GUINode_RequestedConstraints_RequestedConstraintsType::TABLECELL);
	constraints.height = getRequestedConstraintsValue(StringTools::trim(height), -1);
	if (constraints.leftType == GUINode_RequestedConstraints_RequestedConstraintsType::PIXEL) constraints.left*= factor;
	if (constraints.topType == GUINode_RequestedConstraints_RequestedConstraintsType::PIXEL) constraints.top*= factor;
	if (constraints.widthType == GUINode_RequestedConstraints_RequestedConstraintsType::PIXEL) constraints.width*= factor;
	if (constraints.heightType == GUINode_RequestedConstraints_RequestedConstraintsType::PIXEL) constraints.height*= factor;
	return constraints;
}

