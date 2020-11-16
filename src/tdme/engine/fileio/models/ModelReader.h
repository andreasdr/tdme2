#pragma once

#include <string>
#include <vector>

#include <tdme/audio/decoder/fwd-tdme.h>
#include <tdme/engine/fileio/models/fwd-tdme.h>
#include <tdme/engine/fileio/models/ModelFileIOException.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/os/filesystem/fwd-tdme.h>
#include <tdme/os/filesystem/FileSystemException.h>

using std::string;
using std::vector;

using tdme::engine::fileio::models::ModelFileIOException;
using tdme::engine::model::Model;
using tdme::os::filesystem::FileSystemException;

/**
 * Model reader class
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::fileio::models::ModelReader final
{
private:
	static vector<string> extensions;
public:
	/**
	 * Get supported model extensions
	 */
	static const vector<string>& getModelExtensions();

	/**
	 * Reads a model
	 * @param pathName path name
	 * @param fileName file name
	 * @throws tdme::engine::fileio::models::ModelFileIOException
	 * @throws tdme::os::filesystem::FileSystemException
	 * @return texture data instance or null
	 */
	static Model* read(const string& pathName, const string& fileName);
};
