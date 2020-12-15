#include <tdme/tools/shared/views/EntityPhysicsView.h>

#include <tdme/math/Math.h>

#include <tdme/engine/Engine.h>
#include <tdme/engine/Entity.h>
#include <tdme/engine/EntityHierarchy.h>
#include <tdme/engine/Object3D.h>
#include <tdme/engine/Rotation.h>
#include <tdme/engine/Transformations.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/primitives/BoundingVolume.h>
#include <tdme/engine/primitives/Capsule.h>
#include <tdme/engine/primitives/ConvexMesh.h>
#include <tdme/engine/primitives/OrientedBoundingBox.h>
#include <tdme/engine/primitives/Sphere.h>
#include <tdme/engine/primitives/Sphere.h>
#include <tdme/gui/GUI.h>
#include <tdme/gui/events/GUIKeyboardEvent.h>
#include <tdme/gui/events/GUIMouseEvent.h>
#include <tdme/math/Math.h>
#include <tdme/math/Vector3.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/tools/shared/controller/EntityPhysicsSubScreenController_BoundingVolumeType.h>
#include <tdme/tools/shared/controller/EntityPhysicsSubScreenController.h>
#include <tdme/tools/shared/model/LevelEditorEntity.h>
#include <tdme/tools/shared/model/LevelEditorEntityBoundingVolume.h>
#include <tdme/tools/shared/tools/Tools.h>
#include <tdme/tools/shared/views/Gizmo.h>
#include <tdme/tools/shared/views/PopUps.h>
#include <tdme/utilities/Character.h>
#include <tdme/utilities/Console.h>

using tdme::tools::shared::views::EntityPhysicsView;

using tdme::math::Math;

using tdme::engine::Engine;
using tdme::engine::Entity;
using tdme::engine::EntityHierarchy;
using tdme::engine::Object3D;
using tdme::engine::Rotation;
using tdme::engine::Transformations;
using tdme::engine::model::Model;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::primitives::BoundingVolume;
using tdme::engine::primitives::Capsule;
using tdme::engine::primitives::ConvexMesh;
using tdme::engine::primitives::OrientedBoundingBox;
using tdme::engine::primitives::Sphere;
using tdme::gui::GUI;
using tdme::gui::events::GUIKeyboardEvent;
using tdme::gui::events::GUIMouseEvent;
using tdme::math::Math;
using tdme::math::Vector3;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;
using tdme::tools::shared::controller::EntityPhysicsSubScreenController_BoundingVolumeType;
using tdme::tools::shared::controller::EntityPhysicsSubScreenController;
using tdme::tools::shared::model::LevelEditorEntity;
using tdme::tools::shared::model::LevelEditorEntityBoundingVolume;
using tdme::tools::shared::tools::Tools;
using tdme::tools::shared::views::Gizmo;
using tdme::tools::shared::views::PopUps;
using tdme::utilities::Character;
using tdme::utilities::Console;

EntityPhysicsView::EntityPhysicsView(EntityPhysicsSubScreenController* entityPhysicsSubScreenController, PopUps* popUps, int maxBoundingVolumeCount, int32_t boundingVolumeTypeMask): Gizmo(Engine::getInstance(), "epv")
{
	this->engine = Engine::getInstance();
	this->popUps = popUps;
	this->entityPhysicsSubScreenController = entityPhysicsSubScreenController;
	this->maxBoundingVolumeCount = maxBoundingVolumeCount == -1?LevelEditorEntity::MODEL_BOUNDINGVOLUME_COUNT:maxBoundingVolumeCount;
	this->mouseDownLastX = MOUSE_DOWN_LAST_POSITION_NONE;
	this->mouseDownLastY = MOUSE_DOWN_LAST_POSITION_NONE;
	this->displayBoundingVolumeIdx = DISPLAY_BOUNDINGVOLUMEIDX_ALL;
	this->displayBoundingVolumeIdxLast = DISPLAY_BOUNDINGVOLUMEIDX_ALL;
	this->displayBoundingVolume = false;
	this->boundingVolumeTypeMask = boundingVolumeTypeMask;
}

EntityPhysicsView::~EntityPhysicsView() {
}

PopUps* EntityPhysicsView::getPopUpsViews()
{
	return popUps;
}

