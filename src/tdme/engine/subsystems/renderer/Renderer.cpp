#include <tdme/engine/subsystems/renderer/Renderer.h>

#include <string>

#include <tdme/math/Math.h>
#include <tdme/engine/fileio/textures/Texture.h>
#include <tdme/engine/subsystems/renderer/Renderer_Light.h>
#include <tdme/engine/subsystems/renderer/Renderer_SpecularMaterial.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/utilities/ByteBuffer.h>

using std::string;
using std::to_string;

using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::fileio::textures::Texture;
using tdme::engine::subsystems::renderer::Renderer_Light;
using tdme::engine::subsystems::renderer::Renderer_SpecularMaterial;
using tdme::math::Math;
using tdme::math::Matrix4x4;
using tdme::utilities::ByteBuffer;

Renderer::Renderer()
{
	ID_NONE = -1;
	CLEAR_DEPTH_BUFFER_BIT = -1;
	CLEAR_COLOR_BUFFER_BIT = -1;
	CULLFACE_FRONT = -1;
	CULLFACE_BACK = -1;
	TEXTUREUNITS_MAX = -1;
	PROGRAM_OBJECTS = 1;
	PROGRAM_POINTS = 2;
	PROGRAM_LINES = 3;
	PROGRAM_COMPUTE = 4;
	SHADER_FRAGMENT_SHADER = -1;
	SHADER_VERTEX_SHADER = -1;
	SHADER_GEOMETRY_SHADER = -1;
	SHADER_COMPUTE_SHADER = -1;
	DEPTHFUNCTION_ALWAYS = -1;
	DEPTHFUNCTION_EQUAL = -1;
	DEPTHFUNCTION_LESSEQUAL = -1;
	DEPTHFUNCTION_GREATEREQUAL = -1;
	FRAMEBUFFER_DEFAULT = -1;
	FRONTFACE_CW = -1;
	FRONTFACE_CCW = -1;
	LIGHTING_NONE = 0;
	LIGHTING_SPECULAR = 1;
	LIGHTING_PBR = 2;
	UNIFORM_CL_SKINNING_VERTEX_COUNT = -1;
	UNIFORM_CL_SKINNING_MATRIX_COUNT = -1;
	UNIFORM_CL_SKINNING_INSTANCE_COUNT = -1;
	viewPortX = 0;
	viewPortY = 0;
	viewPortWidth = 0;
	viewPortHeight = 0;
	TEXTUREUNITS_MAX = 2;
	projectionMatrix.identity();
	cameraMatrix.identity();
	modelViewMatrix.identity();
	viewportMatrix.identity();
	effectPass = 0;
}

Renderer::~Renderer() {
}

void* Renderer::getDefaultContext() {
	return nullptr;
}

void* Renderer::getContext(int contextIdx) {
	return nullptr;
}

int Renderer::getContextIndex(void* context) {
	return 0;
}

Vector3& Renderer::getCameraPosition() {
	return cameraPosition;
}

Matrix4x4& Renderer::getProjectionMatrix()
{
	return projectionMatrix;
}

Matrix4x4& Renderer::getCameraMatrix()
{
	return cameraMatrix;
}

Matrix4x4& Renderer::getModelViewMatrix()
{
	return modelViewMatrix;
}

Matrix4x4& Renderer::getViewportMatrix()
{
	return viewportMatrix;
}

int32_t Renderer::getViewPortWidth() {
	return viewPortWidth;
}

int32_t Renderer::getViewPortHeight() {
	return viewPortHeight;
}

void Renderer::initializeFrame() {
}

int32_t Renderer::getEffectPass() {
	return effectPass;
}

void Renderer::setEffectPass(int32_t effectPass) {
	this->effectPass = effectPass;
}

const string& Renderer::getShaderPrefix() {
	return shaderPrefix;
}

void Renderer::setShaderPrefix(const string& shaderPrefix) {
	this->shaderPrefix = shaderPrefix;
}

Texture* Renderer::generateMipMap(const string& id, Texture* texture, int32_t level, int32_t atlasBorderSize) {
	// TODO: bilinear filtering
	auto generatedTextureWidth = texture->getTextureWidth() / 2;
	auto generatedTextureHeight = texture->getTextureHeight() / 2;
	auto generatedTextureByteBuffer = ByteBuffer::allocate(generatedTextureWidth * generatedTextureHeight * 4);
	auto atlasTextureSize = texture->getWidth() / texture->getAtlasSize();
	auto materialTextureWidth = texture->getTextureWidth() / texture->getAtlasSize();
	auto materialTextureHeight = texture->getTextureHeight() / texture->getAtlasSize();
	auto materialTextureBytesPerPixel = texture->getDepth() / 8;
	for (auto y = 0; y < generatedTextureHeight; y++)
	for (auto x = 0; x < generatedTextureWidth; x++) {
		auto atlasTextureIdxX = (x * 2) / atlasTextureSize;
		auto atlasTextureIdxY = (y * 2) / atlasTextureSize;
		auto materialTextureX = (x * 2) - (atlasTextureIdxX * atlasTextureSize);
		auto materialTextureY = (y * 2) - (atlasTextureIdxY * atlasTextureSize);
		auto materialTextureXFloat = static_cast<float>(materialTextureX) / static_cast<float>(atlasTextureSize);
		auto materialTextureYFloat = static_cast<float>(materialTextureY) / static_cast<float>(atlasTextureSize);
		{
			auto materialSamples = 0;
			auto materialTextureXInt = static_cast<int>(materialTextureXFloat * static_cast<float>(materialTextureWidth));
			auto materialTextureYInt = static_cast<int>(materialTextureYFloat * static_cast<float>(materialTextureHeight));
			auto materialPixelR = 0;
			auto materialPixelG = 0;
			auto materialPixelB = 0;
			auto materialPixelA = 0;
			for (auto y = -1; y <= 1; y++)
			for (auto x = -1; x <= 1; x++)
			if ((Math::abs(x) == 1 && Math::abs(y) == 1) == false &&
				materialTextureXInt + x >= 0 && materialTextureXInt + x < materialTextureWidth &&
				materialTextureYInt + y >= 0 && materialTextureYInt + y < materialTextureHeight) {
				auto materialTexturePixelOffset =
					(atlasTextureIdxY * materialTextureHeight + materialTextureYInt + y) * texture->getTextureWidth() * materialTextureBytesPerPixel +
					(atlasTextureIdxX * materialTextureWidth + materialTextureXInt + x) * materialTextureBytesPerPixel;
				materialPixelR+= texture->getTextureData()->get(materialTexturePixelOffset + 0);
				materialPixelG+= texture->getTextureData()->get(materialTexturePixelOffset + 1);
				materialPixelB+= texture->getTextureData()->get(materialTexturePixelOffset + 2);
				materialPixelA+= materialTextureBytesPerPixel == 4?texture->getTextureData()->get(materialTexturePixelOffset + 3):0xff;
				materialSamples++;
			}
			generatedTextureByteBuffer->put(materialPixelR / materialSamples);
			generatedTextureByteBuffer->put(materialPixelG / materialSamples);
			generatedTextureByteBuffer->put(materialPixelB / materialSamples);
			generatedTextureByteBuffer->put(materialPixelA / materialSamples);
		}
	}
	auto generatedTexture = new Texture(
		id + ".mipmap." + to_string(level),
		32,
		generatedTextureWidth,
		generatedTextureHeight,
		generatedTextureWidth,
		generatedTextureHeight,
		generatedTextureByteBuffer
	);
	generatedTexture->setAtlasSize(texture->getAtlasSize());
	generatedTexture->acquireReference();
	return generatedTexture;
}