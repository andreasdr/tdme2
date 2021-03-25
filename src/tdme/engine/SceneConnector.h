#pragma once

#include <tdme/tdme.h>
#include <tdme/audio/fwd-tdme.h>
#include <tdme/engine/fileio/fwd-tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/physics/fwd-tdme.h>
#include <tdme/engine/prototype/fwd-tdme.h>
#include <tdme/engine/scene/fwd-tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Vector3.h>
#include <tdme/utilities/fwd-tdme.h>

using tdme::audio::Audio;
using tdme::engine::fileio::ProgressCallback;
using tdme::engine::model::Model;
using tdme::engine::physics::Body;
using tdme::engine::physics::World;
using tdme::engine::prototype::Prototype;
using tdme::engine::prototype::PrototypeParticleSystem;
using tdme::engine::prototype::PrototypePhysics_BodyType;
using tdme::engine::scene::Scene;
using tdme::engine::scene::SceneEntity;
using tdme::engine::Engine;
using tdme::engine::Entity;
using tdme::engine::Transformations;
using tdme::math::Vector3;
using tdme::utilities::MutableString;

/**
 * Scene engine/physics connector
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::SceneConnector final
{

public:
	static constexpr int32_t RIGIDBODY_TYPEID_STATIC { 1 };
	static constexpr int32_t RIGIDBODY_TYPEID_DYNAMIC { 2 };
	static constexpr int32_t RIGIDBODY_TYPEID_COLLISION { 4 };
	static constexpr int32_t RIGIDBODY_TYPEID_TRIGGER { 8 };

	static float renderGroupsPartitionWidth;
	static float renderGroupsPartitionHeight;
	static float renderGroupsPartitionDepth;
	static int renderGroupsReduceBy;
	static int renderGroupsLODLevels;
	static float renderGroupsLOD2MinDistance;
	static float renderGroupsLOD3MinDistance;
	static int renderGroupsLOD2ReduceBy;
	static int renderGroupsLOD3ReduceBy;
	static bool enableEarlyZRejection;

public:

	/**
	 * @return render groups partition size / width
	 */
	inline static float getRenderGroupsPartitionWidth() {
		return renderGroupsPartitionWidth;
	}

	/**
	 * Set render groups partition size / width
	 * @param renderNodesPartitionDepth render groups partition size / width
	 */
	inline static void setRenderGroupsPartitionWidth(float renderNodesPartitionWidth) {
		SceneConnector::renderGroupsPartitionWidth = renderNodesPartitionWidth;
	}

	/**
	 * @return render groups partition size / height
	 */
	inline static float getRenderGroupsPartitionHeight() {
		return renderGroupsPartitionHeight;
	}

	/**
	 * Set render groups partition size / height
	 * @param renderNodesPartitionDepth render groups partition size / height
	 */
	inline static void setRenderGroupsPartitionHeight(float renderNodesPartitionHeight) {
		SceneConnector::renderGroupsPartitionHeight = renderNodesPartitionHeight;
	}

	/**
	 * @return render groups partition size / depth
	 */
	inline static float getRenderGroupsPartitionDepth() {
		return renderGroupsPartitionDepth;
	}

	/**
	 * Set render groups partition size / depth
	 * @param renderNodesPartitionDepth render groups partition size / depth
	 */
	inline static void setRenderGroupsPartitionDepth(float renderNodesPartitionDepth) {
		SceneConnector::renderGroupsPartitionDepth = renderNodesPartitionDepth;
	}

	/**
 	 * Set render groups reduce objects by a given factor
 	 * @param reduceBy render groups objects reduce by factor
 	 */
	inline static void setRenderGroupsReduceBy(int reduceBy) {
		SceneConnector::renderGroupsReduceBy = reduceBy;
	}

	/**
	 * @return render groups objects reduce by factor
	 */
	inline static int getRenderGroupsReduceBy() {
		return renderGroupsReduceBy;
	}

	/**
	 * @return render groups LOD levels
	 */
	inline static int getRenderGroupsLodLevels() {
		return renderGroupsLODLevels;
	}

	/**
	 * Set render groups LOD levels
	 * @param lodLevels render groups LOD levels
	 */
	inline static void setRenderGroupsLodLevels(int lodLevels) {
		renderGroupsLODLevels = lodLevels;
	}

	/**
	 * @return render groups LOD2 minumum distance
	 */
	inline static float getRenderGroupsLod2MinDistance() {
		return renderGroupsLOD2MinDistance;
	}

	/**
	 * Set render groups LOD2 minumum distance
	 * @param minDistance render groups LOD2 minumum distance
	 */
	inline static void setRenderGroupsLod2MinDistance(float minDistance) {
		renderGroupsLOD2MinDistance = minDistance;
	}

	/**
	 * @return render groups LOD3 minumum distance
	 */
	inline static float getRenderGroupsLod3MinDistance() {
		return renderGroupsLOD3MinDistance;
	}

	/**
	 * Set render groups LOD3 minumum distance
	 * @param minDistance render groups LOD3 minumum distance
	 */
	inline static void setRenderGroupsLod3MinDistance(float minDistance) {
		renderGroupsLOD3MinDistance = minDistance;
	}

	/**
	 * @return render groups LOD2 reduce by factor
	 */
	inline static int getRenderGroupsLod2ReduceBy() {
		return renderGroupsLOD2ReduceBy;
	}

	/**
	 * Set render groups LOD2 reduce by factor
	 * @param reduceBy render groups LOD2 reduce by factor
	 */
	inline static void setRenderGroupsLod2ReduceBy(int reduceBy) {
		renderGroupsLOD2ReduceBy = reduceBy;
	}

	/**
	 * @return render groups LOD3 reduce by factor
	 */
	inline static int getRenderGroupsLod3ReduceBy() {
		return renderGroupsLOD3ReduceBy;
	}

	/**
	 * Set render groups LOD3 reduce by factor
	 * @param reduceBy render groups LOD3 reduce by factor
	 */
	inline static void setRenderGroupsLod3ReduceBy(int reduceBy) {
		renderGroupsLOD3ReduceBy = reduceBy;
	}

	/**
	 * @return If early z rejection is enabled, in scene loading case its used for render groups and terrain
	 */
	inline static bool isEnableEarlyZRejection() {
		return enableEarlyZRejection;
	}

	/**
	 * Enable/disable early z rejection, in scene loading case its used for render groups and terrain
	 * @param enableEarlyZRejection enable early z rejection
	 */
	inline static void setEnableEarlyZRejection(bool enableEarlyZRejection) {
		SceneConnector::enableEarlyZRejection = enableEarlyZRejection;
	}

	/**
	 * Set lights from scene
	 * @param engine engine
	 * @param scene scene
	 * @param translation translation
	 */
	static void setLights(Engine* engine, Scene& scene, const Vector3& translation = Vector3(0.0f, 0.0f, 0.0f));

	/**
	 * Create particle system
	 * @param particleSystem prototype particle system
	 * @param id id
	 * @param enableDynamicShadows enable dynamic shadows
	 * @return engine particle system entity
	 */
	static Entity* createParticleSystem(PrototypeParticleSystem* particleSystem, const string& id, bool enableDynamicShadows = true);

	/**
	 * Create engine entity
	 * @param id id
	 * @param transformations transformations
	 * @return entity
	 */
	static Entity* createEmpty(const string& id, const Transformations& transformations);

	/**
	 * Create engine entity
	 * @param prototype prototype
	 * @param id id
	 * @param transformations transformations
	 * @param instances instances which applies only for skinned objects
	 * @param parentEntity parent entity
	 * @return entity
	 */
	static Entity* createEntity(Prototype* prototype, const string& id, const Transformations& transformations, int instances = 1, Entity* parentEntity = nullptr);

	/**
	 * Create engine entity
	 * @param sceneEntity scene object
	 * @param translation translation
	 * @param instances instances which applies only for skinned objects
	 * @param parentEntity parent entity
	 * @return entity
	 */
	static Entity* createEntity(SceneEntity* sceneEntity, const Vector3& translation = Vector3(0.0f, 0.0f, 0.0f), int instances = 1, Entity* parentEntity = nullptr);

	/**
	 * Add scene to engine
	 * @param engine engine
	 * @param scene scene
	 * @param addEmpties add empties
	 * @param addTrigger add trigger
	 * @param addEnvironmentMapping add environment mapping
	 * @param pickable pickable
	 * @param enable enable
	 * @param translation translation
	 * @param progressCallback progress callback
	 */
	static void addScene(Engine* engine, Scene& scene, bool addEmpties, bool addTrigger, bool addEnvironmentMapping, bool pickable, bool enable = true, const Vector3& translation = Vector3(0.0f, 0.0f, 0.0f), ProgressCallback* progressCallback = nullptr);

	/**
	 * Create rigid body
	 * @param world world
	 * @param prototype prototype
	 * @param id id
	 * @param transformations transformations
	 * @param collisionTypeId collision type id or 0 for default
	 * @param index use a optional index or all bounding volumes
	 * @param overrideType override physics type if required
	 * @return rigid body
	 */
	static Body* createBody(World* world, Prototype* prototype, const string& id, const Transformations& transformations, uint16_t collisionTypeId = 0, int index = -1, PrototypePhysics_BodyType* overrideType = nullptr);

	/**
	 * Create rigid body
	 * @param world world
	 * @param sceneEntity scene entity
	 * @param translation translation
	 * @param collisionTypeId collision type id or 0 for default
	 * @param index use a optional index or all bounding volumes
	 * @param overrideType override physics type if required
	 * @return rigid body
	 */
	static Body* createBody(World* world, SceneEntity* sceneEntity, const Vector3& translation = Vector3(0.0f, 0.0f, 0.0f), uint16_t collisionTypeId = 0, int index = -1, PrototypePhysics_BodyType* overrideType = nullptr);

	/**
	 * Add scene to physics world
	 * @param world world
	 * @param scene scene
	 * @param enable enable
	 * @param translation translation
	 * @param progressCallback progress callback
	 */
	static void addScene(World* world, Scene& scene, bool enable = true, const Vector3& translation = Vector3(0.0f, 0.0f, 0.0f), ProgressCallback* progressCallback = nullptr);

	/**
	 * Disable scene in engine
	 * @param engine engine
	 * @param scene scene
	 */
	static void disableScene(Engine* engine, Scene& scene);

	/**
	 * Disable scene in physics world
	 * @param world world
	 * @param scene scene
	 */
	static void disableScene(World* world, Scene& scene);

	/**
	 * Enable disabled scene in engine
	 * @param engine engine
	 * @param scene scene
	 * @param translation translation
	 */
	static void enableScene(Engine* engine, Scene& scene, const Vector3& translation = Vector3(0.0f, 0.0f, 0.0f));

	/**
	 * Enable disabled scene in physics world
	 * @param world world
	 * @param scene scene
	 * @param translation translation
	 */
	static void enableScene(World* world, Scene& scene, const Vector3& translation = Vector3(0.0f, 0.0f, 0.0f));

	/**
	 * Reset engine regarding given scene
	 * @param engine engine
	 * @param scene scene
	 */
	static void resetEngine(Engine* engine, Scene& scene);

	/**
	 * Add scene entity sounds into given audio instance associated with given id
	 * @param audio audio instance to load sounds into
	 * @param prototype scene entity
	 * @param id audio entity id
	 * @param poolSize pool size, which is optional if you want to use a pool for each sound
	 */
	static void addSounds(Audio* audio, Prototype* prototype, const string& id, const int poolSize = 1);

private:
	static Model* emptyModel;
};
