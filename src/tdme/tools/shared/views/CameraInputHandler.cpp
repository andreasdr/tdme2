#include <tdme/tools/shared/views/CameraInputHandler.h>

#include <tdme/engine/Camera.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Rotation.h>
#include <tdme/engine/Timing.h>
#include <tdme/gui/events/GUIKeyboardEvent.h>
#include <tdme/gui/events/GUIMouseEvent.h>
#include <tdme/gui/GUI.h>
#include <tdme/math/Quaternion.h>
#include <tdme/math/Vector3.h>
#include <tdme/tools/shared/views/CameraInputHandlerEventHandler.h>
#include <tdme/utilities/Character.h>

using tdme::engine::Camera;
using tdme::engine::Engine;
using tdme::engine::Rotation;
using tdme::engine::Timing;
using tdme::gui::events::GUIKeyboardEvent;
using tdme::gui::events::GUIMouseEvent;
using tdme::gui::GUI;
using tdme::math::Quaternion;
using tdme::math::Vector3;
using tdme::tools::shared::views::CameraInputHandler;
using tdme::tools::shared::views::CameraInputHandlerEventHandler;
using tdme::utilities::Character;

CameraInputHandler::CameraInputHandler(Engine* engine, CameraInputHandlerEventHandler* eventHandler):
	camLookRotationX(Vector3(1.0f, 0.0f, 0.0f), -45.0f),
	camLookRotationY(Vector3(0.0f, 1.0f, 0.0f), 0.0f)
{
	this->engine = engine;
	this->eventHandler = eventHandler;
	reset();
}

CameraInputHandler::~CameraInputHandler() {
}


void CameraInputHandler::reset() {
	resetRequested = true;
	camLookRotationX.setAngle(-45.0f);
	camLookRotationX.update();
	camLookRotationY.setAngle(0.0f);
	camLookRotationY.update();
	camScale = 1.0f;
}

