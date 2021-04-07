#include <tdme/tools/sceneeditor/views/SceneEditorView.h>

#include <algorithm>
#include <string>
#include <vector>

#include <tdme/engine/fileio/models/ModelReader.h>
#include <tdme/engine/fileio/scenes/SceneReader.h>
#include <tdme/engine/fileio/scenes/SceneWriter.h>
#include <tdme/engine/fileio/ProgressCallback.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Face.h>
#include <tdme/engine/model/FacesEntity.h>
#include <tdme/engine/model/Material.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/model/Node.h>
#include <tdme/engine/model/RotationOrder.h>
#include <tdme/engine/model/SpecularMaterialProperties.h>
#include <tdme/engine/model/TextureCoordinate.h>
#include <tdme/engine/model/UpVector.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/primitives/LineSegment.h>
#include <tdme/engine/prototype/Prototype.h>
#include <tdme/engine/prototype/Prototype_Type.h>
#include <tdme/engine/prototype/PrototypeProperty.h>
#include <tdme/engine/scene/Scene.h>
#include <tdme/engine/scene/SceneEntity.h>
#include <tdme/engine/scene/SceneLibrary.h>
#include <tdme/engine/scene/SceneLight.h>
#include <tdme/engine/scene/ScenePropertyPresets.h>
#include <tdme/engine/Camera.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Entity.h>
#include <tdme/engine/EntityPickingFilter.h>
#include <tdme/engine/Light.h>
#include <tdme/engine/Object3D.h>
#include <tdme/engine/PartitionOctTree.h>
#include <tdme/engine/Rotation.h>
#include <tdme/engine/SceneConnector.h>
#include <tdme/engine/SceneConnector.h>
#include <tdme/engine/Timing.h>
#include <tdme/engine/Transformations.h>
#include <tdme/gui/events/GUIKeyboardEvent.h>
#include <tdme/gui/events/GUIMouseEvent.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/gui/GUI.h>
#include <tdme/math/Math.h>
#include <tdme/math/Quaternion.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Vector4.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/tools/sceneeditor/controller/SceneEditorLibraryScreenController.h>
#include <tdme/tools/sceneeditor/controller/SceneEditorScreenController.h>
#include <tdme/tools/sceneeditor/views/SceneEditorView_EntityColor.h>
#include <tdme/tools/sceneeditor/TDMESceneEditor.h>
#include <tdme/tools/shared/controller/FileDialogPath.h>
#include <tdme/tools/shared/controller/FileDialogScreenController.h>
#include <tdme/tools/shared/controller/InfoDialogScreenController.h>
#include <tdme/tools/shared/controller/ProgressBarScreenController.h>
#include <tdme/tools/shared/tools/Tools.h>
#include <tdme/tools/shared/views/CameraInputHandler.h>
#include <tdme/tools/shared/views/CameraInputHandlerEventHandler.h>
#include <tdme/tools/shared/views/Gizmo.h>
#include <tdme/tools/shared/views/PopUps.h>
#include <tdme/utilities/Character.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>
#include <tdme/utilities/Float.h>
#include <tdme/utilities/ModelTools.h>
#include <tdme/utilities/Properties.h>
#include <tdme/utilities/StringTools.h>

using std::find;
using std::remove;
using std::string;
using std::to_string;
using std::vector;

using tdme::tools::sceneeditor::views::SceneEditorView;

using tdme::engine::fileio::models::ModelReader;
using tdme::engine::fileio::scenes::SceneReader;
using tdme::engine::fileio::scenes::SceneWriter;
using tdme::engine::fileio::ProgressCallback;
using tdme::engine::model::Color4;
using tdme::engine::model::Face;
using tdme::engine::model::FacesEntity;
using tdme::engine::model::Material;
using tdme::engine::model::Model;
using tdme::engine::model::Node;
using tdme::engine::model::RotationOrder;
using tdme::engine::model::SpecularMaterialProperties;
using tdme::engine::model::TextureCoordinate;
using tdme::engine::model::UpVector;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::primitives::LineSegment;
using tdme::engine::prototype::Prototype;
using tdme::engine::prototype::Prototype_Type;
using tdme::engine::prototype::PrototypeProperty;
using tdme::engine::scene::Scene;
using tdme::engine::scene::SceneEntity;
using tdme::engine::scene::SceneLibrary;
using tdme::engine::scene::SceneLight;
using tdme::engine::scene::ScenePropertyPresets;
using tdme::engine::Camera;
using tdme::engine::Engine;
using tdme::engine::Entity;
using tdme::engine::EntityPickingFilter;
using tdme::engine::Light;
using tdme::engine::Object3D;
using tdme::engine::PartitionOctTree;
using tdme::engine::Rotation;
using tdme::engine::SceneConnector;
using tdme::engine::SceneConnector;
using tdme::engine::Timing;
using tdme::engine::Transformations;
using tdme::gui::events::GUIKeyboardEvent;
using tdme::gui::events::GUIMouseEvent;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::GUI;
using tdme::math::Math;
using tdme::math::Quaternion;
using tdme::math::Vector3;
using tdme::math::Vector4;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;
using tdme::tools::sceneeditor::controller::SceneEditorLibraryScreenController;
using tdme::tools::sceneeditor::controller::SceneEditorScreenController;
using tdme::tools::sceneeditor::views::SceneEditorView_EntityColor;
using tdme::tools::sceneeditor::TDMESceneEditor;
using tdme::tools::shared::controller::FileDialogPath;
using tdme::tools::shared::controller::FileDialogScreenController;
using tdme::tools::shared::controller::InfoDialogScreenController;
using tdme::tools::shared::controller::ProgressBarScreenController;
using tdme::tools::shared::tools::Tools;
using tdme::tools::shared::views::Gizmo;
using tdme::tools::shared::views::PopUps;
using tdme::utilities::Character;
using tdme::utilities::Console;
using tdme::utilities::Exception;
using tdme::utilities::Float;
using tdme::utilities::ModelTools;
using tdme::utilities::Properties;
using tdme::utilities::StringTools;

vector<string> SceneEditorView::ENTITYCOLOR_NAMES = {
	"blue",
	"yellow",
	"magenta",
	"cyan",
	"none"
};

