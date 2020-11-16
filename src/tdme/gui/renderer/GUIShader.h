#pragma once

#include <tdme/tdme.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/gui/renderer/fwd-tdme.h>

using tdme::engine::subsystems::renderer::Renderer;

/**
 * GUI shader
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::gui::renderer::GUIShader final
{

private:
	Renderer* renderer { nullptr };
	int32_t vertexShaderId { -1 };
	int32_t fragmentShaderId { -1 };
	int32_t programId { -1 };
	int32_t uniformDiffuseTextureUnit { -1 };
	int32_t uniformDiffuseTextureAvailable { -1 };
	int32_t uniformMaskTextureUnit { -1 };
	int32_t uniformMaskTextureAvailable { -1 };
	int32_t uniformMaskMaxValue { -1 };
	int32_t uniformEffectColorMul { -1 };
	int32_t uniformEffectColorAdd { -1 };
	int32_t uniformTextureMatrix { -1 };
	bool initialized;
	bool isRunning;

public:
	/**
	 * Public constructor
	 * @param renderer renderer
	 */
	GUIShader(Renderer* renderer);

	/**
	 * @return if initialized and ready to use
	 */
	bool isInitialized();

	/**
	 * Init shadow mapping
	 */
	void initialize();

	/**
	 * Use render GUI program
	 */
	void useProgram();

	/**
	 * Un use render GUI program
	 */
	void unUseProgram();

	/**
	 * Bind texture
	 * @param textureId texture id
	 */
	void bindTexture(int32_t textureId);

	/**
	 * Update effect to program
	 */
	void updateEffect();

	/**
	 * Update texure matrix to program
	 */
	void updateTextureMatrix();

};
