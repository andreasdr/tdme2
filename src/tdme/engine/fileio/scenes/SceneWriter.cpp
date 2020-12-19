#include <tdme/engine/fileio/scenes/SceneWriter.h>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include <tdme/engine/fileio/prototypes/PrototypeWriter.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/RotationOrder.h>
#include <tdme/engine/prototype/Prototype.h>
#include <tdme/engine/prototype/Prototype_Type.h>
#include <tdme/engine/prototype/PrototypeProperty.h>
#include <tdme/engine/scene/Scene.h>
#include <tdme/engine/scene/SceneEntity.h>
#include <tdme/engine/scene/SceneLibrary.h>
#include <tdme/engine/scene/SceneLight.h>
#include <tdme/engine/Rotation.h>
#include <tdme/engine/Transformations.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Vector4.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

using std::ostringstream;
using std::string;

using tdme::engine::fileio::prototypes::PrototypeWriter;
using tdme::engine::fileio::scenes::SceneWriter;
using tdme::engine::model::Color4;
using tdme::engine::model::RotationOrder;
using tdme::engine::prototype::Prototype;
using tdme::engine::prototype::Prototype_Type;
using tdme::engine::prototype::PrototypeProperty;
using tdme::engine::scene::Scene;
using tdme::engine::scene::SceneEntity;
using tdme::engine::scene::SceneLibrary;
using tdme::engine::scene::SceneLight;
using tdme::engine::Rotation;
using tdme::engine::Transformations;
using tdme::math::Vector3;
using tdme::math::Vector4;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;

using rapidjson::Document;
using rapidjson::StringBuffer;
using rapidjson::Value;
using rapidjson::Writer;

