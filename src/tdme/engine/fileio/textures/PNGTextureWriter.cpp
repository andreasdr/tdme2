#include <tdme/engine/fileio/textures/PNGTextureWriter.h>

#include <stdlib.h>
#include <stdio.h>

#include <string>

#include <tdme/engine/fileio/textures/Texture.h>
#include <tdme/utilities/ByteBuffer.h>
#include <tdme/utilities/Console.h>

#include <ext/libpng/png.h>

using std::string;
using std::to_string;

using tdme::utilities::ByteBuffer;
using tdme::utilities::Console;

using tdme::engine::fileio::textures::PNGTextureWriter;
using tdme::engine::fileio::textures::Texture;

bool PNGTextureWriter::write(Texture* texture, const string& pathName, const string& fileName, bool removeAlphaChannel, bool flipY) {
	// TODO: use tdme::os::filesystem
	// see: https://gist.github.com/niw/5963798
	FILE *fp = fopen((pathName + "/" + fileName).c_str(), "wb");
	if (!fp) {
		Console::println("PNGTextureWriter::write(): Failed to create file: " + pathName + "/" + fileName);
		return false;
	}

	png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png) {
		fclose(fp);
		return false;
	}

	png_infop info = png_create_info_struct(png);
	if (!info) {
		fclose(fp);
		return false;
	}

	if (setjmp(png_jmpbuf(png))) {
		fclose(fp);
		return false;
	}

	png_init_io(png, fp);

	//
	auto bytesPerPixel = texture->getDepth() / 8;
	auto width = texture->getTextureWidth();
	auto height = texture->getTextureHeight();
	auto pixels = texture->getTextureData();

	// output is 8bit depth, RGBA format.
	png_set_IHDR(
		png,
		info,
		width,
		height,
		8,
		removeAlphaChannel == false && bytesPerPixel == 4?PNG_COLOR_TYPE_RGBA:PNG_COLOR_TYPE_RGB,
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT
	);
	png_write_info(png, info);

	// remove the alpha channel for PNG_COLOR_TYPE_RGB format
	if (removeAlphaChannel == true) {
		png_set_filler(png, 0, PNG_FILLER_AFTER);
	}

	//
	png_bytep* row_pointers = new png_bytep[height];
	if (flipY == true) {
		for (auto y = 0; y < height; y++) row_pointers[y] = pixels->getBuffer() + width * bytesPerPixel * (height - 1 - y);
	} else {
		for (auto y = 0; y < height; y++) row_pointers[y] = pixels->getBuffer() + width * bytesPerPixel * y;
	}

	png_write_image(png, row_pointers);
	png_write_end(png, NULL);

	free (row_pointers);

	fclose(fp);

	png_destroy_write_struct(&png, &info);

	return true;
}