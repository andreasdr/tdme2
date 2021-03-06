#pragma once

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/prototype/fwd-tdme.h>
#include <tdme/gui/events/GUIInputEventHandler.h>
#include <tdme/math/Vector3.h>
#include <tdme/tools/sceneeditor/controller/fwd-tdme.h>
#include <tdme/tools/sceneeditor/views/fwd-tdme.h>
#include <tdme/tools/shared/views/fwd-tdme.h>
#include <tdme/tools/shared/views/CameraRotationInputHandlerEventHandler.h>
#include <tdme/tools/shared/views/View.h>

using tdme::engine::prototype::Prototype;
using tdme::engine::Engine;
using tdme::gui::events::GUIInputEventHandler;
using tdme::math::Vector3;
using tdme::tools::sceneeditor::controller::EnvironmentMappingScreenController;
using tdme::tools::shared::views::CameraRotationInputHandler;
using tdme::tools::shared::views::CameraRotationInputHandlerEventHandler;
using tdme::tools::shared::views::PopUps;
using tdme::tools::shared::views::PrototypePhysicsView;
using tdme::tools::shared::views::View;

/**
 * Environment mapping view
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::sceneeditor::views::EnvironmentMappingView final
	: public View
	, public GUIInputEventHandler
	, protected CameraRotationInputHandlerEventHandler
{
private:
	Engine* engine { nullptr };
	PopUps* popUps { nullptr };
	EnvironmentMappingScreenController* environmentMappingScreenController { nullptr };
	PrototypePhysicsView* prototypePhysicsView { nullptr };
	Prototype* prototype { nullptr };
	CameraRotationInputHandler* cameraRotationInputHandler { nullptr };
	Vector3 objectScale;

public:
	/**
	 * Public constructor
	 * @param popUps pop ups view
	 */
	EnvironmentMappingView(PopUps* popUps);

	/**
	 * Destructor
	 */
	~EnvironmentMappingView();

	/**
	 * @return pop ups
	 */
	PopUps* getPopUps();

	/**
	 * @return prototype
	 */
	Prototype* getPrototype();

	/**
	 * Set prototype
	 * @param prototype prototype
	 */
	void setPrototype(Prototype* prototype);

	/**
	 * Set dimension
	 */
	void setDimension(float width, float height, float depth);

	// overridden methods
	void display() override;
	void updateGUIElements();
	void initialize() override;
	void activate() override;
	void deactivate() override;
	void dispose() override;
	void handleInputEvents() override;

	/**
	 * On rotation event to be overloaded
	 */
	void onCameraRotation() override;

	/**
	 * On scale event to be overloaded
	 */
	void onCameraScale() override;

};
