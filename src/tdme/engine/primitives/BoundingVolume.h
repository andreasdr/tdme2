#pragma once

#include <ext/reactphysics3d/src/collision/shapes/AABB.h>
#include <ext/reactphysics3d/src/collision/shapes/CollisionShape.h>
#include <ext/reactphysics3d/src/mathematics/Transform.h>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/physics/fwd-tdme.h>
#include <tdme/engine/primitives/fwd-tdme.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Vector3.h>

using tdme::engine::Transformations;
using tdme::engine::primitives::BoundingBox;
using tdme::math::Vector3;

/**
 * Bounding volume interface
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::primitives::BoundingVolume
{
	friend class tdme::engine::physics::Body;
	friend class tdme::engine::physics::World;
	friend class BoundingBox;
	friend class Capsule;
	friend class ConvexMesh;
	friend class OrientedBoundingBox;
	friend class Sphere;
	friend class PrimitiveModel;

protected:
	Vector3 scale;
	Vector3 center;
	Vector3 collisionShapeLocalTranslation;
	reactphysics3d::CollisionShape* collisionShape { nullptr };
	reactphysics3d::Transform collisionShapeLocalTransform;
	reactphysics3d::Transform collisionShapeTransform;
	reactphysics3d::AABB collisionShapeAABB;
	BoundingBox boundingBoxTransformed;
	Vector3 centerTransformed;

	/**
	 * Compute bounding box
	 */
	void computeBoundingBox();

public:
	/**
	 * Destructor
	 */
	virtual ~BoundingVolume();

	/**
	 * Transform bounding volume from given transformations
	 * @param transformations transformations
	 */
	virtual void fromTransformations(const Transformations& transformations);

	/**
	 * Get local scale
	 * @return scale
	 */
	const Vector3& getScale();

	/**
	 * Set local scale
	 * @return if collision shape had been recreated
	 */
	virtual void setScale(const Vector3& scale) = 0;

	/**
	 * @return center
	 */
	const Vector3& getCenter() const;

	/**
	 * @return transformed center
	 */
	const Vector3& getCenterTransformed() const;

	/**
	 * Get bounding box transformed
	 * @return bounding box
	 */
	BoundingBox& getBoundingBoxTransformed();

	/**
	 * Clones this bounding volume
	 * @return cloned bounding volume
	 */
	virtual BoundingVolume* clone() const = 0;

};
