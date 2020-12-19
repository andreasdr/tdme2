#include <tdme/tools/shared/views/CameraRotationInputHandler.h>

#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/Camera.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Entity.h>
#include <tdme/engine/Transformations.h>
#include <tdme/gui/events/GUIKeyboardEvent.h>
#include <tdme/gui/events/GUIMouseEvent.h>
#include <tdme/gui/GUI.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Quaternion.h>
#include <tdme/math/Vector3.h>
#include <tdme/tools/shared/views/CameraRotationInputHandlerEventHandler.h>
#include <tdme/utilities/Character.h>

using tdme::engine::Camera;
using tdme::engine::Engine;
using tdme::engine::Transformations;
using tdme::gui::events::GUIKeyboardEvent;
using tdme::gui::events::GUIMouseEvent;
using tdme::gui::GUI;
using tdme::math::Matrix4x4;
using tdme::math::Quaternion;
using tdme::math::Vector3;
using tdme::tools::shared::views::CameraRotationInputHandler;
using tdme::tools::shared::views::CameraRotationInputHandlerEventHandler;
using tdme::utilities::Character;

CameraRotationInputHandler::CameraRotationInputHandler(Engine* engine, CameraRotationInputHandlerEventHandler* eventHandler)
{
	this->engine = engine;
	this->eventHandler = eventHandler;
	keyLeft = false;
	keyRight = false;
	keyUp = false;
	keyDown = false;
	keyPeriod = false;
	keyComma = false;
	keyPlus = false;
	keyMinus = false;
	keyR = false;
	mouseDragging = false;
	maxAxisDimension = 0.0f;
	scale = 1.0f;
	lookFromRotations.addRotation(Vector3(0.0f, 1.0f, 0.0f), -45.0f);
	lookFromRotations.addRotation(Vector3(1.0f, 0.0f, 0.0f), -45.0f);
	lookFromRotations.addRotation(Vector3(0.0f, 0.0f, 1.0f), 0.0f);
	lookFromRotations.update();
	resetRequested = false;
	mouseLastX = 0;
	mouseLastY = 0;
}

CameraRotationInputHandler::~CameraRotationInputHandler() {
}

float CameraRotationInputHandler::getMaxAxisDimension()
{
	return maxAxisDimension;
}

void CameraRotationInputHandler::setMaxAxisDimension(float maxAxisDimension)
{
	this->maxAxisDimension = maxAxisDimension;
}

const Transformations& CameraRotationInputHandler::getLookFromRotations()
{
	return lookFromRotations;
}

float CameraRotationInputHandler::getScale()
{
	return scale;
}

void CameraRotationInputHandler::setScale(float scale)
{
	this->scale = scale;
}

void CameraRotationInputHandler::reset()
{
	resetRequested = true;
}

