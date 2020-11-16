#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/gui/elements/fwd-tdme.h>
#include <tdme/gui/events/fwd-tdme.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/utilities/MutableString.h>
#include <tdme/gui/nodes/GUIElementController.h>

using std::string;

using tdme::gui::nodes::GUINodeController;
using tdme::gui::events::GUIKeyboardEvent;
using tdme::gui::events::GUIMouseEvent;
using tdme::gui::nodes::GUIElementController;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUIParentNode;
using tdme::utilities::MutableString;

/**
 * GUI select box option controller
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::gui::elements::GUISelectBoxOptionController: public GUIElementController
{
	friend class GUISelectBoxOption;
	friend class GUISelectBoxController;
	friend class GUISelectBoxParentOptionController;

private:
	static string CONDITION_SELECTED;
	static string CONDITION_UNSELECTED;
	static string CONDITION_DISABLED;
	static string CONDITION_ENABLED;
	GUIParentNode* selectBoxNode { nullptr };
	bool initialPostLayout;
	bool selected;
	MutableString value;

	/**
	 * Private constructor
	 * @param node node
	 */
	GUISelectBoxOptionController(GUINode* node);

	/**
	 * @return is selected
	 */
	bool isSelected();

	/**
	 * Select
	 */
	void select();

	/**
	 * Unselect
	 */
	void unselect();

	/**
	 * @return if is collapsed in tree view
	 */
	bool isCollapsed();
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
