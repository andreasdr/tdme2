#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/fileio/prototypes/fwd-tdme.h>
#include <tdme/engine/prototype/fwd-tdme.h>

#include <tdme/engine/fileio/models/ModelFileIOException.h>
#include <tdme/os/filesystem/FileSystemException.h>

#include <rapidjson/document.h>

using std::string;

using tdme::engine::fileio::models::ModelFileIOException;
using tdme::engine::prototype::Prototype;
using tdme::engine::prototype::PrototypeLODLevel;
using tdme::os::filesystem::FileSystemException;

using rapidjson::Document;
using rapidjson::Value;

/**
 * Prototype writer
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::fileio::prototypes::PrototypeWriter final
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
	 * Writes LOD level to JSON
	 * @param jDocument JSON document
	 * @param jLodLevelRoot JSON LOD level root value
	 * @param lodLevel lod level
	 * @return json root
	 */
	static void writeLODLevelToJSON(Document& jDocument, Value& jLodLevelRoot, PrototypeLODLevel* lodLevel);

public:

	/**
	 * Writes a prototype to file
	 * @param pathName path name
	 * @param fileName file name
	 * @param prototype prototype
	 * @throws tdme::os::filesystem::FileSystemException
	 * @throws tdme::engine::fileio::models::ModelFileIOException
	 */
	static void write(const string& pathName, const string& fileName, Prototype* prototype);

	/**
	 * Writes a prototype to JSON
	 * @param document JSON document
	 * @param jEntityRoot JSON value to store data in
	 * @param prototype prototype
	 * @throws tdme::os::filesystem::FileSystemException
	 * @throws tdme::engine::fileio::models::ModelFileIOException
	 */
	static void write(Document& jDocument, Value& jEntityRoot, Prototype* prototype);

};
