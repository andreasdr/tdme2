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
class tdme::engine::subsystems::lighting::LightingShaderLightScatteringDefaultImplementation: public LightingShaderBaseImplementation
{
public:

	/**
	 * @return if supported by renderer
	 * @param renderer renderer
	 */
	static bool isSupported(Renderer* renderer);

	// overriden methods
	virtual const string getId() override;
	virtual void initialize() override;

	/**
	 * Public constructor
	 * @param renderer renderer
	 */
	LightingShaderLightScatteringDefaultImplementation(Renderer* renderer);
};
