#include <tdme/engine/subsystems/rendering/Object3DGroupMesh.h>

#include <map>
#include <string>

#include <tdme/utilities/ByteBuffer.h>
#include <tdme/utilities/FloatBuffer.h>
#include <tdme/utilities/ShortBuffer.h>
#include <tdme/engine/model/Face.h>
#include <tdme/engine/model/FacesEntity.h>
#include <tdme/engine/model/Group.h>
#include <tdme/engine/model/Joint.h>
#include <tdme/engine/model/JointWeight.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/model/Skinning.h>
#include <tdme/engine/model/TextureCoordinate.h>
#include <tdme/engine/subsystems/rendering/ObjectBuffer.h>
#include <tdme/engine/subsystems/rendering/Object3DBase.h>
#include <tdme/engine/subsystems/rendering/Object3DGroupMesh.h>
#include <tdme/engine/subsystems/rendering/Object3DGroupRenderer.h>
#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/engine/subsystems/skinning/SkinningShader.h>
#include <tdme/math/Math.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector3.h>
#include <tdme/utilities/Console.h>

using std::map;
using std::string;

using tdme::engine::subsystems::rendering::Object3DGroupMesh;
using tdme::utilities::ByteBuffer;
using tdme::utilities::FloatBuffer;
using tdme::utilities::ShortBuffer;
using tdme::engine::model::Face;
using tdme::engine::model::FacesEntity;
using tdme::engine::model::Group;
using tdme::engine::model::Joint;
using tdme::engine::model::JointWeight;
using tdme::engine::model::Skinning;
using tdme::engine::model::TextureCoordinate;
using tdme::engine::subsystems::rendering::Object3DBase;
using tdme::engine::subsystems::rendering::Object3DGroupRenderer;
using tdme::engine::subsystems::rendering::ObjectBuffer;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::subsystems::skinning::SkinningShader;
using tdme::math::Math;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;
using tdme::utilities::Console;

Object3DGroupMesh::Object3DGroupMesh()
{
	object3DGroupRenderer = nullptr;
	group = nullptr;
	vertices = nullptr;
	normals = nullptr;
	tangents = nullptr;
	bitangents = nullptr;
	cSkinningMaxVertexWeights = -1;
	cGroupTransformationsMatrix = nullptr;
	skinning = false;
	skinningJoints = -1;
	faceCount = 0;
	recreatedBuffers = false;
	animationProcessingTarget = Engine::AnimationProcessingTarget::NONE;
}

