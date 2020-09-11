#include <tdme/engine/subsystems/rendering/Object3DBase.h>

#include <map>
#include <string>
#include <vector>

#include <tdme/math/Math.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Timing.h>
#include <tdme/engine/model/Animation.h>
#include <tdme/engine/model/AnimationSetup.h>
#include <tdme/engine/model/Face.h>
#include <tdme/engine/model/FacesEntity.h>
#include <tdme/engine/model/Group.h>
#include <tdme/engine/model/Joint.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/model/Skinning.h>
#include <tdme/engine/primitives/BoundingVolume.h>
#include <tdme/engine/primitives/Triangle.h>
#include <tdme/engine/subsystems/manager/MeshManager.h>
#include <tdme/engine/subsystems/rendering/AnimationState.h>
#include <tdme/engine/subsystems/rendering/Object3DAnimation.h>
#include <tdme/engine/subsystems/rendering/Object3DBase_TransformedFacesIterator.h>
#include <tdme/engine/subsystems/rendering/Object3DGroup.h>
#include <tdme/engine/subsystems/rendering/Object3DGroupMesh.h>
#include <tdme/engine/subsystems/rendering/Object3DGroupRenderer.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector3.h>
#include <tdme/utilities/Console.h>

using std::map;
using std::vector;
using std::string;
using std::to_string;

using tdme::engine::subsystems::rendering::Object3DBase;
using tdme::math::Math;
using tdme::engine::Engine;
using tdme::engine::Timing;
using tdme::engine::model::Animation;
using tdme::engine::model::AnimationSetup;
using tdme::engine::model::Face;
using tdme::engine::model::FacesEntity;
using tdme::engine::model::Group;
using tdme::engine::model::Joint;
using tdme::engine::model::Model;
using tdme::engine::model::Skinning;
using tdme::engine::primitives::BoundingVolume;
using tdme::engine::primitives::Triangle;
using tdme::engine::subsystems::manager::MeshManager;
using tdme::engine::subsystems::rendering::AnimationState;
using tdme::engine::subsystems::rendering::Object3DAnimation;
using tdme::engine::subsystems::rendering::Object3DBase_TransformedFacesIterator;
using tdme::engine::subsystems::rendering::Object3DGroup;
using tdme::engine::subsystems::rendering::Object3DGroupMesh;
using tdme::engine::subsystems::rendering::Object3DGroupRenderer;
using tdme::utilities::Console;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;

Object3DBase::Object3DBase(Model* model, bool useManagers, Engine::AnimationProcessingTarget animationProcessingTarget, int instances)
{
	this->model = model;
	this->animationProcessingTarget = animationProcessingTarget;
	this->usesManagers = useManagers;
	this->instances = instances;
	this->enabledInstances = instances;
	this->currentInstance = 0;
	instanceAnimations.resize(instances);
	instanceEnabled.resize(instances);
	instanceTransformations.resize(instances);
	for (auto i = 0; i < instances; i++) {
		instanceEnabled[i] = true;
		instanceAnimations[i] = new Object3DAnimation(model, animationProcessingTarget);
	}
	transformedFacesIterator = nullptr;
	// object 3d groups
	Object3DGroup::createGroups(this, useManagers, animationProcessingTarget, object3dGroups);
	// do initial transformations if doing CPU no rendering for deriving bounding boxes and such
	if (animationProcessingTarget == Engine::AnimationProcessingTarget::CPU_NORENDERING) Object3DGroup::computeTransformations(nullptr, object3dGroups);
}

Object3DBase::~Object3DBase() {
	for (auto i = 0; i < instances; i++) {
		delete instanceAnimations[i];
	}
	for (auto i = 0; i < object3dGroups.size(); i++) {
		delete object3dGroups[i];
	}
	if (transformedFacesIterator != nullptr) delete transformedFacesIterator;
}

int Object3DBase::getGroupCount() const {
	return object3dGroups.size();
}

