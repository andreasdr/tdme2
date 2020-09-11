#include <tdme/engine/Object3DRenderGroup.h>

#include <string>
#include <vector>

#include <tdme/engine/Engine.h>
#include <tdme/engine/LODObject3D.h>
#include <tdme/engine/Object3D.h>
#include <tdme/engine/Transformations.h>
#include <tdme/engine/Partition.h>
#include <tdme/engine/model/Face.h>
#include <tdme/engine/model/FacesEntity.h>
#include <tdme/engine/model/Group.h>
#include <tdme/engine/model/Material.h>
#include <tdme/engine/model/Model.h>
#include <tdme/utilities/ModelTools.h>
#include <tdme/engine/model/RotationOrder.h>
#include <tdme/engine/model/UpVector.h>
#include <tdme/engine/model/TextureCoordinate.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/math/Math.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Matrix4x4.h>

using std::string;
using std::to_string;
using std::vector;

using tdme::engine::Object3DRenderGroup;
using tdme::engine::Engine;
using tdme::engine::LODObject3D;
using tdme::engine::Object3D;
using tdme::engine::Partition;
using tdme::engine::model::Face;
using tdme::engine::model::FacesEntity;
using tdme::engine::model::Group;
using tdme::engine::model::Material;
using tdme::engine::model::Model;
using tdme::utilities::ModelTools;
using tdme::engine::model::RotationOrder;
using tdme::engine::model::UpVector;
using tdme::engine::model::TextureCoordinate;
using tdme::engine::Transformations;
using tdme::engine::primitives::BoundingBox;
using tdme::math::Math;
using tdme::math::Vector3;
using tdme::math::Matrix4x4;

Object3DRenderGroup::Object3DRenderGroup(
	const string& id,
	Model* model,
	int lodLevels,
	float modelLOD2MinDistance,
	float modelLOD3MinDistance,
	int modelLOD2ReduceBy,
	int modelLOD3ReduceBy
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
	setModel(model);
}

Object3DRenderGroup::~Object3DRenderGroup() {
	if (combinedEntity != nullptr) delete combinedEntity;
	for (auto combinedModel: combinedModels) {
		if (combinedModel != nullptr) delete combinedModel;
	}
}

void Object3DRenderGroup::setModel(Model* model) {
	// dispose old object and combined model
	if (combinedEntity != nullptr) {
		combinedEntity->dispose();
		delete combinedEntity;
		combinedEntity = nullptr;
	}
	for (auto& combinedModel: combinedModels) {
		if (combinedModel != nullptr) {
			delete combinedModel;
			combinedModel = nullptr;
		}
	}

	// set up new model
	this->model = model;
}

