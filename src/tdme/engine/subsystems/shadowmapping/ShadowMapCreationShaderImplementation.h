#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/engine/subsystems/shadowmapping/fwd-tdme.h>
#include <tdme/math/fwd-tdme.h>

using std::string;

using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::Engine;
using tdme::math::Matrix4x4;

/**
 * Pre shadow mapping shader for create shadow map
 * @author Andreas Drewke
 * @version $Id$
 */
struct tdme::engine::subsystems::shadowmapping::ShadowMapCreationShaderImplementation
{
	/**
	 * Destructor
	 */
	virtual ~ShadowMapCreationShaderImplementation() {}

	/**
	 * @return shader id
	 */
	virtual const string getId() = 0;

	/**
	 * @return if initialized and ready to use
	 */
	virtual bool isInitialized() = 0;

	/**
	 * Init shadow map creation program
	 */
	virtual void initialize() = 0;

	/**
	 * Use shadow map creation program
	 * @param engine engine
	 * @param context context
	 */
	virtual void useProgram(Engine* engine, void* context) = 0;

	/**
	 * Unuse shadow map creation program
	 * @param context context
	 */
	virtual void unUseProgram(void* context) = 0;

	/**
	 * Set up program matrices
	 * @param context context
	 */
	virtual void updateMatrices(void* context) = 0;

	/**
	 * Set up program texture matrix
	 * @param renderer renderer
	 * @param context context
	 */
	virtual void updateTextureMatrix(Renderer* renderer, void* context) = 0;

	/**
	 * Update material
	 * @param renderer renderer
	 * @param context context
	 */
	virtual void updateMaterial(Renderer* renderer, void* context) = 0;

	/**
	 * Update shader parameters
	 * @param renderer renderer
	 * @param context context
	 */
	virtual void updateShaderParameters(Renderer* renderer, void* context) = 0;

	/**
	 * Bind texture
	 * @param renderer renderer
	 * @param context context
	 * @param textureId texture id
	 */
	virtual void bindTexture(Renderer* renderer, void* context, int32_t textureId) = 0;
};
