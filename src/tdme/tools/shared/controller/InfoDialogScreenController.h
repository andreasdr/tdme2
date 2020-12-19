
#pragma once

#include <string>

#include <tdme/gui/events/fwd-tdme.h>
#include <tdme/gui/events/GUIActionListener.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/tools/shared/controller/fwd-tdme.h>
#include <tdme/tools/shared/controller/ScreenController.h>
#include <tdme/utilities/fwd-tdme.h>

using std::string;

using tdme::gui::events::GUIActionListener;
using tdme::gui::events::GUIActionListenerType;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUIMultilineTextNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::nodes::GUITextNode;
using tdme::tools::shared::controller::ScreenController;
using tdme::utilities::MutableString;

/**
 * Info dialog screen controller
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::shared::controller::InfoDialogScreenController final
	: public ScreenController
	, public virtual GUIActionListener
{

private:
	GUIScreenNode* screenNode { nullptr };
	GUITextNode* captionNode { nullptr };
	GUIMultilineTextNode* messageNode { nullptr };

public:
	/**
	 * Public constructor
	 */
	InfoDialogScreenController();

	GUIScreenNode* getScreenNode() override;
	void initialize() override;
	void dispose() override;
	void onActionPerformed(GUIActionListenerType type, GUIElementNode* node) override;

	/**
	 * Shows the pop up
	 */
	void show(const string& caption, const string& message);
	/**
	 * Closes the pop up
	 */
	void close();

};
