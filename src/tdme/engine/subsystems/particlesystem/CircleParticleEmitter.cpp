#include <tdme/engine/subsystems/particlesystem/CircleParticleEmitter.h>

#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Color4Base.h>
#include <tdme/engine/subsystems/particlesystem/Particle.h>
#include <tdme/engine/Transformations.h>
#include <tdme/math/Math.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector3.h>

using tdme::engine::model::Color4;
using tdme::engine::model::Color4Base;
using tdme::engine::subsystems::particlesystem::CircleParticleEmitter;
using tdme::engine::subsystems::particlesystem::Particle;
using tdme::engine::Transformations;
using tdme::math::Math;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;

CircleParticleEmitter::CircleParticleEmitter(int32_t count, int64_t lifeTime, int64_t lifeTimeRnd, const Vector3& axis0, const Vector3& axis1, const Vector3& center, float radius, float mass, float massRnd, const Vector3& velocity, const Vector3& velocityRnd, const Color4& colorStart, const Color4& colorEnd)
{
	this->count = count;
	this->lifeTime = lifeTime;
	this->lifeTimeRnd = lifeTimeRnd;
	this->axis0.set(axis0).normalize();
	this->axis1.set(axis1).normalize();
	this->center.set(center);
	this->radius = radius;
	this->axis0Transformed.set(axis0).normalize();
	this->axis1Transformed.set(axis1).normalize();
	this->centerTransformed.set(center);
	this->radiusTransformed = radius;
	this->mass = mass;
	this->massRnd = massRnd;
	this->velocity.set(velocity);
	this->velocityRnd.set(velocityRnd);
	this->colorStart.set(colorStart);
	this->colorEnd.set(colorEnd);
}

void CircleParticleEmitter::emit(Particle* particle)
{
	Vector3 cosOnAxis0;
	Vector3 sinOnAxis1;
	// set up particle
	particle->active = true;
	particle->spriteIndex = 0.0f;
	// emit particle in circle spanned on axis 0 and axis 1
	auto rnd = static_cast<float>(Math::random());
	cosOnAxis0.set(axis0Transformed).scale(Math::cos(Math::PI * 2 * rnd));
	sinOnAxis1.set(axis1Transformed).scale(Math::sin(Math::PI * 2 * rnd));
	particle->position.set(cosOnAxis0);
	particle->position.add(sinOnAxis1);
	particle->position.scale(radiusTransformed);
	// compute velocity
	particle->velocity.set(
		velocity[0] + (Math::random() * velocityRnd[0] * (Math::random() > 0.5 ? +1.0f : -1.0f)),
		velocity[1] + (Math::random() * velocityRnd[1] * (Math::random() > 0.5 ? +1.0f : -1.0f)),
		velocity[2] + (Math::random() * velocityRnd[2] * (Math::random() > 0.5 ? +1.0f : -1.0f))
	);
	// mass
	particle->mass = mass + static_cast<float>((Math::random() * (massRnd)));
	// life time
	particle->lifeTimeMax = lifeTime + static_cast< int64_t >((Math::random() * lifeTimeRnd));
	particle->lifeTimeCurrent = 0LL;
	// color
	particle->color.set(colorStart);
	particle->colorAdd.set(
		(colorEnd.getRed() - colorStart.getRed()) / particle->lifeTimeMax,
		(colorEnd.getGreen() - colorStart.getGreen()) / particle->lifeTimeMax,
		(colorEnd.getBlue() - colorStart.getBlue()) / particle->lifeTimeMax,
		(colorEnd.getAlpha() - colorStart.getAlpha()) / particle->lifeTimeMax
	);
}

void CircleParticleEmitter::fromTransformations(const Transformations& transformations)
{
	auto& transformationsMatrix = transformations.getTransformationsMatrix();
	// apply rotation, scale, translation
	transformationsMatrix.multiply(center, centerTransformed);
	// apply transformations rotation + scale to axis
	transformationsMatrix.multiplyNoTranslation(axis0, axis0Transformed).normalize();
	transformationsMatrix.multiplyNoTranslation(axis1, axis1Transformed).normalize();
	// scale and radius transformed
	Vector3 scale;
	transformationsMatrix.getScale(scale);
	radiusTransformed = radius * Math::max(scale.getX(), Math::max(scale.getY(), scale.getZ()));
}
