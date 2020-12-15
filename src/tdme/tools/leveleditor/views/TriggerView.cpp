#include <tdme/tools/leveleditor/views/TriggerView.h>

#include <tdme/engine/Camera.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Entity.h>
#include <tdme/engine/PartitionNone.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/gui/GUI.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/math/Vector3.h>
#include <tdme/tools/leveleditor/TDMELevelEditor.h>
#include <tdme/tools/leveleditor/controller/LevelEditorEntityLibraryScreenController.h>
#include <tdme/tools/leveleditor/controller/TriggerScreenController.h>
#include <tdme/tools/shared/controller/EntityPhysicsSubScreenController.h>
#include <tdme/tools/shared/controller/FileDialogScreenController.h>
#include <tdme/tools/shared/controller/InfoDialogScreenController.h>
#include <tdme/tools/shared/model/LevelEditorEntity.h>
#include <tdme/tools/shared/model/LevelEditorEntityLibrary.h>
#include <tdme/tools/shared/model/LevelEditorLevel.h>
#include <tdme/tools/shared/model/PropertyModelClass.h>
#include <tdme/tools/shared/tools/Tools.h>
#include <tdme/tools/shared/views/CameraRotationInputHandler.h>
#include <tdme/tools/shared/views/CameraRotationInputHandlerEventHandler.h>
#include <tdme/tools/shared/views/EntityPhysicsView.h>
#include <tdme/tools/shared/views/PopUps.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>

using tdme::tools::leveleditor::views::TriggerView;
using tdme::engine::Camera;
using tdme::engine::Engine;
using tdme::engine::Entity;
using tdme::engine::PartitionNone;
using tdme::engine::model::Model;
using tdme::engine::primitives::BoundingBox;
using tdme::gui::GUI;
using tdme::gui::nodes::GUIScreenNode;
using tdme::math::Vector3;
using tdme::tools::leveleditor::TDMELevelEditor;
using tdme::tools::leveleditor::controller::LevelEditorEntityLibraryScreenController;
using tdme::tools::leveleditor::controller::TriggerScreenController;
using tdme::tools::shared::controller::EntityPhysicsSubScreenController;
using tdme::tools::shared::controller::FileDialogScreenController;
using tdme::tools::shared::controller::InfoDialogScreenController;
using tdme::tools::shared::model::LevelEditorEntity;
using tdme::tools::shared::model::LevelEditorEntityLibrary;
using tdme::tools::shared::model::LevelEditorLevel;
using tdme::tools::shared::model::PropertyModelClass;
using tdme::tools::shared::tools::Tools;
using tdme::tools::shared::views::CameraRotationInputHandler;
using tdme::tools::shared::views::CameraRotationInputHandlerEventHandler;
using tdme::tools::shared::views::EntityPhysicsView;
using tdme::tools::shared::views::PopUps;
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
	delete entityPhysicsView;
}

PopUps* TriggerView::getPopUpsViews()
{
	return popUps;
}

LevelEditorEntity* TriggerView::getEntity()
{
	return entity;
}

void TriggerView::setEntity(LevelEditorEntity* entity)
{
	engine->reset();
	this->entity = entity;
	entity->setDefaultBoundingVolumes();
	Tools::setupEntity(entity, engine, cameraRotationInputHandler->getLookFromRotations(), cameraRotationInputHandler->getScale(), 1, objectScale);
	Tools::oseThumbnail(entity);
	cameraRotationInputHandler->setMaxAxisDimension(Tools::computeMaxAxisDimension(engine->getEntity(LevelEditorEntity::MODEL_BOUNDINGVOLUMES_ID)->getBoundingBox()));
	updateGUIElements();
}

void TriggerView::handleInputEvents()
{
	entityPhysicsView->handleInputEvents(entity, objectScale);
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
	entityPhysicsView->display(entity);
	engine->getGUI()->handleEvents();
	engine->getGUI()->render();
}

void TriggerView::updateGUIElements()
{
	if (entity != nullptr) {
		triggerScreenController->setScreenCaption("Trigger - " + entity->getName());
		auto preset = entity->getProperty("preset");
		triggerScreenController->setEntityProperties(preset != nullptr ? preset->getValue() : "", "");
		triggerScreenController->setEntityData(entity->getName(), entity->getDescription());
		entityPhysicsView->setBoundingVolumes(entity);
		entityPhysicsView->setPhysics(entity);
	} else {
		triggerScreenController->setScreenCaption("Trigger - no trigger loaded");
		triggerScreenController->unsetEntityProperties();
		triggerScreenController->unsetEntityData();
		entityPhysicsView->unsetBoundingVolumes();
		entityPhysicsView->unsetPhysics();
	}
}

void TriggerView::initialize()
{
	try {
		triggerScreenController = new TriggerScreenController(this);
		triggerScreenController->initialize();
		entityPhysicsView = triggerScreenController->getEntityPhysicsSubScreenController()->getView();
		entityPhysicsView->initialize();
		entityPhysicsView->setDisplayBoundingVolume(true);
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
	engine->getGUI()->addRenderScreen(TDMELevelEditor::getInstance()->getLevelEditorEntityLibraryScreenController()->getScreenNode()->getId());
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
	entityPhysicsView->updateGizmo(entity);
}

void TriggerView::onScale() {
	entityPhysicsView->updateGizmo(entity);
}
