#pragma once

#include <algorithm>
#include <map>
#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/primitives/fwd-tdme.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/engine/subsystems/rendering/Object3DInternal.h>
#include <tdme/engine/Camera.h>
#include <tdme/engine/Entity.h>
#include <tdme/engine/Object3D.h>
#include <tdme/engine/Rotation.h>
#include <tdme/engine/Transformations.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/utilities/Console.h>

using std::map;
using std::remove;
using std::string;
using std::to_string;
using std::vector;

using tdme::engine::model::Color4;
using tdme::engine::model::Model;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::subsystems::rendering::Object3DInternal;
using tdme::engine::Engine;
using tdme::engine::Entity;
using tdme::engine::Object3D;
using tdme::engine::Rotation;
using tdme::engine::Transformations;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;
using tdme::utilities::Console;

/**
 * Entity hierarchy to be used with engine class
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::EntityHierarchy final:
	public Transformations,
	public Entity
{
private:
	struct EntityHierarchyLevel {
		typedef map<string, EntityHierarchyLevel> EntityHierachyLevelMap;
		string id;
		EntityHierarchyLevel* parent { nullptr };
		Entity* entity { nullptr };
		map<string, EntityHierarchyLevel> children;
	};
	Engine* engine { nullptr };
	Renderer* renderer { nullptr };
	Entity* parentEntity { nullptr };
	bool frustumCulling { true };
	bool initialized { false };

	string id;
	bool enabled;
	bool pickable;
	bool contributesShadows;
	bool receivesShadows;
	Color4 effectColorMul;
	Color4 effectColorAdd;
	BoundingBox boundingBox;
	BoundingBox boundingBoxTransformed;
	vector<Entity*> entities;
	EntityHierarchyLevel entityRoot;

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
		boundingBoxTransformed.fromBoundingVolumeWithTransformations(&boundingBox, *this);
	}

	/**
	 * Get entity hierarchy level by given entity id
	 */
	inline EntityHierarchyLevel* getEntityHierarchyLevel(const string& id) {
		if (id.empty()) return &entityRoot;
		return getEntityHierarchyLevel(&entityRoot, id);
	}

	/**
	 * Retrieve entity hierarchy level by given entity id or nullptr if not found
	 * @param entityHierarchyLevel entity hierarchy level
	 * @param id entity id
	 * @return entity hierarchy level by given entity id or nullptr if not found
	 */
	inline EntityHierarchyLevel* getEntityHierarchyLevel(EntityHierarchyLevel* entityHierarchyLevel, const string& id) {
		if (id == entityHierarchyLevel->id) return entityHierarchyLevel;
		for (auto& it: entityHierarchyLevel->children) {
			auto childEntityHierarchyLevel = getEntityHierarchyLevel(&it.second, id);
			if (childEntityHierarchyLevel != nullptr) return childEntityHierarchyLevel;
		}
		return nullptr;
	}

	/**
	 * Update hierarchy from given entity hierarchy level ongoing
	 * @param parentTransformations parent transformations
	 * @param entityHierarchyLevel entity hierarchy level
	 * @param depth depth
	 * @param firstEntity first entity
	 */
	void updateHierarchy(const Transformations& parentTransformations, EntityHierarchyLevel& entityHierarchyLevel, int depth, bool& firstEntity);

public:
	/**
	 * Public constructor
	 * @param id id
	 */
	EntityHierarchy(const string& id);

	/**
	 * Destructor
	 */
	virtual ~EntityHierarchy();

	// overridden method
	inline EntityType getEntityType() override {
		return ENTITY_ENTITYHIERARCHY;
	}

	/**
	 * @return entity from hierarchy by given unique id
	 */
	Entity* getEntity(const string& id);

	/**
	 * Adds a entity to the hierarchy
	 * @param entity entity to add
	 * @param parentId parent entity id to add entity to
	 */
	void addEntity(Entity* entity, const string& parentId = string());

	/**
	 * Removes a entity from hierarchy by given unique entity id
	 */
	void removeEntity(const string& id);

	/**
	 * Query direct sub entities for given parent entity id
	 * @param parentId parent id to entities from
	 * @return entities
	 */
	const vector<Entity*> query(const string& parentId = string());

	/**
	 * @return entities
	 */
	inline const vector<Entity*>& getEntities() {
		return entities;
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
		for (auto entity: entities) entity->setEffectColorMul(effectColorMul);
	}

	inline const Color4& getEffectColorAdd() const override {
		return effectColorAdd;
	}

	inline void setEffectColorAdd(const Color4& effectColorAdd) override {
		this->effectColorAdd = effectColorAdd;
		for (auto entity: entities) entity->setEffectColorAdd(effectColorAdd);
	}

	inline const string& getId() override {
		return id;
	}

	inline bool isContributesShadows() override {
		return contributesShadows;
	}

	inline void setContributesShadows(bool contributesShadows) override {
		this->contributesShadows = contributesShadows;
		for (auto entity: entities) entity->setContributesShadows(contributesShadows);
	}

	inline bool isReceivesShadows() override {
		return receivesShadows;
	}

	inline void setReceivesShadows(bool receivesShadows) override {
		this->receivesShadows = receivesShadows;
		for (auto entity: entities) entity->setReceivesShadows(receivesShadows);
	}

	inline bool isPickable() override {
		return pickable;
	}

	inline void setPickable(bool pickable) override {
		this->pickable = pickable;
		for (auto entity: entities) entity->setPickable(pickable);
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

};
