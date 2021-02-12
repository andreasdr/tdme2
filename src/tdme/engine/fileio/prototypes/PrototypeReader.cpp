#include <tdme/engine/fileio/prototypes/PrototypeReader.h>

#include <string>

#include <tdme/engine/fileio/models/ModelFileIOException.h>
#include <tdme/engine/fileio/models/ModelReader.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/prototype/Prototype.h>
#include <tdme/engine/prototype/Prototype_Type.h>
#include <tdme/engine/prototype/PrototypeAudio.h>
#include <tdme/engine/prototype/PrototypeBoundingVolume.h>
#include <tdme/engine/prototype/PrototypeLODLevel.h>
#include <tdme/engine/prototype/PrototypeParticleSystem.h>
#include <tdme/engine/prototype/PrototypeParticleSystem_BoundingBoxParticleEmitter.h>
#include <tdme/engine/prototype/PrototypeParticleSystem_CircleParticleEmitter.h>
#include <tdme/engine/prototype/PrototypeParticleSystem_CircleParticleEmitterPlaneVelocity.h>
#include <tdme/engine/prototype/PrototypeParticleSystem_Emitter.h>
#include <tdme/engine/prototype/PrototypeParticleSystem_FogParticleSystem.h>
#include <tdme/engine/prototype/PrototypeParticleSystem_ObjectParticleSystem.h>
#include <tdme/engine/prototype/PrototypeParticleSystem_PointParticleEmitter.h>
#include <tdme/engine/prototype/PrototypeParticleSystem_PointParticleSystem.h>
#include <tdme/engine/prototype/PrototypeParticleSystem_SphereParticleEmitter.h>
#include <tdme/engine/prototype/PrototypeParticleSystem_Type.h>
#include <tdme/engine/prototype/PrototypePhysics.h>
#include <tdme/engine/prototype/PrototypePhysics_BodyType.h>
#include <tdme/engine/prototype/PrototypeTerrain.h>
#include <tdme/engine/EntityShaderParameters.h>
#include <tdme/engine/LODObject3D.h>
#include <tdme/engine/ShaderParameter.h>
#include <tdme/math/Vector3.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemException.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/tools/shared/tools/Tools.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>
#include <tdme/utilities/Float.h>
#include <tdme/utilities/ModelTools.h>
#include <tdme/utilities/StringTools.h>

#include <rapidjson/document.h>

using std::string;

using tdme::engine::fileio::models::ModelFileIOException;
using tdme::engine::fileio::models::ModelReader;
using tdme::engine::fileio::prototypes::PrototypeReader;
using tdme::engine::model::Color4;
using tdme::engine::model::Model;
using tdme::engine::prototype::Prototype;
using tdme::engine::prototype::Prototype_Type;
using tdme::engine::prototype::PrototypeAudio;
using tdme::engine::prototype::PrototypeBoundingVolume;
using tdme::engine::prototype::PrototypeParticleSystem;
using tdme::engine::prototype::PrototypeParticleSystem_BoundingBoxParticleEmitter;
using tdme::engine::prototype::PrototypeParticleSystem_CircleParticleEmitter;
using tdme::engine::prototype::PrototypeParticleSystem_CircleParticleEmitterPlaneVelocity;
using tdme::engine::prototype::PrototypeParticleSystem_Emitter;
using tdme::engine::prototype::PrototypeParticleSystem_FogParticleSystem;
using tdme::engine::prototype::PrototypeParticleSystem_ObjectParticleSystem;
using tdme::engine::prototype::PrototypeParticleSystem_PointParticleEmitter;
using tdme::engine::prototype::PrototypeParticleSystem_PointParticleSystem;
using tdme::engine::prototype::PrototypeParticleSystem_SphereParticleEmitter;
using tdme::engine::prototype::PrototypeParticleSystem_Type;
using tdme::engine::prototype::PrototypePhysics;
using tdme::engine::prototype::PrototypePhysics_BodyType;
using tdme::engine::prototype::PrototypeTerrain;
using tdme::engine::EntityShaderParameters;
using tdme::engine::LODObject3D;
using tdme::engine::ShaderParameter;
using tdme::math::Vector3;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemException;
using tdme::os::filesystem::FileSystemInterface;
using tdme::tools::shared::tools::Tools;
using tdme::utilities::Console;
using tdme::utilities::Exception;
using tdme::utilities::Float;
using tdme::utilities::ModelTools;
using tdme::utilities::StringTools;

using rapidjson::Document;
using rapidjson::Value;

Prototype* PrototypeReader::read(int id, const string& pathName, const string& fileName)
{
	auto jsonContent = FileSystem::getInstance()->getContentAsString(pathName, fileName);

	Document jEntityRoot;
	jEntityRoot.Parse(jsonContent.c_str());

	auto prototype = read(id, pathName, jEntityRoot);
	prototype->setFileName(pathName + "/" + fileName);
	return prototype;
}