Object3DGroupMesh* Object3DGroupMesh::createMesh(Object3DGroupRenderer* object3DGroupRenderer, Engine::AnimationProcessingTarget animationProcessingTarget, Group* group, const vector<map<string, Matrix4x4*>*>& transformationMatrices, const vector<map<string, Matrix4x4*>*>& skinningMatrices, int instances)
{
	auto mesh = new Object3DGroupMesh();
	//
	mesh->instances = instances;
	mesh->object3DGroupRenderer = object3DGroupRenderer;
	mesh->group = group;
	// group data
	auto& groupVertices = group->getVertices();
	auto& groupNormals = group->getNormals();
	auto& groupTextureCoordinates = group->getTextureCoordinates();
	auto& groupTangents = group->getTangents();
	auto& groupBitangents = group->getBitangents();
	// determine face count
	mesh->faceCount = group->getFaceCount();
	// animation processing target
	mesh->animationProcessingTarget = animationProcessingTarget;
	// transformations for skinned meshes
	auto skinning = group->getSkinning();
	mesh->skinning = skinning != nullptr;
	mesh->skinningMatrices = skinningMatrices;
	if (skinning != nullptr) {
		mesh->jointsSkinningMatrices.resize(instances);
		for (auto i = 0; i < instances; i++) {
			for (auto& joint: skinning->getJoints()) {
				mesh->jointsSkinningMatrices[i].push_back(skinningMatrices[i]->find(joint.getGroupId())->second);
			}
		}
	}
	// set up transformed vertices, normals and friends
	if (mesh->instances > 1 || (skinning != nullptr && animationProcessingTarget == Engine::AnimationProcessingTarget::CPU) ||
		animationProcessingTarget == Engine::AnimationProcessingTarget::CPU_NORENDERING) {
		// transformed mesh vertices
		mesh->transformedVertices.resize(groupVertices.size() * mesh->instances);
		mesh->vertices = &mesh->transformedVertices;
		{
			auto idx = 0;
			for (auto i = 0; i < mesh->instances; i++)
			for (auto j = 0; j < groupVertices.size(); j++) {
				mesh->transformedVertices[idx++].set(groupVertices[j]);
			}
		}
		// transformed mesh normals
		mesh->transformedNormals.resize(groupNormals.size() * mesh->instances);
		mesh->normals = &mesh->transformedNormals;
		{
			auto idx = 0;
			for (auto i = 0; i < mesh->instances; i++)
			for (auto j = 0; j < groupNormals.size(); j++) {
				mesh->transformedNormals[idx++].set(groupNormals[j]);
			}
		}
		if (mesh->instances > 1) {
			// transformed mesh texture coordinates
			mesh->transformedTextureCoordinates.resize(groupTextureCoordinates.size() * mesh->instances);
			mesh->textureCoordinates = &mesh->transformedTextureCoordinates;
			{
				auto idx = 0;
				for (auto i = 0; i < mesh->instances; i++)
				for (auto j = 0; j < groupTextureCoordinates.size(); j++) {
					mesh->transformedTextureCoordinates[idx++].set(groupTextureCoordinates[j]);
				}
			}
		} else {
			mesh->textureCoordinates = &groupTextureCoordinates;
		}
		// transformed mesh tangents
		if (groupTangents.size() > 0) {
			mesh->transformedTangents.resize(groupTangents.size() * mesh->instances);
			mesh->tangents = &mesh->transformedTangents;
			{
				auto idx = 0;
				for (auto i = 0; i < mesh->instances; i++)
				for (auto j = 0; j < groupTangents.size(); j++) {
					mesh->transformedTangents[idx++].set(groupTangents[j]);
				}
			}
		}
		// transformed mesh bitangents
		if (groupBitangents.size() > 0) {
			mesh->transformedBitangents.resize(groupBitangents.size() * mesh->instances);
			mesh->bitangents = &mesh->transformedBitangents;
			{
				auto idx = 0;
				for (auto i = 0; i < mesh->instances; i++)
				for (auto j = 0; j < groupBitangents.size(); j++) {
					mesh->transformedBitangents[idx++].set(groupBitangents[j]);
				}
			}
		}
	} else {
		// no transformations on CPU, we can use model data
		mesh->vertices = &groupVertices;
		mesh->normals = &groupNormals;
		mesh->textureCoordinates = &groupTextureCoordinates;
		if (groupTangents.size() > 0) {
			mesh->tangents = &groupTangents;
		}
		if (groupBitangents.size() > 0) {
			mesh->bitangents = &groupBitangents;
		}
	}

	// indices
	auto indicesCount = 0;
	for (auto& facesEntity : group->getFacesEntities()) {
		indicesCount += 3 * facesEntity.getFaces().size();
	}
	mesh->indices.resize(mesh->instances * indicesCount);
	{
		auto j = 0;
		for (auto& facesEntity : group->getFacesEntities()) {
			for (auto i = 0; i < mesh->instances; i++) {
				for (auto& face : facesEntity.getFaces())
				for (auto& vertexIndex : face.getVertexIndices()) {
					mesh->indices[j++] = groupVertices.size() * i + vertexIndex;
				}
			}
		}
	}

	//
	mesh->recreatedBuffers = false;
	// group transformations matrix
	if (mesh->animationProcessingTarget == Engine::AnimationProcessingTarget::CPU ||
		mesh->animationProcessingTarget == Engine::AnimationProcessingTarget::CPU_NORENDERING ||
		mesh->animationProcessingTarget == Engine::AnimationProcessingTarget::GPU) {
		// group transformations matrix
		mesh->cGroupTransformationsMatrix = transformationMatrices[0]->find(group->getId())->second;
	}
	// skinning
	if ((skinning != nullptr &&
		(animationProcessingTarget == Engine::AnimationProcessingTarget::CPU || animationProcessingTarget == Engine::AnimationProcessingTarget::CPU_NORENDERING))) {
		// skinning computation caches if computing skinning on CPU
		if (mesh->animationProcessingTarget == Engine::AnimationProcessingTarget::CPU || mesh->animationProcessingTarget == Engine::AnimationProcessingTarget::CPU_NORENDERING) {
			mesh->cSkinningJointWeight.resize(groupVertices.size());
			mesh->cSkinningJointTransformationsMatrices.resize(mesh->instances);
			for (auto i = 0; i < mesh->instances; i++) mesh->cSkinningJointTransformationsMatrices[i].resize(groupVertices.size());
			// compute joint weight caches
			auto& joints = skinning->getJoints();
			auto& weights = skinning->getWeights();
			auto& jointsWeights = skinning->getVerticesJointsWeights();
			for (auto vertexIndex = 0; vertexIndex < groupVertices.size(); vertexIndex++) {
				auto vertexJointWeights = jointsWeights[vertexIndex].size();
				if (vertexJointWeights > mesh->cSkinningMaxVertexWeights) mesh->cSkinningMaxVertexWeights = vertexJointWeights;
				mesh->cSkinningJointWeight[vertexIndex].resize(vertexJointWeights);
				for (auto i = 0; i < mesh->instances; i++) mesh->cSkinningJointTransformationsMatrices[i][vertexIndex].resize(vertexJointWeights);
				{
					auto jointWeightIdx = 0;
					for (auto& jointWeight : jointsWeights[vertexIndex]) {
						auto& joint = joints[jointWeight.getJointIndex()];
						mesh->cSkinningJointWeight[vertexIndex][jointWeightIdx] = weights[jointWeight.getWeightIndex()];
						// next
						jointWeightIdx++;
					}
				}
				for (auto i = 0; i < mesh->instances; i++) {
					auto jointWeightIdx = 0;
					for (auto& jointWeight : jointsWeights[vertexIndex]) {
						auto& joint = joints[jointWeight.getJointIndex()];
						auto skinningMatrixIt = skinningMatrices[i]->find(joint.getGroupId());
						mesh->cSkinningJointTransformationsMatrices[i][vertexIndex][jointWeightIdx] = skinningMatrixIt->second;
						// next
						jointWeightIdx++;
					}
				}
			}
		}
	}
	mesh->recreateBuffers();
	return mesh;
}