void EntityPhysicsView::initialize()
{
	for (auto i = 0; i < maxBoundingVolumeCount; i++) {
		entityPhysicsSubScreenController->setupBoundingVolumeTypes(i, boundingVolumeTypeMask);
		entityPhysicsSubScreenController->selectBoundingVolume(i, EntityPhysicsSubScreenController_BoundingVolumeType::NONE);
	}
}

void EntityPhysicsView::resetBoundingVolume(LevelEditorEntity* entity, int idx, int type)
{
	BoundingBox aabb;
	if (entity->getModel() != nullptr) {
		aabb = *entity->getModel()->getBoundingBox();
	} else
	if (engine->getEntity("model") != nullptr){
		Transformations transformations;
		transformations.setScale(
			Vector3(
				1.0f / engine->getEntity("model")->getScale().getX(),
				1.0f / engine->getEntity("model")->getScale().getY(),
				1.0f / engine->getEntity("model")->getScale().getZ()
			)
		);
		transformations.update();
		aabb.fromBoundingVolumeWithTransformations(engine->getEntity("model")->getBoundingBoxTransformed(), transformations);
	} else {
		aabb = BoundingBox(Vector3(-0.5f, 0.0f, -0.5f), Vector3(0.5f, 3.0f, 0.5f));
	}
	auto obb = OrientedBoundingBox(&aabb);
	auto boundingVolume = entity->getBoundingVolume(idx);
	if (type == 0) {
		entityPhysicsSubScreenController->selectBoundingVolume(idx, EntityPhysicsSubScreenController_BoundingVolumeType::NONE);
	} else
	if (type == 1) {
		entityPhysicsSubScreenController->setupSphere(idx, obb.getCenter(), obb.getHalfExtension().computeLength());
	} else
	if (type == 2) {
		Vector3 a;
		Vector3 b;
		auto radius = 0.0f;
		auto& halfExtensionXYZ = obb.getHalfExtension().getArray();
		if (halfExtensionXYZ[0] > halfExtensionXYZ[1] && halfExtensionXYZ[0] > halfExtensionXYZ[2]) {
			radius = Math::sqrt(halfExtensionXYZ[1] * halfExtensionXYZ[1] + halfExtensionXYZ[2] * halfExtensionXYZ[2]);
			a.set(obb.getAxes()[0]);
			a.scale(-(halfExtensionXYZ[0] - radius));
			a.add(obb.getCenter());
			b.set(obb.getAxes()[0]);
			b.scale(+(halfExtensionXYZ[0] - radius));
			b.add(obb.getCenter());
		} else
		if (halfExtensionXYZ[1] > halfExtensionXYZ[0] && halfExtensionXYZ[1] > halfExtensionXYZ[2]) {
			radius = Math::sqrt(halfExtensionXYZ[0] * halfExtensionXYZ[0] + halfExtensionXYZ[2] * halfExtensionXYZ[2]);
			a.set(obb.getAxes()[1]);
			a.scale(-(halfExtensionXYZ[1] - radius));
			a.add(obb.getCenter());
			b.set(obb.getAxes()[1]);
			b.scale(+(halfExtensionXYZ[1] - radius));
			b.add(obb.getCenter());
		} else {
			radius = Math::sqrt(halfExtensionXYZ[0] * halfExtensionXYZ[0] + halfExtensionXYZ[1] * halfExtensionXYZ[1]);
			a.set(obb.getAxes()[2]);
			a.scale(-(halfExtensionXYZ[2] - radius));
			a.add(obb.getCenter());
			b.set(obb.getAxes()[2]);
			b.scale(+(halfExtensionXYZ[2] - radius));
			b.add(obb.getCenter());
		}
		entityPhysicsSubScreenController->setupCapsule(idx, a, b, radius);
	} else
	if (type == 3) {
		entityPhysicsSubScreenController->setupBoundingBox(idx, aabb.getMin(), aabb.getMax());
	} else
	if (type == 4) {
		entityPhysicsSubScreenController->setupOrientedBoundingBox(idx, obb.getCenter(), obb.getAxes()[0], obb.getAxes()[1], obb.getAxes()[2], obb.getHalfExtension());
	}
}

