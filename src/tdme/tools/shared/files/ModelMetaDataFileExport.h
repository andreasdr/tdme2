#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/tools/shared/files/fwd-tdme.h>
#include <tdme/tools/shared/model/fwd-tdme.h>

#include <tdme/engine/fileio/models/ModelFileIOException.h>
#include <tdme/os/filesystem/FileSystemException.h>

#include <rapidjson/document.h>

using std::string;

using tdme::engine::fileio::models::ModelFileIOException;
using tdme::os::filesystem::FileSystemException;
using tdme::tools::shared::model::LevelEditorEntity;
using tdme::tools::shared::model::LevelEditorEntityLODLevel;

using rapidjson::Document;
using rapidjson::Value;

/**
 * TDME Model meta data file export
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::shared::files::ModelMetaDataFileExport final
{
private:

	/**
	 * Copy file
	 * @param source source
	 * @param dest dest
	 * @throws tdme::os::filesystem::FileSystemException
	 */
	static void copyFile(const string& source, const string& dest);

	/**
	 * Export LOD level to JSON
	 * @param jDocument JSON document
	 * @param jLodLevelRoot JSON LOD level root value
	 * @param lodLevel lod level
	 * @return json root
	 */
	static void exportLODLevelToJSON(Document& jDocument, Value& jLodLevelRoot, LevelEditorEntityLODLevel* lodLevel);

public:

	/**
	 * Exports a level to a TDME level file
	 * @param pathName path name
	 * @param fileName file name
	 * @param entity entity
	 * @throws tdme::os::filesystem::FileSystemException
	 * @throws tdme::engine::fileio::models::ModelFileIOException
	 */
	static void doExport(const string& pathName, const string& fileName, LevelEditorEntity* entity);

	/**
	 * Export model meta data file to JSON node
	 * @param document JSON document
	 * @param jEntityRoot JSON value to store data in
	 * @param entity entity
	 * @throws tdme::os::filesystem::FileSystemException
	 * @throws tdme::engine::fileio::models::ModelFileIOException
	 */
	static void exportToJSON(Document& jDocument, Value& jEntityRoot, LevelEditorEntity* entity);

};
