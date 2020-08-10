#include <tdme/engine/model/Model.h>

#include <map>
#include <string>

#include <tdme/engine/ModelUtilities.h>
#include <tdme/engine/Object3DModel.h>
#include <tdme/engine/model/Animation.h>
#include <tdme/engine/model/AnimationSetup.h>
#include <tdme/engine/model/Group.h>
#include <tdme/engine/model/Material.h>
#include <tdme/engine/model/UpVector.h>
#include <tdme/engine/model/RotationOrder.h>
#include <tdme/engine/model/ShaderModel.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/subsystems/rendering/Object3DModelInternal.h>
#include <tdme/math/Matrix4x4.h>

using std::map;
using std::string;

using tdme::engine::model::Model;
using tdme::engine::ModelUtilities;
using tdme::engine::Object3DModel;
using tdme::engine::model::Animation;
using tdme::engine::model::AnimationSetup;
using tdme::engine::model::Group;
using tdme::engine::model::Material;
using tdme::engine::model::UpVector;
using tdme::engine::model::RotationOrder;
using tdme::engine::model::ShaderModel;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::subsystems::rendering::Object3DModelInternal;
using tdme::math::Matrix4x4;

string Model::ANIMATIONSETUP_DEFAULT = "tdme.default";

constexpr float Model::FPS_DEFAULT;

Model::Model(const string& id, const string& name, UpVector* upVector, RotationOrder* rotationOrder, BoundingBox* boundingBox, AuthoringTool authoringTool)
{
	this->id = id;
	this->name = name;
	this->upVector = upVector;
	this->rotationOrder = rotationOrder;
	this->shaderModel = ShaderModel::SPECULAR;
	skinning = false;
	fps = FPS_DEFAULT;
	importTransformationsMatrix.identity();
	this->boundingBox = boundingBox;
	this->authoringTool = authoringTool;
}

Model::~Model() {
	deleteSubGroups(subGroups);
	for (auto it = materials.begin(); it != materials.end(); ++it) {
		delete it->second;
	}
	for (auto it = animationSetups.begin(); it != animationSetups.end(); ++it) {
		delete it->second;
	}
	if (boundingBox != nullptr) delete boundingBox;
}

void Model::deleteSubGroups(const map<string, Group*>& subGroups) {
	for (auto it = subGroups.begin(); it != subGroups.end(); ++it) {
		deleteSubGroups(it->second->getSubGroups());
		delete it->second;
	}
}

Group* Model::getGroupById(const string& id)
{
	auto groupIt = groups.find(id);
	if (groupIt != groups.end()) {
		return groupIt->second;
	}
	return nullptr;

}

Group* Model::getSubGroupById(const string& id)
{
	auto groupIt = subGroups.find(id);
	if (groupIt != subGroups.end()) {
		return groupIt->second;
	}
	return nullptr;
}

AnimationSetup* Model::addAnimationSetup(const string& id, int32_t startFrame, int32_t endFrame, bool loop, float speed)
{
	auto animationSetupIt = animationSetups.find(id);
	if (animationSetupIt != animationSetups.end()) {
		delete animationSetupIt->second;
		animationSetups.erase(animationSetupIt);
	}
	auto animationSetup = new AnimationSetup(this, id, startFrame, endFrame, loop, "", speed);
	animationSetups[id] = animationSetup;
	return animationSetup;
}

AnimationSetup* Model::addOverlayAnimationSetup(const string& id, const string& overlayFromGroupId, int32_t startFrame, int32_t endFrame, bool loop, float speed)
{
	auto animationSetup = new AnimationSetup(this, id, startFrame, endFrame, loop, overlayFromGroupId, speed);
	animationSetups[id] = animationSetup;
	return animationSetup;
}

AnimationSetup* Model::getAnimationSetup(const string& id)
{
	auto animationSetupIt = animationSetups.find(id);
	if (animationSetupIt != animationSetups.end()) {
		return animationSetupIt->second;
	}
	return nullptr;
}

BoundingBox* Model::getBoundingBox()
{
	// TODO: return const bb
	if (boundingBox == nullptr) {
		boundingBox = ModelUtilities::createBoundingBox(this);
	}
	return boundingBox;
}

bool Model::computeTransformationsMatrix(const map<string, Group*>& groups, const Matrix4x4& parentTransformationsMatrix, int32_t frame, const string& groupId, Matrix4x4& transformationsMatrix)
{
	// iterate through groups
	for (auto it: groups) {
		Group* group = it.second;
		// compute animation matrix if animation setups exist
		auto animation = group->getAnimation();
		if (animation != nullptr) {
			auto& animationMatrices = animation->getTransformationsMatrices();
			transformationsMatrix.set(animationMatrices[frame % animationMatrices.size()]);
		} else {
			// no animation matrix, set up local transformation matrix up as group matrix
			transformationsMatrix.set(group->getTransformationsMatrix());
		}

		// apply parent transformation matrix
		transformationsMatrix.multiply(parentTransformationsMatrix);

		// return matrix if group matches
		if (group->getId() == groupId) return true;

		// calculate sub groups
		auto& subGroups = group->getSubGroups();
		if (subGroups.size() > 0) {
			auto haveTransformationsMatrix = computeTransformationsMatrix(subGroups, transformationsMatrix.clone(), frame, groupId, transformationsMatrix);
			if (haveTransformationsMatrix == true) return true;
		}
	}

	//
	return false;
}
