#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/Transformations.h>
#include <tdme/engine/Entity.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/engine/subsystems/environmentmapping/EnvironmentMapping.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Quaternion.h>

using std::string;

using tdme::engine::Entity;
using tdme::engine::Engine;
using tdme::engine::Transformations;
using tdme::engine::model::Color4;
using tdme::engine::model::Model;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::subsystems::renderer::Renderer;
using EnvironmentMappingImplementation = tdme::engine::subsystems::environmentmapping::EnvironmentMapping;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;
using tdme::math::Quaternion;

/**
 * Environment mapping entity
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::EnvironmentMapping final: public Transformations, public Entity
{

private:
	friend class Engine;

	Engine* engine { nullptr };
	Entity* parentEntity { nullptr };
	bool frustumCulling { true };
	RenderPass renderPass { RENDERPASS_NONE };
	EnvironmentMappingImplementation* environmentMappingImplementation { nullptr };
	bool enabled { true };
	BoundingBox boundingBox;
	BoundingBox boundingBoxTransformed;
	Color4 effectColorMul { 1.0f, 1.0f, 1.0f, 1.0f };
	Color4 effectColorAdd { 0.0f, 0.0f, 0.0f, 0.0f };
	string id;
	int width;
	int height;
	int32_t renderPassMask { Entity::RENDERPASS_ALL - Entity::RENDERPASS_WATER };
	int64_t timeRenderUpdateFrequency { 100LL };

	// overridden methods
	inline void setParentEntity(Entity* entity) override {
		this->parentEntity = entity;
	}
	inline Entity* getParentEntity() override {
		return parentEntity;
	}
	inline void applyParentTransformations(const Transformations& parentTransformations) override {
		Transformations::applyParentTransformations(parentTransformations);
	}

	/**
	 * Render environment map
	 */
	inline void render() {
		Vector3 translation;
		getTransformationsMatrix().getTranslation(translation);
		environmentMappingImplementation->render(translation);
	}

public:
	/**
	 * Public constructor
	 * @param id id
	 * @param width width
	 * @param height height
	 * @param boundingBox bounding box
	 */
	EnvironmentMapping(const string& id, int width, int height, BoundingBox boundingBox);

	// overridden methods
	void setEngine(Engine* engine) override;

	inline void setRenderer(Renderer* renderer) override {
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

	void fromTransformations(const Transformations& transformations) override;
	void update() override;

	inline BoundingBox* getBoundingBox() override {
		return &boundingBox;
	}

	inline BoundingBox* getBoundingBoxTransformed() override {
		return &boundingBoxTransformed;
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
	}

	/**
	 * @return width
	 */
	inline int32_t getWidth() {
		return width;
	}

	/**
	 * @return height
	 */
	inline int32_t getHeight() {
		return height;
	}

	/**
	 * @return render pass mask
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
		if (environmentMappingImplementation != nullptr) environmentMappingImplementation->setRenderPassMask(renderPassMask);
	}

	/**
	 * @return render update time frequency in milliseconds
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
		if (environmentMappingImplementation != nullptr) environmentMappingImplementation->setTimeRenderUpdateFrequency(frequency);
	}

	/**
	 * Get frame buffer of given index
	 * @param idx index
	 * @return frame buffer of given index
	 */
	inline FrameBuffer* getFrameBuffer(int idx) {
		if (environmentMappingImplementation == nullptr) return nullptr;
		return environmentMappingImplementation->getFrameBuffer(idx);
	}

	/**
	 * @return cube map texture id
	 */
	inline int32_t getCubeMapTextureId() {
		if (environmentMappingImplementation == nullptr) return 0;
		return environmentMappingImplementation->getCubeMapTextureId();
	}

};