void EntityPhysicsView::setBoundingVolumes(LevelEditorEntity* entity)
{
	for (auto i = 0; i < maxBoundingVolumeCount; i++) {
		resetBoundingVolume(entity, i, 0);
	}
	for (auto i = 0; i < entity->getBoundingVolumeCount(); i++) {
		auto bv = entity->getBoundingVolume(i);
		if (bv == nullptr) {
			entityPhysicsSubScreenController->selectBoundingVolume(i, EntityPhysicsSubScreenController_BoundingVolumeType::NONE);
			continue;
		} else
		if (dynamic_cast<Sphere*>(bv->getBoundingVolume()) != nullptr) {
			auto sphere = dynamic_cast< Sphere* >(bv->getBoundingVolume());
			entityPhysicsSubScreenController->setupSphere(i, sphere->getCenter(), sphere->getRadius());
			entityPhysicsSubScreenController->selectBoundingVolume(i, EntityPhysicsSubScreenController_BoundingVolumeType::SPHERE);
			entityPhysicsSubScreenController->setupModelBoundingVolumeType(entity, i);
		} else
		if (dynamic_cast<Capsule*>(bv->getBoundingVolume()) != nullptr) {
			auto capsule = dynamic_cast< Capsule* >(bv->getBoundingVolume());
			entityPhysicsSubScreenController->setupCapsule(i, capsule->getA(), capsule->getB(), capsule->getRadius());
			entityPhysicsSubScreenController->selectBoundingVolume(i, EntityPhysicsSubScreenController_BoundingVolumeType::CAPSULE);
			entityPhysicsSubScreenController->setupModelBoundingVolumeType(entity, i);
		} else
		if (dynamic_cast<BoundingBox*>(bv->getBoundingVolume()) != nullptr) {
			auto aabb = dynamic_cast< BoundingBox* >(bv->getBoundingVolume());
			entityPhysicsSubScreenController->setupBoundingBox(i, aabb->getMin(), aabb->getMax());
			entityPhysicsSubScreenController->selectBoundingVolume(i, EntityPhysicsSubScreenController_BoundingVolumeType::BOUNDINGBOX);
			entityPhysicsSubScreenController->setupModelBoundingVolumeType(entity, i);
		} else
		if (dynamic_cast<OrientedBoundingBox*>(bv->getBoundingVolume()) != nullptr) {
			if ((boundingVolumeTypeMask & EntityPhysicsSubScreenController::BOUNDINGVOLUMETYPE_ORIENTEDBOUNDINGBOX) == EntityPhysicsSubScreenController::BOUNDINGVOLUMETYPE_ORIENTEDBOUNDINGBOX) {
				auto obb = dynamic_cast< OrientedBoundingBox* >(bv->getBoundingVolume());
				entityPhysicsSubScreenController->setupOrientedBoundingBox(i, obb->getCenter(), obb->getAxes()[0], obb->getAxes()[1], obb->getAxes()[2], obb->getHalfExtension());
				entityPhysicsSubScreenController->selectBoundingVolume(i, EntityPhysicsSubScreenController_BoundingVolumeType::ORIENTEDBOUNDINGBOX);
				entityPhysicsSubScreenController->setupModelBoundingVolumeType(entity, i);
			} else
			if ((boundingVolumeTypeMask & EntityPhysicsSubScreenController::BOUNDINGVOLUMETYPE_BOUNDINGBOX) == EntityPhysicsSubScreenController::BOUNDINGVOLUMETYPE_BOUNDINGBOX) {
				auto obb = dynamic_cast< OrientedBoundingBox* >(bv->getBoundingVolume());
				BoundingBox aabb(obb);
				entityPhysicsSubScreenController->setupBoundingBox(i, aabb.getMin(), aabb.getMax());
				entityPhysicsSubScreenController->selectBoundingVolume(i, EntityPhysicsSubScreenController_BoundingVolumeType::BOUNDINGBOX);
				entityPhysicsSubScreenController->getView()->selectBoundingVolumeType(i, 3);
			}
		} else
		if (dynamic_cast<ConvexMesh*>(bv->getBoundingVolume()) != nullptr) {
			entityPhysicsSubScreenController->setupConvexMesh(i, bv->getModelMeshFile());
			entityPhysicsSubScreenController->selectBoundingVolume(i, EntityPhysicsSubScreenController_BoundingVolumeType::CONVEXMESH);
			entityPhysicsSubScreenController->setupModelBoundingVolumeType(entity, i);
		}
		entityPhysicsSubScreenController->enableBoundingVolume(i);
	}
}

void EntityPhysicsView::unsetBoundingVolumes()
{
	for (auto i = 0; i < maxBoundingVolumeCount; i++) {
		entityPhysicsSubScreenController->disableBoundingVolume(i);
	}
}

