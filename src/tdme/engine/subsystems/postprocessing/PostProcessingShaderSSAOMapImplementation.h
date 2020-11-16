#pragma once

#include <array>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/subsystems/postprocessing/fwd-tdme.h>
#include <tdme/engine/subsystems/postprocessing/PostProcessingShaderBaseImplementation.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>

using std::array;

using tdme::engine::subsystems::renderer::Renderer;

/**
 * Post processing shader SSAO map generation implementation
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::subsystems::postprocessing::PostProcessingShaderSSAOMapImplementation: public PostProcessingShaderBaseImplementation
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
	PostProcessingShaderSSAOMapImplementation(Renderer* renderer);

	// overridden methods
	virtual void initialize() override;
	virtual void useProgram(void* context) override;

private:
	array<int32_t, 16> uniformSphere;
	int32_t uniformRandomTextureUnit { -1 };
	int32_t randomTextureId { -1 };
};
