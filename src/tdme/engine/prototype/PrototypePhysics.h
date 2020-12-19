#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/prototype/fwd-tdme.h>
#include <tdme/engine/prototype/PrototypePhysics_BodyType.h>
#include <tdme/math/Vector3.h>

using std::string;

using tdme::engine::prototype::Prototype;
using tdme::math::Vector3;

/**
 * Prototype physics body definitions
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::prototype::PrototypePhysics final
{
private:
	PrototypePhysics_BodyType* type { PrototypePhysics_BodyType::STATIC_RIGIDBODY };
	float mass { 0.0f };
	float restitution { 0.5f };
	float friction { 0.5f };
	Vector3 inertiaTensor;

public:

	/**
	 * Public constructor
	 */
	inline PrototypePhysics(): inertiaTensor(1.0f, 1.0f, 1.0f) {
	}

	/**
	 * Destructor
	 */
	~PrototypePhysics();

	/**
	 * @return type
	 */
	inline PrototypePhysics_BodyType* getType() const {
		return type;
	}

	/**
	 * Set type
	 * @param type type
	 */
	inline void setType(PrototypePhysics_BodyType* type) {
		this->type = type;
	}

	/**
	 * @return friction
	 */
	inline float getFriction() const {
		return friction;
	}

	/**
	 * Set friction
	 * @param friction friction
	 */
	inline void setFriction(float friction) {
		this->friction = friction;
	}

	/**
	 * @return mass
	 */
	inline float getMass() const {
		return mass;
	}

	/**
	 * Set mass in kg
	 * @param mass mass
	 */
	inline void setMass(float mass) {
		this->mass = mass;
	}

	/**
	 * @return restitution / bounciness
	 */
	inline float getRestitution() const {
		return restitution;
	}

	/**
	 * Set restitution
	 * @param restitution restitution
	 */
	inline void setRestitution(float restitution) {
		this->restitution = restitution;
	}

	/**
	 * @return inertia tensor
	 */
	inline const Vector3& getInertiaTensor() const {
		return inertiaTensor;
	}

	/**
	 * Set inertia tensor
	 * @param inertiaTensor inertia tensor
	 */
	inline void setInertiaTensor(const Vector3& inertiaTensor) {
		this->inertiaTensor = inertiaTensor;
	}

};
