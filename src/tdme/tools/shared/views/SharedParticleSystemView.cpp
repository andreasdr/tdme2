#include <tdme/tools/shared/views/SharedParticleSystemView.h>

#include <string>

#include <tdme/audio/Audio.h>
#include <tdme/audio/Sound.h>
#include <tdme/engine/fileio/prototypes/PrototypeReader.h>
#include <tdme/engine/fileio/prototypes/PrototypeWriter.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/prototype/Prototype.h>
#include <tdme/engine/prototype/Prototype_Type.h>
#include <tdme/engine/prototype/PrototypeAudio.h>
#include <tdme/engine/prototype/PrototypeParticleSystem_BoundingBoxParticleEmitter.h>
#include <tdme/engine/prototype/PrototypeParticleSystem_CircleParticleEmitter.h>
#include <tdme/engine/prototype/PrototypeParticleSystem_CircleParticleEmitterPlaneVelocity.h>
#include <tdme/engine/prototype/PrototypeParticleSystem_Emitter.h>
#include <tdme/engine/prototype/PrototypeParticleSystem_PointParticleEmitter.h>
#include <tdme/engine/prototype/PrototypeParticleSystem_SphereParticleEmitter.h>
#include <tdme/engine/prototype/PrototypeProperty.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Entity.h>
#include <tdme/engine/ParticleSystemEntity.h>
#include <tdme/engine/ParticleSystemGroup.h>
#include <tdme/engine/PartitionNone.h>
#include <tdme/engine/SceneConnector.h>
#include <tdme/engine/SceneConnector.h>
#include <tdme/gui/events/GUIKeyboardEvent.h>
#include <tdme/gui/events/GUIMouseEvent.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/gui/GUI.h>
#include <tdme/math/Vector3.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/tools/shared/controller/FileDialogPath.h>
#include <tdme/tools/shared/controller/FileDialogScreenController.h>
#include <tdme/tools/shared/controller/InfoDialogScreenController.h>
#include <tdme/tools/shared/controller/ParticleSystemScreenController.h>
#include <tdme/tools/shared/controller/PrototypeDisplaySubScreenController.h>
#include <tdme/tools/shared/controller/PrototypePhysicsSubScreenController.h>
#include <tdme/tools/shared/controller/PrototypeSoundsSubScreenController.h>
#include <tdme/tools/shared/tools/Tools.h>
#include <tdme/tools/shared/views/CameraRotationInputHandler.h>
#include <tdme/tools/shared/views/Gizmo.h>
#include <tdme/tools/shared/views/PlayableSoundView.h>
#include <tdme/tools/shared/views/PopUps.h>
#include <tdme/tools/shared/views/PrototypeDisplayView.h>
#include <tdme/tools/shared/views/PrototypePhysicsView.h>
#include <tdme/tools/shared/views/PrototypeSoundsView.h>
#include <tdme/tools/shared/views/View.h>
#include <tdme/utilities/Character.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>
#include <tdme/utilities/Properties.h>
#include <tdme/utilities/StringTools.h>

using std::string;

using tdme::tools::shared::views::SharedParticleSystemView;

using tdme::audio::Audio;
using tdme::audio::Sound;
using tdme::engine::fileio::prototypes::PrototypeReader;
using tdme::engine::fileio::prototypes::PrototypeWriter;
using tdme::engine::model::Model;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::prototype::Prototype;
using tdme::engine::prototype::PrototypeAudio;
using tdme::engine::prototype::PrototypeParticleSystem_BoundingBoxParticleEmitter;
using tdme::engine::prototype::PrototypeParticleSystem_CircleParticleEmitter;
using tdme::engine::prototype::PrototypeParticleSystem_CircleParticleEmitterPlaneVelocity;
using tdme::engine::prototype::PrototypeParticleSystem_Emitter;
using tdme::engine::prototype::PrototypeParticleSystem_Emitter;
using tdme::engine::prototype::PrototypeParticleSystem_PointParticleEmitter;
using tdme::engine::prototype::PrototypeParticleSystem_SphereParticleEmitter;
using tdme::engine::prototype::PrototypeProperty;
using tdme::engine::Engine;
using tdme::engine::Entity;
using tdme::engine::ParticleSystemEntity;
using tdme::engine::ParticleSystemGroup;
using tdme::engine::PartitionNone;
using tdme::engine::SceneConnector;
using tdme::engine::SceneConnector;
using tdme::gui::events::GUIKeyboardEvent;
using tdme::gui::events::GUIMouseEvent;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::GUI;
using tdme::math::Vector3;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;
using tdme::tools::shared::controller::FileDialogPath;
using tdme::tools::shared::controller::FileDialogScreenController;
using tdme::tools::shared::controller::InfoDialogScreenController;
using tdme::tools::shared::controller::ParticleSystemScreenController;
using tdme::tools::shared::controller::PrototypeDisplaySubScreenController;
using tdme::tools::shared::controller::PrototypePhysicsSubScreenController;
using tdme::tools::shared::controller::PrototypeSoundsSubScreenController;
using tdme::tools::shared::tools::Tools;
using tdme::tools::shared::views::CameraRotationInputHandler;
using tdme::tools::shared::views::Gizmo;
using tdme::tools::shared::views::PlayableSoundView;
using tdme::tools::shared::views::PopUps;
using tdme::tools::shared::views::PrototypeDisplayView;
using tdme::tools::shared::views::PrototypePhysicsView;
using tdme::tools::shared::views::PrototypeSoundsView;
using tdme::tools::shared::views::View;
using tdme::utilities::Character;
using tdme::utilities::Console;
using tdme::utilities::Exception;
using tdme::utilities::Properties;
using tdme::utilities::StringTools;

