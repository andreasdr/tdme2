#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/Camera.h>
#include <tdme/engine/Object3D.h>
#include <tdme/engine/Rotation.h>
#include <tdme/engine/Transformations.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/primitives/fwd-tdme.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/engine/subsystems/rendering/Object3DInternal.h>
#include <tdme/engine/Entity.h>
#include <tdme/utilities/Console.h>

using std::string;
using std::to_string;

using tdme::engine::subsystems::rendering::Object3DInternal;
using tdme::engine::Entity;
using tdme::engine::Engine;
using tdme::engine::Object3D;
using tdme::engine::Rotation;
using tdme::engine::Transformations;
using tdme::engine::model::Color4;
using tdme::engine::model::Model;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;
using tdme::utilities::Console;

/**
 * LOD object 3D to be used with engine class
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::LODObject3D final:
	public Transformations,
	public Entity
{
public:
	enum LODLevelType { LODLEVELTYPE_NONE, LODLEVELTYPE_MODEL, LODLEVELTYPE_IGNORE };

private:
	friend class Object3DRenderGroup;

	Engine* engine { nullptr };
	Entity* parentEntity { nullptr };
	bool frustumCulling { true };

	Model* modelLOD1;
	Model* modelLOD2;
	Model* modelLOD3;
	float modelLOD2MinDistance;
	float modelLOD3MinDistance;
	LODLevelType levelTypeLOD2;
	LODLevelType levelTypeLOD3;

	string id;
	Object3D* objectLOD1 { nullptr };
	Object3D* objectLOD2 { nullptr };
	Object3D* objectLOD3 { nullptr };
	Object3D* objectLOD { nullptr };
	int levelLOD;
	bool enabled;
	bool pickable;
	bool contributesShadows;
	bool receivesShadows;
	Color4 effectColorMul;
	Color4 effectColorAdd;
	Color4 effectColorMulLOD2;
	Color4 effectColorAddLOD2;
	Color4 effectColorMulLOD3;
	Color4 effectColorAddLOD3;
	RenderPass renderPass { RENDERPASS_STANDARD };
	string shaderId { "default" };
	string distanceShaderId { "" };
	float distanceShaderDistance { 50.0f };
	bool enableEarlyZRejection { false };

	map<string, string> shaderParameters;
	map<string, string> distanceShaderParameters;

	/**
	 * Set parent entity, needs to be called before adding to engine
	 * @param entity entity
	 */
	inline void setParentEntity(Entity* entity) override {
		this->parentEntity = entity;
	}

	/**
	 * @return parent entity
	 */
	inline Entity* getParentEntity() override {
		return parentEntity;
	}

	// overridden methods
	inline void applyParentTransformations(const Transformations& parentTransformations) override {
		Transformations::applyParentTransformations(parentTransformations);
		// delegate to LOD objects
		if (objectLOD1 != nullptr) objectLOD1->fromTransformations(*this);
		if (objectLOD2 != nullptr) objectLOD2->fromTransformations(*this);
		if (objectLOD3 != nullptr) objectLOD3->fromTransformations(*this);
	}