void SceneWriter::write(const string& pathName, const string& fileName, Scene& scene)
{
	scene.setFileName(pathName + '/' + fileName);
	auto sceneLibrary = scene.getLibrary();
	Document jDocument;
	jDocument.SetObject();
	auto& jAllocator = jDocument.GetAllocator();
	jDocument.AddMember("version", Value("1.99", jAllocator), jAllocator);
	jDocument.AddMember("ro", Value(scene.getRotationOrder()->getName(), jAllocator), jAllocator);
	Value jLights;
	jLights.SetArray();
	for (auto i = 0; i < scene.getLightCount(); i++) {
		auto light = scene.getLightAt(i);
		Value jLight;
		jLight.SetObject();
		jLight.AddMember("id", Value(i), jAllocator);
		jLight.AddMember("ar", Value(light->getAmbient().getRed()), jAllocator);
		jLight.AddMember("ag", Value(light->getAmbient().getGreen()), jAllocator);
		jLight.AddMember("ab", Value(light->getAmbient().getBlue()), jAllocator);
		jLight.AddMember("aa", Value(light->getAmbient().getAlpha()), jAllocator);
		jLight.AddMember("dr", Value(light->getDiffuse().getRed()), jAllocator);
		jLight.AddMember("dg", Value(light->getDiffuse().getGreen()), jAllocator);
		jLight.AddMember("db", Value(light->getDiffuse().getBlue()), jAllocator);
		jLight.AddMember("da", Value(light->getDiffuse().getAlpha()), jAllocator);
		jLight.AddMember("sr", Value(light->getSpecular().getRed()), jAllocator);
		jLight.AddMember("sg", Value(light->getSpecular().getGreen()), jAllocator);
		jLight.AddMember("sb", Value(light->getSpecular().getBlue()), jAllocator);
		jLight.AddMember("sa", Value(light->getSpecular().getAlpha()), jAllocator);
		jLight.AddMember("px", Value(light->getPosition().getX()), jAllocator);
		jLight.AddMember("py", Value(light->getPosition().getY()), jAllocator);
		jLight.AddMember("pz", Value(light->getPosition().getZ()), jAllocator);
		jLight.AddMember("pw", Value(light->getPosition().getW()), jAllocator);
		jLight.AddMember("stx", Value(light->getSpotTo().getX()), jAllocator);
		jLight.AddMember("sty", Value(light->getSpotTo().getY()), jAllocator);
		jLight.AddMember("stz", Value(light->getSpotTo().getZ()), jAllocator);
		jLight.AddMember("sdx", Value(light->getSpotDirection().getX()), jAllocator);
		jLight.AddMember("sdy", Value(light->getSpotDirection().getY()), jAllocator);
		jLight.AddMember("sdz", Value(light->getSpotDirection().getZ()), jAllocator);
		jLight.AddMember("se", Value(light->getSpotExponent()), jAllocator);
		jLight.AddMember("sco", Value(light->getSpotCutOff()), jAllocator);
		jLight.AddMember("ca", Value(light->getConstantAttenuation()), jAllocator);
		jLight.AddMember("la", Value(light->getLinearAttenuation()), jAllocator);
		jLight.AddMember("qa", Value(light->getQuadraticAttenuation()), jAllocator);
		jLight.AddMember("e", Value(light->isEnabled()), jAllocator);
		jLights.PushBack(jLight, jAllocator);
	}
	jDocument.AddMember("lights", jLights, jAllocator);
	Value jSceneLibrary;
	jSceneLibrary.SetArray();
	for (auto i = 0; i < sceneLibrary->getPrototypeCount(); i++) {
		auto prototype = sceneLibrary->getPrototypeAt(i);
		Value jEntity;
		jEntity.SetObject();
		PrototypeWriter::write(jDocument, jEntity, prototype);
		Value jModel;
		jModel.SetObject();
		jModel.AddMember("id", Value().SetInt(prototype->getId()), jAllocator);
		jModel.AddMember("type", Value(prototype->getType()->getName(), jAllocator), jAllocator);
		jModel.AddMember("name", Value(prototype->getName(), jAllocator), jAllocator);
		jModel.AddMember("descr", Value(prototype->getDescription(), jAllocator), jAllocator);
		jModel.AddMember("entity", jEntity, jAllocator);
		jSceneLibrary.PushBack(jModel, jAllocator);
	}
	jDocument.AddMember("models", jSceneLibrary, jAllocator);
	Value jSceneProperties;
	jSceneProperties.SetArray();
	for (auto i = 0; i < scene.getPropertyCount(); i++) {
		PrototypeProperty* sceneProperty = scene.getPropertyByIndex(i);
		Value jSceneProperty;
		jSceneProperty.SetObject();
		jSceneProperty.AddMember("name", Value(sceneProperty->getName(), jAllocator), jAllocator);
		jSceneProperty.AddMember("value", Value(sceneProperty->getValue(), jAllocator), jAllocator);
		jSceneProperties.PushBack(jSceneProperty, jAllocator);
	}
	jDocument.AddMember("properties", jSceneProperties, jAllocator);
	Value jObjects;
	jObjects.SetArray();
	for (auto i = 0; i < scene.getEntityCount(); i++) {
		auto sceneEntity = scene.getEntityAt(i);
		Value jObject;
		jObject.SetObject();
		auto& transformations = sceneEntity->getTransformations();
		auto& translation = transformations.getTranslation();
		auto& scale = transformations.getScale();
		auto& rotationAroundXAxis = transformations.getRotation(scene.getRotationOrder()->getAxisXIndex());
		auto& rotationAroundYAxis = transformations.getRotation(scene.getRotationOrder()->getAxisYIndex());
		auto& rotationAroundZAxis = transformations.getRotation(scene.getRotationOrder()->getAxisZIndex());
		jObject.AddMember("id", Value(sceneEntity->getId(), jAllocator), jAllocator);
		jObject.AddMember("descr", Value(sceneEntity->getDescription(), jAllocator), jAllocator);;
		jObject.AddMember("mid", Value(sceneEntity->getPrototype()->getId()), jAllocator);
		jObject.AddMember("tx", Value(translation.getX()), jAllocator);
		jObject.AddMember("ty", Value(translation.getY()), jAllocator);
		jObject.AddMember("tz", Value(translation.getZ()), jAllocator);
		jObject.AddMember("sx", Value(scale.getX()), jAllocator);
		jObject.AddMember("sy", Value(scale.getY()), jAllocator);
		jObject.AddMember("sz", Value(scale.getZ()), jAllocator);
		jObject.AddMember("rx", Value(rotationAroundXAxis.getAngle()), jAllocator);
		jObject.AddMember("ry", Value(rotationAroundYAxis.getAngle()), jAllocator);
		jObject.AddMember("rz", Value(rotationAroundZAxis.getAngle()), jAllocator);
		jObject.AddMember("r", Value(sceneEntity->getReflectionEnvironmentMappingId(), jAllocator), jAllocator);
		Value jEntityProperties;
		jEntityProperties.SetArray();
		for (auto i = 0; i < sceneEntity->getPropertyCount(); i++) {
			PrototypeProperty* sceneEntityProperty = sceneEntity->getPropertyByIndex(i);
			Value jSceneEntityProperty;
			jSceneEntityProperty.SetObject();
			jSceneEntityProperty.AddMember("name", Value(sceneEntityProperty->getName(), jAllocator), jAllocator);
			jSceneEntityProperty.AddMember("value", Value(sceneEntityProperty->getValue(), jAllocator), jAllocator);
			jEntityProperties.PushBack(jSceneEntityProperty, jAllocator);
		}
		jObject.AddMember("properties", jEntityProperties, jAllocator);
		jObjects.PushBack(jObject, jAllocator);
	}
	jDocument.AddMember("objects", jObjects, jAllocator);
	jDocument.AddMember("objects_eidx", Value(scene.getEntityIdx()), jAllocator);

	Value jSky;
	jSky.SetObject();
	jSky.AddMember("file", Value(scene.getSkyModelFileName(), jAllocator), jAllocator);
	jSky.AddMember("sx", Value(scene.getSkyModelScale().getX()), jAllocator);
	jSky.AddMember("sy", Value(scene.getSkyModelScale().getY()), jAllocator);
	jSky.AddMember("sz", Value(scene.getSkyModelScale().getZ()), jAllocator);
	jDocument.AddMember("sky", jSky, jAllocator);

	StringBuffer strbuf;
	Writer<StringBuffer> writer(strbuf);
	jDocument.Accept(writer);

	FileSystem::getInstance()->setContentFromString(pathName, fileName, strbuf.GetString());
}
