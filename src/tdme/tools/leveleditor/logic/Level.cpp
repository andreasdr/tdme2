#include <tdme/tools/leveleditor/logic/Level.h>

#if defined(_WIN32) && defined(_MSC_VER)
	#pragma warning(disable:4503)
#endif

#include <vector>
#include <map>
#include <string>

#include <tdme/tdme.h>
#include <tdme/audio/Audio.h>
#include <tdme/audio/Sound.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Entity.h>
#include <tdme/engine/EntityHierarchy.h>
#include <tdme/engine/FogParticleSystem.h>
#include <tdme/engine/Light.h>
#include <tdme/engine/LODObject3D.h>
#include <tdme/engine/Object3D.h>
#include <tdme/engine/Object3DModel.h>
#include <tdme/engine/Object3DRenderGroup.h>
#include <tdme/engine/ObjectParticleSystem.h>
#include <tdme/engine/ParticleSystemEntity.h>
#include <tdme/engine/ParticleSystemGroup.h>
#include <tdme/engine/PointsParticleSystem.h>
#include <tdme/engine/Transformations.h>
#include <tdme/engine/fileio/models/ModelReader.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Color4Base.h>
#include <tdme/engine/model/Model.h>
#include <tdme/utilities/ModelTools.h>
#include <tdme/engine/physics/Body.h>
#include <tdme/engine/physics/World.h>
#include <tdme/engine/primitives/ConvexMesh.h>
#include <tdme/engine/primitives/OrientedBoundingBox.h>
#include <tdme/engine/primitives/PrimitiveModel.h>
#include <tdme/engine/primitives/Sphere.h>
#include <tdme/engine/primitives/TerrainMesh.h>
#include <tdme/engine/subsystems/particlesystem/BoundingBoxParticleEmitter.h>
#include <tdme/engine/subsystems/particlesystem/CircleParticleEmitter.h>
#include <tdme/engine/subsystems/particlesystem/CircleParticleEmitterPlaneVelocity.h>
#include <tdme/engine/subsystems/particlesystem/PointParticleEmitter.h>
#include <tdme/engine/subsystems/particlesystem/SphereParticleEmitter.h>
#include <tdme/math/Math.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Vector4.h>
#include <tdme/tools/shared/files/ModelMetaDataFileImport.h>
#include <tdme/tools/shared/files/ProgressCallback.h>
#include <tdme/tools/shared/model/LevelEditorEntity.h>
#include <tdme/tools/shared/model/LevelEditorEntity_EntityType.h>
#include <tdme/tools/shared/model/LevelEditorEntityAudio.h>
#include <tdme/tools/shared/model/LevelEditorEntityBoundingVolume.h>
#include <tdme/tools/shared/model/LevelEditorEntityLODLevel.h>
#include <tdme/tools/shared/model/LevelEditorEntityModel.h>
#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem_BoundingBoxParticleEmitter.h>
#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem_CircleParticleEmitter.h>
#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem_CircleParticleEmitterPlaneVelocity.h>
#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem_Emitter.h>
#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem_FogParticleSystem.h>
#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem_ObjectParticleSystem.h>
#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem_PointParticleEmitter.h>
#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem_PointParticleSystem.h>
#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem_SphereParticleEmitter.h>
#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem_Type.h>
#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem.h>
#include <tdme/tools/shared/model/LevelEditorEntityPhysics.h>
#include <tdme/tools/shared/model/LevelEditorEntityPhysics_BodyType.h>
#include <tdme/tools/shared/model/LevelEditorLevel.h>
#include <tdme/tools/shared/model/LevelEditorLight.h>
#include <tdme/tools/shared/model/LevelEditorObject.h>
#include <tdme/tools/shared/model/ModelProperties.h>
#include <tdme/tools/shared/model/PropertyModelClass.h>
#include <tdme/tools/shared/tools/Tools.h>
#include <tdme/utilities/MutableString.h>
#include <tdme/utilities/StringTools.h>
#include <tdme/utilities/Console.h>

using std::map;
using std::vector;
using std::string;
using std::to_string;

using tdme::tools::leveleditor::logic::Level;

