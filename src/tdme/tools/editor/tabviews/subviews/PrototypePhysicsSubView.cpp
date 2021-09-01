#include <tdme/tools/editor/tabviews/subviews/PrototypePhysicsSubView.h>

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
#include <tdme/tools/editor/misc/Gizmo.h>
#include <tdme/tools/editor/misc/PopUps.h>
#include <tdme/tools/editor/misc/Tools.h>
#include <tdme/tools/editor/tabcontrollers/subcontrollers/PrototypePhysicsSubController.h>
#include <tdme/tools/editor/tabcontrollers/subcontrollers/PrototypePhysicsSubController_BoundingVolumeType.h>
#include <tdme/utilities/Character.h>
#include <tdme/utilities/Console.h>

using tdme::tools::editor::tabviews::subviews::PrototypePhysicsSubView;

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
using tdme::tools::editor::misc::Gizmo;
using tdme::tools::editor::misc::PopUps;
using tdme::tools::editor::misc::Tools;
using tdme::tools::editor::tabcontrollers::subcontrollers::PrototypePhysicsSubController;
using tdme::tools::editor::tabcontrollers::subcontrollers::PrototypePhysicsSubController_BoundingVolumeType;
using tdme::utilities::Character;
using tdme::utilities::Console;

PrototypePhysicsSubView::PrototypePhysicsSubView(Engine* engine, PrototypePhysicsSubController* prototypePhysicsSubController, PopUps* popUps, int maxBoundingVolumeCount, int32_t boundingVolumeTypeMask): Gizmo(engine, "epv")
{
	this->engine = engine;
	this->popUps = popUps;
	this->prototypePhysicsSubController = prototypePhysicsSubController;
	this->maxBoundingVolumeCount = maxBoundingVolumeCount;
	this->mouseDownLastX = MOUSE_DOWN_LAST_POSITION_NONE;
	this->mouseDownLastY = MOUSE_DOWN_LAST_POSITION_NONE;
	this->displayBoundingVolumeIdx = DISPLAY_BOUNDINGVOLUMEIDX_ALL;
	this->displayBoundingVolume = false;
	this->boundingVolumeTypeMask = boundingVolumeTypeMask;
}

PrototypePhysicsSubView::~PrototypePhysicsSubView() {
}

PopUps* PrototypePhysicsSubView::getPopUps()
{
	return popUps;
}

void PrototypePhysicsSubView::clearModelBoundingVolume(int idx) {
	if (idx != DISPLAY_BOUNDINGVOLUMEIDX_ALL) {
		dynamic_cast<EntityHierarchy*>(engine->getEntity("tdme.prototype.bvs"))->removeEntity("tdme.prototype.bv." + to_string(idx));
	}
	engine->removeEntity("tdme.prototype.bv.editing");
}

