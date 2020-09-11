#include <tdme/tools/shared/views/SharedModelEditorView.h>

#include <string>

#include <tdme/audio/Audio.h>
#include <tdme/audio/Sound.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/ModelUtilities.h>
#include <tdme/engine/PartitionNone.h>
#include <tdme/engine/Object3D.h>
#include <tdme/engine/fileio/models/ModelReader.h>
#include <tdme/engine/model/AnimationSetup.h>
#include <tdme/engine/model/Model.h>
#include <tdme/utilities/ModelTools.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/gui/GUI.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/math/Vector3.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/tools/shared/controller/EntityDisplaySubScreenController.h>
#include <tdme/tools/shared/controller/EntityPhysicsSubScreenController.h>
#include <tdme/tools/shared/controller/EntitySoundsSubScreenController.h>
#include <tdme/tools/shared/controller/FileDialogPath.h>
#include <tdme/tools/shared/controller/FileDialogScreenController.h>
#include <tdme/tools/shared/controller/InfoDialogScreenController.h>
#include <tdme/tools/shared/controller/ModelEditorScreenController.h>
#include <tdme/tools/shared/controller/ProgressBarScreenController.h>
#include <tdme/tools/shared/files/ModelMetaDataFileExport.h>
#include <tdme/tools/shared/files/ModelMetaDataFileImport.h>
#include <tdme/tools/shared/files/ProgressCallback.h>
#include <tdme/tools/shared/model/LevelEditorEntity_EntityType.h>
#include <tdme/tools/shared/model/LevelEditorEntity.h>
#include <tdme/tools/shared/model/LevelEditorEntityAudio.h>
#include <tdme/tools/shared/model/LevelEditorEntityModel.h>
#include <tdme/tools/shared/model/PropertyModelClass.h>
#include <tdme/tools/shared/tools/Tools.h>
#include <tdme/tools/shared/views/CameraRotationInputHandler.h>
#include <tdme/tools/shared/views/EntityPhysicsView.h>
#include <tdme/tools/shared/views/EntityDisplayView.h>
#include <tdme/tools/shared/views/EntitySoundsView.h>
#include <tdme/tools/shared/views/PlayableSoundView.h>
#include <tdme/tools/shared/views/PopUps.h>
#include <tdme/utilities/StringTools.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>
#include <tdme/utilities/Properties.h>
#include <tdme/engine/subsystems/rendering/ModelStatistics.h>

using std::string;

using tdme::audio::Audio;
using tdme::audio::Sound;
using tdme::tools::shared::views::SharedModelEditorView;
using tdme::engine::Engine;
using tdme::engine::ModelUtilities;
using tdme::engine::Object3D;
using tdme::engine::PartitionNone;
using tdme::engine::fileio::models::ModelReader;
using tdme::engine::model::Model;
using tdme::utilities::ModelTools;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::subsystems::rendering::ModelStatistics;
using tdme::gui::GUI;
using tdme::gui::nodes::GUIScreenNode;
using tdme::math::Vector3;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;
using tdme::tools::shared::controller::EntityPhysicsSubScreenController;
using tdme::tools::shared::controller::EntityDisplaySubScreenController;
using tdme::tools::shared::controller::FileDialogPath;
using tdme::tools::shared::controller::FileDialogScreenController;
using tdme::tools::shared::controller::InfoDialogScreenController;
using tdme::tools::shared::controller::ModelEditorScreenController;
using tdme::tools::shared::controller::ProgressBarScreenController;
using tdme::tools::shared::files::ModelMetaDataFileExport;
using tdme::tools::shared::files::ModelMetaDataFileImport;
using tdme::tools::shared::files::ProgressCallback;
using tdme::tools::shared::model::LevelEditorEntity_EntityType;
using tdme::tools::shared::model::LevelEditorEntity;
using tdme::tools::shared::model::LevelEditorEntityAudio;
using tdme::tools::shared::model::LevelEditorEntityModel;
using tdme::tools::shared::model::PropertyModelClass;
using tdme::tools::shared::tools::Tools;
using tdme::tools::shared::views::CameraRotationInputHandler;
using tdme::tools::shared::views::EntityPhysicsView;
using tdme::tools::shared::views::EntityDisplayView;
using tdme::tools::shared::views::EntitySoundsView;
using tdme::tools::shared::views::PlayableSoundView;
using tdme::tools::shared::views::PopUps;
using tdme::utilities::Properties;
using tdme::utilities::StringTools;
using tdme::utilities::Console;
using tdme::utilities::Exception;

