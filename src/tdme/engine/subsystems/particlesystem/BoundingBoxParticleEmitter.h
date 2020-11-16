#pragma once

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/primitives/OrientedBoundingBox.h>
#include <tdme/engine/subsystems/particlesystem/fwd-tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Vector3.h>
#include <tdme/engine/subsystems/particlesystem/ParticleEmitter.h>

using tdme::engine::subsystems::particlesystem::ParticleEmitter;
using tdme::engine::Transformations;
using tdme::engine::model::Color4;
using tdme::engine::primitives::OrientedBoundingBox;
using tdme::engine::subsystems::particlesystem::Particle;
using tdme::math::Vector3;

/**
 * Bounding box particle emitter
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::subsystems::particlesystem::BoundingBoxParticleEmitter final
	: public ParticleEmitter
{
private:
	int32_t count;
	int64_t lifeTime;
	int64_t lifeTimeRnd;
	float mass;
	float massRnd;
	OrientedBoundingBox* obb { nullptr };
	OrientedBoundingBox* obbTransformed { nullptr };
	Vector3 velocity;
	Vector3 velocityRnd;
	Color4 colorStart;
	Color4 colorEnd;
public:
	/**
	 * Public constructor
	 * @param count particles to emit in one second
	 * @param lifeTime life time in milli seconds
	 * @param lifeTimeRnd life time rnd in milli seconds
	 * @param mass mass in kg
	 * @param massRnd mass rnd in kg
	 * @param obb oriented bounding box
	 * @param velocity velocity in meter / seconds
	 * @param velocityRnd velocity rnd in meter / seconds
	 * @param colorStart color start
	 * @param colorEnd color end
	 */
	BoundingBoxParticleEmitter(int32_t count, int64_t lifeTime, int64_t lifeTimeRnd, float mass, float massRnd, OrientedBoundingBox* obb, const Vector3& velocity, const Vector3& velocityRnd, const Color4& colorStart, const Color4& colorEnd);

	/**
	 * Destructor
	 */
	virtual ~BoundingBoxParticleEmitter();

	// overridden methods
	inline const Vector3& getCenter() const override {
		return obbTransformed->getCenter();
	}

	inline int32_t getCount() const override {
		return count;
	}

	inline const Vector3& getVelocity() const {
		return velocity;
	}

	inline const Vector3& getVelocityRnd() const {
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
