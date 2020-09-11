#include <tdme/tools/shared/views/Gizmo.h>

#include <tdme/engine/Camera.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Entity.h>
#include <tdme/engine/Object3D.h>
#include <tdme/engine/model/Group.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/primitives/LineSegment.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector3.h>
#include <tdme/tools/shared/tools/Tools.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/StringTools.h>

using tdme::tools::shared::views::Gizmo;

using tdme::engine::Camera;
using tdme::engine::Engine;
using tdme::engine::Entity;
using tdme::engine::Object3D;
using tdme::engine::model::Group;
using tdme::engine::model::Model;
using tdme::engine::primitives::LineSegment;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;
using tdme::tools::shared::tools::Tools;
using tdme::utilities::Console;
using tdme::utilities::StringTools;

Gizmo::Gizmo(Engine* engine, const string& id, float gizmoBaseScale)
{
	this->engine = engine;
	this->id = id;
	this->gizmoBaseScale = gizmoBaseScale;
	setGizmoType(GIZMOTYPE_ALL);
	setGizmoMode(GIZMOMODE_NONE);
	gizmoLastResultAvailable = false;
}

Gizmo::~Gizmo() {
}

void Gizmo::updateGizmo(const Vector3& gizmoCenter) {
	Object3D* gizmoEntity = nullptr;
	auto scale = (engine->getCamera()->getLookFrom().clone().sub(gizmoCenter).computeLength() * gizmoBaseScale) / 5.0f;
	switch (getGizmoType()) {
		case GIZMOTYPE_ALL:
			{
				engine->removeEntity(id + ".tdme.leveleditor.gizmo.translation");
				engine->removeEntity(id + ".tdme.leveleditor.gizmo.scale");
				gizmoEntity = dynamic_cast<Object3D*>(engine->getEntity(id + ".tdme.leveleditor.gizmo.all"));
				if (gizmoEntity == nullptr) engine->addEntity(gizmoEntity = new Object3D(id + ".tdme.leveleditor.gizmo.all", Tools::getGizmoAll()));
				gizmoEntity->setPickable(true);
				gizmoEntity->setDisableDepthTest(true);
				gizmoEntity->setTranslation(gizmoCenter);
				gizmoEntity->setScale(Vector3(scale, scale, scale));
				gizmoEntity->update();
				gizmoEntity = dynamic_cast<Object3D*>(engine->getEntity(id + ".tdme.leveleditor.gizmo.rotations"));
				if (gizmoEntity == nullptr) engine->addEntity(gizmoEntity = new Object3D(id + ".tdme.leveleditor.gizmo.rotations", Tools::getGizmoRotations()));
				gizmoEntity->setPickable(true);
				gizmoEntity->setDisableDepthTest(true);
				gizmoEntity->setTranslation(gizmoCenter);
				gizmoEntity->setScale(Vector3(scale, scale, scale));
				gizmoEntity->update();
				break;
			}
		case GIZMOTYPE_TRANSLATE:
			{
				engine->removeEntity(id + ".tdme.leveleditor.gizmo.all");
				engine->removeEntity(id + ".tdme.leveleditor.gizmo.scale");
				engine->removeEntity(id + ".tdme.leveleditor.gizmo.rotations");
				gizmoEntity = dynamic_cast<Object3D*>(engine->getEntity(id + ".tdme.leveleditor.gizmo.translation"));
				if (gizmoEntity == nullptr) engine->addEntity(gizmoEntity = new Object3D(id + ".tdme.leveleditor.gizmo.translation", Tools::getGizmoTranslation()));
				gizmoEntity->setPickable(true);
				gizmoEntity->setDisableDepthTest(true);
				gizmoEntity->setTranslation(gizmoCenter);
				gizmoEntity->setScale(Vector3(scale, scale, scale));
				gizmoEntity->update();
				break;
			}
		case GIZMOTYPE_ROTATE:
			{
				engine->removeEntity(id + ".tdme.leveleditor.gizmo.all");
				engine->removeEntity(id + ".tdme.leveleditor.gizmo.translation");
				engine->removeEntity(id + ".tdme.leveleditor.gizmo.scale");
				gizmoEntity = dynamic_cast<Object3D*>(engine->getEntity(id + ".tdme.leveleditor.gizmo.rotations"));
				if (gizmoEntity == nullptr) engine->addEntity(gizmoEntity = new Object3D(id + ".tdme.leveleditor.gizmo.rotations", Tools::getGizmoRotations()));
				gizmoEntity->setPickable(true);
				gizmoEntity->setDisableDepthTest(true);
				gizmoEntity->setTranslation(gizmoCenter);
				gizmoEntity->setScale(Vector3(scale, scale, scale));
				gizmoEntity->update();
				break;
			}
		case GIZMOTYPE_SCALE:
			{
				engine->removeEntity(id + ".tdme.leveleditor.gizmo.all");
				engine->removeEntity(id + ".tdme.leveleditor.gizmo.translation");
				engine->removeEntity(id + ".tdme.leveleditor.gizmo.rotations");
				gizmoEntity = dynamic_cast<Object3D*>(engine->getEntity(id + ".tdme.leveleditor.gizmo.scale"));
				if (gizmoEntity == nullptr) engine->addEntity(gizmoEntity = new Object3D(id + ".tdme.leveleditor.gizmo.scale", Tools::getGizmoScale()));
				gizmoEntity->setPickable(true);
				gizmoEntity->setDisableDepthTest(true);
				gizmoEntity->setTranslation(gizmoCenter);
				gizmoEntity->setScale(Vector3(scale, scale, scale));
				gizmoEntity->update();
				break;
			}
	}
}

