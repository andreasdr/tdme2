#pragma once

#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/engine/subsystems/rendering/TransparentRenderPoint.h>
#include <tdme/utilities/fwd-tdme.h>
#include <tdme/utilities/FloatBuffer.h>
#include <tdme/utilities/ShortBuffer.h>

using std::to_string;
using std::vector;

using tdme::engine::model::Color4;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::subsystems::rendering::TransparentRenderPoint;
using tdme::utilities::ByteBuffer;
using tdme::utilities::FloatBuffer;
using tdme::utilities::ShortBuffer;

/**
 * Batch renderer for points
 * @author andreas.drewke
 * @version $Id$
 */
class tdme::engine::subsystems::rendering::BatchRendererPoints final
{
	friend class EntityRenderer;

private:
	static constexpr int32_t POINT_COUNT { 65535 };
	Renderer* renderer { nullptr };
	vector<int32_t>* vboIds { nullptr };
	int32_t id;
	bool acquired;
	ByteBuffer* fbVerticesByteBuffer { nullptr };
	FloatBuffer fbVertices;
	ByteBuffer* sbTextureSpriteIndicesByteBuffer { nullptr };
	ShortBuffer sbTextureSpriteIndices;
	ByteBuffer* fbColorsByteBuffer { nullptr };
	FloatBuffer fbColors;
	ByteBuffer* fbPointSizesByteBuffer { nullptr };
	FloatBuffer fbPointSizes;
	ByteBuffer* sbSpriteSheetDimensionByteBuffer { nullptr };
	ShortBuffer sbSpriteSheetDimension;
	ByteBuffer* fbEffectColorMulByteBuffer { nullptr };
	FloatBuffer fbEffectColorMul;
	ByteBuffer* fbEffectColorAddByteBuffer { nullptr };
	FloatBuffer fbEffectColorAdd;

	ByteBuffer* fbTextureSpriteIndicesByteBuffer { nullptr };
	FloatBuffer fbTextureSpriteIndices;
	ByteBuffer* fbSpriteSheetDimensionByteBuffer { nullptr };
	FloatBuffer fbSpriteSheetDimension;

	/**
	 * Public constructor
	 */
	BatchRendererPoints(Renderer* renderer, int32_t id);

	/**
	 * Destructor
	 */
	~BatchRendererPoints();

	/**
	 * Render
	 * @param context context
	 */
	void render(void* context);

	/**
	 * Clears this batch vbo renderer
	 */
	void clear();

	/**
	 * Adds a transparent render point to this transparent render points
	 * @param point transparent render point
	 */
	inline void addPoint(const TransparentRenderPoint* point, int textureIndex, float pointSize, const Color4& effectColorMul, const Color4& effectColorAdd, int textureHorizontalSprites, int textureVerticalSprites) {
		fbVertices.put(point->point.getArray());
		sbTextureSpriteIndices.put(textureIndex);
		sbTextureSpriteIndices.put(point->spriteIndex);
		fbColors.put(point->color.getArray());
		fbPointSizes.put(pointSize);
		sbSpriteSheetDimension.put(textureHorizontalSprites);
		sbSpriteSheetDimension.put(textureVerticalSprites);
		fbEffectColorMul.put(effectColorMul.getArray());
		fbEffectColorAdd.put(effectColorAdd.getArray());
	}

	/**
	 * Adds a transparent render point to this transparent render points
	 * @param point transparent render point
	 */
	inline void addPointNoInteger(const TransparentRenderPoint* point, int textureIndex, float pointSize, const Color4& effectColorMul, const Color4& effectColorAdd, int textureHorizontalSprites, int textureVerticalSprites) {
		fbVertices.put(point->point.getArray());
		fbTextureSpriteIndices.put(static_cast<float>(textureIndex + 0.1f));
		fbTextureSpriteIndices.put(static_cast<float>(point->spriteIndex + 0.1f));
		fbColors.put(point->color.getArray());
		fbPointSizes.put(pointSize);
		fbSpriteSheetDimension.put(static_cast<float>(textureHorizontalSprites + 0.1f));
		fbSpriteSheetDimension.put(static_cast<float>(textureVerticalSprites + 0.1f));
		fbEffectColorMul.put(effectColorMul.getArray());
		fbEffectColorAdd.put(effectColorAdd.getArray());
	}

	/**
	 * @return has points
	 */
	inline bool hasPoints() {
		return fbVertices.getPosition() > 0;
	}

	inline int getPointCount() {
		return fbVertices.getPosition() / 3;
	}

public:

	/**
	 * @return acquired
	 */
	inline bool isAcquired() {
		return acquired;
	}

	/**
	 * Acquire
	 */
	inline bool acquire() {
		if (acquired == true) return false;
		acquired = true;
		return true;
	}

	/**
	 * Release
	 */
	inline void release() {
		acquired = false;
	}

	/**
	 * Init
	 */
	void initialize();

	/**
	 * Dispose
	 */
	void dispose();
};