SharedParticleSystemView::SharedParticleSystemView(PopUps* popUps): Gizmo(Engine::getInstance(), "spsv")
{
	this->popUps = popUps;
	engine = Engine::getInstance();
	audio = Audio::getInstance();
	particleSystemScreenController = nullptr;
	prototypeDisplayView = nullptr;
	prototypePhysicsView = nullptr;
	prototypeSoundsView = nullptr;
	loadParticleSystemRequested = false;
	initParticleSystemRequested = false;
	updateParticleSystemRequested = false;
	particleSystemFile = "";
	cameraRotationInputHandler = new CameraRotationInputHandler(engine);
	prototype = nullptr;
}

SharedParticleSystemView::~SharedParticleSystemView() {
	delete cameraRotationInputHandler;
}

PopUps* SharedParticleSystemView::getPopUps()
{
	return popUps;
}

Prototype* SharedParticleSystemView::getPrototype()
{
	return prototype;
}

void SharedParticleSystemView::reset()
{
	engine->reset();
}

void SharedParticleSystemView::setPrototype(Prototype* prototype)
{
	engine->reset();
	this->prototype = prototype;
	this->particleSystemIdx = 0;
	initParticleSystemRequested = true;
	prototype->setDefaultBoundingVolumes();
}

void SharedParticleSystemView::initParticleSystemRequest()
{
	initParticleSystemRequested = true;
}

void SharedParticleSystemView::updateParticleSystemRequest() {
	updateParticleSystemRequested = true;
}

void SharedParticleSystemView::initParticleSystem()
{
	if (prototype == nullptr)
		return;

	particleSystemFile = prototype->getFileName();
	Tools::setupPrototype(prototype, engine, cameraRotationInputHandler->getLookFromRotations(), cameraRotationInputHandler->getScale(), 1, objectScale);
	Tools::oseThumbnail(prototype);
	updateGUIElements();
}

const string SharedParticleSystemView::getFileName()
{
	if (particleSystemFile.length() == 0) return particleSystemFile;
	return FileSystem::getInstance()->getFileName(particleSystemFile);
}

void SharedParticleSystemView::loadFile(const string& pathName, const string& fileName)
{
	loadParticleSystemRequested = true;
	particleSystemFile = pathName + "/" + fileName;
}

void SharedParticleSystemView::saveFile(const string& pathName, const string& fileName)
{
	PrototypeWriter::write(pathName, fileName, prototype);
}

void SharedParticleSystemView::reloadFile()
{
	loadParticleSystemRequested = true;
}

