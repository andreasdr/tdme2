#pragma once

#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/fileio/models/ModelFileIOException.h>
#include <tdme/engine/model/Node.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/tools/shared/files/fwd-tdme.h>
#include <tdme/tools/shared/model/fwd-tdme.h>
#include <tdme/os/filesystem/FileSystemException.h>
#include <rapidjson/document.h>

using std::string;
using std::vector;

using tdme::engine::fileio::models::ModelFileIOException;
using tdme::engine::model::Node;
using tdme::math::Matrix4x4;
using tdme::tools::shared::files::ProgressCallback;
using tdme::tools::shared::model::LevelEditorLevel;
using tdme::os::filesystem::FileSystemException;

using rapidjson::Value;

/**
 * TDME Level Editor File Export
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::shared::files::LevelFileImport final
{

public:

	/**
	 * Imports a level from a TDME level file to Level Editor
	 * @param pathName path name
	 * @param fileName file name
	 * @param level level
	 * @param progressCallback progress callback
	 * @throws tdme::os::filesystem::FileSystemException
	 * @throws tdme::engine::fileio::models::ModelFileIOException
	 */
	static void doImport(const string& pathName, const string& fileName, LevelEditorLevel* level, ProgressCallback* progressCallback = nullptr);

	/**
	 * Imports a level from a TDME level file to Level Editor
	 * @param pathName path name
	 * @param fileName file name
	 * @param level level
	 * @param objectIdPrefix object id prefix
	 * @param progressCallback progress callback
	 * @throws tdme::os::filesystem::FileSystemException
	 * @throws tdme::engine::fileio::models::ModelFileIOException
	 */
	static void doImport(const string& pathName, const string& fileName, LevelEditorLevel* level, const string& objectIdPrefix, ProgressCallback* progressCallback = nullptr);

	/**
	 * Imports a level from a model file to Level Editor
	 * @param pathName path name
	 * @param fileName file name
	 * @param level level
	 * @param progressCallback progress callback
	 * @throws tdme::os::filesystem::FileSystemException
	 * @throws tdme::engine::fileio::models::ModelFileIOException
	 */
	static void doImportFromModel(const string& pathName, const string& fileName, LevelEditorLevel* level, ProgressCallback* progressCallback = nullptr);

private:
	struct LevelEditorEntityMeshNode {
		string id;
		string name;
		Matrix4x4 transformationsMatrix;
		Node* node;
	};

	/**
	 * Determine mesh nodes in node hierarchy
	 * @param level level
	 * @param node node
	 * @param parentName parent node
	 * @param parentTransformationsMatrix parent transformations matrix
	 * @param meshNodes mesh nodes
	 */
	static void determineMeshNodes(LevelEditorLevel* level, Node* node, const string& parentName, const Matrix4x4& parentTransformationsMatrix, vector<LevelEditorEntityMeshNode>& meshNodes);
};