Prototype* PrototypeReader::read(int id, const string& pathName, Value& jEntityRoot)
{
	Prototype* prototype;
	// auto version = Float::parseFloat((jEntityRoot["version"].GetString()));
	auto pivot = Vector3(
		static_cast<float>(jEntityRoot["px"].GetFloat()),
		static_cast<float>(jEntityRoot["py"].GetFloat()),
		static_cast<float>(jEntityRoot["pz"].GetFloat())
	);
	auto prototypeType = Prototype_Type::valueOf((jEntityRoot["type"].GetString()));
	auto modelThumbnail = jEntityRoot.FindMember("thumbnail") != jEntityRoot.MemberEnd()? (jEntityRoot["thumbnail"].GetString()) : "";
	auto name = (jEntityRoot["name"].GetString());
	auto description = (jEntityRoot["descr"].GetString());
	string modelFileName = "";
	string modelPathName = "";
	if (jEntityRoot.FindMember("file") != jEntityRoot.MemberEnd()) {
		modelFileName = (jEntityRoot["file"].GetString());
	}
	Model* model = nullptr;
	if (modelFileName.length() > 0) {
		modelPathName = getResourcePathName(pathName, modelFileName);
		model = ModelReader::read(
			modelPathName,
			FileSystem::getInstance()->getFileName(modelFileName)
		);
	} else
	if (prototypeType == Prototype_Type::EMPTY) {
		model = ModelReader::read("resources/engine/models", "empty.tm");
	}

	prototype = new Prototype(
		id,
		prototypeType,
		name,
		description,
		"",
		modelFileName.length() > 0 ? modelPathName + "/" + FileSystem::getInstance()->getFileName(modelFileName) : "",
		modelThumbnail,
		model,
		pivot
	);
	auto jProperties = jEntityRoot["properties"].GetArray();
	for (auto i = 0; i < jProperties.Size(); i++) {
		auto& jProperty = jProperties[i];
		prototype->addProperty(
			(jProperty["name"].GetString()),
			(jProperty["value"].GetString())
		);
	}

	if (jEntityRoot.FindMember("bv") != jEntityRoot.MemberEnd()) {
		auto boundingVolume = parseBoundingVolume(
			0,
			prototype,
			pathName,
			jEntityRoot["bv"]
		);
		if (boundingVolume->getBoundingVolume() != nullptr) prototype->addBoundingVolume(0, boundingVolume);
	} else
	if (jEntityRoot.FindMember("bvs") != jEntityRoot.MemberEnd()) {
		auto jBoundingVolumes = jEntityRoot["bvs"].GetArray();
		auto bvIdx = 0;
		for (auto i = 0; i < jBoundingVolumes.Size(); i++) {
			auto boundingVolume = parseBoundingVolume(bvIdx, prototype, pathName, jBoundingVolumes[i]);
			if (boundingVolume->getBoundingVolume() != nullptr) prototype->addBoundingVolume(bvIdx++, boundingVolume);
		}
	}
	if (jEntityRoot.FindMember("p") != jEntityRoot.MemberEnd() && prototype->getPhysics() != nullptr) {
		auto physics = prototype->getPhysics();
		auto& jPhysics = jEntityRoot["p"];
		physics->setType(PrototypePhysics_BodyType::valueOf(jPhysics["type"].GetString()));
		physics->setMass(static_cast<float>(jPhysics["mass"].GetFloat()));
		physics->setRestitution(static_cast<float>(jPhysics["restitution"].GetFloat()));
		physics->setFriction(static_cast<float>(jPhysics["friction"].GetFloat()));
		physics->setInertiaTensor(
			Vector3(
				static_cast<float>(jPhysics["itx"].GetFloat()),
				static_cast<float>(jPhysics["ity"].GetFloat()),
				static_cast<float>(jPhysics["itz"].GetFloat())
			)
		);
	}
	if (jEntityRoot.FindMember("sd") != jEntityRoot.MemberEnd()) {
		for (auto& jSound: jEntityRoot["sd"].GetArray()) {
			auto id = jSound["i"].GetString();
			auto sound = prototype->addSound(id);
			if (sound == nullptr) continue;
			sound->setAnimation(jSound["a"].GetString());
			sound->setFileName(jSound["file"].GetString());
			sound->setGain(static_cast<float>(jSound["g"].GetFloat()));
			sound->setPitch(static_cast<float>(jSound["p"].GetFloat()));
			sound->setOffset(static_cast<float>(jSound["o"].GetInt()));
			sound->setLooping(jSound["l"].GetBool());
			sound->setFixed(jSound["f"].GetBool());
		}
	}
	if (prototypeType == Prototype_Type::MODEL) {
		prototype->setTerrainMesh(jEntityRoot["tm"].GetBool());
		if (jEntityRoot.FindMember("ll2") != jEntityRoot.MemberEnd()) prototype->setLODLevel2(parseLODLevel(pathName, jEntityRoot["ll2"]));
		if (jEntityRoot.FindMember("ll3") != jEntityRoot.MemberEnd()) prototype->setLODLevel3(parseLODLevel(pathName, jEntityRoot["ll3"]));
	} else
	if (prototypeType == Prototype_Type::PARTICLESYSTEM) {
		if (jEntityRoot.FindMember("ps") != jEntityRoot.MemberEnd()) {
			prototype->addParticleSystem();
			parseParticleSystem(prototype->getParticleSystemAt(0), pathName, jEntityRoot["ps"]);
		} else
		if (jEntityRoot.FindMember("pss") != jEntityRoot.MemberEnd()) {
			auto jParticleSystems = jEntityRoot["pss"].GetArray();
			for (auto i = 0; i < jParticleSystems.Size(); i++) {
				prototype->addParticleSystem();
				parseParticleSystem(prototype->getParticleSystemAt(prototype->getParticleSystemsCount() - 1), pathName, jParticleSystems[i]);
			}
		}
	}
	if (jEntityRoot.FindMember("ds") != jEntityRoot.MemberEnd()) {
		prototype->setContributesShadows(jEntityRoot["ds"].GetBool());
		prototype->setReceivesShadows(jEntityRoot["ds"].GetBool());
	} else
	if (jEntityRoot.FindMember("cs") != jEntityRoot.MemberEnd() && jEntityRoot.FindMember("rs") != jEntityRoot.MemberEnd()) {
		prototype->setContributesShadows(jEntityRoot["cs"].GetBool());
		prototype->setReceivesShadows(jEntityRoot["rs"].GetBool());
	}
	prototype->setRenderGroups(jEntityRoot.FindMember("rg") != jEntityRoot.MemberEnd()?jEntityRoot["rg"].GetBool():false);
	prototype->setShader(jEntityRoot.FindMember("s") != jEntityRoot.MemberEnd()?jEntityRoot["s"].GetString():"default");
	prototype->setDistanceShader(jEntityRoot.FindMember("sds") != jEntityRoot.MemberEnd()?jEntityRoot["sds"].GetString():"default");
	prototype->setDistanceShaderDistance(jEntityRoot.FindMember("sdsd") != jEntityRoot.MemberEnd()?static_cast<float>(jEntityRoot["sdsd"].GetFloat()):10000.0f);
	if (jEntityRoot.FindMember("sps") != jEntityRoot.MemberEnd()) {
		Value& jShaderParameters = jEntityRoot["sps"];
		EntityShaderParameters shaderParameters;
		shaderParameters.setShader(prototype->getShader());
		for (auto jShaderParameterIt = jShaderParameters.MemberBegin(); jShaderParameterIt != jShaderParameters.MemberEnd(); ++jShaderParameterIt) {
			shaderParameters.setShaderParameter(jShaderParameterIt->name.GetString(), jShaderParameterIt->value.GetString());
		}
		prototype->setShaderParameters(shaderParameters);
	}
	if (jEntityRoot.FindMember("spds") != jEntityRoot.MemberEnd()) {
		Value& jDistanceShaderParameters = jEntityRoot["spds"];
		EntityShaderParameters distanceShaderParameters;
		distanceShaderParameters.setShader(prototype->getDistanceShader());
		for (auto jDistanceShaderParameterIt = jDistanceShaderParameters.MemberBegin(); jDistanceShaderParameterIt != jDistanceShaderParameters.MemberEnd(); ++jDistanceShaderParameterIt) {
			distanceShaderParameters.setShaderParameter(jDistanceShaderParameterIt->name.GetString(), jDistanceShaderParameterIt->value.GetString());
		}
		prototype->setDistanceShaderParameters(distanceShaderParameters);
	}
	if (prototype->getType() == Prototype_Type::ENVIRONMENTMAPPING) {
		prototype->setEnvironmentMapRenderPassMask(jEntityRoot["emrpm"].GetInt());
		prototype->setEnvironmentMapTimeRenderUpdateFrequency(jEntityRoot["emtf"].GetInt64());
	}
	//
	if (prototype->getModel() != nullptr) ModelTools::prepareForShader(prototype->getModel(), prototype->getShader());

	if (prototype->getType() == Prototype_Type::TERRAIN) {
		auto terrain = prototype->getTerrain();
		Value& jTerrain = jEntityRoot["t"];
		terrain->setWidth(jTerrain["w"].GetFloat());
		terrain->setDepth(jTerrain["d"].GetFloat());
		Value jTerrainValues = jTerrain["v"].GetArray(); // TODO: how to avoid this copy???
		for (auto i = 0; i < jTerrainValues.Size(); i++) terrain->getHeightVector().push_back(jTerrainValues[i].GetFloat());
	}

	return prototype;
}

