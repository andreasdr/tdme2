#pragma once

#include <tdme/tdme.h>
#include <tdme/gui/events/fwd-tdme.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/gui/nodes/GUINodeController.h>
#include <tdme/utilities/MutableString.h>

using tdme::gui::events::GUIKeyboardEvent;
using tdme::gui::events::GUIMouseEvent;
using tdme::gui::nodes::GUILayoutNode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeController;
using tdme::gui::nodes::GUIVerticalScrollbarInternalController_State;
using tdme::utilities::MutableString;

/**
 * GUI vertical scroll bar internal controller
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::gui::nodes::GUIVerticalScrollbarInternalController
	: public GUINodeController
{
	friend class GUIVerticalScrollbarInternalNode;
public:
	enum State {STATE_NONE, STATE_MOUSEOVER, STATE_DRAGGING};

private:
	GUILayoutNode* contentNode { nullptr };
	float contentHeight;
	State state;
	int32_t mouseYOffset;
	MutableString value;

	/**
	 * Private constructor
	 * @param node node
	 */
	GUIVerticalScrollbarInternalController(GUINode* node);

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

	/**
	 * @return state
	 */
	virtual State getState();

private:

	/**
	 * @return bar height
	 */
	virtual float getBarHeight();

	/**
	 * @return bar top
	 */
	virtual float getBarTop();

	/**
	 * Set dragged y
	 * @param draggedY dragged y
	 */
	virtual void setDraggedY(float draggedY);

};
