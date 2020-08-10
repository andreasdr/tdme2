#include <tdme/engine/Object3D.h>

#include <string>

#include <tdme/engine/Engine.h>
#include <tdme/engine/Partition.h>
#include <tdme/engine/Transformations.h>
#include <tdme/engine/model/ShaderModel.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Quaternion.h>
#include <tdme/utils/StringUtils.h>

using std::string;

using tdme::engine::Object3D;
using tdme::engine::Engine;
using tdme::engine::Partition;
using tdme::engine::Transformations;
using tdme::engine::model::ShaderModel;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;
using tdme::math::Quaternion;
using tdme::utils::StringUtils;

Object3D::Object3D(const string& id, Model* model, int instances): Object3DInternal(id, model, instances) {
	if (model->getShaderModel() == ShaderModel::PBR) {
		shaderId = StringUtils::startsWith(shaderId, "pbr-") == true || shaderId.empty() == true?shaderId:"pbr-" + shaderId;
		distanceShaderId = StringUtils::startsWith(distanceShaderId, "pbr-") == true || distanceShaderId.empty() == true?distanceShaderId:"pbr-" + distanceShaderId;
	}
}

Object3D::Object3D(const string& id, Model* model): Object3DInternal(id, model, 1)
{
}

void Object3D::setEngine(Engine* engine)
{
	if (this->engine != nullptr) this->engine->deregisterEntity(this);
	this->engine = engine;
	if (engine != nullptr) engine->registerEntity(this);
}

void Object3D::setRenderer(Renderer* renderer)
{
}

void Object3D::fromTransformations(const Transformations& transformations)
{
	Object3DInternal::fromTransformations(transformations);
	if (parentEntity == nullptr && frustumCulling == true && engine != nullptr && enabled == true) engine->partition->updateEntity(this);
}

void Object3D::update()
{
	Object3DInternal::update();
	if (parentEntity == nullptr && frustumCulling == true && engine != nullptr && enabled == true) engine->partition->updateEntity(this);
}

void Object3D::setEnabled(bool enabled)
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
	Object3DInternal::setEnabled(enabled);
}

bool Object3D::isFrustumCulling() {
	return frustumCulling;
}

void Object3D::setFrustumCulling(bool frustumCulling) {
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

void Object3D::dispose()
{
	Object3DInternal::dispose();
}

void Object3D::initialize()
{
	Object3DInternal::initialize();
}

