#include <tdme/tools/shared/files/LevelFileExport.h>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

#include <tdme/engine/Rotation.h>
#include <tdme/engine/Transformations.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/RotationOrder.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Vector4.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/tools/shared/files/ModelMetaDataFileExport.h>
#include <tdme/tools/shared/model/LevelEditorEntity_EntityType.h>
#include <tdme/tools/shared/model/LevelEditorEntity.h>
#include <tdme/tools/shared/model/LevelEditorEntityLibrary.h>
#include <tdme/tools/shared/model/LevelEditorLevel.h>
#include <tdme/tools/shared/model/LevelEditorLight.h>
#include <tdme/tools/shared/model/LevelEditorObject.h>
#include <tdme/tools/shared/model/PropertyModelClass.h>

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

using std::ostringstream;
using std::string;

using tdme::tools::shared::files::LevelFileExport;
using tdme::engine::Rotation;
using tdme::engine::Transformations;
using tdme::engine::model::Color4;
using tdme::engine::model::RotationOrder;
using tdme::math::Vector3;
using tdme::math::Vector4;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;
using tdme::tools::shared::files::ModelMetaDataFileExport;
using tdme::tools::shared::model::LevelEditorEntity_EntityType;
using tdme::tools::shared::model::LevelEditorEntity;
using tdme::tools::shared::model::LevelEditorEntityLibrary;
using tdme::tools::shared::model::LevelEditorLevel;
using tdme::tools::shared::model::LevelEditorLight;
using tdme::tools::shared::model::LevelEditorObject;
using tdme::tools::shared::model::PropertyModelClass;

using rapidjson::Document;
using rapidjson::StringBuffer;
using rapidjson::Writer;
using rapidjson::Value;

void LevelFileExport::doExport(const string& pathName, const string& fileName, LevelEditorLevel& level)
{
	level.setFileName(pathName + '/' + fileName);
	auto entityLibrary = level.getEntityLibrary();
	Document jDocument;
	jDocument.SetObject();
	auto& jAllocator = jDocument.GetAllocator();
	jDocument.AddMember("version", Value("1.99", jAllocator), jAllocator);
	jDocument.AddMember("ro", Value(level.getRotationOrder()->getName(), jAllocator), jAllocator);
	Value jLights;
	jLights.SetArray();
	for (auto i = 0; i < level.getLightCount(); i++) {
		auto light = level.getLightAt(i);
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
	Value jEntityLibrary;
	jEntityLibrary.SetArray();
	for (auto i = 0; i < entityLibrary->getEntityCount(); i++) {
		auto entity = entityLibrary->getEntityAt(i);
		Value jEntity;
		jEntity.SetObject();
		ModelMetaDataFileExport::exportToJSON(jDocument, jEntity, entity);
		Value jModel;
		jModel.SetObject();
		jModel.AddMember("id", Value().SetInt(entity->getId()), jAllocator);
		jModel.AddMember("type", Value(entity->getType()->getName(), jAllocator), jAllocator);
		jModel.AddMember("name", Value(entity->getName(), jAllocator), jAllocator);
		jModel.AddMember("descr", Value(entity->getDescription(), jAllocator), jAllocator);
		jModel.AddMember("entity", jEntity, jAllocator);
		jEntityLibrary.PushBack(jModel, jAllocator);
	}
	jDocument.AddMember("models", jEntityLibrary, jAllocator);
	Value jMapProperties;
	jMapProperties.SetArray();
	for (auto i = 0; i < level.getPropertyCount(); i++) {
		PropertyModelClass* mapProperty = level.getPropertyByIndex(i);
		Value jMapProperty;
		jMapProperty.SetObject();
		jMapProperty.AddMember("name", Value(mapProperty->getName(), jAllocator), jAllocator);
		jMapProperty.AddMember("value", Value(mapProperty->getValue(), jAllocator), jAllocator);
		jMapProperties.PushBack(jMapProperty, jAllocator);
	}
	jDocument.AddMember("properties", jMapProperties, jAllocator);
	Value jObjects;
	jObjects.SetArray();
	for (auto i = 0; i < level.getObjectCount(); i++) {
		auto levelEditorObject = level.getObjectAt(i);
		Value jObject;
		jObject.SetObject();
		auto& transformations = levelEditorObject->getTransformations();
		auto& translation = transformations.getTranslation();
		auto& scale = transformations.getScale();
		auto& rotationAroundXAxis = transformations.getRotation(level.getRotationOrder()->getAxisXIndex());
		auto& rotationAroundYAxis = transformations.getRotation(level.getRotationOrder()->getAxisYIndex());
		auto& rotationAroundZAxis = transformations.getRotation(level.getRotationOrder()->getAxisZIndex());
		jObject.AddMember("id", Value(levelEditorObject->getId(), jAllocator), jAllocator);
		jObject.AddMember("descr", Value(levelEditorObject->getDescription(), jAllocator), jAllocator);;
		jObject.AddMember("mid", Value(levelEditorObject->getEntity()->getId()), jAllocator);
		jObject.AddMember("tx", Value(translation.getX()), jAllocator);
		jObject.AddMember("ty", Value(translation.getY()), jAllocator);
		jObject.AddMember("tz", Value(translation.getZ()), jAllocator);
		jObject.AddMember("sx", Value(scale.getX()), jAllocator);
		jObject.AddMember("sy", Value(scale.getY()), jAllocator);
		jObject.AddMember("sz", Value(scale.getZ()), jAllocator);
		jObject.AddMember("rx", Value(rotationAroundXAxis.getAngle()), jAllocator);
		jObject.AddMember("ry", Value(rotationAroundYAxis.getAngle()), jAllocator);
		jObject.AddMember("rz", Value(rotationAroundZAxis.getAngle()), jAllocator);
		jObject.AddMember("r", Value(levelEditorObject->getReflectionEnvironmentMappingId(), jAllocator), jAllocator);
		Value jObjectProperties;
		jObjectProperties.SetArray();
		for (auto i = 0; i < levelEditorObject->getPropertyCount(); i++) {
			PropertyModelClass* objectProperty = levelEditorObject->getPropertyByIndex(i);
			Value jObjectProperty;
			jObjectProperty.SetObject();
			jObjectProperty.AddMember("name", Value(objectProperty->getName(), jAllocator), jAllocator);
			jObjectProperty.AddMember("value", Value(objectProperty->getValue(), jAllocator), jAllocator);
			jObjectProperties.PushBack(jObjectProperty, jAllocator);
		}
		jObject.AddMember("properties", jObjectProperties, jAllocator);
		jObjects.PushBack(jObject, jAllocator);
	}
	jDocument.AddMember("objects", jObjects, jAllocator);
	jDocument.AddMember("objects_eidx", Value(level.getObjectIdx()), jAllocator);

	Value jSky;
	jSky.SetObject();
	jSky.AddMember("file", Value(level.getSkyModelFileName(), jAllocator), jAllocator);
	jSky.AddMember("sx", Value(level.getSkyModelScale().getX()), jAllocator);
	jSky.AddMember("sy", Value(level.getSkyModelScale().getY()), jAllocator);
	jSky.AddMember("sz", Value(level.getSkyModelScale().getZ()), jAllocator);
	jDocument.AddMember("sky", jSky, jAllocator);

	StringBuffer strbuf;
	Writer<StringBuffer> writer(strbuf);
	jDocument.Accept(writer);

	FileSystem::getInstance()->setContentFromString(pathName, fileName, strbuf.GetString());
}
