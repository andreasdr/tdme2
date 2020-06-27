#pragma once

#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/utils/fwd-tdme.h>
#include <tdme/engine/subsystems/framebuffer/fwd-tdme.h>
#include <tdme/engine/subsystems/lines/fwd-tdme.h>
#include <tdme/engine/subsystems/rendering/fwd-tdme.h>
#include <tdme/engine/subsystems/skinning/fwd-tdme.h>

using tdme::utils::ByteBuffer;

using std::vector;

/** 
 * Buffers used to transfer data between main memory to graphics board memory
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::subsystems::rendering::ObjectBuffer
{
	friend class Object3DGroupMesh;
	friend class EntityRenderer;
	friend class BatchRendererTriangles;
	friend class tdme::engine::Engine;
	friend class tdme::engine::subsystems::framebuffer::FrameBufferRenderShader;
	friend class tdme::engine::subsystems::framebuffer::FrameBufferRenderShader;
	friend class tdme::engine::subsystems::lines::LinesObject3DInternal;
	friend class tdme::engine::subsystems::rendering::Object3DGroupRenderer;
	friend class tdme::engine::subsystems::skinning::SkinningShader;

private:
	static vector<ByteBuffer*> byteBuffers;

	/**
	 * Initialize object buffer per thread
	 */
	static void initialize();

public:
	/** 
	 * Get byte buffer for given context
	 * @param bytes bytes
	 * @return byte buffer
	 */
	static ByteBuffer* getByteBuffer(void* context, int32_t bytes);

};
