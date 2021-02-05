#pragma once

#include <map>
#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/primitives/fwd-tdme.h>
#include <tdme/engine/subsystems/particlesystem/fwd-tdme.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/engine/subsystems/rendering/Object3DAnimation.h>
#include <tdme/engine/subsystems/rendering/Object3DInternal.h>
#include <tdme/engine/subsystems/rendering/Object3DNode.h>
#include <tdme/engine/subsystems/rendering/Object3DNodeRenderer.h>
#include <tdme/engine/subsystems/shadowmapping/fwd-tdme.h>
#include <tdme/engine/Camera.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Entity.h>
#include <tdme/engine/EntityShaderParameters.h>
#include <tdme/engine/Timing.h>
#include <tdme/engine/Transformations.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Quaternion.h>
#include <tdme/math/Vector3.h>

using std::map;
using std::string;

using tdme::engine::model::Color4;
using tdme::engine::model::Model;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::subsystems::rendering::EntityRenderer;
using tdme::engine::subsystems::rendering::Object3DAnimation;
using tdme::engine::subsystems::rendering::Object3DInternal;
using tdme::engine::subsystems::rendering::Object3DNode;
using tdme::engine::subsystems::rendering::Object3DNodeRenderer;
using tdme::engine::subsystems::shadowmapping::ShadowMap;
using tdme::engine::Camera;
using tdme::engine::Engine;
using tdme::engine::Entity;
using tdme::engine::EntityShaderParameters;
using tdme::engine::Timing;
using tdme::engine::Transformations;
using tdme::math::Matrix4x4;
using tdme::math::Quaternion;
using tdme::math::Vector3;

/**
 * Object 3D to be used with engine class
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::Object3D final
	: public Object3DInternal
	, public Entity
{
private:
	friend class Engine;
	friend class LODObject3D;
	friend class Object3DRenderGroup;
	friend class ObjectParticleSystem;
	friend class SkinnedObject3DRenderGroup;
	friend class tdme::engine::subsystems::rendering::EntityRenderer;
	friend class tdme::engine::subsystems::shadowmapping::ShadowMap;

	Engine* engine { nullptr };
	Entity* parentEntity { nullptr };
	bool frustumCulling { true };
	RenderPass renderPass { RENDERPASS_STANDARD };
	string shaderId { "default" };
	string distanceShaderId { "" };
	float distanceShaderDistance { 50.0f };
	string reflectionEnvironmentMappingId;
	Engine::EffectPass excludeFromEffectPass { Engine::EFFECTPASS_NONE };
	bool enableEarlyZRejection { false };
	bool disableDepthTest { false };
	int64_t frameTransformationsLast { -1LL };
	int64_t timeTransformationsLast { -1LL };
	EntityShaderParameters shaderParameters;
	EntityShaderParameters distanceShaderParameters;

	/**
	 * Compute animations
	 * @param context context
	 */
	inline void computeTransformations(void* context) {
		if (getModel()->hasSkinning() == true || getModel()->hasAnimations() == true) {
			auto timing = engine->getTiming();
			auto currentFrameAtTime = timing->getCurrentFrameAtTime();
			auto currentFrame = timing->getFrame();
			auto distanceFromCamera = (engine->getCamera()->getLookFrom() - getBoundingBoxTransformed()->computeClosestPointInBoundingBox(engine->getCamera()->getLookFrom())).computeLengthSquared();
			if (distanceFromCamera > Math::square(Engine::getTransformationsComputingReduction2Distance())) {
				if (frameTransformationsLast != -1LL && currentFrame - frameTransformationsLast < 4) return;
			} else
			if (distanceFromCamera > Math::square(Math::square(Engine::getTransformationsComputingReduction1Distance()))) {
				if (frameTransformationsLast != -1LL && currentFrame - frameTransformationsLast < 2) return;
			}
			computeTransformations(context, timeTransformationsLast, currentFrameAtTime);
			frameTransformationsLast = timing->getFrame();
			timeTransformationsLast = currentFrameAtTime;
		}
	}

	/**
	 * Compute transformations
	 * @param context context
	 * @param lastFrameAtTime time of last animation computation
	 * @param currentFrameAtTime time of current animation computation
	 */
	inline void computeTransformations(void* context, int64_t lastFrameAtTime, int64_t currentFrameAtTime) override {
		Object3DInternal::computeTransformations(context, lastFrameAtTime, currentFrameAtTime);
	}

	/**
	 * Pre render step like uploading VBOs and such
	 * @param context context
	 */
	inline void preRender(void* context) {
		for (auto object3DNode: object3dNodes) {
			if (object3DNode->renderer->needsPreRender() == true) {
				object3DNode->renderer->preRender(context);
			}
		}
	}

	// overridden methods
	inline void setParentEntity(Entity* entity) override {
		this->parentEntity = entity;
	}
	inline Entity* getParentEntity() override {
		return parentEntity;
	}
	inline void applyParentTransformations(const Transformations& parentTransformations) override {
		for (auto& transformations: instanceTransformations) transformations.applyParentTransformations(parentTransformations);
		updateBoundingBox();
	}