void Object3DBase::getTriangles(vector<Triangle>& triangles, int groupIdx)
{
	if (groupIdx == -1) {
		for (auto object3DGroup : object3dGroups) {
			auto groupVerticesTransformed = &object3DGroup->mesh->transformedVertices;
			for (auto& facesEntity : object3DGroup->group->getFacesEntities())
			for (auto& face : facesEntity.getFaces()) {
				auto faceVertexIndices = face.getVertexIndices();
				triangles.push_back(
					Triangle(
						(*groupVerticesTransformed)[faceVertexIndices[0]],
						(*groupVerticesTransformed)[faceVertexIndices[1]],
						(*groupVerticesTransformed)[faceVertexIndices[2]]
					)
				);
			}
		}
	} else {
		auto object3DGroup = object3dGroups[groupIdx];
		auto groupVerticesTransformed = &object3DGroup->mesh->transformedVertices;
		for (auto& facesEntity : object3DGroup->group->getFacesEntities())
		for (auto& face : facesEntity.getFaces()) {
			auto faceVertexIndices = face.getVertexIndices();
			triangles.push_back(
				Triangle(
					(*groupVerticesTransformed)[faceVertexIndices[0]],
					(*groupVerticesTransformed)[faceVertexIndices[1]],
					(*groupVerticesTransformed)[faceVertexIndices[2]]
				)
			);
		}
	}
}

Object3DBase_TransformedFacesIterator* Object3DBase::getTransformedFacesIterator()
{
	if (transformedFacesIterator == nullptr) {
		transformedFacesIterator = new Object3DBase_TransformedFacesIterator(this);
	}
	return transformedFacesIterator;
}

Object3DGroupMesh* Object3DBase::getMesh(const string& groupId)
{
	// TODO: maybe rather use a hash map than an array to have a faster access
	for (auto object3DGroup : object3dGroups) {
		if (object3DGroup->group->getId() == groupId) {
			return object3DGroup->mesh;
		}
	}
	return nullptr;
}

void Object3DBase::initialize()
{
	auto meshManager = Engine::getInstance()->getMeshManager();
	//
	// init mesh
	for (auto i = 0; i < object3dGroups.size(); i++) {
		auto object3DGroup = object3dGroups[i];
		// initiate mesh if not yet done, happens usually after disposing from engine and readding to engine
		if (object3DGroup->mesh == nullptr) {
			vector<map<string, Matrix4x4*>*> instancesTransformationsMatrices;
			vector<map<string, Matrix4x4*>*> instancesSkinningGroupsMatrices;
			for (auto animation: object3DGroup->object->instanceAnimations) {
				instancesTransformationsMatrices.push_back(&animation->transformationsMatrices[0]);
				instancesSkinningGroupsMatrices.push_back(animation->getSkinningGroupsMatrices(object3DGroup->group));
			}
			if (usesManagers == true) {
				object3DGroup->mesh = meshManager->getMesh(object3DGroup->id);
				if (object3DGroup->mesh == nullptr) {
					object3DGroup->mesh = Object3DGroupMesh::createMesh(
						object3DGroup->renderer,
						animationProcessingTarget,
						object3DGroup->group,
						instancesTransformationsMatrices,
						instancesSkinningGroupsMatrices,
						instances
					);
				}
			} else {
				object3DGroup->mesh = Object3DGroupMesh::createMesh(
					object3DGroup->renderer,
					animationProcessingTarget,
					object3DGroup->group,
					instancesTransformationsMatrices,
					instancesSkinningGroupsMatrices,
					instances
				);
			}
		}
	}
}

void Object3DBase::dispose()
{
	auto meshManager = Engine::getInstance()->getMeshManager();
	// dispose mesh
	for (auto i = 0; i < object3dGroups.size(); i++) {
		auto object3DGroup = object3dGroups[i];
		// dispose renderer
		object3DGroup->renderer->dispose();
		// dispose object3d group
		object3DGroup->dispose();
		// dispose mesh
		if (usesManagers == true) {
			meshManager->removeMesh(object3DGroup->id);
		} else {
			delete object3DGroup->mesh;
		}
		object3DGroup->mesh = nullptr;
	}
	//
}
