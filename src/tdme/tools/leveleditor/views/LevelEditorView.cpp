#include <tdme/tools/leveleditor/views/LevelEditorView.h>

#include <algorithm>
#include <string>
#include <vector>

#include <tdme/engine/Camera.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Entity.h>
#include <tdme/engine/EntityPickingFilter.h>
#include <tdme/engine/Light.h>
#include <tdme/engine/Object3D.h>
#include <tdme/engine/PartitionOctTree.h>
#include <tdme/engine/Rotation.h>
#include <tdme/engine/Timing.h>
#include <tdme/engine/Transformations.h>
#include <tdme/engine/fileio/models/ModelReader.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Face.h>
#include <tdme/engine/model/FacesEntity.h>
#include <tdme/engine/model/Group.h>
#include <tdme/engine/model/Material.h>
#include <tdme/engine/model/Model.h>
#include <tdme/utilities/ModelTools.h>
#include <tdme/engine/model/RotationOrder.h>
#include <tdme/engine/model/SpecularMaterialProperties.h>
#include <tdme/engine/model/TextureCoordinate.h>
#include <tdme/engine/model/UpVector.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/primitives/LineSegment.h>
#include <tdme/gui/GUI.h>
#include <tdme/gui/events/GUIKeyboardEvent_Type.h>
#include <tdme/gui/events/GUIKeyboardEvent.h>
#include <tdme/gui/events/GUIMouseEvent.h>
#include <tdme/gui/events/GUIMouseEvent_Type.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/math/Math.h>
#include <tdme/math/Quaternion.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Vector4.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/tools/leveleditor/TDMELevelEditor.h>
#include <tdme/tools/leveleditor/controller/LevelEditorEntityLibraryScreenController.h>
#include <tdme/tools/leveleditor/controller/LevelEditorScreenController.h>
#include <tdme/tools/leveleditor/logic/Level.h>
#include <tdme/tools/leveleditor/views/LevelEditorView_ObjectColor.h>
#include <tdme/tools/shared/controller/FileDialogPath.h>
#include <tdme/tools/shared/controller/FileDialogScreenController.h>
#include <tdme/tools/shared/controller/InfoDialogScreenController.h>
#include <tdme/tools/shared/controller/ProgressBarScreenController.h>
#include <tdme/tools/shared/files/LevelFileExport.h>
#include <tdme/tools/shared/files/LevelFileImport.h>
#include <tdme/tools/shared/files/ProgressCallback.h>
#include <tdme/tools/shared/model/LevelEditorEntity_EntityType.h>
#include <tdme/tools/shared/model/LevelEditorEntity.h>
#include <tdme/tools/shared/model/LevelEditorEntityLibrary.h>
#include <tdme/tools/shared/model/LevelEditorLevel.h>
#include <tdme/tools/shared/model/LevelEditorLight.h>
#include <tdme/tools/shared/model/LevelEditorObject.h>
#include <tdme/tools/shared/model/LevelPropertyPresets.h>
#include <tdme/tools/shared/model/PropertyModelClass.h>
#include <tdme/tools/shared/tools/Tools.h>
#include <tdme/tools/shared/views/Gizmo.h>
#include <tdme/tools/shared/views/PopUps.h>
#include <tdme/utilities/Character.h>
#include <tdme/utilities/Float.h>
#include <tdme/utilities/Properties.h>
#include <tdme/utilities/StringTools.h>
#include <tdme/utilities/Exception.h>
#include <tdme/utilities/Console.h>

using std::find;
using std::remove;
using std::vector;
using std::to_string;
using std::string;

using tdme::tools::leveleditor::views::LevelEditorView;

using tdme::engine::Camera;
using tdme::engine::Engine;
using tdme::engine::Entity;
using tdme::engine::EntityPickingFilter;
using tdme::engine::Light;
using tdme::engine::Object3D;
using tdme::engine::PartitionOctTree;
using tdme::engine::Rotation;
using tdme::engine::Timing;
using tdme::engine::Transformations;
using tdme::engine::fileio::models::ModelReader;
using tdme::engine::model::Color4;
using tdme::engine::model::Face;
using tdme::engine::model::FacesEntity;
using tdme::engine::model::Group;
using tdme::engine::model::Material;
using tdme::engine::model::UpVector;
using tdme::engine::model::Model;
using tdme::utilities::ModelTools;
using tdme::engine::model::RotationOrder;
using tdme::engine::model::SpecularMaterialProperties;
using tdme::engine::model::TextureCoordinate;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::primitives::LineSegment;
using tdme::gui::GUI;
using tdme::gui::events::GUIKeyboardEvent_Type;
using tdme::gui::events::GUIKeyboardEvent;
using tdme::gui::events::GUIMouseEvent;
using tdme::gui::events::GUIMouseEvent_Type;
using tdme::gui::nodes::GUIScreenNode;
using tdme::math::Math;
using tdme::math::Quaternion;
using tdme::math::Vector3;
using tdme::math::Vector4;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;
using tdme::tools::leveleditor::TDMELevelEditor;
using tdme::tools::leveleditor::controller::LevelEditorEntityLibraryScreenController;
using tdme::tools::leveleditor::controller::LevelEditorScreenController;
using tdme::tools::leveleditor::logic::Level;
using tdme::tools::leveleditor::views::LevelEditorView_ObjectColor;
using tdme::tools::shared::controller::FileDialogPath;
using tdme::tools::shared::controller::FileDialogScreenController;
using tdme::tools::shared::controller::InfoDialogScreenController;
using tdme::tools::shared::controller::ProgressBarScreenController;
using tdme::tools::shared::files::LevelFileExport;
using tdme::tools::shared::files::LevelFileImport;
using tdme::tools::shared::files::ProgressCallback;
using tdme::tools::shared::model::LevelEditorEntity_EntityType;
using tdme::tools::shared::model::LevelEditorEntity;
using tdme::tools::shared::model::LevelEditorEntityLibrary;
using tdme::tools::shared::model::LevelEditorLevel;
using tdme::tools::shared::model::LevelEditorLight;
using tdme::tools::shared::model::LevelEditorObject;
using tdme::tools::shared::model::LevelPropertyPresets;
using tdme::tools::shared::model::PropertyModelClass;
using tdme::tools::shared::tools::Tools;
using tdme::tools::shared::views::Gizmo;
using tdme::tools::shared::views::PopUps;
using tdme::utilities::Character;
using tdme::utilities::Float;
using tdme::utilities::Properties;
using tdme::utilities::StringTools;
using tdme::utilities::Exception;
using tdme::utilities::Console;

vector<string> LevelEditorView::OBJECTCOLOR_NAMES = {
	"blue",
	"yellow",
	"magenta",
	"cyan",
	"none"
};

