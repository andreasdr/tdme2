#include <agui/gui/renderer/ApplicationGL2Renderer.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#if !defined(__APPLE__)
	#define GLEW_NO_GLU
	#include <GL/glew.h>
	#if defined(_WIN32)
		#include <GL/wglew.h>
	#endif
#endif

#include <array>
#include <string>

#include <agui/agui.h>
#include <agui/gui/renderer/GUIShader.h>
#include <agui/gui/GUI.h>
#include <agui/gui/GUIVersion.h>
#include <agui/utilities/Console.h>

using agui::gui::renderer::ApplicationGL2Renderer;

using std::array;
using std::string;

using agui::gui::renderer::GUIShader;
using agui::gui::GUI;
using agui::gui::GUIVersion;
using agui::utilities::Console;

ApplicationGL2Renderer::ApplicationGL2Renderer()
{
}

bool ApplicationGL2Renderer::prepareWindowSystemRendererContext(int tryIdx) {
	if (tryIdx > 0) return false;
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_FALSE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	return true;
}

bool ApplicationGL2Renderer::initializeWindowSystemRendererContext(GLFWwindow* glfwWindow) {
	glfwMakeContextCurrent(glfwWindow);
	if (glfwGetCurrentContext() == nullptr) {
		Console::printLine("ApplicationGL2Renderer::initializeWindowSystemRendererContext(): glfwMakeContextCurrent(): Error: No window attached to context");
		return false;
	}
	#if !defined(__APPLE__)
		glewExperimental = true;
		GLenum glewInitStatus = glewInit();
		if (glewInitStatus != GLEW_OK) {
			Console::printLine("ApplicationGL2Renderer::initializeWindowSystemRendererContext(): glewInit(): Error: " + (string((char*)glewGetErrorString(glewInitStatus))));
			return false;
		}
	#endif
	return true;
}

void ApplicationGL2Renderer::onBindTexture(int contextIdx, int32_t textureId)
{
	GUI::getShader()->bindTexture(textureId);
}

void ApplicationGL2Renderer::onUpdateTextureMatrix(int contextIdx)
{
	GUI::getShader()->updateTextureMatrix();
}

void ApplicationGL2Renderer::onUpdateEffect(int contextIdx)
{
	GUI::getShader()->updateEffect();
}

extern "C" ApplicationGL2Renderer* createInstance()
{
	if (ApplicationGL2Renderer::getRendererVersion() != GUIVersion::getVersion()) {
		Console::printLine("ApplicationGL2Renderer::createInstance(): Engine and renderer backend version do not match: '" + ApplicationGL2Renderer::getRendererVersion() + "' != '" + GUIVersion::getVersion() + "'");
		return nullptr;
	}
	Console::printLine("ApplicationGL2Renderer::createInstance(): Creating ApplicationGL2Renderer instance!");
	return new ApplicationGL2Renderer();
}
