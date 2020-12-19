#include <tdme/engine/prototype/PrototypeParticleSystem_SphereParticleEmitter.h>

#include <tdme/engine/model/Color4.h>
#include <tdme/math/Vector3.h>

using tdme::engine::model::Color4;
using tdme::engine::prototype::PrototypeParticleSystem_SphereParticleEmitter;
using tdme::math::Vector3;

PrototypeParticleSystem_SphereParticleEmitter::PrototypeParticleSystem_SphereParticleEmitter()
{
	count = 2000;
	lifeTime = 1500;
	lifeTimeRnd = 500;
	mass = 0.0f;
	massRnd = 0.0f;
	velocity.set(0.0f, 1.0f, 0.0f);
	velocityRnd.set(0.0f, 0.5f, 0.0f);
	colorStart.set(0.5f, 0.5f, 0.5f, 0.5f);
	colorEnd.set(1.0f, 1.0f, 1.0f, 0.5f);
	center.set(0.0f, 0.5f, 0.0f);
	radius = 0.5f;
}