LevelEditorView::LevelEditorView(PopUps* popUps): Gizmo(Engine::getInstance(), "le")
{
	this->popUps = popUps;
	snappingX = 1.0f;
	snappingZ = 1.0f;
	snappingEnabled = false;
	camLookRotationX = new Rotation(-45.0f, Vector3(1.0f, 0.0f, 0.0f));
	camLookRotationY = new Rotation(0.0f, Vector3(0.0f, 1.0f, 0.0f));
	camScaleMax = 15.0f;
	camScaleMin = 0.05f;
	mouseDownLastX = LevelEditorView::MOUSE_DOWN_LAST_POSITION_NONE;
	mouseDownLastY = LevelEditorView::MOUSE_DOWN_LAST_POSITION_NONE;
	mousePanningSide = LevelEditorView::MOUSE_PANNING_NONE;
	mousePanningForward = LevelEditorView::MOUSE_PANNING_NONE;
	mouseRotationX = LevelEditorView::MOUSE_ROTATION_NONE;
	mouseRotationY = LevelEditorView::MOUSE_ROTATION_NONE;
	level = TDMELevelEditor::getInstance()->getLevel();
	reloadEntityLibrary = false;
	selectedEntity = nullptr;
	keyLeft = false;
	keyRight = false;
	keyUp = false;
	keyDown = false;
	keyA = false;
	keyD = false;
	keyW = false;
	keyS = false;
	keyPlus = false;
	keyMinus = false;
	keyR = false;
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
	mouseDraggingLastObject = nullptr;
	gridEnabled = false;
	gridY = 0.0f;
	objectColors["red"] = new LevelEditorView_ObjectColor(this, 1.5f, 0.8f, 0.8f, 0.5f, 0.0f, 0.0f);
	objectColors["green"] = new LevelEditorView_ObjectColor(this, 0.8f, 1.5f, 0.8f, 0.0f, 0.5f, 0.0f);
	objectColors["blue"] = new LevelEditorView_ObjectColor(this, 0.8f, 0.8f, 1.5f, 0.0f, 0.0f, 0.5f);
	objectColors["yellow"] = new LevelEditorView_ObjectColor(this, 1.5f, 1.5f, 0.8f, 0.5f, 0.5f, 0.0f);
	objectColors["magenta"] = new LevelEditorView_ObjectColor(this, 1.5f, 0.8f, 1.5f, 0.5f, 0.0f, 0.5f);
	objectColors["cyan"] = new LevelEditorView_ObjectColor(this, 0.8f, 1.5f, 1.5f, 0.0f, 0.5f, 0.5f);
	objectColors["none"] = new LevelEditorView_ObjectColor(this, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	camScale = 1.0f;
	camLookRotationX->update();
	camLookRotationY->update();
	levelEditorGround = createLevelEditorGroundPlateModel();
	engine = Engine::getInstance();

	{
		// entity picking filter for no grid
		class LevelEditorEntityPickingFilterNoGrid: public virtual EntityPickingFilter
		{
		public:
			bool filterEntity(Entity* entity) override {
				return entity->getId() != "tdme.leveleditor.grid";
			}

			/**
			 * Public constructor
			 * @param levelEditorView level editor view
			 */
			LevelEditorEntityPickingFilterNoGrid(LevelEditorView* levelEditorView): levelEditorView(levelEditorView) {
			}

		private:
			LevelEditorView* levelEditorView;
		};
		entityPickingFilterNoGrid = new LevelEditorEntityPickingFilterNoGrid(this);
	}

	{
		// entity picking filter for no placing object
		class LevelEditorEntityPickingFilterPlacing: public virtual EntityPickingFilter
		{
		public:
			bool filterEntity(Entity* entity) override {
				return
						entity->getId() != "tdme.leveleditor.placeentity" &&
						StringTools::startsWith(entity->getId(), "tdme.leveleditor.paste.") == false &&
						StringTools::startsWith(entity->getId(), "le.tdme.leveleditor.gizmo.") == false;
			}

			/**
			 * Public constructor
			 * @param levelEditorView level editor view
			 */
			LevelEditorEntityPickingFilterPlacing(LevelEditorView* levelEditorView): levelEditorView(levelEditorView) {
			}

		private:
			LevelEditorView* levelEditorView;
		};
		entityPickingFilterPlacing = new LevelEditorEntityPickingFilterPlacing(this);
	}
}

LevelEditorView::~LevelEditorView() {
	delete camLookRotationX;
	delete camLookRotationY;
	delete objectColors["red"];
	delete objectColors["green"];
	delete objectColors["blue"];
	delete objectColors["yellow"];
	delete objectColors["magenta"];
	delete objectColors["cyan"];
	delete objectColors["none"];
	delete levelEditorGround;
	delete entityPickingFilterNoGrid;
	delete entityPickingFilterPlacing;
	delete levelEditorScreenController;
}

PopUps* LevelEditorView::getPopUps()
{
	return popUps;
}

const string LevelEditorView::getFileName()
{
	return FileSystem::getInstance()->getFileName(level->getFileName());
}

LevelEditorLevel* LevelEditorView::getLevel()
{
	return level;
}

LevelEditorEntity* LevelEditorView::getSelectedEntity()
{
	return selectedEntity;
}

LevelEditorObject* LevelEditorView::getSelectedObject()
{
	if (selectedEntityIds.size() != 1) return nullptr;

	auto selectedObject = level->getObjectById(selectedEntityIds[0]);
	return selectedObject != nullptr && StringTools::startsWith(selectedObject->getId(), "tdme.leveleditor.") == false ? level->getObjectById(selectedObject->getId()) : static_cast< LevelEditorObject* >(nullptr);
}

bool LevelEditorView::isGridEnabled()
{
	return gridEnabled;
}

void LevelEditorView::setGridEnabled(bool gridEnabled)
{
	this->gridEnabled = gridEnabled;
	if (gridEnabled) {
		updateGrid();
	} else {
		removeGrid();
	}
}

float LevelEditorView::getGridY()
{
	return gridY;
}

void LevelEditorView::setGridY(float gridY)
{
	if (gridEnabled)
		removeGrid();

	this->gridY = gridY;
	if (gridEnabled)
		updateGrid();

}

void LevelEditorView::getSnapping(bool& snappingEnabled, float& snappingX, float& snappingZ) {
	snappingEnabled = this->snappingEnabled;
	snappingX = this->snappingX;
	snappingZ = this->snappingZ;
}

void LevelEditorView::setSnapping(bool snappingEnabled, float snappingX, float snappingZ) {
	this->snappingEnabled = snappingEnabled;
	this->snappingX = snappingX;
	this->snappingZ = snappingZ;
}

void LevelEditorView::loadEntityFromLibrary(int32_t id)
{
	selectedEntity = TDMELevelEditor::getInstance()->getEntityLibrary()->getEntity(id);
}

void LevelEditorView::handleInputEvents()
{
	#if !defined(GLFW3) && !defined(VUKAN)
		keyControl = false;
		keyShift = false;
	#endif
	auto keyControlX = false;
	auto keyControlC = false;
	auto keyControlV = false;
	auto keyDelete = false;
	mousePanningSide = MOUSE_PANNING_NONE;
	mousePanningForward = MOUSE_PANNING_NONE;
	for (auto i = 0; i < engine->getGUI()->getKeyboardEvents().size(); i++) {
		auto& event = engine->getGUI()->getKeyboardEvents()[i];
		if (event.isProcessed() == true) continue;
		if (event.getType() == GUIKeyboardEvent_Type::KEYBOARDEVENT_KEY_TYPED) continue;
		auto isKeyDown = event.getType() == GUIKeyboardEvent_Type::KEYBOARDEVENT_KEY_PRESSED;
		#if defined(GLFW3) || defined(VUKAN)
			if (event.getKeyCode() == KEYBOARD_KEYCODE_LEFT_SHIFT) keyShift = isKeyDown;
			if (event.getKeyCode() == KEYBOARD_KEYCODE_LEFT_CTRL) keyControl = isKeyDown;
		#else
			keyControl = event.isControlDown();
			keyShift = event.isShiftDown();
		#endif
		if (event.getKeyCode() == GUIKeyboardEvent::KEYCODE_ESCAPE) keyEscape = isKeyDown;
		if (event.getKeyCode() == GUIKeyboardEvent::KEYCODE_LEFT) keyLeft = isKeyDown;
		if (event.getKeyCode() == GUIKeyboardEvent::KEYCODE_RIGHT) keyRight = isKeyDown;
		if (event.getKeyCode() == GUIKeyboardEvent::KEYCODE_UP) keyUp = isKeyDown;
		if (event.getKeyCode() == GUIKeyboardEvent::KEYCODE_DOWN) keyDown = isKeyDown;
		if (event.getKeyCode() == GUIKeyboardEvent::KEYCODE_BACKSPACE) keyDelete = isKeyDown;
		if (Character::toLowerCase(event.getKeyChar()) == 24) keyControlX = isKeyDown;
		if (Character::toLowerCase(event.getKeyChar()) == 3) keyControlC = isKeyDown;
		if (Character::toLowerCase(event.getKeyChar()) == 22) keyControlV = isKeyDown;
		if (Character::toLowerCase(event.getKeyChar()) == 'x' && keyControl == true) keyControlX = isKeyDown;
		if (Character::toLowerCase(event.getKeyChar()) == 'c' && keyControl == true) keyControlC = isKeyDown;
		if (Character::toLowerCase(event.getKeyChar()) == 'v' && keyControl == true) keyControlV = isKeyDown;
		if (Character::toLowerCase(event.getKeyChar()) == 'a') keyA = isKeyDown;
		if (Character::toLowerCase(event.getKeyChar()) == 'd') keyD = isKeyDown;
		if (Character::toLowerCase(event.getKeyChar()) == 'w') keyW = isKeyDown;
		if (Character::toLowerCase(event.getKeyChar()) == 's') keyS = isKeyDown;
		if (Character::toLowerCase(event.getKeyChar()) == '+') keyPlus = isKeyDown;
		if (Character::toLowerCase(event.getKeyChar()) == '-') keyMinus = isKeyDown;
		if (Character::toLowerCase(event.getKeyChar()) == 'r') keyR = isKeyDown;
		if (Character::toLowerCase(event.getKeyChar()) == '.' && !isKeyDown == false) placeEntityYRotation = (placeEntityYRotation + 1) % 4;
		if (Character::toLowerCase(event.getKeyChar()) == ',' && !isKeyDown == false) placeEntityYRotation = (placeEntityYRotation + 3) % 4;
		if (Character::toLowerCase(event.getKeyChar()) == '1' && isKeyDown == true) { setGizmoType(GIZMOTYPE_ALL); updateGizmo(); }
		if (Character::toLowerCase(event.getKeyChar()) == '2' && isKeyDown == true) { setGizmoType(GIZMOTYPE_TRANSLATE); updateGizmo(); }
		if (Character::toLowerCase(event.getKeyChar()) == '3' && isKeyDown == true) { setGizmoType(GIZMOTYPE_ROTATE); updateGizmo(); }
		if (Character::toLowerCase(event.getKeyChar()) == '4' && isKeyDown == true) { setGizmoType(GIZMOTYPE_SCALE); updateGizmo(); }
	}
	for (auto i = 0; i < engine->getGUI()->getMouseEvents().size(); i++) {
		auto& event = engine->getGUI()->getMouseEvents()[i];

		if ((event.getType() == GUIMouseEvent_Type::MOUSEEVENT_MOVED ||
			event.getType() == GUIMouseEvent_Type::MOUSEEVENT_DRAGGED) &&
			event.getXUnscaled() >= 0 &&
			event.getYUnscaled() >= 0) {
			placeEntityMouseX = event.getXUnscaled();
			placeEntityMouseY = event.getYUnscaled();
		}

		if (event.isProcessed() == true) continue;

		if (event.getType() == GUIMouseEvent_Type::MOUSEEVENT_DRAGGED) {
			if (mouseDragging == false) {
				mouseDragging = true;
				mouseDownLastX = event.getXUnscaled();
				mouseDownLastY = event.getYUnscaled();
				mouseDraggingLastObject = nullptr;
			}
		} else {
			if (mouseDragging == true) {
				mouseDownLastX = MOUSE_DOWN_LAST_POSITION_NONE;
				mouseDownLastY = MOUSE_DOWN_LAST_POSITION_NONE;
				mouseDragging = false;
				mouseDraggingLastObject = nullptr;
			}
		}
		if (event.getButton() == MOUSE_BUTTON_LEFT) {
			if (event.getType() == GUIMouseEvent_Type::MOUSEEVENT_RELEASED) {
				if (pasteMode == true && pasteModeValid == true) {
					pasteObjects(false);
					if (keyShift == false) unsetPasteMode();
				} else
				if (placeEntityMode == true && placeEntityValid == true) {
					placeObject();
					if (keyShift == false) unsetPlaceObjectMode();
				} else {
					setGizmoMode(GIZMOMODE_NONE);
				}
			} else {
				Group* selectedEntityGroup = nullptr;
				Entity* selectedEntity = nullptr;
				if (getGizmoMode() == GIZMOMODE_NONE) selectedEntity = engine->getEntityByMousePosition(event.getXUnscaled(), event.getYUnscaled(), entityPickingFilterNoGrid, &selectedEntityGroup);
				if (mouseDragging == true) {
					Vector3 deltaTranslation;
					Vector3 deltaRotation;
					Vector3 deltaScale;
					if (determineGizmoDeltaTransformations(mouseDownLastX, mouseDownLastY, event.getXUnscaled(), event.getYUnscaled(), deltaTranslation, deltaRotation, deltaScale) == true) {
						auto gizmoEntity = getGizmoObject3D();
						if (gizmoEntity != nullptr) {
							Transformations rotations;
							rotations.addRotation(level->getRotationOrder()->getAxis0(), deltaRotation[level->getRotationOrder()->getAxis0VectorIndex()]);
							rotations.addRotation(level->getRotationOrder()->getAxis1(), deltaRotation[level->getRotationOrder()->getAxis1VectorIndex()]);
							rotations.addRotation(level->getRotationOrder()->getAxis2(), deltaRotation[level->getRotationOrder()->getAxis2VectorIndex()]);
							rotations.update();
							for (auto selectedEntityId: selectedEntityIds) {
								auto _selectedEntity = engine->getEntity(selectedEntityId);
								if (_selectedEntity != nullptr && StringTools::startsWith(_selectedEntity->getId(), "tdme.leveleditor.") == false) {
									auto levelEditorObject = level->getObjectById(_selectedEntity->getId());
									if (levelEditorObject == nullptr) continue;
									auto translation = levelEditorObject->getTransformations().getTranslation();
									auto translationRelative = translation.clone().sub(gizmoEntity->getTranslation());
									translation = gizmoEntity->getTranslation().clone().add(rotations.getRotationsQuaternion().multiply(translationRelative, translationRelative));
									levelEditorObject->getTransformations().setTranslation(translation.clone().add(deltaTranslation));
									auto scale = levelEditorObject->getTransformations().getScale().clone().scale(deltaScale);
									if (Math::abs(scale.getX()) < 0.01f) scale.setX(Math::sign(scale.getX()) * 0.01f);
									if (Math::abs(scale.getY()) < 0.01f) scale.setY(Math::sign(scale.getY()) * 0.01f);
									if (Math::abs(scale.getZ()) < 0.01f) scale.setZ(Math::sign(scale.getZ()) * 0.01f);
									if (Math::abs(scale.getX()) > 100.0f) scale.setX(Math::sign(scale.getX()) * 100.0f);
									if (Math::abs(scale.getY()) > 100.0f) scale.setY(Math::sign(scale.getY()) * 100.0f);
									if (Math::abs(scale.getZ()) > 100.0f) scale.setZ(Math::sign(scale.getZ()) * 100.0f);
									levelEditorObject->getTransformations().setScale(scale);
									levelEditorObject->getTransformations().setRotationAngle(level->getRotationOrder()->getAxisXIndex(), levelEditorObject->getTransformations().getRotationAngle(level->getRotationOrder()->getAxisXIndex()) + deltaRotation[0]);
									levelEditorObject->getTransformations().setRotationAngle(level->getRotationOrder()->getAxisYIndex(), levelEditorObject->getTransformations().getRotationAngle(level->getRotationOrder()->getAxisYIndex()) + deltaRotation[1]);
									levelEditorObject->getTransformations().setRotationAngle(level->getRotationOrder()->getAxisZIndex(), levelEditorObject->getTransformations().getRotationAngle(level->getRotationOrder()->getAxisZIndex()) + deltaRotation[2]);
									levelEditorObject->getTransformations().update();
									_selectedEntity->fromTransformations(levelEditorObject->getTransformations());
								}
							}
							if (selectedEntityIds.size() == 1) {
								auto _selectedEntity = engine->getEntity(selectedEntityIds[0]);
								levelEditorScreenController->setObject(
									_selectedEntity->getTranslation(),
									_selectedEntity->getScale(),
									_selectedEntity->getRotationAngle(level->getRotationOrder()->getAxisXIndex()),
									_selectedEntity->getRotationAngle(level->getRotationOrder()->getAxisYIndex()),
									_selectedEntity->getRotationAngle(level->getRotationOrder()->getAxisZIndex())
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
				if (determineGizmoMode(selectedEntity, selectedEntityGroup) == true) {
					// no op
					if (selectedEntityIds.size() == 1) {
						for (auto selectedEntityId: selectedEntityIds) {
							auto _selectedEntity = engine->getEntity(selectedEntityId);
							if (_selectedEntity != nullptr && StringTools::startsWith(_selectedEntity->getId(), "tdme.leveleditor.") == false) {
								setGizmoRotation(_selectedEntity->getTransformations());
							}
						}
					} else
					if (selectedEntityIds.size() > 1) {
						setGizmoRotation(Transformations());
					}
					updateGUITransformationsElements();
				} else {
					if (keyControl == false) {
						vector<Entity*> entitiesToRemove;
						for (auto selectedEntityId: selectedEntityIds) {
							auto selectedEntity = engine->getEntity(selectedEntityId);
							if (mouseDragging == true && mouseDraggingLastObject == selectedEntity) {
								// no op
							} else {
								if (selectedEntity != nullptr) entitiesToRemove.push_back(selectedEntity);
							}
						}
						for (auto entityToRemove: entitiesToRemove) {
							setStandardObjectColorEffect(entityToRemove);
							selectedEntityIds.erase(remove(selectedEntityIds.begin(), selectedEntityIds.end(), entityToRemove->getId()), selectedEntityIds.end());
							auto selectedEntitiyIdByIdIt = selectedEntityIdsById.find(entityToRemove->getId());
							if (selectedEntitiyIdByIdIt != selectedEntityIdsById.end()) {
								selectedEntityIdsById.erase(selectedEntitiyIdByIdIt);
							}
							levelEditorScreenController->unselectObjectInObjectListBox(entityToRemove->getId());
						}
					}
					if (selectedEntity != nullptr) {
						if (selectedEntityIdsById.find(selectedEntity->getId()) == selectedEntityIdsById.end()) {
							setStandardObjectColorEffect(selectedEntity);
							setHighlightObjectColorEffect(selectedEntity);
							selectedEntityIds.push_back(selectedEntity->getId());
							selectedEntityIdsById.insert(selectedEntity->getId());
							levelEditorScreenController->selectObjectInObjectListbox(selectedEntity->getId());
							auto levelEditorObject = level->getObjectById(selectedEntity->getId());
							if (levelEditorObject != nullptr) {
								TDMELevelEditor::getInstance()->getLevelEditorEntityLibraryScreenController()->selectEntity(levelEditorObject->getEntity()->getId());
							}
						} else {
							setStandardObjectColorEffect(selectedEntity);
							selectedEntityIds.erase(remove(selectedEntityIds.begin(), selectedEntityIds.end(), selectedEntity->getId()), selectedEntityIds.end());
							auto selectedEntityIdsByIdIt = selectedEntityIdsById.find(selectedEntity->getId());
							if (selectedEntityIdsByIdIt != selectedEntityIdsById.end()) {
								selectedEntityIdsById.erase(selectedEntityIdsByIdIt);
							}
							levelEditorScreenController->unselectObjectInObjectListBox(selectedEntity->getId());
						}
						if (selectedEntityIds.size() == 1) setGizmoRotation(selectedEntity->getTransformations());
						if (selectedEntityIds.size() > 1) setGizmoRotation(Transformations());
					}
					mouseDraggingLastObject = selectedEntity;
					updateGizmo();
					updateGUIElements();
				}
			}
		} else
		if (event.getButton() == MOUSE_BUTTON_RIGHT && mouseDragging == true) {
			if (mouseDownLastX != MOUSE_DOWN_LAST_POSITION_NONE && mouseDownLastY != MOUSE_DOWN_LAST_POSITION_NONE) {
				mousePanningSide = event.getXUnscaled() - mouseDownLastX;
				mousePanningForward = event.getYUnscaled() - mouseDownLastY;
			}
		} else
		if (event.getButton() == MOUSE_BUTTON_MIDDLE) {
			centerObject();
			if (mouseDownLastX != MOUSE_DOWN_LAST_POSITION_NONE && mouseDownLastY != MOUSE_DOWN_LAST_POSITION_NONE) {
				mouseRotationX = event.getXUnscaled() - mouseDownLastX;
				mouseRotationY = event.getYUnscaled() - mouseDownLastY;
			}
		}
		if (event.getButton() != MOUSE_BUTTON_NONE) {
			mouseDownLastX = event.getXUnscaled();
			mouseDownLastY = event.getYUnscaled();
		}
		auto mouseWheel = event.getWheelY();
		if (mouseWheel != 0) {
			camScale += -mouseWheel * 0.1f;
			if (camScale < camScaleMin) camScale = camScaleMin;
			if (camScale > camScaleMax) camScale = camScaleMax;
			updateGizmo();
		}
	}
	if (keyDelete == true) {
		Console::println("LevelEditorView::handleInputEvents(): Backspace");
		removeGizmo();
		removeObjects();
	}
	if (keyControlX == true) {
		Console::println("LevelEditorView::handleInputEvents(): CTRL-X");
		removeGizmo();
		copyObjects();
		removeObjects();
	}
	if (keyControlC == true) {
		Console::println("LevelEditorView::handleInputEvents(): CTRL-C");
		copyObjects();
	}
	if (keyControlV == true) {
		Console::println("LevelEditorView::handleInputEvents(): CTRL-V");
		removeGizmo();
		setPasteMode();
	}
}

void LevelEditorView::display()
{
	auto cam = engine->getCamera();
	if (reloadEntityLibrary == true) {
		auto entityLibrary = TDMELevelEditor::getInstance()->getEntityLibrary();
		for (auto i = 0; i < entityLibrary->getEntityCount(); i++) {
			selectedEntity = entityLibrary->getEntityAt(i);
			Tools::oseThumbnail(selectedEntity);
		}
		reloadEntityLibrary = false;
		TDMELevelEditor::getInstance()->getLevelEditorEntityLibraryScreenController()->setEntityLibrary();
	}

	if ((placeEntityMode == true || pasteMode == true) && keyEscape == true) {
		unsetPlaceObjectMode();
		unsetPasteMode();
		keyEscape = false;
	}

	{
		auto selectedEngineEntity = engine->getEntity("tdme.leveleditor.placeentity");
		Vector3 worldCoordinate;
		placeEntityValid = false;
		pasteModeValid = false;
		if ((placeEntityMode == true || pasteMode == true) && engine->getEntityByMousePosition(placeEntityMouseX, placeEntityMouseY, worldCoordinate, entityPickingFilterPlacing) != nullptr) {
			if (placeEntityMode == true) {
				Transformations transformations;
				transformations.setTranslation(worldCoordinate);
				transformations.addRotation(level->getRotationOrder()->getAxis0(), 0.0f);
				transformations.addRotation(level->getRotationOrder()->getAxis1(), 0.0f);
				transformations.addRotation(level->getRotationOrder()->getAxis2(), 0.0f);
				transformations.update();
				if (selectedEngineEntity == nullptr && selectedEntity != nullptr) {
					selectedEngineEntity = Level::createEntity(selectedEntity, "tdme.leveleditor.placeentity", transformations);
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
					transformations.setRotationAngle(level->getRotationOrder()->getAxisYIndex(), static_cast<float>(placeEntityYRotation) * 90.0f);
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
				pasteObjects(true);
			}
		}
	}

	if (mouseRotationX != MOUSE_ROTATION_NONE) {
		camLookRotationY->setAngle(camLookRotationY->getAngle() + mouseRotationX);
		camLookRotationY->update();
		mouseRotationX = 0;
	}
	if (mouseRotationY != MOUSE_ROTATION_NONE) {
		camLookRotationX->setAngle(camLookRotationX->getAngle() + mouseRotationY);
		camLookRotationX->update();
		mouseRotationY = 0;
	}
	if (keyA == true) camLookRotationY->setAngle(camLookRotationY->getAngle() + 1.0f);
	if (keyD == true) camLookRotationY->setAngle(camLookRotationY->getAngle() - 1.0f);
	if (keyW == true) camLookRotationX->setAngle(camLookRotationX->getAngle() + 1.0f);
	if (keyS == true) camLookRotationX->setAngle(camLookRotationX->getAngle() - 1.0f);
	if (keyMinus == true || keyPlus == true) {
		if (keyMinus == true) camScale += 0.05f;
		if (keyPlus == true) camScale -= 0.05f;
		if (camScale < camScaleMin) camScale = camScaleMin;
		if (camScale > camScaleMax) camScale = camScaleMax;
		updateGizmo();
	}

	if (keyR == true) {
		camLookRotationX->setAngle(-45.0f);
		camLookRotationX->update();
		camLookRotationY->setAngle(0.0f);
		camLookRotationY->update();
		cam->setLookAt(level->getCenter());
		camScale = 1.0f;
	}
	if (keyA == true|| keyD == true) camLookRotationY->update();

	if (keyW == true || keyS == true) {
		if (camLookRotationX->getAngle() > 89.99f) camLookRotationX->setAngle(89.99f);
		if (camLookRotationX->getAngle() < -89.99f) camLookRotationX->setAngle(-89.99f);
		camLookRotationX->update();
	}

	Vector3 tmpVector3;
	Vector3 FORWARD_VECTOR(0.0f, 0.0f, 1.0f);
	Vector3 SIDE_VECTOR(1.0f, 0.0f, 0.0f);
	Vector3 camLookAtToFromVector;
	Vector3 camForwardVector;
	Vector3 camSideVector;
	camLookRotationX->getQuaternion().multiply(FORWARD_VECTOR, tmpVector3);
	camLookRotationY->getQuaternion().multiply(tmpVector3, tmpVector3);
	camLookAtToFromVector.set(tmpVector3).scale(camScale * 10.0f);
	auto timing = engine->getTiming();
	camLookRotationY->getQuaternion().multiply(FORWARD_VECTOR, camForwardVector).scale(timing->getDeltaTime() / 1000.0f * 60.0f);
	camLookRotationY->getQuaternion().multiply(SIDE_VECTOR, camSideVector).scale(timing->getDeltaTime() / 1000.0f * 60.0f);

	auto camLookAt = cam->getLookAt();
	if (keyUp == true) camLookAt.sub(tmpVector3.set(camForwardVector).scale(0.1f));
	if (keyDown == true) camLookAt.add(tmpVector3.set(camForwardVector).scale(0.1f));
	if (keyLeft == true) camLookAt.sub(tmpVector3.set(camSideVector).scale(0.1f));
	if (keyRight == true) camLookAt.add(tmpVector3.set(camSideVector).scale(0.1f));
	if (mousePanningForward != MOUSE_PANNING_NONE) {
		camLookAt.sub(tmpVector3.set(camForwardVector).scale(mousePanningForward / 30.0f * camScale));
		mousePanningForward = MOUSE_PANNING_NONE;
	}
	if (mousePanningSide != MOUSE_PANNING_NONE) {
		camLookAt.sub(tmpVector3.set(camSideVector).scale(mousePanningSide / 30.0f * camScale));
		mousePanningSide = MOUSE_PANNING_NONE;
	}
	cam->setLookAt(camLookAt);
	cam->setLookFrom(cam->getLookAt().clone().add(camLookAtToFromVector));
	cam->setUpVector(cam->computeUpVector(cam->getLookFrom(), cam->getLookAt()));
	gridCenter.set(cam->getLookAt());
	updateGrid();

	// viewport
	auto xScale = (float)engine->getWidth() / (float)levelEditorScreenController->getScreenNode()->getScreenWidth();
	auto yScale = (float)engine->getHeight() / (float)levelEditorScreenController->getScreenNode()->getScreenHeight();
	auto viewPortLeft = 0;
	auto viewPortTop = 0;
	auto viewPortWidth = 0;
	auto viewPortHeight = 0;
	levelEditorScreenController->getViewPort(viewPortLeft, viewPortTop, viewPortWidth, viewPortHeight);
	viewPortLeft = (int)((float)viewPortLeft * xScale);
	viewPortTop = (int)((float)viewPortTop * yScale);
	viewPortWidth = (int)((float)viewPortWidth * xScale);
	viewPortHeight = (int)((float)viewPortHeight * yScale);
	engine->getCamera()->enableViewPort(viewPortLeft, viewPortTop, viewPortWidth, viewPortHeight);

	// rendering
	engine->getGUI()->handleEvents();
	engine->getGUI()->render();
}

void LevelEditorView::selectObjects(const vector<string>& entityIds)
{
	removeGizmo();
	for (auto entityIdToRemove: selectedEntityIds) {
		auto entityToRemove = engine->getEntity(entityIdToRemove);
		if (entityToRemove != nullptr) setStandardObjectColorEffect(entityToRemove);
	}
	selectedEntityIds.clear();
	selectedEntityIdsById.clear();
	for (auto entityId: entityIds) {
		auto selectedEntity = engine->getEntity(entityId);
		if (selectedEntity == nullptr) continue;
		setStandardObjectColorEffect(selectedEntity);
		setHighlightObjectColorEffect(selectedEntity);
		selectedEntityIds.push_back(entityId);
		selectedEntityIdsById.insert(entityId);
	}
	updateGUIElements();
	updateGizmo();
}

void LevelEditorView::unselectObjects()
{
	removeGizmo();
	for (auto entityIdToRemove: selectedEntityIds) {
		auto entityToRemove = engine->getEntity(entityIdToRemove);
		if (entityToRemove != nullptr) setStandardObjectColorEffect(entityToRemove);
	}
	selectedEntityIds.clear();
	selectedEntityIdsById.clear();
	levelEditorScreenController->unselectObjectsInObjectListBox();
	updateGUIElements();
	removeGizmo();
}

void LevelEditorView::updateGUIElements()
{
	levelEditorScreenController->setScreenCaption("Level Editor - " + Tools::getFileName(level->getFileName()));
	levelEditorScreenController->setLevelSize(level->getDimension().getX(), level->getDimension().getZ(), level->getDimension().getY());
	if (selectedEntityIds.size() == 1) {
		auto selectedEntity = engine->getEntity(selectedEntityIds[0]);
		if (selectedEntity != nullptr && StringTools::startsWith(selectedEntity->getId(), "tdme.leveleditor.") == false) {
			auto levelEditorObject = level->getObjectById(selectedEntity->getId());
			auto preset = levelEditorObject->getProperty("preset");
			levelEditorScreenController->setObjectProperties(preset != nullptr ? preset->getValue() : "", levelEditorObject, "");
		} else {
			levelEditorScreenController->unsetObjectData();
			levelEditorScreenController->unsetObject();
			levelEditorScreenController->unsetObjectProperties();
		}
	} else
	if (selectedEntityIds.size() > 1) {
		levelEditorScreenController->unsetObjectData();
		levelEditorScreenController->unsetObjectProperties();
	} else
	if (selectedEntityIds.size() == 0) {
		levelEditorScreenController->unsetObject();
		levelEditorScreenController->unsetObjectData();
		levelEditorScreenController->unsetObjectProperties();
	}
	for (auto i = 0; i < 4; i++) {
		levelEditorScreenController->setLight(i, level->getLightAt(i)->getAmbient(), level->getLightAt(i)->getDiffuse(), level->getLightAt(i)->getSpecular(), level->getLightAt(i)->getPosition(), level->getLightAt(i)->getConstantAttenuation(), level->getLightAt(i)->getLinearAttenuation(), level->getLightAt(i)->getQuadraticAttenuation(), level->getLightAt(i)->getSpotTo(), level->getLightAt(i)->getSpotDirection(), level->getLightAt(i)->getSpotExponent(), level->getLightAt(i)->getSpotCutOff(), level->getLightAt(i)->isEnabled());
	}
	updateGUITransformationsElements();
}

void LevelEditorView::updateGUITransformationsElements() {
	if (selectedEntityIds.size() == 1) {
		auto selectedEntity = engine->getEntity(selectedEntityIds[0]);
		if (selectedEntity != nullptr && StringTools::startsWith(selectedEntity->getId(), "tdme.leveleditor.") == false) {
			auto levelEditorObject = level->getObjectById(selectedEntity->getId());
			levelEditorScreenController->setObject(
				selectedEntity->getTranslation(),
				selectedEntity->getScale(),
				selectedEntity->getRotationAngle(level->getRotationOrder()->getAxisXIndex()),
				selectedEntity->getRotationAngle(level->getRotationOrder()->getAxisYIndex()),
				selectedEntity->getRotationAngle(level->getRotationOrder()->getAxisZIndex())
			);
			Vector3 objectCenter;
			if (levelEditorObject->getEntity()->getModel() != nullptr) {
				BoundingBox bv;
				bv.fromBoundingVolumeWithTransformations(levelEditorObject->getEntity()->getModel()->getBoundingBox(), levelEditorObject->getTransformations());
				objectCenter = bv.getCenter();
			} else {
				objectCenter = levelEditorObject->getTransformations().getTranslation();
			}
			levelEditorScreenController->setObjectData(levelEditorObject->getId(), levelEditorObject->getDescription(), levelEditorObject->getEntity()->getName(), objectCenter);
		}
	} else
	if (selectedEntityIds.size() > 1) {
		levelEditorScreenController->setObject(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), 0.0f, 0.0f, 0.0f);
	}
}

void LevelEditorView::setObjectsListBox()
{
	levelEditorScreenController->setObjectListbox(level);
}

void LevelEditorView::unselectLightPresets()
{
	levelEditorScreenController->unselectLightPresets();
}

void LevelEditorView::loadSettings()
{
	try {
		Properties settings;
		settings.load("settings", "leveleditor.properties");
		gridEnabled = settings.get("grid.enabled", "false") == "true";
		gridY = Float::parseFloat(settings.get("grid.y", "0.0"));
		snappingEnabled = settings.get("snapping.enabled", "false") == "true";
		snappingX = Float::parseFloat(settings.get("snapping.x", "1.0"));
		snappingZ = Float::parseFloat(settings.get("snapping.z", "1.0"));
		levelEditorScreenController->getMapPath()->setPath(settings.get("map.path", "."));
		TDMELevelEditor::getInstance()->getLevelEditorEntityLibraryScreenController()->setModelPath(settings.get("model.path", "."));
	} catch (Exception& exception) {
		Console::print(string("LevelEditorView::loadSettings(): An error occurred: "));
		Console::println(string(exception.what()));
	}
}

void LevelEditorView::initialize()
{
	try {
		levelEditorScreenController = new LevelEditorScreenController(this);
		levelEditorScreenController->initialize();
		levelEditorScreenController->getScreenNode()->setInputEventHandler(this);
		engine->getGUI()->addScreen(levelEditorScreenController->getScreenNode()->getId(), levelEditorScreenController->getScreenNode());
	} catch (Exception& exception) {
		Console::print(string("LevelEditorView::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	loadSettings();
	levelEditorScreenController->setGrid(gridEnabled, gridY);
	levelEditorScreenController->setSnapping(snappingEnabled, snappingX, snappingZ);
	levelEditorScreenController->setMapProperties(level, "");
	levelEditorScreenController->setObjectPresetIds(LevelPropertyPresets::getInstance()->getObjectPropertiesPresets());
	levelEditorScreenController->setLightPresetsIds(LevelPropertyPresets::getInstance()->getLightPresets());
	updateGUIElements();
	auto light0 = engine->getLightAt(0);
	light0->setAmbient(Color4(1.0f, 1.0f, 1.0f, 1.0f));
	light0->setDiffuse(Color4(1.0f, 1.0f, 1.0f, 1.0f));
	light0->setPosition(Vector4(0.0f, 20.0f, 0.0f, 1.0f));
	light0->setEnabled(true);
	auto cam = engine->getCamera();
	cam->setZNear(0.1f);
	cam->setZFar(150.0f);
	cam->setLookAt(level->getCenter());
	gridCenter.set(cam->getLookAt());
	camLookAt.set(engine->getCamera()->getLookAt());
}

void LevelEditorView::activate()
{
	engine->reset();
	engine->setPartition(new PartitionOctTree());
	engine->getGUI()->resetRenderScreens();
	engine->getGUI()->addRenderScreen(levelEditorScreenController->getScreenNode()->getId());
	engine->getGUI()->addRenderScreen(TDMELevelEditor::getInstance()->getLevelEditorEntityLibraryScreenController()->getScreenNode()->getId());
	engine->getGUI()->addRenderScreen(popUps->getFileDialogScreenController()->getScreenNode()->getId());
	engine->getGUI()->addRenderScreen(popUps->getInfoDialogScreenController()->getScreenNode()->getId());
	engine->getGUI()->addRenderScreen(popUps->getProgressBarScreenController()->getScreenNode()->getId());
	TDMELevelEditor::getInstance()->getLevelEditorEntityLibraryScreenController()->setEntityLibrary();
	loadLevel();
	auto cam = engine->getCamera();
	cam->setZNear(0.1f);
	cam->setZFar(150.0f);
	cam->setLookAt(camLookAt);
}

void LevelEditorView::deactivate()
{
	camLookAt.set(engine->getCamera()->getLookAt());
}

void LevelEditorView::storeSettings()
{
	try {
		Properties settings;
		settings.put("grid.enabled", gridEnabled == true ? "true" : "false");
		settings.put("grid.y", to_string(gridY));
		settings.put("snapping.enabled", snappingEnabled == true ? "true" : "false");
		settings.put("snapping.x", to_string(snappingX));
		settings.put("snapping.z", to_string(snappingZ));
		settings.put("map.path", levelEditorScreenController->getMapPath()->getPath());
		settings.put("model.path", TDMELevelEditor::getInstance()->getLevelEditorEntityLibraryScreenController()->getModelPath());
		settings.store("settings", "leveleditor.properties");
	} catch (Exception& exception) {
		Console::print(string("LevelEditorView::storeSettings(): An error occurred: "));
		Console::println(string(exception.what()));
	}
}

void LevelEditorView::dispose()
{
	Engine::getInstance()->reset();
	storeSettings();
}

void LevelEditorView::setHighlightObjectColorEffect(Entity* object)
{
	auto red = objectColors["red"];
	object->setEffectColorAdd(Color4(red->colorAddR, red->colorAddG, red->colorAddB, 0.0f));
	object->setEffectColorMul(Color4(red->colorMulR, red->colorMulG, red->colorMulB, 1.0f));
}

void LevelEditorView::setStandardObjectColorEffect(Entity* object)
{
	auto color = objectColors["none"];
	object->setEffectColorAdd(Color4(color->colorAddR, color->colorAddG, color->colorAddB, 0.0f));
	object->setEffectColorMul(Color4(color->colorMulR, color->colorMulG, color->colorMulB, 1.0f));
	auto levelEditorObject = level->getObjectById(object->getId());
	if (levelEditorObject == nullptr) return;
	auto colorProperty = levelEditorObject->getProperty("object.color");
	if (colorProperty == nullptr) colorProperty = levelEditorObject->getEntity()->getProperty("object.color");
	if (colorProperty != nullptr) {
		auto objectColorIt = objectColors.find(colorProperty->getValue());
		auto objectColor = objectColorIt != objectColors.end() ? objectColorIt->second : nullptr;
		if (objectColor != nullptr) {
			object->setEffectColorAdd(Color4(object->getEffectColorAdd().getRed() + objectColor->colorAddR, object->getEffectColorAdd().getGreen() + objectColor->colorAddG, object->getEffectColorAdd().getBlue() + objectColor->colorAddB, 0.0f));
			object->setEffectColorMul(Color4(object->getEffectColorMul().getRed() * objectColor->colorMulR, object->getEffectColorMul().getGreen() * objectColor->colorMulG, object->getEffectColorMul().getBlue() * objectColor->colorMulB, 1.0f));
		}
	}
}

void LevelEditorView::loadLevel()
{
	removeGizmo();
	removeGrid();
	engine->reset();
	selectedEntityIds.clear();
	selectedEntityIdsById.clear();
	Level::setLight(engine, level, Vector3());
	Level::addLevel(engine, level, true, true, true);
	setObjectsListBox();
	unselectLightPresets();
	updateGrid();
}

void LevelEditorView::updateGrid()
{
	if (gridEnabled == false) return;

	string entityId = "tdme.leveleditor.grid";
	auto entity = engine->getEntity(entityId);
	if (entity == nullptr) {
		entity = new Object3D(entityId, levelEditorGround);
		entity->setFrustumCulling(false);
		entity->addRotation(level->getRotationOrder()->getAxis0(), 0.0f);
		entity->addRotation(level->getRotationOrder()->getAxis1(), 0.0f);
		entity->addRotation(level->getRotationOrder()->getAxis2(), 0.0f);
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
			setHighlightObjectColorEffect(entity);
		} else {
			setStandardObjectColorEffect(entity);
		}
		engine->addEntity(entity);
	}
}

void LevelEditorView::removeGrid()
{
	engine->removeEntity("tdme.leveleditor.grid");
}

Model* LevelEditorView::createLevelEditorGroundPlateModel()
{
	auto groundPlate = new Model("tdme.leveleditor.grid", "tdme.leveleditor.grid", UpVector::Y_UP, RotationOrder::XYZ, new BoundingBox(Vector3(0.0f, -0.01f, 0.0f), Vector3(10000.0f, +0.01f, 10000.0f)));
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
	groundPlateMaterial->getSpecularMaterialProperties()->setDiffuseTexture("resources/engine/tools/leveleditor/textures", "groundplate.png");
	groundPlateMaterial->getSpecularMaterialProperties()->setSpecularColor(Color4(0.0f, 0.0f, 0.0f, 1.0f));
	groundPlate->getMaterials()["ground"] = groundPlateMaterial;
	auto groundGroup = new Group(groundPlate, nullptr, "ground", "ground");
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
	groundFacesGround.push_back(Face(groundGroup, 0, 1, 2, 0, 0, 0, 0, 1, 2));
	groundFacesGround.push_back(Face(groundGroup, 2, 3, 0, 0, 0, 0, 2, 3, 0));
	FacesEntity groupFacesEntityGround(groundGroup, "tdme.leveleditor.grid.facesentity");
	groupFacesEntityGround.setMaterial(groundPlateMaterial);
	groupFacesEntityGround.setFaces(groundFacesGround);
	vector<FacesEntity> groupFacesEntities;
	groupFacesEntities.push_back(groupFacesEntityGround);
	groundGroup->setVertices(groundVertices);
	groundGroup->setNormals(groundNormals);
	groundGroup->setTextureCoordinates(groundTextureCoordinates);
	groundGroup->setFacesEntities(groupFacesEntities);
	groundPlate->getGroups()[groundGroup->getId()] = groundGroup;
	groundPlate->getSubGroups()[groundGroup->getId()] = groundGroup;
	ModelTools::prepareForIndexedRendering(groundPlate);
	return groundPlate;
}

bool LevelEditorView::objectDataApply(const string& name, const string& description)
{
	if (selectedEntityIds.size() != 1) return false;

	auto selectedEntity = engine->getEntity(selectedEntityIds[0]);
	if (selectedEntity == nullptr || StringTools::startsWith(selectedEntity->getId(), "tdme.leveleditor.")) return false;

	auto levelEditorObject = level->getObjectById(selectedEntity->getId());
	if (levelEditorObject == nullptr) return false;

	levelEditorObject->setDescription(description);
	if (levelEditorObject->getId() != name) {
		if (engine->getEntity(name) != nullptr) return false;
		auto oldId = levelEditorObject->getId();
		level->removeObject(levelEditorObject->getId());
		engine->removeEntity(levelEditorObject->getId());
		selectedEntityIds.clear();
		selectedEntityIdsById.clear();
		levelEditorObject->setId(name);
		level->addObject(levelEditorObject);
		auto entity = Level::createEntity(levelEditorObject);
		if (entity != nullptr) {
			setHighlightObjectColorEffect(entity);
			selectedEntityIds.push_back(entity->getId());
			selectedEntityIdsById.insert(entity->getId());
			levelEditorScreenController->setObjectListbox(level);
			entity->setPickable(true);
			engine->addEntity(entity);
		}
	}
	levelEditorObject->setDescription(description);
	return true;
}

void LevelEditorView::setPlaceObjectMode() {
	placeEntityMode = true;
	placeEntityValid = false;
}

void LevelEditorView::unsetPlaceObjectMode() {
	placeEntityMode = false;
	placeEntityValid = false;
	engine->removeEntity("tdme.leveleditor.placeentity");
}

void LevelEditorView::placeObject()
{
	Transformations levelEditorObjectTransformations;
	levelEditorObjectTransformations.setTranslation(placeEntityTranslation);
	levelEditorObjectTransformations.setScale(Vector3(1.0f, 1.0f, 1.0f));
	levelEditorObjectTransformations.setPivot(selectedEntity->getPivot());
	levelEditorObjectTransformations.addRotation(level->getRotationOrder()->getAxis0(), 0.0f);
	levelEditorObjectTransformations.addRotation(level->getRotationOrder()->getAxis1(), 0.0f);
	levelEditorObjectTransformations.addRotation(level->getRotationOrder()->getAxis2(), 0.0f);
	levelEditorObjectTransformations.setRotationAngle(level->getRotationOrder()->getAxisYIndex(), placeEntityYRotation * 90.0f);
	levelEditorObjectTransformations.update();
	for (auto i = 0; i < level->getObjectCount(); i++) {
		auto levelEditorObject = level->getObjectAt(i);
		if (levelEditorObject->getEntity() == selectedEntity && levelEditorObject->getTransformations().getTranslation().equals(levelEditorObjectTransformations.getTranslation())) {
			return;
		}
	}
	auto levelEditorObject = new LevelEditorObject(
		selectedEntity->getName() + "_" + to_string(level->allocateObjectId()),
		"",
		levelEditorObjectTransformations,
		selectedEntity
	);
	level->addObject(levelEditorObject);
	auto entity = Level::createEntity(levelEditorObject);
	if (entity != nullptr) {
		setStandardObjectColorEffect(entity);
		entity->setPickable(true);
		engine->addEntity(entity);
	}
	levelEditorScreenController->setObjectListbox(level);
	level->update();
	updateGUIElements();
}

void LevelEditorView::removeObjects()
{
	removeGizmo();
	vector<Entity*> entitiesToRemove;
	for (auto selectedEntityId: selectedEntityIds) {
		Entity* selectedEntity = engine->getEntity(selectedEntityId);
		if (selectedEntity != nullptr && StringTools::startsWith(selectedEntity->getId(), "tdme.leveleditor.") == false) {
			level->removeObject(selectedEntity->getId());
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
	level->update();
	levelEditorScreenController->setObjectListbox(level);
	updateGUIElements();
}

void LevelEditorView::colorObject()
{
	if (selectedEntityIds.size() == 0)
		return;

	for (auto selectedEntityId: selectedEntityIds) {
		auto selectedEntity = engine->getEntity(selectedEntityId);
		auto levelEditorObject = level->getObjectById(selectedEntityId);
		if (levelEditorObject == nullptr) continue;
		auto color = OBJECTCOLOR_NAMES[0];
		auto colorProperty = levelEditorObject->getProperty("object.color");
		if (colorProperty == nullptr) {
			levelEditorObject->addProperty("object.color", color);
		} else {
			color = colorProperty->getValue();
			for (auto i = 0; i < OBJECTCOLOR_NAMES.size(); i++) {
				if (StringTools::equalsIgnoreCase(color, OBJECTCOLOR_NAMES[i]) == true) {
					color = OBJECTCOLOR_NAMES[(i + 1) % OBJECTCOLOR_NAMES.size()];
					break;
				}
			}
			if (color == "none") {
				levelEditorObject->removeProperty("object.color");
			} else {
				levelEditorObject->updateProperty(colorProperty->getName(), "object.color", color);
			}
		}
		setStandardObjectColorEffect(selectedEntity);
		setHighlightObjectColorEffect(selectedEntity);
	}

	if (selectedEntityIds.size() == 1) {
		auto selectedEntity = engine->getEntity(selectedEntityIds[0]);
		if (selectedEntity != nullptr && StringTools::startsWith(selectedEntity->getId(), "tdme.leveleditor.") == false) {
			auto levelEditorObject = level->getObjectById(selectedEntity->getId());
			auto preset = levelEditorObject->getProperty("preset");
			levelEditorScreenController->setObjectProperties(preset != nullptr ? preset->getValue() : "", levelEditorObject, "");
		} else {
			levelEditorScreenController->unsetObjectProperties();
		}
	} else
	if (selectedEntityIds.size() > 1) {
		levelEditorScreenController->unsetObjectProperties();
	}
}

void LevelEditorView::centerObject()
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

void LevelEditorView::objectTranslationApply(float x, float y, float z)
{
	if (selectedEntityIds.size() == 0)
		return;

	if (selectedEntityIds.size() == 1) {
		auto selectedEntity = engine->getEntity(selectedEntityIds[0]);
		if (selectedEntity == nullptr) return;
		auto levelEntity = level->getObjectById(selectedEntity->getId());
		if (levelEntity == nullptr) return;

		levelEntity->getTransformations().setTranslation(Vector3(x, y, z));
		levelEntity->getTransformations().update();
		selectedEntity->fromTransformations(levelEntity->getTransformations());
	} else
	if (selectedEntityIds.size() > 1) {
		for (auto selectedEntityId: selectedEntityIds) {
			auto selectedEntity = engine->getEntity(selectedEntityId);
			if (selectedEntity == nullptr) continue;
			auto levelEntity = level->getObjectById(selectedEntity->getId());
			if (levelEntity == nullptr) continue;
			levelEntity->getTransformations().setTranslation(
				levelEntity->getTransformations().getTranslation().clone().add(Vector3(x, y, z))
			);
			levelEntity->getTransformations().update();
			selectedEntity->fromTransformations(levelEntity->getTransformations());
		}
		levelEditorScreenController->setObject(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), 0.0f, 0.0f, 0.0f);
	}
	level->update();
	updateGizmo();
	updateGUIElements();
}

void LevelEditorView::objectScaleApply(float x, float y, float z)
{
	if (selectedEntityIds.size() == 0)
		return;

	if (selectedEntityIds.size() == 1) {
		auto selectedEntity = engine->getEntity(selectedEntityIds[0]);
		if (selectedEntity == nullptr) return;
		auto levelEntity = level->getObjectById(selectedEntity->getId());
		if (levelEntity == nullptr) return;

		levelEntity->getTransformations().setScale(Vector3(x, y, z));
		levelEntity->getTransformations().update();
		selectedEntity->fromTransformations(levelEntity->getTransformations());
	} else
	if (selectedEntityIds.size() > 1) {
		for (auto selectedEntityId: selectedEntityIds) {
			auto selectedEntity = engine->getEntity(selectedEntityId);
			if (selectedEntity == nullptr) continue;
			auto levelEntity = level->getObjectById(selectedEntity->getId());
			if (levelEntity == nullptr) continue;

			levelEntity->getTransformations().setScale(levelEntity->getTransformations().getScale().clone().scale(Vector3(x, y, z)));
			levelEntity->getTransformations().update();
			selectedEntity->fromTransformations(levelEntity->getTransformations());
		}
		levelEditorScreenController->setObject(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), 0.0f, 0.0f, 0.0f);
	}
	level->update();
	updateGizmo();
	updateGUIElements();
}

void LevelEditorView::objectRotationsApply(float x, float y, float z)
{
	if (selectedEntityIds.size() == 0)
		return;

	if (selectedEntityIds.size() == 1) {
		auto selectedEntity = engine->getEntity(selectedEntityIds[0]);
		if (selectedEntity == nullptr) return;
		auto levelEntity = level->getObjectById(selectedEntity->getId());
		if (levelEntity == nullptr) return;
		levelEntity->getTransformations().getRotation(level->getRotationOrder()->getAxisXIndex()).setAngle(x);
		levelEntity->getTransformations().getRotation(level->getRotationOrder()->getAxisYIndex()).setAngle(y);
		levelEntity->getTransformations().getRotation(level->getRotationOrder()->getAxisZIndex()).setAngle(z);
		levelEntity->getTransformations().update();
		selectedEntity->fromTransformations(levelEntity->getTransformations());
	} else
	if (selectedEntityIds.size() > 1) {
		for (auto selectedEntityId: selectedEntityIds) {
			auto selectedEntity = engine->getEntity(selectedEntityId);
			if (selectedEntity == nullptr) continue;
			auto levelEntity = level->getObjectById(selectedEntity->getId());
			if (levelEntity == nullptr) continue;

			levelEntity->getTransformations().getRotation(level->getRotationOrder()->getAxisXIndex()).setAngle(levelEntity->getTransformations().getRotation(level->getRotationOrder()->getAxisXIndex()).getAngle() + x);
			levelEntity->getTransformations().getRotation(level->getRotationOrder()->getAxisYIndex()).setAngle(levelEntity->getTransformations().getRotation(level->getRotationOrder()->getAxisYIndex()).getAngle() + y);
			levelEntity->getTransformations().getRotation(level->getRotationOrder()->getAxisZIndex()).setAngle(levelEntity->getTransformations().getRotation(level->getRotationOrder()->getAxisZIndex()).getAngle() + z);
			levelEntity->getTransformations().update();
			selectedEntity->fromTransformations(levelEntity->getTransformations());
		}
		levelEditorScreenController->setObject(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), 0.0f, 0.0f, 0.0f);
	}
	level->update();
	updateGizmo();
	updateGUIElements();
}

bool LevelEditorView::mapPropertySave(const string& oldName, const string& name, const string& value)
{
	if (level->updateProperty(oldName, name, value) == true) {
		levelEditorScreenController->setMapProperties(level, name);
		return true;
	}
	return false;
}

bool LevelEditorView::mapPropertyAdd()
{
	if (level->addProperty("new.property", "new.value")) {
		levelEditorScreenController->setMapProperties(level, "new.property");
		return true;
	}
	return false;
}

bool LevelEditorView::mapPropertyRemove(const string& name)
{
	auto idx = level->getPropertyIndex(name);
	if (idx != -1 && level->removeProperty(name) == true) {
		auto property = level->getPropertyByIndex(idx);
		if (property == nullptr) {
			property = level->getPropertyByIndex(idx - 1);
		}
		levelEditorScreenController->setMapProperties(level, property == nullptr ? "" : property->getName());
		return true;
	}
	return false;
}

bool LevelEditorView::objectPropertyRemove(const string& name)
{
	if (selectedEntityIds.size() != 1) return false;

	auto selectedEntity = engine->getEntity(selectedEntityIds[0]);
	if (selectedEntity == nullptr) return false;
	auto levelEntity = level->getObjectById(selectedEntity->getId());
	if (levelEntity == nullptr) return false;

	auto idx = levelEntity->getPropertyIndex(name);
	if (idx != -1 && levelEntity->removeProperty(name) == true) {
		auto property = levelEntity->getPropertyByIndex(idx);
		if (property == nullptr) {
			property = levelEntity->getPropertyByIndex(idx - 1);
		}
		levelEditorScreenController->setObjectProperties("", levelEntity, property == nullptr ? "" : property->getName());
		return true;
	}
	return false;
}

void LevelEditorView::objectPropertiesPreset(const string& presetId)
{
	if (selectedEntityIds.size() != 1) return;

	auto selectedEntity = engine->getEntity(selectedEntityIds[0]);
	if (selectedEntity == nullptr) return;
	auto levelEntity = level->getObjectById(selectedEntity->getId());
	if (levelEntity == nullptr) return;

	levelEntity->clearProperties();
	auto& objectPropertiesPresets = LevelPropertyPresets::getInstance()->getObjectPropertiesPresets();
	const vector<PropertyModelClass*>* objectPropertyPresetVector = nullptr;
	auto objectPropertyPresetVectorIt = objectPropertiesPresets.find(presetId);
	if (objectPropertyPresetVectorIt != objectPropertiesPresets.end()) {
		objectPropertyPresetVector = &objectPropertyPresetVectorIt->second;
	}
	if (objectPropertyPresetVector != nullptr) {
		for (auto objectPropertyPreset: *objectPropertyPresetVector) {
			levelEntity->addProperty(objectPropertyPreset->getName(), objectPropertyPreset->getValue());
		}
	}
	levelEditorScreenController->setObjectProperties(presetId, levelEntity, "");
}

bool LevelEditorView::objectPropertySave(const string& oldName, const string& name, const string& value)
{
	if (selectedEntityIds.size() != 1) return false;

	auto selectedEntity = engine->getEntity(selectedEntityIds[0]);
	if (selectedEntity == nullptr) return false;
	auto levelEntity = level->getObjectById(selectedEntity->getId());
	if (levelEntity == nullptr) return false;

	if (levelEntity->updateProperty(oldName, name, value) == true) {
		levelEditorScreenController->setObjectProperties("", levelEntity, name);
		return true;
	}
	return false;
}

bool LevelEditorView::objectPropertyAdd()
{
	if (selectedEntityIds.size() != 1) return false;

	auto selectedEntity = engine->getEntity(selectedEntityIds[0]);
	if (selectedEntity == nullptr) return false;
	auto levelEntity = level->getObjectById(selectedEntity->getId());
	if (levelEntity == nullptr) return false;

	if (levelEntity->addProperty("new.property", "new.value")) {
		levelEditorScreenController->setObjectProperties("", levelEntity, "new.property");
		return true;
	}
	return false;
}

void LevelEditorView::loadMap(const string& path, const string& file)
{
	engine->reset();
	selectedEntity = nullptr;
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
			LevelFileImport::doImportFromModel(path, file, level, new ImportProgressCallback(popUps->getProgressBarScreenController()));
		} else {
			LevelFileImport::doImport(path, file, level, new ImportProgressCallback(popUps->getProgressBarScreenController()));
		}
		popUps->getProgressBarScreenController()->close();
		for (auto i = 0; i < level->getEntityLibrary()->getEntityCount(); i++) {
			level->getEntityLibrary()->getEntityAt(i)->setDefaultBoundingVolumes();
		}
		levelEditorScreenController->setMapProperties(level, "");
		levelEditorScreenController->unsetObjectProperties();
		levelEditorScreenController->unsetObject();
		loadLevel();
		engine->getCamera()->setLookAt(level->getCenter());
		camLookRotationX->setAngle(-45.0f);
		camLookRotationX->update();
		camLookRotationY->setAngle(0.0f);
		camLookRotationY->update();
		camScale = 1.0f;
		gridCenter.set(engine->getCamera()->getLookAt());
		reloadEntityLibrary = true;
		updateGUIElements();
	} catch (Exception& exception) {
		levelEditorScreenController->showErrorPopUp(
			"Warning: Could not load level file",
			(string(exception.what()))
		);
	}
}

void LevelEditorView::saveMap(const string& pathName, const string& fileName)
{
	try {
		LevelFileExport::doExport(pathName, fileName, level);
	} catch (Exception& exception) {
		levelEditorScreenController->showErrorPopUp(
			"Warning: Could not save level file",
			(string(exception.what()))
		);
	}
	updateGUIElements();
}

void LevelEditorView::copyObjects()
{
	pasteObjects_.clear();
	for (auto selectedEntityId: selectedEntityIds) {
		auto selectedEntity = engine->getEntity(selectedEntityId);
		if (selectedEntity != nullptr && StringTools::startsWith(selectedEntity->getId(), "tdme.leveleditor.") == false) {
			auto levelEntity = level->getObjectById(selectedEntity->getId());
			if (levelEntity == nullptr) continue;
			pasteObjects_.push_back(levelEntity);
		}
	}
}

void LevelEditorView::setPasteMode() {
	pasteMode = true;
	pasteModeValid = false;
}

void LevelEditorView::unsetPasteMode() {
	auto pasteObjectIdx = 0;
	for (auto pasteObject: pasteObjects_) {
		auto pasteModel = pasteObject->getEntity();
		auto entityId = "tdme.leveleditor.paste." + pasteModel->getName() + "." + to_string(pasteObjectIdx);
		engine->removeEntity(entityId);
		pasteObjectIdx++;
	}
	pasteMode = false;
	pasteModeValid = false;
}

void LevelEditorView::pasteObjects(bool displayOnly)
{
	auto pasteObjectsMinX = Float::MAX_VALUE;
	auto pasteObjectsMinZ = Float::MAX_VALUE;
	auto pasteObjectsMinY = Float::MAX_VALUE;
	for (auto object: pasteObjects_) {
		auto entity = engine->getEntity(object->getId());
		if (entity == nullptr) continue;
		BoundingBox cbv;
		cbv.fromBoundingVolumeWithTransformations(entity->getBoundingBox(), object->getTransformations());
		auto& objectBBMinXYZ = cbv.getMin().getArray();
		if (objectBBMinXYZ[0] < pasteObjectsMinX) pasteObjectsMinX = objectBBMinXYZ[0];
		if (objectBBMinXYZ[1] < pasteObjectsMinY) pasteObjectsMinY = objectBBMinXYZ[1];
		if (objectBBMinXYZ[2] < pasteObjectsMinZ) pasteObjectsMinZ = objectBBMinXYZ[2];
	}
	auto pasteObjectIdx = 0;
	for (auto pasteObject: pasteObjects_) {
		auto pasteModel = pasteObject->getEntity();
		Transformations levelEditorObjectTransformations;
		levelEditorObjectTransformations.fromTransformations(pasteObject->getTransformations());
		auto objectDiffX = pasteObject->getTransformations().getTranslation().getX() - pasteObjectsMinX;
		auto objectDiffY = pasteObject->getTransformations().getTranslation().getY() - pasteObjectsMinY;
		auto objectDiffZ = pasteObject->getTransformations().getTranslation().getZ() - pasteObjectsMinZ;
		levelEditorObjectTransformations.setTranslation(
			Vector3(
				placeEntityTranslation.getX() + objectDiffX,
				placeEntityTranslation.getY() + objectDiffY,
				placeEntityTranslation.getZ() + objectDiffZ
			)
		);
		levelEditorObjectTransformations.update();
		if (displayOnly == false) {
			for (auto i = 0; i < level->getObjectCount(); i++) {
				auto levelEditorObject = level->getObjectAt(i);
				if (levelEditorObject->getEntity() == pasteModel && levelEditorObject->getTransformations().getTranslation().equals(levelEditorObjectTransformations.getTranslation())) {
					continue;
				}
			}
		}
		if (displayOnly == false) {
			//
			auto levelEditorObjectId = pasteModel->getName() + "_" + to_string(level->allocateObjectId());
			auto levelEditorObject = new LevelEditorObject(
				levelEditorObjectId,
				"",
				levelEditorObjectTransformations,
				pasteModel
			 );
			ModelProperties* properties = pasteObject;
			for (int i = 0; i < properties->getPropertyCount(); i++) {
				PropertyModelClass* property = properties->getPropertyByIndex(i);
				levelEditorObject->addProperty(property->getName(), property->getValue());
			}
			level->addObject(levelEditorObject);
			auto entity = Level::createEntity(pasteModel, levelEditorObjectId, levelEditorObjectTransformations);
			if (entity != nullptr) {
				setStandardObjectColorEffect(entity);
				entity->setPickable(true);
				engine->addEntity(entity);
			}
		} else {
			auto entityId = "tdme.leveleditor.paste." + pasteModel->getName() + "." + to_string(pasteObjectIdx);
			auto entity = engine->getEntity(entityId);
			if (entity != nullptr) {
				entity->fromTransformations(levelEditorObjectTransformations);
			} else {
				entity = Level::createEntity(pasteModel, entityId, levelEditorObjectTransformations);
				if (entity != nullptr) {
					setStandardObjectColorEffect(entity);
					entity->setPickable(true);
					engine->addEntity(entity);
				}
			}
		}
		pasteObjectIdx++;
	}
	if (displayOnly == false) levelEditorScreenController->setObjectListbox(level);
}

void LevelEditorView::computeSpotDirection(int32_t i, const Vector4& position, const Vector3& spotTo)
{
	auto _from = Vector3(position.getX(), position.getY(), position.getZ());
	auto spotDirection = spotTo.clone().sub(_from);
	level->getLightAt(i)->getPosition().set(position.getX(), position.getY(), position.getZ(), position.getW());
	level->getLightAt(i)->getSpotTo().set(spotTo.getX(), spotTo.getY(), spotTo.getZ());
	level->getLightAt(i)->getSpotDirection().set(spotDirection.getX(), spotDirection.getY(), spotDirection.getZ());
	engine->getLightAt(i)->setPosition(Vector4(position.getX(), position.getY(), position.getZ(), position.getW()));
	engine->getLightAt(i)->setSpotDirection(Vector3(spotDirection.getX(), spotDirection.getY(), spotDirection.getZ()));
	levelEditorScreenController->setLight(i, level->getLightAt(i)->getAmbient(), level->getLightAt(i)->getDiffuse(), level->getLightAt(i)->getSpecular(), level->getLightAt(i)->getPosition(), level->getLightAt(i)->getConstantAttenuation(), level->getLightAt(i)->getLinearAttenuation(), level->getLightAt(i)->getQuadraticAttenuation(), level->getLightAt(i)->getSpotTo(), level->getLightAt(i)->getSpotDirection(), level->getLightAt(i)->getSpotExponent(), level->getLightAt(i)->getSpotCutOff(), level->getLightAt(i)->isEnabled());
}

void LevelEditorView::applyLight(int32_t i, const Color4& ambient, const Color4& diffuse, const Color4& specular, const Vector4& position, float constantAttenuation, float linearAttenuation, float quadraticAttenuation, const Vector3& spotTo, const Vector3& spotDirection, float spotExponent, float spotCutoff, bool enabled)
{
	level->getLightAt(i)->getAmbient().set(ambient);
	level->getLightAt(i)->getDiffuse().set(diffuse);
	level->getLightAt(i)->getSpecular().set(specular);
	level->getLightAt(i)->getPosition().set(position);
	level->getLightAt(i)->setConstantAttenuation(constantAttenuation);
	level->getLightAt(i)->setLinearAttenuation(linearAttenuation);
	level->getLightAt(i)->setQuadraticAttenuation(quadraticAttenuation);
	level->getLightAt(i)->getSpotTo().set(spotTo.getX(), spotTo.getY(), spotTo.getZ());
	level->getLightAt(i)->getSpotDirection().set(spotDirection);
	level->getLightAt(i)->setSpotExponent(spotExponent);
	level->getLightAt(i)->setSpotCutOff(spotCutoff);
	level->getLightAt(i)->setEnabled(enabled);
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
	levelEditorScreenController->setLight(i, level->getLightAt(i)->getAmbient(), level->getLightAt(i)->getDiffuse(), level->getLightAt(i)->getSpecular(), level->getLightAt(i)->getPosition(), level->getLightAt(i)->getConstantAttenuation(), level->getLightAt(i)->getLinearAttenuation(), level->getLightAt(i)->getQuadraticAttenuation(), level->getLightAt(i)->getSpotTo(), level->getLightAt(i)->getSpotDirection(), level->getLightAt(i)->getSpotExponent(), level->getLightAt(i)->getSpotCutOff(), level->getLightAt(i)->isEnabled());
}

void LevelEditorView::updateGizmo() {
	if (selectedEntityIds.size() == 0) {
		removeGizmo();
		return;
	}

	//
	Vector3 gizmoCenter;
	auto objectCount = 0;
	for (auto selectedEntityId: selectedEntityIds) {
		auto selectedEntity = engine->getEntity(selectedEntityId);
		if (selectedEntity != nullptr && StringTools::startsWith(selectedEntity->getId(), "tdme.leveleditor.") == false) {
			auto levelEditorObject = level->getObjectById(selectedEntity->getId());
			if (levelEditorObject == nullptr) continue;
			gizmoCenter.add(levelEditorObject->getTransformations().getTranslation());
			objectCount++;
		}
	}
	if (objectCount == 0) {
		removeGizmo();
		return;
	} else
	if (objectCount > 1) gizmoCenter.scale(1.0f / objectCount);

	//
	Gizmo::updateGizmo(gizmoCenter);
}