void CameraInputHandler::handleInputEvents() {
	for (auto i = 0; i < engine->getGUI()->getKeyboardEvents().size(); i++) {
		auto& event = engine->getGUI()->getKeyboardEvents()[i];
		if (event.isProcessed() == true) continue;
		if (event.getType() == GUIKeyboardEvent::KEYBOARDEVENT_KEY_TYPED) continue;
		auto isKeyDown = event.getType() == GUIKeyboardEvent::KEYBOARDEVENT_KEY_PRESSED;

		//
		if (event.getKeyCode() == GUIKeyboardEvent::KEYCODE_LEFT) {
			keyLeft = isKeyDown;
			event.setProcessed(true);
		}
		if (event.getKeyCode() == GUIKeyboardEvent::KEYCODE_RIGHT) {
			keyRight = isKeyDown;
			event.setProcessed(true);
		}
		if (event.getKeyCode() == GUIKeyboardEvent::KEYCODE_UP) {
			keyUp = isKeyDown;
			event.setProcessed(true);
		}
		if (event.getKeyCode() == GUIKeyboardEvent::KEYCODE_DOWN) {
			keyDown = isKeyDown;
			event.setProcessed(true);
		}
		if (Character::toLowerCase(event.getKeyChar()) == 'a') {
			keyA = isKeyDown;
			event.setProcessed(true);
		}
		if (Character::toLowerCase(event.getKeyChar()) == 'd') {
			keyD = isKeyDown;
			event.setProcessed(true);
		}
		if (Character::toLowerCase(event.getKeyChar()) == 'w') {
			keyW = isKeyDown;
			event.setProcessed(true);
		}
		if (Character::toLowerCase(event.getKeyChar()) == 's') {
			keyS = isKeyDown;
			event.setProcessed(true);
		}
		if (Character::toLowerCase(event.getKeyChar()) == '+') {
			keyPlus = isKeyDown;
			event.setProcessed(true);
		}
		if (Character::toLowerCase(event.getKeyChar()) == '-') {
			keyMinus = isKeyDown;
			event.setProcessed(true);
		}
		if (Character::toLowerCase(event.getKeyChar()) == 'r') {
			keyR = isKeyDown;
			event.setProcessed(true);
		}
	}

	auto mousePanningSide = MOUSE_PANNING_NONE;
	auto mousePanningForward = MOUSE_PANNING_NONE;
	for (auto i = 0; i < engine->getGUI()->getMouseEvents().size(); i++) {
		auto& event = engine->getGUI()->getMouseEvents()[i];

		if (event.isProcessed() == true) continue;

		if (event.getType() == GUIMouseEvent::MOUSEEVENT_DRAGGED) {
			if (mouseDragging == false) {
				mouseDragging = true;
				mouseDownLastX = event.getXUnscaled();
				mouseDownLastY = event.getYUnscaled();
				event.setProcessed(true);
			}
		} else {
			if (mouseDragging == true) {
				mouseDownLastX = MOUSE_DOWN_LAST_POSITION_NONE;
				mouseDownLastY = MOUSE_DOWN_LAST_POSITION_NONE;
				mouseDragging = false;
				event.setProcessed(true);
			}
		}
		if (event.getButton() == MOUSE_BUTTON_RIGHT && mouseDragging == true) {
			if (mouseDownLastX != MOUSE_DOWN_LAST_POSITION_NONE && mouseDownLastY != MOUSE_DOWN_LAST_POSITION_NONE) {
				mousePanningSide = event.getXUnscaled() - mouseDownLastX;
				mousePanningForward = event.getYUnscaled() - mouseDownLastY;
			}
			event.setProcessed(true);
		} else
		if (event.getButton() == MOUSE_BUTTON_MIDDLE) {
			if (mouseDownLastX != MOUSE_DOWN_LAST_POSITION_NONE && mouseDownLastY != MOUSE_DOWN_LAST_POSITION_NONE) {
				mouseRotationX = event.getXUnscaled() - mouseDownLastX;
				mouseRotationY = event.getYUnscaled() - mouseDownLastY;
			}
			event.setProcessed(true);
		}
		if (event.getButton() != MOUSE_BUTTON_NONE) {
			mouseDownLastX = event.getXUnscaled();
			mouseDownLastY = event.getYUnscaled();
			event.setProcessed(true);
		}
		auto mouseWheel = event.getWheelY();
		if (mouseWheel != 0) {
			camScale += -mouseWheel * 0.1f;
			if (camScale < camScaleMin) camScale = camScaleMin;
			if (camScale > camScaleMax) camScale = camScaleMax;
			event.setProcessed(true);
		}
	}

	//
	auto cam = engine->getCamera();
	if (mouseRotationX != MOUSE_ROTATION_NONE) {
		camLookRotationY.setAngle(camLookRotationY.getAngle() + mouseRotationX);
		camLookRotationY.update();
		mouseRotationX = 0;
		if (eventHandler != nullptr) eventHandler->onCameraRotation();
	}
	if (mouseRotationY != MOUSE_ROTATION_NONE) {
		camLookRotationX.setAngle(camLookRotationX.getAngle() + mouseRotationY);
		camLookRotationX.update();
		mouseRotationY = 0;
		if (eventHandler != nullptr) eventHandler->onCameraRotation();
	}
	if (keyA == true) camLookRotationY.setAngle(camLookRotationY.getAngle() + 1.0f);
	if (keyD == true) camLookRotationY.setAngle(camLookRotationY.getAngle() - 1.0f);
	if (keyW == true) camLookRotationX.setAngle(camLookRotationX.getAngle() + 1.0f);
	if (keyS == true) camLookRotationX.setAngle(camLookRotationX.getAngle() - 1.0f);
	if (keyMinus == true || keyPlus == true) {
		if (keyMinus == true) camScale += 0.05f;
		if (keyPlus == true) camScale -= 0.05f;
		if (camScale < camScaleMin) camScale = camScaleMin;
		if (camScale > camScaleMax) camScale = camScaleMax;
		if (eventHandler != nullptr) eventHandler->onCameraScale();
	}

	if (keyR == true || resetRequested == true) {
		camLookRotationX.setAngle(-45.0f);
		camLookRotationX.update();
		camLookRotationY.setAngle(0.0f);
		camLookRotationY.update();
		cam->setLookAt(sceneCenter);
		camScale = 1.0f;
		if (eventHandler != nullptr) eventHandler->onCameraTranslation();
		resetRequested = false;
		keyR = false;
	}
	if (keyA == true|| keyD == true) {
		camLookRotationY.update();
		if (eventHandler != nullptr) eventHandler->onCameraTranslation();
	}

	if (keyW == true || keyS == true) {
		if (camLookRotationX.getAngle() > 89.99f) camLookRotationX.setAngle(89.99f);
		if (camLookRotationX.getAngle() < -89.99f) camLookRotationX.setAngle(-89.99f);
		camLookRotationX.update();
		if (eventHandler != nullptr) eventHandler->onCameraTranslation();
	}

	//
	Vector3 forwardVector(0.0f, 0.0f, 1.0f);
	Vector3 sideVector(1.0f, 0.0f, 0.0f);
	auto forwardVectorTransformed = camLookRotationX.getQuaternion().multiply(forwardVector);
	forwardVectorTransformed = camLookRotationY.getQuaternion().multiply(forwardVectorTransformed);
	auto camLookAtToFromVector = forwardVectorTransformed.clone().scale(camScale * 10.0f);
	auto camForwardVector = camLookRotationY.getQuaternion().multiply(forwardVector).scale(engine->getTiming()->getDeltaTime() / 1000.0f * 60.0f);
	auto camSideVector = camLookRotationY.getQuaternion().multiply(sideVector).scale(engine->getTiming()->getDeltaTime() / 1000.0f * 60.0f);

	auto camLookAt = cam->getLookAt();
	if (keyUp == true) camLookAt.sub(forwardVectorTransformed.set(camForwardVector).scale(0.1f));
	if (keyDown == true) camLookAt.add(forwardVectorTransformed.set(camForwardVector).scale(0.1f));
	if (keyLeft == true) camLookAt.sub(forwardVectorTransformed.set(camSideVector).scale(0.1f));
	if (keyRight == true) camLookAt.add(forwardVectorTransformed.set(camSideVector).scale(0.1f));
	if (mousePanningForward != MOUSE_PANNING_NONE) {
		camLookAt.sub(forwardVectorTransformed.set(camForwardVector).scale(mousePanningForward / 30.0f * camScale));
		mousePanningForward = MOUSE_PANNING_NONE;
		if (eventHandler != nullptr) eventHandler->onCameraTranslation();
	}
	if (mousePanningSide != MOUSE_PANNING_NONE) {
		camLookAt.sub(forwardVectorTransformed.set(camSideVector).scale(mousePanningSide / 30.0f * camScale));
		mousePanningSide = MOUSE_PANNING_NONE;
		if (eventHandler != nullptr) eventHandler->onCameraTranslation();
	}
	cam->setLookAt(camLookAt);
	cam->setLookFrom(cam->getLookAt().clone().add(camLookAtToFromVector));
	cam->setUpVector(cam->computeUpVector(cam->getLookFrom(), cam->getLookAt()));
}
