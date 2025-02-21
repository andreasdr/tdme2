#include <tdme/engine/subsystems/renderer/EngineGLES2Renderer.h>

#if defined(_MSC_VER)
	// this suppresses a warning redefinition of APIENTRY macro
	#define NOMINMAX
	#include <windows.h>
#endif
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/subsystems/lighting/LightingShader.h>
#include <tdme/engine/subsystems/lines/LinesShader.h>
#include <tdme/engine/subsystems/particlesystem/ParticlesShader.h>
#include <tdme/engine/subsystems/shadowmapping/ShadowMapping.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Version.h>

using tdme::engine::subsystems::renderer::EngineGLES2Renderer;

using std::string;

using tdme::engine::subsystems::lighting::LightingShader;
using tdme::engine::subsystems::lines::LinesShader;
using tdme::engine::subsystems::particlesystem::ParticlesShader;
using tdme::engine::subsystems::shadowmapping::ShadowMapping;
using tdme::engine::Engine;
using tdme::engine::Version;

EngineGLES2Renderer::EngineGLES2Renderer()
{
}

bool EngineGLES2Renderer::prepareWindowSystemRendererContext(int tryIdx) {
	if (tryIdx > 0) return false;
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	return true;
}

bool EngineGLES2Renderer::initializeWindowSystemRendererContext(GLFWwindow* glfwWindow) {
	glfwMakeContextCurrent(glfwWindow);
	if (glfwGetCurrentContext() == nullptr) {
		Console::printLine("EngineGLES2Renderer::initializeWindowSystemRendererContext(): glfwMakeContextCurrent(): Error: No window attached to context");
		return false;
	}
	return true;
}

void EngineGLES2Renderer::onUpdateProjectionMatrix(int contextIdx)
{
	if (Engine::lightingShader != nullptr)
		Engine::lightingShader->updateMatrices(contextIdx);

	if (Engine::particlesShader != nullptr)
		Engine::particlesShader->updateMatrices(contextIdx);

	if (Engine::linesShader != nullptr)
		Engine::linesShader->updateMatrices(contextIdx);

	if (Engine::currentEngine->shadowMapping != nullptr)
		Engine::currentEngine->shadowMapping->updateMatrices(contextIdx);
}

void EngineGLES2Renderer::onUpdateCameraMatrix(int contextIdx)
{
	if (Engine::lightingShader != nullptr)
		Engine::lightingShader->updateMatrices(contextIdx);

	if (Engine::particlesShader != nullptr)
		Engine::particlesShader->updateMatrices(contextIdx);

	if (Engine::linesShader != nullptr)
		Engine::linesShader->updateMatrices(contextIdx);

	if (Engine::currentEngine->shadowMapping != nullptr)
		Engine::currentEngine->shadowMapping->updateMatrices(contextIdx);
}

void EngineGLES2Renderer::onUpdateModelViewMatrix(int contextIdx)
{
	if (Engine::lightingShader != nullptr)
		Engine::lightingShader->updateMatrices(contextIdx);

	if (Engine::particlesShader != nullptr)
		Engine::particlesShader->updateMatrices(contextIdx);

	if (Engine::linesShader != nullptr)
		Engine::linesShader->updateMatrices(contextIdx);

	if (Engine::currentEngine->shadowMapping != nullptr)
		Engine::currentEngine->shadowMapping->updateMatrices(contextIdx);
}

void EngineGLES2Renderer::onBindTexture(int contextIdx, int32_t textureId)
{
	if (Engine::lightingShader != nullptr)
		Engine::lightingShader->bindTexture(contextIdx, textureId);

	if (Engine::currentEngine->shadowMapping != nullptr)
		Engine::currentEngine->shadowMapping->bindTexture(contextIdx, textureId);
}

void EngineGLES2Renderer::onUpdateTextureMatrix(int contextIdx)
{
	if (Engine::lightingShader != nullptr)
		Engine::lightingShader->updateTextureMatrix(contextIdx);

	if (Engine::currentEngine->shadowMapping != nullptr)
		Engine::currentEngine->shadowMapping->updateTextureMatrix(contextIdx);
}

void EngineGLES2Renderer::onUpdateEffect(int contextIdx)
{
	if (Engine::lightingShader != nullptr)
		Engine::lightingShader->updateEffect(contextIdx);

	if (Engine::particlesShader != nullptr)
		Engine::particlesShader->updateEffect(contextIdx);

	if (Engine::linesShader != nullptr)
		Engine::linesShader->updateEffect(contextIdx);
}

void EngineGLES2Renderer::onUpdateLight(int contextIdx, int32_t lightId)
{
	if (Engine::lightingShader != nullptr)
		Engine::lightingShader->updateLight(contextIdx, lightId);

	if (Engine::currentEngine->shadowMapping != nullptr)
		Engine::currentEngine->shadowMapping->updateLight(contextIdx, lightId);
}

void EngineGLES2Renderer::onUpdateMaterial(int contextIdx)
{
	if (Engine::lightingShader != nullptr)
		Engine::lightingShader->updateMaterial(contextIdx);

	if (Engine::currentEngine->shadowMapping != nullptr)
		Engine::currentEngine->shadowMapping->updateMaterial(contextIdx);
}

void EngineGLES2Renderer::onUpdateShader(int contextIdx) {
	if (Engine::lightingShader != nullptr)
		Engine::lightingShader->setShader(contextIdx, getShader(contextIdx));

	if (Engine::currentEngine->shadowMapping != nullptr)
		Engine::currentEngine->shadowMapping->setShader(contextIdx, getShader(contextIdx));
}

void EngineGLES2Renderer::onUpdateShaderParameters(int contextIdx) {
	if (Engine::lightingShader != nullptr)
		Engine::lightingShader->updateShaderParameters(contextIdx);

	if (Engine::currentEngine->shadowMapping != nullptr)
		Engine::currentEngine->shadowMapping->updateShaderParameters(contextIdx);
}

// end point for engine to create renderer backend
extern "C" EngineGLES2Renderer* createInstance()
{
	if (EngineGLES2Renderer::getRendererVersion() != Version::getVersion()) {
		Console::printLine("EngineGL2Renderer::createInstance(): Engine and renderer backend version do not match: '" + EngineGLES2Renderer::getRendererVersion() + "' != '" + Version::getVersion() + "'");
		return nullptr;
	}
	Console::printLine("EngineGLES2Renderer::createInstance(): Creating EngineGLES2Renderer instance!");
	return new EngineGLES2Renderer();
}
