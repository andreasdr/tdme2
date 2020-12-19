#include <tdme/engine/model/PBRMaterialProperties.h>

#include <string>

#include <tdme/engine/fileio/textures/Texture.h>
#include <tdme/engine/fileio/textures/TextureReader.h>
#include <tdme/engine/model/Color4.h>

using std::string;

using tdme::engine::fileio::textures::Texture;
using tdme::engine::fileio::textures::TextureReader;
using tdme::engine::model::Color4;
using tdme::engine::model::PBRMaterialProperties;

PBRMaterialProperties::PBRMaterialProperties()
{
	baseColorFactor.set(1.0f, 1.0f, 1.0f, 1.0f);
	baseColorTexture = nullptr;
	baseColorTextureTransparency = false;
	baseColorTextureMaskedTransparency = false;
	baseColorTextureMaskedTransparencyThreshold = 0.1f;
	metallicFactor = 1.0f;
	roughnessFactor = 1.0f;
	metallicRoughnessTexture = nullptr;
	normalScale = 1.0f;
	normalTexture = nullptr;
	exposure = 1.0f;
}

PBRMaterialProperties::~PBRMaterialProperties() {
	if (baseColorTexture != nullptr) baseColorTexture->releaseReference();
	if (metallicRoughnessTexture != nullptr) metallicRoughnessTexture->releaseReference();
	if (normalTexture != nullptr) normalTexture->releaseReference();
}

void PBRMaterialProperties::setBaseColorTexture(const string& pathName, const string& fileName)
{
	if (baseColorTexture != nullptr) baseColorTexture->releaseReference();
	baseColorTexturePathName = pathName;
	baseColorTextureFileName = fileName;
	baseColorTexture = TextureReader::read(pathName, fileName);
	checkBaseColorTextureTransparency();
}

void PBRMaterialProperties::setMetallicRoughnessTexture(const string& pathName, const string& fileName)
{
	if (metallicRoughnessTexture != nullptr) metallicRoughnessTexture->releaseReference();
	metallicRoughnessTexturePathName = pathName;
	metallicRoughnessTextureFileName = fileName;
	metallicRoughnessTexture = TextureReader::read(pathName, fileName);
}

void PBRMaterialProperties::setNormalTexture(const string& pathName, const string& fileName)
{
	if (normalTexture != nullptr) normalTexture->releaseReference();
	normalTexturePathName = pathName;
	normalTextureFileName = fileName;
	normalTexture = TextureReader::read(pathName, fileName);
}

void PBRMaterialProperties::checkBaseColorTextureTransparency()
{
	baseColorTextureTransparency = false;
	auto baseColorTextureMaskedTransparencyTest = true;
	if (baseColorTexture != nullptr && baseColorTexture->getDepth() == 32) {
		auto textureData = baseColorTexture->getTextureData();
		for (auto i = 0; i < baseColorTexture->getTextureWidth() * baseColorTexture->getTextureHeight(); i++) {
			auto alpha = textureData->get(i * 4 + 3);
			if (alpha != 255) baseColorTextureTransparency = true;
			if (alpha > 5 || alpha < 250) baseColorTextureMaskedTransparencyTest = false;
		}
	}
	if (baseColorTextureMaskedTransparency == false) baseColorTextureMaskedTransparency = baseColorTextureMaskedTransparencyTest;
}
