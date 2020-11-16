#pragma once

#include <tdme/tdme.h>
#include <tdme/gui/elements/fwd-tdme.h>
#include <tdme/gui/events/fwd-tdme.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/utilities/MutableString.h>
#include <tdme/gui/nodes/GUINodeController.h>

using tdme::gui::events::GUIActionListener;
using tdme::gui::nodes::GUINodeController;
using tdme::gui::events::GUIKeyboardEvent;
using tdme::gui::events::GUIMouseEvent;
using tdme::gui::nodes::GUINode;
using tdme::utilities::MutableString;

/**
 * GUI scroll area horizontal controller
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::gui::elements::GUIScrollAreaHorizontalController final
	: public GUINodeController
{
	friend class GUIScrollAreaHorizontal;
	friend class GUIScrollAreaHorizontalController_initialize_1;

private:
	MutableString value;
	GUIActionListener* actionListener { nullptr };

	/**
	 * Private constructor
	 * @param node node
	 */
	GUIScrollAreaHorizontalController(GUINode* node);

public:
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
