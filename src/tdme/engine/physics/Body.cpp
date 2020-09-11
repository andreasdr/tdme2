#include <string>
#include <vector>

#include <ext/reactphysics3d/src/body/Body.h>
#include <ext/reactphysics3d/src/body/CollisionBody.h>
#include <ext/reactphysics3d/src/body/Body.h>
#include <ext/reactphysics3d/src/collision/CollisionCallback.h>
#include <ext/reactphysics3d/src/collision/NarrowPhaseInfo.h>
#include <ext/reactphysics3d/src/collision/ProxyShape.h>
#include <ext/reactphysics3d/src/collision/shapes/CollisionShape.h>
#include <ext/reactphysics3d/src/collision/narrowphase/DefaultCollisionDispatch.h>
#include <ext/reactphysics3d/src/collision/narrowphase/NarrowPhaseAlgorithm.h>
#include <ext/reactphysics3d/src/constraint/ContactPoint.h>
#include <ext/reactphysics3d/src/engine/CollisionWorld.h>
#include <ext/reactphysics3d/src/engine/DynamicsWorld.h>
#include <ext/reactphysics3d/src/mathematics/Transform.h>
#include <ext/reactphysics3d/src/mathematics/Vector3.h>
#include <ext/reactphysics3d/src/memory/MemoryAllocator.h>

#include <tdme/engine/Rotation.h>
#include <tdme/engine/Transformations.h>
#include <tdme/engine/physics/CollisionListener.h>
#include <tdme/engine/physics/CollisionResponse.h>
#include <tdme/engine/physics/World.h>
#include <tdme/engine/primitives/BoundingVolume.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/primitives/Capsule.h>
#include <tdme/engine/primitives/TerrainMesh.h>
#include <tdme/engine/primitives/OrientedBoundingBox.h>
#include <tdme/math/Math.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Quaternion.h>
#include <tdme/math/Vector3.h>
#include <tdme/utilities/Console.h>
#include "Body.h"

using std::string;
using std::to_string;
using std::vector;

using tdme::engine::physics::Body;
using tdme::engine::Rotation;
using tdme::engine::Transformations;
using tdme::engine::physics::CollisionListener;
using tdme::engine::physics::CollisionResponse;
using tdme::engine::physics::World;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::primitives::BoundingVolume;
using tdme::engine::primitives::Capsule;
using tdme::engine::primitives::OrientedBoundingBox;
using tdme::engine::primitives::TerrainMesh;
using tdme::math::Math;
using tdme::math::Matrix4x4;
using tdme::math::Quaternion;
using tdme::math::Vector3;
using tdme::utilities::Console;

constexpr uint16_t Body::TYPEIDS_ALL;
constexpr uint16_t Body::TYPEID_STATIC;
constexpr uint16_t Body::TYPEID_DYNAMIC;

Body::Body(World* world, const string& id, int type, bool enabled, uint16_t collisionTypeId, const Transformations& transformations, float restitution, float friction, float mass, const Vector3& inertiaTensor, const vector<BoundingVolume*> boundingVolumes)
{
	this->world = world;
	this->id = id;
	this->rootId = id;
	this->inertiaTensor = inertiaTensor;
	this->type = type;
	this->mass = mass;
	this->collideTypeIds = ~0;
	this->collisionTypeId = collisionTypeId;
	switch (type) {
		case TYPE_STATIC:
			this->rigidBody = this->world->world.createRigidBody(reactphysics3d::Transform());
			this->rigidBody->setType(reactphysics3d::BodyType::STATIC);
			this->collisionBody = rigidBody;
			break;
		case TYPE_DYNAMIC:
			this->rigidBody = this->world->world.createRigidBody(reactphysics3d::Transform());
			this->rigidBody->setType(reactphysics3d::BodyType::DYNAMIC);
			this->collisionBody = rigidBody;
			break;
		case TYPE_KINEMATIC:
			this->rigidBody = this->world->world.createRigidBody(reactphysics3d::Transform());
			this->rigidBody->setType(reactphysics3d::BodyType::KINEMATIC);
			this->collisionBody = rigidBody;
			break;
		case TYPE_COLLISION:
			this->rigidBody = nullptr;
			this->collisionBody = this->world->world.createCollisionBody(reactphysics3d::Transform());
			break;
		default:
			Console::println("Body::Body(): unsupported type: " + to_string(type) + ": using collision body");
			this->rigidBody = nullptr;
			this->collisionBody = this->world->world.createCollisionBody(reactphysics3d::Transform());
			break;
	}
	if (rigidBody != nullptr) {
		rigidBody->getMaterial().setFrictionCoefficient(friction);
		rigidBody->getMaterial().setBounciness(restitution);
		rigidBody->setMass(mass);
	}
	collisionBody->setUserData(this);
	for (auto boundingVolume: boundingVolumes) {
		this->boundingVolumes.push_back(dynamic_cast<TerrainMesh*>(boundingVolume) != nullptr?boundingVolume:boundingVolume->clone());
	}
	fromTransformations(transformations);
	setEnabled(enabled);
}

