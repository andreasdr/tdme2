#pragma once

#include <string>

#include <agui/agui.h>
#include <agui/gui/renderer/fwd-agui.h>

using std::string;

/**
 * Renderer backend plugin base class
 * @author Andreas Drewke
 */
class agui::gui::renderer::GUIRendererBackendPlugin
{
public:
	/**
	 * @returns renderer version
	 */
	inline static const string getRendererVersion() {
		return "0.9.85";
	}

};
