#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/tools/shared/files/fwd-tdme.h>
#include <tdme/tools/shared/model/fwd-tdme.h>

#include <tdme/engine/fileio/models/ModelFileIOException.h>
#include <tdme/os/filesystem/FileSystemException.h>
#include <tdme/tools/shared/model/LevelEditorEntity.h>

#include <rapidjson/document.h>

using std::string;

using tdme::engine::fileio::models::ModelFileIOException;
using tdme::tools::shared::model::LevelEditorEntity;
using tdme::tools::shared::model::LevelEditorEntityBoundingVolume;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem;
using tdme::tools::shared::model::LevelEditorEntityLODLevel;
using tdme::os::filesystem::FileSystemException;

using rapidjson::Value;

/**
 * TDME Model meta data file import
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::shared::files::ModelMetaDataFileImport final
{
public:
	/**
	 * Imports a model meta data file from file
	 * @param pathName path name
	 * @param fileName file name
	 * @throws tdme::os::filesystem::FileSystemException
	 * @throws tdme::engine::fileio::models::ModelFileIOException
	 * @return level editor entity
	 */
	inline static LevelEditorEntity* doImport(const string& pathName, const string& fileName) {
		return doImport(LevelEditorEntity::ID_NONE, pathName, fileName);
	}

	/**
	 * Imports a model meta data file from file
	 * @param id id or LevelEditorEntity.ID_NONE
	 * @param pathName path name
	 * @param fileName file name
	 * @throws tdme::os::filesystem::FileSystemException
	 * @throws tdme::engine::fileio::models::ModelFileIOException
	 * @return level editor entity
	 */
	static LevelEditorEntity* doImport(int32_t id, const string& pathName, const string& fileName);

	/**
	 * Imports a model meta data file from JSON object
	 * @param id id or LevelEditorEntity.ID_NONE
	 * @param pathName path name or null
	 * @param jEntityRoot JSON entity root
	 * @throws tdme::os::filesystem::FileSystemException
	 * @throws tdme::engine::fileio::models::ModelFileIOException
	 * @return level editor entity
	 */
	static LevelEditorEntity* doImportFromJSON(int32_t id, const string& pathName, Value& jEntityRoot);

	/**
	 * Get resource path name
	 * @param pathName path name within a TDME2 resource folder where parent resource was loaded from
	 * @param fileName file name containing a path and file in a TDME2 resource folder
	 * @return model path name
	 */
	static const string getResourcePathName(const string& pathName, const string& fileName);

private:

	/**
	 * Parse bounding volume
	 * @param idx idx
	 * @param levelEditorEntity level editor entity
	 * @param pathName path name
	 * @param jBv JSON bounding volume node
	 * @throws tdme::os::filesystem::FileSystemException
	 * @throws tdme::engine::fileio::models::ModelFileIOException
	 * @return level editor entity bounding volume
	 */
	static LevelEditorEntityBoundingVolume* parseBoundingVolume(int32_t idx, LevelEditorEntity* levelEditorEntity, const string& pathName, Value& jBv);

	/**
	 * Parse LOD level
	 * @param pathName path name
	 * @param jLodLevel JSON LOD level
	 * @return level editor lod level
	 */
	static LevelEditorEntityLODLevel* parseLODLevel(const string& pathName, Value& jLodLevel);

	/**
	 * Parse particle system
	 * @param particleSystem particle system to be parsed into
	 * @param pathName path name
	 * @param jParticleSystem JSON particle system object
	 */
	static void parseParticleSystem(LevelEditorEntityParticleSystem* particleSystem, const string& pathName, Value& jParticleSystem);

};
