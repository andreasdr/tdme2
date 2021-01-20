#pragma once

#include <tdme/utilities/fwd-tdme.h>

#include <vector>

#include <tdme/engine/fileio/textures/fwd-tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/math/Vector3.h>

using std::vector;

using tdme::engine::fileio::textures::Texture;
using tdme::engine::model::Model;
using tdme::math::Vector3;

/**
 * Terrain tool
 * @author Andreas Drewke
 */
class tdme::utilities::Terrain
{
private:
	/**
	 * @param vertexIdx vertex index
	 * @param verticesPerX vertices per X
	 * @param verticesPerZ vertices per Z
	 * @return terrain model vertex index at top of given vertex index
	 */
	static inline int getTerrainModelTopVertexIdx(int vertexIdx, int verticesPerX, int verticesPerZ) {
		if (vertexIdx == -1) return -1;
		if (vertexIdx < verticesPerX) return -1;
		return vertexIdx - verticesPerX;
	}

	/**
	 * @param vertexIdx vertex index
	 * @param verticesPerX vertices per X
	 * @param verticesPerZ vertices per Z
	 * @return terrain model vertex index at bottom of given vertex index
	 */
	static inline int getTerrainModelBottomVertexIdx(int vertexIdx, int verticesPerX, int verticesPerZ) {
		if (vertexIdx == -1) return -1;
		if (vertexIdx >= (verticesPerZ - 1) * verticesPerX) return -1;
		return vertexIdx + verticesPerX;
	}

	/**
	 * @param vertexIdx vertex index
	 * @param verticesPerX vertices per X
	 * @param verticesPerZ vertices per Z
	 * @return terrain model vertex index at left of given vertex index
	 */
	static inline int getTerrainModelLeftVertexIdx(int vertexIdx, int verticesPerX, int verticesPerZ) {
		if (vertexIdx == -1) return -1;
		if (vertexIdx < 1) return -1;
		return vertexIdx - 1;
	}

	/**
	 * @param vertexIdx vertex index
	 * @param verticesPerX vertices per X
	 * @param verticesPerZ vertices per Z
	 * @return terrain model vertex index at right of given vertex index
	 */
	static inline int getTerrainModelRightVertexIdx(int vertexIdx, int verticesPerX, int verticesPerZ) {
		if (vertexIdx == -1) return -1;
		if (vertexIdx > verticesPerZ * verticesPerX - 1) return -1;
		return vertexIdx + 1;
	}

	/**
	 * Compute terrain vertex normal
	 * @param terrainVerticesVector terrain vertices vector
	 * @param vertexIdx vertex index
	 * @param verticesPerZ vertices per z
	 * @return normal for given vertex index
	 */
	static const Vector3 computeTerrainVertexNormal(const vector<Vector3>& terrainVerticesVector, int vertexIdx, int verticesPerX);

public:
	static constexpr float STEP_SIZE { 0.5f };

	enum BrushOperation {
		BRUSHOPERATION_ADD,
		BRUSHOPERATION_SUBTRACT,
		BRUSHOPERATION_FLATTEN,
		BRUSHOPERATION_DELETE,
		BRUSHOPERATION_SMOOTH
	};

	/**
	 * Creates a terrain model
	 * @param width width
	 * @param depth depth
	 * @param y float y
	 * @param terrainVerticesVector terrain vertices vector
	 * @return ground model
	 */
	static Model* createTerrainModel(float width, float depth, float y, vector<Vector3>& terrainVerticesVector);

	/**
	 * Apply brush to terrain model
	 * @param terrainModel terrain model
	 * @param terrainVerticesVector terrain vertices vector
	 * @param brushCenterPosition brush center position
	 * @param brushTexture brush texture
	 * @param brushScale brush scale
	 * @param brushStrength brush strength
	 * @param brushOperation brush operation
	 * @param flattenHeight flatten height
	 *
	 */
	static void applyBrushToTerrainModel(
		Model* terrainModel,
		vector<Vector3>& terrainVerticesVector,
		const Vector3& brushCenterPosition,
		Texture* brushTexture,
		float brushScale,
		float brushStrength,
		BrushOperation brushOperation,
		float flattenHeight = 0.0f
	);

	/**
	 * Get terrain model flatten height
	 * @param terrainModel terrain model
	 * @param terrainVerticesVector terrain vertices vector
	 * @param flattenHeight flatten height
	 *
	 */
	static bool getTerrainModelFlattenHeight(
		Model* terrainModel,
		vector<Vector3>& terrainVerticesVector,
		const Vector3& brushCenterPosition,
		float& flattenHeight
	);
};
