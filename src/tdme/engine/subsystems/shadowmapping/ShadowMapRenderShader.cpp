#include <tdme/engine/subsystems/shadowmapping/ShadowMapRenderShader.h>

#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/engine/subsystems/shadowmapping/ShadowMapRenderShaderDefaultImplementation.h>
#include <tdme/engine/subsystems/shadowmapping/ShadowMapRenderShaderFoliageImplementation.h>
#include <tdme/engine/subsystems/shadowmapping/ShadowMapRenderShaderImplementation.h>
#include <tdme/engine/subsystems/shadowmapping/ShadowMapRenderShaderTreeImplementation.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Timing.h>
#include <tdme/math/Matrix4x4.h>

using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::subsystems::shadowmapping::ShadowMapRenderShader;
using tdme::engine::subsystems::shadowmapping::ShadowMapRenderShaderDefaultImplementation;
using tdme::engine::subsystems::shadowmapping::ShadowMapRenderShaderFoliageImplementation;
using tdme::engine::subsystems::shadowmapping::ShadowMapRenderShaderImplementation;
using tdme::engine::subsystems::shadowmapping::ShadowMapRenderShaderTreeImplementation;
using tdme::engine::Engine;
using tdme::engine::Timing;
using tdme::math::Matrix4x4;

ShadowMapRenderShader::ShadowMapRenderShader(Renderer* renderer): renderer(renderer)
{
	if (ShadowMapRenderShaderDefaultImplementation::isSupported(renderer) == true) shader["default"] = new ShadowMapRenderShaderDefaultImplementation(renderer);
	if (ShadowMapRenderShaderFoliageImplementation::isSupported(renderer) == true) shader["foliage"] = new ShadowMapRenderShaderFoliageImplementation(renderer);
	if (ShadowMapRenderShaderTreeImplementation::isSupported(renderer) == true) shader["tree"] = new ShadowMapRenderShaderTreeImplementation(renderer);
	auto threadCount = renderer->isSupportingMultithreadedRendering() == true?Engine::getThreadCount():1;
	contexts.resize(threadCount);
}

ShadowMapRenderShader::~ShadowMapRenderShader()
{
	for (auto shaderIt: shader) {
		delete shaderIt.second;
	}
}

bool ShadowMapRenderShader::isInitialized()
{
	bool initialized = true;
	for (auto shaderIt: shader) {
		initialized&= shaderIt.second->isInitialized();
	}
	return initialized;
}

void ShadowMapRenderShader::initialize()
{
	for (auto shaderIt: shader) {
		shaderIt.second->initialize();
	}
}

void ShadowMapRenderShader::useProgram(Engine* engine)
{
	running = true;
	this->engine = engine;
}

void ShadowMapRenderShader::unUseProgram()
{
	running = false;
	auto i = 0;
	for (auto& shadowMappingShaderRenderContext: contexts) {
		if (shadowMappingShaderRenderContext.implementation != nullptr) {
			shadowMappingShaderRenderContext.implementation->unUseProgram(renderer->getContext(i));
		}
		shadowMappingShaderRenderContext.implementation = nullptr;
		i++;
	}
	engine = nullptr;
}

void ShadowMapRenderShader::updateMatrices(void* context)
{
	auto& shadowMappingShaderRenderContext = contexts[renderer->getContextIndex(context)];
	if (shadowMappingShaderRenderContext.implementation == nullptr) return;
	shadowMappingShaderRenderContext.implementation->updateMatrices(context);
}

void ShadowMapRenderShader::updateTextureMatrix(void* context) {
	auto& shadowMappingShaderRenderContext = contexts[renderer->getContextIndex(context)];
	if (shadowMappingShaderRenderContext.implementation == nullptr) return;
	shadowMappingShaderRenderContext.implementation->updateTextureMatrix(renderer, context);
}

void ShadowMapRenderShader::updateMaterial(void* context)
{
	auto& shadowMappingShaderRenderContext = contexts[renderer->getContextIndex(context)];
	if (shadowMappingShaderRenderContext.implementation == nullptr) return;
	shadowMappingShaderRenderContext.implementation->updateMaterial(renderer, context);
}

void ShadowMapRenderShader::updateLight(void* context, int32_t lightId) {
	auto& shadowMappingShaderRenderContext = contexts[renderer->getContextIndex(context)];
	if (shadowMappingShaderRenderContext.implementation == nullptr) return;
	shadowMappingShaderRenderContext.implementation->updateLight(renderer, context, lightId);
}

void ShadowMapRenderShader::updateShaderParameters(void* context) {
	auto& shadowMappingShaderRenderContext = contexts[renderer->getContextIndex(context)];
	if (shadowMappingShaderRenderContext.implementation == nullptr) return;
	shadowMappingShaderRenderContext.implementation->updateShaderParameters(renderer, context);
}

void ShadowMapRenderShader::bindTexture(void* context, int32_t textureId)
{
	auto& shadowMappingShaderRenderContext = contexts[renderer->getContextIndex(context)];
	if (shadowMappingShaderRenderContext.implementation == nullptr) return;
	shadowMappingShaderRenderContext.implementation->bindTexture(renderer, context, textureId);
}

void ShadowMapRenderShader::setDepthBiasMVPMatrix(void* context, const Matrix4x4& depthBiasMVPMatrix)
{
	this->depthBiasMVPMatrix = depthBiasMVPMatrix;
	auto& shadowMappingShaderRenderContext = contexts[renderer->getContextIndex(context)];
	if (shadowMappingShaderRenderContext.implementation == nullptr) return;
	shadowMappingShaderRenderContext.implementation->setDepthBiasMVPMatrix(context, this->depthBiasMVPMatrix);
}

void ShadowMapRenderShader::setRenderLightId(int32_t lightId) {
	this->lightId = lightId;
}

void ShadowMapRenderShader::setShader(void* context, const string& id) {
	auto& shadowMappingShaderRenderContext = contexts[renderer->getContextIndex(context)];
	auto currentImplementation = shadowMappingShaderRenderContext.implementation;
	auto shaderIt = shader.find(id);
	if (shaderIt == shader.end()) {
		shaderIt = shader.find("default");
	}
	auto nextImplementation = shaderIt->second;
	if (currentImplementation != nextImplementation) {
		if (currentImplementation != nullptr) currentImplementation->unUseProgram(context);
		shadowMappingShaderRenderContext.implementation = nextImplementation;
		shadowMappingShaderRenderContext.implementation->useProgram(engine, context);
	}

	shadowMappingShaderRenderContext.implementation->setDepthBiasMVPMatrix(context, depthBiasMVPMatrix);
	shadowMappingShaderRenderContext.implementation->setRenderLightId(lightId);
}
