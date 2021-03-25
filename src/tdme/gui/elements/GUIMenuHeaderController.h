#pragma once

#include <vector>

#include <tdme/tdme.h>
#include <tdme/gui/elements/fwd-tdme.h>
#include <tdme/gui/events/fwd-tdme.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/gui/nodes/GUINodeController.h>
#include <tdme/utilities/MutableString.h>

using std::vector;

using tdme::gui::events::GUIKeyboardEvent;
using tdme::gui::events::GUIMouseEvent;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeController;
using tdme::utilities::MutableString;

/**
 * GUI menu header controller
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::gui::elements::GUIMenuHeaderController final
	: public GUINodeController
{
	friend class GUIMenuHeader;
	friend class GUIMenuHeaderItemController;

private:
	vector<GUINode*> childControllerNodes;
	vector<GUIMenuHeaderItemController*> menuHeaderItemControllers;
	bool focus;
	MutableString value;

	/**
	 * Private constructor
	 * @param node node
	 */
	GUIMenuHeaderController(GUINode* node);

	/**
	 * @return has focus
	 */
	bool hasFocus();

	/**
	 * Determine menu header item controllers
	 */
	void determineMenuHeaderItemControllers();

	/**
	 * Get selected header item idx
	 * @return selected header item index
	 */
	int getSelectedHeaderItemIdx();

	/**
	 * @return if menu is open
	 */
	bool isOpen();

	/**
	 * Unselect
	 */
	void unselect();

	/**
	 * Select next node
	 */
	void selectNext();

	/**
	 * Select previous
	 */
	void selectPrevious();

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