Object3D* Gizmo::getGizmoObject3D() {
	auto gizmoEntity = dynamic_cast<Object3D*>(engine->getEntity(id + ".tdme.leveleditor.gizmo.all"));
	if (gizmoEntity == nullptr) gizmoEntity = dynamic_cast<Object3D*>(engine->getEntity(id + ".tdme.leveleditor.gizmo.translation"));
	if (gizmoEntity == nullptr) gizmoEntity = dynamic_cast<Object3D*>(engine->getEntity(id + ".tdme.leveleditor.gizmo.rotations"));
	if (gizmoEntity == nullptr) gizmoEntity = dynamic_cast<Object3D*>(engine->getEntity(id + ".tdme.leveleditor.gizmo.scale"));
	return gizmoEntity;
}

void Gizmo::removeGizmo() {
	engine->removeEntity(id + ".tdme.leveleditor.gizmo.all");
	engine->removeEntity(id + ".tdme.leveleditor.gizmo.translation");
	engine->removeEntity(id + ".tdme.leveleditor.gizmo.scale");
	engine->removeEntity(id + ".tdme.leveleditor.gizmo.rotations");
	setGizmoMode(GIZMOMODE_NONE);
}

bool Gizmo::determineGizmoMovement(int mouseX, int mouseY, vector<Vector3> vertices, Vector3& deltaMovement) {
	auto gizmoEntity = getGizmoObject3D();
	if (gizmoEntity == nullptr) return false;
	Vector3 tmpVector3a;
	Vector3 tmpVector3b;
	Vector3 tmpVector3e;
	engine->computeWorldCoordinateByMousePosition(mouseX, mouseY, 0.0f, tmpVector3a);
	engine->computeWorldCoordinateByMousePosition(mouseX, mouseY, 1.0f, tmpVector3b);
	for (auto& vertex: vertices) {
		vertex.add(gizmoEntity->getTranslation());
	}
	if (LineSegment::doesLineSegmentCollideWithTriangle(
		vertices[0],
		vertices[1],
		vertices[2],
		tmpVector3a,
		tmpVector3b,
		tmpVector3e) == true ||
		LineSegment::doesLineSegmentCollideWithTriangle(
		vertices[2],
		vertices[3],
		vertices[0],
		tmpVector3a,
		tmpVector3b,
		tmpVector3e) == true
	) {
		auto success = gizmoLastResultAvailable == true;
		if (success == true) {
			deltaMovement = tmpVector3e.clone().sub(gizmoLastResult);
		}
		gizmoLastResult = tmpVector3e;
		gizmoLastResultAvailable = true;
		return success;
	}
	return false;
}