void PrototypePhysicsSubView::setupModelBoundingVolume(Prototype* prototype, int idx)
{
	auto prototypeBoundingVolume = prototype->getBoundingVolume(idx);
	if (prototypeBoundingVolume == nullptr) return;

	{
		Vector3 objectScale(0.15f, 0.15f, 0.15f); // TODO: have the default not as fixed constant here (this relates to bounding box fallback in Tools::setupPrototype() if none available)
		auto modelEntity = engine->getEntity("model");
		if (modelEntity != nullptr) objectScale = modelEntity->getScale();
		auto modelBoundingVolumeEntityId = "tdme.prototype.bv." + to_string(idx);
		if (prototypeBoundingVolume->getModel() != nullptr) {
			auto modelBoundingVolumeEntity = new Object3D(modelBoundingVolumeEntityId, prototypeBoundingVolume->getModel());
			modelBoundingVolumeEntity->setScale(objectScale);
			modelBoundingVolumeEntity->setRenderPass(Entity::RENDERPASS_POST_POSTPROCESSING);
			modelBoundingVolumeEntity->setEnabled(false);
			modelBoundingVolumeEntity->update();
			dynamic_cast<EntityHierarchy*>(engine->getEntity("tdme.prototype.bvs"))->addEntity(modelBoundingVolumeEntity);
		}
	}
	if (displayBoundingVolumeIdx == idx) {
		auto modelBoundingVolumeEntityId = "tdme.prototype.bv.editing";
		auto boundingVolumesEntity = engine->getEntity("tdme.prototype.bvs");
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

void PrototypePhysicsSubView::applyBoundingVolumeNone(Prototype* prototype, int idx)
{
	if (prototype == nullptr) return;
	clearModelBoundingVolume(idx);
	auto prototypeBoundingVolume = prototype->getBoundingVolume(idx);
	prototypeBoundingVolume->setupNone();
	setupModelBoundingVolume(prototype, idx);
}

void PrototypePhysicsSubView::applyBoundingVolumeSphere(Prototype* prototype, int idx, const Vector3& center, float radius)
{
	if (prototype == nullptr) return;
	clearModelBoundingVolume(idx);
	auto prototypeBoundingVolume = prototype->getBoundingVolume(idx);
	prototypeBoundingVolume->setupSphere(center, radius);
	setupModelBoundingVolume(prototype, idx);
}

void PrototypePhysicsSubView::applyBoundingVolumeCapsule(Prototype* prototype, int idx, const Vector3& a, const Vector3& b, float radius)
{
	if (prototype == nullptr) return;
	clearModelBoundingVolume(idx);
	auto prototypeBoundingVolume = prototype->getBoundingVolume(idx);
	prototypeBoundingVolume->setupCapsule(a, b, radius);
	setupModelBoundingVolume(prototype, idx);
}

void PrototypePhysicsSubView::applyBoundingVolumeAabb(Prototype* prototype, int idx, const Vector3& min, const Vector3& max)
{
	if (prototype == nullptr) return;
	clearModelBoundingVolume(idx);
	auto prototypeBoundingVolume = prototype->getBoundingVolume(idx);
	prototypeBoundingVolume->setupAabb(min, max);
	setupModelBoundingVolume(prototype, idx);
}

void PrototypePhysicsSubView::applyBoundingVolumeObb(Prototype* prototype, int idx, const Vector3& center, const Vector3& axis0, const Vector3& axis1, const Vector3& axis2, const Vector3& halfExtension)
{
	if (prototype == nullptr) return;
	clearModelBoundingVolume(idx);
	auto prototypeBoundingVolume = prototype->getBoundingVolume(idx);
	prototypeBoundingVolume->setupObb(center, axis0, axis1, axis2, halfExtension);
	setupModelBoundingVolume(prototype, idx);
}

void PrototypePhysicsSubView::applyBoundingVolumeConvexMeshClear(Prototype* prototype, int idx)
{
	if (prototype == nullptr) return;
	clearModelBoundingVolume(idx);
	auto prototypeBoundingVolume = prototype->getBoundingVolume(idx);
	prototypeBoundingVolume->clearConvexMesh();
	setupModelBoundingVolume(prototype, idx);
}


void PrototypePhysicsSubView::applyBoundingVolumeConvexMesh(Prototype* prototype, int idx, const string& fileName)
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

void PrototypePhysicsSubView::display(Prototype* prototype) {
	if (prototype == nullptr) return;

	auto bvsEntityHierarchy = dynamic_cast<EntityHierarchy*>(engine->getEntity("tdme.prototype.bvs"));
	for (auto i = 0; i < bvsEntityHierarchy->getEntities().size(); i++) {
		auto modelBoundingVolume = bvsEntityHierarchy->getEntity("tdme.prototype.bv." + to_string(i));
		if (modelBoundingVolume != nullptr) modelBoundingVolume->setEnabled(displayBoundingVolume == true && displayBoundingVolumeIdx == DISPLAY_BOUNDINGVOLUMEIDX_ALL);
	}
	auto modelBoundingVolume = engine->getEntity("tdme.prototype.bv.editing");
	if (modelBoundingVolume != nullptr) {
		modelBoundingVolume->setEnabled(true);
		modelBoundingVolume->setPickable(true);
	} else
	if (displayBoundingVolumeIdx != DISPLAY_BOUNDINGVOLUMEIDX_ALL) {
		startEditingBoundingVolume(prototype);
	}
}

void PrototypePhysicsSubView::handleInputEvents(Prototype* prototype, const Vector3& objectScale) {
	if (displayBoundingVolumeIdx == DISPLAY_BOUNDINGVOLUMEIDX_ALL || displayBoundingVolume == false) {
		removeGizmo();
		return;
	}
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
				auto selectedEntity = engine->getEntity("tdme.prototype.bv.editing");
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
						auto selectedEntity = engine->getEntity("tdme.prototype.bv.editing");
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

void PrototypePhysicsSubView::updateGizmo(Prototype* prototype) {
	auto selectedEntity = engine->getEntity("tdme.prototype.bv.editing");
	if (selectedEntity != nullptr) {
		Gizmo::updateGizmo(selectedEntity->getBoundingBoxTransformed()->getCenter(), selectedEntity->getTransformations());
	} else {
		removeGizmo();
	}
}

void PrototypePhysicsSubView::setGizmoRotation(Prototype* prototype, const Transformations& transformations) {
	if (displayBoundingVolumeIdx == DISPLAY_BOUNDINGVOLUMEIDX_ALL) return;
	Gizmo::setGizmoRotation(transformations);
}

void PrototypePhysicsSubView::applyBoundingVolumeTransformations(Prototype* prototype, int i, const Transformations& _transformations, const Vector3& objectScale, bool guiOnly) {
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
		prototypePhysicsSubController->setBoundingVolumeSphereDetails(center, radius);
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
		prototypePhysicsSubController->setBoundingVolumeCapsuleDetails(a, b, radius);
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
		if ((boundingVolumeTypeMask & PrototypePhysicsSubController::BOUNDINGVOLUMETYPE_ORIENTEDBOUNDINGBOX) == PrototypePhysicsSubController::BOUNDINGVOLUMETYPE_ORIENTEDBOUNDINGBOX) {
			prototypePhysicsSubController->setBoundingVolumeOBBDetails(center, axis0, axis1, axis2, halfExtension);
			if (guiOnly == false) applyBoundingVolumeObb(prototype, i, center, axis0, axis1, axis2, halfExtension);
		} else
		if ((boundingVolumeTypeMask & PrototypePhysicsSubController::BOUNDINGVOLUMETYPE_BOUNDINGBOX) == PrototypePhysicsSubController::BOUNDINGVOLUMETYPE_BOUNDINGBOX) {
			OrientedBoundingBox obb(center, axis0, axis1, axis2, halfExtension);
			BoundingBox aabb(&obb);
			// prototypePhysicsSubController->setBoundingVolumeAABBDetails(i, aabb.getMin(), aabb.getMax());
			if (guiOnly == false) applyBoundingVolumeAabb(prototype, i, aabb.getMin(), aabb.getMax());
		}
	}
}

void PrototypePhysicsSubView::startEditingBoundingVolume(Prototype* prototype) {
	engine->removeEntity("tdme.prototype.bv.editing");
	setupModelBoundingVolume(prototype, displayBoundingVolumeIdx);
	prototypePhysicsSubController->enableTools();
}

void PrototypePhysicsSubView::endEditingBoundingVolume(Prototype* prototype) {
	engine->removeEntity("tdme.prototype.bv.editing");
	prototypePhysicsSubController->disableTools();
}

bool PrototypePhysicsSubView::isEditingBoundingVolume(Prototype* prototype) {
	return displayBoundingVolumeIdx != DISPLAY_BOUNDINGVOLUMEIDX_ALL;
}