void SharedParticleSystemView::handleInputEvents()
{
	if (prototypePhysicsView->isEditingBoundingVolume(prototype) == false) {
		for (auto i = 0; i < engine->getGUI()->getKeyboardEvents().size(); i++) {
			auto& event = engine->getGUI()->getKeyboardEvents()[i];
			if (event.isProcessed() == true) continue;
			auto isKeyDown = event.getType() == GUIKeyboardEvent::KEYBOARDEVENT_KEY_PRESSED;
			if (Character::toLowerCase(event.getKeyChar()) == '1') { if (isKeyDown == true) setGizmoType(GIZMOTYPE_ALL); updateGizmo(prototype); event.setProcessed(true); }
			if (Character::toLowerCase(event.getKeyChar()) == '2') { if (isKeyDown == true) setGizmoType(GIZMOTYPE_TRANSLATE); updateGizmo(prototype); event.setProcessed(true); }
			if (Character::toLowerCase(event.getKeyChar()) == '3') { if (isKeyDown == true) setGizmoType(GIZMOTYPE_ROTATE); updateGizmo(prototype); event.setProcessed(true); }
			if (Character::toLowerCase(event.getKeyChar()) == '4') { if (isKeyDown == true) setGizmoType(GIZMOTYPE_SCALE); updateGizmo(prototype); event.setProcessed(true); }
		}
		for (auto i = 0; i < engine->getGUI()->getMouseEvents().size(); i++) {
			auto& event = engine->getGUI()->getMouseEvents()[i];

			if (event.isProcessed() == true) continue;

			if (event.getButton() == MOUSE_BUTTON_LEFT) {
				if (event.getType() == GUIMouseEvent::MOUSEEVENT_RELEASED) {
					auto selectedEntity = engine->getEntity("model");
					auto psg = dynamic_cast<ParticleSystemGroup*>(selectedEntity);
					if (psg != nullptr) selectedEntity = psg->getParticleSystems()[particleSystemIdx];
					if (getGizmoMode() != GIZMOMODE_NONE) {
						if (selectedEntity != nullptr) applyParticleSystemTransformations(dynamic_cast<ParticleSystemEntity*>(selectedEntity), false);
						setGizmoMode(GIZMOMODE_NONE);
						updateGizmo(prototype);
					}
					totalDeltaScale.set(0.0, 0.0f, 0.0f);
					event.setProcessed(true);
				} else
				if (event.getType() == GUIMouseEvent::MOUSEEVENT_PRESSED) {
					Node* selectedEntityNode = nullptr;
					ParticleSystemEntity* selectedSubParticleSystem = nullptr;
					Entity* selectedEntity = nullptr;
					if (getGizmoMode() == GIZMOMODE_NONE) selectedEntity = engine->getEntityByMousePosition(event.getXUnscaled(), event.getYUnscaled(), nullptr, &selectedEntityNode, &selectedSubParticleSystem);
					if (getGizmoMode() == GIZMOMODE_NONE && selectedEntity == nullptr) {
						removeGizmo();
						totalDeltaScale.set(0.0, 0.0f, 0.0f);
						event.setProcessed(true);
					} else
					if (determineGizmoMode(selectedEntity, selectedEntityNode) == true) {
						mouseDownLastX = event.getXUnscaled();
						mouseDownLastY = event.getYUnscaled();
						event.setProcessed(true);
					} else
					if (selectedEntity != nullptr) {
						auto psg = dynamic_cast<ParticleSystemGroup*>(selectedEntity);
						if (psg != nullptr && selectedSubParticleSystem != nullptr) {
							auto particleSystemIdx = 0;
							for (auto pse: psg->getParticleSystems()) {
								if (pse == selectedSubParticleSystem) {
									setParticleSystemIndex(particleSystemIdx);
									break;
								}
								particleSystemIdx++;
							}
						} else {
							setParticleSystemIndex(0);
						}
						event.setProcessed(true);
					}
				} else
				if (event.getType() == GUIMouseEvent::MOUSEEVENT_DRAGGED) {
					if (getGizmoMode() != GIZMOMODE_NONE) {
						Vector3 deltaTranslation;
						Vector3 deltaRotation;
						Vector3 deltaScale;
						if (determineGizmoDeltaTransformations(mouseDownLastX, mouseDownLastY, event.getXUnscaled(), event.getYUnscaled(), deltaTranslation, deltaRotation, deltaScale) == true) {
							totalDeltaScale.add(deltaScale.clone().sub(Vector3(1.0f, 1.0f, 1.0f)));
							auto gizmoEntity = getGizmoObject3D();
							auto selectedEntity = engine->getEntity("model");
							auto psg = dynamic_cast<ParticleSystemGroup*>(selectedEntity);
							if (psg != nullptr) selectedEntity = psg->getParticleSystems()[particleSystemIdx];
							if (gizmoEntity != nullptr && selectedEntity != nullptr) {
								selectedEntity->setTranslation(selectedEntity->getTranslation().clone().add(deltaTranslation));
								selectedEntity->update();
								auto localTransformations = dynamic_cast<ParticleSystemEntity*>(selectedEntity)->getLocalTransformations();
								localTransformations.setScale(localTransformations.getScale().clone().scale(deltaScale));
								if (localTransformations.getRotationCount() == 0) {
									localTransformations.addRotation(Rotation::Z_AXIS, 0.0f);
									localTransformations.addRotation(Rotation::Y_AXIS, 0.0f);
									localTransformations.addRotation(Rotation::X_AXIS, 0.0f);
								}
								localTransformations.setRotationAngle(0, localTransformations.getRotationAngle(0) + deltaRotation[2]);
								localTransformations.setRotationAngle(1, localTransformations.getRotationAngle(1) + deltaRotation[1]);
								localTransformations.setRotationAngle(2, localTransformations.getRotationAngle(2) + deltaRotation[0]);
								localTransformations.update();
								dynamic_cast<ParticleSystemEntity*>(selectedEntity)->setLocalTransformations(localTransformations);
								setGizmoRotation(prototype, localTransformations);
								applyParticleSystemTransformations(dynamic_cast<ParticleSystemEntity*>(selectedEntity), true);
							}
							if (Math::abs(deltaTranslation.getX()) > Math::EPSILON ||
								Math::abs(deltaTranslation.getY()) > Math::EPSILON ||
								Math::abs(deltaTranslation.getZ()) > Math::EPSILON) {
								updateGizmo(prototype);
							}
						}
						mouseDownLastX = event.getXUnscaled();
						mouseDownLastY = event.getYUnscaled();
						event.setProcessed(true);
					}
				}
			}
		}
	} else {
		removeGizmo();
		prototypePhysicsView->handleInputEvents(prototype, objectScale);
	}
	cameraRotationInputHandler->handleInputEvents();
}

