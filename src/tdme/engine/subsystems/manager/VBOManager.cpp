#include <tdme/engine/subsystems/manager/VBOManager.h>

#include <map>
#include <string>

#include <tdme/engine/subsystems/manager/VBOManager_VBOManaged.h>
#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/os/threading/ReadWriteLock.h>
#include <tdme/utilities/Console.h>

using std::map;
using std::string;

using tdme::engine::subsystems::manager::VBOManager;
using tdme::engine::subsystems::manager::VBOManager_VBOManaged;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::os::threading::ReadWriteLock;
using tdme::utilities::Console;

VBOManager::VBOManager(Renderer* renderer): rwLock("vbomanager-rwlock") {
	this->renderer = renderer;
}

VBOManager::~VBOManager() {
	for (auto it = vbos.begin(); it != vbos.end(); ++it) {
		delete it->second;
	}
}

VBOManager_VBOManaged* VBOManager::addVBO(const string& vboId, int32_t ids, bool useGPUMemory, bool shared, bool& created) {
	rwLock.writeLock();
	// check if we already manage this vbo
	auto vboManagedIt = vbos.find(vboId);
	if (vboManagedIt != vbos.end()) {
		auto vboManaged = vboManagedIt->second;
		vboManaged->incrementReferenceCounter();
		rwLock.unlock();
		// yep, return vbo managed
		created = false;
		return vboManaged;
	}
	// create vertex buffer objects
	auto vboIds = renderer->createBufferObjects(ids, useGPUMemory, shared);
	// create managed texture
	auto vboManaged = new VBOManager_VBOManaged(vboId, vboIds);
	// add it to our textures
	vboManaged->incrementReferenceCounter();
	vbos[vboManaged->getId()] = vboManaged;
	rwLock.unlock();
	created = true;

	// return vbo managed
	return vboManaged;
}

VBOManager_VBOManaged* VBOManager::getVBO(const string& vboId) {
	rwLock.readLock();
	// check if we already manage this vbo
	auto vboManagedIt = vbos.find(vboId);
	if (vboManagedIt != vbos.end()) {
		auto vboManaged = vboManagedIt->second;
		vboManaged->incrementReferenceCounter();
		rwLock.unlock();
		// yep, return vbo managed
		return vboManaged;
	}
	// nope
	rwLock.unlock();
	return nullptr;
}

void VBOManager::removeVBO(const string& vboId) {
	rwLock.writeLock();
	auto vboManagedIt = vbos.find(vboId);
	if (vboManagedIt != vbos.end()) {
		auto vboManaged = vboManagedIt->second;
		if (vboManaged->decrementReferenceCounter()) {
			auto vboIds = vboManaged->getVBOIds();
			// delete vbos from renderer
			renderer->disposeBufferObjects(*vboIds);
			// remove from our list
			vbos.erase(vboManagedIt);
			delete vboManaged;
		}
		rwLock.unlock();
		return;
	}
	rwLock.unlock();
	Console::println(string("Warning: vbo not managed by vbo manager: ") + vboId);
}