void Object3DGroupMesh::computeTransformations(void* context, Object3DBase* object3DBase)
{
	// transformations for skinned meshes
	auto skinning = group->getSkinning();
	if (skinning != nullptr) {
		// compute skinning on CPU if required
		if (animationProcessingTarget == Engine::AnimationProcessingTarget::GPU) {
			Engine::getSkinningShader()->computeSkinning(context, object3DBase, this);
		} else
		if (animationProcessingTarget == Engine::AnimationProcessingTarget::CPU || animationProcessingTarget == Engine::AnimationProcessingTarget::CPU_NORENDERING) {
			Vector3 tmpVector3;
			auto& groupVertices = group->getVertices();
			auto& groupNormals = group->getNormals();
			auto& groupTangent = group->getTangents();
			auto& groupBitangent = group->getBitangents();
			auto& jointsWeights = skinning->getVerticesJointsWeights();
			const Vector3* vertex;
			Vector3* transformedVertex;
			const Vector3* normal;
			Vector3* transformedNormal;
			const Vector3* tangent;
			Vector3* transformedTangent;
			const Vector3* bitangent;
			Vector3* transformedBitangent;
			float totalWeights;
			float weightNormalized;
			auto j = 0;
			Matrix4x4 transformationsMatrix; // TODO: try to avoid multiplying matrix with at each vertex
			auto currentInstance = object3DBase->getCurrentInstance();
			for (auto i = 0; i < instances; i++) {
				if (object3DBase->instanceEnabled[i] == false) continue;
				object3DBase->setCurrentInstance(i);
				for (auto vertexIndex = 0; vertexIndex < groupVertices.size(); vertexIndex++) {
					// do vertices
					vertex = &groupVertices[vertexIndex];
					transformedVertex = &transformedVertices[groupVertices.size() * j + vertexIndex].set(0.0f, 0.0f, 0.0f);
					normal = &groupNormals[vertexIndex];
					transformedNormal = &transformedNormals[groupVertices.size() * j + vertexIndex].set(0.0f, 0.0f, 0.0f);
					tangent = tangents != nullptr?&groupTangent[vertexIndex]:nullptr;
					transformedTangent = tangents != nullptr?&transformedTangents[groupVertices.size() * j + vertexIndex].set(0.0f, 0.0f, 0.0f):nullptr;
					bitangent = bitangents != nullptr?&groupBitangent[vertexIndex]:nullptr;
					transformedBitangent = bitangents != nullptr?&transformedBitangents[groupVertices.size() * j + vertexIndex].set(0.0f, 0.0f, 0.0f):nullptr;
					// compute every influence on vertex and vertex normals
					totalWeights = 0.0f;
					for (auto vertexJointWeightIdx = 0; vertexJointWeightIdx < jointsWeights[vertexIndex].size(); vertexJointWeightIdx++) {
						auto weight = cSkinningJointWeight[vertexIndex][vertexJointWeightIdx];
						// skinning transformation matrix
						transformationsMatrix.set(*cSkinningJointTransformationsMatrices[i][vertexIndex][vertexJointWeightIdx]).multiply(object3DBase->getTransformationsMatrix());
						// vertex
						transformedVertex->add(transformationsMatrix.multiply(*vertex, tmpVector3).scale(weight));
						// normals
						transformedNormal->add(transformationsMatrix.multiplyNoTranslation(*normal, tmpVector3).scale(weight));
						// tangent
						if (tangent != nullptr && transformedTangent != nullptr) {
							transformedTangent->add(transformationsMatrix.multiplyNoTranslation(*tangent, tmpVector3).scale(weight));
						}
						// bitangent
						if (bitangent != nullptr && transformedBitangent != nullptr) {
							transformedBitangent->add(transformationsMatrix.multiplyNoTranslation(*bitangent, tmpVector3).scale(weight));
						}
						//
						totalWeights += weight;
					}
					// scale to full weight
					if (Math::abs(totalWeights - 1.0f) > Math::EPSILON) {
						weightNormalized = 1.0f / totalWeights;
						// vertex
						transformedVertex->scale(weightNormalized);
						// normals
						transformedNormal->scale(weightNormalized);
						// tangent
						if (transformedTangent != nullptr) {
							transformedTangent->scale(weightNormalized);
						}
						// bitangent
						if (transformedBitangent != nullptr) {
							transformedBitangent->scale(weightNormalized);
						}
					}
					// normalize normal
					transformedNormal->normalize();
				}
				j++;
			}
			object3DBase->setCurrentInstance(currentInstance);
			// recreate buffers
			recreateBuffers();
		}
	} else
	if (animationProcessingTarget == Engine::AnimationProcessingTarget::CPU_NORENDERING) {
		Vector3 tmpVector3;
		auto& groupVertices = group->getVertices();
		auto& groupNormals = group->getNormals();
		// transformations for non skinned rendering
		//	vertices
		for (auto vertexIndex = 0; vertexIndex < groupVertices.size(); vertexIndex++) {
			transformedVertices[vertexIndex].set(cGroupTransformationsMatrix->multiply(groupVertices[vertexIndex], tmpVector3));
		}
		//	normals
		for (auto normalIndex = 0; normalIndex < groupNormals.size(); normalIndex++) {
			transformedNormals[normalIndex].set(cGroupTransformationsMatrix->multiplyNoTranslation(groupNormals[normalIndex], tmpVector3).normalize());
		}
		//	TODO: tangents, bitangents, but actually it is only in use for computing bounding volumes, so I am not in a hurry
		// recreate buffers
		recreateBuffers();
	}
}