SharedModelEditorView::SharedModelEditorView(PopUps* popUps) 
{
	this->popUps = popUps;
	engine = Engine::getInstance();
	audio = Audio::getInstance();
	modelEditorScreenController = nullptr;
	entityDisplayView = nullptr;
	entityPhysicsView = nullptr;
	entitySoundsView = nullptr;
	loadModelRequested = false;
	initModelRequested = false;
	initModelRequestedReset = false;
	entity = nullptr;
	modelFile = "";
	lodLevel = 1;
	audioStarted = -1LL;
	audioOffset = -1LL;
	cameraRotationInputHandler = new CameraRotationInputHandler(engine, this);
}

SharedModelEditorView::~SharedModelEditorView() {
	delete modelEditorScreenController;
	delete cameraRotationInputHandler;
}

PopUps* SharedModelEditorView::getPopUpsViews()
{
	return popUps;
}

LevelEditorEntity* SharedModelEditorView::getEntity()
{
	return entity;
}

void SharedModelEditorView::setEntity(LevelEditorEntity* entity)
{
	engine->reset();
	this->entity = entity;
	lodLevel = 1;
	initModelRequested = true;
	initModelRequestedReset = false;
}

void SharedModelEditorView::resetEntity()
{
	engine->reset();
	initModelRequested = true;
	initModelRequestedReset = true;
}

void SharedModelEditorView::reimportEntity()
{
	engine->reset();
	initModelRequested = true;
	initModelRequestedReset = false;
}

void SharedModelEditorView::initModel()
{
	if (entity == nullptr) return;

	modelFile = entity->getEntityFileName().length() > 0 ? entity->getEntityFileName() : entity->getFileName();
	Tools::setupEntity(entity, engine, cameraRotationInputHandler->getLookFromRotations(), cameraRotationInputHandler->getScale(), lodLevel, objectScale);
	Tools::oseThumbnail(entity);
	cameraRotationInputHandler->setMaxAxisDimension(Tools::computeMaxAxisDimension(entity->getModel()->getBoundingBox()));
	auto currentModelObject = dynamic_cast<Object3D*>(engine->getEntity("model"));
	if (currentModelObject != nullptr) {
		ModelStatistics modelStatistics;
		ModelUtilities::computeModelStatistics(currentModelObject->getModel(), &modelStatistics);
		modelEditorScreenController->setStatistics(modelStatistics.opaqueFaceCount, modelStatistics.transparentFaceCount, modelStatistics.materialCount);
	} else {
		modelEditorScreenController->unsetStatistics();
	}
	if (initModelRequestedReset == false) updateGUIElements();
}

const string& SharedModelEditorView::getFileName()
{
	return modelFile;
}

int SharedModelEditorView::getLodLevel() const {
	return lodLevel;
}

void SharedModelEditorView::setLodLevel(int lodLevel) {
	if (this->lodLevel != lodLevel) {
		this->lodLevel = lodLevel;
		engine->reset();
		initModelRequested = true;
		modelEditorScreenController->setMaterials(entity);
		modelEditorScreenController->setAnimations(entity);
	}
}

void SharedModelEditorView::loadFile(const string& pathName, const string& fileName)
{
	loadModelRequested = true;
	modelFile = FileSystem::getInstance()->getFileName(pathName, fileName);
}

void SharedModelEditorView::reimportModel(const string& pathName, const string& fileName)
{
	if (entity == nullptr) return;
	engine->removeEntity("model");
	struct AnimationSetupStruct {
		bool loop;
		string overlayFromGroupId;
		float speed;
	};
	// store old animation setups
	map<string, AnimationSetupStruct> originalAnimationSetups;
	for (auto animationSetupIt: entity->getModel()->getAnimationSetups()) {
		auto animationSetup = animationSetupIt.second;
		originalAnimationSetups[animationSetup->getId()] = {
			.loop = animationSetup->isLoop(),
			.overlayFromGroupId = animationSetup->getOverlayFromGroupId(),
			.speed = animationSetup->getSpeed()
		};
	}
	// new model file
	modelFile = FileSystem::getInstance()->getFileName(pathName, fileName);
	string log;
	try {
		// load model
		auto model = ModelReader::read(
			pathName,
			fileName
		);
		// restore animation setup properties
		for (auto originalAnimationSetupIt: originalAnimationSetups) {
			auto originalAnimationSetupId = originalAnimationSetupIt.first;
			auto originalAnimationSetup = originalAnimationSetupIt.second;
			auto animationSetup = model->getAnimationSetup(originalAnimationSetupId);
			if (animationSetup == nullptr) {
				Console::println("SharedModelEditorView::reimportModel(): missing animation setup: " + originalAnimationSetupId);
				log+= "Missing animation setup: " + originalAnimationSetupId + ", skipping.\n";
				continue;
			}
			Console::println("SharedModelEditorView::reimportModel(): reimport animation setup: " + originalAnimationSetupId);
			animationSetup->setLoop(originalAnimationSetup.loop);
			animationSetup->setOverlayFromGroupId(originalAnimationSetup.overlayFromGroupId);
			animationSetup->setSpeed(originalAnimationSetup.speed);
		}
		// set model in entity
		entity->setModel(model);
	} catch (Exception& exception) {
		modelEditorScreenController->showErrorPopUp("Warning", (string(exception.what())));
	}
	reimportEntity();
	if (log.size() > 0) {
		modelEditorScreenController->showErrorPopUp("Warning", log);
	}
}

