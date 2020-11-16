#pragma once

#include <tdme/tdme.h>
#include <tdme/engine/primitives/fwd-tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Vector3.h>

using tdme::math::Vector3;

/**
 * Plane entity, this is not directly connectable with physics engine
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::primitives::Plane final
{
private:
	Vector3 normal;
	float distance;

public:
	/**
	 * Public constructor
	 * @param normal normal
	 * @param distance distance
	 */
	inline Plane(const Vector3& normal, float distance) {
		this->normal.set(normal);
		this->distance = distance;
	}

	/**
	 * @return float distance from origin
	 */
	inline float getDistance() {
		return distance;
	}

	/**
	 * Set up distance from origin
	 * @param distance distance
	 */
	inline void setDistance(float distance) {
		this->distance = distance;
	}

	/**
	 * @return normal
	 */
	inline Vector3& getNormal() {
		return normal;
	}

	/**
	 * Public constructor
	 */
	inline Plane() {
		normal.set(0.0f, 0.0f, 0.0f);
		distance = 0.0f;
	}

};
