#pragma once

#include <map>
#include <set>
#include <stack>
#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/Transformations.h>
#include <tdme/engine/physics/World.h>
#include <tdme/engine/primitives/BoundingVolume.h>
#include <tdme/math/Math.h>
#include <tdme/math/Vector3.h>
#include <tdme/utils/Console.h>
#include <tdme/utils/FlowMap.h>
#include <tdme/utils/FlowMapCell.h>
#include <tdme/utils/PathFindingCustomTest.h>

using std::map;
using std::set;
using std::stack;
using std::string;
using std::to_string;
using std::vector;

using tdme::engine::Transformations;
using tdme::engine::physics::World;
using tdme::engine::primitives::BoundingVolume;
using tdme::math::Math;
using tdme::math::Vector3;
using tdme::utils::Console;
using tdme::utils::FlowMap;
using tdme::utils::FlowMapCell;
using tdme::utils::PathFindingCustomTest;

/**
 * Path finding class
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::utils::PathFinding final
{
public:
	static constexpr bool VERBOSE { false };

	/**
	 * Public constructor
	 * @param world world
	 * @param sloping sloping
	 * @param stepsMax steps max
	 * @param actorHeight actor height
	 * @param stepSize step size
	 * @param stepSizeLast step size last
	 * @param actorStepUpMax actor step up max
	 * @param skipOnCollisionTypeIds skip cells with given collision type ids
	 * @param maxTries max tries
	 * @param flowMapStepSize flow map step size
	 * @param flowMapScaleActorBoundingVolumes flow map scale actor bounding volumes
	 */
	PathFinding(
		World* world,
		bool sloping = false,
		int stepsMax = 1000,
		float actorHeight = 2.0f,
		float stepSize = 0.5f,
		float stepSizeLast = 0.75f,
		float actorStepUpMax = 0.25f,
		uint16_t skipOnCollisionTypeIds = 0,
		int maxTries = 5,
		float flowMapStepSize = 0.5f,
		float flowMapScaleActorBoundingVolumes = 1.0f
	);

	/**
	 * Destructor
	 */
	~PathFinding();

	/**
	 * @return step size
	 */
	inline float getStepSize() {
		return stepSize;
	}

	/**
	 * Clear caches
	 */
	inline void reset() {
		walkableCache.clear();
	}

	/**
	 * Return string representation of given x,y,z for path finding id
	 * @param x x
	 * @param y y
	 * @param z z
	 * @param stepSize step size  
	 * @return string representation
	 */
	inline string toId(float x, float y, float z) {
		return toId(x, y, z, stepSize);
	}

	/**
	 * Return string representation of given x,y,z for path finding id
	 * @param x x
	 * @param y y
	 * @param z z
	 * @return string representation
	 */
	inline static string toId(float x, float y, float z, float stepSize) {
		string result;
		int32_t value = 0;
		result.reserve(sizeof(value) * 3);
		value = static_cast<int>(Math::ceil(x / stepSize));
		result.append((char*)&value, sizeof(value));
		value = static_cast<int>(Math::ceil(y / stepSize));
		result.append((char*)&value, sizeof(value));
		value = static_cast<int>(Math::ceil(z / stepSize));
		result.append((char*)&value, sizeof(value));
		return result;
	}

	/**
	 * Align position component
	 * @param value value which is usually a position vector 3 position component
	 * @param stepSize step size
	 */
	inline static float alignPositionComponent(float value, float stepSize) {
		return Math::floor(value / stepSize) * stepSize;
	}

	/**
	 * Align position component
	 * @param value value which is usually a position vector 3 position component
	 */
	inline float alignPositionComponent(float value) {
		return alignPositionComponent(value, stepSize);
	}

	/**
	 * Returns integer position component
	 * @param value value
	 * @param stepSize step size
	 * @return integer position component
	 */
	inline static int getIntegerPositionComponent(float value, float stepSize) {
		return static_cast<int>(alignPositionComponent(value, stepSize) / stepSize);
	}

	/**
	 * Returns integer position component
	 * @param value value
	 * @return integer position component
	 */
	inline int getIntegerPositionComponent(float value) {
		return getIntegerPositionComponent(value, stepSize);
	}

	/**
	 * Return string representation of given x,z integer flow map position representation for path finding id
	 * @param x x
	 * @param y y
	 * @param z z
	 * @return string representation
	 */
	inline static string toIdInt(int x, int y, int z) {
		string result;
		result.reserve(sizeof(x) * 3);
		result.append((char*)&x, sizeof(x));
		result.append((char*)&y, sizeof(y));
		result.append((char*)&z, sizeof(z));
		return result;
	}

	/**
	 * Finds path to given end position
	 * @param startPosition start position
	 * @param endPosition end position
	 * @param collisionTypeIds collision type ids
	 * @param path path from actor to target
	 * @param alternativeEndSteps alternative end steps
	 * @param customTest custom test
	 * @return success
	 */
	inline bool findPath(const Vector3& startPosition, const Vector3& endPosition, const uint16_t collisionTypeIds, vector<Vector3>& path, int alternativeEndSteps = 0, PathFindingCustomTest* customTest = nullptr) {
		return findPathCustom(startPosition, endPosition, stepSize, 1.0f, collisionTypeIds, path, alternativeEndSteps, customTest);
	}


	/**
	 * Finds path to given end position for flow maps
	 * @param startPosition start position
	 * @param endPosition end position
	 * @param collisionTypeIds collision type ids
	 * @param path path from actor to target
	 * @param alternativeEndSteps alternative end steps
	 * @param customTest custom test
	 * @return success
	 */
	inline bool findFlowMapPath(const Vector3& startPosition, const Vector3& endPosition, const uint16_t collisionTypeIds, vector<Vector3>& path, int alternativeEndSteps = 0, PathFindingCustomTest* customTest = nullptr) {
		return findPathCustom(startPosition, endPosition, flowMapStepSize, flowMapScaleActorBoundingVolumes, collisionTypeIds, path, alternativeEndSteps, customTest);
	}

	/**
	 * Finds path to given end position
	 * @param startPosition start position
	 * @param endPosition end position
	 * @param stepSize step size
	 * @param scaleActorBoundingVolumes scale actor bounding volumes
	 * @param collisionTypeIds collision type ids
	 * @param path path from actor to target
	 * @param alternativeEndSteps alternative end steps
	 * @param customTest custom test
	 * @return success
	 */
	bool findPathCustom(const Vector3& startPosition, const Vector3& endPosition, float stepSize, float scaleActorBoundingVolumes, const uint16_t collisionTypeIds, vector<Vector3>& path, int alternativeEndSteps = 0, PathFindingCustomTest* customTest = nullptr);

	/**
	 * Checks if a cell is walkable
	 * @param x x
	 * @param y y
	 * @param z z
	 * @param height y stepped up
	 * @param stepSize step size
	 * @param scaleActorBoundingVolumes scale actor bounding volumes
	 * @param collisionTypeIds collision type ids or 0 for default
	 * @param ignoreStepUpMax ignore step up max
	 * @return if cell is walkable
	 */
	bool isWalkable(float x, float y, float z, float& height, float stepSize, float scaleActorBoundingVolumes, uint16_t collisionTypeIds = 0, bool ignoreStepUpMax = false);

	/**
	 * Create flow map
	 * @param endPositions end positions
	 * @param center flow map center
	 * @param depth flow map depth
	 * @param width flow map width
	 * @param collisionTypeIds collision type ids
	 * @param path path to test along
	 * @param complete complete
	 * @param customTest custom test
	 * @return flow map
	 */
	FlowMap* createFlowMap(const vector<Vector3>& endPositions, const Vector3& center, float depth, float width, const uint16_t collisionTypeIds, const vector<Vector3>& path = vector<Vector3>(), bool complete = true, PathFindingCustomTest* customTest = nullptr);

