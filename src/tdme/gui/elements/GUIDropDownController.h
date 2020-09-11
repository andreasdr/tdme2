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
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUIParentNode;
using tdme::utilities::MutableString;

/** 
 * GUI drop down controller
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::gui::elements::GUIDropDownController final
	: public GUIElementController
{
	friend class GUIDropDown;
	friend class GUIDropDownOptionController;

private:
	static string CONDITION_DISABLED;
	static string CONDITION_ENABLED;
	static string CONDITION_OPENED;
	static string CONDITION_CLOSED;
	vector<GUINode*> childControllerNodes;
	vector<GUIDropDownOptionController*> dropDownOptionControllers;
	bool isOpen_;
	bool disabled;
	GUIParentNode* dropDownNode { nullptr };
	GUIElementNode* arrowNode { nullptr };
	GUIElementNode* textElementNode { nullptr };
	MutableString value;

public:
	bool isDisabled() override;
	void setDisabled(bool disabled) override;
	void initialize() override;
	void dispose() override;
	void postLayout() override;

private:

	/** 
	 * @return drop down open state
	 */
	bool isOpen();

	/** 
	 * Unselect all nodes
	 */
	void unselect();

	/** 
	 * Toggle open state
	 */
	void toggleOpenState();

	/** 
	 * Determine drop down option controllers
	 */
	void determineDropDownOptionControllers();

	/** 
	 * Get selected option idx
	 */
	int32_t getSelectedOptionIdx();

	/** 
	 * Select next node
	 */
	void selectNext();

	/** 
	 * Select previous
	 */
	void selectPrevious();

	/**
	 * Private constructor
	 * @param node node
	 */
	GUIDropDownController(GUINode* node);

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
	void init();
};
