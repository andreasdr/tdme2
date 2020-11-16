#pragma once

#include <map>
#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/fileio/textures/fwd-tdme.h>
#include <tdme/engine/subsystems/manager/fwd-tdme.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/os/threading/Mutex.h>

using std::map;
using std::string;

using tdme::engine::fileio::textures::Texture;
using tdme::engine::subsystems::manager::TextureManager_TextureManaged;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::os::threading::Mutex;

/**
 * Texture manager
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::subsystems::manager::TextureManager final
{
	friend class TextureManager_TextureManaged;

private:
	Renderer* renderer { nullptr };
	map<string, TextureManager_TextureManaged*> textures;
	Mutex mutex;

public:

	/**
	 * Adds a texture to manager
	 * @param id id
	 * @param created if managed texture has just been created
	 * @returns texture manager entity
	 */
	TextureManager_TextureManaged* addTexture(const string& id, bool& created);

	/**
	 * Adds a texture to manager
	 * @param texture texture
	 * @param context context or nullptr if using default context
	 * @returns texture id
	 */
	int32_t addTexture(Texture* texture, void* context = nullptr);

	/**
	 * Adds a cube map texture to manager
	 * @param id id
	 * @param textureLeft texture left
	 * @param textureRight texture right
	 * @param textureTop texture top
	 * @param textureBottom texture bottom
	 * @param textureFront texture front
	 * @param textureBack texture back
	 * @param context context or nullptr if using default context
	 * @returns texture id
	 */
	int32_t addCubeMapTexture(const string& id, Texture* textureLeft, Texture* textureRight, Texture* textureTop, Texture* textureBottom, Texture* textureFront, Texture* textureBack, void* context = nullptr);

	/**
	 * Removes a texture from manager / open gl stack
	 * @param textureId texture id
	 */
	void removeTexture(const string& textureId);

	/**
	 * Public constructor
	 * @param renderer renderer
	 */
	TextureManager(Renderer* renderer);

	/**
	 * Destructor
	 */
	~TextureManager();
};
