#include <tdme/engine/subsystems/lighting/LightingShader.h>

#include <tdme/engine/subsystems/lighting/LightingShaderDefaultImplementation.h>
#include <tdme/engine/subsystems/lighting/LightingShaderFoliageImplementation.h>
#include <tdme/engine/subsystems/lighting/LightingShaderImplementation.h>
#include <tdme/engine/subsystems/lighting/LightingShaderLightScatteringDefaultImplementation.h>
#include <tdme/engine/subsystems/lighting/LightingShaderLightScatteringFoliageImplementation.h>
#include <tdme/engine/subsystems/lighting/LightingShaderLightScatteringTreeImplementation.h>
#include <tdme/engine/subsystems/lighting/LightingShaderPBRDefaultImplementation.h>
#include <tdme/engine/subsystems/lighting/LightingShaderSkyImplementation.h>
#include <tdme/engine/subsystems/lighting/LightingShaderTerrainEditorImplementation.h>
#include <tdme/engine/subsystems/lighting/LightingShaderTerrainImplementation.h>
#include <tdme/engine/subsystems/lighting/LightingShaderTreeImplementation.h>
#include <tdme/engine/subsystems/lighting/LightingShaderWaterImplementation.h>
#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/StringTools.h>

using tdme::engine::subsystems::lighting::LightingShader;
using tdme::engine::subsystems::lighting::LightingShaderDefaultImplementation;
using tdme::engine::subsystems::lighting::LightingShaderFoliageImplementation;
using tdme::engine::subsystems::lighting::LightingShaderImplementation;
using tdme::engine::subsystems::lighting::LightingShaderLightScatteringDefaultImplementation;
using tdme::engine::subsystems::lighting::LightingShaderPBRDefaultImplementation;
using tdme::engine::subsystems::lighting::LightingShaderSkyImplementation;
using tdme::engine::subsystems::lighting::LightingShaderTerrainEditorImplementation;
using tdme::engine::subsystems::lighting::LightingShaderTerrainImplementation;
using tdme::engine::subsystems::lighting::LightingShaderTreeImplementation;
using tdme::engine::subsystems::lighting::LightingShaderWaterImplementation;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::utilities::Console;
using tdme::utilities::StringTools;

LightingShader::LightingShader(Renderer* renderer): renderer(renderer)
{
	if (LightingShaderDefaultImplementation::isSupported(renderer) == true) { auto shaderProgram = new LightingShaderDefaultImplementation(renderer); shader[shaderProgram->getId()] = shaderProgram; }
	if (LightingShaderFoliageImplementation::isSupported(renderer) == true) { auto shaderProgram = new LightingShaderFoliageImplementation(renderer); shader[shaderProgram->getId()] = shaderProgram; }
	if (LightingShaderLightScatteringDefaultImplementation::isSupported(renderer) == true) { auto shaderProgram = new LightingShaderLightScatteringDefaultImplementation(renderer); shader[shaderProgram->getId()] = shaderProgram; }
	if (LightingShaderLightScatteringFoliageImplementation::isSupported(renderer) == true) { auto shaderProgram = new LightingShaderLightScatteringFoliageImplementation(renderer); shader[shaderProgram->getId()] = shaderProgram; }
	if (LightingShaderLightScatteringTreeImplementation::isSupported(renderer) == true) { auto shaderProgram = new LightingShaderLightScatteringTreeImplementation(renderer); shader[shaderProgram->getId()] = shaderProgram; }
	if (LightingShaderSkyImplementation::isSupported(renderer) == true) { auto shaderProgram = new LightingShaderSkyImplementation(renderer); shader[shaderProgram->getId()] = shaderProgram; }
	if (LightingShaderTerrainEditorImplementation::isSupported(renderer) == true) { auto shaderProgram = new LightingShaderTerrainEditorImplementation(renderer); shader[shaderProgram->getId()] = shaderProgram; }
	if (LightingShaderTerrainImplementation::isSupported(renderer) == true) { auto shaderProgram = new LightingShaderTerrainImplementation(renderer); shader[shaderProgram->getId()] = shaderProgram; }
	if (LightingShaderTreeImplementation::isSupported(renderer) == true) { auto shaderProgram = new LightingShaderTreeImplementation(renderer); shader[shaderProgram->getId()] = shaderProgram; }
	if (LightingShaderWaterImplementation::isSupported(renderer) == true) { auto shaderProgram = new LightingShaderWaterImplementation(renderer); shader[shaderProgram->getId()] = shaderProgram; }
	#if !defined(VULKAN)
		// TODO: VUlkan
		if (LightingShaderPBRDefaultImplementation::isSupported(renderer) == true) { auto shaderProgram = new LightingShaderPBRDefaultImplementation(renderer); shader[shaderProgram->getId()] = shaderProgram; }
	#endif
	auto threadCount = renderer->isSupportingMultithreadedRendering() == true?Engine::getThreadCount():1;
	contexts.resize(threadCount);
}

