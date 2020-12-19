#include <tdme/engine/subsystems/shadowmapping/ShadowMapCreationShader.h>

#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/engine/subsystems/shadowmapping/ShadowMapCreationShaderDefaultImplementation.h>
#include <tdme/engine/subsystems/shadowmapping/ShadowMapCreationShaderFoliageImplementation.h>
#include <tdme/engine/subsystems/shadowmapping/ShadowMapCreationShaderImplementation.h>
#include <tdme/engine/subsystems/shadowmapping/ShadowMapCreationShaderTreeImplementation.h>
#include <tdme/engine/Engine.h>
#include <tdme/math/Matrix4x4.h>

using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::subsystems::shadowmapping::ShadowMapCreationShader;
using tdme::engine::subsystems::shadowmapping::ShadowMapCreationShaderBaseImplementation;
using tdme::engine::subsystems::shadowmapping::ShadowMapCreationShaderDefaultImplementation;
using tdme::engine::subsystems::shadowmapping::ShadowMapCreationShaderFoliageImplementation;
using tdme::engine::subsystems::shadowmapping::ShadowMapCreationShaderImplementation;
using tdme::engine::subsystems::shadowmapping::ShadowMapCreationShaderTreeImplementation;
using tdme::engine::Engine;
using tdme::math::Matrix4x4;
using tdme::utilities::Console;

ShadowMapCreationShader::ShadowMapCreationShader(Renderer* renderer): renderer(renderer)
{
	if (ShadowMapCreationShaderDefaultImplementation::isSupported(renderer) == true) shader["default"] = new ShadowMapCreationShaderDefaultImplementation(renderer);
	if (ShadowMapCreationShaderFoliageImplementation::isSupported(renderer) == true) shader["foliage"] = new ShadowMapCreationShaderFoliageImplementation(renderer);
	if (ShadowMapCreationShaderTreeImplementation::isSupported(renderer) == true) shader["tree"] = new ShadowMapCreationShaderTreeImplementation(renderer);
	auto threadCount = renderer->isSupportingMultithreadedRendering() == true?Engine::getThreadCount():1;
	contexts.resize(threadCount);
}

ShadowMapCreationShader::~ShadowMapCreationShader() {
	for (auto shaderIt: shader) {
		delete shaderIt.second;
	}
}

bool ShadowMapCreationShader::isInitialized()
{
	bool initialized = true;
	for (auto shaderIt: shader) {
		initialized&= shaderIt.second->isInitialized();
	}
	return initialized;
}

void ShadowMapCreationShader::initialize()
{
	for (auto shaderIt: shader) {
		shaderIt.second->initialize();
	}
}

void ShadowMapCreationShader::useProgram(Engine* engine)
{
	running = true;
	this->engine = engine;
}

void ShadowMapCreationShader::unUseProgram()
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

void ShadowMapCreationShader::updateMatrices(void* context)
{
	auto& shadowMappingShaderPreContext = contexts[renderer->getContextIndex(context)];
	if (shadowMappingShaderPreContext.implementation == nullptr) return;
	shadowMappingShaderPreContext.implementation->updateMatrices(context);
}

void ShadowMapCreationShader::updateTextureMatrix(void* context) {
	auto& shadowMappingShaderPreContext = contexts[renderer->getContextIndex(context)];
	if (shadowMappingShaderPreContext.implementation == nullptr) return;
	shadowMappingShaderPreContext.implementation->updateTextureMatrix(renderer, context);
}

void ShadowMapCreationShader::updateMaterial(void* context)
{
	auto& shadowMappingShaderPreContext = contexts[renderer->getContextIndex(context)];
	if (shadowMappingShaderPreContext.implementation == nullptr) return;
	shadowMappingShaderPreContext.implementation->updateMaterial(renderer, context);
}

void ShadowMapCreationShader::bindTexture(void* context, int32_t textureId)
{
	auto& shadowMappingShaderPreContext = contexts[renderer->getContextIndex(context)];
	if (shadowMappingShaderPreContext.implementation == nullptr) return;
	shadowMappingShaderPreContext.implementation->bindTexture(renderer, context, textureId);
}

void ShadowMapCreationShader::setShader(void* context, const string& id) {
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