void SharedParticleSystemView::display()
{
	// audio
	if (audioOffset > 0 && Time::getCurrentMillis() - audioStarted >= audioOffset) {
		auto sound = audio->getEntity("sound");
		if (sound != nullptr) sound->play();
		audioOffset = -1LL;
	}

	// commands
	if (loadParticleSystemRequested == true) {
		initParticleSystemRequested = true;
		loadParticleSystemRequested = false;
		loadParticleSystem();
		cameraRotationInputHandler->reset();
	}
	if (initParticleSystemRequested == true) {
		initParticleSystem();
		initParticleSystemRequested = false;
		cameraRotationInputHandler->reset();
	}
	if (updateParticleSystemRequested == true) {
		initParticleSystem();
		updateParticleSystemRequested = false;
	}
	auto particleSystemEntity = dynamic_cast< ParticleSystemEntity* >(engine->getEntity("model"));
	if (particleSystemEntity != nullptr && particleSystemEntity->isAutoEmit() == false) {
		particleSystemEntity->emitParticles();
		particleSystemEntity->updateParticles();
	}

	// viewport
	auto xScale = (float)engine->getWidth() / (float)particleSystemScreenController->getScreenNode()->getScreenWidth();
	auto yScale = (float)engine->getHeight() / (float)particleSystemScreenController->getScreenNode()->getScreenHeight();
	auto viewPortLeft = 0;
	auto viewPortTop = 0;
	auto viewPortWidth = 0;
	auto viewPortHeight = 0;
	particleSystemScreenController->getViewPort(viewPortLeft, viewPortTop, viewPortWidth, viewPortHeight);
	viewPortLeft = (int)((float)viewPortLeft * xScale);
	viewPortTop = (int)((float)viewPortTop * yScale);
	viewPortWidth = (int)((float)viewPortWidth * xScale);
	viewPortHeight = (int)((float)viewPortHeight * yScale);
	engine->getCamera()->enableViewPort(viewPortLeft, viewPortTop, viewPortWidth, viewPortHeight);

	// rendering
	prototypeDisplayView->display(prototype);
	prototypePhysicsView->display(prototype);
}

