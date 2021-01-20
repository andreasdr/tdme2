#pragma once

#include <vector>

#include <tdme/tdme.h>
#include <tdme/gui/elements/fwd-tdme.h>
#include <tdme/gui/events/fwd-tdme.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/utilities/MutableString.h>
#include <tdme/gui/nodes/GUIElementController.h>

using std::vector;

using tdme::gui::nodes::GUIElementController;
using tdme::gui::events::GUIKeyboardEvent;
using tdme::gui::events::GUIMouseEvent;
using tdme::gui::nodes::GUINode;
using tdme::utilities::MutableString;

/**
 * GUI tabs header controller
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::gui::elements::GUITabsHeaderController final
	: public GUIElementController
{
	friend class GUITabsHeader;
	friend class GUITabController;

private:
	GUINode* tabsNode { nullptr };
	vector<GUINode*> childControllerNodes;
	vector<GUITabController*> tabControllers;
	bool focus;
	MutableString value;

	/**
	 * Private constructor
	 * @param node node
	 */
	GUITabsHeaderController(GUINode* node);

	/**
	 * @return has focus
	 */
	bool hasFocus();

	/**
	 * Unselect all nodes
	 */
	void unselect();

	/**
	 * Determine select box option controllers
	 */
	void determineTabControllers();

	/**
	 * Get selected tab idx
	 * @return selected tab index
	 */
	int getSelectedTabIdx();

	/**
	 * Select next node
	 */
	void selectNext();

	/**
	 * Select previous
	 */
	void selectPrevious();

	/**
	 * Select current
	 */
	void selectCurrent();

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
