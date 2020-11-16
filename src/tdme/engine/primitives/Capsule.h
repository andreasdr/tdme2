#pragma once

#include <tdme/tdme.h>
#include <tdme/engine/primitives/fwd-tdme.h>
#include <tdme/engine/primitives/BoundingVolume.h>
#include <tdme/math/Vector3.h>

using tdme::engine::primitives::BoundingVolume;
using tdme::math::Vector3;

/**
 * Capsule physics primitive
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::primitives::Capsule final
	: public BoundingVolume
{
public:
	/**
	 * Public constructor
	 * @param a a
	 * @param b b
	 * @param radius radius
	 * @param scale scale
	 */
	Capsule(const Vector3& a, const Vector3& b, float radius, const Vector3& scale = Vector3(1.0f, 1.0f, 1.0f));

	/**
	 * @return radius
	 */
	float getRadius() const;

	/**
	 * @return line segment point a
	 */
	const Vector3& getA() const;

	/**
	 * @return line segment point b
	 */
	const Vector3& getB() const;


	// overrides
	void setScale(const Vector3& scale) override;
	BoundingVolume* clone() const override;

private:
	//
	Vector3 a;
	Vector3 b;
	float radius;
};
