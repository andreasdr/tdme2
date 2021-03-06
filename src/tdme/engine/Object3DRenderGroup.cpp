#include <tdme/engine/Object3DRenderGroup.h>

#include <string>
#include <vector>

#include <tdme/engine/model/Face.h>
#include <tdme/engine/model/FacesEntity.h>
#include <tdme/engine/model/Material.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/model/Node.h>
#include <tdme/engine/model/RotationOrder.h>
#include <tdme/engine/model/TextureCoordinate.h>
#include <tdme/engine/model/UpVector.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/LODObject3D.h>
#include <tdme/engine/Object3D.h>
#include <tdme/engine/Partition.h>
#include <tdme/engine/Transformations.h>
#include <tdme/math/Math.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector3.h>
#include <tdme/utilities/ModelTools.h>

using std::string;
using std::to_string;
using std::vector;

using tdme::engine::model::Face;
using tdme::engine::model::FacesEntity;
using tdme::engine::model::Material;
using tdme::engine::model::Model;
using tdme::engine::model::Node;
using tdme::engine::model::RotationOrder;
using tdme::engine::model::TextureCoordinate;
using tdme::engine::model::UpVector;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::Engine;
using tdme::engine::LODObject3D;
using tdme::engine::Object3D;
using tdme::engine::Object3DRenderGroup;
using tdme::engine::Partition;
using tdme::engine::Transformations;
using tdme::math::Math;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;
using tdme::utilities::ModelTools;

Object3DRenderGroup::Object3DRenderGroup(
	const string& id,
	int lodLevels,
	float modelLOD2MinDistance,
	float modelLOD3MinDistance,
	int modelLOD2ReduceBy,
	int modelLOD3ReduceBy,
	bool optimizeModels
):
	id(id)
{
	this->enabled = true;
	this->pickable = false;
	this->contributesShadows = false;
	this->receivesShadows = false;
	this->effectColorMul.set(1.0f, 1.0f, 1.0f, 1.0f);
	this->effectColorAdd.set(0.0f, 0.0f, 0.0f, 0.0f);
	this->combinedModels.resize(Math::clamp(lodLevels, 1, 3));
	this->combinedEntity = nullptr;
	this->lodReduceBy[0] = 1;
	this->lodReduceBy[1] = modelLOD2ReduceBy;
	this->lodReduceBy[2] = modelLOD3ReduceBy;
	this->modelLOD2MinDistance = modelLOD2MinDistance;
	this->modelLOD3MinDistance = modelLOD3MinDistance;
	this->optimizeModels = optimizeModels;
}

Object3DRenderGroup::~Object3DRenderGroup() {
	if (combinedEntity != nullptr) delete combinedEntity;
	for (auto combinedModel: combinedModels) {
		if (combinedModel != nullptr) delete combinedModel;
	}
}

