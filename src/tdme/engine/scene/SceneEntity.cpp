#include <tdme/engine/scene/SceneEntity.h>

#include <string>

#include <tdme/engine/prototype/Prototype.h>
#include <tdme/engine/prototype/PrototypeProperties.h>
#include <tdme/engine/prototype/PrototypeProperty.h>
#include <tdme/engine/Transformations.h>

using std::string;

using tdme::engine::prototype::Prototype;
using tdme::engine::prototype::PrototypeProperties;
using tdme::engine::prototype::PrototypeProperty;
using tdme::engine::scene::SceneEntity;
using tdme::engine::Transformations;

SceneEntity::SceneEntity(const string& id, const string& description, const Transformations& transformations, Prototype* prototype)
{
	this->id = id;
	this->description = description;
	this->transformations.fromTransformations(transformations);
	this->prototype = prototype;
}

SceneEntity::~SceneEntity() {
}

const PrototypeProperties SceneEntity::getTotalProperties()
{
	PrototypeProperties totalProperties;
	for (auto i = 0; i < getPrototype()->getPropertyCount(); i++) {
		auto prototypeProperty = getPrototype()->getPropertyByIndex(i);
		totalProperties.addProperty(prototypeProperty->getName(), prototypeProperty->getValue());
	}
	for (auto i = 0; i < getPropertyCount(); i++) {
		auto entityProperty = getPropertyByIndex(i);
		auto totalProperty = totalProperties.getProperty(entityProperty->getName());
		if (totalProperty != nullptr) {
			totalProperties.updateProperty(totalProperty->getName(), entityProperty->getName(), entityProperty->getValue());
		} else {
			totalProperties.addProperty(entityProperty->getName(), entityProperty->getValue());
		}
	}
	return totalProperties;
}