public:
	/**
	 * Public constructor
	 * @param id id
	 * @param modelLOD1 model LOD 1
	 * @param levelTypeLOD2 LOD level type LOD2
	 * @param modelLOD2MinDistance model LOD 2 min distance
	 * @param modelLOD2 model LOD 2
	 * @param levelTypeLOD3 LOD level type LOD3
	 * @param modelLOD3MinDistance model LOD 3 min distance
	 * @param modelLOD3 model LOD 3
	 * @param planeRotationYLOD2 model LOD2 plane rotation around Y axis
	 * @param planeRotationYLOD3 model LOD3 plane rotation around Y axis
	 */
	LODObject3D(
		const string& id,
		Model* modelLOD1,
		LODLevelType levelTypeLOD2,
		float modelLOD2MinDistance,
		Model* modelLOD2,
		LODLevelType levelTypeLOD3,
		float modelLOD3MinDistance,
		Model* modelLOD3
	);

	/**
	 * Public destructor
	 */
	~LODObject3D();

	/**
	 * @return LOD object
	 */
	inline Object3D* getLODObject() {
		// set effect colors
		if (objectLOD != nullptr) {
			// set effect colors
			if (objectLOD != nullptr) {
				if (levelLOD == 3) {
					objectLOD->setEffectColorAdd(effectColorAddLOD3);
					objectLOD->setEffectColorMul(effectColorMulLOD3);
				} else
				if (levelLOD == 2) {
					objectLOD->setEffectColorAdd(effectColorAddLOD2);
					objectLOD->setEffectColorMul(effectColorMulLOD2);
				} else {
					objectLOD->setEffectColorAdd(Color4(0.0f, 0.0f, 0.0f, 0.0f));
					objectLOD->setEffectColorMul(Color4(1.0f, 1.0f, 1.0f, 1.0f));
				}
				auto effectColorAdd = objectLOD->getEffectColorAdd();
				auto effectColorMul = objectLOD->getEffectColorMul();
				effectColorAdd.add(this->effectColorAdd);
				effectColorMul.scale(this->effectColorMul);
				objectLOD->setEffectColorAdd(effectColorAdd);
				objectLOD->setEffectColorMul(effectColorMul);
			}
		}

		//
		return objectLOD;
	}

	/**
	 * Get current lod object
	 * @param camera camera
	 * @return LOD object to render
	 */
	inline Object3D* determineLODObject(Camera* camera) {
		if (objectLOD == nullptr || camera->hasFrustumChanged() == true) {
			LODObject3D::LODLevelType lodLevelType = LODObject3D::LODLEVELTYPE_NONE;
			Vector3 objectCamFromAxis;
			float objectCamFromLengthSquared;
			float planeRotationYLOD;

			// determine LOD object and level type
			if (levelTypeLOD3 != LODLEVELTYPE_NONE &&
				(objectCamFromLengthSquared = objectCamFromAxis.set(getBoundingBoxTransformed()->computeClosestPointInBoundingBox(camera->getLookFrom())).sub(camera->getLookFrom()).computeLengthSquared()) >= Math::square(modelLOD3MinDistance)) {
				objectLOD = objectLOD3;
				levelLOD = 3;
			} else
			if (levelTypeLOD2 != LODLEVELTYPE_NONE &&
				(objectCamFromLengthSquared = objectCamFromAxis.set(getBoundingBoxTransformed()->computeClosestPointInBoundingBox(camera->getLookFrom())).sub(camera->getLookFrom()).computeLengthSquared()) >= Math::square(modelLOD2MinDistance)) {
				objectLOD = objectLOD2;
				levelLOD = 2;
			} else {
				objectLOD = objectLOD1;
				levelLOD = 1;
			}
		}

		// set effect colors
		if (objectLOD != nullptr) {
			if (levelLOD == 3) {
				objectLOD->setEffectColorAdd(effectColorAddLOD3);
				objectLOD->setEffectColorMul(effectColorMulLOD3);
			} else
			if (levelLOD == 2) {
				objectLOD->setEffectColorAdd(effectColorAddLOD2);
				objectLOD->setEffectColorMul(effectColorMulLOD2);
			} else {
				objectLOD->setEffectColorAdd(Color4(0.0f, 0.0f, 0.0f, 0.0f));
				objectLOD->setEffectColorMul(Color4(1.0f, 1.0f, 1.0f, 1.0f));
			}
			auto effectColorAdd = objectLOD->getEffectColorAdd();
			auto effectColorMul = objectLOD->getEffectColorMul();
			effectColorAdd.add(this->effectColorAdd);
			effectColorMul.scale(this->effectColorMul);
			objectLOD->setEffectColorAdd(effectColorAdd);
			objectLOD->setEffectColorMul(effectColorMul);
		}

		// done
		return objectLOD;
	}

	/**
	 * @return effect color add for LOD2 level
	 */
	inline const Color4& getEffectColorAddLOD2() const {
		return effectColorAddLOD2;
	}

	/**
	 * Set effect color add for LOD2 level
	 * @param effectColorAddLOD2 effect color add for LOD2 level
	 */
	inline void setEffectColorAddLOD2(const Color4& effectColorAddLOD2) {
		this->effectColorAddLOD2 = effectColorAddLOD2;
	}

	/**
	 * @return effect color mul for LOD2 level
	 */
	inline const Color4& getEffectColorMulLOD2() const {
		return effectColorMulLOD2;
	}

	/**
	 * Set effect color mul for LOD2 level
	 * @param effectColorMulLOD2 effect color mul for LOD2 level
	 */
	inline void setEffectColorMulLOD2(const Color4& effectColorMulLOD2) {
		this->effectColorMulLOD2 = effectColorMulLOD2;
	}

	/**
	 * @return effect color add for LOD3 level
	 */
	inline const Color4& getEffectColorAddLOD3() const {
		return effectColorAddLOD3;
	}

	/**
	 * Set effect color add for LOD3 level
	 * @param effectColorAddLOD3 effect color add for LOD3 level
	 */
	inline void setEffectColorAddLOD3(const Color4& effectColorAddLOD3) {
		this->effectColorAddLOD3 = effectColorAddLOD3;
	}

	/**
	 * @return effect color mul for LOD3 level
	 */
	inline const Color4& getEffectColorMulLOD3() const {
		return effectColorMulLOD3;
	}

	/**
	 * Set effect color mul for LOD3 level
	 * @param effectColorMulLOD3 effect color mul for LOD3 level
	 */
	inline void setEffectColorMulLOD3(const Color4& effectColorMulLOD3) {
		this->effectColorMulLOD3 = effectColorMulLOD3;
	}

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
		return objectLOD1->getBoundingBox();
	}

	inline BoundingBox* getBoundingBoxTransformed() override {
		return objectLOD1->getBoundingBoxTransformed();
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
		if (objectLOD1 != nullptr) objectLOD1->setContributesShadows(contributesShadows);
		if (objectLOD2 != nullptr) objectLOD2->setContributesShadows(contributesShadows);
		if (objectLOD3 != nullptr) objectLOD3->setContributesShadows(contributesShadows);
	}

	inline bool isReceivesShadows() override {
		return receivesShadows;
	}

	inline void setReceivesShadows(bool receivesShadows) override {
		this->receivesShadows = receivesShadows;
		if (objectLOD1 != nullptr) objectLOD1->setReceivesShadows(receivesShadows);
		if (objectLOD2 != nullptr) objectLOD2->setReceivesShadows(receivesShadows);
		if (objectLOD3 != nullptr) objectLOD3->setReceivesShadows(receivesShadows);
	}

	inline void setPickable(bool pickable) override {
		this->pickable = pickable;
	}

	inline const Matrix4x4 getNodeTransformationsMatrix(const string& id) {
		return objectLOD1->getNodeTransformationsMatrix(id);
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
		if (objectLOD1 != nullptr) objectLOD1->setRenderPass(renderPass);
		if (objectLOD2 != nullptr) objectLOD2->setRenderPass(renderPass);
		if (objectLOD3 != nullptr) objectLOD3->setRenderPass(renderPass);
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
		if (objectLOD1 != nullptr) objectLOD1->setShader(shaderId);
		if (objectLOD2 != nullptr) objectLOD2->setShader(shaderId);
		if (objectLOD3 != nullptr) objectLOD3->setShader(shaderId);
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
		if (objectLOD1 != nullptr) objectLOD1->setDistanceShader(distanceShaderId);
		if (objectLOD2 != nullptr) objectLOD2->setDistanceShader(distanceShaderId);
		if (objectLOD3 != nullptr) objectLOD3->setDistanceShader(distanceShaderId);
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
		if (objectLOD1 != nullptr) objectLOD1->setDistanceShaderDistance(distanceShaderDistance);
		if (objectLOD2 != nullptr) objectLOD2->setDistanceShaderDistance(distanceShaderDistance);
		if (objectLOD3 != nullptr) objectLOD3->setDistanceShaderDistance(distanceShaderDistance);
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
	void setEnableEarlyZRejection(bool enableEarlyZRejection);

	/**
	 * Get shader parameters
	 * @return shader parameters
	 */
	inline const map<string, string>& getShaderParameters(const map<string, string>& parameters) {
		return shaderParameters;
	}

	/**
	 * Set shader parameters
	 * @param parameters shader parameters
	 */
	inline void setShaderParameters(const map<string, string>& parameters) {
		shaderParameters = parameters;
		if (objectLOD1 != nullptr) objectLOD1->setShaderParameters(shaderParameters);
		if (objectLOD2 != nullptr) objectLOD2->setShaderParameters(shaderParameters);
		if (objectLOD3 != nullptr) objectLOD3->setShaderParameters(shaderParameters);
	}

	/**
	 * Get distance shader parameters
	 * @return distance shader parameters
	 */
	inline const map<string, string>& getDistanceShaderParameters(const map<string, string>& parameters) {
		return distanceShaderParameters;
	}

	/**
	 * Set distance shader parameters
	 * @param parameters distance shader parameters
	 */
	inline void setDistanceShaderParameters(const map<string, string>& parameters) {
		distanceShaderParameters = parameters;
		if (objectLOD1 != nullptr) objectLOD1->setDistanceShaderParameters(distanceShaderParameters);
		if (objectLOD2 != nullptr) objectLOD2->setDistanceShaderParameters(distanceShaderParameters);
		if (objectLOD3 != nullptr) objectLOD3->setDistanceShaderParameters(distanceShaderParameters);
	}

};
