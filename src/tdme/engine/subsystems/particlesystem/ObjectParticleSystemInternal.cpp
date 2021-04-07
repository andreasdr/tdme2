#include <tdme/engine/subsystems/particlesystem/ObjectParticleSystemInternal.h>

#include <algorithm>
#include <string>
#include <vector>

#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Color4Base.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/subsystems/particlesystem/Particle.h>
#include <tdme/engine/subsystems/particlesystem/ParticleEmitter.h>
#include <tdme/engine/subsystems/rendering/Object3DBase.h>
#include <tdme/engine/subsystems/rendering/Object3DInternal.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Entity.h>
#include <tdme/engine/Object3D.h>
#include <tdme/engine/Timing.h>
#include <tdme/engine/Transformations.h>
#include <tdme/math/Math.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector3.h>

using std::remove;
using std::string;
using std::to_string;
using std::vector;

using tdme::engine::model::Color4;
using tdme::engine::model::Color4Base;
using tdme::engine::model::Model;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::subsystems::particlesystem::ObjectParticleSystemInternal;
using tdme::engine::subsystems::particlesystem::Particle;
using tdme::engine::subsystems::particlesystem::ParticleEmitter;
using tdme::engine::subsystems::rendering::Object3DBase;
using tdme::engine::subsystems::rendering::Object3DInternal;
using tdme::engine::Engine;
using tdme::engine::Entity;
using tdme::engine::Object3D;
using tdme::engine::Timing;
using tdme::engine::Transformations;
using tdme::math::Math;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;

ObjectParticleSystemInternal::ObjectParticleSystemInternal(const string& id, Model* model, const Vector3& scale, bool autoEmit, bool contributesShadows, bool receivesShadows, int32_t maxCount, ParticleEmitter* emitter)
{
	this->id = id;
	this->enabled = true;
	this->model = model;
	this->objectScale = scale;
	this->autoEmit = autoEmit;
	this->contributesShadows = contributesShadows;
	this->receivesShadows = receivesShadows;
	particles.resize(maxCount);
	objects.resize(maxCount);
	for (auto i = 0; i < objects.size(); i++) {
		objects[i] = new Object3D(
			"tdme.opse." +
				id +
				":" +
				to_string(i),
			model
		);
		objects[i]->setEnabled(false);
		objects[i]->setScale(objectScale);
		objects[i]->setContributesShadows(contributesShadows);
		objects[i]->setReceivesShadows(receivesShadows);
		objects[i]->setPickable(false);
	}
	this->boundingBox = new BoundingBox();
	this->boundingBoxTransformed = new BoundingBox();
	this->emitter = emitter;
	this->effectColorMul.set(1.0f, 1.0f, 1.0f, 1.0f);
	this->effectColorAdd.set(0.0f, 0.0f, 0.0f, 0.0f);
	this->pickable = false;
	this->particlesToSpawnRemainder = 0.0f;
}

ObjectParticleSystemInternal::~ObjectParticleSystemInternal() {
	delete emitter;
	for (auto i = 0; i < objects.size(); i++) delete objects[i];
}

const string& ObjectParticleSystemInternal::getId()
{
	return id;
}

void ObjectParticleSystemInternal::setEngine(Engine* engine)
{
	this->engine = engine;
	for (auto i = 0; i < objects.size(); i++) {
		objects[i]->setEngine(engine);
	}
}

void ObjectParticleSystemInternal::setRenderer(Renderer* renderer)
{
	this->renderer = renderer;
	for (auto i = 0; i < objects.size(); i++) {
		objects[i]->setRenderer(renderer);
	}
}

void ObjectParticleSystemInternal::update()
{
	Transformations::update();
	updateInternal();
}

void ObjectParticleSystemInternal::fromTransformations(const Transformations& transformations)
{
	Transformations::fromTransformations(transformations);
	updateInternal();
}