bool Gizmo::determineGizmoDeltaTransformations(int mouseLastX, int mouseLastY, int mouseX, int mouseY, Vector3& deltaTranslation, Vector3& deltaRotation, Vector3& deltaScale) {
	if (getGizmoMode() == GIZMOMODE_NONE) return false;
	auto deltaX = mouseX - mouseLastX;
	auto deltaY = mouseY - mouseLastY;
	deltaTranslation.set(0.0f, 0.0f, 0.0f);
	deltaRotation.set(0.0f, 0.0f, 0.0f);
	deltaScale.set(1.0f, 1.0f, 1.0f);
	Vector3 gizmoDeltaMovement;
	const vector<Vector3> planeXY =
		{
			Vector3(-5000.0f, -5000.0f, 0.0f),
			Vector3(-5000.0f, 5000.0f, 0.0f),
			Vector3(5000.0f, 5000.0f, 0.0f),
			Vector3(5000.0f, -5000.0f, 0.0f)
		};
	const vector<Vector3> planeXZ =
		{
			Vector3(-5000.0f, 0.0f, -5000.0f),
			Vector3(-5000.0f, 0.0f, 5000.0f),
			Vector3(5000.0f, 0.0f, 5000.0f),
			Vector3(5000.0f, 0.0f, -5000.0f)
		};
	const vector<Vector3> planeYZ =
		{
			Vector3(0.0f, -5000.0f, -5000.0f),
			Vector3(0.0f, -5000.0f, 5000.0f),
			Vector3(0.0f, 5000.0f, 5000.0f),
			Vector3(0.0f, 5000.0f, -5000.0f)

		};
	switch (getGizmoMode()) {
		case GIZMOMODE_TRANSLATE_X:
			{
				vector<Vector3> vertices = planeXZ;
				if (determineGizmoMovement(mouseX, mouseY, vertices, gizmoDeltaMovement) == true) {
					deltaTranslation.setX(gizmoDeltaMovement.getX());
				}
				break;
			}
		case GIZMOMODE_TRANSLATE_Y:
			{
				vector<Vector3> vertices = planeYZ;
				if (determineGizmoMovement(mouseX, mouseY, vertices, gizmoDeltaMovement) == true) {
					deltaTranslation.setY(gizmoDeltaMovement.getY());
				}
				break;
			}
		case GIZMOMODE_TRANSLATE_Z:
			{
				vector<Vector3> vertices = planeXZ;
				if (determineGizmoMovement(mouseX, mouseY, vertices, gizmoDeltaMovement) == true) {
					deltaTranslation.setZ(gizmoDeltaMovement.getZ());
				}
				break;
			}
		case GIZMOMODE_TRANSLATEPLANE_X:
			{
				vector<Vector3> vertices = planeYZ;
				if (determineGizmoMovement(mouseX, mouseY, vertices, gizmoDeltaMovement) == true) {
					deltaTranslation.setY(gizmoDeltaMovement.getY());
					deltaTranslation.setZ(gizmoDeltaMovement.getZ());
				}
				break;
			}
		case GIZMOMODE_TRANSLATEPLANE_Y:
			{
				vector<Vector3> vertices = planeXZ;
				if (determineGizmoMovement(mouseX, mouseY, vertices, gizmoDeltaMovement) == true) {
					deltaTranslation.setX(gizmoDeltaMovement.getX());
					deltaTranslation.setZ(gizmoDeltaMovement.getZ());
				}
				break;
			}
		case GIZMOMODE_TRANSLATEPLANE_Z:
			{
				vector<Vector3> vertices = planeXY;
				if (determineGizmoMovement(mouseX, mouseY, vertices, gizmoDeltaMovement) == true) {
					deltaTranslation.setX(gizmoDeltaMovement.getX());
					deltaTranslation.setY(gizmoDeltaMovement.getY());
				}
				break;
			}
		case GIZMOMODE_SCALE_X:
			{
				vector<Vector3> vertices = planeXZ;
				if (determineGizmoMovement(mouseX, mouseY, vertices, gizmoDeltaMovement) == true) {
					deltaScale.add(Vector3(gizmoDeltaMovement.getX(), 0.0f, 0.0f));
				}
				break;
			}
		case GIZMOMODE_SCALE_Y:
			{
				vector<Vector3> vertices = planeYZ;
				if (determineGizmoMovement(mouseX, mouseY, vertices, gizmoDeltaMovement) == true) {
					deltaScale.add(Vector3(0.0f, gizmoDeltaMovement.getY(), 0.0f));
				}
				break;
			}
		case GIZMOMODE_SCALE_Z:
			{
				vector<Vector3> vertices = planeXZ;
				if (determineGizmoMovement(mouseX, mouseY, vertices, gizmoDeltaMovement) == true) {
					deltaScale.add(Vector3(0.0f, 0.0f, -gizmoDeltaMovement.getZ()));
				}
				break;
			}
		case GIZMOMODE_SCALEPLANE_X:
			{
				vector<Vector3> vertices = planeYZ;
				if (determineGizmoMovement(mouseX, mouseY, vertices, gizmoDeltaMovement) == true) {
					deltaScale.add(Vector3(0.0f, gizmoDeltaMovement.getY(), gizmoDeltaMovement.getZ()));
				}
				break;
			}
		case GIZMOMODE_SCALEPLANE_Y:
			{
				vector<Vector3> vertices = planeXZ;
				if (determineGizmoMovement(mouseX, mouseY, vertices, gizmoDeltaMovement) == true) {
					deltaScale.add(Vector3(gizmoDeltaMovement.getX(), 0.0f, gizmoDeltaMovement.getZ()));
				}
				break;
			}
		case GIZMOMODE_SCALEPLANE_Z:
			{
				vector<Vector3> vertices = planeXY;
				if (determineGizmoMovement(mouseX, mouseY, vertices, gizmoDeltaMovement) == true) {
					deltaScale.add(Vector3(gizmoDeltaMovement.getX(), gizmoDeltaMovement.getY(), 0.0f));
				}
				break;
			}
		case GIZMOMODE_ROTATE_X:
			deltaRotation.setX((deltaX + -deltaY) * 20.0f * engine->getTiming()->getDeltaTime() / 1000.0f);
			break;
		case GIZMOMODE_ROTATE_Y:
			deltaRotation.setY((deltaX + -deltaY) * 20.0f * engine->getTiming()->getDeltaTime() / 1000.0f);
			break;
		case GIZMOMODE_ROTATE_Z:
			deltaRotation.setZ((deltaX + -deltaY) * 20.0f * engine->getTiming()->getDeltaTime() / 1000.0f);
			break;
		case GIZMOMODE_NONE:
			// no op, satisfy the compiler
			break;
	}
	return true;
}