using tdme::audio::Audio;
using tdme::audio::Sound;
using tdme::engine::Engine;
using tdme::engine::Entity;
using tdme::engine::EntityHierarchy;
using tdme::engine::FogParticleSystem;
using tdme::engine::Light;
using tdme::engine::LODObject3D;
using tdme::engine::Object3D;
using tdme::engine::Object3DModel;
using tdme::engine::Object3DRenderGroup;
using tdme::engine::ObjectParticleSystem;
using tdme::engine::ParticleSystemEntity;
using tdme::engine::ParticleSystemGroup;
using tdme::engine::PointsParticleSystem;
using tdme::engine::Transformations;
using tdme::engine::fileio::models::ModelReader;
using tdme::engine::model::Color4;
using tdme::engine::model::Color4Base;
using tdme::engine::model::Model;
using tdme::utilities::ModelTools;
using tdme::engine::physics::Body;
using tdme::engine::physics::World;
using tdme::engine::primitives::ConvexMesh;
using tdme::engine::primitives::OrientedBoundingBox;
using tdme::engine::primitives::PrimitiveModel;
using tdme::engine::primitives::Sphere;
using tdme::engine::primitives::TerrainMesh;
using tdme::engine::subsystems::particlesystem::BoundingBoxParticleEmitter;
using tdme::engine::subsystems::particlesystem::CircleParticleEmitter;
using tdme::engine::subsystems::particlesystem::CircleParticleEmitterPlaneVelocity;
using tdme::engine::subsystems::particlesystem::PointParticleEmitter;
using tdme::engine::subsystems::particlesystem::SphereParticleEmitter;
using tdme::math::Math;
using tdme::math::Vector3;
using tdme::math::Vector4;
using tdme::tools::shared::files::ModelMetaDataFileImport;
using tdme::tools::shared::files::ProgressCallback;
using tdme::tools::shared::model::LevelEditorEntity;
using tdme::tools::shared::model::LevelEditorEntity_EntityType;
using tdme::tools::shared::model::LevelEditorEntityAudio;
using tdme::tools::shared::model::LevelEditorEntityBoundingVolume;
using tdme::tools::shared::model::LevelEditorEntityModel;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem_BoundingBoxParticleEmitter;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem_CircleParticleEmitter;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem_CircleParticleEmitterPlaneVelocity;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem_Emitter;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem_FogParticleSystem;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem_ObjectParticleSystem;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem_PointParticleEmitter;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem_PointParticleSystem;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem_SphereParticleEmitter;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem_Type;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem;
using tdme::tools::shared::model::LevelEditorEntityPhysics;
using tdme::tools::shared::model::LevelEditorEntityPhysics_BodyType;
using tdme::tools::shared::model::LevelEditorLevel;
using tdme::tools::shared::model::LevelEditorLight;
using tdme::tools::shared::model::LevelEditorObject;
using tdme::tools::shared::model::ModelProperties;
using tdme::tools::shared::model::PropertyModelClass;
using tdme::tools::shared::tools::Tools;
using tdme::utilities::MutableString;
using tdme::utilities::StringTools;
using tdme::utilities::Console;

Model* Level::emptyModel = nullptr;
float Level::renderGroupsPartitionWidth = 64.0f;
float Level::renderGroupsPartitionHeight = 64.0f;
float Level::renderGroupsPartitionDepth = 64.0f;
int Level::renderGroupsReduceBy = 1;
int Level::renderGroupsLODLevels = 3;
float Level::renderGroupsLOD2MinDistance = 25.0;
float Level::renderGroupsLOD3MinDistance = 50.0;
int Level::renderGroupsLOD2ReduceBy = 4;
int Level::renderGroupsLOD3ReduceBy = 16;
bool Level::enableEarlyZRejection = false;