SceneEditorView::SceneEditorView(PopUps* popUps): Gizmo(Engine::getInstance(), "le")
{
	this->popUps = popUps;
	snappingX = 1.0f;
	snappingZ = 1.0f;
	snappingEnabled = false;
	cameraInputHandler = new CameraInputHandler(engine, this);
	mouseDownLastX = SceneEditorView::MOUSE_DOWN_LAST_POSITION_NONE;
	mouseDownLastY = SceneEditorView::MOUSE_DOWN_LAST_POSITION_NONE;
	reloadEntityLibrary = false;
	selectedPrototype = nullptr;
	keyControl = false;
	keyEscape = false;
	placeEntityMode = false;
	placeEntityValid = false;
	placeEntityYRotation = 0;
	pasteMode = false;
	pasteModeValid = false;
	mouseDownLastX = MOUSE_DOWN_LAST_POSITION_NONE;
	mouseDownLastY = MOUSE_DOWN_LAST_POSITION_NONE;
	mouseDragging = false;
	mouseDraggingLastEntity = nullptr;
	gridEnabled = false;
	gridY = 0.0f;
	entityColors["red"] = new SceneEditorView_EntityColor(this, 1.5f, 0.8f, 0.8f, 0.5f, 0.0f, 0.0f);
	entityColors["green"] = new SceneEditorView_EntityColor(this, 0.8f, 1.5f, 0.8f, 0.0f, 0.5f, 0.0f);
	entityColors["blue"] = new SceneEditorView_EntityColor(this, 0.8f, 0.8f, 1.5f, 0.0f, 0.0f, 0.5f);
	entityColors["yellow"] = new SceneEditorView_EntityColor(this, 1.5f, 1.5f, 0.8f, 0.5f, 0.5f, 0.0f);
	entityColors["magenta"] = new SceneEditorView_EntityColor(this, 1.5f, 0.8f, 1.5f, 0.5f, 0.0f, 0.5f);
	entityColors["cyan"] = new SceneEditorView_EntityColor(this, 0.8f, 1.5f, 1.5f, 0.0f, 0.5f, 0.5f);
	entityColors["none"] = new SceneEditorView_EntityColor(this, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	levelEditorGround = createSceneEditorGroundPlateModel();
	engine = Engine::getInstance();

	{
		// entity picking filter for no grid
		class PrototypePickingFilterNoGrid: public virtual EntityPickingFilter
		{
		public:
			bool filterEntity(Entity* entity) override {
				return entity->getId() != "tdme.sceneeditor.grid";
			}

			/**
			 * Public constructor
			 * @param sceneEditorView scene editor view
			 */
			PrototypePickingFilterNoGrid(SceneEditorView* sceneEditorView): sceneEditorView(sceneEditorView) {
			}

		private:
			SceneEditorView* sceneEditorView;
		};
		entityPickingFilterNoGrid = new PrototypePickingFilterNoGrid(this);
	}

	{
		// entity picking filter for no placing object
		class PrototypePickingFilterPlacing: public virtual EntityPickingFilter
		{
		public:
			bool filterEntity(Entity* entity) override {
				return
						entity->getId() != "tdme.sceneeditor.placeentity" &&
						StringTools::startsWith(entity->getId(), "tdme.sceneeditor.paste.") == false &&
						StringTools::startsWith(entity->getId(), "le.tdme.gizmo.") == false;
			}

			/**
			 * Public constructor
			 * @param sceneEditorView scene editor view
			 */
			PrototypePickingFilterPlacing(SceneEditorView* sceneEditorView): sceneEditorView(sceneEditorView) {
			}

		private:
			SceneEditorView* sceneEditorView;
		};
		entityPickingFilterPlacing = new PrototypePickingFilterPlacing(this);
	}
	this->cameraInputHandler = new CameraInputHandler(engine, this);
}

SceneEditorView::~SceneEditorView() {
	delete entityColors["red"];
	delete entityColors["green"];
	delete entityColors["blue"];
	delete entityColors["yellow"];
	delete entityColors["magenta"];
	delete entityColors["cyan"];
	delete entityColors["none"];
	delete levelEditorGround;
	delete entityPickingFilterNoGrid;
	delete entityPickingFilterPlacing;
	delete sceneEditorScreenController;
}

PopUps* SceneEditorView::getPopUps()
{
	return popUps;
}

const string SceneEditorView::getFileName()
{
	return FileSystem::getInstance()->getFileName(scene.getFileName());
}

Scene* SceneEditorView::getScene()
{
	return &scene;
}

Prototype* SceneEditorView::getSelectedPrototype()
{
	return selectedPrototype;
}

SceneEntity* SceneEditorView::getSelectedSceneEntity()
{
	if (selectedEntityIds.size() != 1) return nullptr;

	auto selectedEntity = scene.getEntity(selectedEntityIds[0]);
	return selectedEntity != nullptr && StringTools::startsWith(selectedEntity->getId(), "tdme.sceneeditor.") == false ? scene.getEntity(selectedEntity->getId()) : static_cast< SceneEntity* >(nullptr);
}

bool SceneEditorView::isGridEnabled()
{
	return gridEnabled;
}

void SceneEditorView::setGridEnabled(bool gridEnabled)
{
	this->gridEnabled = gridEnabled;
	if (gridEnabled) {
		updateGrid();
	} else {
		removeGrid();
	}
}

float SceneEditorView::getGridY()
{
	return gridY;
}

void SceneEditorView::setGridY(float gridY)
{
	if (gridEnabled)
		removeGrid();

	this->gridY = gridY;
	if (gridEnabled)
		updateGrid();

}

void SceneEditorView::getSnapping(bool& snappingEnabled, float& snappingX, float& snappingZ) {
	snappingEnabled = this->snappingEnabled;
	snappingX = this->snappingX;
	snappingZ = this->snappingZ;
}

void SceneEditorView::setSnapping(bool snappingEnabled, float snappingX, float snappingZ) {
	this->snappingEnabled = snappingEnabled;
	this->snappingX = snappingX;
	this->snappingZ = snappingZ;
}

void SceneEditorView::selectPrototypeFromLibrary(int id)
{
	selectedPrototype = TDMESceneEditor::getInstance()->getSceneLibrary()->getPrototype(id);
}

void SceneEditorView::handleInputEvents()
{
	#if !defined(GLFW3) && !defined(VUKAN)
		keyControl = false;
		keyShift = false;
	#endif
	auto keyControlX = false;
	auto keyControlC = false;
	auto keyControlV = false;
	auto keyDelete = false;
	for (auto i = 0; i < engine->getGUI()->getKeyboardEvents().size(); i++) {
		auto& event = engine->getGUI()->getKeyboardEvents()[i];
		if (event.isProcessed() == true) continue;
		if (event.getType() == GUIKeyboardEvent::KEYBOARDEVENT_KEY_TYPED) continue;
		auto isKeyDown = event.getType() == GUIKeyboardEvent::KEYBOARDEVENT_KEY_PRESSED;
		#if defined(GLFW3) || defined(VUKAN)
			if (event.getKeyCode() == KEYBOARD_KEYCODE_LEFT_SHIFT) {
				keyShift = isKeyDown;
				event.setProcessed(true);
			}
			if (event.getKeyCode() == KEYBOARD_KEYCODE_LEFT_CTRL) {
				keyControl = isKeyDown;
				event.setProcessed(true);
			}
		#else
			keyControl = event.isControlDown();
			keyShift = event.isShiftDown();
		#endif
		if (event.getKeyCode() == GUIKeyboardEvent::KEYCODE_ESCAPE) {
			keyEscape = isKeyDown;
			event.setProcessed(true);
		}
		if (event.getKeyCode() == GUIKeyboardEvent::KEYCODE_BACKSPACE) {
			keyDelete = isKeyDown;
			event.setProcessed(true);
		}
		if (Character::toLowerCase(event.getKeyChar()) == 24) {
			keyControlX = isKeyDown;
			event.setProcessed(true);
		}
		if (Character::toLowerCase(event.getKeyChar()) == 3) {
			keyControlC = isKeyDown;
			event.setProcessed(true);
		}
		if (Character::toLowerCase(event.getKeyChar()) == 22) {
			keyControlV = isKeyDown;
			event.setProcessed(true);
		}
		if (Character::toLowerCase(event.getKeyChar()) == 'x' && keyControl == true) {
			keyControlX = isKeyDown;
			event.setProcessed(true);
		}
		if (Character::toLowerCase(event.getKeyChar()) == 'c' && keyControl == true) {
			keyControlC = isKeyDown;
			event.setProcessed(true);
		}
		if (Character::toLowerCase(event.getKeyChar()) == 'v' && keyControl == true) {
			keyControlV = isKeyDown;
			event.setProcessed(true);
		}
		if (Character::toLowerCase(event.getKeyChar()) == '.' && !isKeyDown == false) {
			placeEntityYRotation = (placeEntityYRotation + 1) % 4;
			event.setProcessed(true);
		}
		if (Character::toLowerCase(event.getKeyChar()) == ',' && !isKeyDown == false) {
			placeEntityYRotation = (placeEntityYRotation + 3) % 4;
			event.setProcessed(true);
		}
		if (Character::toLowerCase(event.getKeyChar()) == '1' && isKeyDown == true) {
			setGizmoType(GIZMOTYPE_ALL);
			updateGizmo();
			event.setProcessed(true);
		}
		if (Character::toLowerCase(event.getKeyChar()) == '2' && isKeyDown == true) {
			setGizmoType(GIZMOTYPE_TRANSLATE);
			event.setProcessed(true);
			updateGizmo();
		}
		if (Character::toLowerCase(event.getKeyChar()) == '3' && isKeyDown == true) {
			setGizmoType(GIZMOTYPE_ROTATE);
			event.setProcessed(true);
			updateGizmo();
		}
		if (Character::toLowerCase(event.getKeyChar()) == '4' && isKeyDown == true) {
			setGizmoType(GIZMOTYPE_SCALE);
			event.setProcessed(true);
			updateGizmo();
		}
	}
	for (auto i = 0; i < engine->getGUI()->getMouseEvents().size(); i++) {
		auto& event = engine->getGUI()->getMouseEvents()[i];

		if ((event.getType() == GUIMouseEvent::MOUSEEVENT_MOVED ||
			event.getType() == GUIMouseEvent::MOUSEEVENT_DRAGGED) &&
			event.getXUnscaled() >= 0 &&
			event.getYUnscaled() >= 0) {
			placeEntityMouseX = event.getXUnscaled();
			placeEntityMouseY = event.getYUnscaled();
		}

		if (event.isProcessed() == true) continue;

		if (event.getButton() == MOUSE_BUTTON_LEFT) {
			if (event.getType() == GUIMouseEvent::MOUSEEVENT_DRAGGED) {
				if (mouseDragging == false) {
					mouseDragging = true;
					mouseDownLastX = event.getXUnscaled();
					mouseDownLastY = event.getYUnscaled();
					mouseDraggingLastEntity = nullptr;
					event.setProcessed(true);
				}
			} else {
				if (mouseDragging == true) {
					mouseDownLastX = MOUSE_DOWN_LAST_POSITION_NONE;
					mouseDownLastY = MOUSE_DOWN_LAST_POSITION_NONE;
					mouseDragging = false;
					mouseDraggingLastEntity = nullptr;
					event.setProcessed(true);
				}
			}
		}
		if (event.getButton() == MOUSE_BUTTON_LEFT) {
			if (event.getType() == GUIMouseEvent::MOUSEEVENT_RELEASED) {
				if (pasteMode == true && pasteModeValid == true) {
					pasteEntities(false);
					if (keyShift == false) unsetPasteMode();
				} else
				if (placeEntityMode == true && placeEntityValid == true) {
					placeEntity();
					if (keyShift == false) unsetPlaceEntityMode();
				} else {
					setGizmoMode(GIZMOMODE_NONE);
				}
				event.setProcessed(true);
			} else {
				Node* selectedEntityNode = nullptr;
				Entity* selectedEntity = nullptr;
				if (getGizmoMode() == GIZMOMODE_NONE) selectedEntity = engine->getEntityByMousePosition(event.getXUnscaled(), event.getYUnscaled(), entityPickingFilterNoGrid, &selectedEntityNode);
				if (mouseDragging == true) {
					Vector3 deltaTranslation;
					Vector3 deltaRotation;
					Vector3 deltaScale;
					if (determineGizmoDeltaTransformations(mouseDownLastX, mouseDownLastY, event.getXUnscaled(), event.getYUnscaled(), deltaTranslation, deltaRotation, deltaScale) == true) {
						auto gizmoEntity = getGizmoObject3D();
						if (gizmoEntity != nullptr) {
							Transformations rotations;
							rotations.addRotation(scene.getRotationOrder()->getAxis0(), deltaRotation[scene.getRotationOrder()->getAxis0VectorIndex()]);
							rotations.addRotation(scene.getRotationOrder()->getAxis1(), deltaRotation[scene.getRotationOrder()->getAxis1VectorIndex()]);
							rotations.addRotation(scene.getRotationOrder()->getAxis2(), deltaRotation[scene.getRotationOrder()->getAxis2VectorIndex()]);
							rotations.update();
							for (auto selectedEntityId: selectedEntityIds) {
								auto _selectedEntity = engine->getEntity(selectedEntityId);
								if (_selectedEntity != nullptr && StringTools::startsWith(_selectedEntity->getId(), "tdme.sceneeditor.") == false) {
									auto sceneEntity = scene.getEntity(_selectedEntity->getId());
									if (sceneEntity == nullptr) continue;
									auto translation = sceneEntity->getTransformations().getTranslation();
									translation = gizmoEntity->getTranslation().clone().add(rotations.getRotationsQuaternion().multiply(translation.clone().sub(gizmoEntity->getTranslation())));
									sceneEntity->getTransformations().setTranslation(translation.clone().add(deltaTranslation));
									auto scale = sceneEntity->getTransformations().getScale().clone().scale(deltaScale);
									if (Math::abs(scale.getX()) < 0.01f) scale.setX(Math::sign(scale.getX()) * 0.01f);
									if (Math::abs(scale.getY()) < 0.01f) scale.setY(Math::sign(scale.getY()) * 0.01f);
									if (Math::abs(scale.getZ()) < 0.01f) scale.setZ(Math::sign(scale.getZ()) * 0.01f);
									if (Math::abs(scale.getX()) > 100.0f) scale.setX(Math::sign(scale.getX()) * 100.0f);
									if (Math::abs(scale.getY()) > 100.0f) scale.setY(Math::sign(scale.getY()) * 100.0f);
									if (Math::abs(scale.getZ()) > 100.0f) scale.setZ(Math::sign(scale.getZ()) * 100.0f);
									sceneEntity->getTransformations().setScale(scale);
									if ((sceneEntity->getPrototype()->getType()->getGizmoTypeMask() & Gizmo::GIZMOTYPE_ROTATE) == Gizmo::GIZMOTYPE_ROTATE) {
										sceneEntity->getTransformations().setRotationAngle(scene.getRotationOrder()->getAxisXIndex(), sceneEntity->getTransformations().getRotationAngle(scene.getRotationOrder()->getAxisXIndex()) + deltaRotation[0]);
										sceneEntity->getTransformations().setRotationAngle(scene.getRotationOrder()->getAxisYIndex(), sceneEntity->getTransformations().getRotationAngle(scene.getRotationOrder()->getAxisYIndex()) + deltaRotation[1]);
										sceneEntity->getTransformations().setRotationAngle(scene.getRotationOrder()->getAxisZIndex(), sceneEntity->getTransformations().getRotationAngle(scene.getRotationOrder()->getAxisZIndex()) + deltaRotation[2]);
									}
									sceneEntity->getTransformations().update();
									_selectedEntity->fromTransformations(sceneEntity->getTransformations());
								}
							}
							if (selectedEntityIds.size() == 1) {
								auto _selectedEntity = engine->getEntity(selectedEntityIds[0]);
								auto sceneEntity = scene.getEntity(_selectedEntity->getId());
								sceneEditorScreenController->setEntityTransformations(
									_selectedEntity->getTranslation(),
									_selectedEntity->getScale(),
									_selectedEntity->getRotationAngle(scene.getRotationOrder()->getAxisXIndex()),
									_selectedEntity->getRotationAngle(scene.getRotationOrder()->getAxisYIndex()),
									_selectedEntity->getRotationAngle(scene.getRotationOrder()->getAxisZIndex()),
									(sceneEntity->getPrototype()->getType()->getGizmoTypeMask() & Gizmo::GIZMOTYPE_ROTATE) == 0
								);
								setGizmoRotation(_selectedEntity->getTransformations());
							}
						}
						if (Math::abs(deltaTranslation.getX()) > Math::EPSILON ||
							Math::abs(deltaTranslation.getY()) > Math::EPSILON ||
							Math::abs(deltaTranslation.getZ()) > Math::EPSILON) {
							updateGizmo();
						}
						updateGUITransformationsElements();
					}
				} else
				if (determineGizmoMode(selectedEntity, selectedEntityNode) == true) {
					// no op
					if (selectedEntityIds.size() == 1) {
						for (auto selectedEntityId: selectedEntityIds) {
							auto _selectedEntity = engine->getEntity(selectedEntityId);
							if (_selectedEntity != nullptr && StringTools::startsWith(_selectedEntity->getId(), "tdme.sceneeditor.") == false) {
								setGizmoRotation(_selectedEntity->getTransformations());
							}
						}
					} else
					if (selectedEntityIds.size() > 1) {
						setGizmoRotation(Transformations());
					}
					updateGUITransformationsElements();
				} else {
					if (selectedEntity != nullptr && scene.getEntity(selectedEntity->getId()) == nullptr) selectedEntity = nullptr;
					if (keyControl == false) {
						vector<Entity*> entitiesToRemove;
						for (auto selectedEntityId: selectedEntityIds) {
							auto selectedEntity = engine->getEntity(selectedEntityId);
							if (mouseDragging == true && mouseDraggingLastEntity == selectedEntity) {
								// no op
							} else {
								if (selectedEntity != nullptr) entitiesToRemove.push_back(selectedEntity);
							}
						}
						for (auto entityToRemove: entitiesToRemove) {
							selectedEntityIds.erase(remove(selectedEntityIds.begin(), selectedEntityIds.end(), entityToRemove->getId()), selectedEntityIds.end());
							auto selectedEntitiyIdByIdIt = selectedEntityIdsById.find(entityToRemove->getId());
							if (selectedEntitiyIdByIdIt != selectedEntityIdsById.end()) {
								selectedEntityIdsById.erase(selectedEntitiyIdByIdIt);
							}
							sceneEditorScreenController->unselectEntitiesInEntityListBox(entityToRemove->getId());
							resetEntity(entityToRemove);
						}
					}
					if (selectedEntity != nullptr) {
						if (selectedEntityIdsById.find(selectedEntity->getId()) == selectedEntityIdsById.end()) {
							setStandardEntityColorEffect(selectedEntity);
							setHighlightEntityColorEffect(selectedEntity);
							selectedEntityIds.push_back(selectedEntity->getId());
							selectedEntityIdsById.insert(selectedEntity->getId());
							sceneEditorScreenController->selectEntityInEntityListbox(selectedEntity->getId());
							auto sceneEntity = scene.getEntity(selectedEntity->getId());
							if (sceneEntity != nullptr) {
								TDMESceneEditor::getInstance()->getSceneEditorLibraryScreenController()->selectPrototype(sceneEntity->getPrototype()->getId());
							}
						} else {
							resetEntity(selectedEntity);
							selectedEntityIds.erase(remove(selectedEntityIds.begin(), selectedEntityIds.end(), selectedEntity->getId()), selectedEntityIds.end());
							auto selectedEntityIdsByIdIt = selectedEntityIdsById.find(selectedEntity->getId());
							if (selectedEntityIdsByIdIt != selectedEntityIdsById.end()) {
								selectedEntityIdsById.erase(selectedEntityIdsByIdIt);
							}
							sceneEditorScreenController->unselectEntitiesInEntityListBox(selectedEntity->getId());
						}
						if (selectedEntityIds.size() == 1) {
							auto sceneEntity = scene.getEntity(selectedEntity->getId());
							if (sceneEntity != nullptr && sceneEntity->getPrototype()->getType()->hasNonEditScaleDownMode() == true) {
								selectedEntity->fromTransformations(sceneEntity->getTransformations());
							}
							setGizmoRotation(selectedEntity->getTransformations());
						}
						if (selectedEntityIds.size() > 1) setGizmoRotation(Transformations());
					}
					mouseDraggingLastEntity = selectedEntity;
					updateGizmo();
					updateGUIElements();
				}
				event.setProcessed(true);
			}
		}
		if (event.getButton() != MOUSE_BUTTON_NONE) {
			mouseDownLastX = event.getXUnscaled();
			mouseDownLastY = event.getYUnscaled();
		}
	}
	if (keyDelete == true) {
		Console::println("SceneEditorView::handleInputEvents(): Backspace");
		removeGizmo();
		removeEntities();
	}
	if (keyControlX == true) {
		Console::println("SceneEditorView::handleInputEvents(): CTRL-X");
		removeGizmo();
		copyEntities();
		removeEntities();
	}
	if (keyControlC == true) {
		Console::println("SceneEditorView::handleInputEvents(): CTRL-C");
		copyEntities();
	}
	if (keyControlV == true) {
		Console::println("SceneEditorView::handleInputEvents(): CTRL-V");
		removeGizmo();
		setPasteMode();
	}
	//
	cameraInputHandler->handleInputEvents();
}

void SceneEditorView::display()
{
	if (needGizmoUpdate == true) {
		updateGizmo();
		needGizmoUpdate = false;
	}

	if (reloadEntityLibrary == true) {
		auto sceneLibrary = TDMESceneEditor::getInstance()->getSceneLibrary();
		for (auto i = 0; i < sceneLibrary->getPrototypeCount(); i++) {
			selectedPrototype = sceneLibrary->getPrototypeAt(i);
			Tools::oseThumbnail(selectedPrototype);
		}
		reloadEntityLibrary = false;
		TDMESceneEditor::getInstance()->getSceneEditorLibraryScreenController()->setPrototypeLibrary();
	}

	if ((placeEntityMode == true || pasteMode == true) && keyEscape == true) {
		unsetPlaceEntityMode();
		unsetPasteMode();
		keyEscape = false;
	}

	{
		auto selectedEngineEntity = engine->getEntity("tdme.sceneeditor.placeentity");
		Vector3 worldCoordinate;
		placeEntityValid = false;
		pasteModeValid = false;
		if ((placeEntityMode == true || pasteMode == true) && engine->getEntityByMousePosition(placeEntityMouseX, placeEntityMouseY, worldCoordinate, entityPickingFilterPlacing) != nullptr) {
			if (placeEntityMode == true) {
				Transformations transformations;
				transformations.setTranslation(worldCoordinate);
				transformations.addRotation(scene.getRotationOrder()->getAxis0(), 0.0f);
				transformations.addRotation(scene.getRotationOrder()->getAxis1(), 0.0f);
				transformations.addRotation(scene.getRotationOrder()->getAxis2(), 0.0f);
				transformations.update();
				if (selectedEngineEntity == nullptr && selectedPrototype != nullptr) {
					selectedEngineEntity = SceneConnector::createEntity(selectedPrototype, "tdme.sceneeditor.placeentity", transformations);
					if (selectedEngineEntity != nullptr) engine->addEntity(selectedEngineEntity);
				}
				if (selectedEngineEntity != nullptr) {
					if (snappingEnabled == true && (snappingX > Math::EPSILON || snappingZ > Math::EPSILON)) {
						if (snappingX > Math::EPSILON) worldCoordinate.setX(static_cast<int>(worldCoordinate.getX() / snappingX) * snappingX);
						if (snappingZ > Math::EPSILON) worldCoordinate.setZ(static_cast<int>(worldCoordinate.getZ() / snappingZ) * snappingZ);
						Vector3 snappedWorldCoordinate;
						if (engine->doRayCasting(worldCoordinate.clone().setY(10000.0f), worldCoordinate.clone().setY(-10000.0f), snappedWorldCoordinate, entityPickingFilterPlacing) != nullptr) {
							worldCoordinate = snappedWorldCoordinate;
						}
					}
					worldCoordinate.sub(Vector3(0.0f, selectedEngineEntity->getBoundingBox()->getMin().getY(), 0.0f));
					transformations.setTranslation(worldCoordinate);
					transformations.setRotationAngle(scene.getRotationOrder()->getAxisYIndex(), static_cast<float>(placeEntityYRotation) * 90.0f);
					transformations.update();
					selectedEngineEntity->fromTransformations(transformations);
					placeEntityTranslation = transformations.getTranslation();
					placeEntityValid = true;
				}
			} else
			if (pasteMode == true) {
				if (snappingEnabled == true && (snappingX > Math::EPSILON || snappingZ > Math::EPSILON)) {
					if (snappingX > Math::EPSILON) worldCoordinate.setX(static_cast<int>(worldCoordinate.getX() / snappingX) * snappingX);
					if (snappingZ > Math::EPSILON) worldCoordinate.setZ(static_cast<int>(worldCoordinate.getZ() / snappingZ) * snappingZ);
					Vector3 snappedWorldCoordinate;
					if (engine->doRayCasting(worldCoordinate.clone().setY(10000.0f), worldCoordinate.clone().setY(-10000.0f), snappedWorldCoordinate, entityPickingFilterPlacing) != nullptr) {
						worldCoordinate = snappedWorldCoordinate;
					}
				}
				placeEntityTranslation = worldCoordinate;
				pasteModeValid = true;
				pasteEntities(true);
			}
		}
	}

	//
	updateGrid();

	// viewport
	auto xScale = (float)engine->getWidth() / (float)sceneEditorScreenController->getScreenNode()->getScreenWidth();
	auto yScale = (float)engine->getHeight() / (float)sceneEditorScreenController->getScreenNode()->getScreenHeight();
	auto viewPortLeft = 0;
	auto viewPortTop = 0;
	auto viewPortWidth = 0;
	auto viewPortHeight = 0;
	sceneEditorScreenController->getViewPort(viewPortLeft, viewPortTop, viewPortWidth, viewPortHeight);
	viewPortLeft = (int)((float)viewPortLeft * xScale);
	viewPortTop = (int)((float)viewPortTop * yScale);
	viewPortWidth = (int)((float)viewPortWidth * xScale);
	viewPortHeight = (int)((float)viewPortHeight * yScale);
	engine->getCamera()->enableViewPort(viewPortLeft, viewPortTop, viewPortWidth, viewPortHeight);

	//
	updateSkyPosition();
}

void SceneEditorView::selectEntities(const vector<string>& entityIds)
{
	removeGizmo();
	for (auto entityIdToRemove: selectedEntityIds) {
		auto entityToRemove = engine->getEntity(entityIdToRemove);
		if (entityToRemove != nullptr) setStandardEntityColorEffect(entityToRemove);
	}
	selectedEntityIds.clear();
	selectedEntityIdsById.clear();
	for (auto entityId: entityIds) {
		auto selectedEntity = engine->getEntity(entityId);
		if (selectedEntity == nullptr) continue;
		setStandardEntityColorEffect(selectedEntity);
		setHighlightEntityColorEffect(selectedEntity);
		selectedEntityIds.push_back(entityId);
		selectedEntityIdsById.insert(entityId);
	}
	updateGUIElements();
	updateGizmo();
}

void SceneEditorView::unselectEntities()
{
	removeGizmo();
	for (auto entityIdToRemove: selectedEntityIds) {
		auto entityToRemove = engine->getEntity(entityIdToRemove);
		if (entityToRemove == nullptr) continue;
		resetEntity(entityToRemove);
	}
	selectedEntityIds.clear();
	selectedEntityIdsById.clear();
	sceneEditorScreenController->unselectEntitiesInEntityListBox();
	updateGUIElements();
	removeGizmo();
}

void SceneEditorView::updateGUIElements()
{
	sceneEditorScreenController->setScreenCaption("Scene Editor - " + Tools::getFileName(scene.getFileName()));
	sceneEditorScreenController->setSceneSize(scene.getDimension().getX(), scene.getDimension().getZ(), scene.getDimension().getY());
	if (selectedEntityIds.size() == 1) {
		auto selectedEntity = engine->getEntity(selectedEntityIds[0]);
		if (selectedEntity != nullptr && StringTools::startsWith(selectedEntity->getId(), "tdme.sceneeditor.") == false) {
			auto sceneEntity = scene.getEntity(selectedEntity->getId());
			auto preset = sceneEntity->getProperty("preset");
			sceneEditorScreenController->setEntityProperties(preset != nullptr ? preset->getValue() : "", sceneEntity, "");
			sceneEditorScreenController->setEntityReflectionsEnvironmentMappings(scene, sceneEntity->getReflectionEnvironmentMappingId());
		} else {
			sceneEditorScreenController->unsetEntityData();
			sceneEditorScreenController->unsetEntityTransformations();
			sceneEditorScreenController->unsetEntityProperties();
			sceneEditorScreenController->unsetEntityReflectionsEnvironmentMappings();
		}
	} else
	if (selectedEntityIds.size() > 1) {
		sceneEditorScreenController->unsetEntityData();
		sceneEditorScreenController->unsetEntityProperties();
		sceneEditorScreenController->unsetEntityReflectionsEnvironmentMappings();
	} else
	if (selectedEntityIds.size() == 0) {
		sceneEditorScreenController->unsetEntityTransformations();
		sceneEditorScreenController->unsetEntityData();
		sceneEditorScreenController->unsetEntityProperties();
		sceneEditorScreenController->unsetEntityReflectionsEnvironmentMappings();
	}
	for (auto i = 0; i < 4; i++) {
		sceneEditorScreenController->setLight(i, scene.getLightAt(i)->getAmbient(), scene.getLightAt(i)->getDiffuse(), scene.getLightAt(i)->getSpecular(), scene.getLightAt(i)->getPosition(), scene.getLightAt(i)->getConstantAttenuation(), scene.getLightAt(i)->getLinearAttenuation(), scene.getLightAt(i)->getQuadraticAttenuation(), scene.getLightAt(i)->getSpotTo(), scene.getLightAt(i)->getSpotDirection(), scene.getLightAt(i)->getSpotExponent(), scene.getLightAt(i)->getSpotCutOff(), scene.getLightAt(i)->isEnabled());
	}
	updateGUITransformationsElements();
}

void SceneEditorView::updateGUITransformationsElements() {
	if (selectedEntityIds.size() == 1) {
		auto selectedEntity = engine->getEntity(selectedEntityIds[0]);
		if (selectedEntity != nullptr && StringTools::startsWith(selectedEntity->getId(), "tdme.sceneeditor.") == false) {
			auto sceneEntity = scene.getEntity(selectedEntity->getId());
			sceneEditorScreenController->setEntityTransformations(
				selectedEntity->getTranslation(),
				selectedEntity->getScale(),
				selectedEntity->getRotationAngle(scene.getRotationOrder()->getAxisXIndex()),
				selectedEntity->getRotationAngle(scene.getRotationOrder()->getAxisYIndex()),
				selectedEntity->getRotationAngle(scene.getRotationOrder()->getAxisZIndex()),
				(sceneEntity->getPrototype()->getType()->getGizmoTypeMask() & Gizmo::GIZMOTYPE_ROTATE) == 0
			);
			Vector3 entityCenter;
			if (sceneEntity->getPrototype()->getModel() != nullptr) {
				BoundingBox bv;
				bv.fromBoundingVolumeWithTransformations(sceneEntity->getPrototype()->getModel()->getBoundingBox(), sceneEntity->getTransformations());
				entityCenter = bv.getCenter();
			} else {
				entityCenter = sceneEntity->getTransformations().getTranslation();
			}
			sceneEditorScreenController->setEntityData(sceneEntity->getId(), sceneEntity->getDescription(), sceneEntity->getPrototype()->getName(), entityCenter);
		}
	} else
	if (selectedEntityIds.size() > 1) {
		sceneEditorScreenController->setEntityTransformations(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), 0.0f, 0.0f, 0.0f, false);
	}
}

