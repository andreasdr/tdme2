
#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/gui/events/fwd-tdme.h>
#include <tdme/gui/events/GUIActionListener.h>
#include <tdme/gui/events/GUIChangeListener.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/tools/sceneeditor/controller/fwd-tdme.h>
#include <tdme/tools/sceneeditor/views/fwd-tdme.h>
#include <tdme/tools/shared/controller/fwd-tdme.h>
#include <tdme/tools/shared/controller/fwd-tdme.h>
#include <tdme/tools/shared/controller/ScreenController.h>
#include <tdme/utilities/fwd-tdme.h>

using std::string;

using tdme::gui::events::GUIActionListener;
using tdme::gui::events::GUIActionListenerType;
using tdme::gui::events::GUIChangeListener;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::nodes::GUITextNode;
using tdme::tools::sceneeditor::views::EnvironmentMappingView;
using tdme::tools::shared::controller::FileDialogPath;
using tdme::tools::shared::controller::PrototypeBaseSubScreenController;
using tdme::tools::shared::controller::PrototypePhysicsSubScreenController;
using tdme::tools::shared::controller::ScreenController;
using tdme::utilities::MutableString;

/**
 * Environment mapping screen controller
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::sceneeditor::controller::EnvironmentMappingScreenController final
	: public ScreenController
	, public GUIActionListener
	, public GUIChangeListener
{
private:
	static MutableString TEXT_EMPTY;
	PrototypeBaseSubScreenController* prototypeBaseSubScreenController { nullptr };
	PrototypePhysicsSubScreenController* prototypePhysicsSubScreenController { nullptr };
	EnvironmentMappingView* view { nullptr };
	FileDialogPath* modelPath { nullptr };
	GUIScreenNode* screenNode { nullptr };
	GUITextNode* screenCaption { nullptr };
	GUIElementNode* generationRenderPasses { nullptr };
	GUIElementNode* generationFrequency { nullptr };
	GUIElementNode* generationApply { nullptr };
	GUIElementNode* viewPort { nullptr };

public:
	/**
	 * Public constructor
	 * @param view view
	 */
	EnvironmentMappingScreenController(EnvironmentMappingView* view);

	/**
	 * @return entity physics sub screen controller
	 */
	PrototypePhysicsSubScreenController* getPrototypePhysicsSubScreenController();

	// overridden methods
	GUIScreenNode* getScreenNode() override;
	void initialize() override;
	void dispose() override;

	/**
	 * Set screen caption
	 * @param text text
	 */
	void setScreenCaption(const string& text);

	/**
	 * Set up general prototype data
	 * @param name name
	 * @param description description
	 */
	void setPrototypeData(const string& name, const string& description);

	/**
	 * Unset prototype data
	 */
	void unsetPrototypeData();

	/**
	 * Set up prototype properties
	 * @param presetId preset id
	 * @param selectedName selected name
	 */
	void setPrototypeProperties(const string& presetId, const string& selectedName);

	/**
	 * Unset prototype properties
	 */
	void unsetPrototypeProperties();

	/**
	 * Set generation properties
	 */
	void setGeneration();

	/**
	 * Unset generation properties
	 */
	void unsetGeneration();

	/**
	 * On generation apply
	 */
	void onGenerationApply();

	// overridden methods
	void onValueChanged(GUIElementNode* node) override;
	void onActionPerformed(GUIActionListenerType type, GUIElementNode* node) override;

	/**
	 * Get viewport rectangle
	 * @param left left
	 * @param top top
	 * @param width width
	 * @param height height
	 */
	void getViewPort(int& left, int& top, int& width, int& height);

	/**
	 * Shows the error pop up
	 * @param caption caption
	 * @param message message
	 */
	void showErrorPopUp(const string& caption, const string& message);

	/**
	 * On quit
	 */
	void onQuit();
};