void Object3DGroupMesh::recreateBuffers()
{
	recreatedBuffers = true;
}

bool Object3DGroupMesh::getRecreatedBuffers()
{
	if (recreatedBuffers == true) {
		recreatedBuffers = false;
		return true;
	} else {
		return false;
	}
}

void Object3DGroupMesh::setupVertexIndicesBuffer(Renderer *renderer, void *context, int32_t vboId) {
	// upload
	if (renderer->isUsingShortIndices() == true) {
		if (instances * indices.size() > 65535) {
			Console::println(
				"Object3DGroupMesh::setupVertexIndicesBuffer(): " +
				group->getModel()->getName() + ":" +
				group->getName() + ":" +
				"more than 2^16-1 indices: " +
				to_string(indices.size())
			);
		}
		auto sbIndices = ObjectBuffer::getByteBuffer(context, instances * faceCount * 3 * sizeof(uint16_t))->asShortBuffer();
		// create face vertex indices, will never be changed in engine
		for (auto i = 0; i < instances; i++)
		for (auto index: indices) {
			sbIndices.put(index);
		}
		// done, upload
		renderer->uploadIndicesBufferObject(context, vboId, sbIndices.getPosition() * sizeof(uint16_t), &sbIndices);
	} else {
		auto ibIndices = ObjectBuffer::getByteBuffer(context, instances * faceCount * 3 * sizeof(uint32_t))->asIntBuffer();
		// create face vertex indices, will never be changed in engine
		for (auto i = 0; i < instances; i++)
		for (auto index: indices) {
			ibIndices.put(index);
		}
		// done, upload
		renderer->uploadIndicesBufferObject(context, vboId, ibIndices.getPosition() * sizeof(uint32_t), &ibIndices);
	}
}


