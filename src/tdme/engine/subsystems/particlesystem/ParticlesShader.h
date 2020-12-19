#pragma once

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/subsystems/particlesystem/fwd-tdme.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Matrix4x4.h>

using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::Engine;
using tdme::math::Matrix4x4;

/**
 * Particles shader program
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::subsystems::particlesystem::ParticlesShader final
{
private:
	int32_t renderProgramId { -1 };
	int32_t renderFragmentShaderId { -1 };
	int32_t renderVertexShaderId { -1 };
	int32_t uniformMVPMatrix { -1 };
	array<int32_t, 16> uniformDiffuseTextureUnits;
	int32_t uniformViewPortWidth { -1 };
	int32_t uniformViewPortHeight { -1 };
	int32_t uniformProjectionMatrixXx { -1 };
	int32_t uniformProjectionMatrixYy { -1 };
	Matrix4x4 mvpMatrix;
	bool isRunning;
	bool initialized;
	Engine* engine { nullptr };
	Renderer* renderer { nullptr };
	array<int32_t, 16> boundTextureIds;

public:
	/**
	 * Public constructor
	 * @param engine engine
	 * @param renderer renderer
	 */
	ParticlesShader(Engine* engine, Renderer* renderer);

	/**
	 * @return initialized and ready to be used
	 */
	bool isInitialized();

	/**
	 * Initialize renderer
	 */
	void initialize();

	/**
	 * Use lighting program
	 * @param context context
	 */
	void useProgram(void* context);

	/**
	 * Update effect to program
	 * @param context context
	 */
	void updateEffect(void* context);

	/**
	 * Unuse particles shader program
	 * @param context context
	 */
	void unUseProgram(void* context);

	/**
	 * Update matrices to program
	 * @param context context
	 */
	void updateMatrices(void* context);

	/**
	 * Set parameters
	 * @param context context
	 * @param textureIds texture ids
	 */
	void setParameters(void* context, const array<int32_t, 16>& textureIds);

};
