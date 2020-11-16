#pragma once

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/subsystems/renderer/VKRenderer.h>

using tdme::engine::subsystems::renderer::VKRenderer;
using tdme::engine::Engine;

/**
 * Engine connector of VK renderer to other engine functionality
 * @author Andreas Drewke
 */

class tdme::engine::EngineVKRenderer: public VKRenderer
{
public:
	/**
	 * Public constructor
	 * @param engine engine
	 */
	EngineVKRenderer(Engine* engine);

	// overridden methods
	void onUpdateProjectionMatrix(void* context) override;
	void onUpdateCameraMatrix(void* context) override;
	void onUpdateModelViewMatrix(void* context) override;
	void onBindTexture(void* context, int32_t textureId) override;
	void onUpdateTextureMatrix(void* context) override;
	void onUpdateEffect(void* context) override;
	void onUpdateLight(void* context, int32_t lightId) override;
	void onUpdateMaterial(void* context) override;
	void onUpdateShader(void* context) override;
	void onUpdateShaderParameters(void* context) override;

private:
	Engine* engine { nullptr };
};