void Level::setLight(Engine* engine, LevelEditorLevel* level, const Vector3& translation)
{
	for (auto i = 0; i < Engine::LIGHTS_MAX && i < level->getLightCount(); i++) {
		engine->getLightAt(i)->setAmbient(Color4(level->getLightAt(i)->getAmbient()));
		engine->getLightAt(i)->setDiffuse(Color4(level->getLightAt(i)->getDiffuse()));
		engine->getLightAt(i)->setSpecular(Color4(level->getLightAt(i)->getSpecular()));
		engine->getLightAt(i)->setSpotDirection(level->getLightAt(i)->getSpotDirection());
		engine->getLightAt(i)->setSpotExponent(level->getLightAt(i)->getSpotExponent());
		engine->getLightAt(i)->setSpotCutOff(level->getLightAt(i)->getSpotCutOff());
		engine->getLightAt(i)->setConstantAttenuation(level->getLightAt(i)->getConstantAttenuation());
		engine->getLightAt(i)->setLinearAttenuation(level->getLightAt(i)->getLinearAttenuation());
		engine->getLightAt(i)->setQuadraticAttenuation(level->getLightAt(i)->getQuadraticAttenuation());
		engine->getLightAt(i)->setEnabled(level->getLightAt(i)->isEnabled());
		engine->getLightAt(i)->setPosition(
			Vector4(
				level->getLightAt(i)->getPosition().getX() + translation.getX(),
				level->getLightAt(i)->getPosition().getY() + translation.getY(),
				level->getLightAt(i)->getPosition().getZ() + translation.getZ(),
				1.0f
			)
		);
	}
}

Entity* Level::createParticleSystem(LevelEditorEntityParticleSystem* particleSystem, const string& id, bool enableDynamicShadows)
{
	ParticleEmitter* engineEmitter = nullptr;
	{
		auto v = particleSystem->getEmitter();
		if (v == LevelEditorEntityParticleSystem_Emitter::NONE) {
			return nullptr;
		} else
		if (v == LevelEditorEntityParticleSystem_Emitter::POINT_PARTICLE_EMITTER) {
			auto emitter = particleSystem->getPointParticleEmitter();
			engineEmitter = new PointParticleEmitter(emitter->getCount(), emitter->getLifeTime(), emitter->getLifeTimeRnd(), emitter->getMass(), emitter->getMassRnd(), emitter->getPosition(), emitter->getVelocity(), emitter->getVelocityRnd(), emitter->getColorStart(), emitter->getColorEnd());
		} else
		if (v == LevelEditorEntityParticleSystem_Emitter::BOUNDINGBOX_PARTICLE_EMITTER) {
			auto emitter = particleSystem->getBoundingBoxParticleEmitters();
			engineEmitter = new BoundingBoxParticleEmitter(emitter->getCount(), emitter->getLifeTime(), emitter->getLifeTimeRnd(), emitter->getMass(), emitter->getMassRnd(), new OrientedBoundingBox(emitter->getObbCenter(), emitter->getObbAxis0(), emitter->getObbAxis1(), emitter->getObbAxis2(), emitter->getObbHalfextension()), emitter->getVelocity(), emitter->getVelocityRnd(), emitter->getColorStart(), emitter->getColorEnd());
		} else
		if (v == LevelEditorEntityParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER) {
			auto emitter = particleSystem->getCircleParticleEmitter();
			engineEmitter = new CircleParticleEmitter(emitter->getCount(), emitter->getLifeTime(), emitter->getLifeTimeRnd(), emitter->getAxis0(), emitter->getAxis1(), emitter->getCenter(), emitter->getRadius(), emitter->getMass(), emitter->getMassRnd(), emitter->getVelocity(), emitter->getVelocityRnd(), emitter->getColorStart(), emitter->getColorEnd());
		} else
		if (v == LevelEditorEntityParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER_PLANE_VELOCITY) {
			auto emitter = particleSystem->getCircleParticleEmitterPlaneVelocity();
			engineEmitter = new CircleParticleEmitterPlaneVelocity(emitter->getCount(), emitter->getLifeTime(), emitter->getLifeTimeRnd(), emitter->getAxis0(), emitter->getAxis1(), emitter->getCenter(), emitter->getRadius(), emitter->getMass(), emitter->getMassRnd(), emitter->getVelocity(), emitter->getVelocityRnd(), emitter->getColorStart(), emitter->getColorEnd());
		} else
		if (v == LevelEditorEntityParticleSystem_Emitter::SPHERE_PARTICLE_EMITTER) {
			auto emitter = particleSystem->getSphereParticleEmitter();
			engineEmitter = new SphereParticleEmitter(emitter->getCount(), emitter->getLifeTime(), emitter->getLifeTimeRnd(), emitter->getMass(), emitter->getMassRnd(), new Sphere(emitter->getCenter(), emitter->getRadius()), emitter->getVelocity(), emitter->getVelocityRnd(), emitter->getColorStart(), emitter->getColorEnd());
		} else {
			Console::println(
				string(
					"Level::createParticleSystem(): unknown particle system emitter '" +
					particleSystem->getEmitter()->getName() +
					"'"
				)
			);
			return nullptr;
		}
	}

	{
		{
			auto v = particleSystem->getType();
			if (v == LevelEditorEntityParticleSystem_Type::NONE) {
				return nullptr;
			} else
			if (v == LevelEditorEntityParticleSystem_Type::OBJECT_PARTICLE_SYSTEM) {
				auto objectParticleSystem = particleSystem->getObjectParticleSystem();
				if (objectParticleSystem->getModel() == nullptr) return nullptr;

				return new ObjectParticleSystem(
					id,
					objectParticleSystem->getModel(),
					objectParticleSystem->getScale(),
					objectParticleSystem->isAutoEmit(),
					enableDynamicShadows,
					enableDynamicShadows,
					objectParticleSystem->getMaxCount(),
					engineEmitter
				);
			} else
			if (v == LevelEditorEntityParticleSystem_Type::POINT_PARTICLE_SYSTEM) {
				auto pointParticleSystem = particleSystem->getPointParticleSystem();
				return new PointsParticleSystem(
					id,
					engineEmitter,
					pointParticleSystem->getMaxPoints(),
					pointParticleSystem->getPointSize(),
					pointParticleSystem->isAutoEmit(),
					pointParticleSystem->getTexture(),
					pointParticleSystem->getTextureHorizontalSprites(),
					pointParticleSystem->getTextureVerticalSprites(),
					pointParticleSystem->getTextureSpritesFPS()
				);
			} else
			if (v == LevelEditorEntityParticleSystem_Type::FOG_PARTICLE_SYSTEM) {
				auto fogParticleSystem = particleSystem->getFogParticleSystem();
				return new FogParticleSystem(
					id,
					engineEmitter,
					fogParticleSystem->getMaxPoints(),
					fogParticleSystem->getPointSize(),
					fogParticleSystem->getTexture(),
					fogParticleSystem->getTextureHorizontalSprites(),
					fogParticleSystem->getTextureVerticalSprites(),
					fogParticleSystem->getTextureSpritesFPS()
				);
			} else {
				Console::println(
					string(
						"Level::createParticleSystem(): unknown particle system type '" +
						particleSystem->getType()->getName() +
						"'"
					)
				);
				return nullptr;
			}
		}
	}

}

