#include <tdme/engine/PointsParticleSystem.h>

#include <string>

#include <tdme/engine/fileio/textures/fwd-tdme.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Partition.h>
#include <tdme/engine/Transformations.h>

using std::string;

using tdme::engine::fileio::textures::Texture;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::Engine;
using tdme::engine::Partition;
using tdme::engine::PointsParticleSystem;
using tdme::engine::Transformations;

PointsParticleSystem::PointsParticleSystem(const string& id, ParticleEmitter* emitter, int32_t maxPoints, float pointSize, bool autoEmit, Texture* texture, int32_t textureHorizontalSprites, int32_t textureVerticalSprites, float fps) :
	PointsParticleSystemInternal(id, emitter, maxPoints, pointSize, autoEmit, texture, textureHorizontalSprites, textureVerticalSprites, fps)
{
}

void PointsParticleSystem::initialize()
{
	PointsParticleSystemInternal::initialize();
}

void PointsParticleSystem::fromTransformations(const Transformations& transformations)
{
	PointsParticleSystemInternal::fromTransformations(transformations);
	if (parentEntity == nullptr && frustumCulling == true && engine != nullptr && enabled == true) engine->partition->updateEntity(this);
}

void PointsParticleSystem::update()
{
	PointsParticleSystemInternal::update();
	if (parentEntity == nullptr && frustumCulling == true && engine != nullptr && enabled == true) engine->partition->updateEntity(this);
}

void PointsParticleSystem::setEnabled(bool enabled)
{
	// return if enable state has not changed
	if (this->enabled == enabled) return;
	// frustum if root entity
	if (parentEntity == nullptr) {
		// frustum culling enabled?
		if (frustumCulling == true) {
			// yeo, add or remove from partition
			if (enabled == true) {
				if (engine != nullptr) engine->partition->addEntity(this);
			} else {
				if (engine != nullptr) engine->partition->removeEntity(this);
			}
		}
	}
	// call parent class::setEnabled()
	PointsParticleSystemInternal::setEnabled(enabled);
}

void PointsParticleSystem::updateParticles()
{
	PointsParticleSystemInternal::updateParticles();
	if (parentEntity == nullptr && frustumCulling == true && engine != nullptr && enabled == true) engine->partition->updateEntity(this);
}

bool PointsParticleSystem::isFrustumCulling() {
	return frustumCulling;
}

void PointsParticleSystem::setFrustumCulling(bool frustumCulling) {
	// check if enabled and engine attached
	if (enabled == true && engine != nullptr) {
		// had frustum culling
		if (this->frustumCulling == true) {
			// yep, remove if set to false now
			if (frustumCulling == false) engine->partition->removeEntity(this);
		} else {
			// yep, add if set to true now
			if (frustumCulling == true) engine->partition->addEntity(this);
		}
	}
	this->frustumCulling = frustumCulling;
	// delegate change to engine
	engine->registerEntity(this);
}

void PointsParticleSystem::setAutoEmit(bool autoEmit) {
	if (this->isAutoEmit() == autoEmit) return;
	// delegate to base class
	PointsParticleSystemInternal::setAutoEmit(autoEmit);
	// delegate change to engine
	engine->registerEntity(this);
}

void PointsParticleSystem::dispose()
{
	PointsParticleSystemInternal::dispose();
}

void PointsParticleSystem::setEngine(Engine* engine)
{
	if (this->engine != nullptr) this->engine->deregisterEntity(this);
	this->engine = engine;
	if (engine != nullptr) engine->registerEntity(this);
	PointsParticleSystemInternal::setEngine(engine);
}

void PointsParticleSystem::setRenderer(Renderer* renderer)
{
	PointsParticleSystemInternal::setRenderer(renderer);
}

