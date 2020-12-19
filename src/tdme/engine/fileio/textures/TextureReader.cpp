#include <tdme/engine/fileio/textures/TextureReader.h>

#include <stdlib.h>

#include <string>
#include <vector>

#include <tdme/utilities/ByteBuffer.h>

#include <tdme/engine/fileio/textures/Texture.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/os/threading/Mutex.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>
#include <tdme/utilities/StringTools.h>

#include <ext/libpng/png.h>

using std::string;
using std::to_string;
using std::vector;

using tdme::utilities::ByteBuffer;
using tdme::utilities::Console;
using tdme::utilities::Exception;

using tdme::engine::fileio::textures::Texture;
using tdme::engine::fileio::textures::TextureReader;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;
using tdme::os::threading::Mutex;
using tdme::utilities::StringTools;

vector<string> TextureReader::extensions = {"png"};
Mutex* TextureReader::textureCacheMutex = new Mutex("texturecache-mutex");
map<string, Texture*>* TextureReader::textureCache = new map<string, Texture*>();

const vector<string>& TextureReader::getTextureExtensions() {
	return extensions;
}

Texture* TextureReader::read(const string& pathName, const string& fileName, bool useCache, bool powerOfTwo)
{
	Texture* texture = nullptr;

	// make canonical
	auto canonicalFile = FileSystem::getInstance()->getCanonicalPath(pathName, fileName);
	auto canonicalPathName = FileSystem::getInstance()->getPathName(canonicalFile);
	auto canonicalFileName = FileSystem::getInstance()->getFileName(canonicalFile);

	// do cache look up
	if (useCache == true) {
		textureCacheMutex->lock();
		auto textureCacheIt = textureCache->find(canonicalPathName + "/" + canonicalFileName);
		if (textureCacheIt != textureCache->end()) {
			texture = textureCacheIt->second;
		}
	}

	// have texture?
	if (texture == nullptr) {
		// nope try to load
		try {
			if (StringTools::endsWith(StringTools::toLowerCase(canonicalFileName), ".png") == true) {
				texture = TextureReader::loadPNG(canonicalPathName, canonicalFileName, powerOfTwo);
				if (texture != nullptr && useCache == true) {
					(*textureCache)[texture->getId()] = texture;
				}
			}
		} catch (Exception& exception) {
			Console::println("TextureReader::loadTexture(): Could not load texture: " + canonicalPathName + "/" + canonicalFileName + ": " + (exception.what()));
		}
	}

	// done
	if (texture != nullptr) texture->acquireReference();

	//
	if (useCache == true) textureCacheMutex->unlock();

	// done
	return texture;
}

Texture* TextureReader::read(const string& texturePathName, const string& textureFileName, const string& transparencyTexturePathName, const string& transparencyTextureFileName, bool useCache, bool powerOfTwo) {
	// make canonical
	auto canonicalFile = FileSystem::getInstance()->getCanonicalPath(texturePathName, textureFileName);
	auto canonicalPathName = FileSystem::getInstance()->getPathName(canonicalFile);
	auto canonicalFileName = FileSystem::getInstance()->getFileName(canonicalFile);
	auto cacheId = canonicalPathName + "/" + canonicalFileName + "/transparency";

	// do cache look up
	if (useCache == true) {
		textureCacheMutex->lock();
		auto textureCacheIt = textureCache->find(cacheId);
		if (textureCacheIt != textureCache->end()) {
			auto texture = textureCacheIt->second;
			texture->acquireReference();
			textureCacheMutex->unlock();
			return texture;
		}
	}

	// load diffuse texture
	auto texture = TextureReader::read(texturePathName, textureFileName, false, powerOfTwo);
	if (texture == nullptr) {
		if (useCache == true) textureCacheMutex->unlock();
		return nullptr;
	}
	// additional transparency texture
	auto transparencyTexture = TextureReader::read(transparencyTexturePathName, transparencyTextureFileName, false, powerOfTwo);
	// do we have one?
	if (transparencyTexture == nullptr) {
		Console::println("TextureReader::read(): transparency texture: failed: " + texturePathName + "/" + textureFileName + ";" + transparencyTexturePathName + "/" + transparencyTextureFileName);
		if (useCache == true) {
			(*textureCache)[cacheId] = texture;
			textureCacheMutex->unlock();
		}
		return texture;
	}
	// same dimensions and supported pixel depth?
	if (texture->getTextureWidth() != transparencyTexture->getTextureWidth() || texture->getTextureHeight() != transparencyTexture->getTextureHeight()) {
		Console::println("TextureReader::read(): texture does not match transparency texture dimension: " + texturePathName + "/" + textureFileName + ";" + transparencyTexturePathName + "/" + transparencyTextureFileName);
		if (useCache == true) {
			(*textureCache)[cacheId] = texture;
			textureCacheMutex->unlock();
		}
		return texture;
	}
	// yep, combine diffuse map + diffuse transparency map
	auto textureWidth = texture->getTextureWidth();
	auto textureHeight = texture->getTextureHeight();
	ByteBuffer* textureByteBuffer = new ByteBuffer(textureWidth * textureHeight * 4);
	auto textureWithTransparency = new Texture(
		texture->getId() + "/transparency",
		32,
		texture->getWidth(),
		texture->getHeight(),
		texture->getTextureWidth(),
		texture->getTextureHeight(),
		textureByteBuffer
	);
	auto diffuseTextureBytesPerPixel = texture->getDepth() / 8;
	auto transparencyTextureBytesPerPixel = transparencyTexture->getDepth() / 8;
	for (auto y = 0; y < textureHeight; y++) {
		for (auto x = 0; x < textureWidth; x++) {
			auto transparencyTextureRed = transparencyTexture->getTextureData()->get((y * textureWidth * transparencyTextureBytesPerPixel) + (x * transparencyTextureBytesPerPixel) + 0);
			auto transparencyTextureGreen = transparencyTexture->getTextureData()->get((y * textureWidth * transparencyTextureBytesPerPixel) + (x * transparencyTextureBytesPerPixel) + 1);
			auto transparencyTextureBlue = transparencyTexture->getTextureData()->get((y * textureWidth * transparencyTextureBytesPerPixel) + (x * transparencyTextureBytesPerPixel) + 2);
			textureByteBuffer->put(texture->getTextureData()->get((y * textureWidth * diffuseTextureBytesPerPixel) + (x * diffuseTextureBytesPerPixel) + 0));
			textureByteBuffer->put(texture->getTextureData()->get((y * textureWidth * diffuseTextureBytesPerPixel) + (x * diffuseTextureBytesPerPixel) + 1));
			textureByteBuffer->put(texture->getTextureData()->get((y * textureWidth * diffuseTextureBytesPerPixel) + (x * diffuseTextureBytesPerPixel) + 2));
			textureByteBuffer->put((uint8_t)((transparencyTextureRed + transparencyTextureGreen + transparencyTextureBlue) * 0.3333f));
		}
	}
	textureWithTransparency->acquireReference();
	if (useCache == true) {
		(*textureCache)[cacheId] = textureWithTransparency;
		textureCacheMutex->unlock();
	}
	texture->releaseReference();
	transparencyTexture->releaseReference();
	return textureWithTransparency;
}

