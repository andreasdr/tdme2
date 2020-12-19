#include <tdme/tools/shared/views/Gizmo.h>

#include <tdme/engine/model/Model.h>
#include <tdme/engine/model/Node.h>
#include <tdme/engine/primitives/LineSegment.h>
#include <tdme/engine/Camera.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Entity.h>
#include <tdme/engine/Frustum.h>
#include <tdme/engine/Object3D.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Quaternion.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Vector4.h>
#include <tdme/tools/shared/tools/Tools.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/StringTools.h>

using tdme::tools::shared::views::Gizmo;

using tdme::engine::model::Model;
using tdme::engine::model::Node;
using tdme::engine::primitives::LineSegment;
using tdme::engine::Camera;
using tdme::engine::Engine;
using tdme::engine::Entity;
using tdme::engine::Frustum;
using tdme::engine::Object3D;
using tdme::math::Matrix4x4;
using tdme::math::Quaternion;
using tdme::math::Vector3;
using tdme::math::Vector4;
using tdme::tools::shared::tools::Tools;
using tdme::utilities::Console;
using tdme::utilities::StringTools;

Gizmo::Gizmo(Engine* engine, const string& id, int32_t gizmoTypeMask)
{
	this->engine = engine;
	this->id = id;
	this->gizmoTypeMask = gizmoTypeMask;
	setGizmoType(GIZMOTYPE_ALL);
	setGizmoMode(GIZMOMODE_NONE);
	gizmoLastResultAvailable = false;
}

Gizmo::~Gizmo() {
}

