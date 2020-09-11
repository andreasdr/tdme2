#include <tdme/engine/subsystems/shadowmapping/ShadowMappingShaderPre.h>

#include <tdme/engine/Engine.h>
#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/engine/subsystems/shadowmapping/ShadowMappingShaderPreDefaultImplementation.h>
#include <tdme/engine/subsystems/shadowmapping/ShadowMappingShaderPreFoliageImplementation.h>
#include <tdme/engine/subsystems/shadowmapping/ShadowMappingShaderPreImplementation.h>
#include <tdme/engine/subsystems/shadowmapping/ShadowMappingShaderPreTreeImplementation.h>
#include <tdme/math/Matrix4x4.h>

using tdme::engine::subsystems::shadowmapping::ShadowMappingShaderPre;
using tdme::engine::Engine;
using tdme::engine::subsystems::shadowmapping::ShadowMappingShaderPreBaseImplementation;
using tdme::engine::subsystems::shadowmapping::ShadowMappingShaderPreDefaultImplementation;
using tdme::engine::subsystems::shadowmapping::ShadowMappingShaderPreFoliageImplementation;
using tdme::engine::subsystems::shadowmapping::ShadowMappingShaderPreImplementation;
using tdme::engine::subsystems::shadowmapping::ShadowMappingShaderPreTreeImplementation;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::math::Matrix4x4;
using tdme::utilities::Console;

ShadowMappingShaderPre::ShadowMappingShaderPre(Renderer* renderer): renderer(renderer)
{
	if (ShadowMappingShaderPreDefaultImplementation::isSupported(renderer) == true) shader["default"] = new ShadowMappingShaderPreDefaultImplementation(renderer);
	if (ShadowMappingShaderPreFoliageImplementation::isSupported(renderer) == true) shader["foliage"] = new ShadowMappingShaderPreFoliageImplementation(renderer);
	if (ShadowMappingShaderPreTreeImplementation::isSupported(renderer) == true) shader["tree"] = new ShadowMappingShaderPreTreeImplementation(renderer);
	auto threadCount = renderer->isSupportingMultithreadedRendering() == true?Engine::getThreadCount():1;
	contexts.resize(threadCount);
}

ShadowMappingShaderPre::~ShadowMappingShaderPre() {
	for (auto shaderIt: shader) {
		delete shaderIt.second;
	}
}

bool ShadowMappingShaderPre::isInitialized()
{
	bool initialized = true;
	for (auto shaderIt: shader) {
		initialized&= shaderIt.second->isInitialized();
	}
	return initialized;
}

void ShadowMappingShaderPre::initialize()
{
	for (auto shaderIt: shader) {
		shaderIt.second->initialize();
	}
}

void ShadowMappingShaderPre::useProgram(Engine* engine)
{
	running = true;
	this->engine = engine;
}

void ShadowMappingShaderPre::unUseProgram()
{
	running = false;
	auto i = 0;
	for (auto& shadowMappingShaderPreContext: contexts) {
		if (shadowMappingShaderPreContext.implementation != nullptr) {
			shadowMappingShaderPreContext.implementation->unUseProgram(renderer->getContext(i));
		}
		shadowMappingShaderPreContext.implementation = nullptr;
		i++;
	}
	engine = nullptr;
}

void ShadowMappingShaderPre::updateMatrices(void* context)
{
	auto& shadowMappingShaderPreContext = contexts[renderer->getContextIndex(context)];
	if (shadowMappingShaderPreContext.implementation == nullptr) return;
	shadowMappingShaderPreContext.implementation->updateMatrices(context);
}

void ShadowMappingShaderPre::updateTextureMatrix(void* context) {
	auto& shadowMappingShaderPreContext = contexts[renderer->getContextIndex(context)];
	if (shadowMappingShaderPreContext.implementation == nullptr) return;
	shadowMappingShaderPreContext.implementation->updateTextureMatrix(renderer, context);
}

void ShadowMappingShaderPre::updateMaterial(void* context)
{
	auto& shadowMappingShaderPreContext = contexts[renderer->getContextIndex(context)];
	if (shadowMappingShaderPreContext.implementation == nullptr) return;
	shadowMappingShaderPreContext.implementation->updateMaterial(renderer, context);
}

void ShadowMappingShaderPre::bindTexture(void* context, int32_t textureId)
{
	auto& shadowMappingShaderPreContext = contexts[renderer->getContextIndex(context)];
	if (shadowMappingShaderPreContext.implementation == nullptr) return;
	shadowMappingShaderPreContext.implementation->bindTexture(renderer, context, textureId);
}

void ShadowMappingShaderPre::setShader(void* context, const string& id) {
	auto& shadowMappingShaderPreContext = contexts[renderer->getContextIndex(context)];
	auto currentImplementation = shadowMappingShaderPreContext.implementation;

	auto shaderIt = shader.find(id);
	if (shaderIt == shader.end()) {
		shaderIt = shader.find("default");
	}
	shadowMappingShaderPreContext.implementation = shaderIt->second;

	if (currentImplementation != shadowMappingShaderPreContext.implementation) {
		if (currentImplementation != nullptr) currentImplementation->unUseProgram(context);
		shadowMappingShaderPreContext.implementation->useProgram(engine, context);
	}
}