void TextureReader::readPNGDataFromMemory(png_structp png_ptr, png_bytep outBytes, png_size_t outBytesToRead) {
	png_voidp io_ptr = png_get_io_ptr(png_ptr);
	if (io_ptr == nullptr) return;

	PNGInputStream* pngInputStream = (PNGInputStream*)io_ptr;
	pngInputStream->readBytes((int8_t*)outBytes, outBytesToRead);
}

Texture* TextureReader::loadPNG(const string& pathName, const string& fileName, bool powerOfTwo) {
	// see: http://devcry.heiho.net/html/2015/20150517-libpng.html

	// canonical file name for id
	auto canonicalFileName = FileSystem::getInstance()->getCanonicalPath(pathName, fileName);

	// create PNG input stream
	vector<uint8_t> content;
	FileSystem::getInstance()->getContent(pathName, fileName, content);
	PNGInputStream* pngInputStream = new PNGInputStream(&content);

	// check that the PNG signature is in the file header
	unsigned char sig[8];
	pngInputStream->readBytes((int8_t*)sig, sizeof(sig));
	if (png_sig_cmp(sig, 0, 8)) {
		delete pngInputStream;
		return nullptr;
	}

	// create two data structures: 'png_struct' and 'png_info'
	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	if (png == nullptr) {
		delete pngInputStream;
		return nullptr;
	}
	png_infop info = png_create_info_struct(png);
	if (info == nullptr) {
		png_destroy_read_struct(&png, nullptr, nullptr);
		delete pngInputStream;
		return nullptr;
	}

	// set up custom read function
	png_set_read_fn(png, pngInputStream, TextureReader::readPNGDataFromMemory);

	// set libpng error handling mechanism
	if (setjmp(png_jmpbuf(png))) {
		png_destroy_read_struct(&png, &info, nullptr);
		delete pngInputStream;
		return nullptr;
	}

	// tell libpng we already read the signature
	png_set_sig_bytes(png, sizeof(sig));

	// get image information
	png_read_info(png, info);

	// dimensions
	auto width = png_get_image_width(png, info);
	auto height = png_get_image_height(png, info);

	// set one byte per channel
	if (png_get_bit_depth(png, info) < 8) png_set_packing(png);
	if (png_get_bit_depth(png, info) == 16) png_set_strip_16(png);

	// determine bytes per pixel
	auto bytesPerPixel = -1;
	switch(png_get_color_type(png, info)) {
	    case PNG_COLOR_TYPE_GRAY:
			{
				bytesPerPixel = 3;
				png_set_gray_to_rgb(png);
				break;
			}
	    case PNG_COLOR_TYPE_GRAY_ALPHA:
	    	{
	    		bytesPerPixel = 4;
		        png_set_gray_to_rgb(png);
		        break;
	    	}
	    case PNG_COLOR_TYPE_PALETTE:
	    	{
				// if transparency, convert it to alpha
	    		// does not seem to work though
				bool alpha = false;
				if (png_get_valid(png, info, PNG_INFO_tRNS)) {
					alpha = true;
					png_set_tRNS_to_alpha(png);
				}
				bytesPerPixel = alpha?4:3;
				png_set_expand(png);
				break;
	    	}
	    case PNG_COLOR_TYPE_RGB:
			{
				bytesPerPixel = 3;
				break;
			}
	    case PNG_COLOR_TYPE_RGBA:
			{
				bytesPerPixel = 4;
				break;
			}
	    default:
    		{
    			png_destroy_read_struct(&png, &info, nullptr);
    			delete pngInputStream;
    			return nullptr;
    		}
	}

	// ... what ever :DDD
	png_set_interlace_handling(png);
	png_read_update_info(png, info);

	// allocate pixel buffer
	ByteBuffer* pixelByteBuffer = ByteBuffer::allocate(width * height * bytesPerPixel);

	// setup array with row pointers into pixel buffer
	png_bytep* rows = new png_bytep[height];
	uint8_t* p = (uint8_t*)pixelByteBuffer->getBuffer();
	for(auto i = 0; i < height; i++) {
		rows[i] = p;
		p += width * bytesPerPixel;
	}

	// read all rows (data goes into 'pixels' buffer)
	// Note that any decoding errors will jump to the
	// setjmp point and eventually return nullptr
	png_read_image(png, rows);
	png_read_end(png, nullptr);

	//
	delete [] rows;

	// done
	png_destroy_read_struct(&png, &info, nullptr);
	delete pngInputStream;

	// make width, height a power of 2
	auto textureWidth = width;
	auto textureHeight = height;
	if (powerOfTwo == true) {
		textureWidth = 1;
		while (textureWidth < width) textureWidth*= 2;
		textureHeight = 1;
		while (textureHeight < height) textureHeight*= 2;
		if (textureWidth != width || textureHeight != height) {
			Console::println("TextureReader::loadPNG(): " + pathName + "/" + fileName + ": scaling to fit power of 2: " + to_string(width) + "x" + to_string(height) + " --> " + to_string(textureWidth) + "x" + to_string(textureHeight));
			ByteBuffer* pixelByteBufferScaled = ByteBuffer::allocate(textureWidth * textureHeight * bytesPerPixel);
			auto textureYIncrement = (float)textureHeight / (float)height;
			auto textureYPixelRest = 0.0f;
			auto textureY = 0;
			for (auto y = 0; y < height; y++) {
				for (auto i = 0; i < (int)textureYIncrement + (int)textureYPixelRest; i++) {
					scaleTextureLine(pixelByteBuffer, pixelByteBufferScaled, width, textureWidth, bytesPerPixel, y);
					textureY++;
				}
				textureYPixelRest-= (int)textureYPixelRest;
				textureYPixelRest+= textureYIncrement - (int)textureYIncrement;

			}
			while (textureY < textureHeight) {
				scaleTextureLine(pixelByteBuffer, pixelByteBufferScaled, width, textureWidth, bytesPerPixel, height - 1);
				textureY++;
			}
			delete pixelByteBuffer;
			pixelByteBuffer = pixelByteBufferScaled;
		}
	}

	// thats it
	return new Texture(
		canonicalFileName,
		bytesPerPixel * 8,
		width,
		height,
		textureWidth,
		textureHeight,
		pixelByteBuffer
	);
}