void EntityPhysicsView::selectBoundingVolumeType(int idx, int bvTypeId)
{
	switch (bvTypeId) {
		case 0:
			entityPhysicsSubScreenController->selectBoundingVolume(idx, EntityPhysicsSubScreenController_BoundingVolumeType::NONE);
			break;
		case 1:
			entityPhysicsSubScreenController->selectBoundingVolume(idx, EntityPhysicsSubScreenController_BoundingVolumeType::SPHERE);
			break;
		case 2:
			entityPhysicsSubScreenController->selectBoundingVolume(idx, EntityPhysicsSubScreenController_BoundingVolumeType::CAPSULE);
			break;
		case 3:
			entityPhysicsSubScreenController->selectBoundingVolume(idx, EntityPhysicsSubScreenController_BoundingVolumeType::BOUNDINGBOX);
			break;
		case 4:
			entityPhysicsSubScreenController->selectBoundingVolume(idx, EntityPhysicsSubScreenController_BoundingVolumeType::ORIENTEDBOUNDINGBOX);
			break;
		case 5:
			entityPhysicsSubScreenController->selectBoundingVolume(idx, EntityPhysicsSubScreenController_BoundingVolumeType::CONVEXMESH);
			break;
	}
}

void EntityPhysicsView::clearModelBoundingVolume(int idx) {
	if (idx != DISPLAY_BOUNDINGVOLUMEIDX_ALL) {
		dynamic_cast<EntityHierarchy*>(engine->getEntity(LevelEditorEntity::MODEL_BOUNDINGVOLUMES_ID))->removeEntity(LevelEditorEntity::MODEL_BOUNDINGVOLUME_IDS[idx]);
	}
	engine->removeEntity(LevelEditorEntity::MODEL_BOUNDINGVOLUME_EDITING_ID);
}

void EntityPhysicsView::setupModelBoundingVolume(LevelEditorEntity* entity, int idx)
{
	auto entityBoundingVolume = entity->getBoundingVolume(idx);
	if (entityBoundingVolume == nullptr) return;

	{
		Vector3 objectScale(1.0f, 1.0f, 1.0f);
		auto modelEntity = engine->getEntity("model");
		if (modelEntity != nullptr) objectScale = modelEntity->getScale();
		auto modelBoundingVolumeEntityId = LevelEditorEntity::MODEL_BOUNDINGVOLUME_IDS[idx];
		if (entityBoundingVolume->getModel() != nullptr) {
			auto modelBoundingVolumeEntity = new Object3D(modelBoundingVolumeEntityId, entityBoundingVolume->getModel());
			modelBoundingVolumeEntity->setScale(objectScale);
			modelBoundingVolumeEntity->setRenderPass(Entity::RENDERPASS_POST_POSTPROCESSING);
			modelBoundingVolumeEntity->setEnabled(false);
			modelBoundingVolumeEntity->update();
			dynamic_cast<EntityHierarchy*>(engine->getEntity(LevelEditorEntity::MODEL_BOUNDINGVOLUMES_ID))->addEntity(modelBoundingVolumeEntity);
		}
	}
	if (displayBoundingVolumeIdx == idx) {
		auto modelBoundingVolumeEntityId = LevelEditorEntity::MODEL_BOUNDINGVOLUME_EDITING_ID;
		auto boundingVolumesEntity = engine->getEntity(LevelEditorEntity::MODEL_BOUNDINGVOLUMES_ID);
		if (entityBoundingVolume->getModel() != nullptr) {
			auto bv = entity->getBoundingVolume(idx);
			if (dynamic_cast<OrientedBoundingBox*>(bv->getBoundingVolume()) != nullptr) {
				auto obb = dynamic_cast<OrientedBoundingBox*>(bv->getBoundingVolume());
				Transformations transformations;
				transformations.fromMatrix(
					Matrix4x4().identity().setAxes(obb->getAxes()[0], obb->getAxes()[1], obb->getAxes()[2]),
					RotationOrder::ZYX
				);
				transformations.setTranslation(obb->getCenter());
				transformations.setScale(obb->getHalfExtension().clone().scale(2.0f));
				transformations.setScale(boundingVolumesEntity->getScale().clone().scale(transformations.getScale()));
				transformations.setTranslation(transformations.getTranslation().clone().scale(boundingVolumesEntity->getScale()));
				transformations.update();
				auto modelBoundingVolumeEntity = new Object3D(modelBoundingVolumeEntityId, Tools::getDefaultObb());
				modelBoundingVolumeEntity->fromTransformations(transformations);
				modelBoundingVolumeEntity->setRenderPass(Entity::RENDERPASS_POST_POSTPROCESSING);
				modelBoundingVolumeEntity->setEnabled(false);
				engine->addEntity(modelBoundingVolumeEntity);
			} else {
				Vector3 pivot;
				if (dynamic_cast<Sphere*>(bv->getBoundingVolume()) != nullptr) {
					auto sphere = dynamic_cast<Sphere*>(bv->getBoundingVolume());
					pivot = sphere->getCenter();
				} else
				if (dynamic_cast<Capsule*>(bv->getBoundingVolume()) != nullptr) {
					auto capsule = dynamic_cast<Capsule*>(bv->getBoundingVolume());
					auto a = capsule->getA();
					auto b = capsule->getB();
					pivot = capsule->getA().clone().add(capsule->getB()).scale(0.5f);
				}
				auto modelBoundingVolumeEntity = new Object3D(modelBoundingVolumeEntityId, entityBoundingVolume->getModel());
				modelBoundingVolumeEntity->setRenderPass(Entity::RENDERPASS_POST_POSTPROCESSING);
				modelBoundingVolumeEntity->setPivot(pivot);
				modelBoundingVolumeEntity->setScale(boundingVolumesEntity->getScale());
				modelBoundingVolumeEntity->setPivot(modelBoundingVolumeEntity->getPivot().clone().scale(boundingVolumesEntity->getScale()));
				modelBoundingVolumeEntity->setEnabled(false);
				modelBoundingVolumeEntity->update();
				engine->addEntity(modelBoundingVolumeEntity);
			}
		}
	}
}

