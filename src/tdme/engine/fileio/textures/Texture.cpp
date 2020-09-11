#include <tdme/engine/fileio/textures/Texture.h>
#include <tdme/engine/fileio/textures/TextureReader.h>

#include <tdme/utilities/ByteBuffer.h>

using tdme::engine::fileio::textures::Texture;
using tdme::engine::fileio::textures::TextureReader;

using tdme::utilities::ByteBuffer;

Texture::~Texture() {
	delete textureData;
}

void Texture::onDelete() {
	TextureReader::removeFromCache(this);
}
