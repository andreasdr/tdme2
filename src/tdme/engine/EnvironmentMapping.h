#pragma once

#include <memory>
#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/subsystems/environmentmapping/EnvironmentMappingRenderer.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/engine/Color4.h>
#include <tdme/engine/Entity.h>
#include <tdme/engine/Transform.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Quaternion.h>
#include <tdme/math/Vector3.h>

using std::make_unique;
using std::string;
using std::unique_ptr;

using tdme::engine::model::Model;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::subsystems::environmentmapping::EnvironmentMappingRenderer;
using tdme::engine::subsystems::renderer::RendererBackend;
using tdme::engine::Color4;
using tdme::engine::Engine;
using tdme::engine::Entity;
using tdme::engine::Transform;
using tdme::math::Matrix4x4;
using tdme::math::Quaternion;
using tdme::math::Vector3;

/**
 * Environment mapping entity
 * @author Andreas Drewke
 */
class tdme::engine::EnvironmentMapping final: public Transform, public Entity
{

private:
	friend class Engine;

	Engine* engine { nullptr };
	bool frustumCulling { true };
	RenderPass renderPass { RENDERPASS_NONE };
	unique_ptr<EnvironmentMappingRenderer> environmentMappingRenderer;
	bool enabled { true };
	BoundingBox boundingBox;
	BoundingBox worldBoundingBox;
	Color4 effectColorMul { 1.0f, 1.0f, 1.0f, 1.0f };
	Color4 effectColorAdd { 0.0f, 0.0f, 0.0f, 0.0f };
	string id;
	int width;
	int height;
	int32_t renderPassMask { Entity::RENDERPASS_ALL - Entity::RENDERPASS_WATER };
	int64_t timeRenderUpdateFrequency { 100LL };

	Transform parentTransform;
	Matrix4x4 entityTransformMatrix;

	// overridden methods
	inline void setParentTransform(const Transform& parentTransform) override {
		this->parentTransform = parentTransform;
		auto entityTransform = parentTransform * (*this);
		entityTransformMatrix = entityTransform.getTransformMatrix();
	}

	/**
	 * Render environment map
	 */
	inline void render() {
		Vector3 translation;
		getTransformMatrix().getTranslation(translation);
		environmentMappingRenderer->render(translation);
	}

public:
	// forbid class copy
	FORBID_CLASS_COPY(EnvironmentMapping)

	/**
	 * Public constructor
	 * @param id id
	 * @param width width
	 * @param height height
	 * @param boundingBox bounding box
	 */
	EnvironmentMapping(const string& id, int width, int height, BoundingBox boundingBox);

	// overridden methods
	inline EntityType getEntityType() override {
		return ENTITYTYPE_ENVIRONMENTMAPPING;
	}

	void setEngine(Engine* engine) override;

	inline void setRenderer(RendererBackend* rendererBackend) override {
	}

	inline void initialize() override;
	inline void dispose() override;

	inline bool isEnabled() override {
		return enabled;
	}

	void setEnabled(bool enabled) override;
	inline bool isFrustumCulling() override {
		return frustumCulling;
	}
	void setFrustumCulling(bool frustumCulling) override;
	void update() override;

	inline BoundingBox* getBoundingBox() override {
		return &boundingBox;
	}

	inline BoundingBox* getWorldBoundingBox() override {
		return &worldBoundingBox;
	}

	inline const Color4& getEffectColorAdd() const override {
		return effectColorAdd;
	}

	inline void setEffectColorAdd(const Color4& effectColorAdd) override {
	}

	inline const Color4& getEffectColorMul() const override {
		return effectColorMul;
	}

	inline void setEffectColorMul(const Color4& effectColorMul) override {
	}

	inline const string& getId() override {
		return id;
	}

	inline bool isContributesShadows() override {
		return false;
	}

	inline void setContributesShadows(bool contributesShadows) override {
	}

	inline bool isReceivesShadows() override {
		return false;
	}

	inline void setReceivesShadows(bool receivesShadows) override {
	}

	inline bool isPickable() override {
		return false;
	}

	inline void setPickable(bool pickable) override {
	}

	inline const Vector3& getTranslation() const override {
		return Transform::getTranslation();
	}

	inline void setTranslation(const Vector3& translation) override {
		Transform::setTranslation(translation);
	}

	inline const Vector3& getScale() const override {
		return Transform::getScale();
	}

	inline void setScale(const Vector3& scale) override {
		Transform::setScale(scale);
	}

	inline const int getRotationCount() const override {
		return Transform::getRotationCount();
	}

	inline Rotation& getRotation(const int idx) override {
		return Transform::getRotation(idx);
	}

	inline void addRotation(const Vector3& axis, const float angle) override {
		Transform::addRotation(axis, angle);
	}

	inline void removeRotation(const int idx) override {
		Transform::removeRotation(idx);
	}

	inline const Vector3& getRotationAxis(const int idx) const override {
		return Transform::getRotationAxis(idx);
	}

	inline void setRotationAxis(const int idx, const Vector3& axis) override {
		Transform::setRotationAxis(idx, axis);
	}

	inline const float getRotationAngle(const int idx) const override {
		return Transform::getRotationAngle(idx);
	}

	inline void setRotationAngle(const int idx, const float angle) override {
		Transform::setRotationAngle(idx, angle);
	}

	inline const Quaternion& getRotationsQuaternion() const override {
		return Transform::getRotationsQuaternion();
	}

	inline const Matrix4x4& getTransformMatrix() const override {
		return entityTransformMatrix;
	}

	inline const Transform& getParentTransform() const override {
		return parentTransform;
	}

	inline const Transform& getTransform() const override {
		return *this;
	}

	void setTransform(const Transform& transform) override;

	inline RenderPass getRenderPass() const override {
		return renderPass;
	}

	inline void setRenderPass(RenderPass renderPass) override {
	}

	/**
	 * @returns width
	 */
	inline int32_t getWidth() {
		return width;
	}

	/**
	 * @returns height
	 */
	inline int32_t getHeight() {
		return height;
	}

	/**
	 * @returns render pass mask
	 */
	inline int32_t getRenderPassMask() {
		return renderPassMask;
	}

	/**
	 * Set up render pass mask
	 * @param renderPassMask render pass mask
	 */
	inline void setRenderPassMask(int32_t renderPassMask) {
		this->renderPassMask = renderPassMask;
		if (environmentMappingRenderer != nullptr) environmentMappingRenderer->setRenderPassMask(renderPassMask);
	}

	/**
	 * @returns render update time frequency in milliseconds
	 */
	inline int64_t getTimeRenderUpdateFrequency() {
		return timeRenderUpdateFrequency;
	}

	/**
	 * Set up render update time frequency
	 * @param frequency frequency in milliseconds
	 */
	inline void setTimeRenderUpdateFrequency(int64_t frequency) {
		this->timeRenderUpdateFrequency = frequency;
		if (environmentMappingRenderer != nullptr) environmentMappingRenderer->setTimeRenderUpdateFrequency(frequency);
	}

	/**
	 * @returns cube map texture id
	 */
	inline int32_t getCubeMapTextureId() {
		if (environmentMappingRenderer == nullptr) return 0;
		return environmentMappingRenderer->getCubeMapTextureId();
	}

};
