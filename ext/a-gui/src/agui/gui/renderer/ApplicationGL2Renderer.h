#pragma once

#if defined(_MSC_VER)
	// this suppresses a warning redefinition of APIENTRY macro
	#define NOMINMAX
	#include <windows.h>
#endif
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <string>

#include <agui/agui.h>
#include <agui/gui/renderer/fwd-agui.h>
#include <agui/gui/renderer/GL2Renderer.h>
#include <agui/gui/renderer/GUIRendererBackendPlugin.h>

using std::string;

/**
 * Application GL2 renderer
 * @author Andreas Drewke
 */
class agui::gui::renderer::ApplicationGL2Renderer: public GL2Renderer, public GUIRendererBackendPlugin
{
public:
	// forbid class copy
	FORBID_CLASS_COPY(ApplicationGL2Renderer)

	/**
	 * Public constructor
	 */
	ApplicationGL2Renderer();

	// overridden methods
	bool prepareWindowSystemRendererContext(int tryIdx) override;
	bool initializeWindowSystemRendererContext(GLFWwindow* glfwWindow) override;
	void onBindTexture(int contextIdx, int32_t textureId) override;
	void onUpdateTextureMatrix(int contextIdx) override;
	void onUpdateEffect(int contextIdx) override;

};
