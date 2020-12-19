#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/fileio/textures/fwd-tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/primitives/fwd-tdme.h>
#include <tdme/engine/subsystems/particlesystem/fwd-tdme.h>
#include <tdme/engine/subsystems/particlesystem/PointsParticleSystemInternal.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/engine/Entity.h>
#include <tdme/engine/ParticleSystemEntity.h>
#include <tdme/engine/Transformations.h>
#include <tdme/math/fwd-tdme.h>

using std::string;

using tdme::engine::fileio::textures::Texture;
using tdme::engine::model::Color4;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::subsystems::particlesystem::ParticleEmitter;
using tdme::engine::subsystems::particlesystem::PointsParticleSystemInternal;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::Engine;
using tdme::engine::ParticleSystemEntity;
using tdme::engine::Transformations;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;

/**
 * Point particle system entity to be used with engine class
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::PointsParticleSystem final
	: public PointsParticleSystemInternal
	, public ParticleSystemEntity
{
	friend class tdme::engine::ParticleSystemGroup;

private:
	bool frustumCulling { true };
	Entity* parentEntity { nullptr };
	RenderPass renderPass { RENDERPASS_STANDARD };

	// overridden methods
	inline void setParentEntity(Entity* entity) override {
		this->parentEntity = entity;
	}
	inline Entity* getParentEntity() override {
		return parentEntity;
	}
	inline void applyParentTransformations(const Transformations& parentTransformations) override {
		Transformations::applyParentTransformations(parentTransformations);
		updateInternal();
	}

public:
	/**
	 * Public constructor
	 * @param id id
	 * @param emitter emitter
	 * @param maxPoints max points
	 * @param pointSize point size
	 * @param autoEmit auto emit
	 * @param texture texture
	 * @param textureHorizonalSprites texture horizonal sprites
	 * @param textureVerticalSprites texture vertical sprites
	 * @param fps frames per seconds
	 */
	PointsParticleSystem(const string& id, ParticleEmitter* emitter, int32_t maxPoints, float pointSize, bool autoEmit, Texture* texture = nullptr, int32_t textureHorizontalSprites = 1, int32_t textureVerticalSprites = 1, float fps = 10.0f);

	// overridden methods
	void setEngine(Engine* engine) override;
	void setRenderer(Renderer* renderer) override;
	void initialize() override;
	void dispose() override;

	inline bool isEnabled() override {
		return PointsParticleSystemInternal::isEnabled();
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
		return PointsParticleSystemInternal::getEmitter();
	}

	inline const Color4& getEffectColorAdd() const override {
		return PointsParticleSystemInternal::getEffectColorAdd();
	}

	inline void setEffectColorAdd(const Color4& effectColorAdd) override {
		PointsParticleSystemInternal::setEffectColorAdd(effectColorAdd);
	}

	inline const Color4& getEffectColorMul() const override {
		return PointsParticleSystemInternal::getEffectColorMul();
	}

	inline void setEffectColorMul(const Color4& effectColorMul) override {
		PointsParticleSystemInternal::setEffectColorMul(effectColorMul);
	}

	inline const string& getId() override {
		return PointsParticleSystemInternal::getId();
	}

	inline bool isPickable() override {
		return PointsParticleSystemInternal::isPickable();
	}

	inline void setPickable(bool pickable) override {
		PointsParticleSystemInternal::setPickable(pickable);
	}

	inline bool isContributesShadows() override {
		return PointsParticleSystemInternal::isContributesShadows();
	}

	inline void setContributesShadows(bool contributesShadows) override {
		PointsParticleSystemInternal::setContributesShadows(contributesShadows);
	}

	inline bool isReceivesShadows() override {
		return PointsParticleSystemInternal::isReceivesShadows();
	}

	inline void setReceivesShadows(bool receivesShadows) override {
		PointsParticleSystemInternal::setReceivesShadows(receivesShadows);
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

	inline RenderPass getRenderPass() const override {
		return renderPass;
	}

	inline void setRenderPass(RenderPass renderPass) override {
		this->renderPass = renderPass;
	}

	inline bool isActive() override {
		return PointsParticleSystemInternal::isActive();
	}

	inline bool isAutoEmit() override {
		return PointsParticleSystemInternal::isAutoEmit();
	}

	inline int32_t emitParticles() override {
		return PointsParticleSystemInternal::emitParticles();
	}

	void setAutoEmit(bool autoEmit) override;
	void updateParticles() override;

	inline const Transformations& getLocalTransformations() override {
		return PointsParticleSystemInternal::getLocalTransformations();
	}
	inline void setLocalTransformations(const Transformations& transformations) override {
		PointsParticleSystemInternal::setLocalTransformations(transformations);
	}

};