Entity* Level::createEmpty(const string& id, const Transformations& transformations) {
	if (emptyModel == nullptr) {
		emptyModel = ModelReader::read("resources/engine/tools/leveleditor/models", "empty.dae");
	}
	auto entity = new Object3D(
		id,
		Level::emptyModel
	);
	entity->fromTransformations(transformations);
	return entity;
}

Entity* Level::createEntity(LevelEditorEntity* levelEditorEntity, const string& id, const Transformations& transformations, int instances) {
	Entity* entity = nullptr;

	// objects
	if (levelEditorEntity->getModel() != nullptr) {
		auto lodLevel2 = levelEditorEntity->getLODLevel2();
		auto lodLevel3 = levelEditorEntity->getLODLevel3();
		// with LOD
		if (lodLevel2 != nullptr) {
			entity = new LODObject3D(
				id,
				levelEditorEntity->getModel(),
				lodLevel2->getType(),
				lodLevel2->getMinDistance(),
				lodLevel2->getModel(),
				lodLevel3 != nullptr?lodLevel3->getType():LODObject3D::LODLEVELTYPE_NONE,
				lodLevel3 != nullptr?lodLevel3->getMinDistance():0.0f,
				lodLevel3 != nullptr?lodLevel3->getModel():nullptr
			);
			auto lodObject = dynamic_cast<LODObject3D*>(entity);
			lodObject->setEffectColorAddLOD2(lodLevel2->getColorAdd());
			lodObject->setEffectColorMulLOD2(lodLevel2->getColorMul());
			if (lodLevel3 != nullptr) {
				lodObject->setEffectColorAddLOD3(lodLevel3->getColorAdd());
				lodObject->setEffectColorMulLOD3(lodLevel3->getColorMul());
			}
			lodObject->setShader(levelEditorEntity->getShader());
			lodObject->setDistanceShader(levelEditorEntity->getDistanceShader());
			lodObject->setDistanceShaderDistance(levelEditorEntity->getDistanceShaderDistance());
		} else {
			// single
			entity = new Object3D(
				id,
				levelEditorEntity->getModel(),
				instances
			);
			auto object = dynamic_cast<Object3D*>(entity);
			object->setShader(levelEditorEntity->getShader());
			object->setDistanceShader(levelEditorEntity->getDistanceShader());
			object->setDistanceShaderDistance(levelEditorEntity->getDistanceShaderDistance());
			if (enableEarlyZRejection == true && levelEditorEntity->getModelSettings()->isTerrainMesh() == true) {
				object->setEnableEarlyZRejection(true);
			}
		}
	} else
	// particle system
	if (levelEditorEntity->getType() == LevelEditorEntity_EntityType::PARTICLESYSTEM) {
		vector<ParticleSystemEntity*> particleSystems;
		for (auto i = 0; i < levelEditorEntity->getParticleSystemsCount(); i++) {
			auto particleSystem = createParticleSystem(
				levelEditorEntity->getParticleSystemAt(i),
				id + (i == 0?"":"." + to_string(i)),
				true
			);
			if (particleSystem != nullptr) particleSystems.push_back(dynamic_cast<ParticleSystemEntity*>(particleSystem));
		}
		if (particleSystems.size() == 1) {
			entity = dynamic_cast<Entity*>(particleSystems[0]);
		} else
		if (particleSystems.size() > 1) {
			entity = new ParticleSystemGroup(
				id,
				true,
				true,
				true,
				particleSystems
			);
		}
	} else
	// particle system
	if (levelEditorEntity->getType() == LevelEditorEntity_EntityType::TRIGGER) {
		// bounding volumes
		auto entityBoundingVolumesHierarchy = new EntityHierarchy(id);
		for (auto i = 0; i < levelEditorEntity->getBoundingVolumeCount(); i++) {
			auto entityBoundingVolume = levelEditorEntity->getBoundingVolumeAt(i);
			if (entityBoundingVolume->getModel() != nullptr) {
				auto bvObject = new Object3D(LevelEditorEntity::MODEL_BOUNDINGVOLUME_IDS[i], entityBoundingVolume->getModel());
				entityBoundingVolumesHierarchy->addEntity(bvObject);
			}
		}
		entityBoundingVolumesHierarchy->update();
		if (entityBoundingVolumesHierarchy->getEntities().size() == 0) {
			entityBoundingVolumesHierarchy->dispose();
			delete entityBoundingVolumesHierarchy;
		} else {
			entity = entityBoundingVolumesHierarchy;
		}
	}

	//
	if (entity != nullptr) {
		entity->setContributesShadows(levelEditorEntity->isContributesShadows());
		entity->setReceivesShadows(levelEditorEntity->isReceivesShadows());
		entity->fromTransformations(transformations);
	}

	// done
	return entity;
}

