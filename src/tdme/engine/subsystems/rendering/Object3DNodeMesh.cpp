#include <tdme/engine/subsystems/rendering/Object3DNodeMesh.h>

#include <map>
#include <string>

#include <tdme/utilities/ByteBuffer.h>
#include <tdme/utilities/FloatBuffer.h>
#include <tdme/utilities/ShortBuffer.h>
#include <tdme/engine/model/Face.h>
#include <tdme/engine/model/FacesEntity.h>
#include <tdme/engine/model/Node.h>
#include <tdme/engine/model/Joint.h>
#include <tdme/engine/model/JointWeight.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/model/Skinning.h>
#include <tdme/engine/model/TextureCoordinate.h>
#include <tdme/engine/subsystems/rendering/ObjectBuffer.h>
#include <tdme/engine/subsystems/rendering/Object3DBase.h>
#include <tdme/engine/subsystems/rendering/Object3DNodeMesh.h>
#include <tdme/engine/subsystems/rendering/Object3DNodeRenderer.h>
#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/engine/subsystems/skinning/SkinningShader.h>
#include <tdme/math/Math.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector3.h>
#include <tdme/utilities/Console.h>

using std::map;
using std::string;

using tdme::engine::subsystems::rendering::Object3DNodeMesh;
using tdme::utilities::ByteBuffer;
using tdme::utilities::FloatBuffer;
using tdme::utilities::ShortBuffer;
using tdme::engine::model::Face;
using tdme::engine::model::FacesEntity;
using tdme::engine::model::Node;
using tdme::engine::model::Joint;
using tdme::engine::model::JointWeight;
using tdme::engine::model::Skinning;
using tdme::engine::model::TextureCoordinate;
using tdme::engine::subsystems::rendering::Object3DBase;
using tdme::engine::subsystems::rendering::Object3DNodeRenderer;
using tdme::engine::subsystems::rendering::ObjectBuffer;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::subsystems::skinning::SkinningShader;
using tdme::math::Math;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;
using tdme::utilities::Console;

