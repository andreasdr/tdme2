#include <tdme/engine/subsystems/renderer/EngineGL3Renderer.h>

#if defined(_MSC_VER)
	// this suppresses a warning redefinition of APIENTRY macro
	#define NOMINMAX
	#include <windows.h>
#endif
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

using tdme::engine::subsystems::renderer::EngineGL3Renderer;

using std::string;

using tdme::engine::subsystems::lighting::LightingShader;
using tdme::engine::subsystems::lines::LinesShader;
using tdme::engine::subsystems::particlesystem::ParticlesShader;
using tdme::engine::subsystems::shadowmapping::ShadowMapping;
using tdme::engine::Engine;
using tdme::engine::Version;

EngineGL3Renderer::EngineGL3Renderer()
{
}

bool EngineGL3Renderer::prepareWindowSystemRendererContext(int tryIdx) {
	array<array<int, 3>, 2> glVersions = {{ {{1, 4, 3}}, {{1, 3, 2}} }};
	if (tryIdx >= glVersions.size()) return false;
	#if defined(__APPLE__)
		glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);
	#endif
	const auto& glVersion = glVersions[tryIdx];
	#if !defined(__HAIKU__)
		// does not work for now with Haiku OS
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, glVersion[0] == 1?GLFW_TRUE:GLFW_FALSE);
	#endif
	glfwWindowHint(GLFW_OPENGL_PROFILE, glVersion[0] == 1?GLFW_OPENGL_CORE_PROFILE:GLFW_OPENGL_ANY_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glVersion[1]);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glVersion[2]);
	return true;
}

bool EngineGL3Renderer::initializeWindowSystemRendererContext(GLFWwindow* glfwWindow) {
	glfwMakeContextCurrent(glfwWindow);
	if (glfwGetCurrentContext() == nullptr) {
		Console::printLine("EngineGL3Renderer::initializeWindowSystemRendererContext(): glfwMakeContextCurrent(): Error: No window attached to context");
		return false;
	}
	#if !defined(__APPLE__)
		//glewExperimental = true;
		GLenum glewInitStatus = glewInit();
		if (glewInitStatus != GLEW_OK) {
			Console::printLine("EngineGL3Renderer::initializeWindowSystemRendererContext(): glewInit(): Error: " + (string((char*)glewGetErrorString(glewInitStatus))));
			return false;
		}
	#endif
	return true;
}

void EngineGL3Renderer::onUpdateProjectionMatrix(int contextIdx)
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

void EngineGL3Renderer::onUpdateCameraMatrix(int contextIdx)
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

void EngineGL3Renderer::onUpdateModelViewMatrix(int contextIdx)
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

void EngineGL3Renderer::onBindTexture(int contextIdx, int32_t textureId)
{
	if (Engine::lightingShader != nullptr)
		Engine::lightingShader->bindTexture(contextIdx, textureId);

	if (Engine::currentEngine->shadowMapping != nullptr)
		Engine::currentEngine->shadowMapping->bindTexture(contextIdx, textureId);
}

void EngineGL3Renderer::onUpdateTextureMatrix(int contextIdx)
{
	if (Engine::lightingShader != nullptr)
		Engine::lightingShader->updateTextureMatrix(contextIdx);

	if (Engine::currentEngine->shadowMapping != nullptr)
		Engine::currentEngine->shadowMapping->updateTextureMatrix(contextIdx);
}

void EngineGL3Renderer::onUpdateEffect(int contextIdx)
{
	if (Engine::lightingShader != nullptr)
		Engine::lightingShader->updateEffect(contextIdx);

	if (Engine::particlesShader != nullptr)
		Engine::particlesShader->updateEffect(contextIdx);

	if (Engine::linesShader != nullptr)
		Engine::linesShader->updateEffect(contextIdx);
}

void EngineGL3Renderer::onUpdateLight(int contextIdx, int32_t lightId)
{
	if (Engine::lightingShader != nullptr)
		Engine::lightingShader->updateLight(contextIdx, lightId);

	if (Engine::currentEngine->shadowMapping != nullptr)
		Engine::currentEngine->shadowMapping->updateLight(contextIdx, lightId);
}

void EngineGL3Renderer::onUpdateMaterial(int contextIdx)
{
	if (Engine::lightingShader != nullptr)
		Engine::lightingShader->updateMaterial(contextIdx);

	if (Engine::currentEngine->shadowMapping != nullptr)
		Engine::currentEngine->shadowMapping->updateMaterial(contextIdx);
}

void EngineGL3Renderer::onUpdateShader(int contextIdx) {
	if (Engine::lightingShader != nullptr)
		Engine::lightingShader->setShader(contextIdx, getShader(contextIdx));

	if (Engine::currentEngine->shadowMapping != nullptr)
		Engine::currentEngine->shadowMapping->setShader(contextIdx, getShader(contextIdx));
}

void EngineGL3Renderer::onUpdateShaderParameters(int contextIdx) {
	if (Engine::lightingShader != nullptr)
		Engine::lightingShader->updateShaderParameters(contextIdx);

	if (Engine::currentEngine->shadowMapping != nullptr)
		Engine::currentEngine->shadowMapping->updateShaderParameters(contextIdx);
}

// end point for engine to create renderer backend
extern "C" EngineGL3Renderer* createInstance()
{
	if (EngineGL3Renderer::getRendererVersion() != Version::getVersion()) {
		Console::printLine("EngineGL3Renderer::createInstance(): Engine and renderer backend version do not match: '" + EngineGL3Renderer::getRendererVersion() + "' != '" + Version::getVersion() + "'");
		return nullptr;
	}
	Console::printLine("EngineGL3Renderer::createInstance(): Creating EngineGL3Renderer instance!");
	return new EngineGL3Renderer();
}