void EntityPhysicsView::applyBoundingVolumeNone(LevelEditorEntity* entity, int idx)
{
	if (entity == nullptr) return;
	clearModelBoundingVolume(idx);
	auto entityBoundingVolume = entity->getBoundingVolume(idx);
	entityBoundingVolume->setupNone();
	setupModelBoundingVolume(entity, idx);
}

void EntityPhysicsView::applyBoundingVolumeSphere(LevelEditorEntity* entity, int idx, const Vector3& center, float radius)
{
	if (entity == nullptr) return;
	clearModelBoundingVolume(idx);
	auto entityBoundingVolume = entity->getBoundingVolume(idx);
	entityBoundingVolume->setupSphere(center, radius);
	setupModelBoundingVolume(entity, idx);
}

void EntityPhysicsView::applyBoundingVolumeCapsule(LevelEditorEntity* entity, int idx, const Vector3& a, const Vector3& b, float radius)
{
	if (entity == nullptr) return;
	clearModelBoundingVolume(idx);
	auto entityBoundingVolume = entity->getBoundingVolume(idx);
	entityBoundingVolume->setupCapsule(a, b, radius);
	setupModelBoundingVolume(entity, idx);
}

void EntityPhysicsView::applyBoundingVolumeAabb(LevelEditorEntity* entity, int idx, const Vector3& min, const Vector3& max)
{
	if (entity == nullptr) return;
	clearModelBoundingVolume(idx);
	auto entityBoundingVolume = entity->getBoundingVolume(idx);
	entityBoundingVolume->setupAabb(min, max);
	setupModelBoundingVolume(entity, idx);
}

void EntityPhysicsView::applyBoundingVolumeObb(LevelEditorEntity* entity, int idx, const Vector3& center, const Vector3& axis0, const Vector3& axis1, const Vector3& axis2, const Vector3& halfExtension)
{
	if (entity == nullptr) return;
	clearModelBoundingVolume(idx);
	auto entityBoundingVolume = entity->getBoundingVolume(idx);
	entityBoundingVolume->setupObb(center, axis0, axis1, axis2, halfExtension);
	setupModelBoundingVolume(entity, idx);
}

void EntityPhysicsView::applyBoundingVolumeConvexMesh(LevelEditorEntity* entity, int idx, const string& fileName)
{
	if (entity == nullptr) return;
	clearModelBoundingVolume(idx);
	auto entityBoundingVolume = entity->getBoundingVolume(idx);
	entityBoundingVolume->setupConvexMesh(
		FileSystem::getInstance()->getPathName(fileName),
		FileSystem::getInstance()->getFileName(fileName)
	);
	setupModelBoundingVolume(entity, idx);
}