const string PrototypeReader::getResourcePathName(const string& pathName, const string& fileName) {
	string modelFile = FileSystem::getInstance()->getCanonicalPath(
		(
			StringTools::startsWith(FileSystem::getInstance()->getPathName(fileName), "/") == true?
				FileSystem::getInstance()->getPathName(fileName):
				pathName + "/" +  FileSystem::getInstance()->getPathName(fileName)
		 ),
		FileSystem::getInstance()->getFileName(fileName)
	);
	auto applicationRoot = Tools::getApplicationRootPathName(pathName);
	auto modelRelativeFileName = Tools::getRelativeResourcesFileName(applicationRoot, modelFile);
	return (applicationRoot.length() > 0 ? applicationRoot + "/" : "") + Tools::getPathName(modelRelativeFileName);
}

PrototypeBoundingVolume* PrototypeReader::parseBoundingVolume(int idx, Prototype* prototype, const string& pathName, Value& jBv)
{
	auto entityBoundingVolume = new PrototypeBoundingVolume(idx, prototype);
	BoundingVolume* bv;
	auto bvTypeString = (jBv["type"].GetString());
	if (StringTools::equalsIgnoreCase(bvTypeString, "none") == true) {
		entityBoundingVolume->setupNone();
	} else
	if (StringTools::equalsIgnoreCase(bvTypeString, "sphere") == true) {
		entityBoundingVolume->setupSphere(
			Vector3(
				static_cast<float>(jBv["cx"].GetFloat()),
				static_cast<float>(jBv["cy"].GetFloat()),
				static_cast<float>(jBv["cz"].GetFloat())
			),
			static_cast<float>(jBv["r"].GetFloat())
		);
	} else
	if (StringTools::equalsIgnoreCase(bvTypeString, "capsule") == true) {
		entityBoundingVolume->setupCapsule(
			Vector3(
				static_cast<float>(jBv["ax"].GetFloat()),
				static_cast<float>(jBv["ay"].GetFloat()),
				static_cast<float>(jBv["az"].GetFloat())
			),
			Vector3(
				static_cast<float>(jBv["bx"].GetFloat()),
				static_cast<float>(jBv["by"].GetFloat()),
				static_cast<float>(jBv["bz"].GetFloat())
			),
			static_cast<float>(jBv["r"].GetFloat())
		);
	} else
	if (StringTools::equalsIgnoreCase(bvTypeString, "aabb") == true) {
		entityBoundingVolume->setupAabb(
			Vector3(
				static_cast<float>(jBv["mix"].GetFloat()),
				static_cast<float>(jBv["miy"].GetFloat()),
				static_cast<float>(jBv["miz"].GetFloat())
			),
			Vector3(
				static_cast<float>(jBv["max"].GetFloat()),
				static_cast<float>(jBv["may"].GetFloat()),
				static_cast<float>(jBv["maz"].GetFloat())
			)
		);
	} else
	if (StringTools::equalsIgnoreCase(bvTypeString, "obb") == true) {
		entityBoundingVolume->setupObb(
			Vector3(
				static_cast<float>(jBv["cx"].GetFloat()),
				static_cast<float>(jBv["cy"].GetFloat()),
				static_cast<float>(jBv["cz"].GetFloat())
			),
			Vector3(
				static_cast<float>(jBv["a0x"].GetFloat()),
				static_cast<float>(jBv["a0y"].GetFloat()),
				static_cast<float>(jBv["a0z"].GetFloat())
			),
			Vector3(
				static_cast<float>(jBv["a1x"].GetFloat()),
				static_cast<float>(jBv["a1y"].GetFloat()),
				static_cast<float>(jBv["a1z"].GetFloat())
			),
			Vector3(
				static_cast<float>(jBv["a2x"].GetFloat()),
				static_cast<float>(jBv["a2y"].GetFloat()),
				static_cast<float>(jBv["a2z"].GetFloat())
			),
			Vector3(
				static_cast<float>(jBv["hex"].GetFloat()),
				static_cast<float>(jBv["hey"].GetFloat()),
				static_cast<float>(jBv["hez"].GetFloat())
			)
		);
	} else
	if (StringTools::equalsIgnoreCase(bvTypeString, "convexmesh") == true) {
		try {
			string fileName = jBv["file"].GetString();
			entityBoundingVolume->setupConvexMesh(
				getResourcePathName(pathName, fileName),
				Tools::getFileName(fileName)
			);
		} catch (Exception& exception) {
			Console::print(string("PrototypeReader::parseBoundingVolume(): An error occurred: "));
			Console::println(string(exception.what()));
		}
	}
	return entityBoundingVolume;
}

