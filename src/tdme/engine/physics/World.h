#pragma once

#include <map>
#include <string>
#include <vector>


#include <ext/reactphysics3d/src/engine/DynamicsWorld.h>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/physics/fwd-tdme.h>
#include <tdme/engine/primitives/fwd-tdme.h>
#include <tdme/engine/Transformations.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/utilities/fwd-tdme.h>

using std::map;
using std::string;
using std::vector;

using tdme::engine::physics::Body;
using tdme::engine::physics::CollisionResponse;
using tdme::engine::physics::WorldListener;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::primitives::BoundingVolume;
using tdme::engine::Engine;
using tdme::engine::Transformations;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;

/**
 * Dynamic physics world class
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::physics::World final
{
	friend class Body;

private:
	struct BodyCollisionStruct {
		string body1Id;
		string body2Id;
	};

	reactphysics3d::DynamicsWorld world;

	vector<Body*> bodies;
	vector<Body*> rigidBodiesDynamic;
	map<string, Body*> bodiesById;
	map<string, BodyCollisionStruct> bodyCollisionsLastFrame;
	vector<WorldListener*> worldListeners;

	/**
	 * Synch into cloned body from body
	 * @param clonedBody cloned body
	 * @param body body
	 */
	void synch(Body* clonedBody, Body* body);

public:
	/**
	 * Public constructor
	 */
	World();

	/**
	 * Destructor
	 */
	~World();

	/**
	 * Resets the physic world
	 */
	void reset();

	/**
	 * Add a rigid body
	 * @param id id
	 * @param enabled enabled
	 * @param collisionTypeId collision type id
	 * @param transformations transformations
	 * @param restitution restitution
	 * @param friction friction
	 * @param mass mass
	 * @param inertiaTensor inertia matrix
	 * @param boundingVolumes bounding volumes
	 * @return body
	 */
	Body* addRigidBody(const string& id, bool enabled, uint16_t collisionTypeId, const Transformations& transformations, float restitution, float friction, float mass, const Vector3& inertiaTensor, vector<BoundingVolume*> boundingVolumes);

	/**
	 * Add a collision body
	 * @param id id
	 * @param enabled enabled
	 * @param collisionTypeId collision type id
	 * @param transformations transformations
	 * @param boundingVolumes bounding volumes
	 * @return body
	 */
	Body* addCollisionBody(const string& id, bool enabled, uint16_t collisionTypeId, const Transformations& transformations, vector<BoundingVolume*> boundingVolumes);

	/**
	 * Add a static rigid body
	 * @param id id
	 * @param enabled enabled
	 * @param collisionTypeId collision type id
	 * @param transformations transformations
	 * @param friction friction
	 * @param boundingVolumes bounding volumes
	 * @return body
	 */
	Body* addStaticRigidBody(const string& id, bool enabled, uint16_t collisionTypeId, const Transformations& transformations, float friction, vector<BoundingVolume*> boundingVolumes);

	/**
	 * Returns body identified by id
	 * @param id id
	 * @return ridig body
	 */
	Body* getBody(const string& id);

	/**
	 * Removes body identified by id
	 * @param id id
	 */
	void removeBody(const string& id);

	/**
	 * Update world
	 * @param deltaTime delta time
	 */
	void update(float deltaTime);

	/**
	 * Synch physics world with engine
	 * @param engine engine
	 */
	void synch(Engine* engine);

	/**
	 * Determine height on x,y,u while respecting step up max
	 * @param collisionTypeId collision type ids
	 * @param stepUpMax step up max
	 * @param point point on which height should be calculated
	 * @param dest point where height has been determined
	 * @param minHeight min height to determine height from
	 * @param maxHeight max height to start raytracing from
	 * @return body from which height was determined or null
	 */
	Body* determineHeight(uint16_t collisionTypeId, float stepUpMax, const Vector3& point, Vector3& dest, float minHeight = -10000.0f, float maxHeight = 10000.0f);

	/**
	 * Do a ray cast from given start to given end point, if there is any body with given collision type in between
	 * then the body is returned and a hit point is reported
	 * @param collisionTypeIds collision type ids
	 * @param start start
	 * @param end end
	 * @param hitPoint hit point
	 * @param actorId actor rigid body id, which will be exlcluded from ray tracing
	 * @return body
	 *
	 */
	Body* doRayCasting(uint16_t collisionTypeIds, const Vector3& start, const Vector3& end, Vector3& hitPoint, const string& actorId = string());

	/**
	 * Check if world collides with given body
	 * @param collisionTypeIds collision type ids
	 * @param body body
	 * @param rigidBodies bodies that collide with given body
	 * @return if collision happpened or not
	 */
	bool doesCollideWith(uint16_t collisionTypeIds, Body* body, vector<Body*>& rigidBodies);

	/**
	 * Check if world collides with given bounding volumes and its transformations, which both form a collision for method runtime
	 * @param collisionTypeIds collision type ids
	 * @param transformations transformations
	 * @param boundingVolumes bounding volume
	 * @param rigidBodies bodies that collide with given body
	 * @return if collision happpened or not
	 */
	bool doesCollideWith(uint16_t collisionTypeIds, const Transformations& transformations, vector<BoundingVolume*> boundingVolumes, vector<Body*>& rigidBodies);

	/**
	 * Check if body 1 collides with body 2
	 * @param body1 body 1
	 * @param body2 body 2
	 * @return if collision happpened or not
	 */
	bool doCollide(Body* body1, Body* body2);

	/**
	 * Get collision response
	 * @param body1 body 1
	 * @param body2 body 2
	 * @param collision collision response
	 * @return if having hit points
	 */
	bool getCollisionResponse(Body* body1, Body* body2, CollisionResponse& collision);

	/**
	 * Clone this world
	 * @param collisionTypeIds collision type ids to clone
	 */
	World* clone(uint16_t collisionTypeIds = ~0);

	/**
	 * Updates given world with this world
	 * Given world should be a clone of this world
	 * @param world world
	 */
	void synch(World* world);

	/**
	 * Add a world listener
	 * @param listener listener
	 */
	void addWorldListener(WorldListener* listener);

	/**
	 * Remove a world listener
	 * @param listener listener
	 */
	void removeWorldListener(WorldListener* listener);

};