void EntityPhysicsView::setTerrainMesh(LevelEditorEntity* entity) {
	if (entity == nullptr) return;
	entityPhysicsSubScreenController->setTerrainMesh(entity);
}

void EntityPhysicsView::unsetTerrainMesh() {
	entityPhysicsSubScreenController->unsetTerrainMesh();
}

void EntityPhysicsView::unsetConvexMeshes() {
	entityPhysicsSubScreenController->unsetConvexMeshes();
}

void EntityPhysicsView::setConvexMeshes(LevelEditorEntity* entity) {
	entityPhysicsSubScreenController->setConvexMeshes(entity);
}

void EntityPhysicsView::unsetPhysics() {
	entityPhysicsSubScreenController->unsetPhysics();
}

void EntityPhysicsView::setPhysics(LevelEditorEntity* entity) {
	entityPhysicsSubScreenController->setPhysics(entity);
}

void EntityPhysicsView::display(LevelEditorEntity* entity) {
	if (entity == nullptr) return;

	for (auto i = 0; i < maxBoundingVolumeCount; i++) {
		auto modelBoundingVolume = dynamic_cast<EntityHierarchy*>(engine->getEntity(LevelEditorEntity::MODEL_BOUNDINGVOLUMES_ID))->getEntity(LevelEditorEntity::MODEL_BOUNDINGVOLUME_IDS[i]);
		if (modelBoundingVolume != nullptr) modelBoundingVolume->setEnabled(displayBoundingVolume == true && displayBoundingVolumeIdx == DISPLAY_BOUNDINGVOLUMEIDX_ALL);
	}
	auto modelBoundingVolume = engine->getEntity(LevelEditorEntity::MODEL_BOUNDINGVOLUME_EDITING_ID);
	if (modelBoundingVolume != nullptr) {
		modelBoundingVolume->setEnabled(true);
		modelBoundingVolume->setPickable(true);
	} else
	if (displayBoundingVolumeIdx != DISPLAY_BOUNDINGVOLUMEIDX_ALL) {
		startEditingBoundingVolume(entity);
	}
}