Body::~Body() {
	for (auto boundingVolume: boundingVolumes) {
		if (dynamic_cast<TerrainMesh*>(boundingVolume) != nullptr && cloned == true) continue;
		delete boundingVolume;
	}
}

const Vector3 Body::getNoRotationInertiaTensor() {
	return Vector3(0.0f, 0.0f, 0.0f);
}

Matrix4x4 Body::computeInverseInertiaMatrix(BoundingBox* boundingBox, float mass, float scaleXAxis, float scaleYAxis, float scaleZAxis)
{
	auto width = boundingBox->getDimensions().getX();
	auto height = boundingBox->getDimensions().getY();
	auto depth = boundingBox->getDimensions().getZ();
	return
		(Matrix4x4(
			scaleXAxis > Math::EPSILON && mass > Math::EPSILON?1.0f / (scaleXAxis * 1.0f / 12.0f * mass * (height * height + depth * depth)):0.0f,
			0.0f,
			0.0f,
			0.0f,
			0.0f,
			scaleYAxis > Math::EPSILON && mass > Math::EPSILON?1.0f / (scaleYAxis * 1.0f / 12.0f * mass * (width * width + depth * depth)):0.0f,
			0.0f,
			0.0f,
			0.0f,
			0.0f,
			scaleZAxis > Math::EPSILON && mass > Math::EPSILON?1.0f / (scaleZAxis * 1.0f / 12.0f * mass * (width * width + height * height)):0.0f,
			0.0f,
			0.0f,
			0.0f,
			0.0f,
			1.0f
		));
}

bool Body::isCloned() {
	return cloned;
}

void Body::setCloned(bool cloned) {
	this->cloned = cloned;
}

const string& Body::getId()
{
	return id;
}

const string& Body::getRootId()
{
	return rootId;
}

void Body::setRootId(const string& rootId) {
	this->rootId = rootId;
}

int32_t Body::getType() {
	return type;
}

uint16_t Body::getCollisionTypeId()
{
	return collisionTypeId;
}

void Body::setCollisionTypeId(uint16_t typeId)
{
	this->collisionTypeId = typeId;
	for (auto proxyShape: proxyShapes) {
		proxyShape->setCollisionCategoryBits(typeId);
	}
}

uint16_t Body::getCollisionTypeIds()
{
	return collideTypeIds;
}

void Body::setCollisionTypeIds(uint16_t collisionTypeIds)
{
	this->collideTypeIds = collisionTypeIds;
	for (auto proxyShape: proxyShapes) {
		proxyShape->setCollideWithMaskBits(collisionTypeIds);
	}
}

bool Body::isEnabled()
{
	return collisionBody->isActive();
}

void Body::setEnabled(bool enabled)
{
	collisionBody->setIsActive(enabled);
	if (enabled == true) collisionBody->setIsSleeping(false);
}

bool Body::isSleeping()
{
	return collisionBody->isSleeping();
}

