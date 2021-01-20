#pragma once

#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/gui/elements/fwd-tdme.h>
#include <tdme/gui/events/fwd-tdme.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/gui/nodes/GUINodeController.h>
#include <tdme/utilities/MutableString.h>

using std::string;
using std::vector;

using tdme::gui::events::GUIKeyboardEvent;
using tdme::gui::events::GUIMouseEvent;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeController;
using tdme::utilities::MutableString;

/**
 * GUI tabs controller
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::gui::elements::GUITabsController final
	: public GUINodeController
{
	friend class GUITabs;
	friend class GUITabController;
	friend class GUITabsHeaderController;

private:
	vector<GUINode*> childControllerNodes;
	MutableString value;

	/**
	 * Private constructor
	 * @param node node
	 */
	GUITabsController(GUINode* node);

	/**
	 * Initialize
	 */
	void init();

	/**
	 * Unselect all tab nodes
	 */
	void unselect();

	/**
	 * Set tab content selected
	 * @param id id
	 */
	void setTabContentSelected(const string& id);

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