void Gizmo::updateGizmo(const Vector3& gizmoCenter, const Transformations& transformations) {
	Object3D* gizmoEntity = nullptr;
	auto zNearNormal = engine->getCamera()->getFrustum()->getPlanes()[Frustum::PLANE_NEAR].getNormal();
	auto lookFrom = engine->getCamera()->getLookFrom();
	auto zNearDistance = engine->getCamera()->getFrustum()->getPlanes()[Frustum::PLANE_NEAR].computeDistance(gizmoCenter);
	Vector3 a,b,c,d;
	Vector4 e;
	Vector3 f;
	Quaternion q1, q2;
	q1.rotate(zNearNormal, -45.0f);
	q2.rotate(zNearNormal, 135.0f);
	engine->getCamera()->getModelViewProjectionMatrix().multiply(Vector4(lookFrom + zNearNormal * 1.0f + q1.multiply(Vector3(0.0f, -0.5f, 0.0f), f), 1.0f), e);
	e.scale(1.0f / e.getW());
	a.set(e.getX(), e.getY(), e.getZ());
	engine->getCamera()->getModelViewProjectionMatrix().multiply(Vector4(lookFrom + zNearNormal * 1.0f + q2.multiply(Vector3(0.0f, -0.5f, 0.0f), f), 1.0f), e);
	e.scale(1.0f / e.getW());
	b.set(e.getX(), e.getY(), e.getZ());
	engine->getCamera()->getModelViewProjectionMatrix().multiply(Vector4(lookFrom + zNearNormal * zNearDistance + q1.multiply(Vector3(0.0f, -0.5f, 0.0f), f), 1.0f), e);
	e.scale(1.0f / e.getW());
	c.set(e.getX(), e.getY(), e.getZ());
	engine->getCamera()->getModelViewProjectionMatrix().multiply(Vector4(lookFrom + zNearNormal * zNearDistance + q2.multiply(Vector3(0.0f, -0.5f, 0.0f), f), 1.0f), e);
	e.scale(1.0f / e.getW());
	d.set(e.getX(), e.getY(), e.getZ());
	auto baX = b.getX() - a.getX();
	auto baY = b.getY() - a.getY();
	auto dcX = d.getX() - c.getX();
	auto dcY = d.getY() - c.getY();
	auto baXDivdcX = baX / dcX;
	auto baYDivdcY = baY / dcY;
	auto scale = baXDivdcX / 3.0f ;
	switch (getGizmoType()) {
		case GIZMOTYPE_ALL:
			{
				if ((gizmoTypeMask & GIZMOTYPE_TRANSLATE) == GIZMOTYPE_TRANSLATE &&
					(gizmoTypeMask & GIZMOTYPE_ROTATE) == GIZMOTYPE_ROTATE &&
					(gizmoTypeMask & GIZMOTYPE_SCALE) == GIZMOTYPE_SCALE) {
					engine->removeEntity(id + ".tdme.gizmo.translation");
					engine->removeEntity(id + ".tdme.gizmo.scale");
					gizmoEntity = dynamic_cast<Object3D*>(engine->getEntity(id + ".tdme.gizmo.all"));
					if (gizmoEntity == nullptr) engine->addEntity(gizmoEntity = new Object3D(id + ".tdme.gizmo.all", Tools::getGizmoAll()));
					gizmoEntity->setPickable(true);
					gizmoEntity->setDisableDepthTest(true);
					gizmoEntity->setTranslation(gizmoCenter);
					gizmoEntity->setScale(Vector3(scale, scale, scale));
					gizmoEntity->update();
					gizmoEntity = dynamic_cast<Object3D*>(engine->getEntity(id + ".tdme.gizmo.rotations"));
					if (gizmoEntity == nullptr) engine->addEntity(gizmoEntity = new Object3D(id + ".tdme.gizmo.rotations", Tools::getGizmoRotations()));
					gizmoEntity->setPickable(true);
					gizmoEntity->setDisableDepthTest(true);
					gizmoEntity->setTranslation(gizmoCenter);
					gizmoEntity->setScale(Vector3(scale, scale, scale));
					gizmoEntity->update();
				} else
				if ((gizmoTypeMask & GIZMOTYPE_TRANSLATE) == GIZMOTYPE_TRANSLATE &&
					(gizmoTypeMask & GIZMOTYPE_SCALE) == GIZMOTYPE_SCALE) {
					engine->removeEntity(id + ".tdme.gizmo.translation");
					engine->removeEntity(id + ".tdme.gizmo.rotations");
					engine->removeEntity(id + ".tdme.gizmo.scale");
					gizmoEntity = dynamic_cast<Object3D*>(engine->getEntity(id + ".tdme.gizmo.all"));
					if (gizmoEntity == nullptr) engine->addEntity(gizmoEntity = new Object3D(id + ".tdme.gizmo.all", Tools::getGizmoTranslationScale()));
					gizmoEntity->setPickable(true);
					gizmoEntity->setDisableDepthTest(true);
					gizmoEntity->setTranslation(gizmoCenter);
					gizmoEntity->setScale(Vector3(scale, scale, scale));
					gizmoEntity->update();
				} else {
					if ((gizmoTypeMask & GIZMOTYPE_TRANSLATE) == GIZMOTYPE_TRANSLATE) {
						engine->removeEntity(id + ".tdme.gizmo.all");
						engine->removeEntity(id + ".tdme.gizmo.rotations");
						engine->removeEntity(id + ".tdme.gizmo.scale");
						gizmoEntity = dynamic_cast<Object3D*>(engine->getEntity(id + ".tdme.gizmo.translation"));
						if (gizmoEntity == nullptr) engine->addEntity(gizmoEntity = new Object3D(id + ".tdme.gizmo.translation", Tools::getGizmoTranslation()));
						gizmoEntity->setPickable(true);
						gizmoEntity->setDisableDepthTest(true);
						gizmoEntity->setTranslation(gizmoCenter);
						gizmoEntity->setScale(Vector3(scale, scale, scale));
						gizmoEntity->update();
					}
					if ((gizmoTypeMask & GIZMOTYPE_ROTATE) == GIZMOTYPE_ROTATE) {
						engine->removeEntity(id + ".tdme.gizmo.all");
						engine->removeEntity(id + ".tdme.gizmo.translation");
						engine->removeEntity(id + ".tdme.gizmo.scale");
						gizmoEntity = dynamic_cast<Object3D*>(engine->getEntity(id + ".tdme.gizmo.rotations"));
						if (gizmoEntity == nullptr) engine->addEntity(gizmoEntity = new Object3D(id + ".tdme.gizmo.rotations", Tools::getGizmoRotations()));
						gizmoEntity->setPickable(true);
						gizmoEntity->setDisableDepthTest(true);
						gizmoEntity->setTranslation(gizmoCenter);
						gizmoEntity->setScale(Vector3(scale, scale, scale));
						gizmoEntity->update();
					}
					if ((gizmoTypeMask & GIZMOTYPE_SCALE) == GIZMOTYPE_SCALE) {
						engine->removeEntity(id + ".tdme.gizmo.all");
						engine->removeEntity(id + ".tdme.gizmo.translation");
						engine->removeEntity(id + ".tdme.gizmo.rotations");
						gizmoEntity = dynamic_cast<Object3D*>(engine->getEntity(id + ".tdme.gizmo.scale"));
						if (gizmoEntity == nullptr) engine->addEntity(gizmoEntity = new Object3D(id + ".tdme.gizmo.scale", Tools::getGizmoScale()));
						gizmoEntity->setPickable(true);
						gizmoEntity->setDisableDepthTest(true);
						gizmoEntity->setTranslation(gizmoCenter);
						gizmoEntity->setScale(Vector3(scale, scale, scale));
						gizmoEntity->update();
					}
				}
				break;
			}
		case GIZMOTYPE_TRANSLATE:
			{
				engine->removeEntity(id + ".tdme.gizmo.all");
				engine->removeEntity(id + ".tdme.gizmo.scale");
				engine->removeEntity(id + ".tdme.gizmo.rotations");
				gizmoEntity = dynamic_cast<Object3D*>(engine->getEntity(id + ".tdme.gizmo.translation"));
				if (gizmoEntity == nullptr) engine->addEntity(gizmoEntity = new Object3D(id + ".tdme.gizmo.translation", Tools::getGizmoTranslation()));
				gizmoEntity->setPickable(true);
				gizmoEntity->setDisableDepthTest(true);
				gizmoEntity->setTranslation(gizmoCenter);
				gizmoEntity->setScale(Vector3(scale, scale, scale));
				gizmoEntity->update();
				break;
			}
		case GIZMOTYPE_ROTATE:
			{
				engine->removeEntity(id + ".tdme.gizmo.all");
				engine->removeEntity(id + ".tdme.gizmo.translation");
				engine->removeEntity(id + ".tdme.gizmo.scale");
				gizmoEntity = dynamic_cast<Object3D*>(engine->getEntity(id + ".tdme.gizmo.rotations"));
				if (gizmoEntity == nullptr) engine->addEntity(gizmoEntity = new Object3D(id + ".tdme.gizmo.rotations", Tools::getGizmoRotations()));
				gizmoEntity->setPickable(true);
				gizmoEntity->setDisableDepthTest(true);
				gizmoEntity->setTranslation(gizmoCenter);
				gizmoEntity->setScale(Vector3(scale, scale, scale));
				gizmoEntity->update();
				break;
			}
		case GIZMOTYPE_SCALE:
			{
				engine->removeEntity(id + ".tdme.gizmo.all");
				engine->removeEntity(id + ".tdme.gizmo.translation");
				engine->removeEntity(id + ".tdme.gizmo.rotations");
				gizmoEntity = dynamic_cast<Object3D*>(engine->getEntity(id + ".tdme.gizmo.scale"));
				if (gizmoEntity == nullptr) engine->addEntity(gizmoEntity = new Object3D(id + ".tdme.gizmo.scale", Tools::getGizmoScale()));
				gizmoEntity->setPickable(true);
				gizmoEntity->setDisableDepthTest(true);
				gizmoEntity->setTranslation(gizmoCenter);
				gizmoEntity->setScale(Vector3(scale, scale, scale));
				gizmoEntity->update();
				break;
			}
	}
	setGizmoRotation(transformations);
}