void SceneEditorView::setEntityListBox()
{
	sceneEditorScreenController->setEntityListbox(scene);
}

void SceneEditorView::unselectLightPresets()
{
	sceneEditorScreenController->unselectLightPresets();
}

void SceneEditorView::loadSettings()
{
	try {
		Properties settings;
		settings.load("settings", "sceneeditor.properties");
		gridEnabled = settings.get("grid.enabled", "false") == "true";
		gridY = Float::parseFloat(settings.get("grid.y", "0.0"));
		snappingEnabled = settings.get("snapping.enabled", "false") == "true";
		snappingX = Float::parseFloat(settings.get("snapping.x", "1.0"));
		snappingZ = Float::parseFloat(settings.get("snapping.z", "1.0"));
		sceneEditorScreenController->getMapPath()->setPath(settings.get("map.path", "."));
		TDMESceneEditor::getInstance()->getSceneEditorLibraryScreenController()->setModelPath(settings.get("model.path", "."));
	} catch (Exception& exception) {
		Console::print(string("SceneEditorView::loadSettings(): An error occurred: "));
		Console::println(string(exception.what()));
	}
}

void SceneEditorView::initialize()
{
	try {
		sceneEditorScreenController = new SceneEditorScreenController(this);
		sceneEditorScreenController->initialize();
		sceneEditorScreenController->getScreenNode()->setInputEventHandler(this);
		engine->getGUI()->addScreen(sceneEditorScreenController->getScreenNode()->getId(), sceneEditorScreenController->getScreenNode());
	} catch (Exception& exception) {
		Console::print(string("SceneEditorView::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	loadSettings();
	sceneEditorScreenController->setGrid(gridEnabled, gridY);
	sceneEditorScreenController->setSnapping(snappingEnabled, snappingX, snappingZ);
	sceneEditorScreenController->setSceneProperties(scene, "");
	sceneEditorScreenController->setEntityPresetIds(ScenePropertyPresets::getInstance()->getEntityPropertiesPresets());
	sceneEditorScreenController->setLightPresetsIds(ScenePropertyPresets::getInstance()->getLightPresets());
	updateGUIElements();
	for (auto i = 1; i < engine->getLightCount(); i++) engine->getLightAt(i)->setEnabled(false);
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
	auto cam = engine->getCamera();
	cam->setZNear(0.1f);
	cam->setZFar(150.0f);
	cam->setLookAt(scene.getCenter());
	gridCenter.set(cam->getLookAt());
	camLookAt.set(engine->getCamera()->getLookAt());
}

void SceneEditorView::activate()
{
	engine->reset();
	engine->setPartition(new PartitionOctTree());
	engine->setShadowMapLightEyeDistanceScale(1.0f);
	engine->getGUI()->resetRenderScreens();
	engine->getGUI()->addRenderScreen(sceneEditorScreenController->getScreenNode()->getId());
	engine->getGUI()->addRenderScreen(TDMESceneEditor::getInstance()->getSceneEditorLibraryScreenController()->getScreenNode()->getId());
	engine->getGUI()->addRenderScreen(popUps->getFileDialogScreenController()->getScreenNode()->getId());
	engine->getGUI()->addRenderScreen(popUps->getInfoDialogScreenController()->getScreenNode()->getId());
	engine->getGUI()->addRenderScreen(popUps->getProgressBarScreenController()->getScreenNode()->getId());
	TDMESceneEditor::getInstance()->getSceneEditorLibraryScreenController()->setPrototypeLibrary();
	for (auto i = 1; i < engine->getLightCount(); i++) engine->getLightAt(i)->setEnabled(false);
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
	auto cam = engine->getCamera();
	cam->setZNear(0.1f);
	cam->setZFar(150.0f);
	loadScene();
}

void SceneEditorView::deactivate()
{
	SceneConnector::resetEngine(engine, scene);
	camLookAt.set(engine->getCamera()->getLookAt());
}

void SceneEditorView::storeSettings()
{
	try {
		Properties settings;
		settings.put("grid.enabled", gridEnabled == true ? "true" : "false");
		settings.put("grid.y", to_string(gridY));
		settings.put("snapping.enabled", snappingEnabled == true ? "true" : "false");
		settings.put("snapping.x", to_string(snappingX));
		settings.put("snapping.z", to_string(snappingZ));
		settings.put("map.path", sceneEditorScreenController->getMapPath()->getPath());
		settings.put("model.path", TDMESceneEditor::getInstance()->getSceneEditorLibraryScreenController()->getModelPath());
		settings.store("settings", "sceneeditor.properties");
	} catch (Exception& exception) {
		Console::print(string("SceneEditorView::storeSettings(): An error occurred: "));
		Console::println(string(exception.what()));
	}
}

void SceneEditorView::dispose()
{
	SceneConnector::resetEngine(engine, scene);
	storeSettings();
}

void SceneEditorView::setHighlightEntityColorEffect(Entity* entity)
{
	auto red = entityColors["red"];
	entity->setEffectColorAdd(Color4(red->colorAddR, red->colorAddG, red->colorAddB, 0.0f));
	entity->setEffectColorMul(Color4(red->colorMulR, red->colorMulG, red->colorMulB, 1.0f));
}

void SceneEditorView::setStandardEntityColorEffect(Entity* entity)
{
	auto color = entityColors["none"];
	entity->setEffectColorAdd(Color4(color->colorAddR, color->colorAddG, color->colorAddB, 0.0f));
	entity->setEffectColorMul(Color4(color->colorMulR, color->colorMulG, color->colorMulB, 1.0f));
	auto sceneEntity = scene.getEntity(entity->getId());
	if (sceneEntity == nullptr) return;
	auto colorProperty = sceneEntity->getProperty("object.color");
	if (colorProperty == nullptr) colorProperty = sceneEntity->getPrototype()->getProperty("object.color");
	if (colorProperty != nullptr) {
		auto entityColorIt = entityColors.find(colorProperty->getValue());
		auto entityColor = entityColorIt != entityColors.end() ? entityColorIt->second : nullptr;
		if (entityColor != nullptr) {
			entity->setEffectColorAdd(Color4(entity->getEffectColorAdd().getRed() + entityColor->colorAddR, entity->getEffectColorAdd().getGreen() + entityColor->colorAddG, entity->getEffectColorAdd().getBlue() + entityColor->colorAddB, 0.0f));
			entity->setEffectColorMul(Color4(entity->getEffectColorMul().getRed() * entityColor->colorMulR, entity->getEffectColorMul().getGreen() * entityColor->colorMulG, entity->getEffectColorMul().getBlue() * entityColor->colorMulB, 1.0f));
		}
	}
}

void SceneEditorView::resetEntity(Entity* entity) {
	if (entity == nullptr) return;
	setStandardEntityColorEffect(entity);
	auto sceneEntity = scene.getEntity(entity->getId());
	if (sceneEntity == nullptr) return;
	if (sceneEntity->getPrototype()->getType()->hasNonEditScaleDownMode() == false) return;
	entity->fromTransformations(sceneEntity->getTransformations());
	entity->setScale(
		sceneEntity->getPrototype()->getType()->getNonEditScaleDownModeDimension().
		clone().
		scale(
			Vector3(
				1.0f / (sceneEntity->getTransformations().getScale().getX() * entity->getBoundingBox()->getDimensions().getX()),
				1.0f / (sceneEntity->getTransformations().getScale().getY() * entity->getBoundingBox()->getDimensions().getY()),
				1.0f / (sceneEntity->getTransformations().getScale().getZ() * entity->getBoundingBox()->getDimensions().getZ())
			)
		)
	);
	entity->update();
}

void SceneEditorView::loadScene()
{
	SceneConnector::resetEngine(engine, scene); // TODO: check me!
	removeGizmo();
	removeGrid();
	selectedEntityIds.clear();
	selectedEntityIdsById.clear();
	SceneConnector::setLights(engine, scene, Vector3());
	SceneConnector::addScene(engine, scene, true, true, true, true);
	setEntityListBox();
	unselectLightPresets();
	updateGrid();
	updateSky();
	// center scene
	cameraInputHandler->setSceneCenter(Vector3(scene.getCenter().getX(), scene.getBoundingBox()->getMax().getY() + 3.0f, scene.getCenter().getZ()));
	engine->getCamera()->setLookAt(scene.getCenter());
	cameraInputHandler->reset();
	gridCenter.set(engine->getCamera()->getLookAt());
}

void SceneEditorView::updateGrid()
{
	if (gridEnabled == false) return;

	string entityId = "tdme.sceneeditor.grid";
	auto entity = engine->getEntity(entityId);
	if (entity == nullptr) {
		entity = new Object3D(entityId, levelEditorGround);
		entity->setFrustumCulling(false);
		entity->addRotation(scene.getRotationOrder()->getAxis0(), 0.0f);
		entity->addRotation(scene.getRotationOrder()->getAxis1(), 0.0f);
		entity->addRotation(scene.getRotationOrder()->getAxis2(), 0.0f);
		entity->setTranslation(
			Vector3(
				-5000.0f,
				gridY - 0.05f,
				-5000.0f
			)
		);
		entity->setEnabled(true);
		entity->setPickable(true);
		entity->update();
		auto selectedEntityIdsByIdIt = selectedEntityIdsById.find(entity->getId());
		if (selectedEntityIdsByIdIt != selectedEntityIdsById.end()) {
			setHighlightEntityColorEffect(entity);
		} else {
			setStandardEntityColorEffect(entity);
		}
		engine->addEntity(entity);
	}
}

void SceneEditorView::removeGrid()
{
	engine->removeEntity("tdme.sceneeditor.grid");
}

Model* SceneEditorView::createSceneEditorGroundPlateModel()
{
	auto groundPlate = new Model("tdme.sceneeditor.grid", "tdme.sceneeditor.grid", UpVector::Y_UP, RotationOrder::XYZ, new BoundingBox(Vector3(0.0f, -0.01f, 0.0f), Vector3(10000.0f, +0.01f, 10000.0f)));
	auto groundPlateMaterial = new Material("ground");
	groundPlateMaterial->setSpecularMaterialProperties(new SpecularMaterialProperties());
	groundPlateMaterial->getSpecularMaterialProperties()->setDiffuseColor(
		Color4(
			groundPlateMaterial->getSpecularMaterialProperties()->getDiffuseColor().getRed(),
			groundPlateMaterial->getSpecularMaterialProperties()->getDiffuseColor().getGreen(),
			groundPlateMaterial->getSpecularMaterialProperties()->getDiffuseColor().getBlue(),
			0.75f
		)
	);
	groundPlateMaterial->getSpecularMaterialProperties()->setDiffuseTexture("resources/engine/textures", "groundplate.png");
	groundPlateMaterial->getSpecularMaterialProperties()->setSpecularColor(Color4(0.0f, 0.0f, 0.0f, 1.0f));
	groundPlate->getMaterials()["ground"] = groundPlateMaterial;
	auto groundNode = new Node(groundPlate, nullptr, "ground", "ground");
	vector<Vector3> groundVertices;
	groundVertices.push_back(Vector3(0.0f, 0.0f, 0.0f));
	groundVertices.push_back(Vector3(0.0f, 0.0f, 10000.0f));
	groundVertices.push_back(Vector3(10000.0f, 0.0f, 10000.0f));
	groundVertices.push_back(Vector3(10000.0f, 0.0f, 0.0f));
	vector<Vector3> groundNormals;
	groundNormals.push_back(Vector3(0.0f, 1.0f, 0.0f));
	vector<TextureCoordinate> groundTextureCoordinates;
	groundTextureCoordinates.push_back(TextureCoordinate(0.0f, 10000.0f));
	groundTextureCoordinates.push_back(TextureCoordinate(0.0f, 0.0f));
	groundTextureCoordinates.push_back(TextureCoordinate(10000.0f, 0.0f));
	groundTextureCoordinates.push_back(TextureCoordinate(10000.0f, 10000.0f));
	vector<Face> groundFacesGround;
	groundFacesGround.push_back(Face(groundNode, 0, 1, 2, 0, 0, 0, 0, 1, 2));
	groundFacesGround.push_back(Face(groundNode, 2, 3, 0, 0, 0, 0, 2, 3, 0));
	FacesEntity nodeFacesEntityGround(groundNode, "tdme.sceneeditor.grid.facesentity");
	nodeFacesEntityGround.setMaterial(groundPlateMaterial);
	nodeFacesEntityGround.setFaces(groundFacesGround);
	vector<FacesEntity> nodeFacesEntities;
	nodeFacesEntities.push_back(nodeFacesEntityGround);
	groundNode->setVertices(groundVertices);
	groundNode->setNormals(groundNormals);
	groundNode->setTextureCoordinates(groundTextureCoordinates);
	groundNode->setFacesEntities(nodeFacesEntities);
	groundPlate->getNodes()[groundNode->getId()] = groundNode;
	groundPlate->getSubNodes()[groundNode->getId()] = groundNode;
	ModelTools::prepareForIndexedRendering(groundPlate);
	return groundPlate;
}

bool SceneEditorView::entityDataApply(const string& name, const string& description)
{
	if (selectedEntityIds.size() != 1) return false;

	auto selectedEntity = engine->getEntity(selectedEntityIds[0]);
	if (selectedEntity == nullptr || StringTools::startsWith(selectedEntity->getId(), "tdme.sceneeditor.")) return false;

	auto sceneEntity = scene.getEntity(selectedEntity->getId());
	if (sceneEntity == nullptr) return false;

	sceneEntity->setDescription(description);
	if (sceneEntity->getId() != name) {
		if (engine->getEntity(name) != nullptr) return false;
		auto oldId = sceneEntity->getId();
		scene.removeEntity(sceneEntity->getId());
		engine->removeEntity(sceneEntity->getId());
		selectedEntityIds.clear();
		selectedEntityIdsById.clear();
		sceneEntity->setId(name);
		scene.addEntity(sceneEntity);
		auto entity = SceneConnector::createEntity(sceneEntity);
		if (entity != nullptr) {
			setHighlightEntityColorEffect(entity);
			selectedEntityIds.push_back(entity->getId());
			selectedEntityIdsById.insert(entity->getId());
			sceneEditorScreenController->setEntityListbox(scene);
			entity->setPickable(true);
			engine->addEntity(entity);
		}
	}
	sceneEntity->setDescription(description);
	return true;
}

void SceneEditorView::setPlaceEntityMode() {
	placeEntityMode = true;
	placeEntityValid = false;
}

void SceneEditorView::unsetPlaceEntityMode() {
	placeEntityMode = false;
	placeEntityValid = false;
	engine->removeEntity("tdme.sceneeditor.placeentity");
}

void SceneEditorView::placeEntity()
{
	Transformations sceneEntityTransformations;
	sceneEntityTransformations.setTranslation(placeEntityTranslation);
	sceneEntityTransformations.setScale(Vector3(1.0f, 1.0f, 1.0f));
	sceneEntityTransformations.setPivot(selectedPrototype->getPivot());
	sceneEntityTransformations.addRotation(scene.getRotationOrder()->getAxis0(), 0.0f);
	sceneEntityTransformations.addRotation(scene.getRotationOrder()->getAxis1(), 0.0f);
	sceneEntityTransformations.addRotation(scene.getRotationOrder()->getAxis2(), 0.0f);
	sceneEntityTransformations.setRotationAngle(scene.getRotationOrder()->getAxisYIndex(), placeEntityYRotation * 90.0f);
	sceneEntityTransformations.update();
	for (auto i = 0; i < scene.getEntityCount(); i++) {
		auto sceneEntity = scene.getEntityAt(i);
		if (sceneEntity->getPrototype() == selectedPrototype && sceneEntity->getTransformations().getTranslation().equals(sceneEntityTransformations.getTranslation())) {
			return;
		}
	}
	auto sceneEntity = new SceneEntity(
		selectedPrototype->getName() + "_" + to_string(scene.allocateEntityId()),
		"",
		sceneEntityTransformations,
		selectedPrototype
	);
	scene.addEntity(sceneEntity);
	auto entity = SceneConnector::createEntity(sceneEntity);
	if (entity != nullptr) {
		resetEntity(entity);
		entity->setPickable(true);
		engine->addEntity(entity);
	}
	sceneEditorScreenController->setEntityListbox(scene);
	scene.update();
	cameraInputHandler->setSceneCenter(Vector3(scene.getCenter().getX(), scene.getBoundingBox()->getMax().getY() + 3.0f, scene.getCenter().getZ()));
	updateGUIElements();
}

void SceneEditorView::removeEntities()
{
	removeGizmo();
	vector<Entity*> entitiesToRemove;
	for (auto selectedEntityId: selectedEntityIds) {
		Entity* selectedEntity = engine->getEntity(selectedEntityId);
		if (selectedEntity != nullptr && StringTools::startsWith(selectedEntity->getId(), "tdme.sceneeditor.") == false) {
			scene.removeEntity(selectedEntity->getId());
			engine->removeEntity(selectedEntity->getId());
			entitiesToRemove.push_back(selectedEntity);
		}
	}
	for (auto entityToRemove: entitiesToRemove) {
		selectedEntityIds.erase(remove(selectedEntityIds.begin(), selectedEntityIds.end(), entityToRemove->getId()), selectedEntityIds.end());
		auto selectedEntityIdsByIdIt = selectedEntityIdsById.find(entityToRemove->getId());
		if (selectedEntityIdsByIdIt != selectedEntityIdsById.end()) {
			selectedEntityIdsById.erase(selectedEntityIdsByIdIt);
		}
	}
	scene.update();
	cameraInputHandler->setSceneCenter(Vector3(scene.getCenter().getX(), scene.getBoundingBox()->getMax().getY() + 3.0f, scene.getCenter().getZ()));
	sceneEditorScreenController->setEntityListbox(scene);
	updateGUIElements();
}

void SceneEditorView::colorEntities()
{
	if (selectedEntityIds.size() == 0)
		return;

	for (auto selectedEntityId: selectedEntityIds) {
		auto selectedEntity = engine->getEntity(selectedEntityId);
		auto sceneEntity = scene.getEntity(selectedEntityId);
		if (sceneEntity == nullptr) continue;
		auto color = ENTITYCOLOR_NAMES[0];
		auto colorProperty = sceneEntity->getProperty("object.color");
		if (colorProperty == nullptr) {
			sceneEntity->addProperty("object.color", color);
		} else {
			color = colorProperty->getValue();
			for (auto i = 0; i < ENTITYCOLOR_NAMES.size(); i++) {
				if (StringTools::equalsIgnoreCase(color, ENTITYCOLOR_NAMES[i]) == true) {
					color = ENTITYCOLOR_NAMES[(i + 1) % ENTITYCOLOR_NAMES.size()];
					break;
				}
			}
			if (color == "none") {
				sceneEntity->removeProperty("object.color");
			} else {
				sceneEntity->updateProperty(colorProperty->getName(), "object.color", color);
			}
		}
		setStandardEntityColorEffect(selectedEntity);
		setHighlightEntityColorEffect(selectedEntity);
	}

	if (selectedEntityIds.size() == 1) {
		auto selectedEntity = engine->getEntity(selectedEntityIds[0]);
		if (selectedEntity != nullptr && StringTools::startsWith(selectedEntity->getId(), "tdme.sceneeditor.") == false) {
			auto sceneEntity = scene.getEntity(selectedEntity->getId());
			auto preset = sceneEntity->getProperty("preset");
			sceneEditorScreenController->setEntityProperties(preset != nullptr ? preset->getValue() : "", sceneEntity, "");
		} else {
			sceneEditorScreenController->unsetEntityProperties();
		}
	} else
	if (selectedEntityIds.size() > 1) {
		sceneEditorScreenController->unsetEntityProperties();
	}
}

void SceneEditorView::centerEntity()
{
	if (selectedEntityIds.size() == 0) {
		return;
	}
	Vector3 center;
	for (auto selectedEntityId: selectedEntityIds) {
		auto selectedEntity = engine->getEntity(selectedEntityId);
		if (selectedEntity == nullptr) continue;
		center.add(selectedEntity->getBoundingBoxTransformed()->getMin().clone().add(selectedEntity->getBoundingBoxTransformed()->getMax()).scale(0.5f));
	}
	engine->getCamera()->setLookAt(center.scale(1.0f / selectedEntityIds.size()));
}

void SceneEditorView::entityTranslationApply(float x, float y, float z)
{
	if (selectedEntityIds.size() == 0)
		return;

	if (selectedEntityIds.size() == 1) {
		auto selectedEntity = engine->getEntity(selectedEntityIds[0]);
		if (selectedEntity == nullptr) return;
		auto sceneEntity = scene.getEntity(selectedEntity->getId());
		if (sceneEntity == nullptr) return;

		sceneEntity->getTransformations().setTranslation(Vector3(x, y, z));
		sceneEntity->getTransformations().update();
		selectedEntity->fromTransformations(sceneEntity->getTransformations());
	} else
	if (selectedEntityIds.size() > 1) {
		for (auto selectedEntityId: selectedEntityIds) {
			auto selectedEntity = engine->getEntity(selectedEntityId);
			if (selectedEntity == nullptr) continue;
			auto sceneEntity = scene.getEntity(selectedEntity->getId());
			if (sceneEntity == nullptr) continue;
			sceneEntity->getTransformations().setTranslation(
				sceneEntity->getTransformations().getTranslation().clone().add(Vector3(x, y, z))
			);
			sceneEntity->getTransformations().update();
			selectedEntity->fromTransformations(sceneEntity->getTransformations());
		}
		sceneEditorScreenController->setEntityTransformations(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), 0.0f, 0.0f, 0.0f, false);
	}
	scene.update();
	cameraInputHandler->setSceneCenter(Vector3(scene.getCenter().getX(), scene.getBoundingBox()->getMax().getY() + 3.0f, scene.getCenter().getZ()));
	updateGizmo();
	updateGUIElements();
}

void SceneEditorView::entityScaleApply(float x, float y, float z)
{
	if (selectedEntityIds.size() == 0)
		return;

	if (selectedEntityIds.size() == 1) {
		auto selectedEntity = engine->getEntity(selectedEntityIds[0]);
		if (selectedEntity == nullptr) return;
		auto sceneEntity = scene.getEntity(selectedEntity->getId());
		if (sceneEntity == nullptr) return;

		sceneEntity->getTransformations().setScale(Vector3(x, y, z));
		sceneEntity->getTransformations().update();
		selectedEntity->fromTransformations(sceneEntity->getTransformations());
	} else
	if (selectedEntityIds.size() > 1) {
		for (auto selectedEntityId: selectedEntityIds) {
			auto selectedEntity = engine->getEntity(selectedEntityId);
			if (selectedEntity == nullptr) continue;
			auto sceneEntity = scene.getEntity(selectedEntity->getId());
			if (sceneEntity == nullptr) continue;

			sceneEntity->getTransformations().setScale(sceneEntity->getTransformations().getScale().clone().scale(Vector3(x, y, z)));
			sceneEntity->getTransformations().update();
			selectedEntity->fromTransformations(sceneEntity->getTransformations());
		}
		sceneEditorScreenController->setEntityTransformations(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), 0.0f, 0.0f, 0.0f, false);
	}
	scene.update();
	cameraInputHandler->setSceneCenter(Vector3(scene.getCenter().getX(), scene.getBoundingBox()->getMax().getY() + 3.0f, scene.getCenter().getZ()));
	updateGizmo();
	updateGUIElements();
}

void SceneEditorView::entityRotationsApply(float x, float y, float z)
{
	if (selectedEntityIds.size() == 0)
		return;

	if (selectedEntityIds.size() == 1) {
		auto selectedEntity = engine->getEntity(selectedEntityIds[0]);
		if (selectedEntity == nullptr) return;
		auto sceneEntity = scene.getEntity(selectedEntity->getId());
		if (sceneEntity == nullptr) return;
		sceneEntity->getTransformations().getRotation(scene.getRotationOrder()->getAxisXIndex()).setAngle(x);
		sceneEntity->getTransformations().getRotation(scene.getRotationOrder()->getAxisYIndex()).setAngle(y);
		sceneEntity->getTransformations().getRotation(scene.getRotationOrder()->getAxisZIndex()).setAngle(z);
		sceneEntity->getTransformations().update();
		selectedEntity->fromTransformations(sceneEntity->getTransformations());
	} else
	if (selectedEntityIds.size() > 1) {
		for (auto selectedEntityId: selectedEntityIds) {
			auto selectedEntity = engine->getEntity(selectedEntityId);
			if (selectedEntity == nullptr) continue;
			auto sceneEntity = scene.getEntity(selectedEntity->getId());
			if (sceneEntity == nullptr) continue;
			if ((sceneEntity->getPrototype()->getType()->getGizmoTypeMask() & Gizmo::GIZMOTYPE_ROTATE) == Gizmo::GIZMOTYPE_ROTATE) {
				sceneEntity->getTransformations().getRotation(scene.getRotationOrder()->getAxisXIndex()).setAngle(sceneEntity->getTransformations().getRotation(scene.getRotationOrder()->getAxisXIndex()).getAngle() + x);
				sceneEntity->getTransformations().getRotation(scene.getRotationOrder()->getAxisYIndex()).setAngle(sceneEntity->getTransformations().getRotation(scene.getRotationOrder()->getAxisYIndex()).getAngle() + y);
				sceneEntity->getTransformations().getRotation(scene.getRotationOrder()->getAxisZIndex()).setAngle(sceneEntity->getTransformations().getRotation(scene.getRotationOrder()->getAxisZIndex()).getAngle() + z);
			}
			sceneEntity->getTransformations().update();
			selectedEntity->fromTransformations(sceneEntity->getTransformations());
		}
		sceneEditorScreenController->setEntityTransformations(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), 0.0f, 0.0f, 0.0f, false);
	}
	scene.update();
	cameraInputHandler->setSceneCenter(Vector3(scene.getCenter().getX(), scene.getBoundingBox()->getMax().getY() + 3.0f, scene.getCenter().getZ()));
	updateGizmo();
	updateGUIElements();
}

