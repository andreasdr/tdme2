#pragma once

#include <tdme/gui/elements/fwd-tdme.h>
#include <tdme/gui/events/fwd-tdme.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/gui/nodes/GUIElementController.h>
#include <tdme/utilities/MutableString.h>

using tdme::gui::events::GUIKeyboardEvent;
using tdme::gui::events::GUIMouseEvent;
using tdme::gui::nodes::GUIElementController;
using tdme::gui::nodes::GUINode;
using tdme::utilities::MutableString;

/**
 * GUI horizontal slider controller
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::gui::elements::GUISliderHController final: public GUIElementController
{
	friend class GUISliderH;

private:
	GUINode* sliderNode { nullptr };
	bool disabled { false };
	float valueFloat { 0.0f };
	MutableString value { };

	/**
	 * Private constructor
	 * @param node node
	 */
	GUISliderHController(GUINode* node);

	/**
	 * Update slider
	 */
	void updateSlider();

public:
	// overridden methods
	bool isDisabled() override;
	void setDisabled(bool disabled) override;
	void initialize() override;
	void dispose() override;
	void postLayout() override;
	void handleMouseEvent(GUINode* node, GUIMouseEvent* event) override;
	void handleKeyboardEvent(GUINode* node, GUIKeyboardEvent* event) override;
	void tick() override;
	void onFocusGained() override;
	void onFocusLost() override;
	bool hasValue() override;
	const MutableString& getValue() override;
	void setValue(const MutableString& value) override;

};
