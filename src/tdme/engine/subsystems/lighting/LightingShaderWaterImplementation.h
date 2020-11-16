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
class tdme::engine::subsystems::lighting::LightingShaderWaterImplementation: public LightingShaderBaseImplementation
{
public:

	static constexpr int WAVES_MAX { 4 };

	/**
	 * Public constructor
	 * @param renderer renderer
	 */
	LightingShaderWaterImplementation(Renderer* renderer);

	/**
	 * @return if supported by renderer
	 * @param renderer renderer
	 */
	static bool isSupported(Renderer* renderer);

	// overridden methods
	virtual const string getId() override;
	virtual void initialize() override;
	virtual void useProgram(Engine* engine, void* context) override;
	virtual void updateMatrices(Renderer* renderer, void* context) override;

protected:
	int32_t uniformWaterHeight { -1 };
	int32_t uniformTime { -1 };
	int32_t uniformNumWaves { -1 };
	array<int32_t, WAVES_MAX> uniformAmplitude;
	array<int32_t, WAVES_MAX> uniformWavelength;
	array<int32_t, WAVES_MAX> uniformSpeed;
	array<int32_t, WAVES_MAX> uniformDirection;
	int32_t uniformModelMatrix { -1 };
	float time { 0.0f };
	array<float, WAVES_MAX> angle;
};
