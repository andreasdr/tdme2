#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/gui/elements/fwd-tdme.h>
#include <tdme/gui/events/fwd-tdme.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/gui/nodes/GUIElementController.h>
#include <tdme/utilities/MutableString.h>

using std::string;

using tdme::gui::nodes::GUINodeController;
using tdme::gui::events::GUIKeyboardEvent;
using tdme::gui::events::GUIMouseEvent;
using tdme::gui::nodes::GUIElementController;
using tdme::gui::nodes::GUINode;
using tdme::utilities::MutableString;

/** 
 * GUI check box controller
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::gui::elements::GUICheckboxController final
	: public GUIElementController
{
	friend class GUICheckbox;

private:
	static string CONDITION_CHECKED;
	static string CONDITION_UNCHECKED;
	static string CONDITION_DISABLED;
	static string CONDITION_ENABLED;
	bool checked;
	bool disabled;
	MutableString value;

	/** 
	 * @return is checked
	 */
	bool isChecked();

	/** 
	 * Set checked
	 * @param checked checked
	 */
	void setChecked(bool checked);

	/**
	 * Private constructor
	 * @param node node
	 */
	GUICheckboxController(GUINode* node);

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
