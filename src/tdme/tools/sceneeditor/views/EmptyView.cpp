#include <tdme/tools/sceneeditor/views/EmptyView.h>

#include <string>

#include <tdme/engine/model/Model.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/prototype/Prototype.h>
#include <tdme/engine/prototype/PrototypeProperty.h>
#include <tdme/engine/Camera.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Entity.h>
#include <tdme/engine/PartitionNone.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/gui/GUI.h>
#include <tdme/math/Vector3.h>
#include <tdme/tools/sceneeditor/controller/EmptyScreenController.h>
#include <tdme/tools/sceneeditor/controller/SceneEditorLibraryScreenController.h>
#include <tdme/tools/sceneeditor/TDMESceneEditor.h>
#include <tdme/tools/shared/controller/FileDialogScreenController.h>
#include <tdme/tools/shared/controller/InfoDialogScreenController.h>
#include <tdme/tools/shared/tools/Tools.h>
#include <tdme/tools/shared/views/CameraRotationInputHandler.h>
#include <tdme/tools/shared/views/PopUps.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>

using std::string;

using tdme::engine::model::Model;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::prototype::Prototype;
using tdme::engine::prototype::PrototypeProperty;
using tdme::engine::Camera;
using tdme::engine::Engine;
using tdme::engine::Entity;
using tdme::engine::PartitionNone;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::GUI;
using tdme::math::Vector3;
using tdme::tools::sceneeditor::controller::EmptyScreenController;
using tdme::tools::sceneeditor::controller::SceneEditorLibraryScreenController;
using tdme::tools::sceneeditor::views::EmptyView;
using tdme::tools::sceneeditor::TDMESceneEditor;
using tdme::tools::shared::controller::FileDialogScreenController;
using tdme::tools::shared::controller::InfoDialogScreenController;
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
	prototype = nullptr;
	engine = Engine::getInstance();
	cameraRotationInputHandler = new CameraRotationInputHandler(engine);
}

EmptyView::~EmptyView() {
	delete emptyScreenController;
	delete cameraRotationInputHandler;
}

PopUps* EmptyView::getPopUps()
{
	return popUps;
}

Prototype* EmptyView::getPrototype()
{
	return prototype;
}

void EmptyView::setPrototype(Prototype* prototype)
{
	engine->reset();
	this->prototype = prototype;
	initModelRequested = true;
}

void EmptyView::initModel()
{
	if (prototype == nullptr)
		return;

	Tools::setupPrototype(prototype, engine, cameraRotationInputHandler->getLookFromRotations(), cameraRotationInputHandler->getScale(), 1, objectScale);
	Tools::oseThumbnail(prototype);
	cameraRotationInputHandler->setMaxAxisDimension(Tools::computeMaxAxisDimension(prototype->getModel()->getBoundingBox()));
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
}

void EmptyView::updateGUIElements()
{
	if (prototype != nullptr) {
		emptyScreenController->setScreenCaption("Empty - " + prototype->getName());
		auto preset = prototype->getProperty("preset");
		emptyScreenController->setPrototypeProperties(preset != nullptr ? preset->getValue() : "", "");
		emptyScreenController->setPrototypeData(prototype->getName(), prototype->getDescription());
	} else {
		emptyScreenController->setScreenCaption("Empty - no empty loaded");
		emptyScreenController->unsetPrototypeProperties();
		emptyScreenController->unsetPrototypeData();
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
	engine->setShadowMapLightEyeDistanceScale(0.1f);
	engine->getGUI()->resetRenderScreens();
	engine->getGUI()->addRenderScreen(emptyScreenController->getScreenNode()->getId());
	engine->getGUI()->addRenderScreen(TDMESceneEditor::getInstance()->getSceneEditorLibraryScreenController()->getScreenNode()->getId());
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
