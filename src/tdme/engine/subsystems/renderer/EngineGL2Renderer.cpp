#include <tdme/engine/subsystems/renderer/EngineGL2Renderer.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#if !defined(__APPLE__)
	#define GLEW_NO_GLU
	#include <GL/glew.h>
	#if defined(_WIN32)
		#include <GL/wglew.h>
	#endif
#endif

#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/subsystems/lighting/LightingShader.h>
#include <tdme/engine/subsystems/lines/LinesShader.h>
#include <tdme/engine/subsystems/particlesystem/ParticlesShader.h>
#include <tdme/engine/subsystems/shadowmapping/ShadowMapping.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Version.h>

using tdme::engine::subsystems::renderer::EngineGL2Renderer;

using std::string;

using tdme::engine::subsystems::lighting::LightingShader;
using tdme::engine::subsystems::lines::LinesShader;
using tdme::engine::subsystems::particlesystem::ParticlesShader;
using tdme::engine::subsystems::shadowmapping::ShadowMapping;
using tdme::engine::Engine;
using tdme::engine::Version;

EngineGL2Renderer::EngineGL2Renderer()
{
}

bool EngineGL2Renderer::prepareWindowSystemRendererContext(int tryIdx) {
	if (tryIdx > 0) return false;
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_FALSE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	return true;
}

bool EngineGL2Renderer::initializeWindowSystemRendererContext(GLFWwindow* glfwWindow) {
	glfwMakeContextCurrent(glfwWindow);
	if (glfwGetCurrentContext() == nullptr) {
		Console::printLine("EngineGL2Renderer::initializeWindowSystemRendererContext(): glfwMakeContextCurrent(): Error: No window attached to context");
		return false;
	}
	#if !defined(__APPLE__)
		glewExperimental = true;
		GLenum glewInitStatus = glewInit();
		if (glewInitStatus != GLEW_OK) {
			Console::printLine("EngineGL2Renderer::initializeWindowSystemRendererContext(): glewInit(): Error: " + (string((char*)glewGetErrorString(glewInitStatus))));
			return false;
		}
	#endif
	return true;
}

void EngineGL2Renderer::onUpdateProjectionMatrix(int contextIdx)
{
	if (Engine::lightingShader != nullptr)
		Engine::lightingShader->updateMatrices(contextIdx);

	if (Engine::particlesShader != nullptr)
		Engine::particlesShader->updateMatrices(contextIdx);

	if (Engine::linesShader != nullptr)
		Engine::linesShader->updateMatrices(contextIdx);

	if (Engine::Engine::currentEngine->shadowMapping != nullptr)
		Engine::currentEngine->shadowMapping->updateMatrices(contextIdx);
}

void EngineGL2Renderer::onUpdateCameraMatrix(int contextIdx)
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

void EngineGL2Renderer::onUpdateModelViewMatrix(int contextIdx)
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

void EngineGL2Renderer::onBindTexture(int contextIdx, int32_t textureId)
{
	if (Engine::lightingShader != nullptr)
		Engine::lightingShader->bindTexture(contextIdx, textureId);

	if (Engine::currentEngine->shadowMapping != nullptr)
		Engine::currentEngine->shadowMapping->bindTexture(contextIdx, textureId);
}

void EngineGL2Renderer::onUpdateTextureMatrix(int contextIdx)
{
	if (Engine::lightingShader != nullptr)
		Engine::lightingShader->updateTextureMatrix(contextIdx);

	if (Engine::currentEngine->shadowMapping != nullptr)
		Engine::currentEngine->shadowMapping->updateTextureMatrix(contextIdx);
}

void EngineGL2Renderer::onUpdateEffect(int contextIdx)
{
	if (Engine::lightingShader != nullptr)
		Engine::lightingShader->updateEffect(contextIdx);

	if (Engine::particlesShader != nullptr)
		Engine::particlesShader->updateEffect(contextIdx);

	if (Engine::linesShader != nullptr)
		Engine::linesShader->updateEffect(contextIdx);
}

void EngineGL2Renderer::onUpdateLight(int contextIdx, int32_t lightId)
{
	if (Engine::lightingShader != nullptr)
		Engine::lightingShader->updateLight(contextIdx, lightId);

	if (Engine::currentEngine->shadowMapping != nullptr)
		Engine::currentEngine->shadowMapping->updateLight(contextIdx, lightId);
}

void EngineGL2Renderer::onUpdateMaterial(int contextIdx)
{
	if (Engine::lightingShader != nullptr)
		Engine::lightingShader->updateMaterial(contextIdx);

	if (Engine::currentEngine->shadowMapping != nullptr)
		Engine::currentEngine->shadowMapping->updateMaterial(contextIdx);
}

void EngineGL2Renderer::onUpdateShader(int contextIdx) {
	if (Engine::lightingShader != nullptr)
		Engine::lightingShader->setShader(contextIdx, getShader(contextIdx));

	if (Engine::currentEngine->shadowMapping != nullptr)
		Engine::currentEngine->shadowMapping->setShader(contextIdx, getShader(contextIdx));
}

void EngineGL2Renderer::onUpdateShaderParameters(int contextIdx) {
	if (Engine::lightingShader != nullptr)
		Engine::lightingShader->updateShaderParameters(contextIdx);

	if (Engine::currentEngine->shadowMapping != nullptr)
		Engine::currentEngine->shadowMapping->updateShaderParameters(contextIdx);
}

// end point for engine to create renderer backend
extern "C" EngineGL2Renderer* createInstance()
{
	if (EngineGL2Renderer::getRendererVersion() != Version::getVersion()) {
		Console::printLine("EngineGL2Renderer::createInstance(): Engine and renderer backend version do not match: '" + EngineGL2Renderer::getRendererVersion() + "' != '" + Version::getVersion() + "'");
		return nullptr;
	}
	Console::printLine("EngineGL2Renderer::createInstance(): Creating EngineGL2Renderer instance");
	return new EngineGL2Renderer();
}
