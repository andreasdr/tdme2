#pragma once

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/subsystems/postprocessing/fwd-tdme.h>
#include <tdme/engine/subsystems/postprocessing/PostProcessingShaderBaseImplementation.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>

using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::Engine;

/**
 * Post processing shader desaturation implementation
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::subsystems::postprocessing::PostProcessingShaderDesaturationImplementation: public PostProcessingShaderBaseImplementation
{
private:
	int32_t uniformIntensity { -1 };

public:
	/**
	 * Returns if shader is supported on given renderer
	 * @param renderer renderer
	 * @return if shader is supported
	 */
	static bool isSupported(Renderer* renderer);

	/**
	 * Public constructor
	 * @param renderer renderer
	 */
	PostProcessingShaderDesaturationImplementation(Renderer* renderer);

	// overridden methods
	virtual void initialize() override;
	virtual void setShaderParameters(void* context, Engine* engine) override;

};
