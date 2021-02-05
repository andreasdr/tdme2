#include <tdme/tools/shared/views/SharedModelEditorView.h>

#include <string>

#include <tdme/audio/Audio.h>
#include <tdme/audio/Sound.h>
#include <tdme/engine/fileio/models/ModelReader.h>
#include <tdme/engine/fileio/prototypes/PrototypeReader.h>
#include <tdme/engine/fileio/prototypes/PrototypeWriter.h>
#include <tdme/engine/fileio/ProgressCallback.h>
#include <tdme/engine/model/AnimationSetup.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/prototype/Prototype.h>
#include <tdme/engine/prototype/Prototype_Type.h>
#include <tdme/engine/prototype/PrototypeAudio.h>
#include <tdme/engine/prototype/PrototypeProperty.h>
#include <tdme/engine/subsystems/rendering/ModelStatistics.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/ModelUtilities.h>
#include <tdme/engine/Object3D.h>
#include <tdme/engine/PartitionNone.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/gui/GUI.h>
#include <tdme/math/Vector3.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/tools/shared/controller/FileDialogPath.h>
#include <tdme/tools/shared/controller/FileDialogScreenController.h>
#include <tdme/tools/shared/controller/InfoDialogScreenController.h>
#include <tdme/tools/shared/controller/ModelEditorScreenController.h>
#include <tdme/tools/shared/controller/ProgressBarScreenController.h>
#include <tdme/tools/shared/controller/PrototypeDisplaySubScreenController.h>
#include <tdme/tools/shared/controller/PrototypePhysicsSubScreenController.h>
#include <tdme/tools/shared/controller/PrototypeSoundsSubScreenController.h>
#include <tdme/tools/shared/tools/Tools.h>
#include <tdme/tools/shared/views/CameraRotationInputHandler.h>
#include <tdme/tools/shared/views/PlayableSoundView.h>
#include <tdme/tools/shared/views/PopUps.h>
#include <tdme/tools/shared/views/PrototypeDisplayView.h>
#include <tdme/tools/shared/views/PrototypePhysicsView.h>
#include <tdme/tools/shared/views/PrototypeSoundsView.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>
#include <tdme/utilities/ModelTools.h>
#include <tdme/utilities/Properties.h>
#include <tdme/utilities/StringTools.h>

using std::string;

using tdme::audio::Audio;
using tdme::audio::Sound;
using tdme::engine::fileio::models::ModelReader;
using tdme::engine::fileio::prototypes::PrototypeReader;
using tdme::engine::fileio::prototypes::PrototypeWriter;
using tdme::engine::fileio::ProgressCallback;
using tdme::engine::model::Model;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::prototype::Prototype;
using tdme::engine::prototype::Prototype_Type;
using tdme::engine::prototype::PrototypeAudio;
using tdme::engine::prototype::PrototypeProperty;
using tdme::engine::subsystems::rendering::ModelStatistics;
using tdme::engine::Engine;
using tdme::engine::ModelUtilities;
using tdme::engine::Object3D;
using tdme::engine::PartitionNone;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::GUI;
using tdme::math::Vector3;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;
using tdme::tools::shared::controller::FileDialogPath;
using tdme::tools::shared::controller::FileDialogScreenController;
using tdme::tools::shared::controller::InfoDialogScreenController;
using tdme::tools::shared::controller::ModelEditorScreenController;
using tdme::tools::shared::controller::ProgressBarScreenController;
using tdme::tools::shared::controller::PrototypeDisplaySubScreenController;
using tdme::tools::shared::controller::PrototypePhysicsSubScreenController;
using tdme::tools::shared::tools::Tools;
using tdme::tools::shared::views::CameraRotationInputHandler;
using tdme::tools::shared::views::PlayableSoundView;
using tdme::tools::shared::views::PopUps;
using tdme::tools::shared::views::PrototypeDisplayView;
using tdme::tools::shared::views::PrototypePhysicsView;
using tdme::tools::shared::views::PrototypeSoundsView;
using tdme::tools::shared::views::SharedModelEditorView;
using tdme::utilities::Console;
using tdme::utilities::Exception;
using tdme::utilities::ModelTools;
using tdme::utilities::Properties;
using tdme::utilities::StringTools;

