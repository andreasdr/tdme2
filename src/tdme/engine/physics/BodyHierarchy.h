#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <reactphysics3d/collision/Collider.h>

#include <tdme/tdme.h>
#include <tdme/engine/physics/fwd-tdme.h>
#include <tdme/engine/primitives/fwd-tdme.h>
#include <tdme/engine/Transform.h>

using std::string;
using std::unordered_map;
using std::vector;

using tdme::engine::physics::World;
using tdme::engine::primitives::BoundingVolume;
using tdme::engine::Transform;

/**
 * Body hierarchy
 * @author Andreas Drewke
 */
class tdme::engine::physics::BodyHierarchy final: public Body
{
	friend class World;

protected:

	// forbid class copy
	FORBID_CLASS_COPY(BodyHierarchy)

	/**
	 * Protected constructor
	 * @param world world
	 * @param id id
	 * @param type type
	 * @param collisionTypeId collision type id
	 * @param enabled enabled
	 * @param transform transform
	 * @param restitution restitution
	 * @param friction friction
	 * @param mass mass in kg
	 * @param inertiaTensor inertia tensor vector
	 * @param boundingVolumes bounding volumes
	 */
	BodyHierarchy(World* world, const string& id, BodyType type, uint16_t collisionTypeId, bool enabled, const Transform& transform, float restitution, float friction, float mass, const Vector3& inertiaTensor);

	/**
	 * Destructor
	 */
	~BodyHierarchy();

	/**
	 * Reset colliders
	 */
	void resetColliders() override;

private:
	struct BodyHierarchyLevel {
		BodyHierarchyLevel(const string& id, BodyHierarchyLevel* parent, const Transform& transform, const vector<BoundingVolume*>& boundingVolumes): id(id), parent(parent), transform(transform), boundingVolumes(boundingVolumes) {}
		string id;
		BodyHierarchyLevel* parent { nullptr };
		Transform transform;
		vector<BoundingVolume*> boundingVolumes;
		vector<reactphysics3d::Collider*> colliders;
		unordered_map<string, BodyHierarchyLevel*> children;
	};
	typedef BodyHierarchyLevel SubBody;
	vector<SubBody*> bodies;
	BodyHierarchyLevel bodyRoot { string(), nullptr, Transform(), {} };

	/**
	 * Get body hierarchy level by given body id
	 * @param id id
	 * @returns body hierarchy level
	 *
	 */
	inline BodyHierarchyLevel* getBodyHierarchyLevel(const string& id) {
		if (id.empty()) return &bodyRoot;
		return getBodyHierarchyLevel(&bodyRoot, id);
	}

	/**
	 * Retrieve body hierarchy level by given body id or nullptr if not found
	 * @param bodyHierarchyLevel body hierarchy level
	 * @param id body id
	 * @returns body hierarchy level by given body id or nullptr if not found
	 */
	inline BodyHierarchyLevel* getBodyHierarchyLevel(BodyHierarchyLevel* bodyHierarchyLevel, const string& id) {
		if (id == bodyHierarchyLevel->id) return bodyHierarchyLevel;
		for (const auto& [childId, child]: bodyHierarchyLevel->children) {
			auto childBodyHierarchyLevel = getBodyHierarchyLevel(child, id);
			if (childBodyHierarchyLevel != nullptr) return childBodyHierarchyLevel;
		}
		return nullptr;
	}

	/**
	 * Update hierarchy from given body hierarchy level ongoing
	 * @param parentTransform parent transform
	 * @param bodyHierarchyLevel body hierarchy level
	 * @param depth depth
	 */
	void updateHierarchy(const Transform& parentTransform, BodyHierarchyLevel* bodyHierarchyLevel, int depth);

public:

	/**
	 * @returns body from hierarchy by given unique id
	 */
	inline const SubBody* getBody(const string& id) {
		auto bodyHierarchyLevel = getBodyHierarchyLevel(id);
		if (bodyHierarchyLevel == nullptr || bodyHierarchyLevel->parent == nullptr) return nullptr;
		return bodyHierarchyLevel;
	}

	/**
	 * Adds a body to the hierarchy
	 * @param id unique body id in hierarchy
	 * @param transform transform
	 * @param boundingVolumes bounding volumes
	 * @param parentId parent body id to add body to
	 */
	void addBody(const string& id, const Transform& transform, const vector<BoundingVolume*>& boundingVolumes, const string& parentId = string());

	/**
	 * Updates a body from hierarchy by given unique body id
	 * @param id unique body id in hierarchy
	 * @param transform transform
	 */
	void updateBody(const string& id, const Transform& transform);

	/**
	 * Removes a body from hierarchy by given unique body id
	 * @param id unique body id in hierarchy
	 */
	void removeBody(const string& id);

	/**
	 * Query sub bodies of parent body
	 * @param parentId parent body id
	 * @returns bodies
	 */
	const vector<SubBody*> query(const string& parentId = string());

	/**
	 * @returns bodies
	 */
	inline const vector<SubBody*>& getBodies() {
		return bodies;
	}

	/**
	 * Update body hierarchy
	 */
	void update();
};