LightingShader::~LightingShader() {
	for (auto shaderIt: shader) {
		delete shaderIt.second;
	}
}

bool LightingShader::isInitialized()
{
	bool initialized = true;
	for (auto shaderIt: shader) {
		if (shaderIt.second->isInitialized() == false) {
			Console::println("LightingShader::isInitialized(): " + shaderIt.first + ": not initialized!");
		} else {
			shaderIt.second->registerShader();
		}
		initialized&= shaderIt.second->isInitialized();
	}
	return initialized;
}

void LightingShader::initialize()
{
	for (auto shaderIt: shader) {
		shaderIt.second->initialize();
	}
}

void LightingShader::useProgram(Engine* engine)
{
	running = true;
	this->engine = engine;
}

void LightingShader::unUseProgram()
{
	running = false;
	auto i = 0;
	for (auto& lightingShaderContext: contexts) {
		if (lightingShaderContext.implementation != nullptr) lightingShaderContext.implementation->unUseProgram(renderer->getContext(i));
		lightingShaderContext.implementation = nullptr;
		i++;
	}
	engine = nullptr;
}

void LightingShader::updateEffect(void* context)
{
	auto& lightingShaderContext = contexts[renderer->getContextIndex(context)];
	if (lightingShaderContext.implementation == nullptr) return;
	lightingShaderContext.implementation->updateEffect(renderer, context);
}

void LightingShader::updateMaterial(void* context)
{
	auto& lightingShaderContext = contexts[renderer->getContextIndex(context)];
	if (lightingShaderContext.implementation == nullptr) return;
	lightingShaderContext.implementation->updateMaterial(renderer, context);
}

void LightingShader::updateLight(void* context, int32_t lightId)
{
	auto& lightingShaderContext = contexts[renderer->getContextIndex(context)];
	if (lightingShaderContext.implementation == nullptr) return;
	lightingShaderContext.implementation->updateLight(renderer, context, lightId);
}

void LightingShader::updateMatrices(void* context)
{
	auto& lightingShaderContext = contexts[renderer->getContextIndex(context)];
	if (lightingShaderContext.implementation == nullptr) return;
	lightingShaderContext.implementation->updateMatrices(renderer, context);
}

void LightingShader::updateTextureMatrix(void* context) {
	auto& lightingShaderContext = contexts[renderer->getContextIndex(context)];
	if (lightingShaderContext.implementation == nullptr) return;
	lightingShaderContext.implementation->updateTextureMatrix(renderer, context);
}

void LightingShader::setShader(void* context, const string& id) {
	if (running == false) return;
	auto shaderId = id;
	if (renderer->isPBRAvailable() == false && StringTools::startsWith(id, "pbr-") == true) {
		shaderId = StringTools::substring(id, 4, id.size());
	}
	auto& lightingShaderContext = contexts[renderer->getContextIndex(context)];
	auto currentImplementation = lightingShaderContext.implementation;
	auto shaderIt = shader.find(renderer->getShaderPrefix() + shaderId);
	if (shaderIt == shader.end()) shaderIt = shader.find(renderer->getShaderPrefix() + "default");
	if (shaderIt == shader.end()) shaderIt = shader.find("default");
	auto nextImplementation = shaderIt->second;
	if (currentImplementation != nextImplementation) {
		if (currentImplementation != nullptr) currentImplementation->unUseProgram(context);
		lightingShaderContext.implementation = nextImplementation;
		lightingShaderContext.implementation->useProgram(engine, context);
	}
}

void LightingShader::updateShaderParameters(void* context) {
	auto& lightingShaderContext = contexts[renderer->getContextIndex(context)];
	if (lightingShaderContext.implementation == nullptr) return;
	lightingShaderContext.implementation->updateShaderParameters(renderer, context);
}

void LightingShader::bindTexture(void* context, int32_t textureId)
{
	auto& lightingShaderContext = contexts[renderer->getContextIndex(context)];
	if (lightingShaderContext.implementation == nullptr) return;
	lightingShaderContext.implementation->bindTexture(renderer, context, textureId);
}