private:
	/**
	 * Path finding node
	 */
	struct PathFindingNode final
	{
		/**
		 * Node id
		 */
		string id;

		/**
		 * Previous node
		 */
		string previousNodeId;

		/**
		 * Position
		 */
		Vector3 position;

		/**
		 * Estimated costs to the end plus calculated costs from start to this node
		 */
		float costsAll;

		/**
		 * Calculated costs up to this point (from all previous nodes up to this node) to get to this coordinates from start
		 */
		float costsReachPoint;

		/**
		 * Estimated costs to get to the end
		 */
		float costsEstimated;

		/**
		 * Integer position along x axis
		 */
		int x;

		/**
		 * Integer position along y axis
		 */
		int y;

		/**
		 * Integer position along z axis
		 */
		int z;
	};

	/**
	 * Computes non square rooted distance between a and b
	 * @param a node a
	 * @param b node b
	 * @return non square rooted distance
	 */
	inline float computeDistance(const PathFindingNode& a, const PathFindingNode& b) {
		return a.position.clone().sub(b.position).computeLengthSquared();
	}

	/**
	 * Computes minimal non square rooted distance between node and end point
	 * @param node node
	 * @return non square rooted distance
	 */
	inline float computeDistanceToEnd(const PathFindingNode& node) {
		return node.position.clone().sub(end.position).computeLengthSquared();
	}

	/**
	 * Returns if nodes are equals
	 * @param a a
	 * @param bX b x coordinate
	 * @param bY b y coordinate
	 * @param bZ b z coordinate
	 * @return if node a == node b
	 */
	inline bool equals(const PathFindingNode& a, float bX, float bY, float bZ) {
		return a.position.clone().sub(Vector3(bX, bY, bZ)).computeLengthSquared() < Math::square(0.1f);
	}

	/**
	 * Returns if nodes are equals for (last node test)
	 * @param a a
	 * @param lastNode b
	 * @return if node a == node b
	 */
	inline bool equalsLastNode(const PathFindingNode& a, const PathFindingNode& b) {
		return a.position.clone().sub(b.position).computeLengthSquared() < Math::square(stepSizeLast);
	}

	/**
	 * Checks if a cell is walkable
	 * @param x x
	 * @param y y
	 * @param z z
	 * @param height y stepped up
	 * @param stepSize step size
	 * @param scaleActorBoundingVolumes scale actor bounding volumes
	 * @param collisionTypeIds collision type ids or 0 for default
	 * @param ignoreStepUpMax ignore step up max
	 * @return if cell is walkable
	 */
	bool isWalkableInternal(float x, float y, float z, float& height, float stepSize, float scaleActorBoundingVolumes, uint16_t collisionTypeIds = 0, bool ignoreStepUpMax = false);

	/**
	 * Checks if a cell is slope walkable
	 * @param x x
	 * @param y y
	 * @param z z
	 * @param successorX x
	 * @param successorY y
	 * @param successorZ z
	 * @param stepSize step size
	 * @param scaleActorBoundingVolumes scale actor bounding volumes
	 * @param collisionTypeIds collision type ids or 0 for default
	 * @return if cell is walkable
	 */
	bool isSlopeWalkableInternal(float x, float y, float z, float successorX, float successorY, float successorZ, float stepSize, float scaleActorBoundingVolumes, uint16_t collisionTypeIds = 0);

	/**
	 * Processes one step in AStar path finding
	 * @param node node
	 * @param stepSize step size
	 * @param scaleActorBoundingVolumes scale actor bounding volumes
	 * @param nodesToTest nodes to test or nullptr, applies to flow cost map generation
	 * @param zeroHeightInId have no height stored in ids, applies to flow cost map generation
	 * @return step status
	 */
	void step(const PathFindingNode& node, float stepSize, float scaleActorBoundingVolumes, const set<string>* nodesToTest, bool zeroHeightInId);

	// properties
	World* world { nullptr };
	PathFindingCustomTest* customTest { nullptr };
	bool sloping;
	int stepsMax;
	float actorHeight;
	float stepSize;
	float stepSizeLast;
	float actorStepUpMax;
	uint16_t skipOnCollisionTypeIds;
	uint16_t collisionTypeIds;
	int maxTries;
	float flowMapStepSize;
	float flowMapScaleActorBoundingVolumes;
	PathFindingNode end;
	stack<PathFindingNode> successorNodes;
	map<string, PathFindingNode> openNodes;
	map<string, PathFindingNode> closedNodes;
	set<string> nodesToTest;
	BoundingVolume* actorBoundingVolume { nullptr };
	BoundingVolume* actorBoundingVolumeSlopeTest { nullptr };
	map<string, float> walkableCache;
};