void SharedParticleSystemView::updateGUIElements()
{
	if (prototype != nullptr && prototype->getParticleSystemsCount() > 0) {
		particleSystemScreenController->setScreenCaption("Particle System - " + (prototype->getFileName().length() > 0 ? FileSystem::getInstance()->getFileName(prototype->getFileName()) : prototype->getName()));
		auto preset = prototype->getProperty("preset");
		particleSystemScreenController->setPrototypeProperties(preset != nullptr ? preset->getValue() : "", prototype, "");
		particleSystemScreenController->setPrototypeData(prototype->getName(), prototype->getDescription());
		prototypePhysicsView->setBoundingVolumes(prototype);
		prototypePhysicsView->setPhysics(prototype);
		prototypeSoundsView->setSounds(prototype);
		particleSystemScreenController->setParticleSystemType();
		particleSystemScreenController->setParticleSystemEmitter();
		particleSystemScreenController->setParticleSystemListBox(prototype->getParticleSystemsCount(), particleSystemIdx);
	} else {
		particleSystemScreenController->setScreenCaption("Particle System - no particle system");
		particleSystemScreenController->unsetPrototypeProperties();
		particleSystemScreenController->unsetPrototypeData();
		prototypePhysicsView->unsetBoundingVolumes();
		prototypePhysicsView->unsetPhysics();
		prototypeSoundsView->unsetSounds();
		particleSystemScreenController->unsetParticleSystemType();
		particleSystemScreenController->unsetParticleSystemEmitter();
		particleSystemScreenController->setParticleSystemListBox(0, particleSystemIdx);
		particleSystemScreenController->unsetParticleSystemListBox();
	}
}

void SharedParticleSystemView::onInitAdditionalScreens()
{
}

void SharedParticleSystemView::loadSettings()
{
	try {
		Properties settings;
		settings.load("settings", "particlesystem.properties");
		prototypePhysicsView->setDisplayBoundingVolume(settings.get("display.boundingvolumes", "false") == "true");
		prototypeDisplayView->setDisplayGroundPlate(settings.get("display.groundplate", "true") == "true");
		prototypeDisplayView->setDisplayShadowing(settings.get("display.shadowing", "true") == "true");
		particleSystemScreenController->getParticleSystemPath()->setPath(settings.get("particlesystem.path", "."));
		particleSystemScreenController->getModelPath()->setPath(settings.get("model.path", "."));
	} catch (Exception& exception) {
		Console::print(string("SharedParticleSystemView::loadSettings(): An error occurred: "));
		Console::println(string(exception.what()));
	}
}