void Object3DGroupMesh::setupTextureCoordinatesBuffer(Renderer* renderer, void* context, int32_t vboId)
{
	if (textureCoordinates->size() == 0) return;
	// create texture coordinates buffer, will never be changed in engine
	auto fbTextureCoordinates = ObjectBuffer::getByteBuffer(context, textureCoordinates->size() * 2 * sizeof(float))->asFloatBuffer();
	// construct texture coordinates byte buffer as this will not change usually
	for (auto& textureCoordinate: *textureCoordinates) {
		fbTextureCoordinates.put(textureCoordinate.getArray());
	}
	// done, upload
	renderer->uploadBufferObject(context, vboId, fbTextureCoordinates.getPosition() * sizeof(float), &fbTextureCoordinates);
}

void Object3DGroupMesh::setupVerticesBuffer(Renderer* renderer, void* context, int32_t vboId)
{
	auto fbVertices = ObjectBuffer::getByteBuffer(context, vertices->size() * 3 * sizeof(float))->asFloatBuffer();
	// create vertices buffers
	for (auto& vertex: *vertices) {
		fbVertices.put(vertex.getArray());
	}
	// done, upload
	renderer->uploadBufferObject(context, vboId, fbVertices.getPosition() * sizeof(float), &fbVertices);
}

void Object3DGroupMesh::setupNormalsBuffer(Renderer* renderer, void* context, int32_t vboId)
{
	auto fbNormals = ObjectBuffer::getByteBuffer(context, normals->size() * 3 * sizeof(float))->asFloatBuffer();
	// create normals buffers
	for (auto& normal: *normals) {
		fbNormals.put(normal.getArray());
	}
	// done, upload
	renderer->uploadBufferObject(context, vboId, fbNormals.getPosition() * sizeof(float), &fbNormals);
}

void Object3DGroupMesh::setupTangentsBuffer(Renderer* renderer, void* context, int32_t vboId)
{
	// check if we have tangents
	if (tangents == nullptr) return;
	auto fbTangents = ObjectBuffer::getByteBuffer(context, tangents->size() * 3 * sizeof(float))->asFloatBuffer();
	// create tangents buffers
	for (auto& tangent: *tangents) {
		fbTangents.put(tangent.getArray());
	}
	// done, upload
	renderer->uploadBufferObject(context, vboId, fbTangents.getPosition() * sizeof(float), &fbTangents);
}

void Object3DGroupMesh::setupBitangentsBuffer(Renderer* renderer, void* context, int32_t vboId)
{
	// check if we have bitangents
	if (bitangents == nullptr) return;
	auto fbBitangents = ObjectBuffer::getByteBuffer(context, bitangents->size() * 3 * sizeof(float))->asFloatBuffer();
	// create bitangents buffers
	for (auto& bitangent: *bitangents) {
		fbBitangents.put(bitangent.getArray());
	}
	// done, upload
	renderer->uploadBufferObject(context, vboId, fbBitangents.getPosition() * sizeof(float), &fbBitangents);
}

void Object3DGroupMesh::setupOriginsBuffer(Renderer* renderer, void* context, int32_t vboId) {
	// check if we have texture coordinates
	auto& origins = group->getOrigins();
	if (origins.size() == 0) return;
	// create texture coordinates buffer, will never be changed in engine
	auto fbOrigins = ObjectBuffer::getByteBuffer(context, origins.size() * 3 * sizeof(float))->asFloatBuffer();
	// construct float buffer as this will not change usually
	for (auto& origin: origins) {
		fbOrigins.put(origin.getArray());
	}
	// done, upload
	renderer->uploadBufferObject(context, vboId, fbOrigins.getPosition() * sizeof(float), &fbOrigins);
}
