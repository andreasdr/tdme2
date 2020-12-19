#include <tdme/engine/subsystems/particlesystem/BoundingBoxParticleEmitter.h>

#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Color4Base.h>
#include <tdme/engine/primitives/BoundingVolume.h>
#include <tdme/engine/primitives/OrientedBoundingBox.h>
#include <tdme/engine/subsystems/particlesystem/Particle.h>
#include <tdme/engine/Transformations.h>
#include <tdme/math/Math.h>
#include <tdme/math/Vector3.h>

using tdme::engine::model::Color4;
using tdme::engine::model::Color4Base;
using tdme::engine::primitives::BoundingVolume;
using tdme::engine::primitives::OrientedBoundingBox;
using tdme::engine::subsystems::particlesystem::BoundingBoxParticleEmitter;
using tdme::engine::subsystems::particlesystem::Particle;
using tdme::engine::Transformations;
using tdme::math::Math;
using tdme::math::Vector3;

BoundingBoxParticleEmitter::BoundingBoxParticleEmitter(int32_t count, int64_t lifeTime, int64_t lifeTimeRnd, float mass, float massRnd, OrientedBoundingBox* obb, const Vector3& velocity, const Vector3& velocityRnd, const Color4& colorStart, const Color4& colorEnd)
{
	this->count = count;
	this->lifeTime = lifeTime;
	this->lifeTimeRnd = lifeTimeRnd;
	this->mass = mass;
	this->massRnd = massRnd;
	this->obb = obb;
	this->velocity.set(velocity);
	this->velocityRnd.set(velocityRnd);
	this->colorStart.set(colorStart);
	this->colorEnd.set(colorEnd);
	this->obbTransformed = static_cast<OrientedBoundingBox*>(obb->clone());
}

BoundingBoxParticleEmitter::~BoundingBoxParticleEmitter() {
	delete obb;
	delete obbTransformed;
}

void BoundingBoxParticleEmitter::emit(Particle* particle)
{
	Vector3 tmpAxis;
	// set up particle
	particle->active = true;
	particle->spriteIndex = 0.0f;
	auto obbAxes = obbTransformed->getAxes();
	auto& obbHalfExtensionXYZ = obbTransformed->getHalfExtension().getArray();
	// emit particle in oriented bounding box
	particle->position.set(0.0f, 0.0f, 0.0f);
	particle->position.add(tmpAxis.set(obbAxes[0]).scale((static_cast< float >(Math::random()) * obbHalfExtensionXYZ[0] * 2.0f) - obbHalfExtensionXYZ[0]));
	particle->position.add(tmpAxis.set(obbAxes[1]).scale((static_cast< float >(Math::random()) * obbHalfExtensionXYZ[1] * 2.0f) - obbHalfExtensionXYZ[1]));
	particle->position.add(tmpAxis.set(obbAxes[2]).scale((static_cast< float >(Math::random()) * obbHalfExtensionXYZ[2] * 2.0f) - obbHalfExtensionXYZ[2]));
	// compute velocity
	particle->velocity.set(
		velocity[0] + (Math::random() * velocityRnd[0] * (Math::random() > 0.5 ? +1.0f : -1.0f)),
		velocity[1] + (Math::random() * velocityRnd[1] * (Math::random() > 0.5 ? +1.0f : -1.0f)),
		velocity[2] + (Math::random() * velocityRnd[2] * (Math::random() > 0.5 ? +1.0f : -1.0f))
	);
	// mass
	particle->mass = mass + (Math::random() * (massRnd));
	// life time
	particle->lifeTimeMax = lifeTime + (Math::random() * lifeTimeRnd);
	particle->lifeTimeCurrent = 0LL;
	// color
	particle->color.set(colorStart);
	particle->colorAdd.set(
		(colorEnd.getRed() - colorStart.getRed()) / particle->lifeTimeMax,
		(colorEnd.getGreen() - colorStart.getGreen()) / particle->lifeTimeMax,
		(colorEnd.getBlue() - colorStart.getBlue()) / particle->lifeTimeMax,
		(colorEnd.getAlpha() - colorStart.getAlpha()) / particle->lifeTimeMax);
}

void BoundingBoxParticleEmitter::fromTransformations(const Transformations& transformations)
{
	Vector3 center;
	Vector3 scale;
	array<Vector3, 3> axes;
	array<Vector3, 3> axesTransformed;
	Vector3 halfExtension;
	auto& transformationsMatrix = transformations.getTransformationsMatrix();
	// apply rotation, scale, translation
	transformationsMatrix.multiply(obb->getCenter(), center);
	// apply transformations rotation to axis
	transformationsMatrix.multiplyNoTranslation(obb->getAxes()[0], axesTransformed[0]);
	transformationsMatrix.multiplyNoTranslation(obb->getAxes()[1], axesTransformed[1]);
	transformationsMatrix.multiplyNoTranslation(obb->getAxes()[2], axesTransformed[2]);
	// scale
	scale.set(
		axesTransformed[0].computeLength(),
		axesTransformed[1].computeLength(),
		axesTransformed[2].computeLength()
	);
	// set up axes
	axes[0].set(axesTransformed[0]).normalize();
	axes[1].set(axesTransformed[1]).normalize();
	axes[2].set(axesTransformed[2]).normalize();
	// apply scale to half extension
	halfExtension.set(obb->getHalfExtension());
	halfExtension.scale(scale);
	*obbTransformed = OrientedBoundingBox(center, axes[0], axes[1], axes[2], halfExtension);
}
