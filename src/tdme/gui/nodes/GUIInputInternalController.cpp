#include <tdme/gui/nodes/GUIInputInternalController.h>

#include <tdme/gui/elements/GUIInputController.h>
#include <tdme/gui/events/GUIActionListener_Type.h>
#include <tdme/gui/events/GUIKeyboardEvent_Type.h>
#include <tdme/gui/events/GUIKeyboardEvent.h>
#include <tdme/gui/events/GUIMouseEvent_Type.h>
#include <tdme/gui/events/GUIMouseEvent.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUIInputInternalNode.h>
#include <tdme/gui/nodes/GUINode_Border.h>
#include <tdme/gui/nodes/GUINode_ComputedConstraints.h>
#include <tdme/gui/nodes/GUINode_Padding.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUINodeController.h>
#include <tdme/gui/nodes/GUIParentNode.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/gui/renderer/GUIFont.h>
#include <tdme/utilities/MutableString.h>
#include <tdme/utilities/Time.h>

using tdme::gui::nodes::GUIInputInternalController;
using tdme::gui::elements::GUIInputController;
using tdme::gui::events::GUIActionListener_Type;
using tdme::gui::events::GUIKeyboardEvent_Type;
using tdme::gui::events::GUIKeyboardEvent;
using tdme::gui::events::GUIMouseEvent_Type;
using tdme::gui::events::GUIMouseEvent;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUIInputInternalController_CursorMode;
using tdme::gui::nodes::GUIInputInternalNode;
using tdme::gui::nodes::GUINode_Border;
using tdme::gui::nodes::GUINode_ComputedConstraints;
using tdme::gui::nodes::GUINode_Padding;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeController;
using tdme::gui::nodes::GUIParentNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::renderer::GUIFont;
using tdme::utilities::MutableString;
using tdme::utilities::Time;

constexpr int64_t GUIInputInternalController::CURSOR_MODE_DURATION;
constexpr int64_t GUIInputInternalController::DRAGGING_CALMDOWN;

GUIInputInternalController::GUIInputInternalController(GUINode* node) 
	: GUINodeController(node)
{
	this->cursorModeStarted = -1LL;
	this->cursorMode = CURSORMODE_SHOW;
	this->draggingTickLast = -1LL;
	this->index = 0;
	this->offset = 0;
	this->isDragging = false;
	this->dragPosition = {{ 0, 0 }};
}

bool GUIInputInternalController::isDisabled()
{
	return false;
}

void GUIInputInternalController::setDisabled(bool disabled)
{
}

void GUIInputInternalController::initialize()
{
	this->node->getScreenNode()->addTickNode(this->node);
	inputNode = dynamic_cast< GUIElementNode* >(node->getParentControllerNode());
}

void GUIInputInternalController::dispose()
{
}

void GUIInputInternalController::postLayout()
{
}

int32_t GUIInputInternalController::getIndex()
{
	return index;
}

int32_t GUIInputInternalController::getOffset()
{
	return offset;
}

void GUIInputInternalController::resetCursorMode()
{
	cursorModeStarted = Time::getCurrentMillis();
	cursorMode = CURSORMODE_SHOW;
}

GUIInputInternalController::CursorMode GUIInputInternalController::getCursorMode()
{
	if (cursorModeStarted == -1) {
		resetCursorMode();
		return cursorMode;
	}
	if (Time::getCurrentMillis() - cursorModeStarted > CURSOR_MODE_DURATION) {
		cursorMode = cursorMode == CURSORMODE_SHOW ? CURSORMODE_HIDE : CURSORMODE_SHOW;
		cursorModeStarted = Time::getCurrentMillis();
	}
	return cursorMode;
}

void GUIInputInternalController::handleMouseEvent(GUINode* node, GUIMouseEvent* event)
{
	auto disabled = (dynamic_cast< GUIInputController* >(inputNode->getController()))->isDisabled();
	if (disabled == true) {
		return;
	}
	if (node == this->node &&
		event->getType() == GUIMouseEvent_Type::MOUSEEVENT_RELEASED == true) {
		isDragging = false;
		event->setProcessed(true);
	} else
	if (node == this->node && node->isEventBelongingToNode(event) == true &&
		(event->getType() == GUIMouseEvent_Type::MOUSEEVENT_PRESSED == true ||
		event->getType() == GUIMouseEvent_Type::MOUSEEVENT_DRAGGED == true) &&
		event->getButton() == MOUSE_BUTTON_LEFT) {
		auto textInputNode = (dynamic_cast< GUIInputInternalNode* >(node));
		index = textInputNode->getFont()->getTextIndexByX(
			textInputNode->getText(),
			offset,
			0,
			event->getX() -
				(
					textInputNode->computedConstraints.left + textInputNode->computedConstraints.alignmentLeft +
					textInputNode->border.left+ textInputNode->padding.left
				)
		);
		resetCursorMode();
		event->setProcessed(true);
		isDragging = true;
		dragPosition[0] = 0;
		dragPosition[1] = 0;
	} else
	if (isDragging == true) {
		node->getEventOffNodeRelativePosition(event, dragPosition);
		event->setProcessed(true);
	}
}

