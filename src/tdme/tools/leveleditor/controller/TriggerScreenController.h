
#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/gui/events/fwd-tdme.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/tools/leveleditor/controller/fwd-tdme.h>
#include <tdme/tools/leveleditor/views/fwd-tdme.h>
#include <tdme/tools/shared/controller/fwd-tdme.h>
#include <tdme/utilities/fwd-tdme.h>
#include <tdme/tools/shared/controller/fwd-tdme.h>
#include <tdme/tools/shared/controller/ScreenController.h>
#include <tdme/gui/events/GUIActionListener.h>
#include <tdme/gui/events/GUIChangeListener.h>

using std::string;

using tdme::tools::shared::controller::ScreenController;
using tdme::gui::events::GUIActionListener;
using tdme::gui::events::GUIChangeListener;
using tdme::gui::events::GUIActionListener_Type;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::nodes::GUITextNode;
using tdme::tools::leveleditor::views::TriggerView;
using tdme::tools::shared::controller::EntityBaseSubScreenController;
using tdme::tools::shared::controller::EntityPhysicsSubScreenController;
using tdme::tools::shared::controller::FileDialogPath;
using tdme::utilities::MutableString;

/** 
 * Trigger screen controller
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::leveleditor::controller::TriggerScreenController final
	: public ScreenController
	, public GUIActionListener
	, public GUIChangeListener
{
private:
	static MutableString TEXT_EMPTY;
	EntityBaseSubScreenController* entityBaseSubScreenController { nullptr };
	EntityPhysicsSubScreenController* entityPhysicsSubScreenController { nullptr };
	TriggerView* view { nullptr };
	FileDialogPath* modelPath { nullptr };
	GUIScreenNode* screenNode { nullptr };
	GUITextNode* screenCaption { nullptr };
	GUIElementNode* viewPort { nullptr };

public:
	/**
	 * @return entity physics sub screen controller
	 */
	EntityPhysicsSubScreenController* getEntityPhysicsSubScreenController();

	// overriden methods
	GUIScreenNode* getScreenNode() override;
	void initialize() override;
	void dispose() override;

	/** 
	 * Set screen caption
	 * @param text text
	 */
	void setScreenCaption(const string& text);

	/** 
	 * Set up general entity data
	 * @param name name
	 * @param description description
	 */
	void setEntityData(const string& name, const string& description);

	/** 
	 * Unset entity data
	 */
	void unsetEntityData();

	/** 
	 * Set up entity properties
	 * @param presetId preset id
	 * @param selectedName selected name
	 */
	void setEntityProperties(const string& presetId, const string& selectedName);

	/** 
	 * Unset entity properties
	 */
	void unsetEntityProperties();

	/** 
	 * On quit
	 */
	void onQuit();

	/** 
	 * Shows the error pop up
	 */
	void showErrorPopUp(const string& caption, const string& message);
	void onValueChanged(GUIElementNode* node) override;
	void onActionPerformed(GUIActionListener_Type* type, GUIElementNode* node) override;

	/**
	 * Get viewport rectangle
	 * @param left left
	 * @param top top
	 * @param width width
	 * @param height height
	 */
	void getViewPort(int& left, int& top, int& width, int& height);

	/**
	 * Public constructor
	 * @param view view
	 */
	TriggerScreenController(TriggerView* view);
};
