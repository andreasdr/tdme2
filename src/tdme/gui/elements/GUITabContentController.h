#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/gui/elements/fwd-tdme.h>
#include <tdme/gui/events/fwd-tdme.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/utilities/MutableString.h>
#include <tdme/gui/nodes/GUINodeController.h>

using std::string;

using tdme::gui::nodes::GUINodeController;
using tdme::gui::events::GUIKeyboardEvent;
using tdme::gui::events::GUIMouseEvent;
using tdme::gui::nodes::GUINode;
using tdme::utilities::MutableString;

/**
 * GUI tab content controller
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::gui::elements::GUITabContentController final
	: public GUINodeController
{
	friend class GUITabContent;
	friend class GUITabsController;

private:
	string CONDITION_SELECTED;
	string CONDITION_UNSELECTED;
	bool selected;
	MutableString value;

	/**
	 * Private constructor
	 * @param node node
	 */
	GUITabContentController(GUINode* node);

	/**
	 * @return is checked
	 */
	bool isSelected();

	/**
	 * Set checked
	 * @param selected selected
	 */
	void setSelected(bool selected);

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
