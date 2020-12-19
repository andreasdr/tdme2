#include <tdme/engine/scene/ScenePropertyPresets.h>

#include <stdlib.h>

#include <map>
#include <string>
#include <vector>

#include <tdme/application/Application.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Color4Base.h>
#include <tdme/engine/prototype/PrototypeProperty.h>
#include <tdme/engine/scene/Scene.h>
#include <tdme/engine/scene/SceneLight.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Vector4.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/tools/shared/tools/Tools.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>

#include <ext/tinyxml/tinyxml.h>

using std::map;
using std::string;
using std::vector;

using tdme::application::Application;
using tdme::engine::model::Color4;
using tdme::engine::model::Color4Base;
using tdme::engine::prototype::PrototypeProperty;
using tdme::engine::scene::Scene;
using tdme::engine::scene::SceneLight;
using tdme::engine::scene::ScenePropertyPresets;
using tdme::math::Vector3;
using tdme::math::Vector4;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;
using tdme::tools::shared::tools::Tools;
using tdme::utilities::Console;
using tdme::utilities::Exception;
using tinyxml::TiXmlDocument;
using tinyxml::TiXmlElement;

ScenePropertyPresets* ScenePropertyPresets::instance = nullptr;

ScenePropertyPresets::ScenePropertyPresets(const string& pathName, const string& fileName)
{
	auto xmlContent = FileSystem::getInstance()->getContentAsString(pathName, fileName);
	TiXmlDocument xmlDocument;
	xmlDocument.Parse(xmlContent.c_str());
	if (xmlDocument.Error() == true) {
		Console::println(
			"ScenePropertyPresets::ScenePropertyPresets():: Could not parse file '" +
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
			scenePropertiesPreset.push_back(
				new PrototypeProperty(
					(xmlProperty->Attribute("name")),
					(xmlProperty->Attribute("value"))
				)
			);
		}
	}

	for (auto xmlObject: getChildrenByTagName(xmlRoot, "object")) {
		for (auto xmlType: getChildrenByTagName(xmlObject, "type")) {
			auto typeId = (xmlType->Attribute("id"));
			entityPropertiesPresets[typeId].push_back(new PrototypeProperty("preset", typeId));
			for (auto xmlProperty: getChildrenByTagName(xmlType, "property")) {
				entityPropertiesPresets[typeId].push_back(
					new PrototypeProperty(
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
				auto light = new SceneLight(lightId++);
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

ScenePropertyPresets::~ScenePropertyPresets() {
	for (auto scenePropertiesPresetEntity: scenePropertiesPreset) {
		delete scenePropertiesPresetEntity;
	}
	for (auto it = entityPropertiesPresets.begin(); it != entityPropertiesPresets.end(); ++it) {
		for (auto propertyModelClass: it->second) {
			delete propertyModelClass;
		}
	}
	for (auto it = lightPresets.begin(); it != lightPresets.end(); ++it) {
		delete it->second;
	}
}

ScenePropertyPresets* ScenePropertyPresets::getInstance()
{
	if (instance == nullptr) {
		try {
			instance = new ScenePropertyPresets("resources/engine/tools/sceneeditor/gd", "presets.xml");
		} catch (Exception& exception) {
			Console::println(string(" ScenePropertyPresets::getInstance(): An error occurred: "));
			Console::print(string(exception.what()));
			Application::exit(1);
		}
	}
	return instance;
}

void ScenePropertyPresets::setDefaultSceneProperties(Scene* scene)
{
	for (auto sceneProperty: scenePropertiesPreset) {
		scene->addProperty(sceneProperty->getName(), sceneProperty->getValue());
	}
}

const vector<TiXmlElement*> ScenePropertyPresets::getChildrenByTagName(TiXmlElement* parent, const char* name)
{
	vector<TiXmlElement*> elementList;
	for (auto *child = parent->FirstChildElement(name); child != nullptr; child = child->NextSiblingElement(name)) {
		elementList.push_back(child);
	}
	return elementList;
}
