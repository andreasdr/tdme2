#include <tdme/engine/subsystems/rendering/ModelUtilitiesInternal.h>

#include <map>
#include <string>

#include <tdme/engine/Timing.h>
#include <tdme/engine/model/AnimationSetup.h>
#include <tdme/engine/model/Face.h>
#include <tdme/engine/model/FacesEntity.h>
#include <tdme/engine/model/Node.h>
#include <tdme/engine/model/Material.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/model/SpecularMaterialProperties.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/subsystems/rendering/AnimationState.h>
#include <tdme/engine/subsystems/rendering/ModelStatistics.h>
#include <tdme/engine/subsystems/rendering/Object3DNode.h>
#include <tdme/engine/subsystems/rendering/Object3DNodeMesh.h>
#include <tdme/engine/subsystems/rendering/Object3DModelInternal.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector3.h>

using std::map;
using std::string;

using tdme::engine::subsystems::rendering::ModelUtilitiesInternal;
using tdme::engine::Timing;
using tdme::engine::model::AnimationSetup;
using tdme::engine::model::Face;
using tdme::engine::model::Node;
using tdme::engine::model::Material;
using tdme::engine::model::Model;
using tdme::engine::model::FacesEntity;
using tdme::engine::model::SpecularMaterialProperties;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::subsystems::rendering::AnimationState;
using tdme::engine::subsystems::rendering::ModelStatistics;
using tdme::engine::subsystems::rendering::Object3DNode;
using tdme::engine::subsystems::rendering::Object3DNodeMesh;
using tdme::engine::subsystems::rendering::Object3DModelInternal;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;

BoundingBox* ModelUtilitiesInternal::createBoundingBox(Model* model, const map<string, Matrix4x4*> overriddenNodeTransformationsMatrices)
{
	Object3DModelInternal object3dModel(model);
	object3dModel.instanceAnimations[0]->overriddenTransformationsMatrices = overriddenNodeTransformationsMatrices;
	auto boundingBox = ModelUtilitiesInternal::createBoundingBox(&object3dModel);
	if (boundingBox == nullptr) boundingBox = ModelUtilitiesInternal::createBoundingBoxNoMesh(&object3dModel);
	return boundingBox;
}

BoundingBox* ModelUtilitiesInternal::createBoundingBox(Object3DModelInternal* object3DModelInternal)
{
	auto model = object3DModelInternal->getModel();
	auto defaultAnimation = model->getAnimationSetup(Model::ANIMATIONSETUP_DEFAULT);
	float minX = 0.0f, minY = 0.0f, minZ = 0.0f;
	float maxX = 0.0f, maxY = 0.0f, maxZ = 0.0f;
	auto firstVertex = true;
	// create bounding box for whole animation at 60fps
	AnimationState animationState;
	animationState.setup = defaultAnimation;
	animationState.lastAtTime = Timing::UNDEFINED;
	animationState.currentAtTime = 0LL;
	animationState.time = 0.0f;
	animationState.finished = false;
	for (auto t = 0.0f; t <= (defaultAnimation != nullptr ? static_cast< float >(defaultAnimation->getFrames()) : 0.0f) / model->getFPS(); t += 1.0f / model->getFPS()) {
		// calculate transformations matrices without world transformations
		auto parentTransformationsMatrix = object3DModelInternal->getModel()->getImportTransformationsMatrix();
		parentTransformationsMatrix.multiply(object3DModelInternal->getTransformationsMatrix());
		object3DModelInternal->instanceAnimations[0]->computeTransformationsMatrices(object3DModelInternal->instanceAnimations[0]->nodeLists[0], parentTransformationsMatrix, &animationState);
		Object3DNode::computeTransformations(nullptr, object3DModelInternal->object3dNodes);
		// parse through object nodes to determine min, max
		for (auto object3DNode : object3DModelInternal->object3dNodes) {
			for (auto& vertex : *object3DNode->mesh->vertices) {
				auto& vertexXYZ = vertex.getArray();
				if (firstVertex == true) {
					minX = vertexXYZ[0];
					minY = vertexXYZ[1];
					minZ = vertexXYZ[2];
					maxX = vertexXYZ[0];
					maxY = vertexXYZ[1];
					maxZ = vertexXYZ[2];
					firstVertex = false;
				} else {
					if (vertexXYZ[0] < minX) minX = vertexXYZ[0];
					if (vertexXYZ[1] < minY) minY = vertexXYZ[1];
					if (vertexXYZ[2] < minZ) minZ = vertexXYZ[2];
					if (vertexXYZ[0] > maxX) maxX = vertexXYZ[0];
					if (vertexXYZ[1] > maxY) maxY = vertexXYZ[1];
					if (vertexXYZ[2] > maxZ) maxZ = vertexXYZ[2];
				}
			}
		}
		animationState.currentAtTime = static_cast< int64_t >((t * 1000.0f));
		animationState.lastAtTime = static_cast< int64_t >((t * 1000.0f));
	}
	// skip on models without mesh
	if (firstVertex == true) return nullptr;
	// otherwise go with bounding box
	return new BoundingBox(Vector3(minX, minY, minZ), Vector3(maxX, maxY, maxZ));
}