void SharedModelEditorView::saveFile(const string& pathName, const string& fileName)
{
	ModelMetaDataFileExport::doExport(pathName, fileName, entity);
}

void SharedModelEditorView::reloadFile()
{
	loadModelRequested = true;
}

void SharedModelEditorView::pivotApply(float x, float y, float z)
{
	if (entity == nullptr) return;
	entity->getPivot().set(x, y, z);
}

void SharedModelEditorView::computeNormals() {
	if (entity == nullptr) return;
	engine->removeEntity("model");
	class ComputeNormalsProgressCallback: public ProgressCallback {
	private:
		ProgressBarScreenController* progressBarScreenController;
	public:
		ComputeNormalsProgressCallback(ProgressBarScreenController* progressBarScreenController): progressBarScreenController(progressBarScreenController) {
		}
		virtual void progress(float value) {
			progressBarScreenController->progress(value);
		}
	};
	popUps->getProgressBarScreenController()->show();
	ModelTools::computeNormals(entity->getModel(), new ComputeNormalsProgressCallback(popUps->getProgressBarScreenController()));
	popUps->getProgressBarScreenController()->close();
	resetEntity();
}

void SharedModelEditorView::handleInputEvents()
{
	entityPhysicsView->handleInputEvents(entity, objectScale);
	cameraRotationInputHandler->handleInputEvents();
}

void SharedModelEditorView::display()
{
	// audio
	if (audioOffset > 0 && Time::getCurrentMillis() - audioStarted >= audioOffset) {
		auto sound = audio->getEntity("sound");
		if (sound != nullptr) sound->play();
		audioOffset = -1LL;
	}
	// commands
	if (loadModelRequested == true) {
		initModelRequested = true;
		initModelRequestedReset = false;
		loadModelRequested = false;
		loadModel();
		cameraRotationInputHandler->reset();
	}
	if (initModelRequested == true) {
		initModel();
		initModelRequested = false;
		initModelRequestedReset = false;
		cameraRotationInputHandler->reset();
	}

	// viewport
	auto xScale = (float)engine->getWidth() / (float)modelEditorScreenController->getScreenNode()->getScreenWidth();
	auto yScale = (float)engine->getHeight() / (float)modelEditorScreenController->getScreenNode()->getScreenHeight();
	auto viewPortLeft = 0;
	auto viewPortTop = 0;
	auto viewPortWidth = 0;
	auto viewPortHeight = 0;
	modelEditorScreenController->getViewPort(viewPortLeft, viewPortTop, viewPortWidth, viewPortHeight);
	viewPortLeft = (int)((float)viewPortLeft * xScale);
	viewPortTop = (int)((float)viewPortTop * yScale);
	viewPortWidth = (int)((float)viewPortWidth * xScale);
	viewPortHeight = (int)((float)viewPortHeight * yScale);
	engine->getCamera()->enableViewPort(viewPortLeft, viewPortTop, viewPortWidth, viewPortHeight);

	// rendering
	entityDisplayView->display(entity);
	entityPhysicsView->display(entity);
	engine->getGUI()->handleEvents();
	engine->getGUI()->render();
	audio->update();
}

