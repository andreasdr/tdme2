#include <tdme/gui/nodes/GUIHorizontalScrollbarInternalNode.h>

#include <array>
#include <vector>

#include <tdme/gui/GUI.h>
#include <tdme/gui/nodes/GUIColor.h>
#include <tdme/gui/nodes/GUIHorizontalScrollbarInternalController.h>
#include <tdme/gui/nodes/GUINode_Border.h>
#include <tdme/gui/nodes/GUINode_ComputedConstraints.h>
#include <tdme/gui/nodes/GUINode_Scale9Grid.h>
#include <tdme/gui/nodes/GUINodeController.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/gui/renderer/GUIRenderer.h>

using std::array;
using std::vector;

using tdme::gui::nodes::GUIHorizontalScrollbarInternalNode;
using tdme::gui::GUI;
using tdme::gui::nodes::GUIColor;
using tdme::gui::nodes::GUIHorizontalScrollbarInternalController_State;
using tdme::gui::nodes::GUIHorizontalScrollbarInternalController;
using tdme::gui::nodes::GUINode_Border;
using tdme::gui::nodes::GUINode_ComputedConstraints;
using tdme::gui::nodes::GUINode_Scale9Grid;
using tdme::gui::nodes::GUINodeController;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::renderer::GUIRenderer;

GUIHorizontalScrollbarInternalNode::GUIHorizontalScrollbarInternalNode(
	GUIScreenNode* screenNode,
	GUIParentNode* parentNode, const
	string& id,
	GUINode_Flow* flow,
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
	const GUIColor& barColorNone,
	const GUIColor& barColorMouseOver,
	const GUIColor& barColorDragging
	):
	GUINode(screenNode, parentNode, id, flow, alignments, requestedConstraints, backgroundColor, backgroundImage, backgroundImageScale9Grid, backgroundImageEffectColorMul, backgroundImageEffectColorAdd, border, padding, showOn, hideOn)
{
	this->controller = new GUIHorizontalScrollbarInternalController(this);
	this->barColorNone = barColorNone;
	this->barColorMouseOver = barColorMouseOver;
	this->barColorDragging = barColorDragging;
}

const string GUIHorizontalScrollbarInternalNode::getNodeType()
{
	return "scrollbar";
}

bool GUIHorizontalScrollbarInternalNode::isContentNode()
{
	return false;
}

int GUIHorizontalScrollbarInternalNode::getContentWidth()
{
	return computedConstraints.width;
}

int GUIHorizontalScrollbarInternalNode::getContentHeight()
{
	return computedConstraints.height;
}

void GUIHorizontalScrollbarInternalNode::render(GUIRenderer* guiRenderer)
{
	if (shouldRender() == false) return;

	GUINode::render(guiRenderer);
	auto screenWidth = screenNode->getScreenWidth();
	auto screenHeight = screenNode->getScreenHeight();
	auto controller = dynamic_cast< GUIHorizontalScrollbarInternalController* >(this->controller);
	auto barWidth = controller->getBarWidth();
	auto barLeft = controller->getBarLeft();
	auto left = barLeft;
	float top = computedConstraints.top + computedConstraints.alignmentTop + border.top;
	auto width = barWidth;
	float height = computedConstraints.height - border.top - border.bottom;
	array<float, 4> barColorArray;
	{
		auto v = controller->getState();
		if (v == GUIHorizontalScrollbarInternalController::STATE_NONE) {
			barColorArray = barColorNone.getArray();
		} else
		if (v == GUIHorizontalScrollbarInternalController::STATE_MOUSEOVER) {
			barColorArray = barColorMouseOver.getArray();
		} else
		if (v == GUIHorizontalScrollbarInternalController::STATE_DRAGGING) {
			barColorArray = barColorDragging.getArray();
		}
	}

	guiRenderer->bindTexture(0);
	guiRenderer->addQuad(((left) / (screenWidth / 2.0f)) - 1.0f, ((screenHeight - top) / (screenHeight / 2.0f)) - 1.0f, barColorArray[0], barColorArray[1], barColorArray[2], barColorArray[3], 0.0f, 1.0f, ((left + width) / (screenWidth / 2.0f)) - 1.0f, ((screenHeight - top) / (screenHeight / 2.0f)) - 1.0f, barColorArray[0], barColorArray[1], barColorArray[2], barColorArray[3], 1.0f, 1.0f, ((left + width) / (screenWidth / 2.0f)) - 1.0f, ((screenHeight - top - height) / (screenHeight / 2.0f)) - 1.0f, barColorArray[0], barColorArray[1], barColorArray[2], barColorArray[3], 1.0f, 0.0f, ((left) / (screenWidth / 2.0f)) - 1.0f, ((screenHeight - top - height) / (screenHeight / 2.0f)) - 1.0f, barColorArray[0], barColorArray[1], barColorArray[2], barColorArray[3], 0.0f, 0.0f);
	guiRenderer->render();
}