void Object3DRenderGroup::combineGroup(Group* sourceGroup, const vector<Vector3>& origins, const vector<Matrix4x4>& objectParentTransformationsMatrices, Model* combinedModel) {
	// create group in combined model
	auto combinedModelGroup = combinedModel->getGroupById(sourceGroup->getId());
	if (combinedModelGroup == nullptr) {
		combinedModelGroup = new Group(
			combinedModel,
			sourceGroup->getParentGroup() == nullptr?nullptr:combinedModel->getGroupById(sourceGroup->getParentGroup()->getId()),
			sourceGroup->getId(),
			sourceGroup->getName()
		);
		if (sourceGroup->getParentGroup() == nullptr) {
			combinedModel->getSubGroups()[combinedModelGroup->getId()] = combinedModelGroup;
		} else {
			combinedModelGroup->getParentGroup()->getSubGroups()[combinedModelGroup->getId()] = combinedModelGroup;
		}
		combinedModel->getGroups()[combinedModelGroup->getId()] = combinedModelGroup;
	}

	{
		auto sourceGroupVerticesSize = sourceGroup->getVertices().size();
		auto sourceGroupNormalsSize = sourceGroup->getNormals().size();
		auto sourceGroupTextureCoordinatesSize = sourceGroup->getTextureCoordinates().size();
		auto sourceGroupTangentsSize = sourceGroup->getTangents().size();
		auto sourceGroupBitangentsSize = sourceGroup->getBitangents().size();

		// vertices and such from new model
		auto combinedModelGroupVertices = combinedModelGroup->getVertices();
		auto combinedModelGroupNormals = combinedModelGroup->getNormals();
		auto combinedModelGroupTextureCoordinates = combinedModelGroup->getTextureCoordinates();
		auto combinedModelGroupTangents = combinedModelGroup->getTangents();
		auto combinedModelGroupBitangents = combinedModelGroup->getBitangents();
		auto combinedModelGroupFacesEntities = combinedModelGroup->getFacesEntities();
		auto combinedModelGroupOrigins = combinedModelGroup->getOrigins();

		// current indices
		auto combinedModelGroupVerticesIdxStart = combinedModelGroupVertices.size();
		auto combinedModelGroupNormalsIdxStart = combinedModelGroupNormals.size();
		auto combinedModelGroupTextureCoordinatesIdxStart = combinedModelGroupTextureCoordinates.size();
		auto combinedModelGroupTangentsIdxStart = combinedModelGroupTangents.size();
		auto combinedModelGroupBitangentsIdxStart = combinedModelGroupBitangents.size();

		// add vertices and such from source group to new group
		{
			auto i = 0;
			for (auto& objectParentTransformationsMatrix: objectParentTransformationsMatrices) {
				Matrix4x4 transformationsMatrix;
				transformationsMatrix.set(sourceGroup->getTransformationsMatrix());
				transformationsMatrix.multiply(objectParentTransformationsMatrix);

				//
				Vector3 tmpVector3;
				for (auto& vertex: sourceGroup->getVertices()) {
					combinedModelGroupOrigins.push_back(origins[i]);
					combinedModelGroupVertices.push_back(transformationsMatrix.multiply(vertex, tmpVector3));
				}
				for (auto& normal: sourceGroup->getNormals()) {
					combinedModelGroupNormals.push_back(transformationsMatrix.multiplyNoTranslation(normal, tmpVector3));
				}
				for (auto& textureCoordinate: sourceGroup->getTextureCoordinates()) {
					combinedModelGroupTextureCoordinates.push_back(textureCoordinate);
				}
				for (auto& tangent: sourceGroup->getTangents()) {
					combinedModelGroupTangents.push_back(transformationsMatrix.multiplyNoTranslation(tangent, tmpVector3));
				}
				for (auto& bitangent: sourceGroup->getBitangents()) {
					combinedModelGroupBitangents.push_back(transformationsMatrix.multiplyNoTranslation(bitangent, tmpVector3));
				}
				//
				i++;
			}
		}

		// add source group faces to new new faces entity
		for (auto& facesEntity: sourceGroup->getFacesEntities()) {
			bool haveTextureCoordinates = facesEntity.isTextureCoordinatesAvailable();
			bool haveTangentsBitangents = facesEntity.isTangentBitangentAvailable();

			// get faces entity
			FacesEntity* combinedModelGroupFacesEntity = nullptr;
			for (auto& combinedModelGroupFacesEntityExisting: combinedModelGroupFacesEntities) {
				if (combinedModelGroupFacesEntityExisting.getId() == facesEntity.getId()) {
					combinedModelGroupFacesEntity = &combinedModelGroupFacesEntityExisting;
					break;
				}
			}
			// create
			if (combinedModelGroupFacesEntity == nullptr) {
				auto newFacesEntity = FacesEntity(
					combinedModelGroup,
					facesEntity.getId()
				);
				combinedModelGroupFacesEntities.push_back(newFacesEntity);
				combinedModelGroupFacesEntity = &combinedModelGroupFacesEntities[combinedModelGroupFacesEntities.size() - 1];
				auto combinedModelGroupFacesEntityMaterial = combinedModel->getMaterials()[facesEntity.getMaterial()->getId()];
				if (combinedModelGroupFacesEntityMaterial == nullptr) {
					combinedModelGroupFacesEntityMaterial = ModelTools::cloneMaterial(facesEntity.getMaterial());
					combinedModel->getMaterials()[combinedModelGroupFacesEntityMaterial->getId()] = combinedModelGroupFacesEntityMaterial;
				}
				combinedModelGroupFacesEntity->setMaterial(combinedModelGroupFacesEntityMaterial);
			}

			//
			auto combinedModelGroupFaces = combinedModelGroupFacesEntity->getFaces();

			//
			auto combinedModelGroupVerticesIdx = combinedModelGroupVerticesIdxStart;
			auto combinedModelGroupNormalsIdx = combinedModelGroupNormalsIdxStart;
			auto combinedModelGroupTextureCoordinatesIdx = combinedModelGroupTextureCoordinatesIdxStart;
			auto combinedModelGroupTangentsIdx = combinedModelGroupTangentsIdxStart;
			auto combinedModelGroupBitangentsIdx = combinedModelGroupBitangentsIdxStart;
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
					auto combinedModelGroupFace =
						Face(
							combinedModelGroup,
							combinedModelGroupVerticesIdx + faceVertexIndices[0],
							combinedModelGroupVerticesIdx + faceVertexIndices[1],
							combinedModelGroupVerticesIdx + faceVertexIndices[2],
							combinedModelGroupNormalsIdx + faceNormalIndices[0],
							combinedModelGroupNormalsIdx + faceNormalIndices[1],
							combinedModelGroupNormalsIdx + faceNormalIndices[2]
						);
					if (haveTextureCoordinates == true) {
						combinedModelGroupFace.setTextureCoordinateIndices(
							combinedModelGroupTextureCoordinatesIdx + faceTextureCoordinatesIndices[0],
							combinedModelGroupTextureCoordinatesIdx + faceTextureCoordinatesIndices[1],
							combinedModelGroupTextureCoordinatesIdx + faceTextureCoordinatesIndices[2]
						);
					}
					if (haveTangentsBitangents == true) {
						combinedModelGroupFace.setTangentIndices(
							combinedModelGroupTangentsIdx + faceTangentIndices[0],
							combinedModelGroupTangentsIdx + faceTangentIndices[1],
							combinedModelGroupTangentsIdx + faceTangentIndices[2]
						);
						combinedModelGroupFace.setBitangentIndices(
							combinedModelGroupBitangentsIdx + faceBitangentIndices[0],
							combinedModelGroupBitangentsIdx + faceBitangentIndices[1],
							combinedModelGroupBitangentsIdx + faceBitangentIndices[2]
						);
					}
					combinedModelGroupFaces.push_back(combinedModelGroupFace);
				}

				//
				combinedModelGroupVerticesIdx+= sourceGroupVerticesSize;
				combinedModelGroupNormalsIdx+= sourceGroupNormalsSize;
				combinedModelGroupTextureCoordinatesIdx+= sourceGroupTextureCoordinatesSize;
				combinedModelGroupTangentsIdx+= sourceGroupTangentsSize;
				combinedModelGroupBitangentsIdx+= sourceGroupBitangentsSize;
			}
			combinedModelGroupFacesEntity->setFaces(combinedModelGroupFaces);
		}

		// store back to model
		combinedModelGroup->setVertices(combinedModelGroupVertices);
		combinedModelGroup->setNormals(combinedModelGroupNormals);
		combinedModelGroup->setTextureCoordinates(combinedModelGroupTextureCoordinates);
		combinedModelGroup->setTangents(combinedModelGroupTangents);
		combinedModelGroup->setBitangents(combinedModelGroupBitangents);
		combinedModelGroup->setFacesEntities(combinedModelGroupFacesEntities);
		combinedModelGroup->setOrigins(combinedModelGroupOrigins);
	}

	// do child groups
	for (auto groupIt: sourceGroup->getSubGroups()) {
		combineGroup(groupIt.second, origins, objectParentTransformationsMatrices, combinedModel);
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
	for (auto groupIt: model->getSubGroups()) {
		combineGroup(groupIt.second, origins, objectTransformationMatrices, combinedModel);
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

	if (combinedModels.size() == 1) {
		auto combinedObject3D = new Object3D(id, combinedModels[0]);
		combinedObject3D->setParentEntity(this);
		combinedObject3D->setShader(shaderId);
		combinedObject3D->setDistanceShader(distanceShaderId);
		combinedObject3D->setContributesShadows(contributesShadows);
		combinedObject3D->setReceivesShadows(receivesShadows);
		combinedObject3D->setEngine(engine);
		combinedObject3D->setEnableEarlyZRejection(enableEarlyZRejection);
		combinedObject3D->setShaderParameters(shaderParameters);
		combinedObject3D->setDistanceShaderParameters(distanceShaderParameters);
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
		combinedLODObject3D->setShaderParameters(shaderParameters);
		combinedLODObject3D->setDistanceShaderParameters(distanceShaderParameters);
		combinedLODObject3D->update();
		combinedEntity = combinedLODObject3D;
	}

	//
	updateBoundingBox();
}

void Object3DRenderGroup::addObject(const Transformations& transformations) {
	objectsTransformations.push_back(transformations);
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