bool Gizmo::determineGizmoMode(Entity* selectedEntity, Group* selectedEntityGroup) {
	if (selectedEntity != nullptr &&
		StringTools::startsWith(selectedEntity->getId(), id + ".tdme.leveleditor.gizmo.") == true && selectedEntityGroup != nullptr) {
		auto selectedEntityGroupId = selectedEntityGroup->getId();
		if (StringTools::startsWith(selectedEntityGroupId, "all_") == true) selectedEntityGroupId = StringTools::substring(selectedEntityGroupId, 4);
		if (selectedEntityGroupId == "translate_x") setGizmoMode(GIZMOMODE_TRANSLATE_X); else
		if (selectedEntityGroupId == "translate_y") setGizmoMode(GIZMOMODE_TRANSLATE_Z); else
		if (selectedEntityGroupId == "translate_z") setGizmoMode(GIZMOMODE_TRANSLATE_Y); else
		if (selectedEntityGroupId == "translate_x_plane") setGizmoMode(GIZMOMODE_TRANSLATEPLANE_X); else
		if (selectedEntityGroupId == "translate_y_plane") setGizmoMode(GIZMOMODE_TRANSLATEPLANE_Z); else
		if (selectedEntityGroupId == "translate_z_plane") setGizmoMode(GIZMOMODE_TRANSLATEPLANE_Y); else
		if (selectedEntityGroupId == "rotate_x") setGizmoMode(GIZMOMODE_ROTATE_X); else
		if (selectedEntityGroupId == "rotate_y") setGizmoMode(GIZMOMODE_ROTATE_Z); else
		if (selectedEntityGroupId == "rotate_z") setGizmoMode(GIZMOMODE_ROTATE_Y); else
		if (selectedEntityGroupId == "scale_x") setGizmoMode(GIZMOMODE_SCALE_X); else
		if (selectedEntityGroupId == "scale_y") setGizmoMode(GIZMOMODE_SCALE_Z); else
		if (selectedEntityGroupId == "scale_z") setGizmoMode(GIZMOMODE_SCALE_Y); else
		if (selectedEntityGroupId == "scale_x_plane") setGizmoMode(GIZMOMODE_SCALEPLANE_X); else
		if (selectedEntityGroupId == "scale_y_plane") setGizmoMode(GIZMOMODE_SCALEPLANE_Z); else
		if (selectedEntityGroupId == "scale_z_plane") setGizmoMode(GIZMOMODE_SCALEPLANE_Y); else
			setGizmoMode(GIZMOMODE_NONE);
		return true;
	}
	return false;
}

