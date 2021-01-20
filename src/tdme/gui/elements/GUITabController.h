#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/gui/elements/fwd-tdme.h>
#include <tdme/gui/events/fwd-tdme.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/gui/nodes/GUIColor.h>
#include <tdme/gui/nodes/GUIElementController.h>
#include <tdme/utilities/fwd-tdme.h>
#include <tdme/utilities/MutableString.h>

using std::string;

using tdme::gui::events::GUIKeyboardEvent;
using tdme::gui::events::GUIMouseEvent;
using tdme::gui::nodes::GUIColor;
using tdme::gui::nodes::GUIElementController;
using tdme::gui::nodes::GUINode;
using tdme::utilities::MutableString;

/**
 * GUI tab controller
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::gui::elements::GUITabController final: public GUIElementController
{
	friend class GUITab;
	friend class GUITabsController;
	friend class GUITabsHeaderController;

private:
	static string CONDITION_DISABLED;
	static string CONDITION_ENABLED;
	static string CONDITION_SELECTED;
	static string CONDITION_UNSELECTED;
	GUINode* tabsNode { nullptr };
	GUINode* tabsHeaderNode { nullptr };
	bool selected;
	GUIColor unfocussedNodeBorderLeftColor;
	GUIColor unfocussedNodeBorderRightColor;
	GUIColor unfocussedNodeBorderTopColor;
	GUIColor unfocussedNodeBorderBottomColor;
	bool disabled;
	MutableString value;

	/**
	 * Private constructor
	 */
	GUITabController(GUINode* node);

	/**
	 * @return is checked
	 */
	bool isSelected();

	/**
	 * Set checked
	 * @param selected selected
	 */
	void setSelected(bool selected);

	/**
	 * Select this tab
	 */
	void selectTab();

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
