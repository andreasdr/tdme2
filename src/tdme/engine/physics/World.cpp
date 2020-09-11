#include <tdme/engine/physics/World.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <unordered_set>
#include <string>

#include <ext/reactphysics3d/src/collision/ContactManifold.h>
#include <ext/reactphysics3d/src/collision/OverlapCallback.h>
#include <ext/reactphysics3d/src/collision/RaycastInfo.h>
#include <ext/reactphysics3d/src/collision/shapes/AABB.h>
#include <ext/reactphysics3d/src/constraint/ContactPoint.h>
#include <ext/reactphysics3d/src/engine/CollisionWorld.h>
#include <ext/reactphysics3d/src/engine/DynamicsWorld.h>
#include <ext/reactphysics3d/src/engine/EventListener.h>
#include <ext/reactphysics3d/src/mathematics/Ray.h>
#include <ext/reactphysics3d/src/mathematics/Vector3.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Entity.h>
#include <tdme/engine/Rotation.h>
#include <tdme/engine/Transformations.h>
#include <tdme/engine/physics/Body.h>
#include <tdme/engine/physics/CollisionResponse.h>
#include <tdme/engine/physics/CollisionResponse_Entity.h>
#include <tdme/engine/physics/WorldListener.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/primitives/BoundingVolume.h>
#include <tdme/engine/primitives/LineSegment.h>
#include <tdme/engine/primitives/OrientedBoundingBox.h>
#include <tdme/math/Math.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Quaternion.h>
#include <tdme/math/Vector3.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/VectorIteratorMultiple.h>

using std::find;
using std::remove;
using std::map;
using std::string;
using std::to_string;
using std::unordered_set;

using tdme::engine::physics::World;
using tdme::engine::Engine;
using tdme::engine::Entity;
using tdme::engine::Rotation;
using tdme::engine::Transformations;
using tdme::engine::physics::CollisionResponse;
using tdme::engine::physics::CollisionResponse_Entity;
using tdme::engine::physics::Body;
using tdme::engine::physics::WorldListener;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::primitives::BoundingVolume;
using tdme::engine::primitives::LineSegment;
using tdme::engine::primitives::OrientedBoundingBox;
using tdme::math::Math;
using tdme::math::Matrix4x4;
using tdme::math::Quaternion;
using tdme::math::Vector3;
using tdme::utilities::Console;
using tdme::utilities::VectorIteratorMultiple;

World::World(): world(reactphysics3d::Vector3(0.0, -9.81, 0.0))
{
}

World::~World()
{
	for (auto worldListener: worldListeners) delete worldListener;
	reset();
}

void World::reset()
{
	auto _bodies = bodies;
	for (auto body: _bodies) removeBody(body->getId());
	bodies.clear();
	rigidBodiesDynamic.clear();
	bodiesById.clear();
	bodyCollisionsLastFrame.clear();
}

Body* World::addRigidBody(const string& id, bool enabled, uint16_t collisionTypeId, const Transformations& transformations, float restitution, float friction, float mass, const Vector3& inertiaTensor, vector<BoundingVolume*> boundingVolumes)
{
	removeBody(id);
	auto body = new Body(this, id, Body::TYPE_DYNAMIC, enabled, collisionTypeId, transformations, restitution, friction, mass, inertiaTensor, boundingVolumes);
	bodies.push_back(body);
	rigidBodiesDynamic.push_back(body);
	bodiesById[id] = body;
	for (auto listener: worldListeners) {
		listener->onAddedBody(id, Body::TYPE_DYNAMIC, enabled, collisionTypeId, transformations, restitution, friction, mass, inertiaTensor, boundingVolumes);
	}
	return body;
}

Body* World::addCollisionBody(const string& id, bool enabled, uint16_t collisionTypeId, const Transformations& transformations, vector<BoundingVolume*> boundingVolumes) {
	removeBody(id);
	auto body = new Body(this, id, Body::TYPE_COLLISION, enabled, collisionTypeId, transformations, 0.0f, 0.0f, 0.0f, Body::getNoRotationInertiaTensor(), boundingVolumes);
	bodies.push_back(body);
	bodiesById[id] = body;
	for (auto listener: worldListeners) {
		listener->onAddedBody(id, Body::TYPE_COLLISION, enabled, collisionTypeId, transformations, 0.0f, 0.0f, 0.0f, Body::getNoRotationInertiaTensor(), boundingVolumes);
	}
	return body;
}

