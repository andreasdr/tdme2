#include <tdme/tools/shared/model/ModelProperties.h>

#include <algorithm>
#include <string>

#include <tdme/tools/shared/model/PropertyModelClass.h>

using std::remove;
using std::string;

using tdme::tools::shared::model::ModelProperties;
using tdme::tools::shared::model::PropertyModelClass;

ModelProperties::ModelProperties()
{
}

ModelProperties::~ModelProperties() {
	for (auto property: properties) {
		delete property;
	}
}

void ModelProperties::clearProperties()
{
	for (auto property: properties) {
		delete property;
	}
	properties.clear();
	propertiesByName.clear();
}

PropertyModelClass* ModelProperties::getProperty(const string& name)
{
	auto propertyByNameIt = propertiesByName.find(name);
	if (propertyByNameIt != propertiesByName.end()) {
		return propertyByNameIt->second;
	}
	return nullptr;
}

int ModelProperties::getPropertyIndex(const string& name)
{
	for (auto i = 0; i < properties.size(); i++) {
		if (properties[i]->getName() == name) {
			return i;
		}
	}
	return -1;
}

bool ModelProperties::addProperty(const string& name, const string& value)
{
	if (getProperty(name) != nullptr)
		return false;

	auto property = new PropertyModelClass(name, value);
	propertiesByName[name] = property;
	properties.push_back(property);
	return true;
}

bool ModelProperties::updateProperty(const string& oldName, const string& name, const string& value)
{
	auto propertyByNameIt = propertiesByName.find(oldName);
	if (propertyByNameIt == propertiesByName.end())
		return false;

	if (oldName != name && getProperty(name) != nullptr) {
		return false;
	}

	propertiesByName.erase(propertyByNameIt);

	PropertyModelClass* property = propertyByNameIt->second;
	property->setName(name);
	property->setValue(value);

	propertiesByName[property->getName()] = property;

	return true;
}

bool ModelProperties::removeProperty(const string& name)
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
