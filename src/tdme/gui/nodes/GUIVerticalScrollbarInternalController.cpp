#include <tdme/gui/nodes/GUIVerticalScrollbarInternalController.h>

#include <tdme/gui/GUI.h>
#include <tdme/gui/events/GUIMouseEvent.h>
#include <tdme/gui/nodes/GUILayoutNode.h>
#include <tdme/gui/nodes/GUINode_Border.h>
#include <tdme/gui/nodes/GUINode_ComputedConstraints.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUIParentNode.h>
#include <tdme/gui/nodes/GUIScreenNode.h>

using tdme::gui::nodes::GUIVerticalScrollbarInternalController;

using tdme::gui::GUI;
using tdme::gui::events::GUIMouseEvent;
using tdme::gui::nodes::GUILayoutNode;
using tdme::gui::nodes::GUINode_Border;
using tdme::gui::nodes::GUINode_ComputedConstraints;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUIParentNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::nodes::GUIVerticalScrollbarInternalController_State;

GUIVerticalScrollbarInternalController::GUIVerticalScrollbarInternalController(GUINode* node)
	: GUINodeController(node)
{
	this->contentNode = dynamic_cast< GUILayoutNode* >(node->getScreenNode()->getNodeById(node->getParentControllerNode()->id + "_inner"));
	state = STATE_NONE;
	mouseYOffset = -1;
	contentHeight = 0;
}

bool GUIVerticalScrollbarInternalController::isDisabled()
{
	return false;
}

void GUIVerticalScrollbarInternalController::setDisabled(bool disabled)
{
}

void GUIVerticalScrollbarInternalController::initialize()
{
}

void GUIVerticalScrollbarInternalController::dispose()
{
}

void GUIVerticalScrollbarInternalController::postLayout()
{
	contentHeight = contentNode->getContentHeight();
}

GUIVerticalScrollbarInternalController::State GUIVerticalScrollbarInternalController::getState()
{
	return state;
}

float GUIVerticalScrollbarInternalController::getBarHeight()
{
	float elementHeight = contentNode->computedConstraints.height;
	auto barHeightRelative = (elementHeight / contentHeight);
	if (barHeightRelative > 1.0f) barHeightRelative = 1.0f;
	float barHeight = (node->computedConstraints.height - node->border.top - node->border.bottom) * barHeightRelative;
	if (barHeight < 5.0f) barHeight = 5.0f;
	return barHeight;
}

float GUIVerticalScrollbarInternalController::getBarTop()
{
	float elementHeight = contentNode->computedConstraints.height;
	auto scrollableHeight = contentHeight - elementHeight;
	auto childrenRenderOffsetY = contentNode->getChildrenRenderOffsetY();
	if (scrollableHeight > 0.0f) {
		return node->computedConstraints.top + node->computedConstraints.alignmentTop + node->border.top + (childrenRenderOffsetY * ((node->computedConstraints.height - getBarHeight()) / scrollableHeight));
	} else {
		return node->computedConstraints.top + node->computedConstraints.alignmentTop + node->border.top;
	}
}

void GUIVerticalScrollbarInternalController::setDraggedY(float draggedY)
{
	float elementHeight = contentNode->computedConstraints.height;
	auto scrollableHeight = contentHeight - elementHeight;
	if (scrollableHeight <= 0.0f) return;

	auto childrenRenderOffsetY = contentNode->getChildrenRenderOffsetY() + (draggedY * (scrollableHeight / (node->computedConstraints.height - getBarHeight())));
	if (childrenRenderOffsetY < 0) childrenRenderOffsetY = 0;
	if (childrenRenderOffsetY > scrollableHeight) childrenRenderOffsetY = scrollableHeight;

	contentNode->setChildrenRenderOffsetY(childrenRenderOffsetY);
}

void GUIVerticalScrollbarInternalController::handleMouseEvent(GUINode* node, GUIMouseEvent* event)
{
	if (event->getType() == GUIMouseEvent::MOUSEEVENT_MOVED) {
		if (this->node->isEventBelongingToNode(event) == true) {
			state = STATE_MOUSEOVER;
			this->node->getScreenNode()->getGUI()->addMouseOutCandidateElementNode(this->node);
		} else {
			state = STATE_NONE;
		}
		event->setProcessed(true);
	} else
	if (this->node == node && event->getButton() == MOUSE_BUTTON_LEFT) {
		if (node->isEventBelongingToNode(event) == true && event->getType() == GUIMouseEvent::MOUSEEVENT_PRESSED) {
			auto barOffsetY = node->computeParentChildrenRenderOffsetYTotal();
			auto barTop = getBarTop();
			auto barHeight = getBarHeight();
			if (event->getY() + barOffsetY < barTop) {
				float elementHeight = contentNode->computedConstraints.height;
				auto scrollableHeight = contentHeight - elementHeight;
				setDraggedY(-elementHeight * ((node->computedConstraints.height - barHeight) / scrollableHeight));
			} else
			if (event->getY() + barOffsetY > barTop + barHeight) {
				float elementHeight = contentNode->computedConstraints.height;
				auto scrollableHeight = contentHeight - elementHeight;
				setDraggedY(+elementHeight * ((node->computedConstraints.height - barHeight) / scrollableHeight));
			} else
			if (event->getY() + barOffsetY >= barTop && event->getY() + barOffsetY < barTop + barHeight) {
				mouseYOffset = static_cast< int >((event->getY() - barTop));
				state = STATE_DRAGGING;
			}
			event->setProcessed(true);
		} else
		if (state == STATE_DRAGGING && event->getType() == GUIMouseEvent::MOUSEEVENT_RELEASED) {
			mouseYOffset = -1;
			state = STATE_NONE;
			event->setProcessed(true);
		} else
		if (state == STATE_DRAGGING && event->getType() == GUIMouseEvent::MOUSEEVENT_DRAGGED) {
			auto barTop = getBarTop();
			auto draggedY = event->getY() - barTop - mouseYOffset;
			setDraggedY(draggedY);
			event->setProcessed(true);
		}
	}
}

void GUIVerticalScrollbarInternalController::handleKeyboardEvent(GUINode* node, GUIKeyboardEvent* event)
{
}

void GUIVerticalScrollbarInternalController::tick()
{
}

void GUIVerticalScrollbarInternalController::onFocusGained()
{
}

void GUIVerticalScrollbarInternalController::onFocusLost()
{
}

bool GUIVerticalScrollbarInternalController::hasValue()
{
	return false;
}

const MutableString& GUIVerticalScrollbarInternalController::getValue()
{
	return value;
}

void GUIVerticalScrollbarInternalController::setValue(const MutableString& value)
{
}

