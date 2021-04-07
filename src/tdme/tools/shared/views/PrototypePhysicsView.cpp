#include <tdme/tools/shared/views/PrototypePhysicsView.h>

#include <tdme/math/Math.h>

#include <tdme/engine/model/Model.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/primitives/BoundingVolume.h>
#include <tdme/engine/primitives/Capsule.h>
#include <tdme/engine/primitives/ConvexMesh.h>
#include <tdme/engine/primitives/OrientedBoundingBox.h>
#include <tdme/engine/primitives/Sphere.h>
#include <tdme/engine/primitives/Sphere.h>
#include <tdme/engine/prototype/Prototype.h>
#include <tdme/engine/prototype/PrototypeBoundingVolume.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Entity.h>
#include <tdme/engine/EntityHierarchy.h>
#include <tdme/engine/Object3D.h>
#include <tdme/engine/Rotation.h>
#include <tdme/engine/Transformations.h>
#include <tdme/gui/events/GUIKeyboardEvent.h>
#include <tdme/gui/events/GUIMouseEvent.h>
#include <tdme/gui/GUI.h>
#include <tdme/math/Math.h>
#include <tdme/math/Vector3.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/tools/shared/controller/PrototypePhysicsSubScreenController.h>
#include <tdme/tools/shared/controller/PrototypePhysicsSubScreenController_BoundingVolumeType.h>
#include <tdme/tools/shared/tools/Tools.h>
#include <tdme/tools/shared/views/Gizmo.h>
#include <tdme/tools/shared/views/PopUps.h>
#include <tdme/utilities/Character.h>
#include <tdme/utilities/Console.h>

using tdme::tools::shared::views::PrototypePhysicsView;

using tdme::math::Math;

using tdme::engine::model::Model;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::primitives::BoundingVolume;
using tdme::engine::primitives::Capsule;
using tdme::engine::primitives::ConvexMesh;
using tdme::engine::primitives::OrientedBoundingBox;
using tdme::engine::primitives::Sphere;
using tdme::engine::prototype::Prototype;
using tdme::engine::prototype::PrototypeBoundingVolume;
using tdme::engine::Engine;
using tdme::engine::Entity;
using tdme::engine::EntityHierarchy;
using tdme::engine::Object3D;
using tdme::engine::Rotation;
using tdme::engine::Transformations;
using tdme::gui::events::GUIKeyboardEvent;
using tdme::gui::events::GUIMouseEvent;
using tdme::gui::GUI;
using tdme::math::Math;
using tdme::math::Vector3;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;
using tdme::tools::shared::controller::PrototypePhysicsSubScreenController;
using tdme::tools::shared::controller::PrototypePhysicsSubScreenController_BoundingVolumeType;
using tdme::tools::shared::tools::Tools;
using tdme::tools::shared::views::Gizmo;
using tdme::tools::shared::views::PopUps;
using tdme::utilities::Character;
using tdme::utilities::Console;

PrototypePhysicsView::PrototypePhysicsView(PrototypePhysicsSubScreenController* prototypePhysicsSubScreenController, PopUps* popUps, int maxBoundingVolumeCount, int32_t boundingVolumeTypeMask): Gizmo(Engine::getInstance(), "epv")
{
	this->engine = Engine::getInstance();
	this->popUps = popUps;
	this->prototypePhysicsSubScreenController = prototypePhysicsSubScreenController;
	this->maxBoundingVolumeCount = maxBoundingVolumeCount == -1?Prototype::MODEL_BOUNDINGVOLUME_COUNT:maxBoundingVolumeCount;
	this->mouseDownLastX = MOUSE_DOWN_LAST_POSITION_NONE;
	this->mouseDownLastY = MOUSE_DOWN_LAST_POSITION_NONE;
	this->displayBoundingVolumeIdx = DISPLAY_BOUNDINGVOLUMEIDX_ALL;
	this->displayBoundingVolumeIdxLast = DISPLAY_BOUNDINGVOLUMEIDX_ALL;
	this->displayBoundingVolume = false;
	this->boundingVolumeTypeMask = boundingVolumeTypeMask;
}

PrototypePhysicsView::~PrototypePhysicsView() {
}

PopUps* PrototypePhysicsView::getPopUps()
{
	return popUps;
}