PrototypeLODLevel* PrototypeReader::parseLODLevel(const string& pathName, Value& jLodLevel) {
	auto lodType = static_cast<LODObject3D::LODLevelType>(jLodLevel["t"].GetInt());
	PrototypeLODLevel* lodLevel = new PrototypeLODLevel(
		lodType,
		lodType == LODObject3D::LODLEVELTYPE_MODEL?jLodLevel["f"].GetString():"",
		nullptr,
		static_cast<float>(jLodLevel["d"].GetFloat())
	);
	if (lodType == LODObject3D::LODLEVELTYPE_MODEL) {
		auto modelFileName = lodLevel->getFileName();
		auto modelPathName = getResourcePathName(pathName, modelFileName);
		lodLevel->setModel(
			ModelReader::read(
				modelPathName,
				FileSystem::getInstance()->getFileName(modelFileName)
			)
		);
		lodLevel->setFileName(modelPathName + "/" + FileSystem::getInstance()->getFileName(modelFileName));
	}
	lodLevel->setColorAdd(
		Color4(
			static_cast<float>(jLodLevel["car"].GetFloat()),
			static_cast<float>(jLodLevel["cag"].GetFloat()),
			static_cast<float>(jLodLevel["cab"].GetFloat()),
			static_cast<float>(jLodLevel["caa"].GetFloat())
		)
	);
	lodLevel->setColorMul(
		Color4(
			static_cast<float>(jLodLevel["cmr"].GetFloat()),
			static_cast<float>(jLodLevel["cmg"].GetFloat()),
			static_cast<float>(jLodLevel["cmb"].GetFloat()),
			static_cast<float>(jLodLevel["cma"].GetFloat())
		)
	);
	return lodLevel;
}

