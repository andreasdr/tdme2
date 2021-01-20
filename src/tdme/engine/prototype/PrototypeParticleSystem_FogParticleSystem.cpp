#include <tdme/engine/prototype/PrototypeParticleSystem_FogParticleSystem.h>

#include <string>

#include <tdme/engine/fileio/textures/Texture.h>
#include <tdme/engine/fileio/textures/TextureReader.h>
#include <tdme/tools/shared/tools/Tools.h>

using std::string;

using tdme::engine::fileio::textures::Texture;
using tdme::engine::fileio::textures::TextureReader;
using tdme::tools::shared::tools::Tools;

using tdme::engine::prototype::PrototypeParticleSystem_FogParticleSystem;

PrototypeParticleSystem_FogParticleSystem::PrototypeParticleSystem_FogParticleSystem()
{
}

void PrototypeParticleSystem_FogParticleSystem::setTextureFileName(const string& textureFileName, const string& transparencyTextureFileName) {
	if (texture != nullptr) texture->releaseReference();;
	texture = nullptr;
	this->textureFileName = textureFileName;
	this->transparencyTextureFileName = transparencyTextureFileName;
	if (this->transparencyTextureFileName.size() > 0) {
		texture = TextureReader::read(
			Tools::getPathName(this->textureFileName),
			Tools::getFileName(this->textureFileName),
			Tools::getPathName(this->transparencyTextureFileName),
			Tools::getFileName(this->transparencyTextureFileName)
		);
	} else {
		texture = TextureReader::read(
			Tools::getPathName(this->textureFileName),
			Tools::getFileName(this->textureFileName)
		);
	}
}

PrototypeParticleSystem_FogParticleSystem::~PrototypeParticleSystem_FogParticleSystem() {
	if (texture != nullptr) texture->releaseReference();;
}
