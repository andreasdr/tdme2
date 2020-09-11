#pragma once

#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/physics/fwd-tdme.h>
#include <tdme/engine/physics/CollisionResponse_Entity.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Math.h>
#include <tdme/math/Vector3.h>
#include <tdme/utilities/fwd-tdme.h>
#include <tdme/utilities/Console.h>

using std::vector;

using tdme::engine::physics::CollisionResponse;
using tdme::engine::physics::CollisionResponse_Entity;
using tdme::math::Math;
using tdme::math::Vector3;
using tdme::utilities::Console;

/** 
 * Collision response
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::physics::CollisionResponse final
{
	friend class CollisionResponse_Entity;
	friend class CollisionDetection;
	friend class World;

private:
	static constexpr int32_t HITPOINT_COUNT { 30 };
	vector<CollisionResponse_Entity> entities;
	CollisionResponse_Entity* selectedEntity { nullptr };
	vector<Vector3> fallbackHitPointsVector;

public:

	/** 
	 * Reset
	 */
	inline void reset() {
		entities.clear();
		selectedEntity = nullptr;
	}

	/** 
	 * Adds a collision response entity 
	 * @param distance distance
	 * @return Entity or null
	 */
	inline CollisionResponse_Entity* addResponse(float distance) {
		entities.push_back(CollisionResponse_Entity());
		auto& entity = entities[entities.size() - 1];
		entity.distance = distance;
		// select entity with smallest penetration by default
		if (selectedEntity == nullptr || distance > selectedEntity->distance) {
			selectedEntity = &entity;
		}
		return &entity;
	}

	/** 
	 * @return entity count
	 */
	inline int32_t getEntityCount() {
		return entities.size();
	}

	/** 
	 * @return selected entity
	 */
	inline CollisionResponse_Entity* getSelectedEntity() {
		return selectedEntity;
	}

	/** 
	 * Selects entity at given index
	 * @param idx idx
	 * @return
	 */
	inline CollisionResponse_Entity* getEntityAt(int32_t idx) {
		if (idx < 0 || idx >= entities.size()) return nullptr;
		return &entities[idx];
	}

	/** 
	 * Selects entity at given index
	 * @param idx idx
	 * @return
	 */
	inline CollisionResponse* selectEntityAt(int32_t idx) {
		if (idx < 0 || idx >= entities.size()) return this;
		selectedEntity = &entities[idx];
		return this;
	}

	inline bool hasEntitySelected() {
		return selectedEntity != nullptr;
	}

	inline float getDistance() {
		if (selectedEntity == nullptr) return 0.0f;
		return selectedEntity->distance;
	}

	inline bool hasPenetration() {
		if (selectedEntity == nullptr) return false;
		return selectedEntity->distance < -Math::EPSILON;
	}

	inline float getPenetration() {
		if (selectedEntity == nullptr) return 0.0f;
		return -selectedEntity->distance;
	}

	inline Vector3* getNormal() {
		if (selectedEntity == nullptr) return nullptr;
		return &selectedEntity->normal;
	}

	/**
	 * @return get hit points
	 */
	inline const vector<Vector3>& getHitPoints() {
		if (selectedEntity == nullptr) return fallbackHitPointsVector;
		return selectedEntity->hitPoints;
	}

	/** 
	 * @return hit points count
	 */
	inline int32_t getHitPointsCount() {
		if (selectedEntity == nullptr) return 0;
		return selectedEntity->hitPoints.size();
	}

	/** 
	 * Get hit point of given index 
	 * @param i i
	 * @return hit point for given hit points index
	 */
	inline Vector3* getHitPointAt(int32_t i) {
		if (selectedEntity == nullptr) return nullptr;
		return &selectedEntity->hitPoints[i];
	}

	/**
	 * Public constructor
	 */
	inline CollisionResponse() {
	}

};