void CameraRotationInputHandler::handleInputEvents()
{
	auto scaling = false;
	for (auto i = 0; i < engine->getGUI()->getMouseEvents().size(); i++) {
		auto& event = engine->getGUI()->getMouseEvents()[i];
		if (event.isProcessed() == true) continue;
		if (mouseDragging == true) {
			if (event.getButton() == MOUSE_BUTTON_LEFT) {
				auto xMoved = (event.getX() - mouseLastX) / 5.0f;
				auto yMoved = (event.getY() - mouseLastY) / 5.0f;
				mouseLastX = event.getX();
				mouseLastY = event.getY();
				auto& xRotation = lookFromRotations.getRotation(0);
				auto& yRotation = lookFromRotations.getRotation(1);
				auto xRotationAngle = xRotation.getAngle() + xMoved;
				auto yRotationAngle = yRotation.getAngle() + yMoved;
				xRotation.setAngle(xRotationAngle);
				yRotation.setAngle(yRotationAngle);
				lookFromRotations.update();
			} else {
				mouseDragging = false;
			}
		} else {
			if (event.getButton() == MOUSE_BUTTON_LEFT) {
				mouseDragging = true;
				mouseLastX = event.getX();
				mouseLastY = event.getY();
			}
		}
		auto mouseWheel = event.getWheelY();
		if (mouseWheel != 0) {
			scale += mouseWheel * 0.1f;
			if (scale < 0.0001f) scale = 0.0001f;
			scaling = true;
		}
	}
	for (auto i = 0; i < engine->getGUI()->getKeyboardEvents().size(); i++) {
		auto& event = engine->getGUI()->getKeyboardEvents()[i];
		if (event.isProcessed() == true) continue;
		if (event.getType() == GUIKeyboardEvent::KEYBOARDEVENT_KEY_TYPED) continue;
		auto isKeyDown = event.getType() == GUIKeyboardEvent::KEYBOARDEVENT_KEY_PRESSED;
		if (event.getKeyCode() == GUIKeyboardEvent::KEYCODE_LEFT)
			keyLeft = isKeyDown;

		if (event.getKeyCode() == GUIKeyboardEvent::KEYCODE_RIGHT)
			keyRight = isKeyDown;

		if (event.getKeyCode() == GUIKeyboardEvent::KEYCODE_UP)
			keyUp = isKeyDown;

		if (event.getKeyCode() == GUIKeyboardEvent::KEYCODE_DOWN)
			keyDown = isKeyDown;

		if (Character::toLowerCase(event.getKeyChar()) == '.')
			keyPeriod = isKeyDown;

		if (Character::toLowerCase(event.getKeyChar()) == ',')
			keyComma = isKeyDown;

		if (Character::toLowerCase(event.getKeyChar()) == '+')
			keyPlus = isKeyDown;

		if (Character::toLowerCase(event.getKeyChar()) == '-')
			keyMinus = isKeyDown;

		if (Character::toLowerCase(event.getKeyChar()) == 'r')
			keyR = isKeyDown;

	}

	auto entity = engine->getEntity("model");

	auto& rotationX = lookFromRotations.getRotation(0);
	auto& rotationY = lookFromRotations.getRotation(1);
	auto& rotationZ = lookFromRotations.getRotation(2);

	if (keyLeft == true) rotationX.setAngle(rotationX.getAngle() - 1.0f);
	if (keyRight == true) rotationX.setAngle(rotationX.getAngle() + 1.0f);
	if (keyUp == true) rotationY.setAngle(rotationY.getAngle() + 1.0f);
	if (keyDown == true) rotationY.setAngle(rotationY.getAngle() - 1.0f);
	if (keyComma == true) rotationZ.setAngle(rotationZ.getAngle() - 1.0f);
	if (keyPeriod == true) rotationZ.setAngle(rotationZ.getAngle() + 1.0f);

	if (keyMinus == true) {
		scaling = true;
		scale += 0.05f;
	}
	if (keyPlus  == true) {
		scaling = true;
		scale -= 0.05f;
		if (scale < 0.0001f) scale = 0.0001f;
	}

	if (keyR == true || resetRequested == true) {
		rotationY.setAngle(-45.0f);
		rotationZ.setAngle(0.0f);
		scale = 1.0f;
	}
	if (keyLeft == true ||
		keyRight == true ||
		keyUp == true ||
		keyDown == true ||
		keyComma == true ||
		keyPeriod == true ||
		keyR == true ||
		resetRequested) {
		lookFromRotations.update();
		if (entity != nullptr) {
			boundingBoxTransformed = *entity->getBoundingBoxTransformed();
		}
	}
	auto cam = engine->getCamera();
	auto lookAt = cam->getLookAt();
	if (resetRequested == true) {
		if (entity != nullptr) {
			auto entityBoundingBoxTransformed = entity->getBoundingBoxTransformed();
			for (auto i = 0; i < 3; i++) {
				if (entityBoundingBoxTransformed->getMin()[i] < boundingBoxTransformed.getMin()[i]) boundingBoxTransformed.getMin()[i] = entityBoundingBoxTransformed->getMin()[i];
				if (entityBoundingBoxTransformed->getMax()[i] > boundingBoxTransformed.getMax()[i]) boundingBoxTransformed.getMax()[i] = entityBoundingBoxTransformed->getMax()[i];
			}
			boundingBoxTransformed.update();
			lookAt.set(boundingBoxTransformed.getCenter());
		} else {
			lookAt.set(0.0f, 0.0f, 0.0f);
		}
		resetRequested = false;
	}
	Vector3 forwardVector(0.0f, 0.0f, 1.0f);
	Vector3 forwardVectorTransformed;
	Vector3 upVector;
	lookFromRotations.getTransformationsMatrix().multiply(forwardVector, forwardVectorTransformed).scale(scale);
	lookFromRotations.getRotation(2).getQuaternion().multiply(Vector3(0.0f, 1.0f, 0.0f), upVector).normalize();
	auto lookFrom = lookAt.clone().add(forwardVectorTransformed);
	cam->setLookFrom(lookFrom);
	cam->setLookAt(lookAt);
	cam->setUpVector(upVector);

	if (scaling == true && eventHandler != nullptr) eventHandler->onScale();
	if ((keyLeft == true ||
		keyRight == true ||
		keyUp == true ||
		keyDown == true ||
		keyComma == true ||
		keyPeriod == true ||
		keyR == true ||
		resetRequested == true) &&
		eventHandler != nullptr) {
		if (eventHandler != nullptr) eventHandler->onRotation();
	}
}