bool SceneEditorView::scenePropertySave(const string& oldName, const string& name, const string& value)
{
	if (scene.updateProperty(oldName, name, value) == true) {
		sceneEditorScreenController->setSceneProperties(scene, name);
		return true;
	}
	return false;
}

bool SceneEditorView::scenePropertyAdd()
{
	if (scene.addProperty("new.property", "new.value")) {
		sceneEditorScreenController->setSceneProperties(scene, "new.property");
		return true;
	}
	return false;
}

bool SceneEditorView::scenePropertyRemove(const string& name)
{
	auto idx = scene.getPropertyIndex(name);
	if (idx != -1 && scene.removeProperty(name) == true) {
		auto property = scene.getPropertyByIndex(idx);
		if (property == nullptr) {
			property = scene.getPropertyByIndex(idx - 1);
		}
		sceneEditorScreenController->setSceneProperties(scene, property == nullptr ? "" : property->getName());
		return true;
	}
	return false;
}

bool SceneEditorView::entityPropertyRemove(const string& name)
{
	if (selectedEntityIds.size() != 1) return false;

	auto selectedEntity = engine->getEntity(selectedEntityIds[0]);
	if (selectedEntity == nullptr) return false;
	auto sceneEntity = scene.getEntity(selectedEntity->getId());
	if (sceneEntity == nullptr) return false;

	auto idx = sceneEntity->getPropertyIndex(name);
	if (idx != -1 && sceneEntity->removeProperty(name) == true) {
		auto property = sceneEntity->getPropertyByIndex(idx);
		if (property == nullptr) {
			property = sceneEntity->getPropertyByIndex(idx - 1);
		}
		sceneEditorScreenController->setEntityProperties("", sceneEntity, property == nullptr ? "" : property->getName());
		return true;
	}
	return false;
}

