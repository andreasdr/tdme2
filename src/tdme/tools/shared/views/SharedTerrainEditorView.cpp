#include <tdme/tools/shared/views/SharedTerrainEditorView.h>

#include <string>

#include <tdme/engine/model/Model.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/prototype/Prototype.h>
#include <tdme/engine/prototype/PrototypeProperty.h>
#include <tdme/engine/Camera.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Entity.h>
#include <tdme/engine/Object3D.h>
#include <tdme/engine/PartitionNone.h>
#include <tdme/engine/Timing.h>
#include <tdme/gui/events/GUIKeyboardEvent.h>
#include <tdme/gui/events/GUIMouseEvent.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/gui/GUI.h>
#include <tdme/math/Vector3.h>
#include <tdme/tools/sceneeditor/TDMESceneEditor.h>
#include <tdme/tools/shared/controller/FileDialogScreenController.h>
#include <tdme/tools/shared/controller/InfoDialogScreenController.h>
#include <tdme/tools/shared/controller/TerrainEditorScreenController.h>
#include <tdme/tools/shared/tools/Tools.h>
#include <tdme/tools/shared/views/CameraInputHandler.h>
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
using tdme::engine::Object3D;
using tdme::engine::PartitionNone;
using tdme::engine::Timing;
using tdme::gui::events::GUIKeyboardEvent;
using tdme::gui::events::GUIMouseEvent;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::GUI;
using tdme::math::Vector3;
using tdme::tools::shared::controller::FileDialogScreenController;
using tdme::tools::shared::controller::InfoDialogScreenController;
using tdme::tools::shared::controller::TerrainEditorScreenController;
using tdme::tools::shared::tools::Tools;
using tdme::tools::shared::views::CameraInputHandler;
using tdme::tools::shared::views::PopUps;
using tdme::tools::shared::views::SharedTerrainEditorView;
using tdme::utilities::Console;
using tdme::utilities::Exception;

SharedTerrainEditorView::SharedTerrainEditorView(PopUps* popUps)
{
	this->popUps = popUps;
	terrainEditorScreenController = nullptr;
	initModelRequested = false;
	prototype = nullptr;
	engine = Engine::getInstance();
	cameraInputHandler = new CameraInputHandler(engine);
}

SharedTerrainEditorView::~SharedTerrainEditorView() {
	delete terrainEditorScreenController;
	delete cameraInputHandler;
}

PopUps* SharedTerrainEditorView::getPopUpsViews()
{
	return popUps;
}

Prototype* SharedTerrainEditorView::getPrototype()
{
	return prototype;
}

void SharedTerrainEditorView::setPrototype(Prototype* prototype)
{
	engine->reset();
	this->prototype = prototype;
	initModelRequested = true;
}

void SharedTerrainEditorView::initModel()
{
	if (prototype == nullptr)
		return;

	//
	if (prototype->getModel() != nullptr) {
		engine->removeEntity("terrain");
		auto terrainObject3D = new Object3D("terrain", prototype->getModel());
		terrainObject3D->setShader("terrain");
		terrainObject3D->setContributesShadows(true);
		terrainObject3D->setReceivesShadows(true);
		engine->addEntity(terrainObject3D);
		Vector3 sceneCenter = prototype->getModel()->getBoundingBox()->getCenter();
		sceneCenter.set(Vector3(sceneCenter.getX(), prototype->getModel()->getBoundingBox()->getMax().getY() + 3.0f, sceneCenter.getZ()));
		cameraInputHandler->setSceneCenter(sceneCenter);
		Tools::oseThumbnail(prototype);
		auto modelBoundingVolume = engine->getEntity("model_bv");
		if (modelBoundingVolume != nullptr) {
			modelBoundingVolume->setEnabled(false);
		}
	}

	//
	updateGUIElements();
}

void SharedTerrainEditorView::handleInputEvents()
{
	for (auto i = 0; i < engine->getGUI()->getMouseEvents().size(); i++) {
		auto& event = engine->getGUI()->getMouseEvents()[i];
		if (event.isProcessed() == true) continue;

		if (event.getButton() == MOUSE_BUTTON_LEFT) {
			if (event.getType() == GUIMouseEvent::MOUSEEVENT_PRESSED ||
				event.getType() == GUIMouseEvent::MOUSEEVENT_DRAGGED) {
				brushingEnabled = true;
				engine->computeWorldCoordinateByMousePosition(event.getXUnscaled(), event.getYUnscaled(), brushCenterPosition);
				event.setProcessed(true);
			} else
			if (event.getType() == GUIMouseEvent::MOUSEEVENT_RELEASED) {
				brushingEnabled = false;
				terrainEditorScreenController->unsetCurrentBrushFlattenHeight();
				event.setProcessed(true);
			}
		}
	}
	cameraInputHandler->handleInputEvents();
}

