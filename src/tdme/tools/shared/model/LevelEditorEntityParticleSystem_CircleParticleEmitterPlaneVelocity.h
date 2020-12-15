#pragma once

#include <tdme/tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Vector3.h>
#include <tdme/tools/shared/model/fwd-tdme.h>

using tdme::engine::model::Color4;
using tdme::math::Vector3;

/**
 * Circle particle emitter plane velocity
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::shared::model::LevelEditorEntityParticleSystem_CircleParticleEmitterPlaneVelocity final
{
private:
	int count;
	int64_t lifeTime;
	int64_t lifeTimeRnd;
	float mass;
	float massRnd;
	float velocity;
	float velocityRnd;
	Color4 colorStart;
	Color4 colorEnd;
	Vector3 center;
	float radius;
	Vector3 axis0;
	Vector3 axis1;

public:
	/**
	 * Public constructor
	 */
	LevelEditorEntityParticleSystem_CircleParticleEmitterPlaneVelocity();

	/**
	 * @return count
	 */
	inline int getCount() {
		return count;
	}

	/**
	 * Set count
	 * @param count count
	 */
	inline void setCount(int count) {
		this->count = count;
	}

	/**
	 * @return life time
	 */
	inline int64_t getLifeTime() {
		return lifeTime;
	}

	/**
	 * Set life time
	 * @param lifeTime life time
	 */
	inline void setLifeTime(int64_t lifeTime) {
		this->lifeTime = lifeTime;
	}

	/**
	 * @return life time rnd
	 */
	inline int64_t getLifeTimeRnd() {
		return lifeTimeRnd;
	}

	/**
	 * Set life time rnd
	 * @param lifeTimeRnd life time rnd
	 */
	inline void setLifeTimeRnd(int64_t lifeTimeRnd) {
		this->lifeTimeRnd = lifeTimeRnd;
	}

	/**
	 * @return mass
	 */
	inline float getMass() {
		return mass;
	}

	/**
	 * Set mass
	 * @param mass mass
	 */
	inline void setMass(float mass) {
		this->mass = mass;
	}

	/**
	 * @return mass rnd
	 */
	inline float getMassRnd() {
		return massRnd;
	}

	/**
	 * Set mass rnd
	 * @param massRnd mass rnd
	 */
	inline void setMassRnd(float massRnd) {
		this->massRnd = massRnd;
	}

	/**
	 * @return velocity
	 */
	inline float getVelocity() {
		return velocity;
	}

	/**
	 * Set velocity
	 * @param velocity velocity
	 */
	inline void setVelocity(float velocity) {
		this->velocity = velocity;
	}

	/**
	 * @return velocity rnd
	 */
	inline float getVelocityRnd() {
		return velocityRnd;
	}

	/**
	 * Set velocity rnd
	 * @param velocityRnd velocity rnd
	 */
	inline void setVelocityRnd(float velocityRnd) {
		this->velocityRnd = velocityRnd;
	}

	/**
	 * @return color start
	 */
	inline const Color4& getColorStart() {
		return colorStart;
	}

	/**
	 * Set start color
	 * @param colorStart start color
	 */
	inline void setColorStart(const Color4& colorStart) {
		this->colorStart = colorStart;
	}

	/**
	 * @return color end
	 */
	inline const Color4& getColorEnd() {
		return colorEnd;
	}

	/**
	 * Set end color
	 * @param colorEnd color end
	 */
	inline void setColorEnd(const Color4& colorEnd) {
		this->colorEnd = colorEnd;
	}

	/**
	 * @return center
	 */
	inline const Vector3& getCenter() {
		return center;
	}

	/**
	 * Set center
	 * @param center center
	 */
	inline void setCenter(const Vector3& center) {
		this->center = center;
	}

	/**
	 * @return radius
	 */
	inline float getRadius() {
		return radius;
	}

	/**
	 * Set radius
	 * @param radius radius
	 */
	inline void setRadius(float radius) {
		this->radius = radius;
	}

	/**
	 * @return axis 0
	 */
	inline const Vector3& getAxis0() {
		return axis0;
	}

	/**
	 * Set axis 0
	 * @param axis0 axis 0
	 */
	inline void setAxis0(const Vector3& axis0) {
		this->axis0 = axis0;
	}

	/**
	 * @return axis 1
	 */
	inline const Vector3& getAxis1() {
		return axis1;
	}

	/**
	 * Set axis 1
	 * @param axis0 axis 1
	 */
	inline void setAxis1(const Vector3& axis1) {
		this->axis1 = axis1;
	}

};