void SceneEditorView::entityPropertiesPreset(const string& presetId)
{
	if (selectedEntityIds.size() != 1) return;

	auto selectedEntity = engine->getEntity(selectedEntityIds[0]);
	if (selectedEntity == nullptr) return;
	auto sceneEntity = scene.getEntity(selectedEntity->getId());
	if (sceneEntity == nullptr) return;

	sceneEntity->clearProperties();
	auto& entityPropertiesPresets = ScenePropertyPresets::getInstance()->getEntityPropertiesPresets();
	const vector<PrototypeProperty*>* entityPropertyPresetVector = nullptr;
	auto entityPropertyPresetVectorIt = entityPropertiesPresets.find(presetId);
	if (entityPropertyPresetVectorIt != entityPropertiesPresets.end()) {
		entityPropertyPresetVector = &entityPropertyPresetVectorIt->second;
	}
	if (entityPropertyPresetVector != nullptr) {
		for (auto entityPropertyPreset: *entityPropertyPresetVector) {
			sceneEntity->addProperty(entityPropertyPreset->getName(), entityPropertyPreset->getValue());
		}
	}
	sceneEditorScreenController->setEntityProperties(presetId, sceneEntity, "");
}

bool SceneEditorView::entityPropertySave(const string& oldName, const string& name, const string& value)
{
	if (selectedEntityIds.size() != 1) return false;

	auto selectedEntity = engine->getEntity(selectedEntityIds[0]);
	if (selectedEntity == nullptr) return false;
	auto sceneEntity = scene.getEntity(selectedEntity->getId());
	if (sceneEntity == nullptr) return false;

	if (sceneEntity->updateProperty(oldName, name, value) == true) {
		sceneEditorScreenController->setEntityProperties("", sceneEntity, name);
		return true;
	}
	return false;
}

