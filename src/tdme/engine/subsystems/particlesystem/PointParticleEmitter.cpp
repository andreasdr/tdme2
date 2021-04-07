#include <tdme/engine/subsystems/particlesystem/PointParticleEmitter.h>

#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Color4Base.h>
#include <tdme/engine/subsystems/particlesystem/Particle.h>
#include <tdme/engine/Transformations.h>
#include <tdme/math/Math.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector3.h>

using tdme::engine::model::Color4;
using tdme::engine::model::Color4Base;
using tdme::engine::subsystems::particlesystem::Particle;
using tdme::engine::subsystems::particlesystem::PointParticleEmitter;
using tdme::engine::Transformations;
using tdme::math::Math;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;

PointParticleEmitter::PointParticleEmitter(int32_t count, int64_t lifeTime, int64_t lifeTimeRnd, float mass, float massRnd, const Vector3& position, const Vector3& velocity, const Vector3& velocityRnd, const Color4& colorStart, const Color4& colorEnd)
{
	this->count = count;
	this->lifeTime = lifeTime;
	this->lifeTimeRnd = lifeTimeRnd;
	this->mass = mass;
	this->massRnd = massRnd;
	this->position.set(position);
	this->positionTransformed.set(position);
	this->velocity.set(velocity);
	this->velocityRnd.set(velocityRnd);
	this->colorStart.set(colorStart);
	this->colorEnd.set(colorEnd);
}

void PointParticleEmitter::emit(Particle* particle)
{
	// set up particle
	particle->active = true;
	particle->spriteIndex = 0.0f;
	particle->position.set(0.0f, 0.0f, 0.0f);
	particle->velocity.set(
		velocity[0] + (Math::random() * velocityRnd[0] * (Math::random() > 0.5 ? +1.0f : -1.0f)),
		velocity[1] + (Math::random() * velocityRnd[1] * (Math::random() > 0.5 ? +1.0f : -1.0f)),
		velocity[2] + (Math::random() * velocityRnd[2] * (Math::random() > 0.5 ? +1.0f : -1.0f))
	);
	particle->mass = mass + static_cast<float>((Math::random() * (massRnd)));
	particle->lifeTimeMax = lifeTime + static_cast< int64_t >((Math::random() * lifeTimeRnd));
	particle->lifeTimeCurrent = 0LL;
	particle->color.set(colorStart);
	particle->colorAdd.set(
		(colorEnd.getRed() - colorStart.getRed()) / particle->lifeTimeMax,
		(colorEnd.getGreen() - colorStart.getGreen()) / particle->lifeTimeMax,
		(colorEnd.getBlue() - colorStart.getBlue()) / particle->lifeTimeMax,
		(colorEnd.getAlpha() - colorStart.getAlpha()) / particle->lifeTimeMax
	);
}

void PointParticleEmitter::fromTransformations(const Transformations& transformations)
{
	//
	auto& transformationsMatrix = transformations.getTransformationsMatrix();
	// apply translations
	positionTransformed = transformationsMatrix.multiply(position);
}
