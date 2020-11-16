#pragma once

#include <tdme/tdme.h>
#include <tdme/engine/subsystems/framebuffer/fwd-tdme.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>

using tdme::engine::subsystems::renderer::Renderer;

/**
 * Frame buffer render shader
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::subsystems::framebuffer::FrameBufferRenderShader final
{

private:
	Renderer* renderer { nullptr };
	int32_t vertexShaderId { -1 };
	int32_t fragmentShaderId { -1 };
	int32_t programId { -1 };
	int32_t uniformColorBufferTextureUnit { -1 };
	int32_t uniformDepthBufferTextureUnit { -1 };
	int32_t vboVertices { -1 };
	int32_t vboTextureCoordinates {-1  };
	bool initialized;
	bool isRunning;

public:
	/**
	 * Public constructor
	 * @param renderer renderer
	 */
	FrameBufferRenderShader(Renderer* renderer);

	/**
	 * Public destructor
	 */
	~FrameBufferRenderShader();

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
	 * @return vertices VBO
	 */
	inline int32_t getVBOVertices() {
		return vboVertices;
	}

	/**
	 * @return texture coordinates VBO
	 */
	inline int32_t getVBOTextureCoordinates() {
		return vboTextureCoordinates;
	}

};
