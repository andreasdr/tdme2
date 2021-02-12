#pragma once

#include <tdme/utilities/fwd-tdme.h>

#include <set>
#include <vector>

#include <tdme/engine/fileio/textures/fwd-tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/math/Vector3.h>

using std::set;
using std::vector;

using tdme::engine::fileio::textures::Texture;
using tdme::engine::model::Model;
using tdme::engine::primitives::BoundingBox;
using tdme::math::Vector3;

/**
 * Terrain tool
 * @author Andreas Drewke
 */
class tdme::utilities::Terrain
{
public:
	static constexpr float STEP_SIZE { 1.0f };
	static constexpr float PARTITION_SIZE { 64.0f };

private:
	/**
	 * Get the terrain vertex for given x and z position
	 * @param terrainHeightVector terrain height vector
	 * @param verticesPerX vertices per x
	 * @param verticesPerZ vertices per z
	 * @param x x
	 * @param z z
	 * @return terrain vertex
	 */
	static inline bool getTerrainVertex(const vector<float>& terrainHeightVector, int verticesPerX, int verticesPerZ, int x, int z, Vector3& vertex) {
		vertex.set(
			static_cast<float>(x) * STEP_SIZE,
			0.0f,
			static_cast<float>(z) * STEP_SIZE
		);
		if (x < 0 || x >= verticesPerX) return false;
		if (z < 0 || z >= verticesPerZ) return false;
		vertex.set(
			static_cast<float>(x) * STEP_SIZE,
			terrainHeightVector[z * verticesPerX + x],
			static_cast<float>(z) * STEP_SIZE
		);
		return true;
	}

	/**
	 * Compute terrain vertex normal for given x and z position
	 * @param terrainHeightVector terrain height vector
	 * @param verticesPerX vertices per x
	 * @param verticesPerZ vertices per z
	 * @param x x
	 * @param z z
	 * @return normal for given vertex index
	 */
	static const Vector3 computeTerrainVertexNormal(const vector<float>& terrainHeightVector, int verticesPerX, int verticesPerZ, int x, int z);

	/**
	 * Determine if water can be generated
	 * @param terrainHeightVector terrain height vector
	 * @param verticesPerX vertices per x
	 * @param verticesPerZ vertices per z
	 * @param x x
	 * @param z z
	 * @param waterHeight water height
	 * @return if water can be generated at given position
	 */
	static bool determineWater(const vector<float>& terrainHeightVector, int verticesPerX, int verticesPerZ, int x, int z, float waterHeight) {
		if (x < 0 || x >= verticesPerX ||
			z < 0 || z >= verticesPerZ) return false;
		auto vertexIdx = z * verticesPerX + x;
		auto terrainVertexHeight = terrainHeightVector[vertexIdx];
		if (terrainVertexHeight >= waterHeight) return false;
		return true;
	}

	/**
	 * Determine if water can be generated from left to right starting with x and z
	 * @param terrainHeightVector terrain height vector
	 * @param verticesPerX vertices per x
	 * @param verticesPerZ vertices per z
	 * @param x x
	 * @param z z
	 * @param waterHeight water height
	 * @param waterXPositionSet water x position set
	 * @return if water can be generated at given position
	 */
	static void determineWaterXPositionSet(const vector<float>& terrainHeightVector, int verticesPerX, int verticesPerZ, int x, int z, float waterHeight, set<int>& waterXPositionSet) {
		auto xMin = -1;
		auto xMax = +1;
		while(true == true) {
			auto terrainHeightVectorX = x + xMin;
			if (terrainHeightVectorX < 0 || terrainHeightVectorX >= verticesPerX ||
				z < 0 || z >= verticesPerZ) break;
			auto vertexIdx = z * verticesPerX + terrainHeightVectorX;
			auto terrainVertexHeight = terrainHeightVector[vertexIdx];
			if (terrainVertexHeight >= waterHeight) break;
			waterXPositionSet.insert(terrainHeightVectorX);
			xMin--;
		}
		while(true == true) {
			auto terrainHeightVectorX = x + xMax;
			if (terrainHeightVectorX < 0 || terrainHeightVectorX >= verticesPerX ||
				z < 0 || z >= verticesPerZ) break;
			auto vertexIdx = z * verticesPerX + terrainHeightVectorX;
			auto terrainVertexHeight = terrainHeightVector[vertexIdx];
			if (terrainVertexHeight >= waterHeight) break;
			waterXPositionSet.insert(terrainHeightVectorX);
			xMax++;
		}
	}

public:
	enum BrushOperation {
		BRUSHOPERATION_ADD,
		BRUSHOPERATION_SUBTRACT,
		BRUSHOPERATION_FLATTEN,
		BRUSHOPERATION_DELETE,
		BRUSHOPERATION_SMOOTH,
		BRUSHOPERATION_WATER,
	};

	/**
	 * Create terrain models
	 * @param width width
	 * @param depth depth
	 * @param y float y
	 * @param terrainHeightVector terrain height vector
	 * @param terrainBoundingBox terrain bounding box
	 * @param terrainModels terrain models vector
	 */
	static void createTerrainModels(float width, float depth, float y, vector<float>& terrainHeightVector, BoundingBox& terrainBoundingBox, vector<Model*>& terrainModels);

	/**
	 * Apply brush to given terrain models
	 * @param terrainBoundingBox terrain bounding box
	 * @param terrainModels terrain models vector
	 * @param terrainHeightVector terrain height vector
	 * @param brushCenterPosition brush center position
	 * @param brushTexture brush texture
	 * @param brushScale brush scale
	 * @param brushStrength brush strength
	 * @param brushOperation brush operation
	 * @param flattenHeight flatten height
	 *
	 */
	static void applyBrushToTerrainModels(
		BoundingBox& terrainBoundingBox, // TODO: constness
		vector<Model*>& terrainModels,
		vector<float>& terrainHeightVector,
		const Vector3& brushCenterPosition,
		Texture* brushTexture,
		float brushScale,
		float brushStrength,
		BrushOperation brushOperation,
		float flattenHeight = 0.0f
	);

	/**
	 * Get terrain models height for e.g. flatten
	 * @param terrainBoundingBox terrain bounding box
	 * @param terrainModels terrain models vector
	 * @param terrainHeightVector terrain height vector
	 * @param brushHeight brush height
	 *
	 */
	static bool getTerrainModelsHeight(
		BoundingBox& terrainBoundingBox,  // TODO: constness
		vector<Model*>& terrainModels,
		vector<float>& terrainHeightVector,
		const Vector3& brushCenterPosition,
		float& brushHeight
	);

};