void SharedTerrainEditorView::display()
{
	// commands
	if (initModelRequested == true) {
		initModel();
		cameraInputHandler->reset();
		initModelRequested = false;
	}

	// actually do the brushing
	if (brushingEnabled == true && terrainEditorScreenController->determineCurrentBrushFlattenHeight(brushCenterPosition) == true) {
		terrainEditorScreenController->applyBrush(brushCenterPosition, engine->getTiming()->getDeltaTime());
	}

	// viewport
	auto xScale = (float)engine->getWidth() / (float)terrainEditorScreenController->getScreenNode()->getScreenWidth();
	auto yScale = (float)engine->getHeight() / (float)terrainEditorScreenController->getScreenNode()->getScreenHeight();
	auto viewPortLeft = 0;
	auto viewPortTop = 0;
	auto viewPortWidth = 0;
	auto viewPortHeight = 0;
	terrainEditorScreenController->getViewPort(viewPortLeft, viewPortTop, viewPortWidth, viewPortHeight);
	viewPortLeft = (int)((float)viewPortLeft * xScale);
	viewPortTop = (int)((float)viewPortTop * yScale);
	viewPortWidth = (int)((float)viewPortWidth * xScale);
	viewPortHeight = (int)((float)viewPortHeight * yScale);
	engine->getCamera()->enableViewPort(viewPortLeft, viewPortTop, viewPortWidth, viewPortHeight);

	// rendering
	engine->getGUI()->handleEvents();
	engine->getGUI()->render();
}

void SharedTerrainEditorView::updateGUIElements()
{
	if (prototype != nullptr) {
		terrainEditorScreenController->setScreenCaption("Terrain Editor - " + prototype->getName());
		auto preset = prototype->getProperty("preset");
		terrainEditorScreenController->setPrototypeProperties(preset != nullptr ? preset->getValue() : "", "");
		terrainEditorScreenController->setPrototypeData(prototype->getName(), prototype->getDescription());
		if (prototype->getModel() != nullptr) {
			auto boundingBox = prototype->getModel()->getBoundingBox();
			terrainEditorScreenController->setTerrainDimension(
				boundingBox->getMax().getX() - boundingBox->getMin().getX(),
				boundingBox->getMax().getZ() - boundingBox->getMin().getZ()
			);
		} else {
			terrainEditorScreenController->setTerrainDimension(64.0f, 64.0f);
		}
	} else {
		terrainEditorScreenController->setScreenCaption("Terrain Editor - no terrain loaded");
		terrainEditorScreenController->unsetPrototypeProperties();
		terrainEditorScreenController->unsetPrototypeData();
		terrainEditorScreenController->setTerrainDimension(64.0f, 64.0f);
	}
}

void SharedTerrainEditorView::initialize()
{
	try {
		terrainEditorScreenController = new TerrainEditorScreenController(this);
		terrainEditorScreenController->initialize();
		engine->getGUI()->addScreen(terrainEditorScreenController->getScreenNode()->getId(), terrainEditorScreenController->getScreenNode());
		terrainEditorScreenController->getScreenNode()->setInputEventHandler(this);
	} catch (Exception& exception) {
		Console::print(string("SharedTerrainEditorView::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	updateGUIElements();
}

void SharedTerrainEditorView::activate()
{
	engine->reset();
	engine->setPartition(new PartitionNone());
	engine->setShadowMapLightEyeDistanceScale(0.1f);
	engine->getGUI()->resetRenderScreens();
	engine->getGUI()->addRenderScreen(terrainEditorScreenController->getScreenNode()->getId());
	onInitAdditionalScreens();
	engine->getGUI()->addRenderScreen(popUps->getFileDialogScreenController()->getScreenNode()->getId());
	engine->getGUI()->addRenderScreen(popUps->getInfoDialogScreenController()->getScreenNode()->getId());

	// lights
	for (auto i = 1; i < engine->getLightCount(); i++) engine->getLightAt(i)->setEnabled(false);
	{
		auto light0 = engine->getLightAt(0);
		light0->setAmbient(Color4(0.7f, 0.7f, 0.7f, 1.0f));
		light0->setDiffuse(Color4(0.3f, 0.3f, 0.3f, 1.0f));
		light0->setSpecular(Color4(1.0f, 1.0f, 1.0f, 1.0f));
		light0->setPosition(Vector4(0.0f, 20000.0f, 0.0f, 1.0f));
		light0->setSpotDirection(Vector3(0.0f, 0.0f, 0.0f).sub(Vector3(light0->getPosition().getX(), light0->getPosition().getY(), light0->getPosition().getZ())).normalize());
		light0->setConstantAttenuation(0.5f);
		light0->setLinearAttenuation(0.0f);
		light0->setQuadraticAttenuation(0.0f);
		light0->setSpotExponent(0.0f);
		light0->setSpotCutOff(180.0f);
		light0->setEnabled(true);
	}
}

void SharedTerrainEditorView::deactivate()
{
}

void SharedTerrainEditorView::dispose()
{
	Engine::getInstance()->reset();
}

void SharedTerrainEditorView::onSetPrototypeData() {
}

void SharedTerrainEditorView::onInitAdditionalScreens() {
}
