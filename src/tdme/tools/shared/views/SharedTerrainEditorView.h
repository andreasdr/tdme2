#pragma once

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/prototype/fwd-tdme.h>
#include <tdme/gui/events/GUIInputEventHandler.h>
#include <tdme/math/Vector3.h>
#include <tdme/tools/shared/controller/fwd-tdme.h>
#include <tdme/tools/shared/views/fwd-tdme.h>
#include <tdme/tools/shared/views/View.h>

using tdme::engine::prototype::Prototype;
using tdme::engine::Engine;
using tdme::gui::events::GUIInputEventHandler;
using tdme::math::Vector3;
using tdme::tools::shared::controller::TerrainEditorScreenController;
using tdme::tools::shared::views::CameraInputHandler;
using tdme::tools::shared::views::PopUps;
using tdme::tools::shared::views::View;

/**
 * Shared terrain editor View
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::shared::views::SharedTerrainEditorView
	: public View
	, public GUIInputEventHandler
{
protected:
	Engine* engine { nullptr };

private:
	PopUps* popUps { nullptr };
	TerrainEditorScreenController* terrainEditorScreenController { nullptr };
	Prototype* prototype { nullptr };
	bool initModelRequested { false };
	CameraInputHandler* cameraInputHandler { nullptr };
	bool brushingEnabled { false };
	Vector3 brushCenterPosition;

	/**
	 * Init entity
	 */
	void initModel();

public:
	/**
	 * Public constructor
	 * @param popUps pop ups view
	 */
	SharedTerrainEditorView(PopUps* popUps);

	/**
	 * Destructor
	 */
	~SharedTerrainEditorView();

	/**
	 * @return pop up views
	 */
	PopUps* getPopUpsViews();

	/**
	 * @return prototype
	 */
	Prototype* getPrototype();

	/**
	 * Set prototype
	 * @param prototype prototype
	 */
	void setPrototype(Prototype* entity);

	/**
	 * Init GUI elements
	 */
	void updateGUIElements();

	// overridden methods
	void handleInputEvents() override;
	void display() override;
	void initialize() override;
	void activate() override;
	void deactivate() override;
	void dispose() override;

	/**
	 * On set prototype data hook
	 */
	virtual void onSetPrototypeData();

	/**
	 * On init additional screens
	 */
	virtual void onInitAdditionalScreens();

};
