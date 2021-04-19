#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/fileio/prototypes/fwd-tdme.h>
#include <tdme/engine/fileio/scenes/fwd-tdme.h>
#include <tdme/engine/prototype/fwd-tdme.h>

#include <tdme/engine/fileio/models/ModelFileIOException.h>
#include <tdme/engine/prototype/Prototype.h>
#include <tdme/os/filesystem/FileSystemException.h>

#include <rapidjson/document.h>

using std::string;

using tdme::engine::fileio::models::ModelFileIOException;
using tdme::engine::fileio::prototypes::PrototypeTransformFilter;
using tdme::engine::prototype::Prototype;
using tdme::engine::prototype::PrototypeBoundingVolume;
using tdme::engine::prototype::PrototypeLODLevel;
using tdme::engine::prototype::PrototypeParticleSystem;
using tdme::os::filesystem::FileSystemException;

using rapidjson::Value;

/**
 * Prototype reader
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::fileio::prototypes::PrototypeReader final
{
	friend class tdme::engine::fileio::scenes::SceneReader;

public:
	/**
	 * Reads a prototype from file
	 * @param pathName path name
	 * @param fileName file name
	 * @param transformFilter transform filter or nullptr
	 * @throws tdme::os::filesystem::FileSystemException
	 * @throws tdme::engine::fileio::models::ModelFileIOException
	 * @return prototype
	 */
	inline static Prototype* read(const string& pathName, const string& fileName, PrototypeTransformFilter* transformFilter = nullptr) {
		return read(Prototype::ID_NONE, pathName, fileName, transformFilter);
	}

	/**
	 * Reads a prototype from file
	 * @param id id or Prototype.ID_NONE
	 * @param pathName path name
	 * @param fileName file name
	 * @param transformFilter transform filter or nullptr
	 * @throws tdme::os::filesystem::FileSystemException
	 * @throws tdme::engine::fileio::models::ModelFileIOException
	 * @return prototype
	 */
	static Prototype* read(int id, const string& pathName, const string& fileName, PrototypeTransformFilter* transformFilter = nullptr);

	/**
	 * Get resource path name
	 * @param pathName path name within a TDME2 resource folder where parent resource was loaded from
	 * @param fileName file name containing a path and file in a TDME2 resource folder
	 * @return model path name
	 */
	static const string getResourcePathName(const string& pathName, const string& fileName);

private:
	/**
	 * Reads a prototype from JSON object
	 * @param id id or Prototype.ID_NONE
	 * @param pathName path name or null
	 * @param jEntityRoot JSON entity root
	 * @param transformFilter transform filter or nullptr
	 * @throws tdme::os::filesystem::FileSystemException
	 * @throws tdme::engine::fileio::models::ModelFileIOException
	 * @return prototype
	 */
	static Prototype* read(int id, const string& pathName, Value& jEntityRoot, PrototypeTransformFilter* transformFilter = nullptr);

	/**
	 * Parse bounding volume
	 * @param idx idx
	 * @param prototype prototype
	 * @param pathName path name
	 * @param jBv JSON bounding volume node
	 * @throws tdme::os::filesystem::FileSystemException
	 * @throws tdme::engine::fileio::models::ModelFileIOException
	 * @return prototype bounding volume
	 */
	static PrototypeBoundingVolume* parseBoundingVolume(int idx, Prototype* prototype, const string& pathName, Value& jBv);

	/**
	 * Parse LOD level
	 * @param pathName path name
	 * @param jLodLevel JSON LOD level
	 * @return scene editor lod level
	 */
	static PrototypeLODLevel* parseLODLevel(const string& pathName, Value& jLodLevel);

	/**
	 * Parse particle system
	 * @param particleSystem particle system to be parsed into
	 * @param pathName path name
	 * @param jParticleSystem JSON particle system object
	 */
	static void parseParticleSystem(PrototypeParticleSystem* particleSystem, const string& pathName, Value& jParticleSystem);

};
