#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/subsystems/lighting/fwd-tdme.h>
#include <tdme/engine/subsystems/lighting/LightingShaderBaseImplementation.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>

using tdme::engine::subsystems::lighting::LightingShaderBaseImplementation;
using tdme::engine::subsystems::renderer::Renderer;

/**
 * Lighting shader implementation
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::subsystems::lighting::LightingShaderTerrainImplementation: public LightingShaderBaseImplementation
{
public:
	/**
	 * Public constructor
	 * @param renderer renderer
	 */
	LightingShaderTerrainImplementation(Renderer* renderer);

	/**
	 * @return if supported by renderer
	 * @param renderer renderer
	 */
	static bool isSupported(Renderer* renderer);

	// overridden methods
	virtual const string getId() override;
	virtual void initialize() override;
	virtual void useProgram(Engine* engine, void* context) override;
	virtual void unUseProgram(void* context) override;
	virtual void updateMatrices(Renderer* renderer, void* context) override;

private:
	int32_t uniformModelMatrix { -1 };
	int32_t uniformGrasTextureUnit { -1 };
	int32_t uniformDirtTextureUnit { -1 };
	int32_t uniformSnowTextureUnit { -1 };
	int32_t uniformStoneTextureUnit { -1 };
	int32_t grasTextureId { 0 };
	int32_t dirtTextureId { 0 };
	int32_t snowTextureId { 0 };
	int32_t stoneTextureId { 0 };

};
