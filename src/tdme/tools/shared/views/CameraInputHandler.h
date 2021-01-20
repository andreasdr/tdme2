#pragma once

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/Rotation.h>
#include <tdme/gui/events/GUIInputEventHandler.h>
#include <tdme/math/Vector3.h>
#include <tdme/tools/shared/views/fwd-tdme.h>

using tdme::engine::Engine;
using tdme::engine::Rotation;
using tdme::gui::events::GUIInputEventHandler;
using tdme::math::Vector3;
using tdme::tools::shared::views::CameraInputHandlerEventHandler;

/**
 * Camera Input Handler
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::shared::views::CameraInputHandler final
	: public GUIInputEventHandler
{
private:
	Engine* engine { nullptr };
	CameraInputHandlerEventHandler* eventHandler { nullptr };

	bool keyLeft { false };
	bool keyRight { false };
	bool keyUp { false };
	bool keyDown { false };
	bool keyA { false };
	bool keyD { false };
	bool keyW { false };
	bool keyS { false };
	bool keyPlus { false };
	bool keyMinus { false };
	bool keyR { false };

	static constexpr int MOUSE_DOWN_LAST_POSITION_NONE { -1 };
	static constexpr int MOUSE_PANNING_NONE { 0 };
	static constexpr int MOUSE_ROTATION_NONE { 0 };

	int mouseRotationX { 0 };
	int mouseRotationY { 0 };
	int mouseDownLastX { MOUSE_DOWN_LAST_POSITION_NONE };
	int mouseDownLastY { MOUSE_DOWN_LAST_POSITION_NONE };
	bool mouseDragging { false };

	float camScale { 1.0f };
	float camScaleMax { 15.0f };
	float camScaleMin { 0.05f };

	Rotation camLookRotationX;
	Rotation camLookRotationY;

	Vector3 sceneCenter;

	bool resetRequested { false };

public:
	/**
	 * Public constructor
	 * @param engine engine
	 * @param eventHandler event handler
	 */
	CameraInputHandler(Engine* engine, CameraInputHandlerEventHandler* eventHandler = nullptr);

	/**
	 * Destructor
	 */
	~CameraInputHandler();

	/**
	 * @return scene center
	 */
	inline const Vector3& getSceneCenter() {
		return sceneCenter;
	}

	/**
	 * Set scene center
	 * @param sceneCenter scene center
	 */
	inline void setSceneCenter(const Vector3& sceneCenter) {
		this->sceneCenter = sceneCenter;
	}

	/**
	 * Reset
	 */
	void reset();

	// overriden methods
	void handleInputEvents() override;

};