BoundingBox* ModelUtilitiesInternal::createBoundingBoxNoMesh(Object3DModelInternal* object3DModelInternal)
{
	auto model = object3DModelInternal->getModel();
	auto defaultAnimation = model->getAnimationSetup(Model::ANIMATIONSETUP_DEFAULT);
	float minX = 0.0f, minY = 0.0f, minZ = 0.0f;
	float maxX = 0.0f, maxY = 0.0f, maxZ = 0.0f;
	auto firstVertex = true;
	// create bounding box for whole animation at 60fps
	AnimationState animationState;
	animationState.setup = defaultAnimation;
	animationState.lastAtTime = Timing::UNDEFINED;
	animationState.currentAtTime = 0LL;
	animationState.time = 0.0f;
	animationState.finished = false;
	Vector3 vertex;
	for (auto t = 0.0f; t <= (defaultAnimation != nullptr ? static_cast< float >(defaultAnimation->getFrames()) : 0.0f) / model->getFPS(); t += 1.0f / model->getFPS()) {
		// calculate transformations matrices without world transformations
		auto parentTransformationsMatrix = object3DModelInternal->getModel()->getImportTransformationsMatrix();
		parentTransformationsMatrix.multiply(object3DModelInternal->getTransformationsMatrix());
		object3DModelInternal->instanceAnimations[0]->computeTransformationsMatrices(object3DModelInternal->instanceAnimations[0]->nodeLists[0], parentTransformationsMatrix, &animationState);
		for (auto nodeIt: model->getNodes()) {
			auto& transformedNodeMatrix = object3DModelInternal->getNodeTransformationsMatrix(nodeIt.second->getId());
			transformedNodeMatrix.multiply(vertex.set(0.0f, 0.0f, 0.0f), vertex);
			if (firstVertex == true) {
				minX = vertex[0];
				minY = vertex[1];
				minZ = vertex[2];
				maxX = vertex[0];
				maxY = vertex[1];
				maxZ = vertex[2];
				firstVertex = false;
			} else {
				if (vertex[0] < minX) minX = vertex[0];
				if (vertex[1] < minY) minY = vertex[1];
				if (vertex[2] < minZ) minZ = vertex[2];
				if (vertex[0] > maxX) maxX = vertex[0];
				if (vertex[1] > maxY) maxY = vertex[1];
				if (vertex[2] > maxZ) maxZ = vertex[2];
			}
		}
		animationState.currentAtTime = static_cast< int64_t >((t * 1000.0f));
		animationState.lastAtTime = static_cast< int64_t >((t * 1000.0f));
	}
	// skip on models without nodes
	if (firstVertex == true) return nullptr;
	// otherwise go with bounding box
	return new BoundingBox(Vector3(minX, minY, minZ), Vector3(maxX, maxY, maxZ));
}

void ModelUtilitiesInternal::invertNormals(Model* model)
{
	invertNormals(model->getSubNodes());
}

void ModelUtilitiesInternal::invertNormals(const map<string, Node*>& nodes)
{
	for (auto it: nodes) {
		Node* node = it.second;
		auto normals = node->getNormals();
		for (auto& normal : normals) {
			// invert
			normal.scale(-1.0f);
		}
		node->setNormals(normals);
		// process sub nodes
		invertNormals(node->getSubNodes());
	}
}

void ModelUtilitiesInternal::computeModelStatistics(Model* model, ModelStatistics* modelStatistics)
{
	Object3DModelInternal object3DModelInternal(model);
	computeModelStatistics(&object3DModelInternal, modelStatistics);
}