bool SceneEditorView::entityPropertyAdd()
{
	if (selectedEntityIds.size() != 1) return false;

	auto selectedEntity = engine->getEntity(selectedEntityIds[0]);
	if (selectedEntity == nullptr) return false;
	auto sceneEntity = scene.getEntity(selectedEntity->getId());
	if (sceneEntity == nullptr) return false;

	if (sceneEntity->addProperty("new.property", "new.value")) {
		sceneEditorScreenController->setEntityProperties("", sceneEntity, "new.property");
		return true;
	}
	return false;
}

void SceneEditorView::loadScene(const string& path, const string& file)
{
	SceneConnector::resetEngine(engine, scene);
	selectedPrototype = nullptr;
	try {
		bool haveModelFile = false;
		for (auto &modelExtension: ModelReader::getModelExtensions()) {
			if (StringTools::endsWith(StringTools::toLowerCase(file), "." + modelExtension) == true) {
				haveModelFile = true;
				break;
			}
		}

		// progress call back
		class ImportProgressCallback: public ProgressCallback {
		private:
			ProgressBarScreenController* progressBarScreenController;
		public:
			ImportProgressCallback(ProgressBarScreenController* progressBarScreenController): progressBarScreenController(progressBarScreenController) {
			}
			virtual void progress(float value) {
				progressBarScreenController->progress(value);
			}
		};
		popUps->getProgressBarScreenController()->show();
		if (haveModelFile == true) {
			SceneReader::readFromModel(path, file, scene, new ImportProgressCallback(popUps->getProgressBarScreenController()));
		} else {
			SceneReader::read(path, file, scene, new ImportProgressCallback(popUps->getProgressBarScreenController()));
		}
		popUps->getProgressBarScreenController()->close();
		for (auto i = 0; i < scene.getLibrary()->getPrototypeCount(); i++) {
			auto prototype = scene.getLibrary()->getPrototypeAt(i);
			if (prototype->getType()->getBoundingVolumeCount() != 0) prototype->setDefaultBoundingVolumes(prototype->getType()->getBoundingVolumeCount());
		}
		sceneEditorScreenController->setSky(scene);
		sceneEditorScreenController->setSceneProperties(scene, "");
		sceneEditorScreenController->unsetEntityProperties();
		sceneEditorScreenController->unsetEntityTransformations();
		loadScene();
		reloadEntityLibrary = true;
		updateGUIElements();
	} catch (Exception& exception) {
		sceneEditorScreenController->showErrorPopUp(
			"Warning: Could not load scene file",
			(string(exception.what()))
		);
	}
}

