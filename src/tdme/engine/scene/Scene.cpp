#include <tdme/engine/scene/Scene.h>

#include <algorithm>
#include <map>
#include <string>
#include <vector>

#include <tdme/engine/Transformations.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/model/RotationOrder.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/primitives/BoundingVolume.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Vector4.h>
#include <tdme/engine/prototype/Prototype_Type.h>
#include <tdme/engine/prototype/Prototype.h>
#include <tdme/engine/scene/SceneLibrary.h>
#include <tdme/engine/scene/SceneLight.h>
#include <tdme/engine/scene/SceneEntity.h>
#include <tdme/utilities/Console.h>

using std::map;
using std::remove;
using std::vector;
using std::string;

using tdme::engine::scene::Scene;
using tdme::engine::Transformations;
using tdme::engine::model::Color4;
using tdme::engine::model::Model;
using tdme::engine::model::RotationOrder;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::primitives::BoundingVolume;
using tdme::math::Vector3;
using tdme::math::Vector4;
using tdme::engine::prototype::Prototype_Type;
using tdme::engine::prototype::Prototype;
using tdme::engine::scene::SceneLibrary;
using tdme::engine::scene::SceneLight;
using tdme::engine::scene::SceneEntity;
using tdme::utilities::Console;

Scene::Scene()
{
	applicationRoot = "";
	pathName = ".";
	fileName = "untitled.tl";
	rotationOrder = RotationOrder::XYZ;
	lights.push_back(new SceneLight(0));
	lights.push_back(new SceneLight(1));
	lights.push_back(new SceneLight(2));
	lights.push_back(new SceneLight(3));
	auto light = lights[0];
	light->getAmbient().set(0.7f, 0.7f, 0.7f, 1.0f);
	light->getDiffuse().set(0.3f, 0.3f, 0.3f, 1.0f);
	light->getSpecular().set(1.0f, 1.0f, 1.0f, 1.0f);
	light->getPosition().set(0.0f, 20000.0f, 0.0f, 1.0f);
	light->getSpotDirection().set(0.0f, 0.0f, 0.0f).sub(Vector3(light->getPosition().getX(), light->getPosition().getY(), light->getPosition().getZ()));
	light->getSpotTo().set(light->getPosition().getX(), light->getPosition().getY(), light->getPosition().getZ()).add(light->getSpotDirection());
	light->setConstantAttenuation(0.5f);
	light->setLinearAttenuation(0.0f);
	light->setQuadraticAttenuation(0.0f);
	light->setSpotExponent(0.0f);
	light->setSpotCutOff(180.0f);
	light->setEnabled(true);
	library = new SceneLibrary(this);
	entityIdx = 0;
	skyModelScale = Vector3(1.0f, 1.0f, 1.0f);
}

Scene::~Scene() {
	for (auto light: lights) {
		delete light;
	}
	for (auto object: entities) {
		delete object;
	}
	delete library;
}

void Scene::computeBoundingBox()
{
	dimension.set(0.0f, 0.0f, 0.0f);
	auto haveDimension = false;
	auto levelLeft = 0.0f;
	auto levelRight = 0.0f;
	auto levelNear = 0.0f;
	auto levelFar = 0.0f;
	auto levelTop = 0.0f;
	auto levelBottom = 0.0f;
	Vector3 sideVector(1.0f, 0.0f, 0.0f);
	Vector3 upVector(0.0f, 1.0f, 0.0f);
	Vector3 forwardVector(0.0f, 0.0f, 1.0f);
	Vector3 bbDimension;
	Vector3 bbMin;
	Vector3 bbMax;
	for (auto sceneEntity: entities) {
		if (sceneEntity->getPrototype()->getType() != Prototype_Type::MODEL) continue;
		BoundingBox cbv;
		cbv.fromBoundingVolumeWithTransformations(sceneEntity->getPrototype()->getModel()->getBoundingBox(), sceneEntity->getTransformations());
		bbDimension.set(
			cbv.getDimensions().getX(),
			cbv.getDimensions().getY(),
			cbv.getDimensions().getZ()
		);
		bbDimension.scale(0.5f);
		bbMin.set(cbv.getCenter());
		bbMin.sub(bbDimension);
		bbMax.set(cbv.getCenter());
		bbMax.add(bbDimension);
		auto entityLeft = bbMin.getX();
		auto entityRight = bbMax.getX();
		auto entityNear = bbMin.getZ();
		auto entityFar = bbMax.getZ();
		auto entityBottom = bbMin.getY();
		auto entityTop = bbMax.getY();
		if (haveDimension == false) {
			levelLeft = entityLeft;
			levelRight = entityRight;
			levelNear = entityNear;
			levelFar = entityFar;
			levelTop = entityTop;
			levelBottom = entityBottom;
			haveDimension = true;
		} else {
			if (entityLeft < levelLeft) levelLeft = entityLeft;
			if (entityRight > levelRight) levelRight = entityRight;
			if (entityNear < levelNear) levelNear = entityNear;
			if (entityFar > levelFar) levelFar = entityFar;
			if (entityTop > levelTop) levelTop = entityTop;
			if (entityBottom < levelBottom) levelBottom = entityBottom;
		}
	}
	boundingBox.getMin().set(levelLeft, levelBottom, levelNear);
	boundingBox.getMax().set(levelRight, levelTop, levelFar);
	boundingBox.update();
	dimension.setX(levelRight - levelLeft);
	dimension.setZ(levelFar - levelNear);
	dimension.setY(levelTop - levelBottom);
}