Entity* Level::createEntity(LevelEditorObject* levelEditorObject, const Vector3& translation) {
	Transformations transformations;
	transformations.fromTransformations(levelEditorObject->getTransformations());
	if (translation.equals(Vector3()) == false) {
		transformations.setTranslation(transformations.getTranslation().clone().add(translation));
		transformations.update();
	}
	return createEntity(levelEditorObject->getEntity(), levelEditorObject->getId(), transformations);
}

void Level::addLevel(Engine* engine, LevelEditorLevel* level, bool addEmpties, bool addTrigger, bool pickable, bool enable, const Vector3& translation, ProgressCallback* progressCallback)
{
	if (progressCallback != nullptr) progressCallback->progress(0.0f);
	map<string, map<string, vector<Transformations*>>> renderGroupEntitiesByModelAndPartition;
	map<string, LevelEditorEntity*> renderGroupLevelEditorEntities;
	auto progressStepCurrent = 0;
	for (auto i = 0; i < level->getObjectCount(); i++) {
		auto object = level->getObjectAt(i);

		if (progressCallback != nullptr && progressStepCurrent % 1000 == 0) progressCallback->progress(0.0f + static_cast<float>(progressStepCurrent) / static_cast<float>(level->getObjectCount()) * 0.5f);
		progressStepCurrent++;

		if (addEmpties == false && object->getEntity()->getType() == LevelEditorEntity_EntityType::EMPTY) continue;
		if (addTrigger == false && object->getEntity()->getType() == LevelEditorEntity_EntityType::TRIGGER) continue;

		if (object->getEntity()->isRenderGroups() == true) {
			auto minX = object->getTransformations().getTranslation().getX();
			auto minY = object->getTransformations().getTranslation().getY();
			auto minZ = object->getTransformations().getTranslation().getZ();
			auto partitionX = (int)(minX / renderGroupsPartitionWidth);
			auto partitionY = (int)(minY / renderGroupsPartitionHeight);
			auto partitionZ = (int)(minZ / renderGroupsPartitionDepth);
			renderGroupLevelEditorEntities[object->getEntity()->getModel()->getId()] = object->getEntity();
			renderGroupEntitiesByModelAndPartition[object->getEntity()->getModel()->getId()][to_string(partitionX) + "," + to_string(partitionY) + "," + to_string(partitionZ)].push_back(&object->getTransformations());
		} else {
			Entity* entity = createEntity(object);
			if (entity == nullptr) continue;

			entity->setTranslation(entity->getTranslation().clone().add(translation));
			entity->setPickable(pickable);
			entity->setContributesShadows(object->getEntity()->isContributesShadows());
			entity->setReceivesShadows(object->getEntity()->isReceivesShadows());
			if (object->getEntity()->getType() == LevelEditorEntity_EntityType::EMPTY) {
				entity->setScale(Vector3(Math::sign(entity->getScale().getX()), Math::sign(entity->getScale().getY()), Math::sign(entity->getScale().getZ())));
			}
			entity->update();
			entity->setEnabled(enable);
			engine->addEntity(entity);
		}
	}

	// do render groups
	progressStepCurrent = 0;
	for (auto itModel: renderGroupEntitiesByModelAndPartition) {
		if (progressCallback != nullptr && progressStepCurrent % 1000 == 0) progressCallback->progress(0.0f + static_cast<float>(progressStepCurrent) / static_cast<float>(renderGroupEntitiesByModelAndPartition.size()) * 1.0f);
		progressStepCurrent++;

		for (auto itPartition: itModel.second) {
			auto levelEditorEntity = renderGroupLevelEditorEntities[itModel.first];
			auto object3DRenderGroup = new Object3DRenderGroup(
				"tdme.rendergroup." + itModel.first + "." + itPartition.first,
				levelEditorEntity->getModel(),
				renderGroupsLODLevels,
				renderGroupsLOD2MinDistance,
				renderGroupsLOD3MinDistance,
				renderGroupsLOD2ReduceBy,
				renderGroupsLOD3ReduceBy
			);
			object3DRenderGroup->setShader(levelEditorEntity->getShader());
			object3DRenderGroup->setDistanceShader(levelEditorEntity->getDistanceShader());
			object3DRenderGroup->setDistanceShaderDistance(levelEditorEntity->getDistanceShaderDistance());
			auto objectIdx = -1;
			for (auto transformation: itPartition.second) {
				objectIdx++;
				if (objectIdx % renderGroupsReduceBy != 0) continue;
				object3DRenderGroup->addObject(*transformation);
			}
			object3DRenderGroup->updateRenderGroup();
			engine->addEntity(object3DRenderGroup);
		}
	}

	//
	if (progressCallback != nullptr) {
		progressCallback->progress(1.0f);
		delete progressCallback;
	}
}

