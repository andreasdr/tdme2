#pragma once

#include <vector>
#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/Entity.h>
#include <tdme/engine/ParticleSystemEntity.h>
#include <tdme/engine/Transformations.h>
#include <tdme/engine/ObjectParticleSystem.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/utilities/fwd-tdme.h>
#include <tdme/math/Math.h>

using std::vector;
using std::string;

using tdme::engine::Entity;
using tdme::engine::Engine;
using tdme::engine::Transformations;
using tdme::engine::ObjectParticleSystem;
using tdme::engine::ParticleSystemEntity;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::math::Math;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;

/** 
 * Particle system group, which combines several particle systems into a group, to be used with engine class
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::ParticleSystemGroup final
	: public Transformations
	, public ParticleSystemEntity
{
private:
	Engine* engine { nullptr };
	Entity* parentEntity { nullptr };
	bool frustumCulling { true };

	string id;
	bool contributesShadows;
	bool receivesShadows;
	bool enabled;
	bool pickable;
	bool autoEmit;
	BoundingBox boundingBox;
	BoundingBox boundingBoxTransformed;
	Color4 effectColorMul;
	Color4 effectColorAdd;
	vector<ParticleSystemEntity*> particleSystems;
	Transformations localTransformations;

	// overridden methods
	inline void setParentEntity(Entity* entity) override {
		this->parentEntity = entity;
	}
	inline Entity* getParentEntity() override {
		return parentEntity;
	}
	inline void applyParentTransformations(const Transformations& parentTransformations) override {
		Transformations::applyParentTransformations(parentTransformations);
		for (auto particleSystem: particleSystems) particleSystem->applyParentTransformations(parentTransformations);
	}

public:
	// overriden methods
	void initialize() override;

	inline BoundingBox* getBoundingBox() override {
		if (particleSystems.empty() == false) {
			// compute new bounding box
			boundingBox.fromBoundingVolume(dynamic_cast<Entity*>(particleSystems[0])->getBoundingBox());
			for (auto i = 1; i < particleSystems.size(); i++) {
				boundingBox.extend(dynamic_cast<Entity*>(particleSystems[i])->getBoundingBox());
			}
			boundingBox.update();

			// update bounding box transformed
			boundingBoxTransformed.fromBoundingVolumeWithTransformations(&boundingBox, *this);
		}

		//
		return &boundingBox;
	}

	inline BoundingBox* getBoundingBoxTransformed() override {
		if (particleSystems.empty() == false) {
			// compute new bounding box
			boundingBox.fromBoundingVolume(dynamic_cast<Entity*>(particleSystems[0])->getBoundingBox());
			for (auto i = 1; i < particleSystems.size(); i++) {
				boundingBox.extend(dynamic_cast<Entity*>(particleSystems[i])->getBoundingBox());
			}
			boundingBox.update();

			// update bounding box transformed
			boundingBoxTransformed.fromBoundingVolumeWithTransformations(&boundingBox, *this);
		}

		//
		return &boundingBoxTransformed;
	}

	/** 
	 * @return particle systems
	 */
	inline const vector<ParticleSystemEntity*>& getParticleSystems() {
		return particleSystems;
	}

	// overriden methods
	void fromTransformations(const Transformations& transformations) override;
	void update() override;
	void setEnabled(bool enabled) override;
	inline bool isFrustumCulling() override {
		return frustumCulling;
	}
	void setFrustumCulling(bool frustumCulling) override;

	/**
	 * Public constructor
	 * @param id id
	 * @param autoEmit auto emit
	 * @param contributesShadows contributes shadows
	 * @param receivesShadows receives shadows
	 * @param particleSystems particle systems
	 */
	ParticleSystemGroup(const string& id, bool autoEmit, bool contributesShadows, bool receivesShadows, const vector<ParticleSystemEntity*>& particleSystems);

	/**
	 * Destructor
	 */
	~ParticleSystemGroup();

public:

	// overriden methods
	void dispose() override;
	void setEngine(Engine* engine) override;
	void setRenderer(Renderer* renderer) override;

	inline ParticleEmitter* getEmitter() override {
		return nullptr;
	}

	inline const Color4& getEffectColorAdd() const override {
		return effectColorAdd;
	}

	inline void setEffectColorAdd(const Color4& effectColorAdd) override {
		for (auto particleSystem: particleSystems) particleSystem->setEffectColorAdd(effectColorAdd);
		this->effectColorAdd = effectColorAdd;
	}

	inline const Color4& getEffectColorMul() const override {
		return effectColorMul;
	}

	inline void setEffectColorMul(const Color4& effectColorMul) override {
		for (auto particleSystem: particleSystems) particleSystem->setEffectColorMul(effectColorMul);
		this->effectColorMul = effectColorMul;
	}

	inline const string& getId() override {
		return id;
	}

	inline bool isContributesShadows() override {
		return contributesShadows;
	}

	inline void setContributesShadows(bool contributesShadows) override {
		this->contributesShadows = contributesShadows;
		for (auto particleSystem: particleSystems) {
			auto ops = dynamic_cast<ObjectParticleSystem*>(particleSystem);
			if (ops != nullptr) ops->setContributesShadows(contributesShadows);
		}
	}

	inline bool isReceivesShadows() override {
		return receivesShadows;
	}

	inline void setReceivesShadows(bool receivesShadows) override {
		this->receivesShadows = receivesShadows;
		for (auto particleSystem: particleSystems) {
			auto ops = dynamic_cast<ObjectParticleSystem*>(particleSystem);
			if (ops != nullptr) ops->setReceivesShadows(receivesShadows);
		}
	}

	inline bool isEnabled() override {
		return enabled;
	}

	inline bool isPickable() override {
		return pickable;
	}

	inline void setPickable(bool pickable) override {
		this->pickable = true;
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
		for (auto particleSystem: particleSystems) {
			if (particleSystem->isActive() == true) return true;
		}
		return false;
	}

	inline bool isAutoEmit() override {
		return autoEmit;
	}

	inline int32_t emitParticles() override {
		auto emittedParticles = 0;
		for (auto particleSystem: particleSystems) emittedParticles+= particleSystem->emitParticles();
		return emittedParticles;
	}

	void setAutoEmit(bool autoEmit) override;
	void updateParticles() override;

	inline const Transformations& getLocalTransformations() override {
		return localTransformations;
	}

	inline void setLocalTransformations(const Transformations& transformations) override {
		localTransformations = transformations;
		for (auto particleSystem: particleSystems) particleSystem->setLocalTransformations(localTransformations);
	}

};
