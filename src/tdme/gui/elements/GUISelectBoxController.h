#pragma once

#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/gui/elements/fwd-tdme.h>
#include <tdme/gui/events/fwd-tdme.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/gui/nodes/GUIElementController.h>
#include <tdme/utilities/MutableString.h>

using std::string;
using std::vector;

using tdme::gui::events::GUIKeyboardEvent;
using tdme::gui::events::GUIMouseEvent;
using tdme::gui::nodes::GUIElementController;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeController;
using tdme::utilities::MutableString;

/**
 * GUI select box controller
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::gui::elements::GUISelectBoxController final
	: public GUIElementController
{
	friend class GUISelectBox;
	friend class GUISelectBoxOptionController;

private:
	static string CONDITION_DISABLED;
	static string CONDITION_ENABLED;
	static constexpr char VALUE_DELIMITER { '|' };
	vector<GUINode*> childControllerNodes;
	vector<GUISelectBoxOptionController*> selectBoxMultipleOptionControllers;
	bool disabled;
	bool multipleSelection;
	MutableString value;

	bool keyControl;

	/**
	 * Private constructor
	 * @param node node
	 */
	GUISelectBoxController(GUINode* node);

	/**
	 * @return if multiple selection is enabled
	 */
	bool isMultipleSelection();

	/**
	 * @return is control down
	 */
	bool isKeyControlDown();

	/**
	 * Unselect all nodes
	 */
	void unselect();

	/**
	 * Unfocus all nodes
	 */
	void unfocus();

	/**
	 * Determine select box option controllers
	 */
	void determineSelectBoxMultipleOptionControllers();

	/**
	 * Get focussed option idx
	 */
	int getFocussedOptionIdx();

	/**
	 * Select current options
	 */
	void selectCurrent();

	/**
	 * Focus next node
	 */
	void focusNext();

	/**
	 * Focus previous
	 */
	void focusPrevious();

	/**
	 * Toggle selected node
	 */
	void toggle();

	/**
	 * Select focussed node
	 */
	void select();

	/**
	 * Toggle open state of current parent option
	 */
	void toggleOpenState();

public:
	// overridden methods
	bool isDisabled() override;
	void setDisabled(bool disabled) override;
	void initialize() override;
	void dispose() override;
	void postLayout() override;
	void handleMouseEvent(GUINode* node, GUIMouseEvent* event) override;
	void handleKeyboardEvent(GUIKeyboardEvent* event) override;
	void tick() override;
	void onFocusGained() override;
	void onFocusLost() override;
	bool hasValue() override;
	const MutableString& getValue() override;
	void setValue(const MutableString& value) override;

};