void SceneEditorView::saveScene(const string& pathName, const string& fileName)
{
	try {
		SceneWriter::write(pathName, fileName, scene);
	} catch (Exception& exception) {
		sceneEditorScreenController->showErrorPopUp(
			"Warning: Could not save scene file",
			(string(exception.what()))
		);
	}
	updateGUIElements();
}

void SceneEditorView::copyEntities()
{
	copiedEntities.clear();
	for (auto selectedEntityId: selectedEntityIds) {
		auto selectedEntity = engine->getEntity(selectedEntityId);
		if (selectedEntity != nullptr && StringTools::startsWith(selectedEntity->getId(), "tdme.sceneeditor.") == false) {
			auto sceneEntity = scene.getEntity(selectedEntity->getId());
			if (sceneEntity == nullptr) continue;
			copiedEntities.push_back(sceneEntity);
		}
	}
}

void SceneEditorView::setPasteMode() {
	pasteMode = true;
	pasteModeValid = false;
}

void SceneEditorView::unsetPasteMode() {
	auto pasteEntityIdx = 0;
	for (auto pasteEntity: copiedEntities) {
		auto pastePrototype = pasteEntity->getPrototype();
		auto entityId = "tdme.sceneeditor.paste." + pastePrototype->getName() + "." + to_string(pasteEntityIdx);
		engine->removeEntity(entityId);
		pasteEntityIdx++;
	}
	pasteMode = false;
	pasteModeValid = false;
}

