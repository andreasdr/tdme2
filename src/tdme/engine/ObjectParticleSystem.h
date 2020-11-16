#pragma once

#include <vector>
#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/ParticleSystemEntity.h>
#include <tdme/engine/Transformations.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/primitives/fwd-tdme.h>
#include <tdme/engine/subsystems/particlesystem/fwd-tdme.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/utilities/fwd-tdme.h>
#include <tdme/engine/subsystems/particlesystem/ObjectParticleSystemInternal.h>
#include <tdme/engine/Entity.h>

using std::vector;
using std::string;

using tdme::engine::subsystems::particlesystem::ObjectParticleSystemInternal;
using tdme::engine::Engine;
using tdme::engine::ParticleSystemEntity;
using tdme::engine::Transformations;
using tdme::engine::model::Color4;
using tdme::engine::model::Model;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::subsystems::particlesystem::ParticleEmitter;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;

/**
 * Object particle system entity to be used with engine class
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::ObjectParticleSystem final
	: public ObjectParticleSystemInternal
	, public ParticleSystemEntity
{
	friend class tdme::engine::ParticleSystemGroup;

private:
	bool frustumCulling { true };
	Entity* parentEntity { nullptr };

	// overridden methods
	inline void setParentEntity(Entity* entity) override {
		this->parentEntity = entity;
	}
	inline Entity* getParentEntity() override {
		return parentEntity;
	}
	inline void applyParentTransformations(const Transformations& parentTransformations) override {
		Transformations::applyParentTransformations(parentTransformations);
		// TODO: a.drewke, bounding box and emitter needs a update here
	}

public:
	/**
	 * Public constructor
	 * @param id id
	 * @param model model
	 * @param scale scale
	 * @param autoEmit auto emit
	 * @param contributesShadows enable contributes shadows
	 * @param receivesShadows enable receives shadows
	 * @param maxCount max count
	 * @param emitter emitter
	 */
	ObjectParticleSystem(const string& id, Model* model, const Vector3& scale, bool autoEmit, bool contributesShadows, bool receivesShadows, int32_t maxCount, ParticleEmitter* emitter);

	/**
	 * @return enabled objects
	 */
	inline const vector<Object3D*>& getEnabledObjects() {
		return enabledObjects;
	}

	void setEngine(Engine* engine) override;
	void setRenderer(Renderer* renderer) override;
	void initialize() override;
	void dispose() override;

	inline bool isEnabled() override {
		return ObjectParticleSystemInternal::isEnabled();
	}

	void setEnabled(bool enabled) override;
	bool isFrustumCulling() override;
	void setFrustumCulling(bool frustumCulling) override;
	void fromTransformations(const Transformations& transformations) override;
	void update() override;

	inline BoundingBox* getBoundingBox() override {
		return &boundingBox;
	}

	inline BoundingBox* getBoundingBoxTransformed() override {
		return &boundingBoxTransformed;
	}

	inline ParticleEmitter* getEmitter() override {
		return ObjectParticleSystemInternal::getEmitter();
	}

	inline const Color4& getEffectColorAdd() const override {
		return ObjectParticleSystemInternal::getEffectColorAdd();
	}

	inline void setEffectColorAdd(const Color4& effectColorAdd) override {
		ObjectParticleSystemInternal::setEffectColorAdd(effectColorAdd);
	}

	inline const Color4& getEffectColorMul() const override {
		return ObjectParticleSystemInternal::getEffectColorMul();
	}

	inline void setEffectColorMul(const Color4& effectColorMul) override {
		ObjectParticleSystemInternal::setEffectColorMul(effectColorMul);
	}

	inline const string& getId() override {
		return ObjectParticleSystemInternal::getId();
	}

	inline bool isContributesShadows() override {
		return ObjectParticleSystemInternal::isContributesShadows();
	}

	inline void setContributesShadows(bool contributesShadows) override {
		ObjectParticleSystemInternal::setContributesShadows(contributesShadows);
	}

	inline bool isReceivesShadows() override {
		return ObjectParticleSystemInternal::isReceivesShadows();
	}

	inline void setReceivesShadows(bool receivesShadows) override {
		ObjectParticleSystemInternal::setReceivesShadows(receivesShadows);
	}

	inline bool isPickable() override {
		return ObjectParticleSystemInternal::isPickable();
	}

	inline void setPickable(bool pickable) override {
		ObjectParticleSystemInternal::setPickable(pickable);
	}

	inline const Vector3& getTranslation() const override {
		return Transformations::getTranslation();
	}

	inline void setTranslation(const Vector3& translation) override {
		Transformations::setTranslation(translation);
	}

	inline const Vector3& getScale() const override {
		return Transformations::getScale();
	}

	inline void setScale(const Vector3& scale) override {
		Transformations::setScale(scale);
	}

	inline const Vector3& getPivot() const override {
		return Transformations::getPivot();
	}

	inline void setPivot(const Vector3& pivot) override {
		Transformations::setPivot(pivot);
	}

	inline const int getRotationCount() const override {
		return Transformations::getRotationCount();
	}

	inline Rotation& getRotation(const int idx) override {
		return Transformations::getRotation(idx);
	}

	inline void addRotation(const Vector3& axis, const float angle) override {
		Transformations::addRotation(axis, angle);
	}

	inline void removeRotation(const int idx) override {
		Transformations::removeRotation(idx);
	}

	inline const Vector3& getRotationAxis(const int idx) const override {
		return Transformations::getRotationAxis(idx);
	}

	inline void setRotationAxis(const int idx, const Vector3& axis) override {
		Transformations::setRotationAxis(idx, axis);
	}

	inline const float getRotationAngle(const int idx) const override {
		return Transformations::getRotationAngle(idx);
	}

	inline void setRotationAngle(const int idx, const float angle) override {
		Transformations::setRotationAngle(idx, angle);
	}

	inline const Quaternion& getRotationsQuaternion() const override {
		return Transformations::getRotationsQuaternion();
	}

	inline const Matrix4x4& getTransformationsMatrix() const override {
		return Transformations::getTransformationsMatrix();
	}

	inline const Transformations& getTransformations() const override {
		return *this;
	}

	inline bool isActive() override {
		return ObjectParticleSystemInternal::isActive();
	}

	inline bool isAutoEmit() override {
		return ObjectParticleSystemInternal::isAutoEmit();
	}

	inline int32_t emitParticles() override {
		return ObjectParticleSystemInternal::emitParticles();
	}

	void setAutoEmit(bool autoEmit) override;
	void updateParticles() override;

	inline const Transformations& getLocalTransformations() override {
		return ObjectParticleSystemInternal::getLocalTransformations();
	}
	inline void setLocalTransformations(const Transformations& transformations) override {
		ObjectParticleSystemInternal::setLocalTransformations(transformations);
	}

};
