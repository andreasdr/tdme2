#include <tdme/engine/subsystems/earlyzrejection/EZRShaderPre.h>

#include <tdme/engine/Engine.h>
#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/engine/subsystems/earlyzrejection/EZRShaderPreDefaultImplementation.h>
#include <tdme/engine/subsystems/earlyzrejection/EZRShaderPreImplementation.h>
#include <tdme/math/Matrix4x4.h>

using tdme::engine::subsystems::earlyzrejection::EZRShaderPre;

using tdme::engine::Engine;
using tdme::engine::subsystems::earlyzrejection::EZRShaderPreBaseImplementation;
using tdme::engine::subsystems::earlyzrejection::EZRShaderPreDefaultImplementation;
using tdme::engine::subsystems::earlyzrejection::EZRShaderPreImplementation;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::math::Matrix4x4;
using tdme::utilities::Console;

EZRShaderPre::EZRShaderPre(Renderer* renderer): renderer(renderer)
{
	if (EZRShaderPreDefaultImplementation::isSupported(renderer) == true) { auto shaderProgram = new EZRShaderPreDefaultImplementation(renderer); shader[shaderProgram->getId()] = shaderProgram; }
	auto threadCount = renderer->isSupportingMultithreadedRendering() == true?Engine::getThreadCount():1;
	contexts.resize(threadCount);
}

EZRShaderPre::~EZRShaderPre() {
	for (auto shaderIt: shader) {
		delete shaderIt.second;
	}
}

bool EZRShaderPre::isInitialized()
{
	bool initialized = true;
	for (auto shaderIt: shader) {
		initialized&= shaderIt.second->isInitialized();
	}
	return initialized;
}

void EZRShaderPre::initialize()
{
	for (auto shaderIt: shader) {
		shaderIt.second->initialize();
	}
}

void EZRShaderPre::useProgram(Engine* engine)
{
	running = true;
	this->engine = engine;
}

void EZRShaderPre::unUseProgram()
{
	running = false;
	auto i = 0;
	for (auto& ezrShaderPreContext: contexts) {
		if (ezrShaderPreContext.implementation != nullptr) {
			ezrShaderPreContext.implementation->unUseProgram(renderer->getContext(i));
		}
		ezrShaderPreContext.implementation = nullptr;
		i++;
	}
	engine = nullptr;
}

void EZRShaderPre::updateMatrices(void* context)
{
	auto& ezrShaderPreContext = contexts[renderer->getContextIndex(context)];
	if (ezrShaderPreContext.implementation == nullptr) return;
	ezrShaderPreContext.implementation->updateMatrices(renderer, context);
}

void EZRShaderPre::updateTextureMatrix(void* context) {
	auto& ezrShaderPreContext = contexts[renderer->getContextIndex(context)];
	if (ezrShaderPreContext.implementation == nullptr) return;
	ezrShaderPreContext.implementation->updateTextureMatrix(renderer, context);
}

void EZRShaderPre::updateMaterial(void* context)
{
	auto& ezrShaderPreContext = contexts[renderer->getContextIndex(context)];
	if (ezrShaderPreContext.implementation == nullptr) return;
	ezrShaderPreContext.implementation->updateMaterial(renderer, context);
}

void EZRShaderPre::bindTexture(void* context, int32_t textureId)
{
	auto& ezrShaderPreContext = contexts[renderer->getContextIndex(context)];
	if (ezrShaderPreContext.implementation == nullptr) return;
	ezrShaderPreContext.implementation->bindTexture(renderer, context, textureId);
}

void EZRShaderPre::setShader(void* context, const string& id) {
	if (running == false) return;

	auto& ezrShaderPreContext = contexts[renderer->getContextIndex(context)];
	auto currentImplementation = ezrShaderPreContext.implementation;

	auto shaderIt = shader.find(id);
	if (shaderIt == shader.end()) {
		shaderIt = shader.find("default");
	}
	ezrShaderPreContext.implementation = shaderIt->second;

	if (currentImplementation != ezrShaderPreContext.implementation) {
		if (currentImplementation != nullptr) currentImplementation->unUseProgram(context);
		ezrShaderPreContext.implementation->useProgram(engine, context);
	}
}
