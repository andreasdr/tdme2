#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/primitives/fwd-tdme.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Vector3.h>
#include <tdme/tools/shared/model/fwd-tdme.h>
#include <tdme/utilities/fwd-tdme.h>
#include <tdme/tools/shared/model/ModelProperties.h>

using std::map;
using std::set;
using std::string;
using std::vector;

using tdme::tools::shared::model::ModelProperties;
using tdme::engine::model::Model;
using tdme::engine::model::RotationOrder;
using tdme::engine::primitives::BoundingBox;
using tdme::math::Vector3;
using tdme::tools::shared::model::LevelEditorEntityLibrary;
using tdme::tools::shared::model::LevelEditorLight;
using tdme::tools::shared::model::LevelEditorObject;

/**
 * Level
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::shared::model::LevelEditorLevel final
	: public ModelProperties
{
private:
	string applicationRoot;
	string pathName;
	string fileName;
	RotationOrder* rotationOrder { nullptr };
	vector<LevelEditorLight*> lights;
	LevelEditorEntityLibrary* entityLibrary { nullptr };
	map<string, LevelEditorObject*> objectsById;
	vector<LevelEditorObject*> objects;
	set<string> environmentMappingIds;
	int objectIdx;
	BoundingBox boundingBox;
	Vector3 dimension;
	Vector3 center;
	string skyModelFileName;
	Model* skyModel { nullptr };
	Vector3 skyModelScale;

	/**
	 * Computes level bounding box
	 * @return dimension
	 */
	void computeBoundingBox();

	/**
	 * @return level center
	 */
	void computeCenter();

public:
	/**
	 * Public constructor
	 */
	LevelEditorLevel();

	/**
	 * Destructor
	 */
	~LevelEditorLevel();

	/**
	 * @return game root
	 */
	inline const string& getApplicationRoot() {
		return applicationRoot;
	}

	/**
	 * Set game root
	 * @param gameRoot gameRoot
	 */
	inline void setApplicationRoot(const string& applicationRoot) {
		this->applicationRoot = applicationRoot;
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
	 * @return file name
	 */
	inline const string& getFileName() {
		return fileName;
	}

	/**
	 * Set up level file name
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
	 * Get light at index i
	 * @param i i
	 * @return
	 */
	inline LevelEditorLight* getLightAt(int i) {
		return lights[i];
	}

	/**
	 * @return entity library
	 */
	inline LevelEditorEntityLibrary* getEntityLibrary() {
		return entityLibrary;
	}

	/**
	 * @return dimension
	 */
	inline const Vector3& getDimension() {
		return dimension;
	}

	/**
	 * @return level bounding box
	 */
	inline BoundingBox* getBoundingBox() {
		return &boundingBox;
	}

	/**
	 * @return level center
	 */
	inline const Vector3& getCenter() {
		return center;
	}

	/**
	 * @return new object idx
	 */
	inline int allocateObjectId() {
		return objectIdx++;
	}

	/**
	 * @return object idx
	 */
	inline int getObjectIdx() {
		return objectIdx;
	}

	/**
	 * Set entity idx
	 * @param entityIdx objectIdx
	 */
	inline void setObjectIdx(int entityIdx) {
		this->objectIdx = entityIdx;
	}

	/**
	 * Clears all level objects
	 */
	void clearObjects();

	/**
	 * Get objects with given entity id
	 * @param entityId entity id
	 * @param objectsByEntityId objects by entity id
	 */
	void getObjectsByEntityId(int entityId, vector<string>& objectsByEntityId);

	/**
	 * Remove objects with given entity id
	 * @param entityId entity id
	 */
	void removeObjectsByEntityId(int entityId);

	/**
	 * Replace entity
	 * @param searchEntityId search model id
	 * @param replaceEntityId replace model id
	 */
	void replaceEntity(int searchEntityId, int replaceEntityId);

	/**
	 * @return environment mapping object ids
	 */
	inline set<string> getEnvironmentMappingIds() {
		return environmentMappingIds;
	}

	/**
	 * Adds an object to level
	 * @param object object
	 */
	void addObject(LevelEditorObject* object);

	/**
	 * Removes an object from level
	 * @param id id
	 */
	void removeObject(const string& id);

	/**
	 * Returns level editor object by id
	 * @param id id
	 * @return level editor object or null
	 */
	LevelEditorObject* getObjectById(const string& id);

	/**
	 * @return number of objects
	 */
	inline int getObjectCount() {
		return objects.size();
	}

	/**
	 * Returns object at idx
	 * @param idx idx
	 * @return level object
	 */
	inline LevelEditorObject* getObjectAt(int idx) {
		return objects[idx];
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
	 * Update level dimension, bounding box, center
	 */
	void update();

};
