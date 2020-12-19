#include <tdme/engine/EnvironmentMapping.h>

#include <string>

#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/subsystems/environmentmapping/EnvironmentMapping.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Partition.h>
#include <tdme/engine/Transformations.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Quaternion.h>
#include <tdme/math/Vector3.h>

using std::string;

using tdme::engine::primitives::BoundingBox;
using tdme::engine::Engine;
using tdme::engine::EnvironmentMapping;
using tdme::engine::Partition;
using tdme::engine::Transformations;
using tdme::math::Matrix4x4;
using tdme::math::Quaternion;
using tdme::math::Vector3;
using EnvironmentMappingImplementation = tdme::engine::subsystems::environmentmapping::EnvironmentMapping;

EnvironmentMapping::EnvironmentMapping(const string& id, int width, int height, BoundingBox boundingBox)
{
	this->id = id;
	this->width = width;
	this->height = height;
	this->boundingBox = boundingBox;
}

void EnvironmentMapping::setEngine(Engine* engine) {
	if (this->engine != nullptr) this->engine->deregisterEntity(this);
	this->engine = engine;
	if (engine != nullptr) engine->registerEntity(this);
}

void EnvironmentMapping::initialize() {
	environmentMappingImplementation = new EnvironmentMappingImplementation(engine, width, height);
	environmentMappingImplementation->setRenderPassMask(renderPassMask);
	environmentMappingImplementation->setTimeRenderUpdateFrequency(timeRenderUpdateFrequency);
	environmentMappingImplementation->initialize();

}
void EnvironmentMapping::dispose() {
	environmentMappingImplementation->dispose();
}

void EnvironmentMapping::fromTransformations(const Transformations& transformations)
{
	Transformations::fromTransformations(transformations);
	Transformations translationTransformations;
	translationTransformations.setTranslation(getTranslation());
	translationTransformations.update();
	boundingBoxTransformed.fromBoundingVolumeWithTransformations(&boundingBox, translationTransformations);
	if (parentEntity == nullptr && frustumCulling == true && engine != nullptr && enabled == true) engine->partition->updateEntity(this);
}

void EnvironmentMapping::update()
{
	Transformations::update();
	Transformations translationTransformations;
	translationTransformations.setTranslation(getTranslation());
	translationTransformations.update();
	boundingBoxTransformed.fromBoundingVolumeWithTransformations(&boundingBox, translationTransformations);
	if (parentEntity == nullptr && frustumCulling == true && engine != nullptr && enabled == true) engine->partition->updateEntity(this);
}

void EnvironmentMapping::setEnabled(bool enabled)
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
}

void EnvironmentMapping::setFrustumCulling(bool frustumCulling) {
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

