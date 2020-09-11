#include <tdme/tools/leveleditor/views/EmptyView.h>

#include <string>

#include <tdme/engine/Engine.h>
#include <tdme/engine/Entity.h>
#include <tdme/engine/PartitionNone.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/gui/GUI.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/math/Vector3.h>
#include <tdme/tools/leveleditor/TDMELevelEditor.h>
#include <tdme/tools/leveleditor/controller/EmptyScreenController.h>
#include <tdme/tools/leveleditor/controller/LevelEditorEntityLibraryScreenController.h>
#include <tdme/tools/shared/controller/FileDialogScreenController.h>
#include <tdme/tools/shared/controller/InfoDialogScreenController.h>
#include <tdme/tools/shared/model/LevelEditorEntity.h>
#include <tdme/tools/shared/model/PropertyModelClass.h>
#include <tdme/tools/shared/tools/Tools.h>
#include <tdme/tools/shared/views/CameraRotationInputHandler.h>
#include <tdme/tools/shared/views/PopUps.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>

using std::string;

using tdme::tools::leveleditor::views::EmptyView;
using tdme::engine::Engine;
using tdme::engine::Entity;
using tdme::engine::PartitionNone;
using tdme::engine::model::Model;
using tdme::engine::primitives::BoundingBox;
using tdme::gui::GUI;
using tdme::gui::nodes::GUIScreenNode;
using tdme::math::Vector3;
using tdme::tools::leveleditor::TDMELevelEditor;
using tdme::tools::leveleditor::controller::EmptyScreenController;
using tdme::tools::leveleditor::controller::LevelEditorEntityLibraryScreenController;
using tdme::tools::shared::controller::FileDialogScreenController;
using tdme::tools::shared::controller::InfoDialogScreenController;
using tdme::tools::shared::model::LevelEditorEntity;
using tdme::tools::shared::model::PropertyModelClass;
using tdme::tools::shared::tools::Tools;
using tdme::tools::shared::views::CameraRotationInputHandler;
using tdme::tools::shared::views::PopUps;
using tdme::utilities::Console;
using tdme::utilities::Exception;

EmptyView::EmptyView(PopUps* popUps) 
{
	this->popUps = popUps;
	emptyScreenController = nullptr;
	initModelRequested = false;
	entity = nullptr;
	engine = Engine::getInstance();
	cameraRotationInputHandler = new CameraRotationInputHandler(engine);
}

EmptyView::~EmptyView() {
	delete emptyScreenController;
	delete cameraRotationInputHandler;
}

PopUps* EmptyView::getPopUpsViews()
{
	return popUps;
}

LevelEditorEntity* EmptyView::getEntity()
{
	return entity;
}

void EmptyView::setEntity(LevelEditorEntity* entity)
{
	engine->reset();
	this->entity = entity;
	initModelRequested = true;
}

void EmptyView::initModel()
{
	if (entity == nullptr)
		return;

	Tools::setupEntity(entity, engine, cameraRotationInputHandler->getLookFromRotations(), cameraRotationInputHandler->getScale(), 1, objectScale);
	Tools::oseThumbnail(entity);
	cameraRotationInputHandler->setMaxAxisDimension(Tools::computeMaxAxisDimension(entity->getModel()->getBoundingBox()));
	auto model = engine->getEntity("model");
	auto ground = engine->getEntity("ground");
	model->setContributesShadows(false);
	model->setReceivesShadows(false);
	ground->setEnabled(false);
	auto modelBoundingVolume = engine->getEntity("model_bv");
	if (modelBoundingVolume != nullptr) {
		modelBoundingVolume->setEnabled(false);
	}
	updateGUIElements();
}

void EmptyView::handleInputEvents()
{
	cameraRotationInputHandler->handleInputEvents();
}

void EmptyView::display()
{
	// commands
	if (initModelRequested == true) {
		initModel();
		cameraRotationInputHandler->reset();
		initModelRequested = false;
	}

	// viewport
	auto xScale = (float)engine->getWidth() / (float)emptyScreenController->getScreenNode()->getScreenWidth();
	auto yScale = (float)engine->getHeight() / (float)emptyScreenController->getScreenNode()->getScreenHeight();
	auto viewPortLeft = 0;
	auto viewPortTop = 0;
	auto viewPortWidth = 0;
	auto viewPortHeight = 0;
	emptyScreenController->getViewPort(viewPortLeft, viewPortTop, viewPortWidth, viewPortHeight);
	viewPortLeft = (int)((float)viewPortLeft * xScale);
	viewPortTop = (int)((float)viewPortTop * yScale);
	viewPortWidth = (int)((float)viewPortWidth * xScale);
	viewPortHeight = (int)((float)viewPortHeight * yScale);
	engine->getCamera()->enableViewPort(viewPortLeft, viewPortTop, viewPortWidth, viewPortHeight);

	// rendering
	engine->getGUI()->handleEvents();
	engine->getGUI()->render();
}

void EmptyView::updateGUIElements()
{
	if (entity != nullptr) {
		emptyScreenController->setScreenCaption("Empty - " + entity->getName());
		auto preset = entity->getProperty("preset");
		emptyScreenController->setEntityProperties(preset != nullptr ? preset->getValue() : "", "");
		emptyScreenController->setEntityData(entity->getName(), entity->getDescription());
		Vector3 dimension;
		dimension.set(entity->getModel()->getBoundingBox()->getMax());
		dimension.sub(entity->getModel()->getBoundingBox()->getMin());
	} else {
		emptyScreenController->setScreenCaption("Empty - no trigger loaded");
		emptyScreenController->unsetEntityProperties();
		emptyScreenController->unsetEntityData();
	}
}

void EmptyView::initialize()
{
	try {
		emptyScreenController = new EmptyScreenController(this);
		emptyScreenController->initialize();
		engine->getGUI()->addScreen(emptyScreenController->getScreenNode()->getId(), emptyScreenController->getScreenNode());
		emptyScreenController->getScreenNode()->setInputEventHandler(this);
	} catch (Exception& exception) {
		Console::print(string("EmptyView::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	updateGUIElements();
}

void EmptyView::activate()
{
	engine->reset();
	engine->setPartition(new PartitionNone());
	engine->getGUI()->resetRenderScreens();
	engine->getGUI()->addRenderScreen(emptyScreenController->getScreenNode()->getId());
	engine->getGUI()->addRenderScreen(TDMELevelEditor::getInstance()->getLevelEditorEntityLibraryScreenController()->getScreenNode()->getId());
	engine->getGUI()->addRenderScreen(popUps->getFileDialogScreenController()->getScreenNode()->getId());
	engine->getGUI()->addRenderScreen(popUps->getInfoDialogScreenController()->getScreenNode()->getId());
}

void EmptyView::deactivate()
{
}

void EmptyView::dispose()
{
	Engine::getInstance()->reset();
}