Body* World::addStaticRigidBody(const string& id, bool enabled, uint16_t collisionTypeId, const Transformations& transformations, float friction, vector<BoundingVolume*> boundingVolumes)
{
	removeBody(id);
	auto body = new Body(this, id, Body::TYPE_STATIC, enabled, collisionTypeId, transformations, 0.0f, friction, 0.0f, Body::getNoRotationInertiaTensor(), boundingVolumes);
	bodies.push_back(body);
	bodiesById[id] = body;
	for (auto listener: worldListeners) {
		listener->onAddedBody(id, Body::TYPE_STATIC, enabled, collisionTypeId, transformations, 0.0f, friction, 0.0f, Body::getNoRotationInertiaTensor(), boundingVolumes);
	}
	return body;
}

Body* World::getBody(const string& id)
{
	auto bodyByIdIt = bodiesById.find(id);
	if (bodyByIdIt != bodiesById.end()) {
		return bodyByIdIt->second;
	}
	return nullptr;
}

void World::removeBody(const string& id) {
	auto bodyByIdIt = bodiesById.find(id);
	if (bodyByIdIt != bodiesById.end()) {
		auto body = bodyByIdIt->second;
		if (body->rigidBody != nullptr) {
			world.destroyRigidBody(body->rigidBody);
		} else {
			world.destroyCollisionBody(body->collisionBody);
		}
		bodies.erase(remove(bodies.begin(), bodies.end(), body), bodies.end());
		rigidBodiesDynamic.erase(remove(rigidBodiesDynamic.begin(), rigidBodiesDynamic.end(), body), rigidBodiesDynamic.end());
		bodiesById.erase(bodyByIdIt);
		for (auto listener: worldListeners) {
			listener->onRemovedBody(id, body->getType(), body->getCollisionTypeId());
		}
		delete body;
	}
}