void Object3DRenderGroup::combineNode(Node* sourceNode, const vector<Vector3>& origins, const vector<Matrix4x4>& objectParentTransformationsMatrices, Model* combinedModel) {
	// create node in combined model
	auto combinedModelNode = combinedModel->getNodeById(sourceNode->getId());
	if (combinedModelNode == nullptr) {
		combinedModelNode = new Node(
			combinedModel,
			sourceNode->getParentNode() == nullptr?nullptr:combinedModel->getNodeById(sourceNode->getParentNode()->getId()),
			sourceNode->getId(),
			sourceNode->getName()
		);
		if (sourceNode->getParentNode() == nullptr) {
			combinedModel->getSubNodes()[combinedModelNode->getId()] = combinedModelNode;
		} else {
			combinedModelNode->getParentNode()->getSubNodes()[combinedModelNode->getId()] = combinedModelNode;
		}
		combinedModel->getNodes()[combinedModelNode->getId()] = combinedModelNode;
	}

	{
		auto sourceNodeVerticesSize = sourceNode->getVertices().size();
		auto sourceNodeNormalsSize = sourceNode->getNormals().size();
		auto sourceNodeTextureCoordinatesSize = sourceNode->getTextureCoordinates().size();
		auto sourceNodeTangentsSize = sourceNode->getTangents().size();
		auto sourceNodeBitangentsSize = sourceNode->getBitangents().size();

		// vertices and such from new model
		auto combinedModelNodeVertices = combinedModelNode->getVertices();
		auto combinedModelNodeNormals = combinedModelNode->getNormals();
		auto combinedModelNodeTextureCoordinates = combinedModelNode->getTextureCoordinates();
		auto combinedModelNodeTangents = combinedModelNode->getTangents();
		auto combinedModelNodeBitangents = combinedModelNode->getBitangents();
		auto combinedModelNodeFacesEntities = combinedModelNode->getFacesEntities();
		auto combinedModelNodeOrigins = combinedModelNode->getOrigins();

		// current indices
		auto combinedModelNodeVerticesIdxStart = combinedModelNodeVertices.size();
		auto combinedModelNodeNormalsIdxStart = combinedModelNodeNormals.size();
		auto combinedModelNodeTextureCoordinatesIdxStart = combinedModelNodeTextureCoordinates.size();
		auto combinedModelNodeTangentsIdxStart = combinedModelNodeTangents.size();
		auto combinedModelNodeBitangentsIdxStart = combinedModelNodeBitangents.size();

		// add vertices and such from source node to new node
		{
			auto i = 0;
			for (auto& objectParentTransformationsMatrix: objectParentTransformationsMatrices) {
				Matrix4x4 transformationsMatrix;
				transformationsMatrix.set(sourceNode->getTransformationsMatrix());
				transformationsMatrix.multiply(objectParentTransformationsMatrix);

				//
				Vector3 tmpVector3;
				for (auto& vertex: sourceNode->getVertices()) {
					combinedModelNodeOrigins.push_back(origins[i]);
					combinedModelNodeVertices.push_back(transformationsMatrix.multiply(vertex, tmpVector3));
				}
				for (auto& normal: sourceNode->getNormals()) {
					combinedModelNodeNormals.push_back(transformationsMatrix.multiplyNoTranslation(normal, tmpVector3));
				}
				for (auto& textureCoordinate: sourceNode->getTextureCoordinates()) {
					combinedModelNodeTextureCoordinates.push_back(textureCoordinate);
				}
				for (auto& tangent: sourceNode->getTangents()) {
					combinedModelNodeTangents.push_back(transformationsMatrix.multiplyNoTranslation(tangent, tmpVector3));
				}
				for (auto& bitangent: sourceNode->getBitangents()) {
					combinedModelNodeBitangents.push_back(transformationsMatrix.multiplyNoTranslation(bitangent, tmpVector3));
				}
				//
				i++;
			}
		}

		// add source node faces to new new faces entity
		for (auto& facesEntity: sourceNode->getFacesEntities()) {
			bool haveTextureCoordinates = facesEntity.isTextureCoordinatesAvailable();
			bool haveTangentsBitangents = facesEntity.isTangentBitangentAvailable();

			// get faces entity
			FacesEntity* combinedModelNodeFacesEntity = nullptr;
			for (auto& combinedModelNodeFacesEntityExisting: combinedModelNodeFacesEntities) {
				if (combinedModelNodeFacesEntityExisting.getId() == facesEntity.getId()) {
					combinedModelNodeFacesEntity = &combinedModelNodeFacesEntityExisting;
					break;
				}
			}
			// create
			if (combinedModelNodeFacesEntity == nullptr) {
				auto newFacesEntity = FacesEntity(
					combinedModelNode,
					facesEntity.getId()
				);
				combinedModelNodeFacesEntities.push_back(newFacesEntity);
				combinedModelNodeFacesEntity = &combinedModelNodeFacesEntities[combinedModelNodeFacesEntities.size() - 1];
				auto combinedModelNodeFacesEntityMaterial = combinedModel->getMaterials()[facesEntity.getMaterial()->getId()];
				if (combinedModelNodeFacesEntityMaterial == nullptr) {
					combinedModelNodeFacesEntityMaterial = ModelTools::cloneMaterial(facesEntity.getMaterial());
					combinedModel->getMaterials()[combinedModelNodeFacesEntityMaterial->getId()] = combinedModelNodeFacesEntityMaterial;
				}
				combinedModelNodeFacesEntity->setMaterial(combinedModelNodeFacesEntityMaterial);
			}

			//
			auto combinedModelNodeFaces = combinedModelNodeFacesEntity->getFaces();

			//
			auto combinedModelNodeVerticesIdx = combinedModelNodeVerticesIdxStart;
			auto combinedModelNodeNormalsIdx = combinedModelNodeNormalsIdxStart;
			auto combinedModelNodeTextureCoordinatesIdx = combinedModelNodeTextureCoordinatesIdxStart;
			auto combinedModelNodeTangentsIdx = combinedModelNodeTangentsIdxStart;
			auto combinedModelNodeBitangentsIdx = combinedModelNodeBitangentsIdxStart;
			for (auto& objectParentTransformationsMatrix: objectParentTransformationsMatrices) {
				// add faces
				for (auto& face: facesEntity.getFaces()) {
					// get face vertices and such
					auto& faceVertexIndices = face.getVertexIndices();
					auto& faceNormalIndices = face.getNormalIndices();
					auto& faceTextureCoordinatesIndices = face.getTextureCoordinateIndices();
					auto& faceTangentIndices = face.getTangentIndices();
					auto& faceBitangentIndices = face.getBitangentIndices();

					//
					auto combinedModelNodeFace =
						Face(
							combinedModelNode,
							combinedModelNodeVerticesIdx + faceVertexIndices[0],
							combinedModelNodeVerticesIdx + faceVertexIndices[1],
							combinedModelNodeVerticesIdx + faceVertexIndices[2],
							combinedModelNodeNormalsIdx + faceNormalIndices[0],
							combinedModelNodeNormalsIdx + faceNormalIndices[1],
							combinedModelNodeNormalsIdx + faceNormalIndices[2]
						);
					if (haveTextureCoordinates == true) {
						combinedModelNodeFace.setTextureCoordinateIndices(
							combinedModelNodeTextureCoordinatesIdx + faceTextureCoordinatesIndices[0],
							combinedModelNodeTextureCoordinatesIdx + faceTextureCoordinatesIndices[1],
							combinedModelNodeTextureCoordinatesIdx + faceTextureCoordinatesIndices[2]
						);
					}
					if (haveTangentsBitangents == true) {
						combinedModelNodeFace.setTangentIndices(
							combinedModelNodeTangentsIdx + faceTangentIndices[0],
							combinedModelNodeTangentsIdx + faceTangentIndices[1],
							combinedModelNodeTangentsIdx + faceTangentIndices[2]
						);
						combinedModelNodeFace.setBitangentIndices(
							combinedModelNodeBitangentsIdx + faceBitangentIndices[0],
							combinedModelNodeBitangentsIdx + faceBitangentIndices[1],
							combinedModelNodeBitangentsIdx + faceBitangentIndices[2]
						);
					}
					combinedModelNodeFaces.push_back(combinedModelNodeFace);
				}

				//
				combinedModelNodeVerticesIdx+= sourceNodeVerticesSize;
				combinedModelNodeNormalsIdx+= sourceNodeNormalsSize;
				combinedModelNodeTextureCoordinatesIdx+= sourceNodeTextureCoordinatesSize;
				combinedModelNodeTangentsIdx+= sourceNodeTangentsSize;
				combinedModelNodeBitangentsIdx+= sourceNodeBitangentsSize;
			}
			combinedModelNodeFacesEntity->setFaces(combinedModelNodeFaces);
		}

		// store back to model
		combinedModelNode->setVertices(combinedModelNodeVertices);
		combinedModelNode->setNormals(combinedModelNodeNormals);
		combinedModelNode->setTextureCoordinates(combinedModelNodeTextureCoordinates);
		combinedModelNode->setTangents(combinedModelNodeTangents);
		combinedModelNode->setBitangents(combinedModelNodeBitangents);
		combinedModelNode->setFacesEntities(combinedModelNodeFacesEntities);
		combinedModelNode->setOrigins(combinedModelNodeOrigins);
	}

	// do child nodes
	for (auto nodeIt: sourceNode->getSubNodes()) {
		combineNode(nodeIt.second, origins, objectParentTransformationsMatrices, combinedModel);
	}
}

