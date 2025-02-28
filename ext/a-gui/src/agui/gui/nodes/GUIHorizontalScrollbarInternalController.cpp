#include <agui/gui/nodes/GUIHorizontalScrollbarInternalController.h>

#include <agui/agui.h>
#include <agui/gui/events/GUIMouseEvent.h>
#include <agui/gui/nodes/GUILayoutNode.h>
#include <agui/gui/nodes/GUINode.h>
#include <agui/gui/nodes/GUINode_Border.h>
#include <agui/gui/nodes/GUINode_ComputedConstraints.h>
#include <agui/gui/nodes/GUIParentNode.h>
#include <agui/gui/nodes/GUIScreenNode.h>
#include <agui/gui/GUI.h>
#include <agui/utilities/MutableString.h>

using agui::gui::nodes::GUIHorizontalScrollbarInternalController;

using agui::gui::events::GUIMouseEvent;
using agui::gui::nodes::GUIHorizontalScrollbarInternalController_State;
using agui::gui::nodes::GUILayoutNode;
using agui::gui::nodes::GUINode;
using agui::gui::nodes::GUINode_Border;
using agui::gui::nodes::GUINode_ComputedConstraints;
using agui::gui::nodes::GUIParentNode;
using agui::gui::nodes::GUIScreenNode;
using agui::gui::GUI;
using agui::utilities::MutableString;

GUIHorizontalScrollbarInternalController::GUIHorizontalScrollbarInternalController(GUINode* node)
	: GUINodeController(node)
{
	state = STATE_NONE;
	mouseXOffset = -1;
	contentNode = required_dynamic_cast<GUILayoutNode*>(node->getScreenNode()->getNodeById(node->getParentControllerNode()->id + "_inner"));
	contentWidth = 0;
}

bool GUIHorizontalScrollbarInternalController::isDisabled()
{
	return false;
}

void GUIHorizontalScrollbarInternalController::setDisabled(bool disabled)
{
}

void GUIHorizontalScrollbarInternalController::initialize()
{
}

void GUIHorizontalScrollbarInternalController::dispose()
{
}

void GUIHorizontalScrollbarInternalController::postLayout()
{
	contentWidth = contentNode->getContentWidth();
}

GUIHorizontalScrollbarInternalController::State GUIHorizontalScrollbarInternalController::getState()
{
	return state;
}

float GUIHorizontalScrollbarInternalController::getBarWidth()
{
	float elementWidth = contentNode->computedConstraints.width;
	auto barWidthRelative = (elementWidth / contentWidth);
	if (barWidthRelative > 1.0f) barWidthRelative = 1.0f;
	float barWidth = (node->computedConstraints.width - node->border.left - node->border.right) * barWidthRelative;
	if (barWidth < 20.0f) barWidth = 20.0f;
	return barWidth;
}

float GUIHorizontalScrollbarInternalController::getBarLeft()
{
	float elementWidth = contentNode->computedConstraints.width;
	auto scrollableWidth = contentWidth - elementWidth;
	auto childrenRenderOffsetX = contentNode->getChildrenRenderOffsetX();
	if (scrollableWidth > 0.0f) {
		return node->computedConstraints.left + node->computedConstraints.alignmentLeft + node->border.left+ (childrenRenderOffsetX * ((node->computedConstraints.width - getBarWidth()) / scrollableWidth));
	} else {
		return node->computedConstraints.left + node->computedConstraints.alignmentLeft + node->border.left;
	}
}

void GUIHorizontalScrollbarInternalController::setDraggedX(float draggedX)
{
	float elementWidth = contentNode->computedConstraints.width;
	auto scrollableWidth = contentWidth - elementWidth;
	if (scrollableWidth <= 0.0f) return;

	auto childrenRenderOffsetX = contentNode->getChildrenRenderOffsetX() + (draggedX * (scrollableWidth / (node->computedConstraints.width - getBarWidth())));
	if (childrenRenderOffsetX < 0) childrenRenderOffsetX = 0;
	if (childrenRenderOffsetX > scrollableWidth) childrenRenderOffsetX = scrollableWidth;

	contentNode->setChildrenRenderOffsetX(childrenRenderOffsetX);
}

void GUIHorizontalScrollbarInternalController::handleMouseEvent(GUINode* node, GUIMouseEvent* event)
{
	if (event->getType() == GUIMouseEvent::MOUSEEVENT_MOVED) {
		if (this->node->isEventBelongingToNode(event) == true) {
			state = STATE_MOUSEOVER;
			this->node->getScreenNode()->getGUI()->addMouseOutCandidateNode(this->node);
		} else {
			state = STATE_NONE;
		}
		event->setProcessed(true);
	} else
	if (this->node == node && event->getButton() == MOUSE_BUTTON_LEFT) {
		if (node->isEventBelongingToNode(event) == true && event->getType() == GUIMouseEvent::MOUSEEVENT_PRESSED) {
			auto barOffsetX = node->computeParentChildrenRenderOffsetXTotal();
			auto barLeft = getBarLeft();
			auto barWidth = getBarWidth();
			if (event->getX() + barOffsetX < barLeft) {
				float elementWidth = contentNode->computedConstraints.width;
				auto scrollableWidth = contentWidth - elementWidth;
				setDraggedX(-elementWidth * ((node->computedConstraints.width - barWidth) / scrollableWidth));
			} else
			if (event->getX() + barOffsetX > barLeft + barWidth) {
				float elementWidth = contentNode->computedConstraints.width;
				auto scrollableWidth = contentWidth - elementWidth;
				setDraggedX(+elementWidth * ((node->computedConstraints.width - barWidth) / scrollableWidth));
			} else
			if (event->getX() + barOffsetX >= barLeft && event->getX() + barOffsetX < barLeft + barWidth) {
				mouseXOffset = static_cast<int>((event->getX() - barLeft));
				state = STATE_DRAGGING;
			}
			event->setProcessed(true);
		} else
		if (state == STATE_DRAGGING && event->getType() == GUIMouseEvent::MOUSEEVENT_RELEASED) {
			mouseXOffset = -1;
			state = STATE_NONE;
			event->setProcessed(true);
		} else
		if (state == STATE_DRAGGING && event->getType() == GUIMouseEvent::MOUSEEVENT_DRAGGED) {
			auto barLeft = getBarLeft();
			auto draggedX = event->getX() - barLeft - mouseXOffset;
			setDraggedX(draggedX);
			event->setProcessed(true);
		}
	}
}

void GUIHorizontalScrollbarInternalController::handleKeyboardEvent(GUIKeyboardEvent* event)
{
}

void GUIHorizontalScrollbarInternalController::tick()
{
}

void GUIHorizontalScrollbarInternalController::onFocusGained()
{
}

void GUIHorizontalScrollbarInternalController::onFocusLost()
{
}

bool GUIHorizontalScrollbarInternalController::hasValue()
{
	return false;
}

const MutableString& GUIHorizontalScrollbarInternalController::getValue()
{
	return value;
}

void GUIHorizontalScrollbarInternalController::setValue(const MutableString& value)
{
}

void GUIHorizontalScrollbarInternalController::onSubTreeChange()
{
}
