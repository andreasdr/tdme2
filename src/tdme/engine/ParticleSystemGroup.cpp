#include <tdme/engine/ParticleSystemGroup.h>

#include <string>
#include <vector>

#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/FogParticleSystem.h>
#include <tdme/engine/Object3D.h>
#include <tdme/engine/ObjectParticleSystem.h>
#include <tdme/engine/ParticleSystemEntity.h>
#include <tdme/engine/Partition.h>
#include <tdme/engine/PointsParticleSystem.h>
#include <tdme/engine/Transformations.h>

using std::string;
using std::vector;

using tdme::engine::primitives::BoundingBox;
using tdme::engine::Engine;
using tdme::engine::FogParticleSystem;
using tdme::engine::Object3D;
using tdme::engine::ObjectParticleSystem;
using tdme::engine::ParticleSystemEntity;
using tdme::engine::ParticleSystemGroup;
using tdme::engine::Partition;
using tdme::engine::PointsParticleSystem;
using tdme::engine::Transformations;

ParticleSystemGroup::ParticleSystemGroup(const string& id, bool autoEmit, bool contributesShadows, bool receivesShadows, const vector<ParticleSystemEntity*>& particleSystems) :
	id(id), autoEmit(autoEmit), contributesShadows(contributesShadows), receivesShadows(receivesShadows), particleSystems(particleSystems)
{
	this->enabled = true;
	this->pickable = false;
	this->effectColorMul.set(1.0f, 1.0f, 1.0f, 1.0f);
	this->effectColorAdd.set(0.0f, 0.0f, 0.0f, 0.0f);
	this->contributesShadows = false;
	this->receivesShadows = false;
	// TODO: put parent entity into a interface
	for (auto particleSystem: particleSystems) {
		auto ops = dynamic_cast<ObjectParticleSystem*>(particleSystem);
		if (ops != nullptr) ops->setParentEntity(this);
		auto pps = dynamic_cast<PointsParticleSystem*>(particleSystem);
		if (pps != nullptr) pps->setParentEntity(this);
		auto fps = dynamic_cast<FogParticleSystem*>(particleSystem);
		if (fps != nullptr) fps->setParentEntity(this);
	}
}

ParticleSystemGroup::~ParticleSystemGroup() {
	for (auto particleSystem: particleSystems) delete particleSystem;
}

void ParticleSystemGroup::initialize()
{
	for (auto particleSystem: particleSystems) dynamic_cast<Entity*>(particleSystem)->initialize();
}

void ParticleSystemGroup::fromTransformations(const Transformations& transformations)
{
	Transformations::fromTransformations(transformations);
	//
	for (auto particleSystem: particleSystems) dynamic_cast<Entity*>(particleSystem)->fromTransformations(transformations);
	// update bounding box transformed
	boundingBoxTransformed.fromBoundingVolumeWithTransformations(&boundingBox, *this);
	// update object
	if (parentEntity == nullptr && frustumCulling == true && engine != nullptr && enabled == true) engine->partition->updateEntity(this);
}

void ParticleSystemGroup::update()
{
	Transformations::update();
	//
	for (auto particleSystem: particleSystems) dynamic_cast<Entity*>(particleSystem)->fromTransformations(*this);
	// update bounding box transformed
	boundingBoxTransformed.fromBoundingVolumeWithTransformations(&boundingBox, *this);
	// update object
	if (parentEntity == nullptr && frustumCulling == true && engine != nullptr && enabled == true) engine->partition->updateEntity(this);
}

void ParticleSystemGroup::setEnabled(bool enabled)
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

	//
	this->enabled = enabled;
}

void ParticleSystemGroup::updateParticles()
{
	// skip if no ps attached
	if (particleSystems.size() == 0) return;

	// update particles
	for (auto particleSystem: particleSystems) particleSystem->updateParticles();

	//
	if (parentEntity == nullptr && frustumCulling == true && engine != nullptr && enabled == true) engine->partition->updateEntity(this);
}

void ParticleSystemGroup::setFrustumCulling(bool frustumCulling) {
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
	if (engine != nullptr) engine->registerEntity(this);
}

void ParticleSystemGroup::setAutoEmit(bool autoEmit) {
	// delegate to sub particle systems
	for (auto particleSystem: particleSystems) particleSystem->setAutoEmit(autoEmit);
	this->autoEmit = autoEmit;
	// delegate change to engine
	if (engine != nullptr) engine->registerEntity(this);
}

void ParticleSystemGroup::dispose()
{
	for (auto particleSystem: particleSystems) dynamic_cast<Entity*>(particleSystem)->dispose();
}

void ParticleSystemGroup::setEngine(Engine* engine)
{
	if (this->engine != nullptr) this->engine->deregisterEntity(this);
	this->engine = engine;
	if (engine != nullptr) engine->registerEntity(this);
	for (auto particleSystem: particleSystems) dynamic_cast<Entity*>(particleSystem)->setEngine(engine);
}

void ParticleSystemGroup::setRenderer(Renderer* renderer)
{
	for (auto particleSystem: particleSystems) dynamic_cast<Entity*>(particleSystem)->setRenderer(renderer);
}