void Object3DRenderGroup::combineObjects(Model* model, const vector<Transformations>& objectsTransformations, Model* combinedModel) {
	vector<Matrix4x4> objectTransformationMatrices;
	vector<Vector3> origins;
	for (auto& objectTransformations: objectsTransformations) {
		Matrix4x4 transformationsMatrix;
		transformationsMatrix.set(model->getImportTransformationsMatrix());
		transformationsMatrix.multiply(objectTransformations.getTransformationsMatrix());
		objectTransformationMatrices.push_back(transformationsMatrix);
		origins.push_back(objectTransformations.getTranslation());
	}
	for (auto nodeIt: model->getSubNodes()) {
		combineNode(nodeIt.second, origins, objectTransformationMatrices, combinedModel);
	}
}

void Object3DRenderGroup::updateRenderGroup() {
	// dispose old object and combined model
	if (combinedEntity != nullptr) {
		combinedEntity->dispose();
		delete combinedEntity;
		combinedEntity = nullptr;
	}

	// combine objects to a new model
	for (auto i = 0; i < combinedModels.size(); i++) {
		combinedModels[i] = new Model(
			id + ".lod." + to_string(i),
			id + ".lod." + to_string(i),
			UpVector::Y_UP,
			RotationOrder::ZYX,
			nullptr
		);
	}

	for (auto& transformationsByModelIt: transformationsByModel) {
		auto model = transformationsByModelIt.first;
		auto& objectsTransformations = transformationsByModelIt.second;
		auto lodLevel = 0;
		for (auto combinedModel: combinedModels) {
			auto reduceByFactor = lodReduceBy[lodLevel];
			lodLevel++;
			auto objectCount = 0;
			vector<Transformations> reducedObjectsTransformations;
			for (auto& objectTransformations: objectsTransformations) {
				if (objectCount % reduceByFactor != 0) {
					objectCount++;
					continue;
				}
				reducedObjectsTransformations.push_back(objectTransformations);
				objectCount++;
			}
			combineObjects(model, reducedObjectsTransformations, combinedModel);
		}
	}

	// create new combined object
	for (auto combinedModel: combinedModels) {
		if (combinedModel != nullptr) {
			// post process combined model
			ModelTools::shrinkToFit(combinedModel);
			ModelTools::createDefaultAnimation(combinedModel, 0);
			ModelTools::setupJoints(combinedModel);
			ModelTools::fixAnimationLength(combinedModel);
		}
	}

	// optimize models
	if (optimizeModels == true) {
		for (auto i = 0; i < combinedModels.size(); i++) {
			combinedModels[i] = ModelTools::optimizeModel(combinedModels[i]);
		}
	}

	if (combinedModels.size() == 1) {
		auto combinedObject3D = new Object3D(id, combinedModels[0]);
		combinedObject3D->setParentEntity(this);
		combinedObject3D->setShader(shaderId);
		combinedObject3D->setDistanceShader(distanceShaderId);
		combinedObject3D->setContributesShadows(contributesShadows);
		combinedObject3D->setReceivesShadows(receivesShadows);
		combinedObject3D->setEngine(engine);
		combinedObject3D->setEnableEarlyZRejection(enableEarlyZRejection);
		combinedObject3D->shaderParameters = shaderParameters;
		combinedObject3D->distanceShaderParameters = distanceShaderParameters;
		combinedObject3D->update();
		combinedEntity = combinedObject3D;
	} else
	if (combinedModels.size() > 1) {
		// create object, initialize and
		auto combinedLODObject3D = new LODObject3D(
			id,
			combinedModels[0],
			combinedModels[1] == nullptr?LODObject3D::LODLEVELTYPE_NONE:LODObject3D::LODLEVELTYPE_MODEL,
			modelLOD2MinDistance,
			combinedModels[1],
			combinedModels[2] == nullptr?LODObject3D::LODLEVELTYPE_NONE:LODObject3D::LODLEVELTYPE_MODEL,
			modelLOD3MinDistance,
			combinedModels[2]
		);
		combinedLODObject3D->setParentEntity(this);
		combinedLODObject3D->setShader(shaderId);
		combinedLODObject3D->setDistanceShader(distanceShaderId);
		combinedLODObject3D->setDistanceShaderDistance(distanceShaderDistance);
		combinedLODObject3D->setContributesShadows(contributesShadows);
		combinedLODObject3D->setReceivesShadows(receivesShadows);
		combinedLODObject3D->setEngine(engine);
		combinedLODObject3D->setEnableEarlyZRejection(enableEarlyZRejection);
		combinedLODObject3D->shaderParameters = shaderParameters;
		combinedLODObject3D->distanceShaderParameters = distanceShaderParameters;
		if (combinedLODObject3D->objectLOD1 != nullptr) {
			combinedLODObject3D->objectLOD1->shaderParameters = shaderParameters;
			combinedLODObject3D->objectLOD1->distanceShaderParameters = distanceShaderParameters;
		}
		if (combinedLODObject3D->objectLOD2 != nullptr) {
			combinedLODObject3D->objectLOD2->shaderParameters = shaderParameters;
			combinedLODObject3D->objectLOD2->distanceShaderParameters = distanceShaderParameters;
		}
		if (combinedLODObject3D->objectLOD3 != nullptr) {
			combinedLODObject3D->objectLOD3->shaderParameters = shaderParameters;
			combinedLODObject3D->objectLOD3->distanceShaderParameters = distanceShaderParameters;
		}
		combinedLODObject3D->update();
		combinedEntity = combinedLODObject3D;
	}

	//
	updateBoundingBox();
	update();
}