SharedModelEditorView::SharedModelEditorView(PopUps* popUps)
{
	this->popUps = popUps;
	engine = Engine::getInstance();
	audio = Audio::getInstance();
	modelEditorScreenController = nullptr;
	prototypeDisplayView = nullptr;
	prototypePhysicsView = nullptr;
	prototypeSoundsView = nullptr;
	loadModelRequested = false;
	initModelRequested = false;
	initModelRequestedReset = false;
	prototype = nullptr;
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

PopUps* SharedModelEditorView::getPopUps()
{
	return popUps;
}

Prototype* SharedModelEditorView::getPrototype()
{
	return prototype;
}

void SharedModelEditorView::setPrototype(Prototype* prototype)
{
	engine->reset();
	this->prototype = prototype;
	lodLevel = 1;
	initModelRequested = true;
	initModelRequestedReset = false;
}

void SharedModelEditorView::resetPrototype()
{
	engine->reset();
	initModelRequested = true;
	initModelRequestedReset = true;
}

void SharedModelEditorView::reimportPrototype()
{
	engine->reset();
	initModelRequested = true;
	initModelRequestedReset = false;
}

void SharedModelEditorView::initModel()
{
	if (prototype == nullptr) return;
	engine->removeEntity("model");
	engine->removeEntity("attachment1");
	if (attachment1Model != nullptr) {
		delete attachment1Model;
		attachment1Model = nullptr;
	}
	modelFile = prototype->getFileName().length() > 0 ? prototype->getFileName() : prototype->getModelFileName();
	Tools::setupPrototype(prototype, engine, cameraRotationInputHandler->getLookFromRotations(), cameraRotationInputHandler->getScale(), lodLevel, objectScale);
	Tools::oseThumbnail(prototype);
	cameraRotationInputHandler->setMaxAxisDimension(Tools::computeMaxAxisDimension(prototype->getModel()->getBoundingBox()));
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
		modelEditorScreenController->setMaterials();
		modelEditorScreenController->setAnimations();
	}
}

void SharedModelEditorView::loadFile(const string& pathName, const string& fileName)
{
	loadModelRequested = true;
	modelFile = FileSystem::getInstance()->getFileName(pathName, fileName);
}

void SharedModelEditorView::reimportModel(const string& pathName, const string& fileName)
{
	if (prototype == nullptr) return;
	engine->removeEntity("model");
	engine->removeEntity("attachment1");
	if (attachment1Model != nullptr) {
		delete attachment1Model;
		attachment1Model = nullptr;
	}
	struct AnimationSetupStruct {
		bool loop;
		string overlayFromNodeId;
		float speed;
	};
	// store old animation setups
	map<string, AnimationSetupStruct> originalAnimationSetups;
	for (auto animationSetupIt: prototype->getModel()->getAnimationSetups()) {
		auto animationSetup = animationSetupIt.second;
		originalAnimationSetups[animationSetup->getId()] = {
			.loop = animationSetup->isLoop(),
			.overlayFromNodeId = animationSetup->getOverlayFromNodeId(),
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
			animationSetup->setOverlayFromNodeId(originalAnimationSetup.overlayFromNodeId);
			animationSetup->setSpeed(originalAnimationSetup.speed);
		}
		// set model in entity
		prototype->setModel(model);
	} catch (Exception& exception) {
		modelEditorScreenController->showErrorPopUp("Warning", (string(exception.what())));
	}
	reimportPrototype();
	if (log.size() > 0) {
		modelEditorScreenController->showErrorPopUp("Warning", log);
	}
}

void SharedModelEditorView::saveFile(const string& pathName, const string& fileName)
{
	PrototypeWriter::write(pathName, fileName, prototype);
}

