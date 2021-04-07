#include <tdme/engine/primitives/Capsule.h>

#include <ext/reactphysics3d/src/collision/shapes/CapsuleShape.h>

#include <tdme/engine/primitives/BoundingVolume.h>
#include <tdme/engine/Transformations.h>
#include <tdme/math/Math.h>
#include <tdme/math/Math.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector3.h>

using tdme::engine::primitives::Capsule;

using tdme::engine::primitives::BoundingVolume;
using tdme::engine::Transformations;
using tdme::math::Math;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;

Capsule::Capsule(const Vector3& a, const Vector3& b, float radius, const Vector3& scale)
{
	this->a.set(a);
	this->b.set(b);
	this->radius = radius;
	center.set(a).add(b).scale(0.5f);
	setScale(scale);
}

float Capsule::getRadius() const
{
	return radius;
}

const Vector3& Capsule::getA() const
{
	return a;
}

const Vector3& Capsule::getB() const
{
	return b;
}

void Capsule::setScale(const Vector3& scale) {
	// store new scale
	this->scale.set(scale);

	// remove old collision shape
	if (collisionShape != nullptr) delete collisionShape;

	//
	Vector3 aScaled;
	Vector3 bScaled;
	aScaled.set(a).scale(scale);
	bScaled.set(b).scale(scale);

	// determine local rotation around Y axis
	Vector3 yAxis(0.0f, -1.0f, 0.0f);
	Vector3 abNormalized = aScaled.clone().sub(bScaled).normalize();
	auto& abNormalizedVectorXYZ = abNormalized.getArray();
	Vector3 rotationAxis;
	if (Math::abs(abNormalizedVectorXYZ[0]) < Math::EPSILON && Math::abs(abNormalizedVectorXYZ[2]) < Math::EPSILON) {
		rotationAxis.set(abNormalizedVectorXYZ[1], 0.0f, 0.0f);
	} else {
		rotationAxis = Vector3::computeCrossProduct(yAxis, abNormalized).normalize();
	}
	auto angle = Vector3::computeAngle(yAxis, abNormalized, yAxis);
	Quaternion rotationQuaternion;
	rotationQuaternion.rotate(rotationAxis, angle);

	// set shape orientation
	collisionShapeLocalTransform.setOrientation(
		reactphysics3d::Quaternion(
			rotationQuaternion.getX(),
			rotationQuaternion.getY(),
			rotationQuaternion.getZ(),
			rotationQuaternion.getW()
		)
	);

	// determine local translation
	collisionShapeLocalTranslation.set(center).scale(scale);
	collisionShapeLocalTransform.setPosition(
		reactphysics3d::Vector3(
			collisionShapeLocalTranslation.getX(),
			collisionShapeLocalTranslation.getY(),
			collisionShapeLocalTranslation.getZ()
		)
	);

	// create capsule
	collisionShape = new reactphysics3d::CapsuleShape(
		Math::max(Math::EPSILON, radius * Math::max(Math::abs(scale.getZ()), Math::max(Math::abs(scale.getX()), Math::abs(scale.getY())))),
		Math::max(Math::EPSILON, bScaled.clone().sub(aScaled).computeLength())
	);

	// compute bounding box
	computeBoundingBox();
}

BoundingVolume* Capsule::clone() const
{
	return new Capsule(a, b, radius, scale);
}
