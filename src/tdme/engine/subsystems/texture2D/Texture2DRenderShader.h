#pragma once

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/subsystems/texture2D/fwd-tdme.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/math/Vector2.h>

using tdme::engine::subsystems::renderer::Renderer;

using tdme::engine::Engine;
using tdme::math::Vector2;

/**
 * Texture 2D render shader
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::subsystems::texture2D::Texture2DRenderShader final
{

private:
	Renderer* renderer { nullptr };
	int32_t vertexShaderId { -1 };
	int32_t fragmentShaderId { -1 };
	int32_t programId { -1 };
	int32_t uniformTextureUnit { -1 };
	int32_t vboVertices { -1 };
	int32_t vboTextureCoordinates {-1  };
	bool initialized;
	bool isRunning;

public:
	/**
	 * Public constructor
	 * @param renderer renderer
	 */
	Texture2DRenderShader(Renderer* renderer);

	/**
	 * Public destructor
	 */
	~Texture2DRenderShader();

	/**
	 * @return if initialized and ready to use
	 */
	bool isInitialized();

	/**
	 * Initialize
	 */
	void initialize();

	/**
	 * Use render program
	 */
	void useProgram();

	/**
	 * Un use render program
	 */
	void unUseProgram();

	/**
	 * Render texture
	 * @param engine engine to use
	 * @param position 2D position on screen
	 * @param dimension 2D dimenson on screen
	 * @param textureId texture id
	 * @param width optional render target width
	 * @param height optional render target height
	 */
	void renderTexture(Engine* engine, const Vector2& position, const Vector2& dimension, int textureId, int width = -1, int height = -1);

};
