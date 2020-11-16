#pragma once

#include <map>
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
using std::vector;
using std::string;

using tdme::tools::shared::model::ModelProperties;
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
	int32_t objectIdx;
	BoundingBox boundingBox;
	Vector3 dimension;
	Vector3 center;

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
	inline int32_t getLightCount() {
		return lights.size();
	}

	/**
	 * Get light at index i
	 * @param i i
	 * @return
	 */
	inline LevelEditorLight* getLightAt(int32_t i) {
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
	inline int32_t allocateObjectId() {
		return objectIdx++;
	}

	/**
	 * @return object idx
	 */
	inline int32_t getObjectIdx() {
		return objectIdx;
	}

	/**
	 * Set entity idx
	 * @param entityIdx objectIdx
	 */
	inline void setObjectIdx(int32_t entityIdx) {
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
	void getObjectsByEntityId(int32_t entityId, vector<string>& objectsByEntityId);

	/**
	 * Remove objects with given entity id
	 * @param entityId entity id
	 */
	void removeObjectsByEntityId(int32_t entityId);

	/**
	 * Replace entity
	 * @param searchEntityId search model id
	 * @param replaceEntityId replace model id
	 */
	void replaceEntity(int32_t searchEntityId, int32_t replaceEntityId);

	/**
	 * Updates pivot
	 * @param modelId model id
	 * @param pivot pivot
	 */
	void updatePivot(int32_t modelId, const Vector3& pivot);

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
	inline int32_t getObjectCount() {
		return objects.size();
	}

	/**
	 * Returns object at idx
	 * @param idx idx
	 * @return level object
	 */
	inline LevelEditorObject* getObjectAt(int32_t idx) {
		return objects[idx];
	}

	/**
	 * Update level dimension, bounding box, center
	 */
	void update();

};
