#include <tdme/gui/elements/GUISliderVController.h>

#include <array>
#include <string>

#include <tdme/gui/events/GUIKeyboardEvent.h>
#include <tdme/gui/events/GUIMouseEvent.h>
#include <tdme/gui/nodes/GUIElementController.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/gui/GUI.h>
#include <tdme/math/Math.h>
#include <tdme/utilities/Float.h>
#include <tdme/utilities/MutableString.h>
#include <tdme/utilities/Time.h>

using std::array;
using std::to_string;

using tdme::gui::events::GUIKeyboardEvent;
using tdme::gui::events::GUIMouseEvent;
using tdme::gui::nodes::GUIElementController;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::GUI;
using tdme::math::Math;
using tdme::utilities::Float;
using tdme::utilities::MutableString;
using tdme::utilities::Time;

using tdme::gui::elements::GUISliderVController;

GUISliderVController::GUISliderVController(GUINode* node): GUIElementController(node)
{
	// TODO: maybe introduce a custom relative node alignment, I use content alignment for now
}

bool GUISliderVController::isDisabled() {
	return false;
}

void GUISliderVController::setDisabled(bool disabled) {

}

void GUISliderVController::initialize() {
	sliderNode = this->node->getScreenNode()->getNodeById(this->node->getId() + "_slider");
	setValue(MutableString(dynamic_cast<GUIElementNode*>(node)->getValue()));

	//
	GUIElementController::initialize();
}

void GUISliderVController::dispose() {
	GUIElementController::dispose();
}

void GUISliderVController::postLayout() {
	updateSlider();
}

void GUISliderVController::handleMouseEvent(GUINode* node, GUIMouseEvent* event) {
	GUIElementController::handleMouseEvent(node, event);
	array<float, 2> nodeMousePosition;
	if (node == this->node &&
		event->getType() == GUIMouseEvent::MOUSEEVENT_RELEASED == true) {
		this->node->getScreenNode()->getGUI()->setFoccussedNode(dynamic_cast<GUIElementNode*>(this->node));
		event->setProcessed(true);
	} else
	if (node == this->node && node->isEventBelongingToNode(event, nodeMousePosition) == true &&
		(event->getType() == GUIMouseEvent::MOUSEEVENT_PRESSED == true ||
		event->getType() == GUIMouseEvent::MOUSEEVENT_DRAGGED == true) &&
		event->getButton() == MOUSE_BUTTON_LEFT) {
		event->setProcessed(true);
		auto sliderPosition = Math::clamp(
			static_cast<int>(nodeMousePosition[1]) - sliderNode->getContentHeight() / 2 - this->node->getPadding().top,
			0,
			this->node->getComputedConstraints().height - this->node->getPadding().top - this->node->getPadding().bottom - sliderNode->getContentHeight()
		);
		valueFloat = 1.0f - static_cast<float>(sliderPosition) / static_cast<float>(this->node->getComputedConstraints().height - this->node->getPadding().top - this->node->getPadding().bottom - sliderNode->getContentHeight());
		updateSlider();
		node->getScreenNode()->delegateValueChanged(dynamic_cast< GUIElementNode* >(this->node));
	}
}

void GUISliderVController::handleKeyboardEvent(GUINode* node, GUIKeyboardEvent* event) {
	GUIElementController::handleKeyboardEvent(node, event);
	if (node == this->node) {
		switch (event->getKeyCode()) {
			case GUIKeyboardEvent::KEYCODE_UP: {
					event->setProcessed(true);
					if (event->getType() == GUIKeyboardEvent::KEYBOARDEVENT_KEY_PRESSED) {
						this->valueFloat = Math::clamp(valueFloat + 0.1f, 0.0f, 1.0f);
						updateSlider();
						node->getScreenNode()->delegateValueChanged(dynamic_cast< GUIElementNode* >(this->node));
					}
				}
				break;
			case GUIKeyboardEvent::KEYCODE_DOWN: {
					event->setProcessed(true);
					if (event->getType() == GUIKeyboardEvent::KEYBOARDEVENT_KEY_PRESSED) {
						this->valueFloat = Math::clamp(valueFloat - 0.1f, 0.0f, 1.0f);
						updateSlider();
						node->getScreenNode()->delegateValueChanged(dynamic_cast< GUIElementNode* >(this->node));
					}
				}
				break;
		}
	}
}

void GUISliderVController::tick() {
	GUIElementController::tick();
}

void GUISliderVController::onFocusGained() {
}

void GUISliderVController::onFocusLost() {
}

bool GUISliderVController::hasValue() {
	return true;
}

const MutableString& GUISliderVController::getValue() {
	return value.set(valueFloat, 4);
}

void GUISliderVController::setValue(const MutableString& value) {
	this->value.set(value);
	this->valueFloat = Math::clamp(Float::parseFloat(this->value.getString()), 0.0f, 1.0f);
	updateSlider();
}

void GUISliderVController::updateSlider() {
	auto sliderPosition = static_cast<int>((1.0f - valueFloat) * (this->node->getComputedConstraints().height - this->node->getPadding().top - this->node->getPadding().bottom - sliderNode->getContentHeight()));
	sliderNode->getComputedConstraints().contentAlignmentLeft = (node->getComputedConstraints().width - this->node->getPadding().left - this->node->getPadding().right - sliderNode->getContentWidth()) / 2;
	sliderNode->getComputedConstraints().contentAlignmentTop = sliderPosition;
}