Body* Level::createBody(World* world, LevelEditorEntity* levelEditorEntity, const string& id, const Transformations& transformations, uint16_t collisionTypeId, int index) {
	if (levelEditorEntity->getType() == LevelEditorEntity_EntityType::EMPTY) return nullptr;

	if (levelEditorEntity->getType() == LevelEditorEntity_EntityType::TRIGGER) {
		vector<BoundingVolume*> boundingVolumes;
		for (auto j = 0; j < levelEditorEntity->getBoundingVolumeCount(); j++) {
			auto entityBv = levelEditorEntity->getBoundingVolumeAt(j);
			if (index == -1 || index == j) boundingVolumes.push_back(entityBv->getBoundingVolume());
		}
		if (boundingVolumes.size() == 0) return nullptr;
		return world->addCollisionBody(
			id,
			true,
			collisionTypeId == 0?RIGIDBODY_TYPEID_TRIGGER:collisionTypeId,
			transformations,
			boundingVolumes
		);
	} else
	if (levelEditorEntity->getType() == LevelEditorEntity_EntityType::MODEL &&
		levelEditorEntity->getModelSettings()->isTerrainMesh() == true) {
		Object3DModel terrainModel(levelEditorEntity->getModel());
		auto terrainMesh = new TerrainMesh(&terrainModel, transformations);
		if (levelEditorEntity->getPhysics()->getType() == LevelEditorEntityPhysics_BodyType::COLLISION_BODY) {
			return world->addCollisionBody(
				id,
				true,
				collisionTypeId == 0?RIGIDBODY_TYPEID_COLLISION:collisionTypeId,
				Transformations(),
				{terrainMesh}
			);
		} else
		if (levelEditorEntity->getPhysics()->getType() == LevelEditorEntityPhysics_BodyType::STATIC_RIGIDBODY) {
			return world->addStaticRigidBody(
				id,
				true,
				collisionTypeId == 0?RIGIDBODY_TYPEID_STATIC:collisionTypeId,
				Transformations(),
				levelEditorEntity->getPhysics()->getFriction(),
				{terrainMesh}
			);
		} else
		if (levelEditorEntity->getPhysics()->getType() == LevelEditorEntityPhysics_BodyType::DYNAMIC_RIGIDBODY) {
			return world->addRigidBody(
				id,
				true,
				collisionTypeId == 0?RIGIDBODY_TYPEID_DYNAMIC:collisionTypeId,
				Transformations(),
				levelEditorEntity->getPhysics()->getRestitution(),
				levelEditorEntity->getPhysics()->getFriction(),
				levelEditorEntity->getPhysics()->getMass(),
				levelEditorEntity->getPhysics()->getInertiaTensor(),
				{terrainMesh}
			);
		}
	} else {
		vector<BoundingVolume*> boundingVolumes;
		for (auto j = 0; j < levelEditorEntity->getBoundingVolumeCount(); j++) {
			auto entityBv = levelEditorEntity->getBoundingVolumeAt(j);
			if (index == -1 || index == j) boundingVolumes.push_back(entityBv->getBoundingVolume());
		}
		if (boundingVolumes.size() == 0) return nullptr;
		if (levelEditorEntity->getPhysics()->getType() == LevelEditorEntityPhysics_BodyType::COLLISION_BODY) {
			return world->addCollisionBody(
				id,
				true,
				collisionTypeId == 0?RIGIDBODY_TYPEID_COLLISION:collisionTypeId,
				transformations,
				boundingVolumes
			);
		} else
		if (levelEditorEntity->getPhysics()->getType() == LevelEditorEntityPhysics_BodyType::STATIC_RIGIDBODY) {
			return world->addStaticRigidBody(
				id,
				true,
				collisionTypeId == 0?RIGIDBODY_TYPEID_STATIC:collisionTypeId,
				transformations,
				levelEditorEntity->getPhysics()->getFriction(),
				boundingVolumes
			);
		} else
		if (levelEditorEntity->getPhysics()->getType() == LevelEditorEntityPhysics_BodyType::DYNAMIC_RIGIDBODY) {
			return world->addRigidBody(
				id,
				true,
				collisionTypeId == 0?RIGIDBODY_TYPEID_DYNAMIC:collisionTypeId,
				transformations,
				levelEditorEntity->getPhysics()->getRestitution(),
				levelEditorEntity->getPhysics()->getFriction(),
				levelEditorEntity->getPhysics()->getMass(),
				levelEditorEntity->getPhysics()->getInertiaTensor(),
				boundingVolumes
			);
		}
	}
	return nullptr;
}