void SharedParticleSystemView::initialize()
{
	try {
		particleSystemScreenController = new ParticleSystemScreenController(this);
		particleSystemScreenController->initialize();
		prototypeDisplayView = particleSystemScreenController->getPrototypeDisplaySubScreenController()->getView();
		prototypePhysicsView = particleSystemScreenController->getPrototypePhysicsSubScreenController()->getView();
		prototypeSoundsView = particleSystemScreenController->getPrototypeSoundsSubScreenController()->getView();
		engine->getGUI()->addScreen(particleSystemScreenController->getScreenNode()->getId(), particleSystemScreenController->getScreenNode());
		particleSystemScreenController->getScreenNode()->setInputEventHandler(this);
	} catch (Exception& exception) {
		Console::print(string("SharedParticleSystemView::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}

	loadSettings();
	prototypePhysicsView->initialize();
	vector<string> particleSystemTypes;
	particleSystemTypes.push_back("None");
	particleSystemTypes.push_back("Object Particle System");
	particleSystemTypes.push_back("Points Particle System");
	particleSystemTypes.push_back("Fog Particle System");
	particleSystemScreenController->setParticleSystemTypes(particleSystemTypes);
	vector<string> particleSystemEmitters;
	particleSystemEmitters.push_back("None");
	particleSystemEmitters.push_back("Point Particle Emitter");
	particleSystemEmitters.push_back("BoundingBox Particle Emitter");
	particleSystemEmitters.push_back("Circle Particle Emitter");
	particleSystemEmitters.push_back("Circle Particle Emitter Plane Velocity");
	particleSystemEmitters.push_back("Sphere Particle Emitter");
	particleSystemScreenController->setParticleSystemEmitters(particleSystemEmitters);
	particleSystemScreenController->getPrototypeDisplaySubScreenController()->setupDisplay();
	updateGUIElements();
	initParticleSystemRequested = true;
}

void SharedParticleSystemView::activate()
{
	engine->reset();
	engine->setPartition(new PartitionNone());
	engine->setShadowMapLightEyeDistanceScale(0.1f);
	engine->getGUI()->resetRenderScreens();
	engine->getGUI()->addRenderScreen(particleSystemScreenController->getScreenNode()->getId());
	onInitAdditionalScreens();
	engine->getGUI()->addRenderScreen(popUps->getFileDialogScreenController()->getScreenNode()->getId());
	engine->getGUI()->addRenderScreen(popUps->getInfoDialogScreenController()->getScreenNode()->getId());
}

void SharedParticleSystemView::storeSettings()
{
	try {
		Properties settings;
		settings.put("display.boundingvolumes", prototypePhysicsView->isDisplayBoundingVolume() == true ? "true" : "false");
		settings.put("display.groundplate", prototypeDisplayView->isDisplayGroundPlate() == true ? "true" : "false");
		settings.put("display.shadowing", prototypeDisplayView->isDisplayShadowing() == true ? "true" : "false");
		settings.put("particlesystem.path", particleSystemScreenController->getParticleSystemPath()->getPath());
		settings.put("model.path", particleSystemScreenController->getModelPath()->getPath());
		settings.store("settings", "particlesystem.properties");
	} catch (Exception& exception) {
		Console::print(string("SharedParticleSystemView::storeSettings(): An error occurred "));
		Console::println(string(exception.what()));
	}
}

void SharedParticleSystemView::dispose()
{
	storeSettings();
	engine->reset();
	audio->reset();
}

void SharedParticleSystemView::deactivate()
{
	audio->removeEntity("sound");
}

void SharedParticleSystemView::onLoadParticleSystem(Prototype* oldEntity, Prototype* entity)
{
	delete oldEntity;
}

void SharedParticleSystemView::loadParticleSystem()
{
	Console::println(string("Particle system file: " + particleSystemFile));
	try {
		auto oldEntity = prototype;
		setPrototype(
			loadParticleSystemPrototype(
				particleSystemFile,
				"",
				FileSystem::getInstance()->getPathName(particleSystemFile),
				FileSystem::getInstance()->getFileName(particleSystemFile)
			)
		);
		onLoadParticleSystem(oldEntity, prototype);
	} catch (Exception& exception) {
		popUps->getInfoDialogScreenController()->show("Warning", (exception.what()));
	}
}

Prototype* SharedParticleSystemView::loadParticleSystemPrototype(const string& name, const string& description, const string& pathName, const string& fileName)
{
	if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".tparticle") == true) {
		auto prototype = PrototypeReader::read(pathName, fileName);
		prototype->setDefaultBoundingVolumes();
		return prototype;
	}
	return nullptr;
}

void SharedParticleSystemView::onSetEntityData()
{
}

int SharedParticleSystemView::getParticleSystemIndex() {
	return particleSystemIdx;
}

void SharedParticleSystemView::setParticleSystemIndex(int idx) {
	Console::println("SharedParticleSystemView::setParticleSystemIndex(): " + to_string(idx));
	this->particleSystemIdx = idx;
	updateGUIElements();
	updateGizmo(prototype);
	totalDeltaScale.set(0.0, 0.0f, 0.0f);
}

void SharedParticleSystemView::playSound(const string& soundId) {
	audio->removeEntity("sound");
	auto soundDefinition = prototype->getSound(soundId);
	if (soundDefinition != nullptr && soundDefinition->getFileName().length() > 0) {
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

void SharedParticleSystemView::updateGizmo(Prototype* entity) {
	auto selectedEntity = engine->getEntity("model");
	auto psg = dynamic_cast<ParticleSystemGroup*>(selectedEntity);
	if (psg != nullptr) {
		selectedEntity = particleSystemIdx >= psg->getParticleSystems().size()?nullptr:psg->getParticleSystems()[particleSystemIdx];
	} else {
		if (particleSystemIdx > 0) selectedEntity = nullptr;
	}
	auto pse = dynamic_cast<ParticleSystemEntity*>(selectedEntity);
	if (selectedEntity != nullptr) {
		if (pse != nullptr) {
			Gizmo::updateGizmo(pse->getEmitter()->getCenter().clone().scale(objectScale).add(selectedEntity->getTranslation()), selectedEntity->getTransformations());
		} else {
			Gizmo::updateGizmo(selectedEntity->getBoundingBoxTransformed()->getCenter(), selectedEntity->getTransformations());
		}
	} else {
		removeGizmo();
	}
}

void SharedParticleSystemView::setGizmoRotation(Prototype* entity, const Transformations& transformations) {
	Gizmo::setGizmoRotation(transformations);
}

void SharedParticleSystemView::applyParticleSystemTransformations(ParticleSystemEntity* particleSystemEntity, bool guiOnly) {
	{
		auto transformations = particleSystemEntity->getTransformations();
		auto localTransformations = particleSystemEntity->getLocalTransformations();
		auto objectScaleInverted = Vector3(
			1.0f / objectScale.getX(),
			1.0f / objectScale.getY(),
			1.0f / objectScale.getZ()
		);
		transformations.setScale(objectScaleInverted);
		transformations.update();
		auto particleSystem = prototype->getParticleSystemAt(particleSystemIdx);
		auto emitterType = particleSystem->getEmitter();
		if (emitterType == PrototypeParticleSystem_Emitter::NONE) {
			// no op
		} else
		if (emitterType == PrototypeParticleSystem_Emitter::POINT_PARTICLE_EMITTER) {
			auto emitter = particleSystem->getPointParticleEmitter();
			auto position = transformations.getTranslation().clone().scale(objectScaleInverted).add(emitter->getPosition());
			if (guiOnly == false) {
				emitter->setPosition(position);
			} else {
				particleSystemScreenController->updatePointParticleSystemEmitter(position);
			}
		} else
		if (emitterType == PrototypeParticleSystem_Emitter::BOUNDINGBOX_PARTICLE_EMITTER) {
			auto emitter = particleSystem->getBoundingBoxParticleEmitters();
			auto center = transformations.getTranslation().clone().scale(objectScaleInverted).add(emitter->getObbCenter());
			auto axis0 = emitter->getObbAxis0().clone().scale(emitter->getObbHalfextension().getX() * 2.0f);
			auto axis1 = emitter->getObbAxis1().clone().scale(emitter->getObbHalfextension().getY() * 2.0f);
			auto axis2 = emitter->getObbAxis2().clone().scale(emitter->getObbHalfextension().getZ() * 2.0f);
			auto halfExtension = emitter->getObbHalfextension();
			axis0 = localTransformations.getTransformationsMatrix().multiplyNoTranslation(axis0);
			axis1 = localTransformations.getTransformationsMatrix().multiplyNoTranslation(axis1);
			axis2 = localTransformations.getTransformationsMatrix().multiplyNoTranslation(axis2);
			halfExtension.set(
				Vector3(
					Math::clamp(axis0.computeLength() / 2.0f, 0.01f, 1000.0f),
					Math::clamp(axis1.computeLength() / 2.0f, 0.01f, 1000.0f),
					Math::clamp(axis2.computeLength() / 2.0f, 0.01f, 1000.0f)
				)
			);
			axis0.normalize();
			axis1.normalize();
			axis2.normalize();
			if (guiOnly == false) {
				emitter->setObbCenter(center);
				emitter->setObbAxis0(axis0);
				emitter->setObbAxis1(axis1);
				emitter->setObbAxis2(axis2);
				emitter->setObbHalfextension(halfExtension);
			} else {
				particleSystemScreenController->updateBoundingBoxParticleSystemEmitter(center, axis0, axis1, axis2, halfExtension);
			}
		} else
		if (emitterType == PrototypeParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER) {
			Vector3 tmpVector3;
			auto emitter = particleSystem->getCircleParticleEmitter();
			auto center = transformations.getTranslation().clone().scale(objectScaleInverted).add(emitter->getCenter());
			auto axis0 = emitter->getAxis0();
			auto axis1 = emitter->getAxis1();
			axis0 = localTransformations.getTransformationsMatrix().multiplyNoTranslation(axis0).normalize();
			axis1 = localTransformations.getTransformationsMatrix().multiplyNoTranslation(axis1).normalize();
			auto scale = 1.0f;
			if (Math::abs(totalDeltaScale.getX()) > Math::abs(totalDeltaScale.getY()) &&
				Math::abs(totalDeltaScale.getX()) > Math::abs(totalDeltaScale.getZ())) {
				scale+= totalDeltaScale.getX();
			} else
			if (Math::abs(totalDeltaScale.getY()) > Math::abs(totalDeltaScale.getX()) &&
				Math::abs(totalDeltaScale.getY()) > Math::abs(totalDeltaScale.getZ())) {
				scale+= totalDeltaScale.getY();
			} else
			if (Math::abs(totalDeltaScale.getZ()) > Math::abs(totalDeltaScale.getX()) &&
				Math::abs(totalDeltaScale.getZ()) > Math::abs(totalDeltaScale.getY())) {
				scale+= totalDeltaScale.getZ();
			}
			auto radius = Math::clamp(emitter->getRadius() * Math::abs(scale), 0.01f, 1000.0f);
			if (guiOnly == false) {
				emitter->setCenter(center);
				emitter->setAxis0(axis0);
				emitter->setAxis1(axis1);
				emitter->setRadius(radius);
			} else {
				particleSystemScreenController->updateCircleParticleSystemEmitter(center, axis0, axis1, radius);
			}
		} else
		if (emitterType == PrototypeParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER_PLANE_VELOCITY) {
			auto emitter = particleSystem->getCircleParticleEmitterPlaneVelocity();
			auto center = transformations.getTranslation().clone().scale(objectScaleInverted).add(emitter->getCenter());
			auto axis0 = emitter->getAxis0();
			auto axis1 = emitter->getAxis1();
			axis0 = localTransformations.getTransformationsMatrix().multiplyNoTranslation(axis0).normalize();
			axis1 = localTransformations.getTransformationsMatrix().multiplyNoTranslation(axis1).normalize();
			auto scale = 1.0f;
			if (Math::abs(totalDeltaScale.getX()) > Math::abs(totalDeltaScale.getY()) &&
				Math::abs(totalDeltaScale.getX()) > Math::abs(totalDeltaScale.getZ())) {
				scale+= totalDeltaScale.getX();
			} else
			if (Math::abs(totalDeltaScale.getY()) > Math::abs(totalDeltaScale.getX()) &&
				Math::abs(totalDeltaScale.getY()) > Math::abs(totalDeltaScale.getZ())) {
				scale+= totalDeltaScale.getY();
			} else
			if (Math::abs(totalDeltaScale.getZ()) > Math::abs(totalDeltaScale.getX()) &&
				Math::abs(totalDeltaScale.getZ()) > Math::abs(totalDeltaScale.getY())) {
				scale+= totalDeltaScale.getZ();
			}
			auto radius = Math::clamp(emitter->getRadius() * Math::abs(scale), 0.01f, 1000.0f);
			if (guiOnly == false) {
				emitter->setCenter(center);
				emitter->setAxis0(axis0);
				emitter->setAxis1(axis1);
				emitter->setRadius(radius);
			} else {
				particleSystemScreenController->updateCirclePlaneVelocityParticleSystemEmitter(center, axis0, axis1, radius);
			}
		} else
		if (emitterType == PrototypeParticleSystem_Emitter::SPHERE_PARTICLE_EMITTER) {
			auto emitter = particleSystem->getSphereParticleEmitter();
			auto center = transformations.getTranslation().clone().scale(objectScaleInverted).add(emitter->getCenter());
			auto scale = 1.0f;
			if (Math::abs(totalDeltaScale.getX()) > Math::abs(totalDeltaScale.getY()) &&
				Math::abs(totalDeltaScale.getX()) > Math::abs(totalDeltaScale.getZ())) {
				scale+= totalDeltaScale.getX();
			} else
			if (Math::abs(totalDeltaScale.getY()) > Math::abs(totalDeltaScale.getX()) &&
				Math::abs(totalDeltaScale.getY()) > Math::abs(totalDeltaScale.getZ())) {
				scale+= totalDeltaScale.getY();
			} else
			if (Math::abs(totalDeltaScale.getZ()) > Math::abs(totalDeltaScale.getX()) &&
				Math::abs(totalDeltaScale.getZ()) > Math::abs(totalDeltaScale.getY())) {
				scale+= totalDeltaScale.getZ();
			}
			auto radius = Math::clamp(emitter->getRadius() * Math::abs(scale), 0.01f, 1000.0f);
			if (guiOnly == false) {
				emitter->setCenter(center);
				emitter->setRadius(radius);
			} else {
				particleSystemScreenController->updateSphereParticleSystemEmitter(center, radius);
			}
		} else {
			Console::println(
				string(
					"SharedParticleSystemView::applyParticleSystemTransformations(): unknown particle system emitter '" +
					particleSystem->getEmitter()->getName() +
					"'"
				)
			);
		}
	}
	if (guiOnly == false) {
		particleSystemScreenController->setParticleSystemEmitter();
		auto modelEntity = engine->getEntity("model");
		if (modelEntity != nullptr) engine->removeEntity("model");
		Transformations transformations;
		transformations.setScale(objectScale);
		transformations.update();
		modelEntity = SceneConnector::createEntity(prototype, "model", transformations);
		if (modelEntity != nullptr) {
			modelEntity->setPickable(true);
			engine->addEntity(modelEntity);
		}
	}
}