void World::update(float deltaTime)
{
	if (deltaTime < Math::EPSILON) return;

	// do the job
	world.update(deltaTime);

	// collision events
	{
		// fire on collision begin, on collision
		map<string, BodyCollisionStruct> bodyCollisionsCurrentFrame;
		CollisionResponse collision;
		auto manifolds = world.getContactsList();
		for (auto manifold: manifolds) {
			auto body1 = static_cast<Body*>(manifold->getBody1()->getUserData());
			auto body2 = static_cast<Body*>(manifold->getBody2()->getUserData());
			BodyCollisionStruct bodyCollisionStruct;
			bodyCollisionStruct.body1Id = body1->getId();
			bodyCollisionStruct.body2Id = body2->getId();
			string bodyKey = bodyCollisionStruct.body1Id + "," + bodyCollisionStruct.body2Id;
			string bodyKeyInverted = bodyCollisionStruct.body2Id + "," + bodyCollisionStruct.body1Id;
			bodyCollisionsCurrentFrame[bodyKey] = bodyCollisionStruct;
			for (int i=0; i<manifold->getNbContactPoints(); i++) {
				auto contactPoint = manifold->getContactPoints();
				while (contactPoint != nullptr) {
					// construct collision
					auto entity = collision.addResponse(-contactPoint->getPenetrationDepth());
					auto normal = contactPoint->getNormal();
					entity->getNormal().set(normal.x, normal.y, normal.z);
					auto shape1 = manifold->getShape1();
					auto shape2 = manifold->getShape2();
					auto& shapeLocalToWorldTransform1 = shape1->getLocalToWorldTransform();
					auto& shapeLocalToWorldTransform2 = shape2->getLocalToWorldTransform();
					auto& localPoint1 = contactPoint->getLocalPointOnShape1();
					auto& localPoint2 = contactPoint->getLocalPointOnShape2();
					auto worldPoint1 = shapeLocalToWorldTransform1 * localPoint1;
					auto worldPoint2 = shapeLocalToWorldTransform2 * localPoint2;
					entity->addHitPoint(Vector3(worldPoint1.x, worldPoint1.y, worldPoint1.z));
					entity->addHitPoint(Vector3(worldPoint2.x, worldPoint2.y, worldPoint2.z));
					contactPoint = contactPoint->getNext();
					// fire events
					if (bodyCollisionsLastFrame.find(bodyKey) == bodyCollisionsLastFrame.end() &&
						bodyCollisionsLastFrame.find(bodyKeyInverted) == bodyCollisionsLastFrame.end()) {
						// fire on collision begin
						body1->fireOnCollisionBegin(body2, collision);
					}
					// fire on collision
					body1->fireOnCollision(body2, collision);
					// reset collision
					collision.reset();
				}
			}
		}

		// fire on collision end
		//	check each collision last frame that disappeared in current frame
		for (auto it: bodyCollisionsLastFrame) {
			BodyCollisionStruct* bodyCollisionStruct = &it.second;
			{
				string bodyKey = bodyCollisionStruct->body1Id + "," + bodyCollisionStruct->body2Id;
				auto bodyCollisionsCurrentFrameIt = bodyCollisionsCurrentFrame.find(bodyKey);
				if (bodyCollisionsCurrentFrameIt != bodyCollisionsCurrentFrame.end()) continue;
			}
			{
				string bodyKey = bodyCollisionStruct->body2Id + "," + bodyCollisionStruct->body1Id;
				auto bodyCollisionsCurrentFrameIt = bodyCollisionsCurrentFrame.find(bodyKey);
				if (bodyCollisionsCurrentFrameIt != bodyCollisionsCurrentFrame.end()) continue;
			}
			auto body1It = bodiesById.find(bodyCollisionStruct->body1Id);
			auto body1 = body1It == bodiesById.end()?nullptr:body1It->second;
			auto body2It = bodiesById.find(bodyCollisionStruct->body2Id);
			auto body2 = body2It == bodiesById.end()?nullptr:body2It->second;
			if (body1 == nullptr || body2 == nullptr) continue;
			body1->fireOnCollisionEnd(body2);
		}
		// swap rigid body collisions current and last frame
		bodyCollisionsLastFrame = bodyCollisionsCurrentFrame;
	}

	// update transformations for rigid body
	for (auto i = 0; i < rigidBodiesDynamic.size(); i++) {
		auto body = rigidBodiesDynamic[i];
		// skip if disabled
		if (body->isEnabled() == false) {
			continue;
		}
		// skip if static or sleeping
		if (body->isSleeping() == true) {
			continue;
		}

		// rp3d transform
		auto transform = body->rigidBody->getTransform();
		auto transformPosition = transform.getPosition();
		auto transformOrientation = transform.getOrientation();

		// tdme2 transformations
		auto& physicsTransformations = body->transformations;

		// rotations
		// keep care that only 1 rotation exists
		while (physicsTransformations.getRotationCount() > 1) {
			physicsTransformations.removeRotation(physicsTransformations.getRotationCount() - 1);
		}
		if (physicsTransformations.getRotationCount() < 1) {
			physicsTransformations.addRotation(Vector3(0.0f, 1.0f, 0.0f), 0.0f);
		}

		// rotations
		Quaternion rotationsQuaternion(transformOrientation.x, transformOrientation.y, transformOrientation.z, transformOrientation.w);
		physicsTransformations.getRotation(0).fromQuaternion(rotationsQuaternion);
		// scale
		physicsTransformations.setScale(body->transformationsScale);
		// translation
		physicsTransformations.setTranslation(Vector3(transformPosition.x, transformPosition.y, transformPosition.z));
		// done
		physicsTransformations.update();
	}
}

void World::synch(Engine* engine)
{
	for (auto i = 0; i < rigidBodiesDynamic.size(); i++) {
		// update rigid body
		auto body = rigidBodiesDynamic[i];

		// skip on sleeping objects
		if (body->isSleeping() == true) continue;

		// synch with engine entity
		auto engineEntity = engine->getEntity(body->id);
		if (engineEntity == nullptr) {
			Console::println(
				string("World::entity '") +
				body->id +
				string("' not found")
			);
			continue;
		}

		// enable
		engineEntity->setEnabled(body->isEnabled());

		//apply inverse local transformation for engine update
		if (body->isEnabled() == true) {
			engineEntity->fromTransformations(body->transformations);
		}
	}
}