void GUIInputInternalController::checkOffset()
{
	if (index < offset) {
		offset = index;
		return;
	}
	auto textInputNode = (dynamic_cast< GUIInputInternalNode* >(node));
	auto textInputNodeConstraints = textInputNode->computedConstraints;
	auto textInputNodeBorder = textInputNode->border;
	auto textInputNodePadding = textInputNode->padding;
	auto textInputNodeWidth = textInputNodeConstraints.width - textInputNodeBorder.left - textInputNodeBorder.right - textInputNodePadding.left - textInputNodePadding.right;
	auto charsMax = textInputNode->getFont()->getTextIndexByX(textInputNode->getText(), offset, 0, textInputNodeWidth) - offset;
	if (index - offset >= charsMax) {
		offset = index - charsMax;
	}
}

void GUIInputInternalController::handleKeyboardEvent(GUINode* node, GUIKeyboardEvent* event)
{
	auto disabled = (dynamic_cast< GUIInputController* >(inputNode->getController()))->isDisabled();
	if (disabled == true) {
		return;
	}
	if (node == this->node) {
		auto textInputNode = (dynamic_cast< GUIInputInternalNode* >(node));
		auto keyChar = event->getKeyChar();
		if (disabled == false && keyChar >= 32 && keyChar < 127) {
			event->setProcessed(true);
			#if defined(VULKAN) || defined(GLFW3)
				if (event->getType() == GUIKeyboardEvent_Type::KEYBOARDEVENT_KEY_TYPED) {
			#else
				if (event->getType() == GUIKeyboardEvent_Type::KEYBOARDEVENT_KEY_PRESSED) {
			#endif
				if (textInputNode->getMaxLength() == 0 || textInputNode->getText().length() < textInputNode->getMaxLength()) {
					textInputNode->getText().insert(index, event->getKeyChar());
					index++;
					resetCursorMode();
					checkOffset();
					node->getScreenNode()->delegateValueChanged(dynamic_cast< GUIElementNode* >(node->getParentControllerNode()));
				}
			}
		} else {
			switch (event->getKeyCode()) {
			case GUIKeyboardEvent::KEYCODE_LEFT: {
					event->setProcessed(true);
					if (event->getType() == GUIKeyboardEvent_Type::KEYBOARDEVENT_KEY_PRESSED) {
						if (index > 0) {
							index--;
							checkOffset();
							resetCursorMode();
						}
					}
				}
				break;
			case GUIKeyboardEvent::KEYCODE_RIGHT: {
					event->setProcessed(true);
					if (event->getType() == GUIKeyboardEvent_Type::KEYBOARDEVENT_KEY_PRESSED) {
						if (index < textInputNode->getText().length()) {
							index++;
							checkOffset();
							resetCursorMode();
						}
					}
				}
				break;
			case GUIKeyboardEvent::KEYCODE_BACKSPACE: {
					if (disabled == false) {
						event->setProcessed(true);
						if (event->getType() == GUIKeyboardEvent_Type::KEYBOARDEVENT_KEY_PRESSED) {
							if (index > 0) {
								textInputNode->getText().delete_(index - 1, 1);
								index--;
								checkOffset();
								resetCursorMode();
								node->getScreenNode()->delegateValueChanged(dynamic_cast< GUIElementNode* >(node->getParentControllerNode()));
							}
						}
					}
				}
				break;
			case GUIKeyboardEvent::KEYCODE_DELETE: {
					if (disabled == false) {
						event->setProcessed(true);
						if (event->getType() == GUIKeyboardEvent_Type::KEYBOARDEVENT_KEY_PRESSED) {
							if (index < textInputNode->getText().length()) {
								textInputNode->getText().delete_(index, 1);
								resetCursorMode();
								node->getScreenNode()->delegateValueChanged(dynamic_cast< GUIElementNode* >(node->getParentControllerNode()));
							}
						}
					}
				}
				break;
			case GUIKeyboardEvent::KEYCODE_RETURN: {
					if (disabled == false) {
						event->setProcessed(true);
						if (event->getType() == GUIKeyboardEvent_Type::KEYBOARDEVENT_KEY_PRESSED) {
							node->getScreenNode()->delegateActionPerformed(GUIActionListener_Type::PERFORMED, dynamic_cast<GUIElementNode*>(node->getParentControllerNode()));
						}
					}
				}
				break;
			case GUIKeyboardEvent::KEYCODE_POS1: {
					if (disabled == false) {
						event->setProcessed(true);
						resetCursorMode();
						index = 0;
						checkOffset();
					}
				}
				break;
			case GUIKeyboardEvent::KEYCODE_END: {
					if (disabled == false) {
						resetCursorMode();
						index = textInputNode->getText().length();
						checkOffset();
					}
				}
				break;
			}
		}
	}
}

void GUIInputInternalController::tick()
{
	if (isDragging == true) {
		auto now = Time::getCurrentMillis();
		if (draggingTickLast != -1LL && now - draggingTickLast < DRAGGING_CALMDOWN) {
			return;
		}
		draggingTickLast = now;
		auto textInputNode = (dynamic_cast< GUIInputInternalNode* >(node));
		if (dragPosition[0] < 0) {
			if (index > 0) {
				index--;
				checkOffset();
			}
		} else
		if (dragPosition[0] > 0) {
			if (index < textInputNode->getText().length()) {
				index++;
				checkOffset();
			}
		}
	}
}

void GUIInputInternalController::onFocusGained()
{
}

void GUIInputInternalController::onFocusLost()
{
}

bool GUIInputInternalController::hasValue()
{
	return false;
}

const MutableString& GUIInputInternalController::getValue()
{
	return value;
}

void GUIInputInternalController::setValue(const MutableString& value)
{
}

void GUIInputInternalController::reset()
{
	index = 0;
	offset = 0;
	isDragging = false;
	resetCursorMode();
}

