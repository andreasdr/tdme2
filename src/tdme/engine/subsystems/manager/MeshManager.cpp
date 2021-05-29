#include <tdme/engine/subsystems/manager/MeshManager.h>

#include <string>
#include <unordered_map>

#include <tdme/engine/subsystems/manager/MeshManager_MeshManaged.h>
#include <tdme/engine/subsystems/rendering/Object3DNodeMesh.h>
#include <tdme/utilities/Console.h>

using std::string;
using std::unordered_map;

using tdme::engine::subsystems::manager::MeshManager;
using tdme::engine::subsystems::manager::MeshManager_MeshManaged;
using tdme::engine::subsystems::rendering::Object3DNodeMesh;
using tdme::utilities::Console;

MeshManager::MeshManager()
{
}

MeshManager::~MeshManager() {
	for (auto it = meshes.begin(); it != meshes.end(); ++it) {
		delete it->second;
	}
}

Object3DNodeMesh* MeshManager::getMesh(const string& meshId)
{
	// check if we already manage this mesh
	auto meshManagedIt = meshes.find(meshId);
	if (meshManagedIt != meshes.end()) {
		auto meshManaged = meshManagedIt->second;
		meshManaged->incrementReferenceCounter();
		return meshManaged->getMesh();
	}
	// otherwise no mesh
	return nullptr;
}

void MeshManager::addMesh(const string& meshId, Object3DNodeMesh* mesh)
{
	// create managed texture
	auto meshManaged = new MeshManager_MeshManaged(meshId, mesh);
	meshManaged->incrementReferenceCounter();
	// add it to our textures
	meshes[meshManaged->getId()] = meshManaged;
}

void MeshManager::removeMesh(const string& meshId)
{
	auto meshManagedIt = meshes.find(meshId);
	if (meshManagedIt != meshes.end()) {
		auto meshManaged = meshManagedIt->second;
		if (meshManaged->decrementReferenceCounter()) {
			// remove from our list
			meshes.erase(meshManagedIt);
			delete meshManaged;
		}
		return;
	}
	Console::println(string("Warning: mesh not managed by mesh manager: " + meshId));
}
