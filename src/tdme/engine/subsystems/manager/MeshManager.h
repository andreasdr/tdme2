#pragma once

#include <map>
#include <string>

#include <tdme/engine/subsystems/manager/fwd-tdme.h>
#include <tdme/engine/subsystems/rendering/fwd-tdme.h>
#include <tdme/utilities/fwd-tdme.h>

using std::map;
using std::string;

using tdme::engine::subsystems::manager::MeshManager_MeshManaged;
using tdme::engine::subsystems::rendering::Object3DGroupMesh;

/** 
 * Mesh manager
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::subsystems::manager::MeshManager final
{
private:
	map<string, MeshManager_MeshManaged*> meshes;

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
	 * @return object 3d group mesh or null
	 */
	Object3DGroupMesh* getMesh(const string& meshId);

	/** 
	 * Adds a mesh to manager
	 * @param meshId mesh id
	 * @param mesh mesh
	 */
	void addMesh(const string& meshId, Object3DGroupMesh* mesh);

	/** 
	 * Removes a mesh from manager
	 * @param meshId mesh id
	 */
	void removeMesh(const string& meshId);

};