void TextureReader::scaleTextureLine(ByteBuffer* pixelByteBuffer, ByteBuffer* pixelByteBufferScaled, int width, int textureWidth, int bytesPerPixel, int y) {
	auto textureXIncrement = (float)textureWidth / (float)width;
	auto textureXPixelRest = 0.0f;
	auto textureX = 0;
	for (auto x = 0; x < width; x++) {
		for (auto i = 0; i < (int)textureXIncrement + (int)textureXPixelRest; i++) {
			for (auto bytePerPixel = 0; bytePerPixel < bytesPerPixel; bytePerPixel++) {
				pixelByteBufferScaled->put(pixelByteBuffer->get((y * width * bytesPerPixel) + (x * bytesPerPixel) + bytePerPixel));
			}
			textureX++;
		}
		textureXPixelRest-= (int)textureXPixelRest;
		textureXPixelRest+= textureXIncrement - (int)textureXIncrement;
	}
	{
		auto x = width - 1;
		while (textureX < textureWidth) {
			for (auto bytePerPixel = 0; bytePerPixel < bytesPerPixel; bytePerPixel++) {
				pixelByteBufferScaled->put(pixelByteBuffer->get((y * width * bytesPerPixel) + (x * bytesPerPixel) + bytePerPixel));
			}
			textureX++;
		}
	}
}

void TextureReader::removeFromCache(Texture* texture) {
	textureCacheMutex->lock();
	auto textureCacheIt = textureCache->find(texture->getId());
	if (textureCacheIt != textureCache->end()) textureCache->erase(textureCacheIt);
	textureCacheMutex->unlock();
}
