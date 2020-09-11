#pragma once

#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/gui/elements/fwd-tdme.h>
#include <tdme/gui/events/fwd-tdme.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/utilities/MutableString.h>
#include <tdme/gui/nodes/GUINodeController.h>

using std::vector;
using std::string;

using tdme::gui::nodes::GUINodeController;
using tdme::gui::events::GUIKeyboardEvent;
using tdme::gui::events::GUIMouseEvent;
using tdme::gui::nodes::GUINode;
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
	 * Unselect all tab nodes
	 */
	void unselect();

	/** 
	 * Set tab content selected
	 * @param id id
	 */
	void setTabContentSelected(const string& id);

public:
	bool isDisabled() override;
	void setDisabled(bool disabled) override;
	void initialize() override;
	void dispose() override;
	void postLayout() override;

public:
	void handleMouseEvent(GUINode* node, GUIMouseEvent* event) override;
	void handleKeyboardEvent(GUINode* node, GUIKeyboardEvent* event) override;
	void tick() override;
	void onFocusGained() override;
	void onFocusLost() override;
	bool hasValue() override;
	const MutableString& getValue() override;
	void setValue(const MutableString& value) override;

private:
	/**
	 * Private constructor
	 * @param node node
	 */
	GUITabsController(GUINode* node);

	/**
	 * Init
	 */
	void init();
};
