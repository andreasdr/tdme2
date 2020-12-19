#pragma once

#include <array>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/TextureCoordinate.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/engine/subsystems/rendering/fwd-tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector2.h>
#include <tdme/math/Vector3.h>
#include <tdme/utilities/fwd-tdme.h>
#include <tdme/utilities/FloatBuffer.h>

using std::array;
using std::vector;

using tdme::engine::model::Color4;
using tdme::engine::model::TextureCoordinate;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::math::Matrix4x4;
using tdme::math::Vector2;
using tdme::math::Vector3;
using tdme::utilities::ByteBuffer;
using tdme::utilities::FloatBuffer;

/**
 * Batch renderer for transparent triangles
 * @author andreas.drewke
 * @version $Id$
 */
class tdme::engine::subsystems::rendering::BatchRendererTriangles final
{
	friend class TransparentRenderFacesGroup;

private:
	static constexpr int32_t TRIANGLE_COUNT { 1024 };
	static constexpr int32_t VERTEX_COUNT { TRIANGLE_COUNT * 3 };
	Renderer* renderer { nullptr };
	vector<int32_t>* vboIds { nullptr };
	int32_t id;
	bool acquired;
	int32_t vertices;
	ByteBuffer* fbModelMatricesByteBuffer{ nullptr };
	FloatBuffer fbModelMatrices;
	ByteBuffer* fbEffectColorMulsByteBuffer{ nullptr };
	FloatBuffer fbEffectColorMuls;
	ByteBuffer* fbEffectColorAddsByteBuffer{ nullptr };
	FloatBuffer fbEffectColorAdds;
	ByteBuffer* fbVerticesByteBuffer{ nullptr };
	FloatBuffer fbVertices;
	ByteBuffer* fbNormalsByteBuffer { nullptr };
	FloatBuffer fbNormals;
	ByteBuffer* fbTextureCoordinatesByteBuffer { nullptr };
	FloatBuffer fbTextureCoordinates;

	/**
	 * Clears this batch vbo renderer
	 */
	void clear();

	/**
	 * Render
	 */
	void render();

	/**
	 * Adds a vertex to this transparent render faces group
	 * @param vertex vertex
	 * @param normal normal
	 * @param textureCoordinate texture coordinate
	 * @return success
	 */
	inline bool addVertex(const Vector3& vertex, const Vector3& normal, const Vector2& textureCoordinate) {
		// check if full
		if (vertices == VERTEX_COUNT) return false;

		// otherwise
		fbVertices.put(vertex.getArray());
		fbNormals.put(normal.getArray());
		fbTextureCoordinates.put(textureCoordinate.getArray());

		//
		vertices++;
		return true;
	}

public:
	/**
	 * Public constructor
	 * @param renderer renderer
	 * @param id id
	 */
	BatchRendererTriangles(Renderer* renderer, int32_t id);

	/**
	 * Destructor
	 */
	~BatchRendererTriangles();

	/**
	 * @return acquired
	 */
	bool isAcquired();

	/**
	 * Acquire
	 */
	bool acquire();

	/**
	 * Release
	 */
	void release();

	/**
	 * Init
	 */
	void initialize();

	/**
	 * Dispose
	 */
	void dispose();

};
