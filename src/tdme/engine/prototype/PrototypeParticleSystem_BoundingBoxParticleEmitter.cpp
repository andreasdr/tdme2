#include <tdme/engine/prototype/PrototypeParticleSystem_BoundingBoxParticleEmitter.h>

#include <tdme/engine/model/Color4.h>
#include <tdme/math/Vector3.h>

using tdme::engine::model::Color4;
using tdme::engine::prototype::PrototypeParticleSystem_BoundingBoxParticleEmitter;
using tdme::math::Vector3;

PrototypeParticleSystem_BoundingBoxParticleEmitter::PrototypeParticleSystem_BoundingBoxParticleEmitter()
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
	obbCenter.set(0.0f, 0.5f, 0.0f);
	obbHalfextension.set(0.5f, 0.5f, 0.5f);
	obbAxis0.set(1.0f, 0.0f, 0.0f);
	obbAxis1.set(0.0f, 1.0f, 0.0f);
	obbAxis2.set(0.0f, 0.0f, 1.0f);
}
