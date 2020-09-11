#pragma once

#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/gui/elements/fwd-tdme.h>
#include <tdme/gui/events/fwd-tdme.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/gui/nodes/GUIElementController.h>
#include <tdme/utilities/MutableString.h>

using std::vector;
using std::string;

using tdme::gui::nodes::GUINodeController;
using tdme::gui::events::GUIKeyboardEvent;
using tdme::gui::events::GUIMouseEvent;
using tdme::gui::nodes::GUIElementController;
using tdme::gui::nodes::GUINode;
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
	vector<GUINode*> childControllerNodes;
	vector<GUISelectBoxOptionController*> selectBoxOptionControllers;
	bool disabled;
	MutableString value;

public:
	bool isDisabled() override;
	void setDisabled(bool disabled) override;
	void initialize() override;
	void dispose() override;
	void postLayout() override;

private:
	/** 
	 * Unselect all nodes
	 */
	void unselect();

	/** 
	 * Determine select box option controllers
	 */
	void determineSelectBoxOptionControllers();

	/** 
	 * Get selected option idx
	 */
	int32_t getSelectedOptionIdx();

	/** 
	 * Select current option
	 */
	void selectCurrent();

	/** 
	 * Select next node
	 */
	void selectNext();

	/** 
	 * Select previous
	 */
	void selectPrevious();

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
	GUISelectBoxController(GUINode* node);

	/**
	 * Init
	 */
	void init();
};
