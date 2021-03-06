#pragma once

#include <array>
#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/model/Node.h>
#include <tdme/engine/primitives/fwd-tdme.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/engine/Camera.h>
#include <tdme/engine/Entity.h>
#include <tdme/engine/Entity.h>
#include <tdme/engine/EntityShaderParameters.h>
#include <tdme/engine/LODObject3D.h>
#include <tdme/engine/Object3D.h>
#include <tdme/engine/Rotation.h>
#include <tdme/engine/Transformations.h>
#include <tdme/engine/Transformations.h>
#include <tdme/math/fwd-tdme.h>

using std::array;
using std::string;
using std::to_string;
using std::vector;

using tdme::engine::model::Color4;
using tdme::engine::model::Model;
using tdme::engine::model::Node;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::Engine;
using tdme::engine::Entity;
using tdme::engine::EntityShaderParameters;
using tdme::engine::LODObject3D;
using tdme::engine::Object3D;
using tdme::engine::Rotation;
using tdme::engine::Transformations;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;

/**
 * Object 3D render group for static objects that might be animated by shaders
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::Object3DRenderGroup final:
	public Transformations,
	public Entity
{
private:
	Engine* engine { nullptr };
	Entity* parentEntity { nullptr };
	bool frustumCulling { true };

	string id;
	bool enabled;
	bool pickable;
	bool contributesShadows;
	bool receivesShadows;
	Color4 effectColorMul;
	Color4 effectColorAdd;
	BoundingBox boundingBox;
	BoundingBox boundingBoxTransformed;
	float modelLOD2MinDistance;
	float modelLOD3MinDistance;
	Entity* combinedEntity;
	map<Model*, vector<Transformations>> transformationsByModel;
	vector<Model*> combinedModels;
	RenderPass renderPass { RENDERPASS_STANDARD };
	string shaderId { "default" };
	string distanceShaderId { "" };
	float distanceShaderDistance { 50.0f };
	array<int, 3> lodReduceBy;
	bool enableEarlyZRejection { false };

	EntityShaderParameters shaderParameters;
	EntityShaderParameters distanceShaderParameters;

	bool optimizeModels;

	/**
	 * Compute bounding box
	 */
	inline void updateBoundingBox() {
		if (combinedEntity == nullptr) return;
		boundingBox.fromBoundingVolume(combinedEntity->getBoundingBox());
		boundingBoxTransformed.fromBoundingVolumeWithTransformations(&boundingBox, *this);
	}

	/**
	 * Combine node into given combined model
	 * @param sourceNode source node to combine into current model
	 * @param origins origins
	 * @param objectParentTransformationsMatrices object parent transformations matrix
	 * @param combinedModel combined model
	 * @param reduceFactorBy reduce factor by
	 */
	static void combineNode(Node* sourceNode, const vector<Vector3>& origins, const vector<Matrix4x4>& objectParentTransformationsMatrices, Model* combinedModel);

	/**
	 * Combine model with transformations into current model
	 * @param model model
	 * @param transformations transformations
	 * @param combinedModel combined model
	 * @param reduceFactorBy reduce factor by
	 * @return model
	 */
	static void combineObjects(Model* model, const vector<Transformations>& objectsTransformations, Model* combinedModel);

	// overridden methods
	inline void setParentEntity(Entity* entity) override {
		this->parentEntity = entity;
	}
	inline Entity* getParentEntity() override {
		return parentEntity;
	}
	inline void applyParentTransformations(const Transformations& parentTransformations) override {
		Transformations::applyParentTransformations(parentTransformations);
		updateBoundingBox();
	}