Object3DNodeMesh::Object3DNodeMesh(Object3DNodeRenderer* object3DNodeRenderer, Engine::AnimationProcessingTarget animationProcessingTarget, Node* node, const vector<map<string, Matrix4x4*>*>& transformationMatrices, const vector<map<string, Matrix4x4*>*>& skinningMatrices, int instances)
{
	//
	this->instances = instances;
	this->object3DNodeRenderer = object3DNodeRenderer;
	this->node = node;
	// node data
	auto& nodeVertices = node->getVertices();
	auto& nodeNormals = node->getNormals();
	auto& nodeTextureCoordinates = node->getTextureCoordinates();
	auto& nodeTangents = node->getTangents();
	auto& nodeBitangents = node->getBitangents();
	// determine face count
	faceCount = node->getFaceCount();
	// animation processing target
	this->animationProcessingTarget = animationProcessingTarget;
	// transformations for skinned meshes
	auto skinning = node->getSkinning();
	this->skinning = skinning != nullptr;
	this->skinningMatrices = skinningMatrices;
	if (skinning != nullptr) {
		jointsSkinningMatrices.resize(instances);
		for (auto i = 0; i < instances; i++) {
			for (auto& joint: skinning->getJoints()) {
				jointsSkinningMatrices[i].push_back(skinningMatrices[i]->find(joint.getNodeId())->second);
			}
		}
	}
	// set up transformed vertices, normals and friends
	if (instances > 1 || (skinning != nullptr && animationProcessingTarget == Engine::AnimationProcessingTarget::CPU) ||
		animationProcessingTarget == Engine::AnimationProcessingTarget::CPU_NORENDERING) {
		// transformed mesh vertices
		transformedVertices.resize(nodeVertices.size() * instances);
		vertices = &transformedVertices;
		{
			auto idx = 0;
			for (auto i = 0; i < instances; i++)
			for (auto j = 0; j < nodeVertices.size(); j++) {
				transformedVertices[idx++].set(nodeVertices[j]);
			}
		}
		// transformed mesh normals
		transformedNormals.resize(nodeNormals.size() * instances);
		normals = &transformedNormals;
		{
			auto idx = 0;
			for (auto i = 0; i < instances; i++)
			for (auto j = 0; j < nodeNormals.size(); j++) {
				transformedNormals[idx++].set(nodeNormals[j]);
			}
		}
		if (instances > 1) {
			// transformed mesh texture coordinates
			transformedTextureCoordinates.resize(nodeTextureCoordinates.size() * instances);
			textureCoordinates = &transformedTextureCoordinates;
			{
				auto idx = 0;
				for (auto i = 0; i < instances; i++)
				for (auto j = 0; j < nodeTextureCoordinates.size(); j++) {
					transformedTextureCoordinates[idx++].set(nodeTextureCoordinates[j]);
				}
			}
		} else {
			textureCoordinates = &nodeTextureCoordinates;
		}
		// transformed mesh tangents
		if (nodeTangents.size() > 0) {
			transformedTangents.resize(nodeTangents.size() * instances);
			tangents = &transformedTangents;
			{
				auto idx = 0;
				for (auto i = 0; i < instances; i++)
				for (auto j = 0; j < nodeTangents.size(); j++) {
					transformedTangents[idx++].set(nodeTangents[j]);
				}
			}
		}
		// transformed mesh bitangents
		if (nodeBitangents.size() > 0) {
			transformedBitangents.resize(nodeBitangents.size() * instances);
			bitangents = &transformedBitangents;
			{
				auto idx = 0;
				for (auto i = 0; i < instances; i++)
				for (auto j = 0; j < nodeBitangents.size(); j++) {
					transformedBitangents[idx++].set(nodeBitangents[j]);
				}
			}
		}
	} else {
		// no transformations on CPU, we can use model data
		vertices = &nodeVertices;
		normals = &nodeNormals;
		textureCoordinates = &nodeTextureCoordinates;
		if (nodeTangents.size() > 0) {
			tangents = &nodeTangents;
		}
		if (nodeBitangents.size() > 0) {
			bitangents = &nodeBitangents;
		}
	}

	// indices
	auto indicesCount = 0;
	for (auto& facesEntity : node->getFacesEntities()) {
		indicesCount += 3 * facesEntity.getFaces().size();
	}
	indices.resize(instances * indicesCount);
	{
		auto j = 0;
		for (auto& facesEntity : node->getFacesEntities()) {
			for (auto i = 0; i < instances; i++) {
				for (auto& face : facesEntity.getFaces())
				for (auto& vertexIndex : face.getVertexIndices()) {
					indices[j++] = nodeVertices.size() * i + vertexIndex;
				}
			}
		}
	}

	//
	recreatedBuffers = false;
	// node transformations matrix
	if (animationProcessingTarget == Engine::AnimationProcessingTarget::CPU ||
		animationProcessingTarget == Engine::AnimationProcessingTarget::CPU_NORENDERING ||
		animationProcessingTarget == Engine::AnimationProcessingTarget::GPU) {
		// node transformations matrix
		cNodeTransformationsMatrix = transformationMatrices[0]->find(node->getId())->second;
	}
	// skinning
	if ((skinning != nullptr &&
		(animationProcessingTarget == Engine::AnimationProcessingTarget::CPU || animationProcessingTarget == Engine::AnimationProcessingTarget::CPU_NORENDERING))) {
		// skinning computation caches if computing skinning on CPU
		if (animationProcessingTarget == Engine::AnimationProcessingTarget::CPU || animationProcessingTarget == Engine::AnimationProcessingTarget::CPU_NORENDERING) {
			cSkinningJointWeight.resize(nodeVertices.size());
			cSkinningJointTransformationsMatrices.resize(instances);
			for (auto i = 0; i < instances; i++) cSkinningJointTransformationsMatrices[i].resize(nodeVertices.size());
			// compute joint weight caches
			auto& joints = skinning->getJoints();
			auto& weights = skinning->getWeights();
			auto& jointsWeights = skinning->getVerticesJointsWeights();
			for (auto vertexIndex = 0; vertexIndex < nodeVertices.size(); vertexIndex++) {
				auto vertexJointWeights = jointsWeights[vertexIndex].size();
				if (vertexJointWeights > cSkinningMaxVertexWeights) cSkinningMaxVertexWeights = vertexJointWeights;
				cSkinningJointWeight[vertexIndex].resize(vertexJointWeights);
				for (auto i = 0; i < instances; i++) cSkinningJointTransformationsMatrices[i][vertexIndex].resize(vertexJointWeights);
				{
					auto jointWeightIdx = 0;
					for (auto& jointWeight : jointsWeights[vertexIndex]) {
						auto& joint = joints[jointWeight.getJointIndex()];
						cSkinningJointWeight[vertexIndex][jointWeightIdx] = weights[jointWeight.getWeightIndex()];
						// next
						jointWeightIdx++;
					}
				}
				for (auto i = 0; i < instances; i++) {
					auto jointWeightIdx = 0;
					for (auto& jointWeight : jointsWeights[vertexIndex]) {
						auto& joint = joints[jointWeight.getJointIndex()];
						auto skinningMatrixIt = skinningMatrices[i]->find(joint.getNodeId());
						cSkinningJointTransformationsMatrices[i][vertexIndex][jointWeightIdx] = skinningMatrixIt->second;
						// next
						jointWeightIdx++;
					}
				}
			}
		}
	}
	recreateBuffers();
}