void PrototypePhysicsView::initialize()
{
	for (auto i = 0; i < maxBoundingVolumeCount; i++) {
		prototypePhysicsSubScreenController->setupBoundingVolumeTypes(i, boundingVolumeTypeMask);
		prototypePhysicsSubScreenController->selectBoundingVolume(i, PrototypePhysicsSubScreenController_BoundingVolumeType::NONE);
	}
}

void PrototypePhysicsView::resetBoundingVolume(Prototype* prototype, int idx, int type)
{
	BoundingBox aabb;
	if (prototype->getModel() != nullptr) {
		aabb = *prototype->getModel()->getBoundingBox();
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
	auto boundingVolume = prototype->getBoundingVolume(idx);
	if (type == 0) {
		prototypePhysicsSubScreenController->selectBoundingVolume(idx, PrototypePhysicsSubScreenController_BoundingVolumeType::NONE);
	} else
	if (type == 1) {
		prototypePhysicsSubScreenController->setupSphere(idx, obb.getCenter(), obb.getHalfExtension().computeLength());
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
		prototypePhysicsSubScreenController->setupCapsule(idx, a, b, radius);
	} else
	if (type == 3) {
		prototypePhysicsSubScreenController->setupBoundingBox(idx, aabb.getMin(), aabb.getMax());
	} else
	if (type == 4) {
		prototypePhysicsSubScreenController->setupOrientedBoundingBox(idx, obb.getCenter(), obb.getAxes()[0], obb.getAxes()[1], obb.getAxes()[2], obb.getHalfExtension());
	}
}

void PrototypePhysicsView::setBoundingVolumes(Prototype* prototype)
{
	for (auto i = 0; i < maxBoundingVolumeCount; i++) {
		resetBoundingVolume(prototype, i, 0);
	}
	for (auto i = 0; i < prototype->getBoundingVolumeCount(); i++) {
		auto bv = prototype->getBoundingVolume(i);
		if (bv == nullptr) {
			prototypePhysicsSubScreenController->selectBoundingVolume(i, PrototypePhysicsSubScreenController_BoundingVolumeType::NONE);
			continue;
		} else
		if (dynamic_cast<Sphere*>(bv->getBoundingVolume()) != nullptr) {
			auto sphere = dynamic_cast< Sphere* >(bv->getBoundingVolume());
			prototypePhysicsSubScreenController->setupSphere(i, sphere->getCenter(), sphere->getRadius());
			prototypePhysicsSubScreenController->selectBoundingVolume(i, PrototypePhysicsSubScreenController_BoundingVolumeType::SPHERE);
			prototypePhysicsSubScreenController->setupModelBoundingVolumeType(prototype, i);
		} else
		if (dynamic_cast<Capsule*>(bv->getBoundingVolume()) != nullptr) {
			auto capsule = dynamic_cast< Capsule* >(bv->getBoundingVolume());
			prototypePhysicsSubScreenController->setupCapsule(i, capsule->getA(), capsule->getB(), capsule->getRadius());
			prototypePhysicsSubScreenController->selectBoundingVolume(i, PrototypePhysicsSubScreenController_BoundingVolumeType::CAPSULE);
			prototypePhysicsSubScreenController->setupModelBoundingVolumeType(prototype, i);
		} else
		if (dynamic_cast<BoundingBox*>(bv->getBoundingVolume()) != nullptr) {
			auto aabb = dynamic_cast< BoundingBox* >(bv->getBoundingVolume());
			prototypePhysicsSubScreenController->setupBoundingBox(i, aabb->getMin(), aabb->getMax());
			prototypePhysicsSubScreenController->selectBoundingVolume(i, PrototypePhysicsSubScreenController_BoundingVolumeType::BOUNDINGBOX);
			prototypePhysicsSubScreenController->setupModelBoundingVolumeType(prototype, i);
		} else
		if (dynamic_cast<OrientedBoundingBox*>(bv->getBoundingVolume()) != nullptr) {
			if ((boundingVolumeTypeMask & PrototypePhysicsSubScreenController::BOUNDINGVOLUMETYPE_ORIENTEDBOUNDINGBOX) == PrototypePhysicsSubScreenController::BOUNDINGVOLUMETYPE_ORIENTEDBOUNDINGBOX) {
				auto obb = dynamic_cast< OrientedBoundingBox* >(bv->getBoundingVolume());
				prototypePhysicsSubScreenController->setupOrientedBoundingBox(i, obb->getCenter(), obb->getAxes()[0], obb->getAxes()[1], obb->getAxes()[2], obb->getHalfExtension());
				prototypePhysicsSubScreenController->selectBoundingVolume(i, PrototypePhysicsSubScreenController_BoundingVolumeType::ORIENTEDBOUNDINGBOX);
				prototypePhysicsSubScreenController->setupModelBoundingVolumeType(prototype, i);
			} else
			if ((boundingVolumeTypeMask & PrototypePhysicsSubScreenController::BOUNDINGVOLUMETYPE_BOUNDINGBOX) == PrototypePhysicsSubScreenController::BOUNDINGVOLUMETYPE_BOUNDINGBOX) {
				auto obb = dynamic_cast< OrientedBoundingBox* >(bv->getBoundingVolume());
				BoundingBox aabb(obb);
				prototypePhysicsSubScreenController->setupBoundingBox(i, aabb.getMin(), aabb.getMax());
				prototypePhysicsSubScreenController->selectBoundingVolume(i, PrototypePhysicsSubScreenController_BoundingVolumeType::BOUNDINGBOX);
				prototypePhysicsSubScreenController->getView()->selectBoundingVolumeType(i, 3);
			}
		} else
		if (dynamic_cast<ConvexMesh*>(bv->getBoundingVolume()) != nullptr) {
			prototypePhysicsSubScreenController->setupConvexMesh(i, bv->getModelMeshFile());
			prototypePhysicsSubScreenController->selectBoundingVolume(i, PrototypePhysicsSubScreenController_BoundingVolumeType::CONVEXMESH);
			prototypePhysicsSubScreenController->setupModelBoundingVolumeType(prototype, i);
		}
		prototypePhysicsSubScreenController->enableBoundingVolume(i);
	}
}

void PrototypePhysicsView::unsetBoundingVolumes()
{
	for (auto i = 0; i < maxBoundingVolumeCount; i++) {
		prototypePhysicsSubScreenController->disableBoundingVolume(i);
	}
}

void PrototypePhysicsView::selectBoundingVolumeType(int idx, int bvTypeId)
{
	switch (bvTypeId) {
		case 0:
			prototypePhysicsSubScreenController->selectBoundingVolume(idx, PrototypePhysicsSubScreenController_BoundingVolumeType::NONE);
			break;
		case 1:
			prototypePhysicsSubScreenController->selectBoundingVolume(idx, PrototypePhysicsSubScreenController_BoundingVolumeType::SPHERE);
			break;
		case 2:
			prototypePhysicsSubScreenController->selectBoundingVolume(idx, PrototypePhysicsSubScreenController_BoundingVolumeType::CAPSULE);
			break;
		case 3:
			prototypePhysicsSubScreenController->selectBoundingVolume(idx, PrototypePhysicsSubScreenController_BoundingVolumeType::BOUNDINGBOX);
			break;
		case 4:
			prototypePhysicsSubScreenController->selectBoundingVolume(idx, PrototypePhysicsSubScreenController_BoundingVolumeType::ORIENTEDBOUNDINGBOX);
			break;
		case 5:
			prototypePhysicsSubScreenController->selectBoundingVolume(idx, PrototypePhysicsSubScreenController_BoundingVolumeType::CONVEXMESH);
			break;
	}
}

void PrototypePhysicsView::clearModelBoundingVolume(int idx) {
	if (idx != DISPLAY_BOUNDINGVOLUMEIDX_ALL) {
		dynamic_cast<EntityHierarchy*>(engine->getEntity(Prototype::MODEL_BOUNDINGVOLUMES_ID))->removeEntity(Prototype::MODEL_BOUNDINGVOLUME_IDS[idx]);
	}
	engine->removeEntity(Prototype::MODEL_BOUNDINGVOLUME_EDITING_ID);
}

void PrototypePhysicsView::setupModelBoundingVolume(Prototype* prototype, int idx)
{
	auto prototypeBoundingVolume = prototype->getBoundingVolume(idx);
	if (prototypeBoundingVolume == nullptr) return;

	{
		Vector3 objectScale(1.0f, 1.0f, 1.0f);
		auto modelEntity = engine->getEntity("model");
		if (modelEntity != nullptr) objectScale = modelEntity->getScale();
		auto modelBoundingVolumeEntityId = Prototype::MODEL_BOUNDINGVOLUME_IDS[idx];
		if (prototypeBoundingVolume->getModel() != nullptr) {
			auto modelBoundingVolumeEntity = new Object3D(modelBoundingVolumeEntityId, prototypeBoundingVolume->getModel());
			modelBoundingVolumeEntity->setScale(objectScale);
			modelBoundingVolumeEntity->setRenderPass(Entity::RENDERPASS_POST_POSTPROCESSING);
			modelBoundingVolumeEntity->setEnabled(false);
			modelBoundingVolumeEntity->update();
			dynamic_cast<EntityHierarchy*>(engine->getEntity(Prototype::MODEL_BOUNDINGVOLUMES_ID))->addEntity(modelBoundingVolumeEntity);
		}
	}
	if (displayBoundingVolumeIdx == idx) {
		auto modelBoundingVolumeEntityId = Prototype::MODEL_BOUNDINGVOLUME_EDITING_ID;
		auto boundingVolumesEntity = engine->getEntity(Prototype::MODEL_BOUNDINGVOLUMES_ID);
		if (prototypeBoundingVolume->getModel() != nullptr) {
			auto bv = prototype->getBoundingVolume(idx);
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
				auto modelBoundingVolumeEntity = new Object3D(modelBoundingVolumeEntityId, prototypeBoundingVolume->getModel());
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

void PrototypePhysicsView::applyBoundingVolumeNone(Prototype* prototype, int idx)
{
	if (prototype == nullptr) return;
	clearModelBoundingVolume(idx);
	auto prototypeBoundingVolume = prototype->getBoundingVolume(idx);
	prototypeBoundingVolume->setupNone();
	setupModelBoundingVolume(prototype, idx);
}

void PrototypePhysicsView::applyBoundingVolumeSphere(Prototype* prototype, int idx, const Vector3& center, float radius)
{
	if (prototype == nullptr) return;
	clearModelBoundingVolume(idx);
	auto prototypeBoundingVolume = prototype->getBoundingVolume(idx);
	prototypeBoundingVolume->setupSphere(center, radius);
	setupModelBoundingVolume(prototype, idx);
}

void PrototypePhysicsView::applyBoundingVolumeCapsule(Prototype* prototype, int idx, const Vector3& a, const Vector3& b, float radius)
{
	if (prototype == nullptr) return;
	clearModelBoundingVolume(idx);
	auto prototypeBoundingVolume = prototype->getBoundingVolume(idx);
	prototypeBoundingVolume->setupCapsule(a, b, radius);
	setupModelBoundingVolume(prototype, idx);
}

void PrototypePhysicsView::applyBoundingVolumeAabb(Prototype* prototype, int idx, const Vector3& min, const Vector3& max)
{
	if (prototype == nullptr) return;
	clearModelBoundingVolume(idx);
	auto prototypeBoundingVolume = prototype->getBoundingVolume(idx);
	prototypeBoundingVolume->setupAabb(min, max);
	setupModelBoundingVolume(prototype, idx);
}

void PrototypePhysicsView::applyBoundingVolumeObb(Prototype* prototype, int idx, const Vector3& center, const Vector3& axis0, const Vector3& axis1, const Vector3& axis2, const Vector3& halfExtension)
{
	if (prototype == nullptr) return;
	clearModelBoundingVolume(idx);
	auto prototypeBoundingVolume = prototype->getBoundingVolume(idx);
	prototypeBoundingVolume->setupObb(center, axis0, axis1, axis2, halfExtension);
	setupModelBoundingVolume(prototype, idx);
}

void PrototypePhysicsView::applyBoundingVolumeConvexMesh(Prototype* prototype, int idx, const string& fileName)
{
	if (prototype == nullptr) return;
	clearModelBoundingVolume(idx);
	auto prototypeBoundingVolume = prototype->getBoundingVolume(idx);
	prototypeBoundingVolume->setupConvexMesh(
		FileSystem::getInstance()->getPathName(fileName),
		FileSystem::getInstance()->getFileName(fileName)
	);
	setupModelBoundingVolume(prototype, idx);
}

void PrototypePhysicsView::setTerrainMesh(Prototype* prototype) {
	if (prototype == nullptr) return;
	prototypePhysicsSubScreenController->setTerrainMesh(prototype);
}

void PrototypePhysicsView::unsetTerrainMesh() {
	prototypePhysicsSubScreenController->unsetTerrainMesh();
}

void PrototypePhysicsView::unsetConvexMeshes() {
	prototypePhysicsSubScreenController->unsetConvexMeshes();
}

void PrototypePhysicsView::setConvexMeshes(Prototype* prototype) {
	prototypePhysicsSubScreenController->setConvexMeshes(prototype);
}

void PrototypePhysicsView::unsetPhysics() {
	prototypePhysicsSubScreenController->unsetPhysics();
}

void PrototypePhysicsView::setPhysics(Prototype* prototype) {
	prototypePhysicsSubScreenController->setPhysics(prototype);
}

void PrototypePhysicsView::display(Prototype* prototype) {
	if (prototype == nullptr) return;

	for (auto i = 0; i < maxBoundingVolumeCount; i++) {
		auto modelBoundingVolume = dynamic_cast<EntityHierarchy*>(engine->getEntity(Prototype::MODEL_BOUNDINGVOLUMES_ID))->getEntity(Prototype::MODEL_BOUNDINGVOLUME_IDS[i]);
		if (modelBoundingVolume != nullptr) modelBoundingVolume->setEnabled(displayBoundingVolume == true && displayBoundingVolumeIdx == DISPLAY_BOUNDINGVOLUMEIDX_ALL);
	}
	auto modelBoundingVolume = engine->getEntity(Prototype::MODEL_BOUNDINGVOLUME_EDITING_ID);
	if (modelBoundingVolume != nullptr) {
		modelBoundingVolume->setEnabled(true);
		modelBoundingVolume->setPickable(true);
	} else
	if (displayBoundingVolumeIdx != DISPLAY_BOUNDINGVOLUMEIDX_ALL) {
		startEditingBoundingVolume(prototype);
	}
}

void PrototypePhysicsView::handleInputEvents(Prototype* prototype, const Vector3& objectScale) {
	if (displayBoundingVolumeIdx == DISPLAY_BOUNDINGVOLUMEIDX_ALL || displayBoundingVolumeIdx != displayBoundingVolumeIdxLast) {
		displayBoundingVolumeIdxLast = displayBoundingVolumeIdx;
		removeGizmo();
		return;
	}
	displayBoundingVolumeIdxLast = displayBoundingVolumeIdx;

	// we only support sphere, capsule and obb
	auto bv = prototype->getBoundingVolume(displayBoundingVolumeIdx);
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
				auto selectedEntity = engine->getEntity(Prototype::MODEL_BOUNDINGVOLUME_EDITING_ID);
				if (selectedEntity != nullptr) applyBoundingVolumeTransformations(prototype, displayBoundingVolumeIdx, selectedEntity->getTransformations(), objectScale, false);
				if (getGizmoMode() != GIZMOMODE_NONE) {
					setGizmoMode(GIZMOMODE_NONE);
					updateGizmo(prototype);
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
					updateGizmo(prototype);
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
						auto selectedEntity = engine->getEntity(Prototype::MODEL_BOUNDINGVOLUME_EDITING_ID);
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
							setGizmoRotation(prototype, selectedEntity->getTransformations());
							applyBoundingVolumeTransformations(prototype, displayBoundingVolumeIdx, selectedEntity->getTransformations(), objectScale, true);
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
}

void PrototypePhysicsView::updateGizmo(Prototype* prototype) {
	auto selectedEntity = engine->getEntity(Prototype::MODEL_BOUNDINGVOLUME_EDITING_ID);
	if (selectedEntity != nullptr) {
		Gizmo::updateGizmo(selectedEntity->getBoundingBoxTransformed()->getCenter(), selectedEntity->getTransformations());
	} else {
		removeGizmo();
	}
}

void PrototypePhysicsView::setGizmoRotation(Prototype* prototype, const Transformations& transformations) {
	if (displayBoundingVolumeIdx == DISPLAY_BOUNDINGVOLUMEIDX_ALL) return;
	Gizmo::setGizmoRotation(transformations);
}

void PrototypePhysicsView::applyBoundingVolumeTransformations(Prototype* prototype, int i, const Transformations& _transformations, const Vector3& objectScale, bool guiOnly) {
	auto modelEntity = engine->getEntity("model");
	auto transformations = _transformations;
	auto objectScaleInverted = Vector3(
		1.0f / objectScale.getX(),
		1.0f / objectScale.getY(),
		1.0f / objectScale.getZ()
	);
	transformations.setScale(transformations.getScale().clone().scale(objectScaleInverted));
	transformations.update();
	auto bv = prototype->getBoundingVolume(i);
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
		prototypePhysicsSubScreenController->setupSphere(i, center, radius);
		if (guiOnly == false) applyBoundingVolumeSphere(prototype, i, center, radius);
	} else
	if (dynamic_cast<Capsule*>(bv->getBoundingVolume()) != nullptr) {
		auto capsule = dynamic_cast<Capsule*>(bv->getBoundingVolume());
		auto a = capsule->getA();
		auto b = capsule->getB();
		transformations.setTranslation(transformations.getTranslation().clone().scale(objectScaleInverted));
		transformations.setPivot(transformations.getPivot().clone().scale(objectScaleInverted));
		transformations.update();
		a = transformations.getTransformationsMatrix().multiply(a);
		b = transformations.getTransformationsMatrix().multiply(b);
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
		prototypePhysicsSubScreenController->setupCapsule(i, a, b, radius);
		if (guiOnly == false) applyBoundingVolumeCapsule(prototype, i, a, b, radius);
	} else
	if (dynamic_cast<OrientedBoundingBox*>(bv->getBoundingVolume()) != nullptr) {
		auto obb = dynamic_cast<OrientedBoundingBox*>(bv->getBoundingVolume());
		auto center = transformations.getTranslation().clone().scale(objectScaleInverted);
		auto axis0 = OrientedBoundingBox::AABB_AXIS_X;
		auto axis1 = OrientedBoundingBox::AABB_AXIS_Y;
		auto axis2 = OrientedBoundingBox::AABB_AXIS_Z;
		auto halfExtension = obb->getHalfExtension();
		axis0 = transformations.getTransformationsMatrix().multiplyNoTranslation(axis0);
		axis1 = transformations.getTransformationsMatrix().multiplyNoTranslation(axis1);
		axis2 = transformations.getTransformationsMatrix().multiplyNoTranslation(axis2);
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
		if ((boundingVolumeTypeMask & PrototypePhysicsSubScreenController::BOUNDINGVOLUMETYPE_ORIENTEDBOUNDINGBOX) == PrototypePhysicsSubScreenController::BOUNDINGVOLUMETYPE_ORIENTEDBOUNDINGBOX) {
			prototypePhysicsSubScreenController->setupOrientedBoundingBox(i, center, axis0, axis1, axis2, halfExtension);
			if (guiOnly == false) applyBoundingVolumeObb(prototype, i, center, axis0, axis1, axis2, halfExtension);
		} else
		if ((boundingVolumeTypeMask & PrototypePhysicsSubScreenController::BOUNDINGVOLUMETYPE_BOUNDINGBOX) == PrototypePhysicsSubScreenController::BOUNDINGVOLUMETYPE_BOUNDINGBOX) {
			OrientedBoundingBox obb(center, axis0, axis1, axis2, halfExtension);
			BoundingBox aabb(&obb);
			prototypePhysicsSubScreenController->setupBoundingBox(i, aabb.getMin(), aabb.getMax());
			if (guiOnly == false) applyBoundingVolumeAabb(prototype, i, aabb.getMin(), aabb.getMax());
		}
	}
}

void PrototypePhysicsView::startEditingBoundingVolume(Prototype* prototype) {
	engine->removeEntity(Prototype::MODEL_BOUNDINGVOLUME_EDITING_ID);
	setupModelBoundingVolume(prototype, displayBoundingVolumeIdx);
}

void PrototypePhysicsView::endEditingBoundingVolume(Prototype* prototype) {
	engine->removeEntity(Prototype::MODEL_BOUNDINGVOLUME_EDITING_ID);
}

bool PrototypePhysicsView::isEditingBoundingVolume(Prototype* prototype) {
	return displayBoundingVolumeIdx != DISPLAY_BOUNDINGVOLUMEIDX_ALL;
}
