#include <tdme/engine/DynamicColorTexture.h>

#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/fileio/textures/Texture.h>
#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/engine/Engine.h>
#include <tdme/utilities/ByteBuffer.h>

using tdme::engine::fileio::textures::Texture;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::DynamicColorTexture;
using tdme::engine::Engine;
using tdme::utilities::ByteBuffer;

uint32_t DynamicColorTexture::counter = 0;

void DynamicColorTexture::initialize()
{
	colorBufferTextureId = Engine::renderer->createColorBufferTexture(width, height, Engine::renderer->ID_NONE, Engine::renderer->ID_NONE);
	auto textureByteBuffer = ByteBuffer::allocate(width * height * 4);
	texture = new Texture(
		"dynamic-color-texture:" + to_string(DynamicColorTexture::counter++),
		32,
		width, height,
		width, height,
		textureByteBuffer
	);
	texture->acquireReference();
	texture->setUseMipMap(false);
	texture->setRepeat(false);
}

void DynamicColorTexture::reshape(int32_t width, int32_t height)
{
	Engine::renderer->resizeColorBufferTexture(colorBufferTextureId, width, height);
	this->width = width;
	this->height = height;
}

void DynamicColorTexture::dispose()
{
	Engine::renderer->disposeTexture(colorBufferTextureId);
	texture->releaseReference();
}

void DynamicColorTexture::update()
{
	Engine::renderer->bindTexture(Engine::renderer->CONTEXTINDEX_DEFAULT, colorBufferTextureId);
	Engine::renderer->uploadTexture(Engine::renderer->CONTEXTINDEX_DEFAULT, texture);
	Engine::renderer->bindTexture(Engine::renderer->CONTEXTINDEX_DEFAULT, Engine::renderer->ID_NONE);
}