#include <tdme/tools/sceneeditor/views/TriggerView.h>

#include <tdme/engine/model/Model.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/prototype/Prototype.h>
#include <tdme/engine/prototype/PrototypeProperty.h>
#include <tdme/engine/scene/Scene.h>
#include <tdme/engine/scene/SceneLibrary.h>
#include <tdme/engine/Camera.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Entity.h>
#include <tdme/engine/PartitionNone.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/gui/GUI.h>
#include <tdme/math/Vector3.h>
#include <tdme/tools/sceneeditor/controller/SceneEditorLibraryScreenController.h>
#include <tdme/tools/sceneeditor/controller/TriggerScreenController.h>
#include <tdme/tools/sceneeditor/TDMESceneEditor.h>
#include <tdme/tools/shared/controller/FileDialogScreenController.h>
#include <tdme/tools/shared/controller/InfoDialogScreenController.h>
#include <tdme/tools/shared/controller/PrototypePhysicsSubScreenController.h>
#include <tdme/tools/shared/tools/Tools.h>
#include <tdme/tools/shared/views/CameraRotationInputHandler.h>
#include <tdme/tools/shared/views/CameraRotationInputHandlerEventHandler.h>
#include <tdme/tools/shared/views/PopUps.h>
#include <tdme/tools/shared/views/PrototypePhysicsView.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>

using tdme::engine::model::Model;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::prototype::Prototype;
using tdme::engine::prototype::PrototypeProperty;
using tdme::engine::scene::Scene;
using tdme::engine::scene::SceneLibrary;
using tdme::engine::Camera;
using tdme::engine::Engine;
using tdme::engine::Entity;
using tdme::engine::PartitionNone;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::GUI;
using tdme::math::Vector3;
using tdme::tools::sceneeditor::controller::SceneEditorLibraryScreenController;
using tdme::tools::sceneeditor::controller::TriggerScreenController;
using tdme::tools::sceneeditor::views::TriggerView;
using tdme::tools::sceneeditor::TDMESceneEditor;
using tdme::tools::shared::controller::FileDialogScreenController;
using tdme::tools::shared::controller::InfoDialogScreenController;
using tdme::tools::shared::controller::PrototypePhysicsSubScreenController;
using tdme::tools::shared::tools::Tools;
using tdme::tools::shared::views::CameraRotationInputHandler;
using tdme::tools::shared::views::CameraRotationInputHandlerEventHandler;
using tdme::tools::shared::views::PopUps;
using tdme::tools::shared::views::PrototypePhysicsView;
using tdme::utilities::Console;
using tdme::utilities::Exception;

TriggerView::TriggerView(PopUps* popUps)
{
	this->popUps = popUps;
	engine = Engine::getInstance();
	cameraRotationInputHandler = new CameraRotationInputHandler(engine, this);
}

TriggerView::~TriggerView() {
	delete cameraRotationInputHandler;
	delete triggerScreenController;
	delete prototypePhysicsView;
}

PopUps* TriggerView::getPopUpsViews()
{
	return popUps;
}

Prototype* TriggerView::getPrototype()
{
	return prototype;
}

void TriggerView::setPrototype(Prototype* prototype)
{
	engine->reset();
	this->prototype = prototype;
	prototype->setDefaultBoundingVolumes();
	Tools::setupEntity(prototype, engine, cameraRotationInputHandler->getLookFromRotations(), cameraRotationInputHandler->getScale(), 1, objectScale);
	Tools::oseThumbnail(prototype);
	cameraRotationInputHandler->setMaxAxisDimension(Tools::computeMaxAxisDimension(engine->getEntity(Prototype::MODEL_BOUNDINGVOLUMES_ID)->getBoundingBox()));
	updateGUIElements();
}

void TriggerView::handleInputEvents()
{
	prototypePhysicsView->handleInputEvents(prototype, objectScale);
	cameraRotationInputHandler->handleInputEvents();
}

void TriggerView::display()
{
	// viewport
	auto xScale = (float)engine->getWidth() / (float)triggerScreenController->getScreenNode()->getScreenWidth();
	auto yScale = (float)engine->getHeight() / (float)triggerScreenController->getScreenNode()->getScreenHeight();
	auto viewPortLeft = 0;
	auto viewPortTop = 0;
	auto viewPortWidth = 0;
	auto viewPortHeight = 0;
	triggerScreenController->getViewPort(viewPortLeft, viewPortTop, viewPortWidth, viewPortHeight);
	viewPortLeft = (int)((float)viewPortLeft * xScale);
	viewPortTop = (int)((float)viewPortTop * yScale);
	viewPortWidth = (int)((float)viewPortWidth * xScale);
	viewPortHeight = (int)((float)viewPortHeight * yScale);
	engine->getCamera()->enableViewPort(viewPortLeft, viewPortTop, viewPortWidth, viewPortHeight);

	// rendering
	prototypePhysicsView->display(prototype);
	engine->getGUI()->handleEvents();
	engine->getGUI()->render();
}

void TriggerView::updateGUIElements()
{
	if (prototype != nullptr) {
		triggerScreenController->setScreenCaption("Trigger - " + prototype->getName());
		auto preset = prototype->getProperty("preset");
		triggerScreenController->setPrototypeProperties(preset != nullptr ? preset->getValue() : "", "");
		triggerScreenController->setPrototypeData(prototype->getName(), prototype->getDescription());
		prototypePhysicsView->setBoundingVolumes(prototype);
		prototypePhysicsView->setPhysics(prototype);
	} else {
		triggerScreenController->setScreenCaption("Trigger - no trigger loaded");
		triggerScreenController->unsetPrototypeProperties();
		triggerScreenController->unsetPrototypeData();
		prototypePhysicsView->unsetBoundingVolumes();
		prototypePhysicsView->unsetPhysics();
	}
}

void TriggerView::initialize()
{
	try {
		triggerScreenController = new TriggerScreenController(this);
		triggerScreenController->initialize();
		prototypePhysicsView = triggerScreenController->getPrototypePhysicsSubScreenController()->getView();
		prototypePhysicsView->initialize();
		prototypePhysicsView->setDisplayBoundingVolume(true);
		engine->getGUI()->addScreen(triggerScreenController->getScreenNode()->getId(), triggerScreenController->getScreenNode());
		triggerScreenController->getScreenNode()->setInputEventHandler(this);
	} catch (Exception& exception) {
		Console::print(string("TriggerView::initialize(): An error occurred: "));
		Console::println(exception.what());
	}
}

void TriggerView::activate()
{
	engine->reset();
	engine->setPartition(new PartitionNone());
	engine->setShadowMapLightEyeDistanceScale(0.1f);
	engine->getGUI()->resetRenderScreens();
	engine->getGUI()->addRenderScreen(triggerScreenController->getScreenNode()->getId());
	engine->getGUI()->addRenderScreen(TDMESceneEditor::getInstance()->getSceneEditorLibraryScreenController()->getScreenNode()->getId());
	engine->getGUI()->addRenderScreen(popUps->getFileDialogScreenController()->getScreenNode()->getId());
	engine->getGUI()->addRenderScreen(popUps->getInfoDialogScreenController()->getScreenNode()->getId());
}

void TriggerView::deactivate()
{
}

void TriggerView::dispose()
{
	Engine::getInstance()->reset();
}

void TriggerView::onRotation() {
	prototypePhysicsView->updateGizmo(prototype);
}

void TriggerView::onScale() {
	prototypePhysicsView->updateGizmo(prototype);
}