void EntityPhysicsView::handleInputEvents(LevelEditorEntity* entity, const Vector3& objectScale) {
	if (displayBoundingVolumeIdx == DISPLAY_BOUNDINGVOLUMEIDX_ALL || displayBoundingVolumeIdx != displayBoundingVolumeIdxLast) {
		displayBoundingVolumeIdxLast = displayBoundingVolumeIdx;
		removeGizmo();
		return;
	}
	displayBoundingVolumeIdxLast = displayBoundingVolumeIdx;

	// we only support sphere, capsule and obb
	auto bv = entity->getBoundingVolume(displayBoundingVolumeIdx);
	if (bv == nullptr) return;

	if (dynamic_cast<Sphere*>(bv->getBoundingVolume()) == nullptr &&
		dynamic_cast<Capsule*>(bv->getBoundingVolume()) == nullptr &&
		dynamic_cast<OrientedBoundingBox*>(bv->getBoundingVolume()) == nullptr) {
		return;
	}

	for (auto i = 0; i < engine->getGUI()->getKeyboardEvents().size(); i++) {
		auto& event = engine->getGUI()->getKeyboardEvents()[i];
		if (event.isProcessed() == true) continue;
		auto isKeyDown = event.getType() == GUIKeyboardEvent::KEYBOARDEVENT_KEY_PRESSED;
		if (Character::toLowerCase(event.getKeyChar()) == '1') { if (isKeyDown == true) setGizmoType(GIZMOTYPE_ALL); updateGizmo(entity); event.setProcessed(true); }
		if (Character::toLowerCase(event.getKeyChar()) == '2') { if (isKeyDown == true) setGizmoType(GIZMOTYPE_TRANSLATE); updateGizmo(entity); event.setProcessed(true); }
		if (Character::toLowerCase(event.getKeyChar()) == '3') { if (isKeyDown == true) setGizmoType(GIZMOTYPE_ROTATE); updateGizmo(entity); event.setProcessed(true); }
		if (Character::toLowerCase(event.getKeyChar()) == '4') { if (isKeyDown == true) setGizmoType(GIZMOTYPE_SCALE); updateGizmo(entity); event.setProcessed(true); }
	}
	for (auto i = 0; i < engine->getGUI()->getMouseEvents().size(); i++) {
		auto& event = engine->getGUI()->getMouseEvents()[i];

		if (event.isProcessed() == true) continue;

		if (event.getButton() == MOUSE_BUTTON_LEFT) {
			if (event.getType() == GUIMouseEvent::MOUSEEVENT_RELEASED) {
				auto selectedEntity = engine->getEntity(LevelEditorEntity::MODEL_BOUNDINGVOLUME_EDITING_ID);
				if (selectedEntity != nullptr) applyBoundingVolumeTransformations(entity, displayBoundingVolumeIdx, selectedEntity->getTransformations(), objectScale, false);
				if (getGizmoMode() != GIZMOMODE_NONE) {
					setGizmoMode(GIZMOMODE_NONE);
					updateGizmo(entity);
				}
				totalDeltaScale.set(0.0, 0.0f, 0.0f);
				event.setProcessed(true);
			} else
			if (event.getType() == GUIMouseEvent::MOUSEEVENT_PRESSED) {
				Node* selectedEntityNode = nullptr;
				Entity* selectedEntity = nullptr;
				if (getGizmoMode() == GIZMOMODE_NONE) selectedEntity = engine->getEntityByMousePosition(event.getXUnscaled(), event.getYUnscaled(), nullptr, &selectedEntityNode);
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
					updateGizmo(entity);
					totalDeltaScale.set(0.0, 0.0f, 0.0f);
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
						auto selectedEntity = engine->getEntity(LevelEditorEntity::MODEL_BOUNDINGVOLUME_EDITING_ID);
						if (gizmoEntity != nullptr && selectedEntity != nullptr) {
							selectedEntity->setTranslation(selectedEntity->getTranslation().clone().add(deltaTranslation));
							selectedEntity->setScale(selectedEntity->getScale().clone().scale(deltaScale));
							if (selectedEntity->getRotationCount() == 0) {
								selectedEntity->addRotation(Rotation::Z_AXIS, 0.0f);
								selectedEntity->addRotation(Rotation::Y_AXIS, 0.0f);
								selectedEntity->addRotation(Rotation::X_AXIS, 0.0f);
							}
							selectedEntity->setRotationAngle(0, selectedEntity->getRotationAngle(0) + deltaRotation[2]);
							selectedEntity->setRotationAngle(1, selectedEntity->getRotationAngle(1) + deltaRotation[1]);
							selectedEntity->setRotationAngle(2, selectedEntity->getRotationAngle(2) + deltaRotation[0]);
							selectedEntity->update();
							setGizmoRotation(entity, selectedEntity->getTransformations());
							applyBoundingVolumeTransformations(entity, displayBoundingVolumeIdx, selectedEntity->getTransformations(), objectScale, true);
						}
						if (Math::abs(deltaTranslation.getX()) > Math::EPSILON ||
							Math::abs(deltaTranslation.getY()) > Math::EPSILON ||
							Math::abs(deltaTranslation.getZ()) > Math::EPSILON) {
							updateGizmo(entity);
						}
					}
					mouseDownLastX = event.getXUnscaled();
					mouseDownLastY = event.getYUnscaled();
					event.setProcessed(true);
				}
			}
		}
	}
}

void EntityPhysicsView::updateGizmo(LevelEditorEntity* entity) {
	auto selectedEntity = engine->getEntity(LevelEditorEntity::MODEL_BOUNDINGVOLUME_EDITING_ID);
	if (selectedEntity != nullptr) {
		Gizmo::updateGizmo(selectedEntity->getBoundingBoxTransformed()->getCenter(), selectedEntity->getTransformations());
	} else {
		removeGizmo();
	}
}

void EntityPhysicsView::setGizmoRotation(LevelEditorEntity* entity, const Transformations& transformations) {
	if (displayBoundingVolumeIdx == DISPLAY_BOUNDINGVOLUMEIDX_ALL) return;
	Gizmo::setGizmoRotation(transformations);
}

