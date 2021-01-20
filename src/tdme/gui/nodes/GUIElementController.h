#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/gui/events/fwd-tdme.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/gui/nodes/GUINodeController.h>
#include <tdme/utilities/MutableString.h>

using std::string;

using tdme::gui::events::GUIKeyboardEvent;
using tdme::gui::events::GUIMouseEvent;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeController;
using tdme::utilities::MutableString;

/**
 * GUI element controller
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::gui::nodes::GUIElementController
	: public GUINodeController
{
	friend class GUIElementNode;

private:
	static constexpr int64_t TIME_DOUBLECLICK { 200LL };

	bool disabled;
	bool isActionPerforming;
	bool initialized;
	MutableString value;
	int64_t timeLastClicked { -1LL };

protected:
	static string CONDITION_DISABLED;
	static string CONDITION_ENABLED;

	/**
	 * Constructor
	 * @param node node
	 */
	GUIElementController(GUINode* node);

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
