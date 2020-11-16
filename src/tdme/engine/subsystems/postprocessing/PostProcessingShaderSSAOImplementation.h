#pragma once

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/subsystems/postprocessing/fwd-tdme.h>
#include <tdme/engine/subsystems/postprocessing/PostProcessingShaderBaseImplementation.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>

using tdme::engine::subsystems::renderer::Renderer;

/**
 * Post processing shader SSAO rendering implementation
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::subsystems::postprocessing::PostProcessingShaderSSAOImplementation: public PostProcessingShaderBaseImplementation
{
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
	PostProcessingShaderSSAOImplementation(Renderer* renderer);

	// overridden methods
	virtual void initialize() override;

};
