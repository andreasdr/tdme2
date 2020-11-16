#pragma once

#include <map>
#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/tools/shared/model/fwd-tdme.h>
#include <tdme/utilities/fwd-tdme.h>

using std::map;
using std::vector;
using std::string;

using tdme::math::Vector3;
using tdme::tools::shared::model::LevelEditorEntity;
using tdme::tools::shared::model::LevelEditorLevel;

/**
 * Model Editor Entity Library
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::shared::model::LevelEditorEntityLibrary final
{
public:
	static constexpr int32_t ID_ALLOCATE { -1 };

private:
	LevelEditorLevel* level { nullptr };
	map<int32_t, LevelEditorEntity*> entitiesById;
	vector<LevelEditorEntity*> entities;
	int32_t entityIdx;

	/**
	 * Allocata a unique entity idx
	 * @return
	 */
	int32_t allocateEntityId();

public:
	/**
	 * Public constructor
	 */
	LevelEditorEntityLibrary(LevelEditorLevel* level);

	/**
	 * Destructor
	 */
	~LevelEditorEntityLibrary();

	/**
	 * Clears this model library
	 */
	void clear();

	/**
	 * Adds a model
	 * @param id id
	 * @param name name
	 * @param description description
	 * @param pathName path name
	 * @param fileName file name
	 * @param pivot pivot
	 * @return level editor entity
	 * @throws tdme::utilities::Exception
	 */
	LevelEditorEntity* addModel(int32_t id, const string& name, const string& description, const string& pathName, const string& fileName, const Vector3& pivot) /* throws(Exception) */;

	/**
	 * Add a trigger
	 * @param id id
	 * @param name name
	 * @param description description
	 * @param width width
	 * @param height height
	 * @param depth depth
	 * @return level editor entity
	 * @throws tdme::utilities::Exception
	 */
	LevelEditorEntity* addTrigger(int32_t id, const string& name, const string& description, float width, float height, float depth) /* throws(Exception) */;

	/**
	 * Add a empty
	 * @param id id
	 * @param name name
	 * @param description description
	 * @return level editor entity
	 * @throws tdme::utilities::Exception
	 */
	LevelEditorEntity* addEmpty(int32_t id, const string& name, const string& description) /* throws(Exception) */;

	/**
	 * Add a empty
	 * @param id id
	 * @param name name
	 * @param description description
	 * @return level editor entity
	 * @throws tdme::utilities::Exception
	 */
	LevelEditorEntity* addParticleSystem(int32_t id, const string& name, const string& description) /* throws(Exception) */;

	/**
	 * Add a entity
	 * @param levelEditorEntity model
	 */
	void addEntity(LevelEditorEntity* levelEditorEntity) /* throws(Exception) */;

	/**
	 * @param idx idx
	 * @return entity
	 */
	LevelEditorEntity* getEntityAt(int32_t idx);

	/**
	 * Retrieve a entity
	 * @param id id
	 * @return level editor entity
	 */
	LevelEditorEntity* getEntity(int32_t id);

	/**
	 * Remove a entity
	 * @param id id
	 */
	void removeEntity(int32_t id);

	/**
	 * @return entity count
	 */
	int32_t getEntityCount();

};