void EntityPhysicsView::applyBoundingVolumeTransformations(LevelEditorEntity* entity, int i, const Transformations& _transformations, const Vector3& objectScale, bool guiOnly) {
	auto modelEntity = engine->getEntity("model");
	auto transformations = _transformations;
	auto objectScaleInverted = Vector3(
		1.0f / objectScale.getX(),
		1.0f / objectScale.getY(),
		1.0f / objectScale.getZ()
	);
	transformations.setScale(transformations.getScale().clone().scale(objectScaleInverted));
	transformations.update();
	auto bv = entity->getBoundingVolume(i);
	if (dynamic_cast<Sphere*>(bv->getBoundingVolume()) != nullptr) {
		auto sphere = dynamic_cast<Sphere*>(bv->getBoundingVolume());
		auto center = sphere->getCenter().clone().add(transformations.getTranslation().clone().scale(objectScaleInverted));
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
		auto radius = Math::clamp(sphere->getRadius() * Math::abs(scale), 0.01f, 1000.0f);
		entityPhysicsSubScreenController->setupSphere(i, center, radius);
		if (guiOnly == false) applyBoundingVolumeSphere(entity, i, center, radius);
	} else
	if (dynamic_cast<Capsule*>(bv->getBoundingVolume()) != nullptr) {
		auto capsule = dynamic_cast<Capsule*>(bv->getBoundingVolume());
		auto a = capsule->getA();
		auto b = capsule->getB();
		transformations.setTranslation(transformations.getTranslation().clone().scale(objectScaleInverted));
		transformations.setPivot(transformations.getPivot().clone().scale(objectScaleInverted));
		transformations.update();
		transformations.getTransformationsMatrix().multiply(a, a);
		transformations.getTransformationsMatrix().multiply(b, b);
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
		auto radius = Math::clamp(capsule->getRadius() * Math::abs(scale), 0.01f, 1000.0f);
		entityPhysicsSubScreenController->setupCapsule(i, a, b, radius);
		if (guiOnly == false) applyBoundingVolumeCapsule(entity, i, a, b, radius);
	} else
	if (dynamic_cast<OrientedBoundingBox*>(bv->getBoundingVolume()) != nullptr) {
		auto obb = dynamic_cast<OrientedBoundingBox*>(bv->getBoundingVolume());
		auto center = transformations.getTranslation().clone().scale(objectScaleInverted);
		auto axis0 = OrientedBoundingBox::AABB_AXIS_X;
		auto axis1 = OrientedBoundingBox::AABB_AXIS_Y;
		auto axis2 = OrientedBoundingBox::AABB_AXIS_Z;
		auto halfExtension = obb->getHalfExtension();
		transformations.getTransformationsMatrix().multiplyNoTranslation(axis0, axis0);
		transformations.getTransformationsMatrix().multiplyNoTranslation(axis1, axis1);
		transformations.getTransformationsMatrix().multiplyNoTranslation(axis2, axis2);
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
		if ((boundingVolumeTypeMask & EntityPhysicsSubScreenController::BOUNDINGVOLUMETYPE_ORIENTEDBOUNDINGBOX) == EntityPhysicsSubScreenController::BOUNDINGVOLUMETYPE_ORIENTEDBOUNDINGBOX) {
			entityPhysicsSubScreenController->setupOrientedBoundingBox(i, center, axis0, axis1, axis2, halfExtension);
			if (guiOnly == false) applyBoundingVolumeObb(entity, i, center, axis0, axis1, axis2, halfExtension);
		} else
		if ((boundingVolumeTypeMask & EntityPhysicsSubScreenController::BOUNDINGVOLUMETYPE_BOUNDINGBOX) == EntityPhysicsSubScreenController::BOUNDINGVOLUMETYPE_BOUNDINGBOX) {
			OrientedBoundingBox obb(center, axis0, axis1, axis2, halfExtension);
			BoundingBox aabb(&obb);
			entityPhysicsSubScreenController->setupBoundingBox(i, aabb.getMin(), aabb.getMax());
			if (guiOnly == false) applyBoundingVolumeAabb(entity, i, aabb.getMin(), aabb.getMax());
		}
	}
}

void EntityPhysicsView::startEditingBoundingVolume(LevelEditorEntity* entity) {
	engine->removeEntity(LevelEditorEntity::MODEL_BOUNDINGVOLUME_EDITING_ID);
	setupModelBoundingVolume(entity, displayBoundingVolumeIdx);
}

void EntityPhysicsView::endEditingBoundingVolume(LevelEditorEntity* entity) {
	engine->removeEntity(LevelEditorEntity::MODEL_BOUNDINGVOLUME_EDITING_ID);
}

bool EntityPhysicsView::isEditingBoundingVolume(LevelEditorEntity* entity) {
	return displayBoundingVolumeIdx != DISPLAY_BOUNDINGVOLUMEIDX_ALL;
}