void Object3DNodeMesh::computeTransformations(void* context, Object3DBase* object3DBase)
{
	// transformations for skinned meshes
	auto skinning = node->getSkinning();
	if (skinning != nullptr) {
		// compute skinning on CPU if required
		if (animationProcessingTarget == Engine::AnimationProcessingTarget::GPU) {
			Engine::getSkinningShader()->computeSkinning(context, object3DBase, this);
		} else
		if (animationProcessingTarget == Engine::AnimationProcessingTarget::CPU || animationProcessingTarget == Engine::AnimationProcessingTarget::CPU_NORENDERING) {
			Vector3 tmpVector3;
			auto& nodeVertices = node->getVertices();
			auto& nodeNormals = node->getNormals();
			auto& nodeTangent = node->getTangents();
			auto& nodeBitangent = node->getBitangents();
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
				for (auto vertexIndex = 0; vertexIndex < nodeVertices.size(); vertexIndex++) {
					// do vertices
					vertex = &nodeVertices[vertexIndex];
					transformedVertex = &transformedVertices[nodeVertices.size() * j + vertexIndex].set(0.0f, 0.0f, 0.0f);
					normal = &nodeNormals[vertexIndex];
					transformedNormal = &transformedNormals[nodeVertices.size() * j + vertexIndex].set(0.0f, 0.0f, 0.0f);
					tangent = tangents != nullptr?&nodeTangent[vertexIndex]:nullptr;
					transformedTangent = tangents != nullptr?&transformedTangents[nodeVertices.size() * j + vertexIndex].set(0.0f, 0.0f, 0.0f):nullptr;
					bitangent = bitangents != nullptr?&nodeBitangent[vertexIndex]:nullptr;
					transformedBitangent = bitangents != nullptr?&transformedBitangents[nodeVertices.size() * j + vertexIndex].set(0.0f, 0.0f, 0.0f):nullptr;
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
		auto& nodeVertices = node->getVertices();
		auto& nodeNormals = node->getNormals();
		// transformations for non skinned rendering
		//	vertices
		for (auto vertexIndex = 0; vertexIndex < nodeVertices.size(); vertexIndex++) {
			transformedVertices[vertexIndex].set(cNodeTransformationsMatrix->multiply(nodeVertices[vertexIndex], tmpVector3));
		}
		//	normals
		for (auto normalIndex = 0; normalIndex < nodeNormals.size(); normalIndex++) {
			transformedNormals[normalIndex].set(cNodeTransformationsMatrix->multiplyNoTranslation(nodeNormals[normalIndex], tmpVector3).normalize());
		}
		//	TODO: tangents, bitangents, but actually it is only in use for computing bounding volumes, so I am not in a hurry
		// recreate buffers
		recreateBuffers();
	}
}

void Object3DNodeMesh::recreateBuffers()
{
	recreatedBuffers = true;
}

bool Object3DNodeMesh::getRecreatedBuffers()
{
	if (recreatedBuffers == true) {
		recreatedBuffers = false;
		return true;
	} else {
		return false;
	}
}

void Object3DNodeMesh::setupVertexIndicesBuffer(Renderer *renderer, void *context, int32_t vboId) {
	// upload
	if (renderer->isUsingShortIndices() == true) {
		if (instances * indices.size() > 65535) {
			Console::println(
				"Object3DNodeMesh::setupVertexIndicesBuffer(): " +
				node->getModel()->getName() + ":" +
				node->getName() + ":" +
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


void Object3DNodeMesh::setupTextureCoordinatesBuffer(Renderer* renderer, void* context, int32_t vboId)
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

void Object3DNodeMesh::setupVerticesBuffer(Renderer* renderer, void* context, int32_t vboId)
{
	auto fbVertices = ObjectBuffer::getByteBuffer(context, vertices->size() * 3 * sizeof(float))->asFloatBuffer();
	// create vertices buffers
	for (auto& vertex: *vertices) {
		fbVertices.put(vertex.getArray());
	}
	// done, upload
	renderer->uploadBufferObject(context, vboId, fbVertices.getPosition() * sizeof(float), &fbVertices);
}

void Object3DNodeMesh::setupNormalsBuffer(Renderer* renderer, void* context, int32_t vboId)
{
	auto fbNormals = ObjectBuffer::getByteBuffer(context, normals->size() * 3 * sizeof(float))->asFloatBuffer();
	// create normals buffers
	for (auto& normal: *normals) {
		fbNormals.put(normal.getArray());
	}
	// done, upload
	renderer->uploadBufferObject(context, vboId, fbNormals.getPosition() * sizeof(float), &fbNormals);
}

void Object3DNodeMesh::setupTangentsBuffer(Renderer* renderer, void* context, int32_t vboId)
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

void Object3DNodeMesh::setupBitangentsBuffer(Renderer* renderer, void* context, int32_t vboId)
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

void Object3DNodeMesh::setupOriginsBuffer(Renderer* renderer, void* context, int32_t vboId) {
	// check if we have texture coordinates
	auto& origins = node->getOrigins();
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
