#pragma once

#include <array>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/subsystems/lighting/fwd-tdme.h>
#include <tdme/engine/subsystems/lighting/LightingShaderConstants.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Matrix4x4.h>

using std::array;

using tdme::engine::subsystems::lighting::LightingShaderConstants;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::Engine;
using tdme::math::Matrix4x4;

/**
 * Lighting shader implementation interface
 * @author Andreas Drewke
 * @version $Id$
 */
struct tdme::engine::subsystems::lighting::LightingShaderImplementation
{
	/**
	 * Destructor
	 */
	virtual ~LightingShaderImplementation() {}

	/**
	 * @return shader id
	 */
	virtual const string getId() = 0;

	/**
	 * @return initialized and ready to be used
	 */
	virtual bool isInitialized() = 0;

	/**
	 * Initialize renderer
	 */
	virtual void initialize() = 0;

	/**
	 * Use lighting program
	 * @param engine engine
	 * @param context context
	 */
	virtual void useProgram(Engine* engine, void* context) = 0;

	/**
	 * Unuse lighting program
	 * @param context context
	 */
	virtual void unUseProgram(void* context) = 0;

	/**
	 * Update effect to program
	 * @param renderer renderer
	 * @param context context
	 */
	virtual void updateEffect(Renderer* renderer, void* context) = 0;

	/**
	 * Update material to program
	 * @param renderer renderer
	 * @param context context
	 */
	virtual void updateMaterial(Renderer* renderer, void* context) = 0;

	/**
	 * Update light to program
	 * @param renderer renderer
	 * @param context context
	 * @param lightId light id
	 */
	virtual void updateLight(Renderer* renderer, void* context, int32_t lightId) = 0;

	/**
	 * Update matrices to program
	 * @param renderer renderer
	 * @param context context
	 */
	virtual void updateMatrices(Renderer* renderer, void* context) = 0;

	/**
	 * Update texture matrix to program
	 * @param renderer renderer
	 * @param context context
	 */
	virtual void updateTextureMatrix(Renderer* renderer, void* context) = 0;

	/**
	 * Bind texture
	 * @param renderer renderer
	 * @param context context
	 * @param textureId texture id
	 */
	virtual void bindTexture(Renderer* renderer, void* context, int32_t textureId) = 0;

};