void Gizmo::setGizmoRotation(const Transformations& transformations) {
	Matrix4x4 rotationsMatrix;
	transformations.getRotationsQuaternion().computeMatrix(rotationsMatrix);
	{
		auto gizmoEntity = dynamic_cast<Object3D*>(engine->getEntity(id + ".tdme.leveleditor.gizmo.scale"));
		if (gizmoEntity != nullptr) {
			gizmoEntity->setGroupTransformationsMatrix("scale_x", gizmoEntity->getModel()->getGroupById("scale_x")->getTransformationsMatrix().clone().multiply(rotationsMatrix));
			gizmoEntity->setGroupTransformationsMatrix("scale_y", gizmoEntity->getModel()->getGroupById("scale_y")->getTransformationsMatrix().clone().multiply(rotationsMatrix));
			gizmoEntity->setGroupTransformationsMatrix("scale_z", gizmoEntity->getModel()->getGroupById("scale_z")->getTransformationsMatrix().clone().multiply(rotationsMatrix));
			gizmoEntity->setGroupTransformationsMatrix("scale_x_plane", gizmoEntity->getModel()->getGroupById("scale_x_plane")->getTransformationsMatrix().clone().multiply(rotationsMatrix));
			gizmoEntity->setGroupTransformationsMatrix("scale_y_plane", gizmoEntity->getModel()->getGroupById("scale_y_plane")->getTransformationsMatrix().clone().multiply(rotationsMatrix));
			gizmoEntity->setGroupTransformationsMatrix("scale_z_plane", gizmoEntity->getModel()->getGroupById("scale_x_plane")->getTransformationsMatrix().clone().multiply(rotationsMatrix));
			gizmoEntity->update();
		}
	}
	{
		auto gizmoEntity = dynamic_cast<Object3D*>(engine->getEntity(id + ".tdme.leveleditor.gizmo.all"));
		if (gizmoEntity != nullptr) {
			gizmoEntity->setGroupTransformationsMatrix("all_scale_x", gizmoEntity->getModel()->getGroupById("all_scale_x")->getTransformationsMatrix().clone().multiply(rotationsMatrix));
			gizmoEntity->setGroupTransformationsMatrix("all_scale_y", gizmoEntity->getModel()->getGroupById("all_scale_y")->getTransformationsMatrix().clone().multiply(rotationsMatrix));
			gizmoEntity->setGroupTransformationsMatrix("all_scale_z", gizmoEntity->getModel()->getGroupById("all_scale_z")->getTransformationsMatrix().clone().multiply(rotationsMatrix));
			gizmoEntity->update();
		}
	}
}