void PrototypeReader::parseParticleSystem(PrototypeParticleSystem* particleSystem, const string& pathName, Value& jParticleSystem) {
	particleSystem->setType(PrototypeParticleSystem_Type::valueOf((jParticleSystem["t"].GetString())));
	{
		auto v = particleSystem->getType();
		if (v == PrototypeParticleSystem_Type::NONE) {
			// no op
		} else
		if (v == PrototypeParticleSystem_Type::OBJECT_PARTICLE_SYSTEM) {
			auto& jObjectParticleSystem = jParticleSystem["ops"];
			auto objectParticleSystem = particleSystem->getObjectParticleSystem();
			objectParticleSystem->setMaxCount(jObjectParticleSystem["mc"].GetInt());
			objectParticleSystem->getScale().setX(static_cast<float>(jObjectParticleSystem["sx"].GetFloat()));
			objectParticleSystem->getScale().setY(static_cast<float>(jObjectParticleSystem["sy"].GetFloat()));
			objectParticleSystem->getScale().setZ(static_cast<float>(jObjectParticleSystem["sz"].GetFloat()));
			objectParticleSystem->setAutoEmit(jObjectParticleSystem["ae"].GetBool());
			try {
				auto particleModelFile = (jObjectParticleSystem["mf"].GetString());
				auto particleModelPath = getResourcePathName(pathName, particleModelFile);
				objectParticleSystem->setModelFile(
					particleModelPath + "/" + Tools::getFileName(particleModelFile)
				);
			} catch (Exception& exception) {
				Console::print(string("PrototypeReader::doImport(): An error occurred: "));
				Console::println(string(exception.what()));
			}
		} else
		if (v == PrototypeParticleSystem_Type::POINT_PARTICLE_SYSTEM) {
			auto pointParticleSystem = particleSystem->getPointParticleSystem();
			auto& jPointParticleSystem = jParticleSystem["pps"];
			pointParticleSystem->setMaxPoints(jPointParticleSystem["mp"].GetInt());
			if (jPointParticleSystem.FindMember("ps") != jPointParticleSystem.MemberEnd()) pointParticleSystem->setPointSize(static_cast<float>(jPointParticleSystem["ps"].GetFloat()));
			if (jPointParticleSystem.FindMember("t") != jPointParticleSystem.MemberEnd()) {
				try {
					auto particleTextureFileName = jPointParticleSystem["t"].GetString();
					auto particleTexturePathName = getResourcePathName(pathName, particleTextureFileName);
					auto particleTransparencyTextureFileName = string(jPointParticleSystem.FindMember("tt") != jPointParticleSystem.MemberEnd()?jPointParticleSystem["tt"].GetString():"");
					auto particleTransparencyTexturePathName = particleTransparencyTextureFileName.size() == 0?string():getResourcePathName(pathName, particleTransparencyTextureFileName);
					pointParticleSystem->setTextureFileName(
						particleTexturePathName + "/" + Tools::getFileName(particleTextureFileName),
						particleTransparencyTextureFileName.size() == 0?string():particleTransparencyTexturePathName + "/" + Tools::getFileName(particleTransparencyTextureFileName)
					);
					if (jPointParticleSystem.FindMember("ths") != jPointParticleSystem.MemberEnd()) pointParticleSystem->setTextureHorizontalSprites(jPointParticleSystem["ths"].GetInt());
					if (jPointParticleSystem.FindMember("tvs") != jPointParticleSystem.MemberEnd()) pointParticleSystem->setTextureVerticalSprites(jPointParticleSystem["tvs"].GetInt());
					if (jPointParticleSystem.FindMember("fps") != jPointParticleSystem.MemberEnd()) pointParticleSystem->setTextureSpritesFPS(jPointParticleSystem["fps"].GetFloat());
				} catch (Exception& exception) {
					Console::print(string("PrototypeReader::doImport(): An error occurred: "));
					Console::println(string(exception.what()));
				}
			}
			pointParticleSystem->setAutoEmit(jPointParticleSystem["ae"].GetBool());
		} else
		if (v == PrototypeParticleSystem_Type::FOG_PARTICLE_SYSTEM) {
			auto fogParticleSystem = particleSystem->getFogParticleSystem();
			auto& jFogParticleSystem = jParticleSystem["fps"];
			fogParticleSystem->setMaxPoints(jFogParticleSystem["mp"].GetInt());
			if (jFogParticleSystem.FindMember("ps") != jFogParticleSystem.MemberEnd()) fogParticleSystem->setPointSize(static_cast<float>(jFogParticleSystem["ps"].GetFloat()));
			if (jFogParticleSystem.FindMember("t") != jFogParticleSystem.MemberEnd()) {
				try {
					auto particleTextureFileName = jFogParticleSystem["t"].GetString();
					auto particleTexturePathName = getResourcePathName(pathName, particleTextureFileName);
					auto particleTransparencyTextureFileName = jFogParticleSystem.FindMember("tt") != jFogParticleSystem.MemberEnd()?string():jFogParticleSystem["tt"].GetString();
					auto particleTransparencyTexturePathName = particleTransparencyTextureFileName.size() == 0?string():getResourcePathName(pathName, particleTransparencyTextureFileName);
					fogParticleSystem->setTextureFileName(
						particleTexturePathName + "/" + Tools::getFileName(particleTextureFileName),
						particleTransparencyTextureFileName.size() == 0?string():particleTransparencyTexturePathName + "/" + Tools::getFileName(particleTransparencyTextureFileName)
					);
					if (jFogParticleSystem.FindMember("ths") != jFogParticleSystem.MemberEnd()) fogParticleSystem->setTextureHorizontalSprites(jFogParticleSystem["ths"].GetInt());
					if (jFogParticleSystem.FindMember("tvs") != jFogParticleSystem.MemberEnd()) fogParticleSystem->setTextureVerticalSprites(jFogParticleSystem["tvs"].GetInt());
					if (jFogParticleSystem.FindMember("fps") != jFogParticleSystem.MemberEnd()) fogParticleSystem->setTextureSpritesFPS(jFogParticleSystem["fps"].GetFloat());
				} catch (Exception& exception) {
					Console::print(string("PrototypeReader::doImport(): An error occurred: "));
					Console::println(string(exception.what()));
				}
			}
		} else {
			Console::println(
				string(
					 "PrototypeWriter::export(): unknown particle system type '" +
					 particleSystem->getType()->getName() +
					 "'"
				 )
			 );
		}
	}

	particleSystem->setEmitter(PrototypeParticleSystem_Emitter::valueOf((jParticleSystem["e"].GetString())));
	{
		auto v = particleSystem->getEmitter();
		if (v == PrototypeParticleSystem_Emitter::NONE) {
			// no op
		} else
		if (v == PrototypeParticleSystem_Emitter::POINT_PARTICLE_EMITTER) {
			auto& jPointParticleEmitter = jParticleSystem["ppe"];
			auto emitter = particleSystem->getPointParticleEmitter();
			emitter->setCount(jPointParticleEmitter["c"].GetInt());
			emitter->setLifeTime(jPointParticleEmitter["lt"].GetInt());
			emitter->setLifeTimeRnd(jPointParticleEmitter["ltrnd"].GetInt());
			emitter->setMass(static_cast<float>(jPointParticleEmitter["m"].GetFloat()));
			emitter->setMassRnd(static_cast<float>(jPointParticleEmitter["mrnd"].GetFloat()));
			emitter->setPosition(
				Vector3(
					static_cast<float>(jPointParticleEmitter["px"].GetFloat()),
					static_cast<float>(jPointParticleEmitter["py"].GetFloat()),
					static_cast<float>(jPointParticleEmitter["pz"].GetFloat())
				)
			);
			emitter->setVelocity(
				Vector3(
					static_cast<float>(jPointParticleEmitter["vx"].GetFloat()),
					static_cast<float>(jPointParticleEmitter["vy"].GetFloat()),
					static_cast<float>(jPointParticleEmitter["vz"].GetFloat())
				)
			);
			emitter->setVelocityRnd(
				Vector3(
					static_cast<float>(jPointParticleEmitter["vrndx"].GetFloat()),
					static_cast<float>(jPointParticleEmitter["vrndy"].GetFloat()),
					static_cast<float>(jPointParticleEmitter["vrndz"].GetFloat())
				)
			);
			emitter->setColorStart(
				Color4(
					static_cast<float>(jPointParticleEmitter["csr"].GetFloat()),
					static_cast<float>(jPointParticleEmitter["csg"].GetFloat()),
					static_cast<float>(jPointParticleEmitter["csb"].GetFloat()),
					static_cast<float>(jPointParticleEmitter["csa"].GetFloat())
				)
			);
			emitter->setColorEnd(
				Color4(
					static_cast<float>(jPointParticleEmitter["cer"].GetFloat()),
					static_cast<float>(jPointParticleEmitter["ceg"].GetFloat()),
					static_cast<float>(jPointParticleEmitter["ceb"].GetFloat()),
					static_cast<float>(jPointParticleEmitter["cea"].GetFloat())
				)
			);
		} else
		if (v == PrototypeParticleSystem_Emitter::BOUNDINGBOX_PARTICLE_EMITTER) {
			auto& jBoundingBoxParticleEmitter = jParticleSystem["bbpe"];
			auto emitter = particleSystem->getBoundingBoxParticleEmitters();
			emitter->setCount(jBoundingBoxParticleEmitter["c"].GetInt());
			emitter->setLifeTime(jBoundingBoxParticleEmitter["lt"].GetInt());
			emitter->setLifeTimeRnd(jBoundingBoxParticleEmitter["ltrnd"].GetInt());
			emitter->setMass(static_cast<float>(jBoundingBoxParticleEmitter["m"].GetFloat()));
			emitter->setMassRnd(static_cast<float>(jBoundingBoxParticleEmitter["mrnd"].GetFloat()));
			emitter->setVelocity(
				Vector3(
					static_cast<float>(jBoundingBoxParticleEmitter["vx"].GetFloat()),
					static_cast<float>(jBoundingBoxParticleEmitter["vy"].GetFloat()),
					static_cast<float>(jBoundingBoxParticleEmitter["vz"].GetFloat())
				)
			);
			emitter->setVelocityRnd(
				Vector3(
					static_cast<float>(jBoundingBoxParticleEmitter["vrndx"].GetFloat()),
					static_cast<float>(jBoundingBoxParticleEmitter["vrndy"].GetFloat()),
					static_cast<float>(jBoundingBoxParticleEmitter["vrndz"].GetFloat())
				)
			);
			emitter->setColorStart(
				Color4(
					static_cast<float>(jBoundingBoxParticleEmitter["csr"].GetFloat()),
					static_cast<float>(jBoundingBoxParticleEmitter["csg"].GetFloat()),
					static_cast<float>(jBoundingBoxParticleEmitter["csb"].GetFloat()),
					static_cast<float>(jBoundingBoxParticleEmitter["csa"].GetFloat())
				)
			);
			emitter->setColorEnd(
				Color4(
					static_cast<float>(jBoundingBoxParticleEmitter["cer"].GetFloat()),
					static_cast<float>(jBoundingBoxParticleEmitter["ceg"].GetFloat()),
					static_cast<float>(jBoundingBoxParticleEmitter["ceb"].GetFloat()),
					static_cast<float>(jBoundingBoxParticleEmitter["cea"].GetFloat())
				)
			);
			emitter->setObbCenter(
				Vector3(
					static_cast<float>(jBoundingBoxParticleEmitter["ocx"].GetFloat()),
					static_cast<float>(jBoundingBoxParticleEmitter["ocy"].GetFloat()),
					static_cast<float>(jBoundingBoxParticleEmitter["ocz"].GetFloat())
				)
			);
			emitter->setObbHalfextension(
				Vector3(
					static_cast<float>(jBoundingBoxParticleEmitter["ohex"].GetFloat()),
					static_cast<float>(jBoundingBoxParticleEmitter["ohey"].GetFloat()),
					static_cast<float>(jBoundingBoxParticleEmitter["ohez"].GetFloat())
				)
			);
			emitter->setObbAxis0(
				Vector3(
					static_cast<float>(jBoundingBoxParticleEmitter["oa0x"].GetFloat()),
					static_cast<float>(jBoundingBoxParticleEmitter["oa0y"].GetFloat()),
					static_cast<float>(jBoundingBoxParticleEmitter["oa0z"].GetFloat())
				)
			);
			emitter->setObbAxis1(
				Vector3(
					static_cast<float>(jBoundingBoxParticleEmitter["oa1x"].GetFloat()),
					static_cast<float>(jBoundingBoxParticleEmitter["oa1y"].GetFloat()),
					static_cast<float>(jBoundingBoxParticleEmitter["oa1z"].GetFloat())
				)
			);
			emitter->setObbAxis2(
				Vector3(
					static_cast<float>(jBoundingBoxParticleEmitter["oa2x"].GetFloat()),
					static_cast<float>(jBoundingBoxParticleEmitter["oa2y"].GetFloat()),
					static_cast<float>(jBoundingBoxParticleEmitter["oa2z"].GetFloat())
				)
			);
		} else
		if (v == PrototypeParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER) {
			auto& jCircleParticleEmitter = jParticleSystem["cpe"];
			auto emitter = particleSystem->getCircleParticleEmitter();
			emitter->setCount(jCircleParticleEmitter["c"].GetInt());
			emitter->setLifeTime(jCircleParticleEmitter["lt"].GetInt());
			emitter->setLifeTimeRnd(jCircleParticleEmitter["ltrnd"].GetInt());
			emitter->setMass(static_cast<float>(jCircleParticleEmitter["m"].GetFloat()));
			emitter->setMassRnd(static_cast<float>(jCircleParticleEmitter["mrnd"].GetFloat()));
			emitter->setVelocity(
				Vector3(
					static_cast<float>(jCircleParticleEmitter["vx"].GetFloat()),
					static_cast<float>(jCircleParticleEmitter["vy"].GetFloat()),
					static_cast<float>(jCircleParticleEmitter["vz"].GetFloat())
				)
			);
			emitter->setVelocityRnd(
				Vector3(
					static_cast<float>(jCircleParticleEmitter["vrndx"].GetFloat()),
					static_cast<float>(jCircleParticleEmitter["vrndy"].GetFloat()),
					static_cast<float>(jCircleParticleEmitter["vrndz"].GetFloat())
				)
			);
			emitter->setColorStart(
				Color4(
					static_cast<float>(jCircleParticleEmitter["csr"].GetFloat()),
					static_cast<float>(jCircleParticleEmitter["csg"].GetFloat()),
					static_cast<float>(jCircleParticleEmitter["csb"].GetFloat()),
					static_cast<float>(jCircleParticleEmitter["csa"].GetFloat())
				)
			);
			emitter->setColorEnd(
				Color4(
					static_cast<float>(jCircleParticleEmitter["cer"].GetFloat()),
					static_cast<float>(jCircleParticleEmitter["ceg"].GetFloat()),
					static_cast<float>(jCircleParticleEmitter["ceb"].GetFloat()),
					static_cast<float>(jCircleParticleEmitter["cea"].GetFloat())
				)
			);
			emitter->setCenter(
				Vector3(
					static_cast<float>(jCircleParticleEmitter["cx"].GetFloat()),
					static_cast<float>(jCircleParticleEmitter["cy"].GetFloat()),
					static_cast<float>(jCircleParticleEmitter["cz"].GetFloat())
				)
			);
			emitter->setRadius(static_cast<float>(jCircleParticleEmitter["r"].GetFloat()));
			emitter->setAxis0(
				Vector3(
					static_cast<float>(jCircleParticleEmitter["a0x"].GetFloat()),
					static_cast<float>(jCircleParticleEmitter["a0y"].GetFloat()),
					static_cast<float>(jCircleParticleEmitter["a0z"].GetFloat())
				)
			);
			emitter->setAxis1(
				Vector3(
					static_cast<float>(jCircleParticleEmitter["a1x"].GetFloat()),
					static_cast<float>(jCircleParticleEmitter["a1y"].GetFloat()),
					static_cast<float>(jCircleParticleEmitter["a1z"].GetFloat())
				)
			);
		} else
		if (v == PrototypeParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER_PLANE_VELOCITY) {
			auto& jCircleParticleEmitterPlaneVelocity = jParticleSystem["cpeev"];
			auto emitter = particleSystem->getCircleParticleEmitterPlaneVelocity();
			emitter->setCount(jCircleParticleEmitterPlaneVelocity["c"].GetInt());
			emitter->setLifeTime(jCircleParticleEmitterPlaneVelocity["lt"].GetInt());
			emitter->setLifeTimeRnd(jCircleParticleEmitterPlaneVelocity["ltrnd"].GetInt());
			emitter->setMass(static_cast<float>(jCircleParticleEmitterPlaneVelocity["m"].GetFloat()));
			emitter->setMassRnd(static_cast<float>(jCircleParticleEmitterPlaneVelocity["mrnd"].GetFloat()));
			emitter->setVelocity(static_cast<float>(jCircleParticleEmitterPlaneVelocity["v"].GetFloat()));
			emitter->setVelocityRnd(static_cast<float>(jCircleParticleEmitterPlaneVelocity["vrnd"].GetFloat()));
			emitter->setColorStart(
				Color4(
					static_cast<float>(jCircleParticleEmitterPlaneVelocity["csr"].GetFloat()),
					static_cast<float>(jCircleParticleEmitterPlaneVelocity["csg"].GetFloat()),
					static_cast<float>(jCircleParticleEmitterPlaneVelocity["csb"].GetFloat()),
					static_cast<float>(jCircleParticleEmitterPlaneVelocity["csa"].GetFloat())
				)
			);
			emitter->setColorEnd(
				Color4(
					static_cast<float>(jCircleParticleEmitterPlaneVelocity["cer"].GetFloat()),
					static_cast<float>(jCircleParticleEmitterPlaneVelocity["ceg"].GetFloat()),
					static_cast<float>(jCircleParticleEmitterPlaneVelocity["ceb"].GetFloat()),
					static_cast<float>(jCircleParticleEmitterPlaneVelocity["cea"].GetFloat())
				)
			);
			emitter->setCenter(
				Vector3(
					static_cast<float>(jCircleParticleEmitterPlaneVelocity["cx"].GetFloat()),
					static_cast<float>(jCircleParticleEmitterPlaneVelocity["cy"].GetFloat()),
					static_cast<float>(jCircleParticleEmitterPlaneVelocity["cz"].GetFloat())
				)
			);
			emitter->setRadius(static_cast<float>(jCircleParticleEmitterPlaneVelocity["r"].GetFloat()));
			emitter->setAxis0(
				Vector3(
					static_cast<float>(jCircleParticleEmitterPlaneVelocity["a0x"].GetFloat()),
					static_cast<float>(jCircleParticleEmitterPlaneVelocity["a0y"].GetFloat()),
					static_cast<float>(jCircleParticleEmitterPlaneVelocity["a0z"].GetFloat())
				)
			);
			emitter->setAxis1(
				Vector3(
					static_cast<float>(jCircleParticleEmitterPlaneVelocity["a1x"].GetFloat()),
					static_cast<float>(jCircleParticleEmitterPlaneVelocity["a1y"].GetFloat()),
					static_cast<float>(jCircleParticleEmitterPlaneVelocity["a1z"].GetFloat())
				)
			);
		} else
		if (v == PrototypeParticleSystem_Emitter::SPHERE_PARTICLE_EMITTER) {
			auto& jSphereParticleEmitter = jParticleSystem["spe"];
			auto emitter = particleSystem->getSphereParticleEmitter();
			emitter->setCount(jSphereParticleEmitter["c"].GetInt());
			emitter->setLifeTime(jSphereParticleEmitter["lt"].GetInt());
			emitter->setLifeTimeRnd(jSphereParticleEmitter["ltrnd"].GetInt());
			emitter->setMass(static_cast<float>(jSphereParticleEmitter["m"].GetFloat()));
			emitter->setMassRnd(static_cast<float>(jSphereParticleEmitter["mrnd"].GetFloat()));
			emitter->setVelocity(
				Vector3(
					static_cast<float>(jSphereParticleEmitter["vx"].GetFloat()),
					static_cast<float>(jSphereParticleEmitter["vy"].GetFloat()),
					static_cast<float>(jSphereParticleEmitter["vz"].GetFloat())
				)
			);
			emitter->setVelocityRnd(
				Vector3(
					static_cast<float>(jSphereParticleEmitter["vrndx"].GetFloat()),
					static_cast<float>(jSphereParticleEmitter["vrndy"].GetFloat()),
					static_cast<float>(jSphereParticleEmitter["vrndz"].GetFloat())
				)
			);
			emitter->setColorStart(
				Color4(
					static_cast<float>(jSphereParticleEmitter["csr"].GetFloat()),
					static_cast<float>(jSphereParticleEmitter["csg"].GetFloat()),
					static_cast<float>(jSphereParticleEmitter["csb"].GetFloat()),
					static_cast<float>(jSphereParticleEmitter["csa"].GetFloat())
				)
			);
			emitter->setColorEnd(
				Color4(
					static_cast<float>(jSphereParticleEmitter["cer"].GetFloat()),
					static_cast<float>(jSphereParticleEmitter["ceg"].GetFloat()),
					static_cast<float>(jSphereParticleEmitter["ceb"].GetFloat()),
					static_cast<float>(jSphereParticleEmitter["cea"].GetFloat())
				)
			);
			emitter->setCenter(
				Vector3(
					static_cast<float>(jSphereParticleEmitter["cx"].GetFloat()),
					static_cast<float>(jSphereParticleEmitter["cy"].GetFloat()),
					static_cast<float>(jSphereParticleEmitter["cz"].GetFloat())
				)
			);
			emitter->setRadius(static_cast<float>(jSphereParticleEmitter["r"].GetFloat()));
		} else {
			Console::println(
				"PrototypeWriter::export(): unknown particle system emitter '" +
				particleSystem->getEmitter()->getName() +
				"'"
			 );
		}
	}
}
