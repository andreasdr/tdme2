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
 * Point particle emitter
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::shared::model::LevelEditorEntityParticleSystem_PointParticleEmitter final
{
private:
	int32_t count;
	int64_t lifeTime;
	int64_t lifeTimeRnd;
	float mass;
	float massRnd;
	Vector3 position;
	Vector3 velocity;
	Vector3 velocityRnd;
	Color4 colorStart;
	Color4 colorEnd;

public:
	/**
	 * Public constructor
	 */
	LevelEditorEntityParticleSystem_PointParticleEmitter();

	/**
	 * @return count
	 */
	inline int32_t getCount() {
		return count;
	}

	/**
	 * Set count
	 * @param count count
	 */
	inline void setCount(int32_t count) {
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
	 * @return position
	 */
	inline Vector3& getPosition() {
		return position;
	}

	/**
	 * Set position
	 * @param position position
	 */
	inline void setPosition(const Vector3& position) {
		this->position = position;
	}

	/**
	 * @return velocity
	 */
	inline const Vector3& getVelocity() {
		return velocity;
	}

	/**
	 * Set velocity
	 * @param velocity velocity
	 */
	inline void setVelocity(const Vector3& velocity) {
		this->velocity = velocity;
	}

	/**
	 * @return velocity rnd
	 */
	inline const Vector3& getVelocityRnd() {
		return velocityRnd;
	}

	/**
	 * Set random velocity
	 * @param velocityRnd random velocity
	 */
	inline void setVelocityRnd(const Vector3& velocityRnd) {
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

};
