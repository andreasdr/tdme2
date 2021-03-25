#include <tdme/engine/model/Model.h>

#include <map>
#include <string>

#include <tdme/engine/model/Animation.h>
#include <tdme/engine/model/AnimationSetup.h>
#include <tdme/engine/model/Material.h>
#include <tdme/engine/model/Node.h>
#include <tdme/engine/model/RotationOrder.h>
#include <tdme/engine/model/ShaderModel.h>
#include <tdme/engine/model/UpVector.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/subsystems/rendering/Object3DModelInternal.h>
#include <tdme/engine/ModelUtilities.h>
#include <tdme/engine/Object3DModel.h>
#include <tdme/math/Matrix4x4.h>

using std::map;
using std::string;

using tdme::engine::model::Animation;
using tdme::engine::model::AnimationSetup;
using tdme::engine::model::Material;
using tdme::engine::model::Model;
using tdme::engine::model::Node;
using tdme::engine::model::RotationOrder;
using tdme::engine::model::ShaderModel;
using tdme::engine::model::UpVector;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::subsystems::rendering::Object3DModelInternal;
using tdme::engine::ModelUtilities;
using tdme::engine::Object3DModel;
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
	this->boundingBoxUpdated = false;
}

Model::~Model() {
	deleteSubNodes(subNodes);
	for (auto it = materials.begin(); it != materials.end(); ++it) {
		delete it->second;
	}
	for (auto it = animationSetups.begin(); it != animationSetups.end(); ++it) {
		delete it->second;
	}
	if (boundingBox != nullptr) delete boundingBox;
}

void Model::deleteSubNodes(const map<string, Node*>& subNodes) {
	for (auto it = subNodes.begin(); it != subNodes.end(); ++it) {
		deleteSubNodes(it->second->getSubNodes());
		delete it->second;
	}
}

Node* Model::getNodeById(const string& id)
{
	auto nodeIt = nodes.find(id);
	if (nodeIt != nodes.end()) {
		return nodeIt->second;
	}
	return nullptr;

}

Node* Model::getSubNodeById(const string& id)
{
	auto nodeIt = subNodes.find(id);
	if (nodeIt != subNodes.end()) {
		return nodeIt->second;
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
	auto animationSetup = new AnimationSetup(this, id, startFrame, endFrame, loop, string(), speed);
	animationSetups[id] = animationSetup;
	return animationSetup;
}

AnimationSetup* Model::addOverlayAnimationSetup(const string& id, const string& overlayFromNodeId, int32_t startFrame, int32_t endFrame, bool loop, float speed)
{
	auto animationSetup = new AnimationSetup(this, id, startFrame, endFrame, loop, overlayFromNodeId, speed);
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

bool Model::computeTransformationsMatrix(const map<string, Node*>& nodes, const Matrix4x4& parentTransformationsMatrix, int32_t frame, const string& nodeId, Matrix4x4& transformationsMatrix)
{
	// iterate through nodes
	for (auto it: nodes) {
		Node* node = it.second;
		// compute animation matrix if animation setups exist
		auto animation = node->getAnimation();
		if (animation != nullptr) {
			auto& animationMatrices = animation->getTransformationsMatrices();
			transformationsMatrix.set(animationMatrices[frame % animationMatrices.size()]);
		} else {
			// no animation matrix, set up local transformation matrix up as node matrix
			transformationsMatrix.set(node->getTransformationsMatrix());
		}

		// apply parent transformation matrix
		transformationsMatrix.multiply(parentTransformationsMatrix);

		// return matrix if node matches
		if (node->getId() == nodeId) return true;

		// calculate sub nodes
		auto& subNodes = node->getSubNodes();
		if (subNodes.size() > 0) {
			auto haveTransformationsMatrix = computeTransformationsMatrix(subNodes, transformationsMatrix.clone(), frame, nodeId, transformationsMatrix);
			if (haveTransformationsMatrix == true) return true;
		}
	}

	//
	return false;
}

void Model::invalidateBoundingBox() {
	if (boundingBox != nullptr) {
		delete boundingBox;
		boundingBox = nullptr;
		boundingBoxUpdated = true;
	}
}