Body* World::determineHeight(uint16_t collisionTypeIds, float stepUpMax, const Vector3& point, Vector3& dest, float minHeight)
{
	class CustomCallbackClass : public reactphysics3d::RaycastCallback {
	public:
		CustomCallbackClass(float stepUpMax, const Vector3& point, float minHeight = -10000.0f): stepUpMax(stepUpMax), point(point), height(minHeight), body(nullptr) {
		}
		virtual reactphysics3d::decimal notifyRaycastHit(const reactphysics3d::RaycastInfo& info) {
			Vector3 hitPoint(info.worldPoint.x, info.worldPoint.y, info.worldPoint.z);
			auto _body = static_cast<Body*>(info.body->getUserData());
			if (hitPoint.getY() >= height) {
				height = hitPoint.getY();
				body = static_cast<Body*>(info.body->getUserData());
			}
			return reactphysics3d::decimal(info.hitFraction);
		};
		Body* getBody() {
			return body;
		}
		const float getHeight() {
			return height;
		}
	private:
		float stepUpMax;
		Vector3 point;
		float height;
		Body* body;
	};
	reactphysics3d::Vector3 startPoint(point.getX(), point.getY() + stepUpMax, point.getZ());
	reactphysics3d::Vector3 endPoint(point.getX(), minHeight, point.getZ());
	reactphysics3d::Ray ray(startPoint, endPoint);
	CustomCallbackClass customCallbackObject(stepUpMax, point, minHeight);
	world.raycast(ray, &customCallbackObject, collisionTypeIds);
	if (customCallbackObject.getBody() != nullptr) {
		dest.set(point);
		dest.setY(customCallbackObject.getHeight());
		return customCallbackObject.getBody();
	} else {
		return nullptr;
	}
}

Body* World::doRayCasting(uint16_t collisionTypeIds, const Vector3& start, const Vector3& end, Vector3& hitPoint, const string& actorId)
{
	class CustomCallbackClass : public reactphysics3d::RaycastCallback {
	public:
		CustomCallbackClass(const string& actorId): actorId(actorId), body(nullptr) {
		}
		virtual reactphysics3d::decimal notifyRaycastHit(const reactphysics3d::RaycastInfo& info) {
			auto _body = static_cast<Body*>(info.body->getUserData());
			if (actorId.size() == 0 || _body->getId() != actorId) {
				body = _body;
				hitPoint.set(info.worldPoint.x, info.worldPoint.y, info.worldPoint.z);
				return reactphysics3d::decimal(info.hitFraction);
			} else {
				return reactphysics3d::decimal(1.0);
			}
		};
		Body* getBody() {
			return body;
		}
		const Vector3& getHitPoint() {
			return hitPoint;
		}
	private:
		string actorId;
		Vector3 hitPoint;
		Body* body;
	};
	reactphysics3d::Vector3 startPoint(start.getX(), start.getY(), start.getZ());
	reactphysics3d::Vector3 endPoint(end.getX(), end.getY(), end.getZ());
	reactphysics3d::Ray ray(startPoint, endPoint);
	CustomCallbackClass customCallbackObject(actorId);
	world.raycast(ray, &customCallbackObject, collisionTypeIds);
	if (customCallbackObject.getBody() != nullptr) {
		hitPoint.set(customCallbackObject.getHitPoint());
		return customCallbackObject.getBody();
	} else {
		return nullptr;
	}
}

bool World::doesCollideWith(uint16_t collisionTypeIds, Body* body, vector<Body*>& rigidBodies) {
	// callback
	class CustomOverlapCallback: public reactphysics3d::OverlapCallback {
	    public:
			CustomOverlapCallback(vector<Body*>& rigidBodies): rigidBodies(rigidBodies) {
			}

			virtual void notifyOverlap(reactphysics3d::CollisionBody* collisionBody) {
				rigidBodies.push_back(static_cast<Body*>(collisionBody->getUserData()));
			}
	    private:
			vector<Body*>& rigidBodies;
	};

	// do the test
	CustomOverlapCallback customOverlapCallback(rigidBodies);
	world.testOverlap(body->collisionBody, &customOverlapCallback, collisionTypeIds);

	// done
	return rigidBodies.size() > 0;
}

bool World::doesCollideWith(uint16_t collisionTypeIds, const Transformations& transformations, vector<BoundingVolume*> boundingVolumes, vector<Body*>& rigidBodies) {
	auto collisionBody = addCollisionBody("tdme.world.doescollidewith", true, 32768, transformations, boundingVolumes);
	doesCollideWith(collisionTypeIds, collisionBody, rigidBodies);
	removeBody("tdme.world.doescollidewith");
	return rigidBodies.size() > 0;
}

bool World::doCollide(Body* body1, Body* body2) {
	return world.testOverlap(body1->collisionBody, body2->collisionBody);
}

