#pragma once

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/subsystems/renderer/GL2Renderer.h>

using tdme::engine::subsystems::renderer::GL2Renderer;
using tdme::engine::Engine;

/**
 * Engine connector of GL2 renderer to other engine functionality
 * @author Andreas Drewke
 */
class tdme::engine::EngineGL2Renderer: public GL2Renderer
{

public:
	/**
	 * Public constructor
	 * @param engine engine
	 */
	EngineGL2Renderer(Engine* engine);

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