void SceneEditorView::pasteEntities(bool displayOnly)
{
	auto pasteEntitiesMinX = Float::MAX_VALUE;
	auto pasteEntitiesMinZ = Float::MAX_VALUE;
	auto pasteEntitiesMinY = Float::MAX_VALUE;
	for (auto copiedEntity: copiedEntities) {
		auto entity = engine->getEntity(copiedEntity->getId());
		if (entity == nullptr) continue;
		BoundingBox cbv;
		cbv.fromBoundingVolumeWithTransformations(entity->getBoundingBox(), copiedEntity->getTransformations());
		auto& entityBBMinXYZ = cbv.getMin().getArray();
		if (entityBBMinXYZ[0] < pasteEntitiesMinX) pasteEntitiesMinX = entityBBMinXYZ[0];
		if (entityBBMinXYZ[1] < pasteEntitiesMinY) pasteEntitiesMinY = entityBBMinXYZ[1];
		if (entityBBMinXYZ[2] < pasteEntitiesMinZ) pasteEntitiesMinZ = entityBBMinXYZ[2];
	}
	auto pasteEntitiesIdx = 0;
	for (auto copiedEntity: copiedEntities) {
		auto pastePrototype = copiedEntity->getPrototype();
		Transformations sceneEntityTransformations;
		sceneEntityTransformations.fromTransformations(copiedEntity->getTransformations());
		auto entityDiffX = copiedEntity->getTransformations().getTranslation().getX() - pasteEntitiesMinX;
		auto entityDiffY = copiedEntity->getTransformations().getTranslation().getY() - pasteEntitiesMinY;
		auto entityDiffZ = copiedEntity->getTransformations().getTranslation().getZ() - pasteEntitiesMinZ;
		sceneEntityTransformations.setTranslation(
			Vector3(
				placeEntityTranslation.getX() + entityDiffX,
				placeEntityTranslation.getY() + entityDiffY,
				placeEntityTranslation.getZ() + entityDiffZ
			)
		);
		sceneEntityTransformations.update();
		if (displayOnly == false) {
			for (auto i = 0; i < scene.getEntityCount(); i++) {
				auto sceneEntity = scene.getEntityAt(i);
				if (sceneEntity->getPrototype() == pastePrototype && sceneEntity->getTransformations().getTranslation().equals(sceneEntityTransformations.getTranslation())) {
					continue;
				}
			}
		}
		if (displayOnly == false) {
			//
			auto sceneEntityId = pastePrototype->getName() + "_" + to_string(scene.allocateEntityId());
			auto sceneEntity = new SceneEntity(
				sceneEntityId,
				"",
				sceneEntityTransformations,
				pastePrototype
			 );
			PrototypeProperties* properties = copiedEntity;
			for (int i = 0; i < properties->getPropertyCount(); i++) {
				PrototypeProperty* property = properties->getPropertyByIndex(i);
				sceneEntity->addProperty(property->getName(), property->getValue());
			}
			scene.addEntity(sceneEntity);
			auto entity = SceneConnector::createEntity(pastePrototype, sceneEntityId, sceneEntityTransformations);
			if (entity != nullptr) {
				resetEntity(entity);
				entity->setPickable(true);
				engine->addEntity(entity);
			}
		} else {
			auto entityId = "tdme.sceneeditor.paste." + pastePrototype->getName() + "." + to_string(pasteEntitiesIdx);
			auto entity = engine->getEntity(entityId);
			if (entity != nullptr) {
				entity->fromTransformations(sceneEntityTransformations);
			} else {
				entity = SceneConnector::createEntity(pastePrototype, entityId, sceneEntityTransformations);
				if (entity != nullptr) {
					setStandardEntityColorEffect(entity);
					entity->setPickable(true);
					engine->addEntity(entity);
				}
			}
		}
		pasteEntitiesIdx++;
	}
	if (displayOnly == false) sceneEditorScreenController->setEntityListbox(scene);
}

void SceneEditorView::computeSpotDirection(int i, const Vector4& position, const Vector3& spotTo)
{
	auto _from = Vector3(position.getX(), position.getY(), position.getZ());
	auto spotDirection = spotTo.clone().sub(_from);
	scene.getLightAt(i)->getPosition().set(position.getX(), position.getY(), position.getZ(), position.getW());
	scene.getLightAt(i)->getSpotTo().set(spotTo.getX(), spotTo.getY(), spotTo.getZ());
	scene.getLightAt(i)->getSpotDirection().set(spotDirection.getX(), spotDirection.getY(), spotDirection.getZ());
	engine->getLightAt(i)->setPosition(Vector4(position.getX(), position.getY(), position.getZ(), position.getW()));
	engine->getLightAt(i)->setSpotDirection(Vector3(spotDirection.getX(), spotDirection.getY(), spotDirection.getZ()));
	sceneEditorScreenController->setLight(i, scene.getLightAt(i)->getAmbient(), scene.getLightAt(i)->getDiffuse(), scene.getLightAt(i)->getSpecular(), scene.getLightAt(i)->getPosition(), scene.getLightAt(i)->getConstantAttenuation(), scene.getLightAt(i)->getLinearAttenuation(), scene.getLightAt(i)->getQuadraticAttenuation(), scene.getLightAt(i)->getSpotTo(), scene.getLightAt(i)->getSpotDirection(), scene.getLightAt(i)->getSpotExponent(), scene.getLightAt(i)->getSpotCutOff(), scene.getLightAt(i)->isEnabled());
}

void SceneEditorView::applyLight(int i, const Color4& ambient, const Color4& diffuse, const Color4& specular, const Vector4& position, float constantAttenuation, float linearAttenuation, float quadraticAttenuation, const Vector3& spotTo, const Vector3& spotDirection, float spotExponent, float spotCutoff, bool enabled)
{
	scene.getLightAt(i)->getAmbient().set(ambient);
	scene.getLightAt(i)->getDiffuse().set(diffuse);
	scene.getLightAt(i)->getSpecular().set(specular);
	scene.getLightAt(i)->getPosition().set(position);
	scene.getLightAt(i)->setConstantAttenuation(constantAttenuation);
	scene.getLightAt(i)->setLinearAttenuation(linearAttenuation);
	scene.getLightAt(i)->setQuadraticAttenuation(quadraticAttenuation);
	scene.getLightAt(i)->getSpotTo().set(spotTo.getX(), spotTo.getY(), spotTo.getZ());
	scene.getLightAt(i)->getSpotDirection().set(spotDirection);
	scene.getLightAt(i)->setSpotExponent(spotExponent);
	scene.getLightAt(i)->setSpotCutOff(spotCutoff);
	scene.getLightAt(i)->setEnabled(enabled);
	engine->getLightAt(i)->setAmbient(Color4(ambient));
	engine->getLightAt(i)->setDiffuse(Color4(diffuse));
	engine->getLightAt(i)->setSpecular(Color4(specular));
	engine->getLightAt(i)->setPosition(Vector4(position));
	engine->getLightAt(i)->setConstantAttenuation(constantAttenuation);
	engine->getLightAt(i)->setLinearAttenuation(linearAttenuation);
	engine->getLightAt(i)->setQuadraticAttenuation(quadraticAttenuation);
	engine->getLightAt(i)->setSpotDirection(spotDirection);
	engine->getLightAt(i)->setSpotExponent(spotExponent);
	engine->getLightAt(i)->setSpotCutOff(spotCutoff);
	engine->getLightAt(i)->setEnabled(enabled);
	sceneEditorScreenController->setLight(i, scene.getLightAt(i)->getAmbient(), scene.getLightAt(i)->getDiffuse(), scene.getLightAt(i)->getSpecular(), scene.getLightAt(i)->getPosition(), scene.getLightAt(i)->getConstantAttenuation(), scene.getLightAt(i)->getLinearAttenuation(), scene.getLightAt(i)->getQuadraticAttenuation(), scene.getLightAt(i)->getSpotTo(), scene.getLightAt(i)->getSpotDirection(), scene.getLightAt(i)->getSpotExponent(), scene.getLightAt(i)->getSpotCutOff(), scene.getLightAt(i)->isEnabled());
}

void SceneEditorView::updateSky() {
	engine->removeEntity("tdme.sky");
	if (scene.getSkyModel() == nullptr) return;
	auto sky = new Object3D("tdme.sky", scene.getSkyModel());
	sky->setRenderPass(Entity::RENDERPASS_NOFRUSTUMCULLING);
	sky->setShader("sky");
	sky->setFrustumCulling(false);
	sky->setTranslation(Vector3(0.0f, 0.0f, 0.0f));
	sky->setScale(scene.getSkyModelScale());
	sky->update();
	sky->setContributesShadows(false);
	sky->setReceivesShadows(false);
	sky->setExcludeEffectPass(Engine::EFFECTPASS_LIGHTSCATTERING);
	engine->addEntity(sky);
}

void SceneEditorView::updateSkyPosition() {
	auto sky = engine->getEntity("tdme.sky");
	if (sky == nullptr) return;
	sky->setTranslation(engine->getCamera()->getLookAt());
	sky->update();
}

void SceneEditorView::applyReflectionEnvironmentMappingId(const string& reflectionEnvironmentMappingId) {
	for (auto& selectedEntityId: selectedEntityIds) {
		auto sceneEntity = scene.getEntity(selectedEntityId);
		if (sceneEntity == nullptr) continue;
		sceneEntity->setReflectionEnvironmentMappingId(reflectionEnvironmentMappingId);
	}
}

void SceneEditorView::updateGizmo() {
	if (selectedEntityIds.size() == 0) {
		removeGizmo();
		return;
	}

	// rotation for gizmo
	Transformations transformations;

	//
	Vector3 gizmoCenter;
	auto entityCount = 0;
	for (auto selectedEntityId: selectedEntityIds) {
		auto selectedEntity = engine->getEntity(selectedEntityId);
		if (selectedEntity != nullptr && StringTools::startsWith(selectedEntity->getId(), "tdme.sceneeditor.") == false) {
			auto sceneEntity = scene.getEntity(selectedEntity->getId());
			if (sceneEntity == nullptr) continue;
			gizmoCenter.add(sceneEntity->getTransformations().getTranslation());
			entityCount++;
		}
	}
	if (entityCount == 0) {
		removeGizmo();
		return;
	} else
	if (entityCount == 1) {
		auto selectedSceneEntity = scene.getEntity(selectedEntityIds[0]);
		auto selectedPrototype = selectedSceneEntity != nullptr?selectedSceneEntity->getPrototype():nullptr;
		if (selectedSceneEntity != nullptr) transformations.fromTransformations(selectedSceneEntity->getTransformations());
		setGizmoTypeMask(selectedPrototype->getType()->getGizmoTypeMask());
	} else {
		gizmoCenter.scale(1.0f / entityCount);
	}

	//
	Gizmo::updateGizmo(gizmoCenter, transformations);
}

void SceneEditorView::onCameraTranslation() {
	needGizmoUpdate = true;
}

void SceneEditorView::onCameraRotation() {
	needGizmoUpdate = true;
}

void SceneEditorView::onCameraScale() {
	needGizmoUpdate = true;
}