void ModelUtilitiesInternal::computeModelStatistics(Object3DModelInternal* object3DModelInternal, ModelStatistics* modelStatistics)
{
	map<string, int32_t> materialCountById;
	auto opaqueFaceCount = 0;
	auto transparentFaceCount = 0;
	for (auto object3DNode : object3DModelInternal->object3dNodes) {
		// check each faces entity
		auto& facesEntities = object3DNode->node->getFacesEntities();
		auto facesEntityIdxCount = facesEntities.size();
		for (auto faceEntityIdx = 0; faceEntityIdx < facesEntityIdxCount; faceEntityIdx++) {
			auto& facesEntity = facesEntities[faceEntityIdx];
			auto faces = facesEntity.getFaces().size();
			// material
			auto material = facesEntity.getMaterial();
			// determine if transparent
			auto transparentFacesEntity = false;
			//	via material
			if (material != nullptr) {
				auto specularMaterialProperties = material->getSpecularMaterialProperties();
				if (specularMaterialProperties != nullptr &&
					(specularMaterialProperties->hasColorTransparency() == true || specularMaterialProperties->hasTextureTransparency() == true))
					transparentFacesEntity = true;

			}
			// setup material usage
			auto materialId = material == nullptr ? "tdme.material.none" : material->getId();
			materialCountById[materialId]++;
			// skip, if requested
			if (transparentFacesEntity == true) {
				// keep track of rendered faces
				transparentFaceCount += faces;
				// skip to next entity
				continue;
			}
			opaqueFaceCount += faces;
		}
	}
	// determine final material count
	auto materialCount = materialCountById.size();
	modelStatistics->opaqueFaceCount = opaqueFaceCount;
	modelStatistics->transparentFaceCount = transparentFaceCount;
	modelStatistics->materialCount = materialCount;
}

bool ModelUtilitiesInternal::equals(Model* model1, Model* model2)
{
	Object3DModelInternal object3DModel1(model1);
	Object3DModelInternal object3DModel2(model2);
	return ModelUtilitiesInternal::equals(&object3DModel1, &object3DModel2);
}

bool ModelUtilitiesInternal::equals(Object3DModelInternal* object3DModel1Internal, Object3DModelInternal* object3DModel2Internal)
{
	// check number of object 3d nodes
	if (object3DModel1Internal->object3dNodes.size() != object3DModel2Internal->object3dNodes.size())
		return false;

	for (auto i = 0; i < object3DModel1Internal->object3dNodes.size(); i++) {
		auto object3DNodeModel1 = object3DModel1Internal->object3dNodes[i];
		auto object3DNodeModel2 = object3DModel2Internal->object3dNodes[i];
		auto node1 = object3DModel1Internal->object3dNodes[i]->node;
		auto node2 = object3DModel2Internal->object3dNodes[i]->node;
		auto& facesEntitiesModel1 = object3DNodeModel1->node->getFacesEntities();
		auto& facesEntitiesModel2 = object3DNodeModel2->node->getFacesEntities();
		// check transformation matrix
		if (object3DNodeModel1->node->getTransformationsMatrix().equals(object3DNodeModel2->node->getTransformationsMatrix()) == false)
			return false;
		// check vertices count
		if (node1->getVertices().size() != node2->getVertices().size())
			return false;
		// check vertices
		for (auto j = 0; j < node1->getVertices().size(); j++) {
			if (node1->getVertices()[j].equals(node2->getVertices()[j]) == false)
				return false;
		}
		// check normals count
		if (node1->getNormals().size() != node2->getNormals().size())
			return false;
		// check normals
		for (auto j = 0; j < node1->getNormals().size(); j++) {
			if (node1->getNormals()[j].equals(node2->getNormals()[j]) == false)
				return false;
		}
		// check number of faces entities
		if (facesEntitiesModel1.size() != facesEntitiesModel2.size())
			return false;
		// check each faces entity
		for (auto j = 0; j < facesEntitiesModel1.size(); j++) {
			auto facesEntityModel1 = facesEntitiesModel1[j];
			auto facesEntityModel2 = facesEntitiesModel2[j];
			// check material
			//	TODO: check if it should be allowed to have NULL material
			if (facesEntityModel1.getMaterial() == nullptr && facesEntityModel2.getMaterial() != nullptr)
				return false;

			if (facesEntityModel1.getMaterial() != nullptr && facesEntityModel2.getMaterial() == nullptr)
				return false;

			if (facesEntityModel1.getMaterial() != nullptr && facesEntityModel2.getMaterial() != nullptr &&
				facesEntityModel1.getMaterial()->getId() != facesEntityModel2.getMaterial()->getId()) {
				return false;
			}
			// check faces
			auto& facesModel1 = facesEntityModel1.getFaces();
			auto& facesModel2 = facesEntityModel2.getFaces();
			// number of faces in faces entity
			if (facesModel1.size() != facesModel2.size())
				return false;
			// face indices
			for (auto k = 0; k < facesModel1.size(); k++) {
				// vertex indices
				auto vertexIndicesModel1 = facesModel1[k].getVertexIndices();
				auto vertexIndicesModel2 = facesModel2[k].getVertexIndices();
				if (vertexIndicesModel1[0] != vertexIndicesModel2[0] ||
					vertexIndicesModel1[1] != vertexIndicesModel2[1] ||
					vertexIndicesModel1[2] != vertexIndicesModel2[2]) {
					return false;
				}
				// TODO: maybe other indices
			}
			// TODO: check vertices, normals and such
		}
	}
	//
	return true;
}