public:
	/**
	 * Public constructor
	 * @param id id
	 * @param model model
	 * @param instances instances to compute and render by duplication, which only is intended to be used with skinned meshes
	 */
	Object3D(const string& id, Model* model, int instances);

	/**
	 * Public constructor
	 * @param id id
	 * @param model model
	 */
	Object3D(const string& id, Model* model);

	// overridden methods
	inline EntityType getEntityType() override {
		return ENTITY_OBJECT3D;
	}

	void setEngine(Engine* engine) override;
	void setRenderer(Renderer* renderer) override;
	void initialize() override;
	void dispose() override;

	inline bool isEnabled() override {
		return Object3DInternal::isEnabled();
	}

	void setEnabled(bool enabled) override;
	bool isFrustumCulling() override;
	void setFrustumCulling(bool frustumCulling) override;
	void fromTransformations(const Transformations& transformations) override;
	void update() override;

	inline BoundingBox* getBoundingBox() override {
		return Object3DInternal::getBoundingBox();
	}

	inline BoundingBox* getBoundingBoxTransformed() override {
		return Object3DInternal::getBoundingBoxTransformed();
	}

	inline const Color4& getEffectColorAdd() const override {
		return Object3DInternal::getEffectColorAdd();
	}

	inline void setEffectColorAdd(const Color4& effectColorAdd) override {
		return Object3DInternal::setEffectColorAdd(effectColorAdd);
	}

	inline const Color4& getEffectColorMul() const override {
		return Object3DInternal::getEffectColorMul();
	}

	inline void setEffectColorMul(const Color4& effectColorMul) override {
		return Object3DInternal::setEffectColorMul(effectColorMul);
	}

	inline const string& getId() override {
		return Object3DInternal::getId();
	}

	inline bool isContributesShadows() override {
		return Object3DInternal::isContributesShadows();
	}

	inline void setContributesShadows(bool contributesShadows) override {
		Object3DInternal::setContributesShadows(contributesShadows);
	}

	inline bool isReceivesShadows() override {
		return Object3DInternal::isReceivesShadows();
	}

	inline void setReceivesShadows(bool receivesShadows) override {
		Object3DInternal::setReceivesShadows(receivesShadows);
	}

	inline bool isPickable() override {
		return Object3DInternal::isPickable();
	}

	inline void setPickable(bool pickable) override {
		Object3DInternal::setPickable(pickable);
	}

	inline const Vector3& getTranslation() const override {
		return instanceTransformations[currentInstance].getTranslation();
	}

	inline void setTranslation(const Vector3& translation) override {
		instanceTransformations[currentInstance].setTranslation(translation);
	}

	inline const Vector3& getScale() const override {
		return instanceTransformations[currentInstance].getScale();
	}

	inline void setScale(const Vector3& scale) override {
		instanceTransformations[currentInstance].setScale(scale);
	}

	inline const Vector3& getPivot() const override {
		return instanceTransformations[currentInstance].getPivot();
	}

	inline void setPivot(const Vector3& pivot) override {
		instanceTransformations[currentInstance].setPivot(pivot);
	}

	inline const int getRotationCount() const override {
		return instanceTransformations[currentInstance].getRotationCount();
	}

	inline Rotation& getRotation(const int idx) override {
		return instanceTransformations[currentInstance].getRotation(idx);
	}

	inline void addRotation(const Vector3& axis, const float angle) override {
		instanceTransformations[currentInstance].addRotation(axis, angle);
	}

	inline void removeRotation(const int idx) override {
		instanceTransformations[currentInstance].removeRotation(idx);
	}

	inline const Vector3& getRotationAxis(const int idx) const override {
		return instanceTransformations[currentInstance].getRotationAxis(idx);
	}

	inline void setRotationAxis(const int idx, const Vector3& axis) override {
		instanceTransformations[currentInstance].setRotationAxis(idx, axis);
	}

	inline const float getRotationAngle(const int idx) const override {
		return instanceTransformations[currentInstance].getRotationAngle(idx);
	}

	inline void setRotationAngle(const int idx, const float angle) override {
		instanceTransformations[currentInstance].setRotationAngle(idx, angle);
	}

	inline const Quaternion& getRotationsQuaternion() const override {
		return instanceTransformations[currentInstance].getRotationsQuaternion();
	}

	inline const Matrix4x4& getTransformationsMatrix() const override {
		return instanceTransformations[currentInstance].getTransformationsMatrix();
	}

	inline const Transformations& getTransformations() const override {
		return instanceTransformations[currentInstance];
	}

	inline RenderPass getRenderPass() const override {
		return renderPass;
	}

	inline void setRenderPass(RenderPass renderPass) override {
		this->renderPass = renderPass;
	}

	/**
	 * @return shader id
	 */
	inline const string& getShader() {
		return shaderId;
	}

	/**
	 * Set shader
	 * @param id shader id
	 */
	void setShader(const string& id);

	/**
	 * @return distance shader id
	 */
	inline const string& getDistanceShader() {
		return distanceShaderId;
	}

	/**
	 * Set distance shader
	 * @param id shader id
	 */
	void setDistanceShader(const string& id);

	/**
	 * @return distance shader distance
	 */
	inline float getDistanceShaderDistance() {
		return distanceShaderDistance;
	}

	/**
	 * Set distance shader distance
	 * @param distanceShaderDistance shader
	 */
	inline void setDistanceShaderDistance(float distanceShaderDistance) {
		this->distanceShaderDistance = distanceShaderDistance;
	}

	/**
	 * @return reflection environment mapping id
	 */
	inline const string& getReflectionEnvironmentMappingId() {
		return reflectionEnvironmentMappingId;
	}

	/**
	 * @return reflection environment mapping id
	 */
	inline void setReflectionEnvironmentMappingId(const string& reflectionEnvironmentMappingId) {
		this->reflectionEnvironmentMappingId = reflectionEnvironmentMappingId;
	}

	/**
	 * @return if to exclude from a certain effect pass
	 */
	inline Engine::EffectPass getExcludeFromEffectPass() const {
		return excludeFromEffectPass;
	}

	/**
	 * Set exclude from effect pass
	 * @param effectPass effect pass
	 */
	inline void setExcludeEffectPass(Engine::EffectPass effectPass) {
		this->excludeFromEffectPass = effectPass;
	}

	/**
	 * @return If early z rejection is enabled
	 */
	inline bool isEnableEarlyZRejection() const {
		return enableEarlyZRejection;
	}

	/**
	 * Enable/disable early z rejection
	 * @param enableEarlyZRejection enable early z rejection
	 */
	inline void setEnableEarlyZRejection(bool enableEarlyZRejection) {
		this->enableEarlyZRejection = enableEarlyZRejection;
	}

	/**
	 * @return if depth test is disabled
	 */
	inline bool isDisableDepthTest() const {
		return disableDepthTest;
	}

	/**
	 * Set disable depth test
	 * @param disableDepthTest disable depth test
	 */
	inline void setDisableDepthTest(bool disableDepthTest) {
		this->disableDepthTest = disableDepthTest;
	}

	/**
	 * Returns shader parameter for given parameter name, if the value does not exist, the default will be returned
	 * @param shaderId shader id
	 * @param parameterName parameter name
	 * @return shader parameter
	 */
	inline const ShaderParameter getShaderParameter(const string& parameterName) {
		return shaderParameters.getShaderParameter(parameterName);
	}

	/**
	 * Set shader parameter for given parameter name
	 * @param shaderId shader id
	 * @param parameterName parameter name
	 * @param paraemterValue parameter value
	 */
	inline void setShaderParameter(const string& parameterName, const ShaderParameter& parameterValue) {
		shaderParameters.setShaderParameter(parameterName, parameterValue);
	}

	/**
	 * Returns distance shader parameter for given parameter name, if the value does not exist, the default will be returned
	 * @param shaderId shader id
	 * @param parameterName parameter name
	 * @return shader parameter
	 */
	inline const ShaderParameter getDistanceShaderParameter(const string& parameterName) {
		return distanceShaderParameters.getShaderParameter(parameterName);
	}

	/**
	 * Set distance shader parameter for given parameter name
	 * @param shaderId shader id
	 * @param parameterName parameter name
	 * @param paraemterValue parameter value
	 */
	inline void setDistanceShaderParameter(const string& parameterName, const ShaderParameter& parameterValue) {
		distanceShaderParameters.setShaderParameter(parameterName, parameterValue);
	}

};