Body* Level::createBody(World* world, LevelEditorObject* levelEditorObject, const Vector3& translation, uint16_t collisionTypeId, int index) {
	Transformations transformations;
	transformations.fromTransformations(levelEditorObject->getTransformations());
	if (translation.equals(Vector3()) == false) {
		transformations.setTranslation(transformations.getTranslation().clone().add(translation));
		transformations.update();
	}
	return createBody(world, levelEditorObject->getEntity(), levelEditorObject->getId(), transformations, collisionTypeId, index);
}

void Level::addLevel(World* world, LevelEditorLevel* level, bool enable, const Vector3& translation, ProgressCallback* progressCallback)
{
	if (progressCallback != nullptr) progressCallback->progress(0.0f);
	auto progressStepCurrent = 0;

	//
	for (auto i = 0; i < level->getObjectCount(); i++) {
		auto levelEditorObject = level->getObjectAt(i);

		//
		if (progressCallback != nullptr && progressStepCurrent % 1000 == 0) progressCallback->progress(0.0f + static_cast<float>(progressStepCurrent) / static_cast<float>(level->getObjectCount()) * 1.0f);
		progressStepCurrent++;

		//
		auto rigidBody = createBody(world, levelEditorObject);
		if (rigidBody == nullptr) continue;
		if (translation.equals(Vector3()) == false) {
			auto transformations = levelEditorObject->getTransformations();
			transformations.setTranslation(transformations.getTranslation().clone().add(translation));
			transformations.update();
			rigidBody->fromTransformations(transformations);
		}
		rigidBody->setEnabled(enable);
	}

	//
	if (progressCallback != nullptr) {
		progressCallback->progress(1.0f);
		delete progressCallback;
	}
}