void SharedModelEditorView::updateGUIElements()
{
	if (entity != nullptr) {
		modelEditorScreenController->setScreenCaption("Model Editor - " + (entity->getEntityFileName().length() > 0 ? Tools::getFileName(entity->getEntityFileName()) : Tools::getFileName(entity->getFileName())));
		auto preset = entity->getProperty("preset");
		modelEditorScreenController->setEntityProperties(preset != nullptr ? preset->getValue() : "", entity, "");
		modelEditorScreenController->setEntityData(entity->getName(), entity->getDescription());
		modelEditorScreenController->setPivot(entity->getPivot());
		entityPhysicsView->setBoundingVolumes(entity);
		entityPhysicsView->setPhysics(entity);
		entityPhysicsView->setTerrainMesh(entity);
		entityPhysicsView->setConvexMeshes(entity);
		modelEditorScreenController->setRendering(entity);
		modelEditorScreenController->setLODLevel(entity, lodLevel);
		modelEditorScreenController->setMaterials(entity);
		modelEditorScreenController->setAnimations(entity);
		modelEditorScreenController->setTools();
		entitySoundsView->setSounds(entity);
	} else {
		modelEditorScreenController->setScreenCaption("Model Editor - no entity loaded");
		modelEditorScreenController->unsetEntityProperties();
		modelEditorScreenController->unsetEntityData();
		modelEditorScreenController->unsetPivot();
		entityPhysicsView->unsetBoundingVolumes();
		entityPhysicsView->unsetPhysics();
		entityPhysicsView->unsetTerrainMesh();
		entityPhysicsView->unsetConvexMeshes();
		modelEditorScreenController->unsetRendering();
		modelEditorScreenController->unsetLODLevel();
		modelEditorScreenController->unsetMaterials();
		modelEditorScreenController->unsetAnimations();
		modelEditorScreenController->unsetTools();
		entitySoundsView->unsetSounds();
	}
}

void SharedModelEditorView::onInitAdditionalScreens()
{
}

void SharedModelEditorView::loadSettings()
{
	try {
		Properties settings;
		settings.load("settings", "modeleditor.properties");
		entityPhysicsView->setDisplayBoundingVolume(settings.get("display.boundingvolumes", "false") == "true");
		entityDisplayView->setDisplayGroundPlate(settings.get("display.groundplate", "true") == "true");
		entityDisplayView->setDisplayShadowing(settings.get("display.shadowing", "true") == "true");
		modelEditorScreenController->getModelPath()->setPath(settings.get("model.path", "."));
		modelEditorScreenController->getAudioPath()->setPath(settings.get("audio.path", "."));
	} catch (Exception& exception) {
		Console::print(string("SharedModelEditorView::loadSettings(): An error occurred: "));
		Console::println(string(exception.what()));
	}
}