void Body::setSleeping(bool sleeping) {
	collisionBody->setIsSleeping(sleeping);
}

vector<BoundingVolume*>& Body::getBoundingVolumes() {
	return boundingVolumes;
}

void Body::resetProxyShapes() {
	// remove proxy shapes
	for (auto proxyShape: proxyShapes) {
		if (rigidBody != nullptr) {
			rigidBody->removeCollisionShape(proxyShape);
		} else {
			collisionBody->removeCollisionShape(proxyShape);
		}
	}
	proxyShapes.clear();

	// set up scale
	for (auto boundingVolume: boundingVolumes) {
		// scale bounding volume and recreate it if nessessary
		if (boundingVolume->getScale().equals(transformations.getScale()) == false) {
			boundingVolume->setScale(transformations.getScale());
		}
	}

	// reset proxy shapes with mass
	if (mass > Math::EPSILON) {
		// determine total volume
		float volumeTotal = 0.0f;
		for (auto boundingVolume: boundingVolumes) {
			volumeTotal+=
				boundingVolume->boundingBoxTransformed.getDimensions().getX() *
				boundingVolume->boundingBoxTransformed.getDimensions().getY() *
				boundingVolume->boundingBoxTransformed.getDimensions().getZ();
		}
		// add bounding volumes with mass
		for (auto boundingVolume: boundingVolumes) {
			reactphysics3d::ProxyShape* proxyShape = nullptr;
			float volumeBoundingVolume =
				boundingVolume->boundingBoxTransformed.getDimensions().getX() *
				boundingVolume->boundingBoxTransformed.getDimensions().getY() *
				boundingVolume->boundingBoxTransformed.getDimensions().getZ();
			if (rigidBody != nullptr) {
				proxyShape = rigidBody->addCollisionShape(boundingVolume->collisionShape, boundingVolume->collisionShapeLocalTransform, mass / volumeTotal * volumeBoundingVolume);
			} else {
				proxyShape = collisionBody->addCollisionShape(boundingVolume->collisionShape, boundingVolume->collisionShapeLocalTransform);
			}
			proxyShapes.push_back(proxyShape);
		}
	} else {
		// add bounding volumes without mass
		for (auto boundingVolume: boundingVolumes) {
			reactphysics3d::ProxyShape* proxyShape = nullptr;
			if (rigidBody != nullptr) {
				proxyShape = rigidBody->addCollisionShape(boundingVolume->collisionShape, boundingVolume->collisionShapeLocalTransform, 0.0f);
			} else {
				proxyShape = collisionBody->addCollisionShape(boundingVolume->collisionShape, boundingVolume->collisionShapeLocalTransform);
			}
			proxyShapes.push_back(proxyShape);
		}
	}

	// set up collisioin type ids and type id
	for (auto proxyShape: proxyShapes) {
		proxyShape->setCollideWithMaskBits(collideTypeIds);
		proxyShape->setCollisionCategoryBits(collisionTypeId);
	}

	// set up inverse inertia tensor local
	if (rigidBody != nullptr) {
		// set inverse inertia tensor local
		// 	TODO: transform with identity transform, this is a bit sub optimal but currently required to get rigid body dimensions
		// 	this method is only used when setting TDME2 transform, so no need to transform back
		reactphysics3d::Transform transform;
		collisionBody->setTransform(transform);

		auto boundingBoxTransformed = computeBoundingBoxTransformed();
		auto& inverseInertiaMatrixArray = computeInverseInertiaMatrix(&boundingBoxTransformed, mass, inertiaTensor.getX(), inertiaTensor.getY(), inertiaTensor.getZ()).getArray();
		rigidBody->setInverseInertiaTensorLocal(
			reactphysics3d::Matrix3x3(
				inverseInertiaMatrixArray[0],
				inverseInertiaMatrixArray[1],
				inverseInertiaMatrixArray[2],
				inverseInertiaMatrixArray[4],
				inverseInertiaMatrixArray[5],
				inverseInertiaMatrixArray[6],
				inverseInertiaMatrixArray[8],
				inverseInertiaMatrixArray[9],
				inverseInertiaMatrixArray[10]
			)
		);
	};
}