void SharedModelEditorView::reloadFile()
{
	loadModelRequested = true;
}

void SharedModelEditorView::pivotApply(float x, float y, float z)
{
	if (prototype == nullptr) return;
	prototype->getPivot().set(x, y, z);
}

void SharedModelEditorView::computeNormals() {
	if (prototype == nullptr || prototype->getModel() == nullptr) return;
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
	ModelTools::computeNormals(prototype->getModel(), new ComputeNormalsProgressCallback(popUps->getProgressBarScreenController()));
	popUps->getProgressBarScreenController()->close();
	resetPrototype();
}

void SharedModelEditorView::optimizeModel() {
	if (prototype == nullptr || prototype->getModel() == nullptr) return;
	engine->removeEntity("model");
	prototype->setModel(ModelTools::optimizeModel(prototype->unsetModel()));
	initModelRequested = true;
	modelEditorScreenController->setMaterials();
}

void SharedModelEditorView::handleInputEvents()
{
	prototypePhysicsView->handleInputEvents(prototype, objectScale);
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

	//
	auto model = static_cast<Object3D*>(engine->getEntity("model"));

	// attachment1
	auto attachment1 = static_cast<Object3D*>(engine->getEntity("attachment1"));
	if (model != nullptr && attachment1 != nullptr) {
		// model attachment bone matrix
		auto transformationsMatrix = model->getNodeTransformationsMatrix(attachment1Bone);
		transformationsMatrix*= model->getTransformations().getTransformationsMatrix();
		attachment1->setTranslation(transformationsMatrix * Vector3(0.0f, 0.0f, 0.0f));
		// euler angles
		Vector3 euler;
		transformationsMatrix.computeEulerAngles(euler);
		// rotations
		attachment1->setRotationAngle(0, euler.getZ());
		attachment1->setRotationAngle(1, euler.getY());
		attachment1->setRotationAngle(2, euler.getX());
		// scale
		Vector3 scale;
		transformationsMatrix.getScale(scale);
		attachment1->setScale(scale);
		// finally update
		attachment1->update();
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
	prototypeDisplayView->display(prototype);
	prototypePhysicsView->display(prototype);
	engine->getGUI()->handleEvents();
	engine->getGUI()->render();
	audio->update();
}

void SharedModelEditorView::updateGUIElements()
{
	if (prototype != nullptr) {
		modelEditorScreenController->setScreenCaption("Model Editor - " + (prototype->getFileName().length() > 0 ? Tools::getFileName(prototype->getFileName()) : Tools::getFileName(prototype->getModelFileName())));
		auto preset = prototype->getProperty("preset");
		modelEditorScreenController->setPrototypeProperties(preset != nullptr ? preset->getValue() : "", "");
		modelEditorScreenController->setPrototypeData(prototype->getName(), prototype->getDescription());
		modelEditorScreenController->setPivot(prototype->getPivot());
		prototypePhysicsView->setBoundingVolumes(prototype);
		prototypePhysicsView->setPhysics(prototype);
		prototypePhysicsView->setTerrainMesh(prototype);
		prototypePhysicsView->setConvexMeshes(prototype);
		modelEditorScreenController->setRendering();
		modelEditorScreenController->setShaderParameters();
		modelEditorScreenController->setLODLevel(lodLevel);
		modelEditorScreenController->setMaterials();
		modelEditorScreenController->setAnimations();
		modelEditorScreenController->setPreview();
		modelEditorScreenController->setTools();
		prototypeSoundsView->setSounds(prototype);
	} else {
		modelEditorScreenController->setScreenCaption("Model Editor - no entity loaded");
		modelEditorScreenController->unsetPrototypeProperties();
		modelEditorScreenController->unsetPrototypeData();
		modelEditorScreenController->unsetPivot();
		prototypePhysicsView->unsetBoundingVolumes();
		prototypePhysicsView->unsetPhysics();
		prototypePhysicsView->unsetTerrainMesh();
		prototypePhysicsView->unsetConvexMeshes();
		modelEditorScreenController->unsetRendering();
		modelEditorScreenController->unsetShaderParameters();
		modelEditorScreenController->unsetLODLevel();
		modelEditorScreenController->unsetMaterials();
		modelEditorScreenController->unsetAnimations();
		modelEditorScreenController->unsetPreview();
		modelEditorScreenController->unsetTools();
		prototypeSoundsView->unsetSounds();
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
		prototypePhysicsView->setDisplayBoundingVolume(settings.get("display.boundingvolumes", "false") == "true");
		prototypeDisplayView->setDisplayGroundPlate(settings.get("display.groundplate", "true") == "true");
		prototypeDisplayView->setDisplayShadowing(settings.get("display.shadowing", "true") == "true");
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
		prototypePhysicsView = modelEditorScreenController->getPrototypePhysicsSubScreenController()->getView();
		prototypeDisplayView = modelEditorScreenController->getPrototypeDisplaySubScreenController()->getView();
		prototypeSoundsView = modelEditorScreenController->getPrototypeSoundsSubScreenController()->getView();
		engine->getGUI()->addScreen(modelEditorScreenController->getScreenNode()->getId(), modelEditorScreenController->getScreenNode());
		modelEditorScreenController->getScreenNode()->setInputEventHandler(this);
	} catch (Exception& exception) {
		Console::print(string("SharedModelEditorView::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	loadSettings();
	modelEditorScreenController->getPrototypeDisplaySubScreenController()->setupDisplay();
	modelEditorScreenController->setRenderingShaders(Engine::getRegisteredShader(Engine::ShaderType::SHADERTYPE_OBJECT3D));
	prototypePhysicsView->initialize();
	updateGUIElements();
}

void SharedModelEditorView::activate()
{
	engine->reset();
	engine->setPartition(new PartitionNone());
	engine->setShadowMapLightEyeDistanceScale(0.1f);
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
		settings.put("display.boundingvolumes", prototypePhysicsView->isDisplayBoundingVolume() == true ? "true" : "false");
		settings.put("display.groundplate", prototypeDisplayView->isDisplayGroundPlate() == true ? "true" : "false");
		settings.put("display.shadowing", prototypeDisplayView->isDisplayShadowing() == true ? "true" : "false");
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

void SharedModelEditorView::onLoadModel(Prototype* oldEntity, Prototype* entity)
{
	delete oldEntity;
}

void SharedModelEditorView::loadModel()
{
	Console::println(string("Model file: " + modelFile));
	try {
		auto oldEntity = prototype;
		setPrototype(
			loadModel(
				FileSystem::getInstance()->getFileName(modelFile),
				"",
				FileSystem::getInstance()->getPathName(modelFile),
				FileSystem::getInstance()->getFileName(modelFile),
				Vector3()
			)
		);
		onLoadModel(oldEntity, prototype);
	} catch (Exception& exception) {
		popUps->getInfoDialogScreenController()->show("Warning", (exception.what()));
	}
}

Prototype* SharedModelEditorView::loadModel(const string& name, const string& description, const string& pathName, const string& fileName, const Vector3& pivot)
{
	if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".tmm") == true) {
		auto prototype = PrototypeReader::read(
			pathName,
			fileName
		);
		prototype->setDefaultBoundingVolumes();
		return prototype;
	} else {
		auto model = ModelReader::read(
			pathName,
			fileName
		);
		auto prototype = new Prototype(
			Prototype::ID_NONE,
			Prototype_Type::MODEL,
			name,
			description,
			"",
			pathName + "/" + fileName,
			StringTools::replace(StringTools::replace(StringTools::replace(model->getId(), "\\", "_"), "/", "_"), ":", "_") + ".png",
			model,
			pivot
			);
		prototype->setDefaultBoundingVolumes();
		return prototype;

	}
	return nullptr;
}

void SharedModelEditorView::playAnimation(const string& baseAnimationId, const string& overlay1AnimationId, const string& overlay2AnimationId, const string& overlay3AnimationId) {
	auto object = dynamic_cast<Object3D*>(engine->getEntity("model"));
	if (object != nullptr) {
		audio->removeEntity("sound");
		object->removeOverlayAnimations();
		object->setAnimation(baseAnimationId);
		if (overlay1AnimationId.empty() == false) object->addOverlayAnimation(overlay1AnimationId);
		if (overlay2AnimationId.empty() == false) object->addOverlayAnimation(overlay2AnimationId);
		if (overlay3AnimationId.empty() == false) object->addOverlayAnimation(overlay3AnimationId);
	}
}

void SharedModelEditorView::addAttachment1(const string& nodeId, const string& attachmentModelFile) {
	engine->removeEntity("attachment1");
	try {
		if (attachment1Model != nullptr) delete attachment1Model;
		attachment1Model = attachmentModelFile.empty() == true?nullptr:ModelReader::read(Tools::getPathName(attachmentModelFile), Tools::getFileName(attachmentModelFile));
	} catch (Exception& exception) {
		Console::print(string("SharedModelEditorView::addAttachment1(): An error occurred: "));
		Console::println(string(exception.what()));
		popUps->getInfoDialogScreenController()->show("Warning", (exception.what()));
	}
	if (attachment1Model != nullptr) {
		Entity* attachment = nullptr;
		engine->addEntity(attachment = new Object3D("attachment1", attachment1Model));
		attachment->addRotation(Vector3(0.0f, 0.0f, 1.0f), 0.0f);
		attachment->addRotation(Vector3(0.0f, 1.0f, 0.0f), 0.0f);
		attachment->addRotation(Vector3(1.0f, 0.0f, 0.0f), 0.0f);
	}
	attachment1Bone = nodeId;
}

void SharedModelEditorView::playSound(const string& soundId) {
	auto object = dynamic_cast<Object3D*>(engine->getEntity("model"));
	audio->removeEntity("sound");
	auto soundDefinition = prototype->getSound(soundId);
	if (soundDefinition != nullptr && soundDefinition->getFileName().length() > 0) {
		if (object != nullptr && soundDefinition->getAnimation().size() > 0) object->setAnimation(soundDefinition->getAnimation());
		string pathName = PrototypeReader::getResourcePathName(
			Tools::getPathName(prototype->getFileName()),
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
	if (object == nullptr || prototype == nullptr) return;
	engine->removeEntity("model");
	ModelTools::prepareForShader(prototype->getModel(), prototype->getShader());
	resetPrototype();
}

void SharedModelEditorView::updateShaderParemeters() {
	auto object = dynamic_cast<Object3D*>(engine->getEntity("model"));
	if (object == nullptr || prototype == nullptr) return;
	auto shaderParametersDefault = Engine::getShaderParameterDefaults(prototype->getShader());
	auto distanceShaderParametersDefault = Engine::getShaderParameterDefaults(prototype->getDistanceShader());
	for (auto& parameterIt: shaderParametersDefault) {
		auto& parameterName = parameterIt.first;
		auto parameterValue = prototype->getShaderParameters().getShaderParameter(parameterName);
		object->setShaderParameter(parameterName, parameterValue);
	}
	for (auto& parameterIt: distanceShaderParametersDefault) {
		auto& parameterName = parameterIt.first;
		auto parameterValue = prototype->getDistanceShaderParameters().getShaderParameter(parameterName);
		object->setDistanceShaderParameter(parameterName, parameterValue);
	}
}

void SharedModelEditorView::onSetPrototypeData() {
}

void SharedModelEditorView::onCameraRotation() {
	prototypePhysicsView->updateGizmo(prototype);
}

void SharedModelEditorView::onCameraScale() {
	prototypePhysicsView->updateGizmo(prototype);
}
