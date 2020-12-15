#include <tdme/engine/EntityHierarchy.h>

#include <string>

#include <tdme/engine/Engine.h>
#include <tdme/engine/Object3D.h>
#include <tdme/engine/Transformations.h>
#include <tdme/engine/Partition.h>

using std::string;

using tdme::engine::EntityHierarchy;
using tdme::engine::Engine;
using tdme::engine::Object3D;
using tdme::engine::Partition;
using tdme::engine::Transformations;

EntityHierarchy::EntityHierarchy(const string& id): id(id)
{
	this->enabled = true;
	this->pickable = false;
	this->contributesShadows = false;
	this->receivesShadows = false;
	this->effectColorMul.set(1.0f, 1.0f, 1.0f, 1.0f);
	this->effectColorAdd.set(0.0f, 0.0f, 0.0f, 0.0f);
}

EntityHierarchy::~EntityHierarchy()
{
	for (auto entity: entities) delete entity;
}

void EntityHierarchy::setEngine(Engine* engine)
{
	if (this->engine != nullptr) this->engine->deregisterEntity(this);
	this->engine = engine;
	if (engine != nullptr) engine->registerEntity(this);
	for (auto entity: entities) entity->setEngine(engine);
}

void EntityHierarchy::setRenderer(Renderer* renderer)
{
	this->renderer = renderer;
	for (auto entity: entities) entity->setRenderer(renderer);
}

Entity* EntityHierarchy::getEntity(const string& id) {
	auto entityHierarchyLevel = getEntityHierarchyLevel(id);
	if (entityHierarchyLevel == nullptr || entityHierarchyLevel->parent == nullptr) return nullptr;
	return entityHierarchyLevel->entity;
}

void EntityHierarchy::addEntity(Entity* entity, const string& parentId) {
	removeEntity(entity->getId());

	// base properties
	entity->setParentEntity(this);
	entity->setEngine(engine);
	entity->setRenderer(renderer);
	entity->setPickable(pickable);
	entity->setContributesShadows(contributesShadows);
	entity->setReceivesShadows(receivesShadows);
	entity->setEffectColorMul(effectColorMul);
	entity->setEffectColorAdd(effectColorAdd);
	if (initialized == true) entity->initialize();

	// add to hierarchy
	auto parentEntityHierarchyLevel = getEntityHierarchyLevel(parentId);
	if (parentEntityHierarchyLevel == nullptr) {
		Console::println("EntityHierarchy::addEntity(): parent '" + parentId + "': not found");
		return;
	}
	EntityHierarchyLevel childEntityHierarchyLevel;
	childEntityHierarchyLevel.id = entity->getId();
	childEntityHierarchyLevel.parent = parentEntityHierarchyLevel;
	childEntityHierarchyLevel.entity = entity;
	parentEntityHierarchyLevel->children[entity->getId()] = childEntityHierarchyLevel;

	// and entities
	entities.push_back(entity);
}

void EntityHierarchy::removeEntity(const string& id) {
	// remove from hierarchy and entities
	auto entityHierarchyLevel = getEntityHierarchyLevel(id);
	if (entityHierarchyLevel == nullptr || entityHierarchyLevel->parent == nullptr) {
		return;
	}

	//
	vector<string> children;
	for (auto& childIt: entityHierarchyLevel->children) children.push_back(childIt.first);
	for (auto child: children) removeEntity(child);

	//
	auto entity = entityHierarchyLevel->entity;
	entities.erase(remove(entities.begin(), entities.end(), entity), entities.end());
	entityHierarchyLevel->parent->children.erase(id);

	// dispose
	entity->dispose();
	delete entity;
}

const vector<Entity*> EntityHierarchy::query(const string& parentId) {
	vector<Entity*> entities;
	auto parentEntityHierarchyLevel = getEntityHierarchyLevel(parentId);
	if (parentEntityHierarchyLevel == nullptr) {
		return entities;
	}
	for (auto entityIt: parentEntityHierarchyLevel->children) {
		entities.push_back(entityIt.second.entity);
	}
	return entities;
}

void EntityHierarchy::updateHierarchy(const Transformations& parentTransformations, EntityHierarchyLevel& entityHierarchyLevel, int depth, bool& firstEntity) {
	for (auto entityIt: entityHierarchyLevel.children) {
		auto entity = entityIt.second.entity;
		entity->update();
		if (firstEntity == true) {
			boundingBox = entity->getBoundingBoxTransformed();
			firstEntity = false;
		} else {
			boundingBox.extend(entity->getBoundingBoxTransformed());
		}
		entity->applyParentTransformations(parentTransformations);
	}
	for (auto& childIt: entityHierarchyLevel.children) {
		updateHierarchy(childIt.second.entity->getTransformations(), childIt.second, depth++, firstEntity);
	}
	if (depth == 0) {
		// bounding boxes
		boundingBox.update();
		boundingBoxTransformed.fromBoundingVolumeWithTransformations(&boundingBox, *this);
		// update entity
		if (parentEntity == nullptr && frustumCulling == true && engine != nullptr && enabled == true) engine->partition->updateEntity(this);
	}
}

void EntityHierarchy::fromTransformations(const Transformations& transformations)
{
	Transformations::fromTransformations(transformations);
	// update hierarchy
	auto firstEntity = true;
	updateHierarchy(*this, entityRoot, 0, firstEntity);
}

void EntityHierarchy::update()
{
	Transformations::update();
	// update hierarchy
	auto firstEntity = true;
	updateHierarchy(*this, entityRoot, 0, firstEntity);
}

void EntityHierarchy::setEnabled(bool enabled)
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
}

bool EntityHierarchy::isFrustumCulling() {
	return frustumCulling;
}

void EntityHierarchy::setFrustumCulling(bool frustumCulling) {
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

void EntityHierarchy::dispose()
{
	//
	initialized = false;
	// delegate to objects
	for (auto entity: entities) entity->dispose();
}

void EntityHierarchy::initialize()
{
	//
	initialized = true;
	// delegate to objects
	for (auto entity: entities) entity->initialize();
}
