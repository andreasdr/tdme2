#include <tdme/engine/prototype/PrototypeParticleSystem_CircleParticleEmitterPlaneVelocity.h>

#include <tdme/engine/model/Color4.h>
#include <tdme/math/Vector3.h>

using tdme::engine::model::Color4;
using tdme::engine::prototype::PrototypeParticleSystem_CircleParticleEmitterPlaneVelocity;
using tdme::math::Vector3;

PrototypeParticleSystem_CircleParticleEmitterPlaneVelocity::PrototypeParticleSystem_CircleParticleEmitterPlaneVelocity()
{
	count = 2000;
	lifeTime = 1500;
	lifeTimeRnd = 500;
	mass = 0.0f;
	massRnd = 0.0f;
	velocity = 1.0f;
	velocityRnd = 0.5f;
	colorStart.set(0.5f, 0.5f, 0.5f, 0.5f);
	colorEnd.set(1.0f, 1.0f, 1.0f, 0.5f);
	center.set(0.0f, 0.0f, 0.0f);
	radius = 0.5f;
	axis0.set(1.0f, 0.0f, 0.0f);
	axis1.set(0.0f, 0.0f, 1.0f);
}
