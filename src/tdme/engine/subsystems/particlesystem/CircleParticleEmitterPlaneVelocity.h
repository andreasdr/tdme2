
#pragma once

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/subsystems/particlesystem/fwd-tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Vector3.h>
#include <tdme/engine/subsystems/particlesystem/ParticleEmitter.h>

using tdme::engine::subsystems::particlesystem::ParticleEmitter;
using tdme::engine::Transformations;
using tdme::engine::model::Color4;
using tdme::engine::subsystems::particlesystem::Particle;
using tdme::math::Vector3;

/**
 * Circle particle emitter with velocity that lives in plane only
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::subsystems::particlesystem::CircleParticleEmitterPlaneVelocity final
	: public ParticleEmitter
{
private:
	int32_t count;
	int64_t lifeTime;
	int64_t lifeTimeRnd;
	Vector3 axis0;
	Vector3 axis1;
	Vector3 center;
	float radius;
	float mass;
	float massRnd;
	float velocity;
	float velocityRnd;
	Color4 colorStart;
	Color4 colorEnd;
	Vector3 centerTransformed;
	float radiusTransformed;
	Vector3 axis0Transformed;
	Vector3 axis1Transformed;

public:
	/**
	 * Public constructor
	 * @param count particles to emit in one second
	 * @param lifeTime life time
	 * @param lifeTimeRnd life time rnd
	 * @param axis0 axis 0
	 * @param axis1 axis 1
	 * @param center center
	 * @param radius radius
	 * @param mass mass
	 * @param massRnd mass rnd
	 * @param velocity velocity
	 * @param velocityRnd velocity rnd
	 * @param colorStart color start
	 * @param colorEnd color end
	 */
	CircleParticleEmitterPlaneVelocity(int32_t count, int64_t lifeTime, int64_t lifeTimeRnd, const Vector3& axis0, const Vector3& axis1, const Vector3& center, float radius, float mass, float massRnd, float velocity, float velocityRnd, const Color4& colorStart, const Color4& colorEnd);

	// overridden methods
	inline const Vector3& getCenter() const override {
		return centerTransformed;
	}

	inline int32_t getCount() const override {
		return count;
	}

	inline float getVelocity() const {
		return velocity;
	}

	inline float getVelocityRnd() const {
		return velocityRnd;
	}

	inline const Color4& getColorStart() const override {
		return colorStart;
	}

	inline void setColorStart(const Color4& colorStart) override {
		this->colorStart = colorStart;
	}

	inline const Color4& getColorEnd() const override {
		return colorEnd;
	}

	inline void setColorEnd(const Color4& colorEnd) override {
		this->colorEnd = colorEnd;
	}

	void emit(Particle* particle) override;
	void fromTransformations(const Transformations& transformations) override;

};