void Level::disableLevel(Engine* engine, LevelEditorLevel* level)
{
	for (auto i = 0; i < level->getObjectCount(); i++) {
		auto object = level->getObjectAt(i);
		auto entity = engine->getEntity(object->getId());
		if (entity == nullptr)
			continue;

		entity->setEnabled(false);
	}
}

void Level::disableLevel(World* world, LevelEditorLevel* level)
{
	Transformations transformations;
	for (auto i = 0; i < level->getObjectCount(); i++) {
		auto object = level->getObjectAt(i);
		auto rigidBody = world->getBody(object->getId());
		if (rigidBody == nullptr) continue;
		rigidBody->setEnabled(false);
	}
}

void Level::enableLevel(Engine* engine, LevelEditorLevel* level, const Vector3& translation)
{
	// TODO: a.drewke, Object3DRenderGroups
	for (auto i = 0; i < level->getObjectCount(); i++) {
		auto object = level->getObjectAt(i);
		auto entity = engine->getEntity(object->getId());
		if (entity == nullptr)
			continue;

		entity->fromTransformations(object->getTransformations());
		entity->setTranslation(entity->getTranslation().clone().add(translation));
		if (object->getEntity()->getType() == LevelEditorEntity_EntityType::EMPTY) {
			entity->setScale(Vector3(Math::sign(entity->getScale().getX()), Math::sign(entity->getScale().getY()), Math::sign(entity->getScale().getZ())));
		}
		entity->update();
		entity->setEnabled(true);
	}
}

void Level::enableLevel(World* world, LevelEditorLevel* level, const Vector3& translation)
{
	Transformations transformations;
	for (auto i = 0; i < level->getObjectCount(); i++) {
		auto object = level->getObjectAt(i);
		auto rigidBody = world->getBody(object->getId());
		if (rigidBody == nullptr) continue;
		transformations.fromTransformations(object->getTransformations());
		transformations.setTranslation(transformations.getTranslation().clone().add(translation));
		transformations.update();
		rigidBody->fromTransformations(transformations);
		rigidBody->setEnabled(true);
	}
}

void Level::addEntitySounds(Audio* audio, LevelEditorEntity* levelEditorEntity, const string& id, const int poolSize) {
	for (auto soundDefinition: levelEditorEntity->getSounds()) {
		if (soundDefinition->getFileName().length() > 0) {
			for (auto poolIdx = 0; poolIdx < poolSize; poolIdx++) {
				string pathName = ModelMetaDataFileImport::getResourcePathName(
					Tools::getPath(levelEditorEntity->getEntityFileName()),
					soundDefinition->getFileName()
				);
				string fileName = Tools::getFileName(soundDefinition->getFileName());
				auto sound = new Sound(
					id + "." + soundDefinition->getId() + (poolSize > 1?"." + to_string(poolIdx):""),
					pathName,
					fileName
				);
				sound->setGain(soundDefinition->getGain());
				sound->setPitch(soundDefinition->getPitch());
				sound->setLooping(soundDefinition->isLooping());
				sound->setFixed(soundDefinition->isFixed());
				audio->addEntity(sound);
			}
		}
	}
}

