#pragma once

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/engine/subsystems/shadowmapping/fwd-tdme.h>
#include <tdme/math/Matrix4x4.h>

using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::Engine;
using tdme::math::Matrix4x4;

/**
 * Shadow mapping shader interface to render shadow map
 * @author Andreas Drewke
 * @version $Id$
 */
struct tdme::engine::subsystems::shadowmapping::ShadowMapRenderShaderImplementation
{
	/**
	 * Destructor
	 */
	virtual ~ShadowMapRenderShaderImplementation() {}

	/**
	 * @return shader id
	 */
	virtual const string getId() = 0;

	/**
	 * @return if initialized and ready to use
	 */
	virtual bool isInitialized() = 0;

	/**
	 * Init shadow mapping
	 */
	virtual void initialize() = 0;

	/**
	 * Use shadow map render shader program
	 * @param engine engine
	 * @param context context
	 */
	virtual void useProgram(Engine* engine, void* context) = 0;

	/**
	 * Un use shadow map render shader program
	 * @param context context
	 */
	virtual void unUseProgram(void* context) = 0;

	/**
	 * Update matrices
	 * @param context context
	 */
	virtual void updateMatrices(void* context) = 0;

	/**
	 * Update texture matrix
	 * @param renderer renderer
	 * @param context contet
	 */
	virtual void updateTextureMatrix(Renderer* renderer, void* context) = 0;

	/**
	 * Update material
	 * @param renderer renderer
	 * @param context contet
	 */
	virtual void updateMaterial(Renderer* renderer, void* context) = 0;

	/**
	 * Update light
	 * @param renderer renderer
	 * @param context contet
	 * @param lightId light id
	 */
	virtual void updateLight(Renderer* renderer, void* context, int32_t lightId) = 0;

	/**
	 * Bind texture
	 * @param renderer renderer
	 * @param context contet
	 * @param textureId texture id
	 */
	virtual void bindTexture(Renderer* renderer, void* context, int32_t textureId) = 0;

	/**
	 * Set up program depth bias mvp matrix
	 * @param context contet
	 * @param depthBiasMVPMatrix depth bias mvp matrix
	 */
	virtual void setDepthBiasMVPMatrix(void* context, const Matrix4x4& depthBiasMVPMatrix) = 0;

	/**
	 * Set light id
	 * @param lightId light id to render
	 */
	virtual void setRenderLightId(int32_t lightId) = 0;
};
