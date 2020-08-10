#include <tdme/engine/primitives/BoundingVolume.h>

#include <string>

#include <ext/reactphysics3d/src/body/CollisionBody.h>
#include <ext/reactphysics3d/src/collision/CollisionCallback.h>
#include <ext/reactphysics3d/src/collision/NarrowPhaseInfo.h>
#include <ext/reactphysics3d/src/collision/ProxyShape.h>
#include <ext/reactphysics3d/src/collision/shapes/CollisionShape.h>
#include <ext/reactphysics3d/src/collision/narrowphase/DefaultCollisionDispatch.h>
#include <ext/reactphysics3d/src/collision/narrowphase/NarrowPhaseAlgorithm.h>
#include <ext/reactphysics3d/src/constraint/ContactPoint.h>
#include <ext/reactphysics3d/src/engine/CollisionWorld.h>
#include <ext/reactphysics3d/src/engine/OverlappingPair.h>
#include <ext/reactphysics3d/src/mathematics/Transform.h>
#include <ext/reactphysics3d/src/memory/MemoryAllocator.h>

#include <tdme/engine/Transformations.h>
#include <tdme/math/Vector3.h>

using std::to_string;

using tdme::engine::Transformations;
using tdme::engine::physics::CollisionResponse;
using tdme::engine::primitives::BoundingVolume;
using tdme::math::Vector3;

BoundingVolume::~BoundingVolume() {
	if (collisionShape != nullptr) delete collisionShape;
}

void BoundingVolume::computeBoundingBox() {
	collisionShape->computeAABB(collisionShapeAABB, collisionShapeTransform * collisionShapeLocalTransform);
	boundingBoxTransformed = BoundingBox(
		Vector3(collisionShapeAABB.getMin().x, collisionShapeAABB.getMin().y, collisionShapeAABB.getMin().z),
		Vector3(collisionShapeAABB.getMax().x, collisionShapeAABB.getMax().y, collisionShapeAABB.getMax().z)
	);
	centerTransformed = boundingBoxTransformed.getCenter();
}

void BoundingVolume::fromTransformations(const Transformations& transformations) {
	//
	setScale(transformations.getScale());

	// set transform
	collisionShapeTransform.setPosition(
		reactphysics3d::Vector3(
			transformations.getTranslation().getX(),
			transformations.getTranslation().getY(),
			transformations.getTranslation().getZ()
		)
	);
	collisionShapeTransform.setOrientation(
		reactphysics3d::Quaternion(
			transformations.getRotationsQuaternion().getX(),
			transformations.getRotationsQuaternion().getY(),
			transformations.getRotationsQuaternion().getZ(),
			transformations.getRotationsQuaternion().getW()
		)
	);

	// compute bounding box
	computeBoundingBox();
}

const Vector3& BoundingVolume::getScale() {
	return scale;
}

const Vector3& BoundingVolume::getCenter() const {
	return center;
}

const Vector3& BoundingVolume::getCenterTransformed() const {
	return centerTransformed;
}

BoundingBox& BoundingVolume::getBoundingBoxTransformed() {
	return boundingBoxTransformed;
}
