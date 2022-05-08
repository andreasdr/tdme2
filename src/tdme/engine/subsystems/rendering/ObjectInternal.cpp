#include <tdme/engine/subsystems/rendering/ObjectInternal.h>

#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Face.h>
#include <tdme/engine/model/FacesEntity.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/model/Node.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/primitives/BoundingVolume.h>
#include <tdme/engine/subsystems/rendering/ModelUtilitiesInternal.h>
#include <tdme/engine/subsystems/rendering/ObjectNode.h>
#include <tdme/engine/ColorTexture.h>
#include <tdme/engine/Engine.h>
#include <tdme/math/Vector3.h>

using std::string;

using tdme::engine::model::Color4;
using tdme::engine::model::Face;
using tdme::engine::model::FacesEntity;
using tdme::engine::model::Model;
using tdme::engine::model::Node;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::primitives::BoundingVolume;
using tdme::engine::subsystems::rendering::ModelUtilitiesInternal;
using tdme::engine::subsystems::rendering::ObjectInternal;
using tdme::engine::subsystems::rendering::ObjectNode;
using tdme::engine::ColorTexture;
using tdme::engine::Engine;
using tdme::math::Vector3;

ObjectInternal::ObjectInternal(const string& id, Model* model, int instances) :
	ObjectBase(model, true, Engine::animationProcessingTarget, instances)
{
	this->id = id;
	enabled = true;
	pickable = false;
	contributesShadows = false;
	receivesShadows = false;
	effectColorMul.set(1.0f, 1.0f, 1.0f, 1.0f);
	effectColorAdd.set(0.0f, 0.0f, 0.0f, 0.0f);
	updateBoundingBox();
}

ObjectInternal::~ObjectInternal() {
}

void ObjectInternal::bindDiffuseTexture(ColorTexture* texture, const string& nodeId, const string& facesEntityId)
{
	bindDiffuseTexture(texture != nullptr?texture->getColorTextureId():ObjectNode::TEXTUREID_NONE, nodeId, facesEntityId);
}

void ObjectInternal::unbindDiffuseTexture(const string& nodeId, const string& facesEntityId)
{
	bindDiffuseTexture(ObjectNode::TEXTUREID_NONE, nodeId, facesEntityId);
}

void ObjectInternal::bindDiffuseTexture(int32_t textureId, const string& nodeId, const string& facesEntityId)
{
	for (auto i = 0; i < objectNodes.size(); i++) {
		auto objectNode = objectNodes[i];
		// skip if a node is desired but not matching
		if (nodeId != "" && nodeId != objectNode->node->getId())
			continue;

		auto& facesEntities = objectNode->node->getFacesEntities();
		for (auto facesEntityIdx = 0; facesEntityIdx < facesEntities.size(); facesEntityIdx++) {
			auto& facesEntity = facesEntities[facesEntityIdx];
			// skip if a faces entity is desired but not matching
			if (facesEntityId != "" && facesEntityId != facesEntity.getId())
				continue;
			// set dynamic texture id
			objectNode->specularMaterialDynamicDiffuseTextureIdsByEntities[facesEntityIdx] = textureId;
		}
	}
}

void ObjectInternal::setTextureMatrix(const Matrix2D3x3& textureMatrix, const string& nodeId, const string& facesEntityId) {
	for (auto i = 0; i < objectNodes.size(); i++) {
		auto objectNode = objectNodes[i];
		// skip if a node is desired but not matching
		if (nodeId != "" && nodeId != objectNode->node->getId())
			continue;

		auto& facesEntities = objectNode->node->getFacesEntities();
		for (auto facesEntityIdx = 0; facesEntityIdx < facesEntities.size(); facesEntityIdx++) {
			auto& facesEntity = facesEntities[facesEntityIdx];
			// skip if a faces entity is desired but not matching
			if (facesEntityId != "" && facesEntityId != facesEntity.getId())
				continue;
			// set dynamic texture id
			objectNode->textureMatricesByEntities[facesEntityIdx].set(textureMatrix);
		}
	}
}

void ObjectInternal::setNodeTransformMatrix(const string& id, const Matrix4x4& matrix) {
	ObjectBase::setNodeTransformMatrix(id, matrix);
	map<string, Matrix4x4*> _overriddenTransformMatrices;
	for (auto overriddenTransformMatrixIt: instanceAnimations[currentInstance]->overriddenTransformMatrices) {
		_overriddenTransformMatrices[overriddenTransformMatrixIt.first] = new Matrix4x4(*overriddenTransformMatrixIt.second);
	}
	auto newBoundingBox = ModelUtilitiesInternal::createBoundingBox(this->getModel(), _overriddenTransformMatrices);
	boundingBox.fromBoundingVolume(newBoundingBox);
	delete newBoundingBox;
}

void ObjectInternal::unsetNodeTransformMatrix(const string& id) {
	ObjectBase::unsetNodeTransformMatrix(id);
	map<string, Matrix4x4*> _overriddenTransformMatrices;
	for (auto overriddenTransformMatrixIt: instanceAnimations[currentInstance]->overriddenTransformMatrices) {
		_overriddenTransformMatrices[overriddenTransformMatrixIt.first] = new Matrix4x4(*overriddenTransformMatrixIt.second);
	}
	auto newBoundingBox = ModelUtilitiesInternal::createBoundingBox(this->getModel(), _overriddenTransformMatrices);
	boundingBox.fromBoundingVolume(newBoundingBox);
	delete newBoundingBox;
}

void ObjectInternal::fromTransform(const Transform& transform)
{
	instanceTransform[currentInstance].fromTransform(transform);
	updateBoundingBox();
}

void ObjectInternal::update()
{
	instanceTransform[currentInstance].update();
	updateBoundingBox();
}

void ObjectInternal::updateBoundingBox() {
	BoundingBox instanceBoundingBox;
	boundingBox.fromBoundingVolume(model->getBoundingBox());
	boundingBoxTransformed.fromBoundingVolumeWithTransform(model->getBoundingBox(), instanceTransform[0]);
	for (auto i = 1; i < instances; i++) {
		instanceBoundingBox.fromBoundingVolumeWithTransform(model->getBoundingBox(), instanceTransform[i]);
		boundingBoxTransformed.extend(&instanceBoundingBox);
	}
}