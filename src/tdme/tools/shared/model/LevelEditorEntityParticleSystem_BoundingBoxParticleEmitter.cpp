#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem_BoundingBoxParticleEmitter.h>

#include <tdme/engine/model/Color4.h>
#include <tdme/math/Vector3.h>

using tdme::tools::shared::model::LevelEditorEntityParticleSystem_BoundingBoxParticleEmitter;
using tdme::engine::model::Color4;
using tdme::math::Vector3;

LevelEditorEntityParticleSystem_BoundingBoxParticleEmitter::LevelEditorEntityParticleSystem_BoundingBoxParticleEmitter()
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