void SharedModelEditorView::initialize()
{
	try {
		modelEditorScreenController = new ModelEditorScreenController(this);
		modelEditorScreenController->initialize();
		entityPhysicsView = modelEditorScreenController->getEntityPhysicsSubScreenController()->getView();
		entityDisplayView = modelEditorScreenController->getEntityDisplaySubScreenController()->getView();
		entitySoundsView = modelEditorScreenController->getEntitySoundsSubScreenController()->getView();
		engine->getGUI()->addScreen(modelEditorScreenController->getScreenNode()->getId(), modelEditorScreenController->getScreenNode());
		modelEditorScreenController->getScreenNode()->setInputEventHandler(this);
	} catch (Exception& exception) {
		Console::print(string("SharedModelEditorView::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	loadSettings();
	modelEditorScreenController->getEntityDisplaySubScreenController()->setupDisplay();
	modelEditorScreenController->setRenderingShaders(Engine::getRegisteredShader(Engine::ShaderType::OBJECT3D));
	entityPhysicsView->initialize();
	updateGUIElements();
}

void SharedModelEditorView::activate()
{
	engine->reset();
	engine->setPartition(new PartitionNone());
	engine->getGUI()->resetRenderScreens();
	engine->getGUI()->addRenderScreen(modelEditorScreenController->getScreenNode()->getId());
	onInitAdditionalScreens();
	engine->getGUI()->addRenderScreen(popUps->getFileDialogScreenController()->getScreenNode()->getId());
	engine->getGUI()->addRenderScreen(popUps->getInfoDialogScreenController()->getScreenNode()->getId());
	engine->getGUI()->addRenderScreen(popUps->getProgressBarScreenController()->getScreenNode()->getId());
}

void SharedModelEditorView::storeSettings()
{
	try {
		Properties settings;
		settings.put("display.boundingvolumes", entityPhysicsView->isDisplayBoundingVolume() == true ? "true" : "false");
		settings.put("display.groundplate", entityDisplayView->isDisplayGroundPlate() == true ? "true" : "false");
		settings.put("display.shadowing", entityDisplayView->isDisplayShadowing() == true ? "true" : "false");
		settings.put("model.path", modelEditorScreenController->getModelPath()->getPath());
		settings.put("audio.path", modelEditorScreenController->getAudioPath()->getPath());
		settings.store("settings", "modeleditor.properties");
	} catch (Exception& exception) {
		Console::print(string("SharedModelEditorView::storeSettings(): An error occurred: "));
		Console::println(string(exception.what()));
	}
}

void SharedModelEditorView::deactivate()
{
	audio->removeEntity("sound");
}

void SharedModelEditorView::dispose()
{
	storeSettings();
	engine->reset();
	audio->reset();
}

void SharedModelEditorView::onLoadModel(LevelEditorEntity* oldEntity, LevelEditorEntity* entity)
{
	delete oldEntity;
}

void SharedModelEditorView::loadModel()
{
	Console::println(string("Model file: " + modelFile));
	try {
		auto oldEntity = entity;
		setEntity(
			loadModel(
				FileSystem::getInstance()->getFileName(modelFile),
				"",
				FileSystem::getInstance()->getPathName(modelFile),
				FileSystem::getInstance()->getFileName(modelFile),
				Vector3()
			)
		);
		onLoadModel(oldEntity, entity);
	} catch (Exception& exception) {
		popUps->getInfoDialogScreenController()->show("Warning", (exception.what()));
	}
}

LevelEditorEntity* SharedModelEditorView::loadModel(const string& name, const string& description, const string& pathName, const string& fileName, const Vector3& pivot)
{
	if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".tmm") == true) {
		auto levelEditorEntity = ModelMetaDataFileImport::doImport(
			pathName,
			fileName
		);
		levelEditorEntity->setDefaultBoundingVolumes();
		return levelEditorEntity;
	} else {
		auto model = ModelReader::read(
			pathName,
			fileName
		);
		auto levelEditorEntity = new LevelEditorEntity(
			LevelEditorEntity::ID_NONE,
			LevelEditorEntity_EntityType::MODEL,
			name,
			description,
			"",
			pathName + "/" + fileName,
			StringTools::replace(StringTools::replace(StringTools::replace(model->getId(), "\\", "_"), "/", "_"), ":", "_") + ".png",
			model,
			pivot
			);
		levelEditorEntity->setDefaultBoundingVolumes();
		return levelEditorEntity;

	}
	return nullptr;
}

void SharedModelEditorView::playAnimation(const string& animationId) {
	auto object = dynamic_cast<Object3D*>(engine->getEntity("model"));
	if (object != nullptr) {
		audio->removeEntity("sound");
		object->setAnimation(animationId);
	}
}

void SharedModelEditorView::playSound(const string& soundId) {
	auto object = dynamic_cast<Object3D*>(engine->getEntity("model"));
	audio->removeEntity("sound");
	auto soundDefinition = entity->getSound(soundId);
	if (soundDefinition != nullptr && soundDefinition->getFileName().length() > 0) {
		if (object != nullptr && soundDefinition->getAnimation().size() > 0) object->setAnimation(soundDefinition->getAnimation());
		string pathName = ModelMetaDataFileImport::getResourcePathName(
			Tools::getPath(entity->getEntityFileName()),
			soundDefinition->getFileName()
		);
		string fileName = Tools::getFileName(soundDefinition->getFileName());
		auto sound = new Sound(
			"sound",
			pathName,
			fileName
		);
		sound->setGain(soundDefinition->getGain());
		sound->setPitch(soundDefinition->getPitch());
		sound->setLooping(soundDefinition->isLooping());
		sound->setFixed(true);
		audio->addEntity(sound);
		audioStarted = Time::getCurrentMillis();
		audioOffset = -1LL;
		if (soundDefinition->getOffset() <= 0) {
			sound->play();
		} else {
			audioOffset = soundDefinition->getOffset();
		}
	}
}

void SharedModelEditorView::updateRendering() {
	auto object = dynamic_cast<Object3D*>(engine->getEntity("model"));
	if (object == nullptr || entity == nullptr) return;
	engine->removeEntity("model");
	object->setShader(entity->getShader());
	object->setDistanceShader(entity->getDistanceShader());
	object->setDistanceShaderDistance(entity->getDistanceShaderDistance());
	ModelTools::prepareForShader(entity->getModel(), entity->getShader());
	resetEntity();
}

void SharedModelEditorView::onSetEntityData() {
}

void SharedModelEditorView::onRotation() {
	entityPhysicsView->updateGizmo(entity);
}

void SharedModelEditorView::onScale() {
	entityPhysicsView->updateGizmo(entity);
}
