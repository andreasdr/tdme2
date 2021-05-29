#pragma once

#include <string>
#include <unordered_map>

#include <tdme/engine/subsystems/manager/fwd-tdme.h>
#include <tdme/engine/subsystems/rendering/fwd-tdme.h>
#include <tdme/utilities/fwd-tdme.h>

using std::string;
using std::unordered_map;

using tdme::engine::subsystems::manager::MeshManager_MeshManaged;
using tdme::engine::subsystems::rendering::Object3DNodeMesh;

/**
 * Mesh manager
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::subsystems::manager::MeshManager final
{
private:
	unordered_map<string, MeshManager_MeshManaged*> meshes;

public:
	/**
	 * Public constructor
	 */
	MeshManager();

	/**
	 * Destructor
	 */
	~MeshManager();

	/**
	 * Get mesh from managed meshes
	 * @param meshId mesh id
	 * @return object 3d node mesh or null
	 */
	Object3DNodeMesh* getMesh(const string& meshId);

	/**
	 * Adds a mesh to manager
	 * @param meshId mesh id
	 * @param mesh mesh
	 */
	void addMesh(const string& meshId, Object3DNodeMesh* mesh);

	/**
	 * Removes a mesh from manager
	 * @param meshId mesh id
	 */
	void removeMesh(const string& meshId);

};
