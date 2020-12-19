#include <tdme/engine/subsystems/particlesystem/CircleParticleEmitterPlaneVelocity.h>

#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Color4Base.h>
#include <tdme/engine/subsystems/particlesystem/Particle.h>
#include <tdme/engine/Transformations.h>
#include <tdme/math/Math.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector3.h>

using tdme::engine::model::Color4;
using tdme::engine::model::Color4Base;
using tdme::engine::subsystems::particlesystem::CircleParticleEmitterPlaneVelocity;
using tdme::engine::subsystems::particlesystem::Particle;
using tdme::engine::Transformations;
using tdme::math::Math;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;

CircleParticleEmitterPlaneVelocity::CircleParticleEmitterPlaneVelocity(int32_t count, int64_t lifeTime, int64_t lifeTimeRnd, const Vector3& axis0, const Vector3& axis1, const Vector3& center, float radius, float mass, float massRnd, float velocity, float velocityRnd, const Color4& colorStart, const Color4& colorEnd)
{
	this->count = count;
	this->lifeTime = lifeTime;
	this->lifeTimeRnd = lifeTimeRnd;
	this->axis0.set(axis0).normalize();
	this->axis1.set(axis1).normalize();
	this->center.set(center);
	this->radius = radius;
	this->mass = mass;
	this->massRnd = massRnd;
	this->velocity = velocity;
	this->velocityRnd = velocityRnd;
	this->colorStart.set(colorStart);
	this->colorEnd.set(colorEnd);
	this->centerTransformed.set(center);
	this->radiusTransformed = radius;
	this->axis0Transformed.set(axis0).normalize();
	this->axis1Transformed.set(axis1).normalize();
}

void CircleParticleEmitterPlaneVelocity::emit(Particle* particle)
{
	Vector3 cosOnAxis0;
	Vector3 sinOnAxis1;
	// set up particle
	particle->active = true;
	particle->spriteIndex = 0.0f;
	// emit particle on circle spanned on axis 0 and axis 1
	auto rnd = Math::random();
	cosOnAxis0.set(axis0Transformed).scale(Math::cos(Math::PI * 2 * rnd));
	sinOnAxis1.set(axis1Transformed).scale(Math::sin(Math::PI * 2 * rnd));
	particle->position.set(cosOnAxis0);
	particle->position.add(sinOnAxis1);
	particle->position.scale(radiusTransformed);
	// compute velocity
	particle->velocity.set(particle->position).normalize().scale(velocity + (Math::random() * velocityRnd));
	// mass
	particle->mass = mass + static_cast< float >((Math::random() * (massRnd)));
	// life time
	particle->lifeTimeMax = lifeTime + static_cast< int64_t >((Math::random() * lifeTimeRnd));
	particle->lifeTimeCurrent = 0LL;
	// color
	particle->color.set(colorStart);
	particle->colorAdd.set(
		(colorEnd.getRed() - colorStart.getRed()) / particle->lifeTimeMax,
		(colorEnd.getGreen() - colorStart.getGreen()) / particle->lifeTimeMax,
		(colorEnd.getBlue() - colorStart.getBlue()) / particle->lifeTimeMax,
		(colorEnd.getAlpha() - colorStart.getAlpha()) / particle->lifeTimeMax);
}

void CircleParticleEmitterPlaneVelocity::fromTransformations(const Transformations& transformations)
{
	auto& transformationsMatrix = transformations.getTransformationsMatrix();
	// apply rotation, scale, translation
	transformationsMatrix.multiply(center, centerTransformed);
	// apply transformations rotation + scale to axis
	transformationsMatrix.multiplyNoTranslation(axis0, axis0Transformed);
	transformationsMatrix.multiplyNoTranslation(axis1, axis1Transformed);
	// scale and radius transformed
	Vector3 scale;
	transformationsMatrix.getScale(scale);
	radiusTransformed = radius * Math::max(scale.getX(), Math::max(scale.getY(), scale.getZ()));
}
