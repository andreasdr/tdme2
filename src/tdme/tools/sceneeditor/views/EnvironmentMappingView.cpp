#include <tdme/tools/sceneeditor/views/EnvironmentMappingView.h>

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
#include <tdme/tools/sceneeditor/controller/EnvironmentMappingScreenController.h>
#include <tdme/tools/sceneeditor/controller/SceneEditorLibraryScreenController.h>
#include <tdme/tools/sceneeditor/TDMESceneEditor.h>
#include <tdme/tools/shared/controller/FileDialogScreenController.h>
#include <tdme/tools/shared/controller/InfoDialogScreenController.h>
#include <tdme/tools/shared/controller/PrototypePhysicsSubScreenController.h>
#include <tdme/tools/shared/tools/Tools.h>
#include <tdme/tools/shared/views/CameraRotationInputHandler.h>
#include <tdme/tools/shared/views/CameraRotationInputHandlerEventHandler.h>
#include <tdme/tools/shared/views/Gizmo.h>
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
using tdme::tools::sceneeditor::controller::EnvironmentMappingScreenController;
using tdme::tools::sceneeditor::controller::SceneEditorLibraryScreenController;
using tdme::tools::sceneeditor::views::EnvironmentMappingView;
using tdme::tools::sceneeditor::TDMESceneEditor;
using tdme::tools::shared::controller::FileDialogScreenController;
using tdme::tools::shared::controller::InfoDialogScreenController;
using tdme::tools::shared::controller::PrototypePhysicsSubScreenController;
using tdme::tools::shared::tools::Tools;
using tdme::tools::shared::views::CameraRotationInputHandler;
using tdme::tools::shared::views::CameraRotationInputHandlerEventHandler;
using tdme::tools::shared::views::Gizmo;
using tdme::tools::shared::views::PopUps;
using tdme::tools::shared::views::PrototypePhysicsView;
using tdme::utilities::Console;
using tdme::utilities::Exception;

EnvironmentMappingView::EnvironmentMappingView(PopUps* popUps)
{
	this->popUps = popUps;
	environmentMappingScreenController = nullptr;
	prototype = nullptr;
	engine = Engine::getInstance();
	cameraRotationInputHandler = new CameraRotationInputHandler(engine, this);
}

EnvironmentMappingView::~EnvironmentMappingView() {
	delete cameraRotationInputHandler;
	delete environmentMappingScreenController;
	delete prototypePhysicsView;
}

PopUps* EnvironmentMappingView::getPopUps()
{
	return popUps;
}

Prototype* EnvironmentMappingView::getPrototype()
{
	return prototype;
}

void EnvironmentMappingView::setPrototype(Prototype* prototype)
{
	engine->reset();
	this->prototype = prototype;
	prototype->setDefaultBoundingVolumes(1);
	Tools::setupPrototype(prototype, engine, cameraRotationInputHandler->getLookFromRotations(), cameraRotationInputHandler->getScale(), 1, objectScale);
	Tools::oseThumbnail(prototype);
	cameraRotationInputHandler->setMaxAxisDimension(Tools::computeMaxAxisDimension(engine->getEntity(Prototype::MODEL_BOUNDINGVOLUMES_ID)->getBoundingBox()));
	updateGUIElements();
}

void EnvironmentMappingView::handleInputEvents()
{
	prototypePhysicsView->handleInputEvents(prototype, objectScale);
	cameraRotationInputHandler->handleInputEvents();
}

void EnvironmentMappingView::display()
{
	// viewport
	auto xScale = (float)engine->getWidth() / (float)environmentMappingScreenController->getScreenNode()->getScreenWidth();
	auto yScale = (float)engine->getHeight() / (float)environmentMappingScreenController->getScreenNode()->getScreenHeight();
	auto viewPortLeft = 0;
	auto viewPortTop = 0;
	auto viewPortWidth = 0;
	auto viewPortHeight = 0;
	environmentMappingScreenController->getViewPort(viewPortLeft, viewPortTop, viewPortWidth, viewPortHeight);
	viewPortLeft = (int)((float)viewPortLeft * xScale);
	viewPortTop = (int)((float)viewPortTop * yScale);
	viewPortWidth = (int)((float)viewPortWidth * xScale);
	viewPortHeight = (int)((float)viewPortHeight * yScale);
	engine->getCamera()->enableViewPort(viewPortLeft, viewPortTop, viewPortWidth, viewPortHeight);

	// rendering
	prototypePhysicsView->display(prototype);
}

void EnvironmentMappingView::updateGUIElements()
{
	if (prototype != nullptr) {
		environmentMappingScreenController->setScreenCaption("Environment Mapping - " + prototype->getName());
		auto preset = prototype->getProperty("preset");
		environmentMappingScreenController->setPrototypeProperties(preset != nullptr ? preset->getValue() : "", "");
		environmentMappingScreenController->setPrototypeData(prototype->getName(), prototype->getDescription());
		environmentMappingScreenController->setGeneration();
		prototypePhysicsView->setBoundingVolumes(prototype);
	} else {
		environmentMappingScreenController->setScreenCaption("Environment Mapping - no environment mapping loaded");
		environmentMappingScreenController->unsetPrototypeProperties();
		environmentMappingScreenController->unsetPrototypeData();
		environmentMappingScreenController->unsetGeneration();
		prototypePhysicsView->unsetBoundingVolumes();
	}
}

void EnvironmentMappingView::initialize()
{
	try {
		environmentMappingScreenController = new EnvironmentMappingScreenController(this);
		environmentMappingScreenController->initialize();
		prototypePhysicsView = environmentMappingScreenController->getPrototypePhysicsSubScreenController()->getView();
		prototypePhysicsView->initialize();
		prototypePhysicsView->setDisplayBoundingVolume(true);
		prototypePhysicsView->setDisplayBoundingVolumeIdx(0);
		prototypePhysicsView->setGizmoTypeMask(Gizmo::GIZMOTYPE_SCALE);
		engine->getGUI()->addScreen(environmentMappingScreenController->getScreenNode()->getId(), environmentMappingScreenController->getScreenNode());
		environmentMappingScreenController->getScreenNode()->setInputEventHandler(this);
	} catch (Exception& exception) {
		Console::print(string("EnvironmentMappingView::initialize(): An error occurred: "));
		Console::println(exception.what());
	}
}

void EnvironmentMappingView::activate()
{
	engine->reset();
	engine->setPartition(new PartitionNone());
	engine->setShadowMapLightEyeDistanceScale(0.1f);
	engine->getGUI()->resetRenderScreens();
	engine->getGUI()->addRenderScreen(environmentMappingScreenController->getScreenNode()->getId());
	engine->getGUI()->addRenderScreen(TDMESceneEditor::getInstance()->getSceneEditorLibraryScreenController()->getScreenNode()->getId());
	engine->getGUI()->addRenderScreen(popUps->getFileDialogScreenController()->getScreenNode()->getId());
	engine->getGUI()->addRenderScreen(popUps->getInfoDialogScreenController()->getScreenNode()->getId());
}

void EnvironmentMappingView::deactivate()
{
}

void EnvironmentMappingView::dispose()
{
	Engine::getInstance()->reset();
}

void EnvironmentMappingView::onCameraRotation() {
	prototypePhysicsView->updateGizmo(prototype);
}

void EnvironmentMappingView::onCameraScale() {
	prototypePhysicsView->updateGizmo(prototype);
}
