#include <tdme/engine/subsystems/manager/TextureManager.h>

#include <map>
#include <string>

#include <tdme/engine/fileio/textures/Texture.h>
#include <tdme/engine/subsystems/manager/TextureManager_TextureManaged.h>
#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/os/threading/Mutex.h>
#include <tdme/utilities/Console.h>

using std::map;
using std::string;

using tdme::engine::fileio::textures::Texture;
using tdme::engine::subsystems::manager::TextureManager;
using tdme::engine::subsystems::manager::TextureManager_TextureManaged;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::os::threading::Mutex;
using tdme::utilities::Console;;

TextureManager::TextureManager(Renderer* renderer): mutex("texturemanager-mutex") {
	this->renderer = renderer;
}

TextureManager::~TextureManager() {
	for (auto it = textures.begin(); it != textures.end(); ++it) {
		delete it->second;
	}
}

TextureManager_TextureManaged* TextureManager::addTexture(const string& id, bool& created)
{
	// check if we already manage this texture
	mutex.lock();
	auto textureManagedIt = textures.find(id);
	if (textureManagedIt != textures.end()) {
		//
		auto textureManaged = textureManagedIt->second;
		textureManaged->incrementReferenceCounter();
		mutex.unlock();
		// yep, return renderer texture id
		created = false;
		return textureManaged;
	}
	// create texture
	auto textureId = renderer->createTexture();
	// create managed texture
	auto textureManaged = new TextureManager_TextureManaged(id, textureId);
	// add it to our textures
	textureManaged->incrementReferenceCounter();
	textures[id] = textureManaged;
	//
	mutex.unlock();
	//
	created = true;
	return textureManaged;
}

int32_t TextureManager::addTexture(Texture* texture, void* context)
{
	bool created;
	auto textureManaged = addTexture(texture->getId(), created);
	auto rendererId = textureManaged->getRendererId();
	if (context == nullptr) context = renderer->getDefaultContext();

	// upload if it was created
	if (created == true) {
		// bind texture
		renderer->bindTexture(context, rendererId);
		// upload texture
		renderer->uploadTexture(context, texture);
		// unbind texture
		renderer->bindTexture(context, renderer->ID_NONE);
		//
		textureManaged->setUploaded(true);
	}

	// return renderer id
	return rendererId;
}

int32_t TextureManager::addCubeMapTexture(const string& id, Texture* textureLeft, Texture* textureRight, Texture* textureTop, Texture* textureBottom, Texture* textureFront, Texture* textureBack, void* context)
{
	bool created;
	auto textureManaged = addTexture(id, created);
	auto rendererId = textureManaged->getRendererId();
	if (context == nullptr) context = renderer->getDefaultContext();

	// upload if it was created
	if (created == true) {
		// bind texture
		renderer->bindCubeMapTexture(context, rendererId);
		// upload texture
		renderer->uploadCubeMapTexture(context, textureLeft, textureRight, textureTop, textureBottom, textureFront, textureBack);
		// unbind texture
		renderer->bindCubeMapTexture(context, renderer->ID_NONE);
		//
		textureManaged->setUploaded(true);
	}

	// return renderer id
	return rendererId;
}

void TextureManager::removeTexture(const string& textureId)
{
	mutex.lock();
	auto textureManagedIt = textures.find(textureId);
	if (textureManagedIt != textures.end()) {
		auto textureManaged = textureManagedIt->second;
		if (textureManaged->decrementReferenceCounter()) {
			// delete texture
			auto textureRendererId = textureManaged->getRendererId();
			// remove from our list
			textures.erase(textureManagedIt);
			//
			mutex.unlock();
			// remove from renderer
			renderer->disposeTexture(textureRendererId);
			delete textureManaged;
			//
			return;
		}
		//
		mutex.unlock();
		return;
	}
	mutex.unlock();
	Console::println(string("Warning: texture not loaded by texture manager"));
}
