#pragma once

#include <array>

#include <tdme/tdme.h>
#include <tdme/gui/events/fwd-tdme.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/gui/nodes/GUINodeController.h>
#include <tdme/utilities/MutableString.h>

using std::array;

using tdme::gui::events::GUIKeyboardEvent;
using tdme::gui::events::GUIMouseEvent;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUIInputInternalController_CursorMode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeController;
using tdme::utilities::MutableString;

/**
 * GUI input internal controller
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::gui::nodes::GUIInputInternalController final
	: public GUINodeController
{
	friend class GUIInputInternalNode;
	friend class GUIInputInternalController_CursorMode;
public:
	enum CursorMode { CURSORMODE_HIDE, CURSORMODE_SHOW};
private:
	static constexpr int64_t CURSOR_MODE_DURATION { 500LL };
	static constexpr int64_t DRAGGING_CALMDOWN { 50LL };
	GUIElementNode* inputNode { nullptr };
	int64_t cursorModeStarted { -1LL };
	CursorMode cursorMode { CURSORMODE_SHOW };
	int index { 0 };
	int offset { 0 };
	bool draggingInit { false };
	bool draggingActive { false };
	array<int, 2> dragPosition;
	MutableString value;

	enum Type { TYPE_STRING, TYPE_FLOAT, TYPE_INT};
	Type type { TYPE_STRING };
	bool haveMin { false };
	bool haveMax { false };
	bool haveStep { false };
	float min { 0.0f };
	float max { 0.0f };
	float step { 0.0f };
	int decimals { 3 };

	bool showCursor { false };

	/**
	 * Private constructor
	 * @param node node
	 */
	GUIInputInternalController(GUINode* node);

	/**
	 * @return index
	 */
	int getIndex();

	/**
	 * @return offset
	 */
	int getOffset();

	/**
	 * Reset cursor mode
	 */
	void resetCursorMode();

	/**
	 * @return cursor mode
	 */
	CursorMode getCursorMode();

	/**
	 * Check and correct offset
	 */
	void checkOffset();

public:
	/**
	 * Reset cursor index and offset
	 */
	void reset();

	/**
	 * @return show cursor
	 */
	bool isShowCursor();

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