BoundingBox Body::computeBoundingBoxTransformed() {
	auto aabb = rigidBody->getAABB();
	return BoundingBox(
		Vector3(
			aabb.getMin().x,
			aabb.getMin().y,
			aabb.getMin().z
		),
		Vector3(
			aabb.getMax().x,
			aabb.getMax().y,
			aabb.getMax().z
		)
	);
}

float Body::getFriction()
{
	if (rigidBody == nullptr) {
		Console::println("Body::getFriction(): no rigid body attached");
		return 0.0f;
	}
	return rigidBody->getMaterial().getFrictionCoefficient();
}

void Body::setFriction(float friction)
{
	if (rigidBody == nullptr) {
		Console::println("Body::setFriction(): no rigid body attached");
		return;
	}
	rigidBody->getMaterial().setFrictionCoefficient(friction);
}

float Body::getRestitution()
{
	if (rigidBody == nullptr) {
		Console::println("Body::getRestitution(): no rigid body attached");
		return 0.0f;
	}
	return rigidBody->getMaterial().getBounciness();
}

void Body::setRestitution(float restitution)
{
	if (rigidBody == nullptr) {
		Console::println("Body::setRestitution(): no rigid body attached");
		return;
	}
	rigidBody->getMaterial().setBounciness(restitution);
}

float Body::getMass()
{
	return mass;
}

void Body::setMass(float mass)
{
	this->mass = mass;
	if (rigidBody == nullptr) {
		Console::println("Body::setMass(): no rigid body attached");
		return;
	}
	rigidBody->setMass(mass);
}

const Vector3 Body::getLinearVelocity()
{
	if (rigidBody == nullptr) {
		Console::println("Body::getLinearVelocity(): no rigid body attached");
		return Vector3();
	}
	return Vector3(
		rigidBody->getLinearVelocity().x,
		rigidBody->getLinearVelocity().y,
		rigidBody->getLinearVelocity().z
	);
}

void Body::setLinearVelocity(const Vector3& linearVelocity) {
	if (rigidBody == nullptr) {
		Console::println("Body::setLinearVelocity(): no rigid body attached");
		return;
	}
	rigidBody->setLinearVelocity(reactphysics3d::Vector3(linearVelocity.getX(), linearVelocity.getY(), linearVelocity.getZ()));
}

const Vector3 Body::getAngularVelocity()
{
	if (rigidBody == nullptr) {
		Console::println("Body::getAngularVelocity(): no rigid body attached");
		return Vector3();
	}
	return Vector3(
		rigidBody->getAngularVelocity().x,
		rigidBody->getAngularVelocity().y,
		rigidBody->getAngularVelocity().z
	);
}

void Body::setAngularVelocity(const Vector3& angularVelocity) {
	if (rigidBody == nullptr) {
		Console::println("Body::setAngularVelocity(): no rigid body attached");
		return;
	}
	rigidBody->setAngularVelocity(reactphysics3d::Vector3(angularVelocity.getX(), angularVelocity.getY(), angularVelocity.getZ()));
}

float Body::getLinearDamping()
{
	if (rigidBody == nullptr) {
		Console::println("Body::getLinearDamping(): no rigid body attached");
		return 0.0f;
	}
	return rigidBody->getLinearDamping();
}

void Body::setLinearDamping(float linearDamping)
{
	if (rigidBody == nullptr) {
		Console::println("Body::setLinearDamping(): no rigid body attached");
		return;
	}
	rigidBody->setLinearDamping(linearDamping);
}

float Body::getAngularDamping()
{
	if (rigidBody == nullptr) {
		Console::println("Body::getAngularDamping(): no rigid body attached");
		return 0.0f;
	}
	return rigidBody->getAngularDamping();
}