public:
	/**
	 * Public constructor
	 * @param id id
	 * @param model model
	 * @param lodLevels lod levels
	 * @param modelLOD2MinDistance model LOD 2 min distance
	 * @param modelLOD3MinDistance model LOD 3 min distance
	 * @param modelLOD2ReduceBy model LOD 2 reduce by factor
	 * @param modelLOD3ReduceBy model LOD 3 reduce by factor
	 * @param optimizeModels optimize models
	 */
	Object3DRenderGroup(
		const string& id,
		int lodLevels = 1,
		float modelLOD2MinDistance = 25.0f,
		float modelLOD3MinDistance = 50.0f,
		int modelLOD2ReduceBy = 4,
		int modelLOD3ReduceBy = 16,
		bool optimizeModels = true
	);

	/**
	 * Destructor
	 */
	~Object3DRenderGroup();

	// overridden methods
	inline EntityType getEntityType() override {
		return ENTITY_OBJECT3DRENDERGROUP;
	}

	/**
	 * Update render group model and bounding box
	 */
	void updateRenderGroup();

	/**
	 * @return entity
	 */
	inline Entity* getEntity() {
		return combinedEntity;
	}

	/**
	 * Adds a instance to this render group
	 * @param model model
	 * @param transformations transformations
	 */
	void addObject(Model* model, const Transformations& transformations);

	// overridden methods
	void setEngine(Engine* engine) override;
	void setRenderer(Renderer* renderer) override;
	void initialize() override;
	void dispose() override;

	inline bool isEnabled() override {
		return enabled;
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

	inline const Color4& getEffectColorMul() const override {
		return effectColorMul;
	}

	inline void setEffectColorMul(const Color4& effectColorMul) override {
		this->effectColorMul = effectColorMul;
	}

	inline const Color4& getEffectColorAdd() const override {
		return effectColorAdd;
	}

	inline void setEffectColorAdd(const Color4& effectColorAdd) override {
		this->effectColorAdd = effectColorAdd;
	}

	inline const string& getId() override {
		return id;
	}

	inline bool isPickable() override {
		return pickable;
	}

	inline bool isContributesShadows() override {
		return contributesShadows;
	}

	inline void setContributesShadows(bool contributesShadows) override {
		this->contributesShadows = contributesShadows;
		if (combinedEntity != nullptr) combinedEntity->setContributesShadows(contributesShadows);
	}

	inline bool isReceivesShadows() override {
		return receivesShadows;
	}

	inline void setReceivesShadows(bool receivesShadows) override {
		this->receivesShadows = receivesShadows;
		if (combinedEntity != nullptr) combinedEntity->setReceivesShadows(receivesShadows);
	}

	inline void setPickable(bool pickable) override {
		if (combinedEntity != nullptr) combinedEntity->setPickable(pickable);
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
		if (combinedEntity == nullptr) return;
		combinedEntity->setRenderPass(renderPass);
	}

	/**
	 * @return shader id
	 */
	inline const string& getShader() {
		return shaderId;
	}

	/**
	 * Set shader id
	 * @param id shader
	 */
	inline void setShader(const string& id) {
		this->shaderId = id;
		shaderParameters.setShader(id);
		// TODO: put me into entity interface
		if (combinedEntity == nullptr) return;
		if (combinedEntity->getEntityType() == Entity::ENTITY_OBJECT3D) {
			static_cast<Object3D*>(combinedEntity)->setShader(id);
			shaderParameters.setShader(static_cast<Object3D*>(combinedEntity)->getShader());
		} else
		if (combinedEntity->getEntityType() == Entity::ENTITY_LODOBJECT3D) {
			static_cast<LODObject3D*>(combinedEntity)->setShader(id);
			shaderParameters.setShader(static_cast<LODObject3D*>(combinedEntity)->getShader());
		}
	}

	/**
	 * @return distance shader id
	 */
	inline const string& getDistanceShader() {
		return distanceShaderId;
	}

	/**
	 * Set distance shader id
	 * @param id shader
	 */
	inline void setDistanceShader(const string& id) {
		this->distanceShaderId = id;
		distanceShaderParameters.setShader(id);
		// TODO: put me into entity interface
		if (combinedEntity == nullptr) return;
		if (combinedEntity->getEntityType() == Entity::ENTITY_OBJECT3D) {
			static_cast<Object3D*>(combinedEntity)->setDistanceShader(id);
			shaderParameters.setShader(static_cast<Object3D*>(combinedEntity)->getDistanceShader());
		} else
		if (combinedEntity->getEntityType() == Entity::ENTITY_LODOBJECT3D) {
			static_cast<LODObject3D*>(combinedEntity)->setDistanceShader(id);
			shaderParameters.setShader(static_cast<LODObject3D*>(combinedEntity)->getDistanceShader());
		}
	}

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
		// TODO: put me into entity interface
		if (combinedEntity == nullptr) return;
		if (combinedEntity->getEntityType() == Entity::ENTITY_OBJECT3D) {
			static_cast<Object3D*>(combinedEntity)->setDistanceShaderDistance(distanceShaderDistance);
		} else
		if (combinedEntity->getEntityType() == Entity::ENTITY_LODOBJECT3D) {
			static_cast<LODObject3D*>(combinedEntity)->setDistanceShaderDistance(distanceShaderDistance);
		}
	}

	/**
	 * @return If early z rejection is enabled
	 */
	bool isEnableEarlyZRejection() const {
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
		if (combinedEntity == nullptr) return;
		if (combinedEntity->getEntityType() == Entity::ENTITY_OBJECT3D) {
			static_cast<Object3D*>(combinedEntity)->setShaderParameter(parameterName, parameterValue);
		} else
		if (combinedEntity->getEntityType() == Entity::ENTITY_LODOBJECT3D) {
			static_cast<LODObject3D*>(combinedEntity)->setShaderParameter(parameterName, parameterValue);
		}
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
		if (combinedEntity == nullptr) return;
		if (combinedEntity->getEntityType() == Entity::ENTITY_OBJECT3D) {
			static_cast<Object3D*>(combinedEntity)->setDistanceShaderParameter(parameterName, parameterValue);
		} else
		if (combinedEntity->getEntityType() == Entity::ENTITY_LODOBJECT3D) {
			static_cast<LODObject3D*>(combinedEntity)->setDistanceShaderParameter(parameterName, parameterValue);
		}
	}

};
