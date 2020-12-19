#pragma once

#include <map>
#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/fileio/textures/fwd-tdme.h>
#include <tdme/os/filesystem/fwd-tdme.h>
#include <tdme/os/filesystem/FileSystemException.h>
#include <tdme/os/threading/Mutex.h>
#include <tdme/utilities/ByteBuffer.h>

#include <ext/libpng/png.h>

using std::map;
using std::string;
using std::vector;

using tdme::engine::fileio::textures::Texture;
using tdme::os::filesystem::FileSystemException;
using tdme::os::threading::Mutex;
using tdme::utilities::ByteBuffer;

/**
 * Texture reader class
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::fileio::textures::TextureReader final
{
	friend class Texture;

public:
	/**
	 * @return texture extensions
	 */
	static const vector<string>& getTextureExtensions();

	/**
	 * Reads a texture
	 * @param pathName path name
	 * @param fileName file name
	 * @param useCache use cache
	 * @param powerOfTwo scale image to fit power of two dimensions
	 * @return texture data instance or null
	 */
	static Texture* read(const string& pathName, const string& fileName, bool useCache = true, bool powerOfTwo = true);

	/**
	 * Reads a texture with additional transparency texture
	 * @param texturePathName texture path name
	 * @param textureFileName texture file name
	 * @param transparencyTexturePathName transparency texture path name
	 * @param transparencyTextureFileName transparency texture file name
	 * @param useCache use cache
	 * @param powerOfTwo scale image to fit power of two dimensions
	 * @return texture data instance or null
	 */
	static Texture* read(const string& texturePathName, const string& textureFileName, const string& transparencyTexturePathName, const string& transparencyTextureFileName, bool useCache = true, bool powerOfTwo = true);

private:
	/**
	 * PNG input stream
	 */
	class PNGInputStream {
	public:

		/**
		 * Public constructor
		 * @author Andreas Drewke
		 * @version $Id$
		 */
		PNGInputStream(vector<uint8_t>* data) {
			this->offset = 0;
			this->data = data;
		}

		/**
		 * Destructor
		 */
		~PNGInputStream() {
		}

		/**
		 * Read byte
		 * @param outBytes out bytes
		 * @param outBytesToRead out bytes to read
		 */
		void readBytes(int8_t* outBytes, int32_t outBytesToRead) {
			for (int32_t i = 0; i < outBytesToRead && offset < data->size(); i++) {
				outBytes[i] = (*data)[offset++];
			}
		}

	private:
		int32_t offset;
		vector<uint8_t>* data;

	};

	/**
	 * Read PNG data from memory
	 * @param png_ptr png structure
	 * @param outBytes out bytes
	 * @param outBytesToRead out bytes to read
	 */
	static void readPNGDataFromMemory(png_structp png_ptr, png_bytep outBytes, png_size_t outBytesToRead);

	/**
	 * Remove texture from cache
	 * @param texture texture
	 */
	static void removeFromCache(Texture* texture);

	/**
	 * Load PNG
	 * @param path path name
	 * @param fileName file name
	 * @param powerOfTwo scale image to fit power of two dimensions
	 */
	static Texture* loadPNG(const string& path, const string& fileName, bool powerOfTwo = true);

	/**
	 * Scales a texture line
	 * @param pixelByteBuffer pixel byte buffer aka original texture
	 * @param pixelByteBufferScaled pixel byte buffer scaled aka new texture, its offset should point to start of line you want to write
	 * @param width orginal width
	 * @param textureWidth new texture width
	 * @param bytesPerPixel bytes per pixel
	 * @param y y position in original image
	 */
	static void scaleTextureLine(ByteBuffer* pixelByteBuffer, ByteBuffer* pixelByteBufferScaled, int width, int textureWidth, int bytesPerPixel, int y);

	//
	static vector<string> extensions;

	// maybe have a read write lock here for texture cache, but currently I have no multithreaded access to it
	static map<string, Texture*>* textureCache;
	static Mutex* textureCacheMutex;
};
