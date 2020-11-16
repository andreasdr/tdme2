#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/math/Vector3.h>
#include <tdme/tools/shared/model/fwd-tdme.h>
#include <tdme/tools/shared/model/LevelEditorEntityPhysics_BodyType.h>

using std::string;

using tdme::math::Vector3;
using tdme::tools::shared::model::LevelEditorEntity;

/**
 * Level editor entity physics body settings
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::shared::model::LevelEditorEntityPhysics final
{
private:
	LevelEditorEntityPhysics_BodyType* type { LevelEditorEntityPhysics_BodyType::STATIC_RIGIDBODY };
	float mass { 0.0f };
	float restitution { 0.5f };
	float friction { 0.5f };
	Vector3 inertiaTensor;

public:

	/**
	 * Public constructor
	 */
	inline LevelEditorEntityPhysics(): inertiaTensor(1.0f, 1.0f, 1.0f) {
	}

	/**
	 * Destructor
	 */
	~LevelEditorEntityPhysics();

	/**
	 * @return type
	 */
	inline LevelEditorEntityPhysics_BodyType* getType() const {
		return type;
	}

	/**
	 * Set type
	 * @param type type
	 */
	inline void setType(LevelEditorEntityPhysics_BodyType* type) {
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
