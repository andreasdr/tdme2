#include <map>
#include <string>

#include <tdme/engine/subsystems/postprocessing/PostProcessingShaderDesaturationImplementation.h>
#include <tdme/engine/subsystems/postprocessing/PostProcessingShader.h>
#include <tdme/engine/subsystems/postprocessing/PostProcessingShaderBlurImplementation.h>
#include <tdme/engine/subsystems/postprocessing/PostProcessingShaderDefaultImplementation.h>
#include <tdme/engine/subsystems/postprocessing/PostProcessingShaderDesaturationImplementation.h>
#include <tdme/engine/subsystems/postprocessing/PostProcessingShaderImplementation.h>
#include <tdme/engine/subsystems/postprocessing/PostProcessingShaderLightScatteringImplementation.h>
#include <tdme/engine/subsystems/postprocessing/PostProcessingShaderSSAOImplementation.h>
#include <tdme/engine/subsystems/postprocessing/PostProcessingShaderSSAOMapImplementation.h>
#include <tdme/engine/subsystems/postprocessing/PostProcessingShaderVignetteImplementation.h>
#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/engine/Engine.h>

using std::map;
using std::string;

using tdme::engine::subsystems::postprocessing::PostProcessingShader;
using tdme::engine::subsystems::postprocessing::PostProcessingShaderBlurImplementation;
using tdme::engine::subsystems::postprocessing::PostProcessingShaderDefaultImplementation;
using tdme::engine::subsystems::postprocessing::PostProcessingShaderDesaturationImplementation;
using tdme::engine::subsystems::postprocessing::PostProcessingShaderImplementation;
using tdme::engine::subsystems::postprocessing::PostProcessingShaderLightScatteringImplementation;
using tdme::engine::subsystems::postprocessing::PostProcessingShaderSSAOImplementation;
using tdme::engine::subsystems::postprocessing::PostProcessingShaderSSAOMapImplementation;
using tdme::engine::subsystems::postprocessing::PostProcessingShaderVignetteImplementation;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::Engine;

PostProcessingShader::PostProcessingShader(Renderer* renderer)
{
	if (PostProcessingShaderBlurImplementation::isSupported(renderer) == true) shader["depth_blur"] = new PostProcessingShaderBlurImplementation(renderer);
	if (PostProcessingShaderDefaultImplementation::isSupported(renderer) == true) shader["default"] = new PostProcessingShaderDefaultImplementation(renderer);
	if (PostProcessingShaderDesaturationImplementation::isSupported(renderer) == true) shader["desaturation"] = new PostProcessingShaderDesaturationImplementation(renderer);
	if (PostProcessingShaderLightScatteringImplementation::isSupported(renderer) == true) shader["light_scattering"] = new PostProcessingShaderLightScatteringImplementation(renderer);
	if (PostProcessingShaderSSAOMapImplementation::isSupported(renderer) == true) shader["ssao_map"] = new PostProcessingShaderSSAOMapImplementation(renderer);
	if (PostProcessingShaderSSAOImplementation::isSupported(renderer) == true) shader["ssao"] = new PostProcessingShaderSSAOImplementation(renderer);
	if (PostProcessingShaderVignetteImplementation::isSupported(renderer) == true) shader["vignette"] = new PostProcessingShaderVignetteImplementation(renderer);
	implementation = nullptr;
}

PostProcessingShader::~PostProcessingShader()
{
	for (auto shaderIt: shader) {
		delete shaderIt.second;
	}
}

bool PostProcessingShader::isInitialized()
{
	auto initialized = true;
	for (auto shaderIt: shader) {
		initialized&= shaderIt.second->isInitialized();
	}
	return initialized;
}

void PostProcessingShader::initialize()
{
	for (auto shaderIt: shader) {
		shaderIt.second->initialize();
	}
}

void PostProcessingShader::useProgram()
{
	running = true;
}

void PostProcessingShader::unUseProgram()
{
	running = false;
	if (implementation != nullptr) {
		implementation->unUseProgram();;
	}
	implementation = nullptr;
}

bool PostProcessingShader::hasShader(const string& id) {
	return shader.find(id) != shader.end();
}

void PostProcessingShader::setShader(void* context, const string& id) {
	if (running == false) return;

	auto currentImplementation = implementation;
	auto shaderIt = shader.find(id);
	if (shaderIt == shader.end()) {
		shaderIt = shader.find("default");
	}
	implementation = shaderIt->second;

	if (currentImplementation != implementation) {
		if (currentImplementation != nullptr) currentImplementation->unUseProgram();
		implementation->useProgram(context);
	}
}

void PostProcessingShader::setBufferPixelWidth(void* context, float pixelWidth) {
	if (implementation == nullptr) return;
	implementation->setBufferPixelWidth(context, pixelWidth);
}

void PostProcessingShader::setBufferPixelHeight(void* context, float pixelHeight) {
	if (implementation == nullptr) return;
	implementation->setBufferPixelHeight(context, pixelHeight);
}

void PostProcessingShader::setTextureLightPositionX(void* context, float textureLightPositionX) {
	if (implementation == nullptr) return;
	implementation->setTextureLightPositionX(context, textureLightPositionX);
}

void PostProcessingShader::setTextureLightPositionY(void* context, float textureLightPositionY) {
	if (implementation == nullptr) return;
	implementation->setTextureLightPositionY(context, textureLightPositionY);
}

void PostProcessingShader::setIntensity(void* context, float intensity) {
	if (implementation == nullptr) return;
	implementation->setIntensity(context, intensity);
}