void Body::setAngularDamping(float angularDamping)
{
	if (rigidBody == nullptr) {
		Console::println("Body::setAngularDamping(): no rigid body attached");
		return;
	}
	rigidBody->setAngularDamping(angularDamping);
}

const Transformations& Body::getTransformations() {
	return transformations;
}

void Body::fromTransformations(const Transformations& transformations)
{
	// store engine transformations
	this->transformations.fromTransformations(transformations);

	// reset proxy shapes if bounding volumes do not match proxy shapes or if scaling has changed
	if (proxyShapes.size() != boundingVolumes.size() || transformationsScale.equals(transformations.getScale()) == false) {
		resetProxyShapes();
		transformationsScale.set(transformations.getScale());
	}

	/*
	// TODO: center of mass ~ pivot
	// set center of mass which is basically center of bv for now
	body->setCenterOfMassLocal(boundingVolume->collisionShapeLocalTransform.getPosition());
	// find final position, not sure yet if its working 100%, but still works with some tests
	auto centerOfMassWorld = transform * boundingVolume->collisionShapeLocalTransform.getPosition();
	transform.setPosition(
		transform.getPosition() +
		transform.getPosition() -
		centerOfMassWorld +
		(
			reactphysics3d::Vector3(
				boundingVolume->collisionShapeLocalTranslation.getX(),
				boundingVolume->collisionShapeLocalTranslation.getY(),
				boundingVolume->collisionShapeLocalTranslation.getZ()
			) * scaleVectorTransformed
		)
	);
	*/

	// set transform
	collisionBody->setTransform(
		reactphysics3d::Transform(
			reactphysics3d::Vector3(
				this->transformations.getTranslation().getX(),
				this->transformations.getTranslation().getY(),
				this->transformations.getTranslation().getZ()
			),
			reactphysics3d::Quaternion(
				this->transformations.getRotationsQuaternion().getX(),
				this->transformations.getRotationsQuaternion().getY(),
				this->transformations.getRotationsQuaternion().getZ(),
				this->transformations.getRotationsQuaternion().getW()
			)
		)
	);
}

void Body::addForce(const Vector3& forceOrigin, const Vector3& force)
{
	if (rigidBody == nullptr) {
		Console::println("Body::addForce(): no rigid body attached");
		return;
	}
	rigidBody->applyForce(
		reactphysics3d::Vector3(force.getX(), force.getY(), force.getZ()),
		reactphysics3d::Vector3(forceOrigin.getX(), forceOrigin.getY(), forceOrigin.getZ())
	);
}

void Body::addForce(const Vector3& force)
{
	if (rigidBody == nullptr) {
		Console::println("Body::addForce(): no rigid body attached");
		return;
	}
	rigidBody->applyForceToCenterOfMass(
		reactphysics3d::Vector3(force.getX(), force.getY(), force.getZ())
	);
}

void Body::addTorque(const Vector3& torque)
{
	if (rigidBody == nullptr) {
		Console::println("Body::addTorque(): no rigid body attached");
		return;
	}
	rigidBody->applyTorque(
		reactphysics3d::Vector3(torque.getX(), torque.getY(), torque.getZ())
	);
}

void Body::addCollisionListener(CollisionListener* listener)
{
	collisionListener.push_back(listener);
}

void Body::removeCollisionListener(CollisionListener* listener)
{
	collisionListener.erase(remove(collisionListener.begin(), collisionListener.end(), listener), collisionListener.end());
}

void Body::fireOnCollision(Body* other, CollisionResponse& collisionResponse)
{
	for (auto listener: collisionListener) {
		listener->onCollision(this, other, collisionResponse);
	}
}

void Body::fireOnCollisionBegin(Body* other, CollisionResponse& collisionResponse)
{
	for (auto listener: collisionListener) {
		listener->onCollisionBegin(this, other, collisionResponse);
	}
}

void Body::fireOnCollisionEnd(Body* other)
{
	for (auto listener: collisionListener) {
		listener->onCollisionEnd(this, other);
	}
}
