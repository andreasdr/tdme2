#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/primitives/fwd-tdme.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/prototype/fwd-tdme.h>
#include <tdme/engine/prototype/PrototypeProperties.h>
#include <tdme/engine/scene/fwd-tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Vector3.h>
#include <tdme/utilities/fwd-tdme.h>

using std::map;
using std::set;
using std::string;
using std::vector;

using tdme::engine::model::Model;
using tdme::engine::model::RotationOrder;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::prototype::PrototypeProperties;
using tdme::engine::scene::SceneEntity;
using tdme::engine::scene::SceneLibrary;
using tdme::engine::scene::SceneLight;
using tdme::math::Vector3;

/**
 * Scene definition
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::scene::Scene final
	: public PrototypeProperties
{
private:
	string applicationRootPathName;
	string pathName;
	string fileName;
	RotationOrder* rotationOrder { nullptr };
	vector<SceneLight*> lights;
	SceneLibrary* library { nullptr };
	map<string, SceneEntity*> entitiesById;
	vector<SceneEntity*> entities;
	set<string> environmentMappingIds;
	int entityIdx;
	BoundingBox boundingBox;
	Vector3 dimension;
	Vector3 center;
	string skyModelFileName;
	Model* skyModel { nullptr };
	Vector3 skyModelScale;

	/**
	 * Computes scene bounding box
	 * @return dimension
	 */
	void computeBoundingBox();

	/**
	 * @return scene center
	 */
	void computeCenter();

public:
	/**
	 * Public constructor
	 */
	Scene();

	/**
	 * Destructor
	 */
	~Scene();

	/**
	 * @return application root path name
	 */
	inline const string& getApplicationRootPathName() {
		return applicationRootPathName;
	}

	/**
	 * Set application root path name
	 * @param applicationRootPathName application root path name
	 */
	inline void setApplicationRootPathName(const string& applicationRootPathName) {
		this->applicationRootPathName = applicationRootPathName;
	}

	/**
	 * @return path name
	 */
	inline const string& getPathName() {
		return pathName;
	}

	/**
	 * Set up path name
	 * @param pathName pathName
	 */
	inline void setPathName(const string& pathName) {
		this->pathName = pathName;
	}

	/**
	 * @return scene file name
	 */
	inline const string& getFileName() {
		return fileName;
	}

	/**
	 * Set up scene file name
	 * @param fileName file name
	 */
	inline void setFileName(const string& fileName) {
		this->fileName = fileName;
	}

	/**
	 * @return rotation order
	 */
	inline RotationOrder* getRotationOrder() {
		return rotationOrder;
	}

	/**
	 * Set rotation order
	 * @param rotationOrder rotation order
	 */
	inline void setRotationOrder(RotationOrder* rotationOrder) {
		this->rotationOrder = rotationOrder;
	}

	/**
	 * @return number of lights
	 */
	inline int getLightCount() {
		return lights.size();
	}

	/**
	 * Get light at given index
	 * @param i index
	 * @return light
	 */
	inline SceneLight* getLightAt(int i) {
		return lights[i];
	}

	/**
	 * @return scene prototype library
	 */
	inline SceneLibrary* getLibrary() {
		return library;
	}

	/**
	 * @return dimension
	 */
	inline const Vector3& getDimension() {
		return dimension;
	}

	/**
	 * @return scene bounding box
	 */
	inline BoundingBox* getBoundingBox() {
		return &boundingBox;
	}

	/**
	 * @return scene center
	 */
	inline const Vector3& getCenter() {
		return center;
	}

	/**
	 * @return new entity id
	 */
	inline int allocateEntityId() {
		return entityIdx++;
	}

	/**
	 * @return entity idx
	 */
	inline int getEntityIdx() {
		return entityIdx;
	}

	/**
	 * Set entity idx
	 * @param entityIdx objectIdx
	 */
	inline void setEntityIdx(int entityIdx) {
		this->entityIdx = entityIdx;
	}

	/**
	 * Clears all scene entities
	 */
	void clearEntities();

	/**
	 * Get entities with given prototype id
	 * @param prototypeId prototype id
	 * @param entitiesByPrototypeId entities by prototype id
	 */
	void getEntitiesByPrototypeId(int prototypeId, vector<string>& entitiesByPrototypeId);

	/**
	 * Remove entities with given prototype id
	 * @param prototypeId prototype id
	 */
	void removeEntitiesByPrototypeId(int prototypeId);

	/**
	 * Replace entity
	 * @param searchPrototypeId search prototype id
	 * @param newPrototypeId new prototype id
	 */
	void replacePrototype(int searchPrototypeId, int newPrototypeId);

	/**
	 * @return environment mapping object ids
	 */
	inline set<string> getEnvironmentMappingIds() {
		return environmentMappingIds;
	}

	/**
	 * Adds an entity to scene
	 * @param object object
	 */
	void addEntity(SceneEntity* entity);

	/**
	 * Removes an entity from scene
	 * @param id id
	 */
	void removeEntity(const string& id);

	/**
	 * Returns scene entity by id
	 * @param id id
	 * @return scene entity
	 */
	SceneEntity* getEntity(const string& id);

	/**
	 * @return number of entities
	 */
	inline int getEntityCount() {
		return entities.size();
	}

	/**
	 * Returns entity at given index
	 * @param idx index
	 * @return scene entity
	 */
	inline SceneEntity* getEntityAt(int idx) {
		return entities[idx];
	}

	/**
	 * @return sky model file name
	 */
	inline const string& getSkyModelFileName() {
		return skyModelFileName;
	}

	/**
	 * Set sky model file name
	 * @param skyModelFileName sky model file name
	 */
	inline void setSkyModelFileName(const string& skyModelFileName) {
		this->skyModelFileName = skyModelFileName;
	}

	/**
	 * @return sky model
	 */
	inline Model* getSkyModel() {
		return skyModel;
	}

	/**
	 * Set sky model
	 */
	void setSkyModel(Model* model);

	/**
	 * @return sky model scale
	 */
	inline const Vector3& getSkyModelScale() {
		return skyModelScale;
	}

	/**
	 * Set sky model scale
	 */
	void setSkyModelScale(const Vector3& skyModelScale) {
		this->skyModelScale = skyModelScale;
	}

	/**
	 * Update scene dimension, bounding box, center
	 */
	void update();

};
