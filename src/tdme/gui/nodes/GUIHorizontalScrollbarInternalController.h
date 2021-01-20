#pragma once

#include <tdme/tdme.h>
#include <tdme/gui/events/fwd-tdme.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/utilities/fwd-tdme.h>
#include <tdme/utilities/MutableString.h>
#include <tdme/gui/nodes/GUINodeController.h>

using tdme::gui::nodes::GUINodeController;
using tdme::gui::events::GUIKeyboardEvent;
using tdme::gui::events::GUIMouseEvent;
using tdme::gui::nodes::GUIHorizontalScrollbarInternalController_State;
using tdme::gui::nodes::GUILayoutNode;
using tdme::gui::nodes::GUINode;
using tdme::utilities::MutableString;

/**
 * GUI scroll bar controller
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::gui::nodes::GUIHorizontalScrollbarInternalController
	: public GUINodeController
{
	friend class GUIHorizontalScrollbarInternalNode;
public:
	enum State { STATE_NONE, STATE_MOUSEOVER, STATE_DRAGGING };

private:
	GUILayoutNode* contentNode { nullptr };
	float contentWidth;
	State state { STATE_NONE };
	int32_t mouseXOffset;
	MutableString value;

	/**
	 * @return bar width
	 */
	virtual float getBarWidth();

	/**
	 * @return bar left
	 */
	virtual float getBarLeft();

	/**
	 * Set dragged x
	 * @param draggedX dragged x
	 */
	virtual void setDraggedX(float draggedX);

protected:
	/**
	 * Constructor
	 * @param node node
	 */
	GUIHorizontalScrollbarInternalController(GUINode* node);

public:
	/**
	 * @return state
	 */
	virtual State getState();

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
