#include <tdme/tools/shared/model/LevelPropertyPresets.h>

#include <stdlib.h>

#include <map>
#include <string>
#include <vector>

#include <tdme/application/Application.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Color4Base.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Vector4.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/tools/shared/model/LevelEditorLevel.h>
#include <tdme/tools/shared/model/LevelEditorLight.h>
#include <tdme/tools/shared/model/PropertyModelClass.h>
#include <tdme/tools/shared/tools/Tools.h>
#include <tdme/utilities/Exception.h>
#include <tdme/utilities/Console.h>

#include <ext/tinyxml/tinyxml.h>

using std::map;
using std::string;
using std::vector;

using tdme::tools::shared::model::LevelPropertyPresets;
using tdme::application::Application;
using tdme::engine::model::Color4;
using tdme::engine::model::Color4Base;
using tdme::math::Vector3;
using tdme::math::Vector4;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;
using tdme::tools::shared::model::LevelEditorLevel;
using tdme::tools::shared::model::LevelEditorLight;
using tdme::tools::shared::model::PropertyModelClass;
using tdme::tools::shared::tools::Tools;
using tdme::utilities::Exception;
using tdme::utilities::Console;
using tinyxml::TiXmlDocument;
using tinyxml::TiXmlElement;

LevelPropertyPresets* LevelPropertyPresets::instance = nullptr;

LevelPropertyPresets::LevelPropertyPresets(const string& pathName, const string& fileName)  /* throws(Exception) */
{
	auto xmlContent = FileSystem::getInstance()->getContentAsString(pathName, fileName);
	TiXmlDocument xmlDocument;
	xmlDocument.Parse(xmlContent.c_str());
	if (xmlDocument.Error() == true) {
		Console::println(
			"LevelPropertyPresets::ctor():: Could not parse file '" +
			pathName + "/" + fileName +
			"'. Error='" +
			xmlDocument.ErrorDesc() +
			"'. Exiting.\n"
		);
		Application::exit(1);
	}
	TiXmlElement* xmlRoot = xmlDocument.RootElement();

	for (auto xmlMap: getChildrenByTagName(xmlRoot, "map")) {
		for (auto xmlProperty: getChildrenByTagName(xmlMap, "property")) {
			mapPropertiesPreset.push_back(
				new PropertyModelClass(
					(xmlProperty->Attribute("name")),
					(xmlProperty->Attribute("value"))
				)
			);
		}
	}

	for (auto xmlObject: getChildrenByTagName(xmlRoot, "object")) {
		for (auto xmlType: getChildrenByTagName(xmlObject, "type")) {
			auto typeId = (xmlType->Attribute("id"));
			objectPropertiesPresets[typeId].push_back(new PropertyModelClass("preset", typeId));
			for (auto xmlProperty: getChildrenByTagName(xmlType, "property")) {
				objectPropertiesPresets[typeId].push_back(
					new PropertyModelClass(
						(xmlProperty->Attribute("name")),
						(xmlProperty->Attribute("value"))
					)
				);
			}
		}
	}

	auto lightId = 0;
	for (auto xmlLights: getChildrenByTagName(xmlRoot, "lights")) {
		for (auto xmlType: getChildrenByTagName(xmlLights, "type")) {
			{
				auto typeId = ((xmlType->Attribute("id")));
				auto light = new LevelEditorLight(lightId++);
				light->getAmbient().set(Tools::convertToColor4((getChildrenByTagName(xmlType, "ambient")[0]->GetText())));
				light->getDiffuse().set(Tools::convertToColor4((getChildrenByTagName(xmlType, "diffuse")[0]->GetText())));
				light->getSpecular().set(Tools::convertToColor4((getChildrenByTagName(xmlType, "specular")[0]->GetText())));
				light->getPosition().set(Tools::convertToVector4((getChildrenByTagName(xmlType, "position")[0]->GetText())));
				light->setConstantAttenuation(Tools::convertToFloat((getChildrenByTagName(xmlType, "constant_attenuation")[0]->GetText())));
				light->setLinearAttenuation(Tools::convertToFloat((getChildrenByTagName(xmlType, "linear_attenuation")[0]->GetText())));
				light->setQuadraticAttenuation(Tools::convertToFloat((getChildrenByTagName(xmlType, "quadratic_attenuation")[0]->GetText())));
				light->getSpotTo().set(Tools::convertToVector3((getChildrenByTagName(xmlType, "spot_to")[0]->GetText())));
				light->getSpotDirection().set(Tools::convertToVector3((getChildrenByTagName(xmlType, "spot_direction")[0]->GetText())));
				light->setSpotExponent(Tools::convertToFloat((getChildrenByTagName(xmlType, "spot_exponent")[0]->GetText())));
				light->setSpotCutOff(Tools::convertToFloat((getChildrenByTagName(xmlType, "spot_cutoff")[0]->GetText())));
				light->setEnabled(true);
				lightPresets[typeId] = light;
			}
		}
	}
}

LevelPropertyPresets::~LevelPropertyPresets() {
	for (auto mapPropertiesPresetEntity: mapPropertiesPreset) {
		delete mapPropertiesPresetEntity;
	}
	for (auto it = objectPropertiesPresets.begin(); it != objectPropertiesPresets.end(); ++it) {
		for (auto propertyModelClass: it->second) {
			delete propertyModelClass;
		}
	}
	for (auto it = lightPresets.begin(); it != lightPresets.end(); ++it) {
		delete it->second;
	}
}

LevelPropertyPresets* LevelPropertyPresets::getInstance()
{
	if (instance == nullptr) {
		try {
			instance = new LevelPropertyPresets("resources/engine/tools/leveleditor/gd", "presets.xml");
		} catch (Exception& exception) {
			Console::println(string(" LevelPropertyPresets::getInstance(): An error occurred: "));
			Console::print(string(exception.what()));
			Application::exit(1);
		}
	}
	return instance;
}

void LevelPropertyPresets::setDefaultLevelProperties(LevelEditorLevel* level)
{
	for (auto mapProperty: mapPropertiesPreset) {
		level->addProperty(mapProperty->getName(), mapProperty->getValue());
	}
}

const vector<TiXmlElement*> LevelPropertyPresets::getChildrenByTagName(TiXmlElement* parent, const char* name)
{
	vector<TiXmlElement*> elementList;
	for (auto *child = parent->FirstChildElement(name); child != nullptr; child = child->NextSiblingElement(name)) {
		elementList.push_back(child);
	}
	return elementList;
}
