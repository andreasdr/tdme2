#include <tdme/engine/subsystems/rendering/Object3DInternal.h>

#include <string>

#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Face.h>
#include <tdme/engine/model/FacesEntity.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/model/Node.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/primitives/BoundingVolume.h>
#include <tdme/engine/subsystems/rendering/ModelUtilitiesInternal.h>
#include <tdme/engine/subsystems/rendering/Object3DNode.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/FrameBuffer.h>
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
using tdme::engine::subsystems::rendering::Object3DInternal;
using tdme::engine::subsystems::rendering::Object3DNode;
using tdme::engine::Engine;
using tdme::engine::FrameBuffer;
using tdme::math::Vector3;

Object3DInternal::Object3DInternal(const string& id, Model* model, int instances) :
	Object3DBase(model, true, Engine::animationProcessingTarget, instances)
{
	this->id = id;
	enabled = true;
	pickable = false;
	contributesShadows = false;
	receivesShadows = false;
	effectColorMul.set(1.0f, 1.0f, 1.0f, 1.0f);
	effectColorAdd.set(0.0f, 0.0f, 0.0f, 0.0f);
	boundingBox.fromBoundingVolume(model->getBoundingBox());
	updateBoundingBox();
}

Object3DInternal::~Object3DInternal() {
}

void Object3DInternal::bindDiffuseTexture(FrameBuffer* frameBuffer, const string& nodeId, const string& facesEntityId)
{
	bindDiffuseTexture(frameBuffer->getColorBufferTextureId(), nodeId, facesEntityId);
}

void Object3DInternal::unbindDiffuseTexture(const string& nodeId, const string& facesEntityId)
{
	bindDiffuseTexture(Object3DNode::TEXTUREID_NONE, nodeId, facesEntityId);
}

void Object3DInternal::bindDiffuseTexture(int32_t textureId, const string& nodeId, const string& facesEntityId)
{
	for (auto i = 0; i < object3dNodes.size(); i++) {
		auto object3DNode = object3dNodes[i];
		// skip if a node is desired but not matching
		if (nodeId != "" && nodeId != object3DNode->node->getId())
			continue;

		auto& facesEntities = object3DNode->node->getFacesEntities();
		for (auto facesEntityIdx = 0; facesEntityIdx < facesEntities.size(); facesEntityIdx++) {
			auto& facesEntity = facesEntities[facesEntityIdx];
			// skip if a faces entity is desired but not matching
			if (facesEntityId != "" && facesEntityId != facesEntity.getId())
				continue;
			// set dynamic texture id
			object3DNode->specularMaterialDynamicDiffuseTextureIdsByEntities[facesEntityIdx] = textureId;
		}
	}
}

void Object3DInternal::setTextureMatrix(const Matrix2D3x3& textureMatrix, const string& nodeId, const string& facesEntityId) {
	for (auto i = 0; i < object3dNodes.size(); i++) {
		auto object3DNode = object3dNodes[i];
		// skip if a node is desired but not matching
		if (nodeId != "" && nodeId != object3DNode->node->getId())
			continue;

		auto& facesEntities = object3DNode->node->getFacesEntities();
		for (auto facesEntityIdx = 0; facesEntityIdx < facesEntities.size(); facesEntityIdx++) {
			auto& facesEntity = facesEntities[facesEntityIdx];
			// skip if a faces entity is desired but not matching
			if (facesEntityId != "" && facesEntityId != facesEntity.getId())
				continue;
			// set dynamic texture id
			object3DNode->textureMatricesByEntities[facesEntityIdx].set(textureMatrix);
		}
	}
}

void Object3DInternal::setNodeTransformationsMatrix(const string& id, const Matrix4x4& matrix) {
	Object3DBase::setNodeTransformationsMatrix(id, matrix);
	map<string, Matrix4x4*> _overriddenTransformationsMatrices;
	for (auto overriddenTransformationsMatrixIt: instanceAnimations[currentInstance]->overriddenTransformationsMatrices) {
		_overriddenTransformationsMatrices[overriddenTransformationsMatrixIt.first] = new Matrix4x4(*overriddenTransformationsMatrixIt.second);
	}
	auto newBoundingBox = ModelUtilitiesInternal::createBoundingBox(this->getModel(), _overriddenTransformationsMatrices);
	boundingBox.fromBoundingVolume(newBoundingBox);
	delete newBoundingBox;
}

void Object3DInternal::unsetNodeTransformationsMatrix(const string& id) {
	Object3DBase::unsetNodeTransformationsMatrix(id);
	map<string, Matrix4x4*> _overriddenTransformationsMatrices;
	for (auto overriddenTransformationsMatrixIt: instanceAnimations[currentInstance]->overriddenTransformationsMatrices) {
		_overriddenTransformationsMatrices[overriddenTransformationsMatrixIt.first] = new Matrix4x4(*overriddenTransformationsMatrixIt.second);
	}
	auto newBoundingBox = ModelUtilitiesInternal::createBoundingBox(this->getModel(), _overriddenTransformationsMatrices);
	boundingBox.fromBoundingVolume(newBoundingBox);
	delete newBoundingBox;
}

void Object3DInternal::fromTransformations(const Transformations& transformations)
{
	instanceTransformations[currentInstance].fromTransformations(transformations);
	updateBoundingBox();
}

void Object3DInternal::update()
{
	instanceTransformations[currentInstance].update();
	updateBoundingBox();
}

void Object3DInternal::updateBoundingBox() {
	BoundingBox instanceBoundingBox;
	boundingBoxTransformed.fromBoundingVolumeWithTransformations(model->getBoundingBox(), instanceTransformations[0]);
	for (auto i = 1; i < instances; i++) {
		instanceBoundingBox.fromBoundingVolumeWithTransformations(model->getBoundingBox(), instanceTransformations[i]);
		boundingBoxTransformed.extend(&instanceBoundingBox);
	}
}
