#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/gui/elements/GUISelectBoxMultipleOptionController.h>
#include <tdme/gui/events/fwd-tdme.h>
#include <tdme/gui/events/GUIActionListener.h>
#include <tdme/gui/nodes/fwd-tdme.h>

using std::string;

using tdme::gui::events::GUIActionListener;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUIParentNode;
using tdme::utilities::MutableString;

/**
 * GUI select box multiple parent option controller
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::gui::elements::GUISelectBoxMultipleParentOptionController final: public GUISelectBoxMultipleOptionController
{
	friend class GUISelectBoxMultipleController;
	friend class GUISelectBoxMultipleOptionController;
	friend class GUISelectBoxMultipleParentOption;

private:
	static string CONDITION_OPENED;
	static string CONDITION_CLOSED;

	GUIElementNode* arrowNode { nullptr };
	bool open;
	GUIActionListener* arrowNodeActionListener { nullptr };

	/**
	 * Private constructor
	 * @param node node
	 */
	GUISelectBoxMultipleParentOptionController(GUINode* node);

	/**
	 * Toggle open state
	 */
	void toggleOpenState();

public:
	// overriden methods
	void initialize() override;
	void dispose() override;
};