int32_t ObjectParticleSystemInternal::emitParticles()
{
	// particles to spawn
	auto particlesToSpawn = 0;
	{
		auto particlesToSpawnWithFraction = emitter->getCount() * engine->getTiming()->getDeltaTime() / 1000.0f;
		if (autoEmit == true) {
			particlesToSpawn = static_cast< int32_t >(particlesToSpawnWithFraction);
			particlesToSpawnRemainder += particlesToSpawnWithFraction - particlesToSpawn;
			if (particlesToSpawnRemainder > 1.0f) {
				particlesToSpawn++;
				particlesToSpawnRemainder -= 1.0f;
			}
		} else {
			particlesToSpawn = emitter->getCount();
		}
	}
	// skip if nothing to spawn
	if (particlesToSpawn == 0) return 0;
	// spawn
	auto particlesSpawned = 0;
	for (auto i = 0; i < particles.size(); i++) {
		auto& particle = particles[i];
		if (particle.active == true) continue;
		// emit particle
		emitter->emit(&particle);
		// enable object
		auto object = objects[i];
		object->setTranslation(particle.position);
		object->update();
		object->setEnabled(true);
		object->setEffectColorAdd(effectColorAdd);
		object->setEffectColorMul(effectColorMul);
		enabledObjects.push_back(object);
		// all particles spawned?
		particlesSpawned++;
		if (particlesSpawned == particlesToSpawn) break;
	}
	// done
	return particlesSpawned;
}

void ObjectParticleSystemInternal::updateParticles()
{
	//
	Vector3 center;
	auto& localTransformationsMatrix = localTransformations.getTransformationsMatrix();
	localTransformationsMatrix.getTranslation(center);
	center.add(emitter->getCenter());
	// transformations
	auto& transformationsMatrix = getTransformationsMatrix();
	//
	Vector3 point;
	Vector3 velocityForTime;
	auto first = true;
	auto& bbMinXYZ = boundingBoxTransformed.getMin().getArray();
	auto& bbMaxXYZ = boundingBoxTransformed.getMax().getArray();
	auto timeDelta = engine->getTiming()->getDeltaTime();
	for (auto i = 0; i < particles.size(); i++) {
		auto& particle = particles[i];
		if (particle.active == false)
			continue;

		auto object = objects[i];
		// life time
		particle.lifeTimeCurrent += timeDelta;
		if (particle.lifeTimeCurrent >= particle.lifeTimeMax) {
			particle.active = false;
			object->setEnabled(false);
			enabledObjects.erase(remove(enabledObjects.begin(), enabledObjects.end(), object), enabledObjects.end());
			continue;
		}
		// add gravity if our particle have a noticable mass
		if (particle.mass > Math::EPSILON)
			particle.velocity.sub(Vector3(0.0f, 0.5f * Math::g * static_cast<float>(timeDelta) / 1000.0f, 0.0f));
		// TODO:
		//	maybe take air resistance into account like a huge paper needs more time to fall than a sphere of paper
		//	or heat for smoke or fire, whereas having no mass for those particles works around this problem for now
		// translation
		particle.position.add(velocityForTime.set(particle.velocity).scale(static_cast<float>(timeDelta) / 1000.0f));
		// update up effect colors
		object->setEffectColorAdd(effectColorAdd);
		object->setEffectColorMul(effectColorMul);
		// translation
		point = localTransformationsMatrix.multiply(particle.position);
		point.add(center);
		// transform particle according to its transformations
		point = transformationsMatrix.multiply(point);
		// apply to object
		object->setTranslation(point);
		object->update();
		if (first == true) {
			boundingBoxTransformed.getMin().set(object->getBoundingBoxTransformed()->getMin());
			boundingBoxTransformed.getMax().set(object->getBoundingBoxTransformed()->getMax());
			first = false;
		} else {
			auto& objBbMinXYZ = object->getBoundingBoxTransformed()->getMin().getArray();
			auto& objBbMaxXYZ = object->getBoundingBoxTransformed()->getMax().getArray();
			if (objBbMinXYZ[0] < bbMinXYZ[0]) bbMinXYZ[0] = objBbMinXYZ[0];
			if (objBbMinXYZ[1] < bbMinXYZ[1]) bbMinXYZ[1] = objBbMinXYZ[1];
			if (objBbMinXYZ[2] < bbMinXYZ[2]) bbMinXYZ[2] = objBbMinXYZ[2];
			if (objBbMaxXYZ[0] > bbMaxXYZ[0]) bbMaxXYZ[0] = objBbMaxXYZ[0];
			if (objBbMaxXYZ[1] > bbMaxXYZ[1]) bbMaxXYZ[1] = objBbMaxXYZ[1];
			if (objBbMaxXYZ[2] > bbMaxXYZ[2]) bbMaxXYZ[2] = objBbMaxXYZ[2];
		}
	}
	// compute bounding boxes
	boundingBoxTransformed.update();
	boundingBox.fromBoundingVolumeWithTransformations(&boundingBoxTransformed, inverseTransformation);
}

void ObjectParticleSystemInternal::dispose()
{
	for (auto i = 0; i < objects.size(); i++) {
		objects[i]->dispose();
	}
}