bool World::getCollisionResponse(Body* body1, Body* body2, CollisionResponse& collision) {
	// callback
	class CustomCollisionCallback: public reactphysics3d::CollisionCallback {
	    public:
			CustomCollisionCallback(CollisionResponse& collision): collision(collision) {
			}

			void notifyContact(const CollisionCallbackInfo& collisionCallbackInfo) {
				auto manifold = collisionCallbackInfo.contactManifoldElements;
				while (manifold != nullptr) {
					auto contactPoint = manifold->getContactManifold()->getContactPoints();
					while (contactPoint != nullptr) {
						// construct collision
						auto entity = collision.addResponse(-contactPoint->getPenetrationDepth());
						auto normal = contactPoint->getNormal();
						entity->getNormal().set(normal.x, normal.y, normal.z);
						auto shape1 = manifold->getContactManifold()->getShape1();
						auto shape2 = manifold->getContactManifold()->getShape2();
						auto& shapeLocalToWorldTransform1 = shape1->getLocalToWorldTransform();
						auto& shapeLocalToWorldTransform2 = shape2->getLocalToWorldTransform();
						auto& localPoint1 = contactPoint->getLocalPointOnShape1();
						auto& localPoint2 = contactPoint->getLocalPointOnShape2();
						auto worldPoint1 = shapeLocalToWorldTransform1 * localPoint1;
						auto worldPoint2 = shapeLocalToWorldTransform2 * localPoint2;
						entity->addHitPoint(Vector3(worldPoint1.x, worldPoint1.y, worldPoint1.z));
						entity->addHitPoint(Vector3(worldPoint2.x, worldPoint2.y, worldPoint2.z));
						contactPoint = contactPoint->getNext();
					}
					manifold = collisionCallbackInfo.contactManifoldElements->getNext();
				}
			}

	    private:
			CollisionResponse& collision;
	};
	// do the test
	CustomCollisionCallback customCollisionCallback(collision);
	world.testCollision(body1->collisionBody, body2->collisionBody, &customCollisionCallback);
	return collision.getEntityCount() > 0;
}

World* World::clone(uint16_t collisionTypeIds)
{
	auto clonedWorld = new World();
	for (auto i = 0; i < bodies.size(); i++) {
		auto body = bodies[i];
		// clone obv
		Body* clonedBody = nullptr;
		auto bodyType = body->getType();

		// test type
		if (((body->getCollisionTypeId() & collisionTypeIds) == body->getCollisionTypeId()) == false) continue;

		// clone rigid body
		switch(bodyType) {
			case Body::TYPE_STATIC:
				clonedBody = clonedWorld->addStaticRigidBody(body->id, body->isEnabled(), body->getCollisionTypeId(), body->transformations, body->getFriction(), body->boundingVolumes);
				break;
			case Body::TYPE_DYNAMIC:
				clonedBody = clonedWorld->addRigidBody(body->id, body->isEnabled(), body->getCollisionTypeId(), body->transformations, body->getRestitution(), body->getFriction(), body->getMass(), body->inertiaTensor, body->boundingVolumes);
				break;
			case Body::TYPE_COLLISION:
				clonedBody = clonedWorld->addCollisionBody(body->id, body->isEnabled(), body->getCollisionTypeId(), body->transformations, body->boundingVolumes);
				break;
			default:
				Console::println("World::clone(): Unsupported type: " + to_string(bodyType));
				continue;
		}

		// set cloned
		clonedBody->setCloned(true);

		// synch additional properties
		synch(clonedBody, clonedBody);
	}
	return clonedWorld;
}

void World::synch(Body* clonedBody, Body* body)
{
	clonedBody->setCollisionTypeIds(body->getCollisionTypeIds());
	clonedBody->setEnabled(body->isEnabled());
	clonedBody->setMass(body->getMass());
	clonedBody->fromTransformations(body->transformations);
	if (clonedBody->getType() == Body::TYPE_DYNAMIC) {
		clonedBody->setLinearVelocity(body->getLinearVelocity());
		clonedBody->setAngularVelocity(body->getAngularVelocity());
	}
}

void World::synch(World* world)
{
	for (auto i = 0; i < rigidBodiesDynamic.size(); i++) {
		auto body = rigidBodiesDynamic.at(i);
		auto clonedBody = world->getBody(body->id);
		if (clonedBody == nullptr) {
			Console::println(
				string("Cloned world::entity '") +
				body->id +
				string("' not found")
			);
			continue;
		}
		// synch rigid bodies
		synch(clonedBody, body);
	}
}

void World::addWorldListener(WorldListener* listener)
{
	worldListeners.push_back(listener);
}

void World::removeWorldListener(WorldListener* listener)
{
	worldListeners.erase(remove(worldListeners.begin(), worldListeners.end(), listener), worldListeners.end());
	delete listener;
}