void Scene::computeCenter()
{
	center.set(0.0f, 0.0f, 0.0f);
	auto entityCount = 0;
	for (auto sceneEntity: entities) {
		if (sceneEntity->getPrototype()->getType() != Prototype_Type::MODEL)
			continue;

		center.add(sceneEntity->getTransformations().getTranslation());
		entityCount++;
	}
	if (entityCount != 0)
		center.scale(1.0f / entityCount);
}

void Scene::clearEntities()
{
	entitiesById.clear();
	entities.clear();
	environmentMappingIds.clear();
	entityIdx = 0;
}

void Scene::getEntitiesByPrototypeId(int prototypeId, vector<string>& entitiesByPrototypeId) {
	for (auto entity: entities) {
		if (entity->getPrototype()->getId() == prototypeId) {
			entitiesByPrototypeId.push_back(entity->getId());
		}
	}
}

void Scene::removeEntitiesByPrototypeId(int prototypeId)
{
	vector<string> entitiesToRemove;
	getEntitiesByPrototypeId(prototypeId, entitiesToRemove);
	for (auto entityId: entitiesToRemove) {
		removeEntity(entityId);
	}
}

void Scene::replacePrototype(int searchPrototypeId, int newEntityId)
{
	auto replaceEntity = getLibrary()->getPrototype(newEntityId);
	if (replaceEntity == nullptr)
		return;

	for (auto entity: entities) {
		if (entity->getPrototype()->getId() == searchPrototypeId) {
			entity->setPrototype(replaceEntity);
		}
	}
}

void Scene::addEntity(SceneEntity* entity)
{
	auto _entity = getEntity(entity->getId());
	if (_entity != nullptr) {
		removeEntity(entity->getId());
		Console::println(
			"Scene::addEntity():: entity with id '" +
			entity->getId() +
			"' already exists. Removing it!"
		);
	}
	entitiesById[entity->getId()] = entity;
	entities.push_back(entity);
	if (entity->getPrototype()->getType() == Prototype_Type::ENVIRONMENTMAPPING) environmentMappingIds.insert(entity->getId());
}

void Scene::removeEntity(const string& id)
{
	auto entityByIdIt = entitiesById.find(id);
	if (entityByIdIt != entitiesById.end()) {
		auto entity = entityByIdIt->second;
		entitiesById.erase(entityByIdIt);
		entities.erase(remove(entities.begin(), entities.end(), entity), entities.end());
		if (entity->getPrototype()->getType() == Prototype_Type::ENVIRONMENTMAPPING) environmentMappingIds.erase(entity->getId());
		delete entity;
	}
}

SceneEntity* Scene::getEntity(const string& id)
{
	auto entityByIdIt = entitiesById.find(id);
	if (entityByIdIt != entitiesById.end()) {
		return entityByIdIt->second;
	}
	return nullptr;
}

void Scene::setSkyModel(Model* model) {
	if (this->skyModel== model) return;
	delete this->skyModel;
	this->skyModel = model;
}

void Scene::update() {
	computeBoundingBox();
	computeCenter();
}