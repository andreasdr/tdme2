#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/fileio/textures/fwd-tdme.h>

using std::string;

using tdme::engine::fileio::textures::Texture;

/** 
 * PNG texture writer class
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::fileio::textures::PNGTextureWriter final
{
	friend class Texture;

public:

	/** 
	 * Writes a texture to PNG
	 * @param texture texture
	 * @param pathName path name
	 * @param fileName file name
	 * @param removeAlphaChannel remove alpha channel
	 * @param flipY flip Y
	 * @return texture data instance or null
	 */
	static bool write(Texture* texture, const string& pathName, const string& fileName, bool removeAlphaChannel = true, bool flipY = true);

};
