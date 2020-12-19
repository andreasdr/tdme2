#include <tdme/engine/prototype/PrototypeProperties.h>

#include <algorithm>
#include <string>

#include <tdme/engine/prototype/PrototypeProperty.h>

using std::remove;
using std::string;

using tdme::engine::prototype::PrototypeProperties;
using tdme::engine::prototype::PrototypeProperty;

PrototypeProperties::PrototypeProperties()
{
}

PrototypeProperties::~PrototypeProperties() {
	for (auto property: properties) {
		delete property;
	}
}

void PrototypeProperties::clearProperties()
{
	for (auto property: properties) {
		delete property;
	}
	properties.clear();
	propertiesByName.clear();
}

PrototypeProperty* PrototypeProperties::getProperty(const string& name)
{
	auto propertyByNameIt = propertiesByName.find(name);
	if (propertyByNameIt != propertiesByName.end()) {
		return propertyByNameIt->second;
	}
	return nullptr;
}

int PrototypeProperties::getPropertyIndex(const string& name)
{
	for (auto i = 0; i < properties.size(); i++) {
		if (properties[i]->getName() == name) {
			return i;
		}
	}
	return -1;
}

bool PrototypeProperties::addProperty(const string& name, const string& value)
{
	if (getProperty(name) != nullptr)
		return false;

	auto property = new PrototypeProperty(name, value);
	propertiesByName[name] = property;
	properties.push_back(property);
	return true;
}

bool PrototypeProperties::updateProperty(const string& oldName, const string& name, const string& value)
{
	auto propertyByNameIt = propertiesByName.find(oldName);
	if (propertyByNameIt == propertiesByName.end())
		return false;

	if (oldName != name && getProperty(name) != nullptr) {
		return false;
	}

	propertiesByName.erase(propertyByNameIt);

	PrototypeProperty* property = propertyByNameIt->second;
	property->setName(name);
	property->setValue(value);

	propertiesByName[property->getName()] = property;

	return true;
}

bool PrototypeProperties::removeProperty(const string& name)
{
	auto propertyByNameIt = propertiesByName.find(name);
	if (propertyByNameIt != propertiesByName.end()) {
		auto property = propertyByNameIt->second;
		propertiesByName.erase(propertyByNameIt);
		properties.erase(remove(properties.begin(), properties.end(), property), properties.end());
		return true;
	}

	return false;
}