void Object3DRenderGroup::addObject(Model* model, const Transformations& transformations) {
	transformationsByModel[model].push_back(transformations);
}

void Object3DRenderGroup::setEngine(Engine* engine)
{
	if (this->engine != nullptr) this->engine->deregisterEntity(this);
	this->engine = engine;
	if (engine != nullptr) engine->registerEntity(this);
	if (combinedEntity != nullptr) combinedEntity->setEngine(engine);
}

void Object3DRenderGroup::setRenderer(Renderer* renderer)
{
}

void Object3DRenderGroup::fromTransformations(const Transformations& transformations)
{
	Transformations::fromTransformations(transformations);
	// update bounding box transformed
	boundingBoxTransformed.fromBoundingVolumeWithTransformations(&boundingBox, *this);
	// update object
	if (parentEntity == nullptr && frustumCulling == true && engine != nullptr && enabled == true) engine->partition->updateEntity(this);
}

void Object3DRenderGroup::update()
{
	Transformations::update();
	// update bounding box transformed
	boundingBoxTransformed.fromBoundingVolumeWithTransformations(&boundingBox, *this);
	// update object
	if (parentEntity == nullptr && frustumCulling == true && engine != nullptr && enabled == true) engine->partition->updateEntity(this);
}

void Object3DRenderGroup::setEnabled(bool enabled)
{
	// return if enable state has not changed
	if (this->enabled == enabled) return;

	// frustum if root entity
	if (parentEntity == nullptr) {
		// frustum culling enabled?
		if (frustumCulling == true) {
			// yeo, add or remove from partition
			if (enabled == true) {
				if (engine != nullptr) engine->partition->addEntity(this);
			} else {
				if (engine != nullptr) engine->partition->removeEntity(this);
			}
		}
	}

	//
	this->enabled = enabled;
}

bool Object3DRenderGroup::isFrustumCulling() {
	return frustumCulling;
}

void Object3DRenderGroup::setFrustumCulling(bool frustumCulling) {
	// check if enabled and engine attached
	if (enabled == true && engine != nullptr) {
		// had frustum culling
		if (this->frustumCulling == true) {
			// yep, remove if set to false now
			if (frustumCulling == false) engine->partition->removeEntity(this);
		} else {
			// yep, add if set to true now
			if (frustumCulling == true) engine->partition->addEntity(this);
		}
	}
	this->frustumCulling = frustumCulling;
	// delegate change to engine
	if (engine != nullptr) engine->registerEntity(this);
}

void Object3DRenderGroup::dispose()
{
	// delegate to combined object
	if (combinedEntity != nullptr) {
		combinedEntity->dispose();
		delete combinedEntity;
		combinedEntity = nullptr;
	}
	// disose combined model
	for (auto& combinedModel: combinedModels) {
		if (combinedModel != nullptr) {
			delete combinedModel;
			combinedModel = nullptr;
		}
	}
}

void Object3DRenderGroup::initialize()
{
	if (combinedEntity != nullptr) combinedEntity->initialize();
}