Object3D* Gizmo::getGizmoObject3D() {
	auto gizmoEntity = dynamic_cast<Object3D*>(engine->getEntity(id + ".tdme.gizmo.all"));
	if (gizmoEntity == nullptr) gizmoEntity = dynamic_cast<Object3D*>(engine->getEntity(id + ".tdme.gizmo.translation"));
	if (gizmoEntity == nullptr) gizmoEntity = dynamic_cast<Object3D*>(engine->getEntity(id + ".tdme.gizmo.rotations"));
	if (gizmoEntity == nullptr) gizmoEntity = dynamic_cast<Object3D*>(engine->getEntity(id + ".tdme.gizmo.scale"));
	return gizmoEntity;
}

void Gizmo::removeGizmo() {
	engine->removeEntity(id + ".tdme.gizmo.all");
	engine->removeEntity(id + ".tdme.gizmo.translation");
	engine->removeEntity(id + ".tdme.gizmo.scale");
	engine->removeEntity(id + ".tdme.gizmo.rotations");
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

bool Gizmo::determineGizmoMode(Entity* selectedEntity, Node* selectedEntityNode) {
	if (selectedEntity != nullptr &&
		StringTools::startsWith(selectedEntity->getId(), id + ".tdme.gizmo.") == true && selectedEntityNode != nullptr) {
		auto selectedEntityNodeId = selectedEntityNode->getId();
		if (StringTools::startsWith(selectedEntityNodeId, "all_") == true) selectedEntityNodeId = StringTools::substring(selectedEntityNodeId, 4);
		if (selectedEntityNodeId == "translate_x") setGizmoMode(GIZMOMODE_TRANSLATE_X); else
		if (selectedEntityNodeId == "translate_y") setGizmoMode(GIZMOMODE_TRANSLATE_Z); else
		if (selectedEntityNodeId == "translate_z") setGizmoMode(GIZMOMODE_TRANSLATE_Y); else
		if (selectedEntityNodeId == "translate_x_plane") setGizmoMode(GIZMOMODE_TRANSLATEPLANE_X); else
		if (selectedEntityNodeId == "translate_y_plane") setGizmoMode(GIZMOMODE_TRANSLATEPLANE_Z); else
		if (selectedEntityNodeId == "translate_z_plane") setGizmoMode(GIZMOMODE_TRANSLATEPLANE_Y); else
		if (selectedEntityNodeId == "rotate_x") setGizmoMode(GIZMOMODE_ROTATE_X); else
		if (selectedEntityNodeId == "rotate_y") setGizmoMode(GIZMOMODE_ROTATE_Z); else
		if (selectedEntityNodeId == "rotate_z") setGizmoMode(GIZMOMODE_ROTATE_Y); else
		if (selectedEntityNodeId == "scale_x") setGizmoMode(GIZMOMODE_SCALE_X); else
		if (selectedEntityNodeId == "scale_y") setGizmoMode(GIZMOMODE_SCALE_Z); else
		if (selectedEntityNodeId == "scale_z") setGizmoMode(GIZMOMODE_SCALE_Y); else
		if (selectedEntityNodeId == "scale_x_plane") setGizmoMode(GIZMOMODE_SCALEPLANE_X); else
		if (selectedEntityNodeId == "scale_y_plane") setGizmoMode(GIZMOMODE_SCALEPLANE_Z); else
		if (selectedEntityNodeId == "scale_z_plane") setGizmoMode(GIZMOMODE_SCALEPLANE_Y); else
			setGizmoMode(GIZMOMODE_NONE);
		return true;
	}
	return false;
}

void Gizmo::setGizmoRotation(const Transformations& transformations) {
	Matrix4x4 rotationsMatrix;
	transformations.getRotationsQuaternion().computeMatrix(rotationsMatrix);
	{
		auto gizmoEntity = dynamic_cast<Object3D*>(engine->getEntity(id + ".tdme.gizmo.scale"));
		if (gizmoEntity != nullptr) {
			gizmoEntity->setNodeTransformationsMatrix("scale_x", gizmoEntity->getModel()->getNodeById("scale_x")->getTransformationsMatrix().clone().multiply(rotationsMatrix));
			gizmoEntity->setNodeTransformationsMatrix("scale_y", gizmoEntity->getModel()->getNodeById("scale_y")->getTransformationsMatrix().clone().multiply(rotationsMatrix));
			gizmoEntity->setNodeTransformationsMatrix("scale_z", gizmoEntity->getModel()->getNodeById("scale_z")->getTransformationsMatrix().clone().multiply(rotationsMatrix));
			gizmoEntity->setNodeTransformationsMatrix("scale_x_plane", gizmoEntity->getModel()->getNodeById("scale_x_plane")->getTransformationsMatrix().clone().multiply(rotationsMatrix));
			gizmoEntity->setNodeTransformationsMatrix("scale_y_plane", gizmoEntity->getModel()->getNodeById("scale_y_plane")->getTransformationsMatrix().clone().multiply(rotationsMatrix));
			gizmoEntity->setNodeTransformationsMatrix("scale_z_plane", gizmoEntity->getModel()->getNodeById("scale_x_plane")->getTransformationsMatrix().clone().multiply(rotationsMatrix));
			gizmoEntity->update();
		}
	}
	{
		auto gizmoEntity = dynamic_cast<Object3D*>(engine->getEntity(id + ".tdme.gizmo.rotations"));
		if (gizmoEntity != nullptr) {
			gizmoEntity->setNodeTransformationsMatrix("rotate_x", gizmoEntity->getModel()->getNodeById("rotate_x")->getTransformationsMatrix().clone().multiply(rotationsMatrix));
			gizmoEntity->setNodeTransformationsMatrix("rotate_y", gizmoEntity->getModel()->getNodeById("rotate_y")->getTransformationsMatrix().clone().multiply(rotationsMatrix));
			gizmoEntity->setNodeTransformationsMatrix("rotate_z", gizmoEntity->getModel()->getNodeById("rotate_z")->getTransformationsMatrix().clone().multiply(rotationsMatrix));
			gizmoEntity->update();
		}
	}
	{
		auto gizmoEntity = dynamic_cast<Object3D*>(engine->getEntity(id + ".tdme.gizmo.all"));
		if (gizmoEntity != nullptr) {
			if (gizmoEntity->getModel()->getNodeById("all_scale_x") != nullptr) gizmoEntity->setNodeTransformationsMatrix("all_scale_x", gizmoEntity->getModel()->getNodeById("all_scale_x")->getTransformationsMatrix().clone().multiply(rotationsMatrix));
			if (gizmoEntity->getModel()->getNodeById("all_scale_y") != nullptr) gizmoEntity->setNodeTransformationsMatrix("all_scale_y", gizmoEntity->getModel()->getNodeById("all_scale_y")->getTransformationsMatrix().clone().multiply(rotationsMatrix));
			if (gizmoEntity->getModel()->getNodeById("all_scale_z") != nullptr) gizmoEntity->setNodeTransformationsMatrix("all_scale_z", gizmoEntity->getModel()->getNodeById("all_scale_z")->getTransformationsMatrix().clone().multiply(rotationsMatrix));
			if (gizmoEntity->getModel()->getNodeById("scale_x") != nullptr) gizmoEntity->setNodeTransformationsMatrix("scale_x", gizmoEntity->getModel()->getNodeById("scale_x")->getTransformationsMatrix().clone().multiply(rotationsMatrix));
			if (gizmoEntity->getModel()->getNodeById("scale_y") != nullptr) gizmoEntity->setNodeTransformationsMatrix("scale_y", gizmoEntity->getModel()->getNodeById("scale_y")->getTransformationsMatrix().clone().multiply(rotationsMatrix));
			if (gizmoEntity->getModel()->getNodeById("scale_z") != nullptr) gizmoEntity->setNodeTransformationsMatrix("scale_z", gizmoEntity->getModel()->getNodeById("scale_z")->getTransformationsMatrix().clone().multiply(rotationsMatrix));
			if (gizmoEntity->getModel()->getNodeById("scale_x_plane") != nullptr) gizmoEntity->setNodeTransformationsMatrix("scale_x_plane", gizmoEntity->getModel()->getNodeById("scale_x_plane")->getTransformationsMatrix().clone().multiply(rotationsMatrix));
			if (gizmoEntity->getModel()->getNodeById("scale_y_plane") != nullptr) gizmoEntity->setNodeTransformationsMatrix("scale_y_plane", gizmoEntity->getModel()->getNodeById("scale_y_plane")->getTransformationsMatrix().clone().multiply(rotationsMatrix));
			if (gizmoEntity->getModel()->getNodeById("scale_z_plane") != nullptr) gizmoEntity->setNodeTransformationsMatrix("scale_z_plane", gizmoEntity->getModel()->getNodeById("scale_x_plane")->getTransformationsMatrix().clone().multiply(rotationsMatrix));
			if (gizmoEntity->getModel()->getNodeById("rotate_x") != nullptr) gizmoEntity->setNodeTransformationsMatrix("rotate_x", gizmoEntity->getModel()->getNodeById("rotate_x")->getTransformationsMatrix().clone().multiply(rotationsMatrix));
			if (gizmoEntity->getModel()->getNodeById("rotate_y") != nullptr) gizmoEntity->setNodeTransformationsMatrix("rotate_y", gizmoEntity->getModel()->getNodeById("rotate_y")->getTransformationsMatrix().clone().multiply(rotationsMatrix));
			if (gizmoEntity->getModel()->getNodeById("rotate_z") != nullptr) gizmoEntity->setNodeTransformationsMatrix("rotate_z", gizmoEntity->getModel()->getNodeById("rotate_z")->getTransformationsMatrix().clone().multiply(rotationsMatrix));
			gizmoEntity->update();
		}
	}
}
