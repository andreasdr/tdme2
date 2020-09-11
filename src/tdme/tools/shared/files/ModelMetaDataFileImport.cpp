#include <tdme/tools/shared/files/ModelMetaDataFileImport.h>

#include <string>

#include <tdme/engine/LODObject3D.h>
#include <tdme/engine/fileio/models/ModelFileIOException.h>
#include <tdme/engine/fileio/models/ModelReader.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Model.h>
#include <tdme/utilities/ModelTools.h>
#include <tdme/math/Vector3.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/tools/shared/model/LevelEditorEntity_EntityType.h>
#include <tdme/tools/shared/model/LevelEditorEntity.h>
#include <tdme/tools/shared/model/LevelEditorEntityAudio.h>
#include <tdme/tools/shared/model/LevelEditorEntityBoundingVolume.h>
#include <tdme/tools/shared/model/LevelEditorEntityLODLevel.h>
#include <tdme/tools/shared/model/LevelEditorEntityModel.h>
#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem_BoundingBoxParticleEmitter.h>
#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem_CircleParticleEmitter.h>
#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem_CircleParticleEmitterPlaneVelocity.h>
#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem_Emitter.h>
#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem_FogParticleSystem.h>
#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem_ObjectParticleSystem.h>
#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem_PointParticleEmitter.h>
#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem_PointParticleSystem.h>
#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem_SphereParticleEmitter.h>
#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem_Type.h>
#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem.h>
#include <tdme/tools/shared/model/LevelEditorEntityPhysics.h>
#include <tdme/tools/shared/model/LevelEditorEntityPhysics_BodyType.h>
#include <tdme/tools/shared/tools/Tools.h>
#include <tdme/os/filesystem/FileSystemException.h>
#include <tdme/utilities/Float.h>
#include <tdme/utilities/StringTools.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>

#include <rapidjson/document.h>

using std::string;

using tdme::engine::LODObject3D;
using tdme::tools::shared::files::ModelMetaDataFileImport;
using tdme::engine::fileio::models::ModelFileIOException;
using tdme::engine::fileio::models::ModelReader;
using tdme::engine::model::Color4;
using tdme::engine::model::Model;
using tdme::utilities::ModelTools;
using tdme::math::Vector3;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemException;
using tdme::os::filesystem::FileSystemInterface;
using tdme::tools::shared::model::LevelEditorEntity_EntityType;
using tdme::tools::shared::model::LevelEditorEntity;
using tdme::tools::shared::model::LevelEditorEntityAudio;
using tdme::tools::shared::model::LevelEditorEntityBoundingVolume;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem_BoundingBoxParticleEmitter;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem_CircleParticleEmitter;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem_CircleParticleEmitterPlaneVelocity;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem_Emitter;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem_FogParticleSystem;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem_ObjectParticleSystem;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem_PointParticleEmitter;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem_PointParticleSystem;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem_SphereParticleEmitter;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem_Type;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem;
using tdme::tools::shared::model::LevelEditorEntityPhysics;
using tdme::tools::shared::model::LevelEditorEntityPhysics_BodyType;
using tdme::tools::shared::tools::Tools;
using tdme::utilities::Float;
using tdme::utilities::StringTools;
using tdme::utilities::Console;
using tdme::utilities::Exception;

using rapidjson::Document;
using rapidjson::Value;

LevelEditorEntity* ModelMetaDataFileImport::doImport(int32_t id, const string& pathName, const string& fileName)
{
	auto jsonContent = FileSystem::getInstance()->getContentAsString(pathName, fileName);

	Document jEntityRoot;
	jEntityRoot.Parse(jsonContent.c_str());

	auto levelEditorEntity = doImportFromJSON(id, pathName, jEntityRoot);
	levelEditorEntity->setEntityFileName(pathName + "/" + fileName);
	return levelEditorEntity;
}

LevelEditorEntity* ModelMetaDataFileImport::doImportFromJSON(int32_t id, const string& pathName, Value& jEntityRoot)
{
	LevelEditorEntity* levelEditorEntity;
	// auto version = Float::parseFloat((jEntityRoot["version"].GetString()));
	auto pivot = Vector3(
		static_cast< float >(jEntityRoot["px"].GetFloat()),
		static_cast< float >(jEntityRoot["py"].GetFloat()),
		static_cast< float >(jEntityRoot["pz"].GetFloat())
	);
	auto modelType = LevelEditorEntity_EntityType::valueOf((jEntityRoot["type"].GetString()));
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
	if (modelType == LevelEditorEntity_EntityType::EMPTY) {
		model = ModelReader::read("resources/engine/tools/leveleditor/models", "empty.dae");
	}

	levelEditorEntity = new LevelEditorEntity(
		id,
		modelType,
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
		levelEditorEntity->addProperty(
			(jProperty["name"].GetString()),
			(jProperty["value"].GetString())
		);
	}

	if (jEntityRoot.FindMember("bv") != jEntityRoot.MemberEnd()) {
		auto boundingVolume = parseBoundingVolume(
			0,
			levelEditorEntity,
			pathName,
			jEntityRoot["bv"]
		);
		if (boundingVolume->getBoundingVolume() != nullptr) levelEditorEntity->addBoundingVolume(0, boundingVolume);
	} else
	if (jEntityRoot.FindMember("bvs") != jEntityRoot.MemberEnd()) {
		auto jBoundingVolumes = jEntityRoot["bvs"].GetArray();
		auto bvIdx = 0;
		for (auto i = 0; i < jBoundingVolumes.Size(); i++) {
			auto boundingVolume = parseBoundingVolume(bvIdx, levelEditorEntity, pathName, jBoundingVolumes[i]);
			if (boundingVolume->getBoundingVolume() != nullptr) levelEditorEntity->addBoundingVolume(bvIdx++, boundingVolume);
		}
	}
	if (jEntityRoot.FindMember("p") != jEntityRoot.MemberEnd() && levelEditorEntity->getPhysics() != nullptr) {
		auto physics = levelEditorEntity->getPhysics();
		auto& jPhysics = jEntityRoot["p"];
		physics->setType(LevelEditorEntityPhysics_BodyType::valueOf(jPhysics["type"].GetString()));
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
			auto sound = levelEditorEntity->addSound(id);
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
	if (modelType == LevelEditorEntity_EntityType::MODEL) {
		levelEditorEntity->getModelSettings()->setTerrainMesh(jEntityRoot["tm"].GetBool());
		if (jEntityRoot.FindMember("ll2") != jEntityRoot.MemberEnd()) levelEditorEntity->setLODLevel2(parseLODLevel(pathName, jEntityRoot["ll2"]));
		if (jEntityRoot.FindMember("ll3") != jEntityRoot.MemberEnd()) levelEditorEntity->setLODLevel3(parseLODLevel(pathName, jEntityRoot["ll3"]));
	} else
	if (modelType == LevelEditorEntity_EntityType::PARTICLESYSTEM) {
		if (jEntityRoot.FindMember("ps") != jEntityRoot.MemberEnd()) {
			levelEditorEntity->addParticleSystem();
			parseParticleSystem(levelEditorEntity->getParticleSystemAt(0), pathName, jEntityRoot["ps"]);
		} else
		if (jEntityRoot.FindMember("pss") != jEntityRoot.MemberEnd()) {
			auto jParticleSystems = jEntityRoot["pss"].GetArray();
			for (auto i = 0; i < jParticleSystems.Size(); i++) {
				levelEditorEntity->addParticleSystem();
				parseParticleSystem(levelEditorEntity->getParticleSystemAt(levelEditorEntity->getParticleSystemsCount() - 1), pathName, jParticleSystems[i]);
			}
		}
	}
	if (jEntityRoot.FindMember("ds") != jEntityRoot.MemberEnd()) {
		levelEditorEntity->setContributesShadows(jEntityRoot["ds"].GetBool());
		levelEditorEntity->setReceivesShadows(jEntityRoot["ds"].GetBool());
	} else
	if (jEntityRoot.FindMember("cs") != jEntityRoot.MemberEnd() && jEntityRoot.FindMember("rs") != jEntityRoot.MemberEnd()) {
		levelEditorEntity->setContributesShadows(jEntityRoot["cs"].GetBool());
		levelEditorEntity->setReceivesShadows(jEntityRoot["rs"].GetBool());
	}
	levelEditorEntity->setRenderGroups(jEntityRoot.FindMember("rg") != jEntityRoot.MemberEnd()?jEntityRoot["rg"].GetBool():false);
	levelEditorEntity->setShader(jEntityRoot.FindMember("s") != jEntityRoot.MemberEnd()?jEntityRoot["s"].GetString():"default");
	levelEditorEntity->setDistanceShader(jEntityRoot.FindMember("sds") != jEntityRoot.MemberEnd()?jEntityRoot["sds"].GetString():"default");
	levelEditorEntity->setDistanceShaderDistance(jEntityRoot.FindMember("sdsd") != jEntityRoot.MemberEnd()?static_cast<float>(jEntityRoot["sdsd"].GetFloat()):10000.0f);
	if (levelEditorEntity->getModel() != nullptr) ModelTools::prepareForShader(levelEditorEntity->getModel(), levelEditorEntity->getShader());
	return levelEditorEntity;
}

const string ModelMetaDataFileImport::getResourcePathName(const string& pathName, const string& fileName) {
	string modelFile = FileSystem::getInstance()->getCanonicalPath(
		(
			StringTools::startsWith(FileSystem::getInstance()->getPathName(fileName), "/") == true?
				FileSystem::getInstance()->getPathName(fileName):
				pathName + "/" +  FileSystem::getInstance()->getPathName(fileName)
		 ),
		FileSystem::getInstance()->getFileName(fileName)
	);
	auto gameRoot = Tools::getGameRootPath(pathName);
	auto modelRelativeFileName = Tools::getRelativeResourcesFileName(gameRoot, modelFile);
	return (gameRoot.length() > 0 ? gameRoot + "/" : "") + Tools::getPath(modelRelativeFileName);
}

LevelEditorEntityBoundingVolume* ModelMetaDataFileImport::parseBoundingVolume(int32_t idx, LevelEditorEntity* levelEditorEntity, const string& pathName, Value& jBv)
{
	auto entityBoundingVolume = new LevelEditorEntityBoundingVolume(idx, levelEditorEntity);
	BoundingVolume* bv;
	auto bvTypeString = (jBv["type"].GetString());
	if (StringTools::equalsIgnoreCase(bvTypeString, "none") == true) {
		entityBoundingVolume->setupNone();
	} else
	if (StringTools::equalsIgnoreCase(bvTypeString, "sphere") == true) {
		entityBoundingVolume->setupSphere(
			Vector3(
				static_cast< float >(jBv["cx"].GetFloat()),
				static_cast< float >(jBv["cy"].GetFloat()),
				static_cast< float >(jBv["cz"].GetFloat())
			),
			static_cast< float >(jBv["r"].GetFloat())
		);
	} else
	if (StringTools::equalsIgnoreCase(bvTypeString, "capsule") == true) {
		entityBoundingVolume->setupCapsule(
			Vector3(
				static_cast< float >(jBv["ax"].GetFloat()),
				static_cast< float >(jBv["ay"].GetFloat()),
				static_cast< float >(jBv["az"].GetFloat())
			),
			Vector3(
				static_cast< float >(jBv["bx"].GetFloat()),
				static_cast< float >(jBv["by"].GetFloat()),
				static_cast< float >(jBv["bz"].GetFloat())
			),
			static_cast< float >(jBv["r"].GetFloat())
		);
	} else
	if (StringTools::equalsIgnoreCase(bvTypeString, "aabb") == true) {
		entityBoundingVolume->setupAabb(
			Vector3(
				static_cast< float >(jBv["mix"].GetFloat()),
				static_cast< float >(jBv["miy"].GetFloat()),
				static_cast< float >(jBv["miz"].GetFloat())
			),
			Vector3(
				static_cast< float >(jBv["max"].GetFloat()),
				static_cast< float >(jBv["may"].GetFloat()),
				static_cast< float >(jBv["maz"].GetFloat())
			)
		);
	} else
	if (StringTools::equalsIgnoreCase(bvTypeString, "obb") == true) {
		entityBoundingVolume->setupObb(
			Vector3(
				static_cast< float >(jBv["cx"].GetFloat()),
				static_cast< float >(jBv["cy"].GetFloat()),
				static_cast< float >(jBv["cz"].GetFloat())
			),
			Vector3(
				static_cast< float >(jBv["a0x"].GetFloat()),
				static_cast< float >(jBv["a0y"].GetFloat()),
				static_cast< float >(jBv["a0z"].GetFloat())
			),
			Vector3(
				static_cast< float >(jBv["a1x"].GetFloat()),
				static_cast< float >(jBv["a1y"].GetFloat()),
				static_cast< float >(jBv["a1z"].GetFloat())
			),
			Vector3(
				static_cast< float >(jBv["a2x"].GetFloat()),
				static_cast< float >(jBv["a2y"].GetFloat()),
				static_cast< float >(jBv["a2z"].GetFloat())
			),
			Vector3(
				static_cast< float >(jBv["hex"].GetFloat()),
				static_cast< float >(jBv["hey"].GetFloat()),
				static_cast< float >(jBv["hez"].GetFloat())
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
			Console::print(string("ModelMetaDataFileImport::parseBoundingVolume(): An error occurred: "));
			Console::println(string(exception.what()));
		}
	}
	return entityBoundingVolume;
}

LevelEditorEntityLODLevel* ModelMetaDataFileImport::parseLODLevel(const string& pathName, Value& jLodLevel) {
	auto lodType = static_cast<LODObject3D::LODLevelType>(jLodLevel["t"].GetInt());
	LevelEditorEntityLODLevel* lodLevel = new LevelEditorEntityLODLevel(
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

void ModelMetaDataFileImport::parseParticleSystem(LevelEditorEntityParticleSystem* particleSystem, const string& pathName, Value& jParticleSystem) {
	particleSystem->setType(LevelEditorEntityParticleSystem_Type::valueOf((jParticleSystem["t"].GetString())));
	{
		auto v = particleSystem->getType();
		if (v == LevelEditorEntityParticleSystem_Type::NONE) {
			// no op
		} else
		if (v == LevelEditorEntityParticleSystem_Type::OBJECT_PARTICLE_SYSTEM) {
			auto& jObjectParticleSystem = jParticleSystem["ops"];
			auto objectParticleSystem = particleSystem->getObjectParticleSystem();
			objectParticleSystem->setMaxCount(jObjectParticleSystem["mc"].GetInt());
			objectParticleSystem->getScale().setX(static_cast< float >(jObjectParticleSystem["sx"].GetFloat()));
			objectParticleSystem->getScale().setY(static_cast< float >(jObjectParticleSystem["sy"].GetFloat()));
			objectParticleSystem->getScale().setZ(static_cast< float >(jObjectParticleSystem["sz"].GetFloat()));
			objectParticleSystem->setAutoEmit(jObjectParticleSystem["ae"].GetBool());
			try {
				auto particleModelFile = (jObjectParticleSystem["mf"].GetString());
				auto particleModelPath = getResourcePathName(pathName, particleModelFile);
				objectParticleSystem->setModelFile(
					particleModelPath + "/" + Tools::getFileName(particleModelFile)
				);
			} catch (Exception& exception) {
				Console::print(string("ModelMetaDataFileImport::doImport(): An error occurred: "));
				Console::println(string(exception.what()));
			}
		} else
		if (v == LevelEditorEntityParticleSystem_Type::POINT_PARTICLE_SYSTEM) {
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
					Console::print(string("ModelMetaDataFileImport::doImport(): An error occurred: "));
					Console::println(string(exception.what()));
				}
			}
			pointParticleSystem->setAutoEmit(jPointParticleSystem["ae"].GetBool());
		} else
		if (v == LevelEditorEntityParticleSystem_Type::FOG_PARTICLE_SYSTEM) {
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
					Console::print(string("ModelMetaDataFileImport::doImport(): An error occurred: "));
					Console::println(string(exception.what()));
				}
			}
		} else {
			Console::println(
				string(
					 "ModelMetaDataFileExport::export(): unknown particle system type '" +
					 particleSystem->getType()->getName() +
					 "'"
				 )
			 );
		}
	}

	particleSystem->setEmitter(LevelEditorEntityParticleSystem_Emitter::valueOf((jParticleSystem["e"].GetString())));
	{
		auto v = particleSystem->getEmitter();
		if (v == LevelEditorEntityParticleSystem_Emitter::NONE) {
			// no op
		} else
		if (v == LevelEditorEntityParticleSystem_Emitter::POINT_PARTICLE_EMITTER) {
			auto& jPointParticleEmitter = jParticleSystem["ppe"];
			auto emitter = particleSystem->getPointParticleEmitter();
			emitter->setCount(jPointParticleEmitter["c"].GetInt());
			emitter->setLifeTime(jPointParticleEmitter["lt"].GetInt());
			emitter->setLifeTimeRnd(jPointParticleEmitter["ltrnd"].GetInt());
			emitter->setMass(static_cast< float >(jPointParticleEmitter["m"].GetFloat()));
			emitter->setMassRnd(static_cast< float >(jPointParticleEmitter["mrnd"].GetFloat()));
			emitter->setPosition(
				Vector3(
					static_cast< float >(jPointParticleEmitter["px"].GetFloat()),
					static_cast< float >(jPointParticleEmitter["py"].GetFloat()),
					static_cast< float >(jPointParticleEmitter["pz"].GetFloat())
				)
			);
			emitter->setVelocity(
				Vector3(
					static_cast< float >(jPointParticleEmitter["vx"].GetFloat()),
					static_cast< float >(jPointParticleEmitter["vy"].GetFloat()),
					static_cast< float >(jPointParticleEmitter["vz"].GetFloat())
				)
			);
			emitter->setVelocityRnd(
				Vector3(
					static_cast< float >(jPointParticleEmitter["vrndx"].GetFloat()),
					static_cast< float >(jPointParticleEmitter["vrndy"].GetFloat()),
					static_cast< float >(jPointParticleEmitter["vrndz"].GetFloat())
				)
			);
			emitter->setColorStart(
				Color4(
					static_cast< float >(jPointParticleEmitter["csr"].GetFloat()),
					static_cast< float >(jPointParticleEmitter["csg"].GetFloat()),
					static_cast< float >(jPointParticleEmitter["csb"].GetFloat()),
					static_cast< float >(jPointParticleEmitter["csa"].GetFloat())
				)
			);
			emitter->setColorEnd(
				Color4(
					static_cast< float >(jPointParticleEmitter["cer"].GetFloat()),
					static_cast< float >(jPointParticleEmitter["ceg"].GetFloat()),
					static_cast< float >(jPointParticleEmitter["ceb"].GetFloat()),
					static_cast< float >(jPointParticleEmitter["cea"].GetFloat())
				)
			);
		} else
		if (v == LevelEditorEntityParticleSystem_Emitter::BOUNDINGBOX_PARTICLE_EMITTER) {
			auto& jBoundingBoxParticleEmitter = jParticleSystem["bbpe"];
			auto emitter = particleSystem->getBoundingBoxParticleEmitters();
			emitter->setCount(jBoundingBoxParticleEmitter["c"].GetInt());
			emitter->setLifeTime(jBoundingBoxParticleEmitter["lt"].GetInt());
			emitter->setLifeTimeRnd(jBoundingBoxParticleEmitter["ltrnd"].GetInt());
			emitter->setMass(static_cast< float >(jBoundingBoxParticleEmitter["m"].GetFloat()));
			emitter->setMassRnd(static_cast< float >(jBoundingBoxParticleEmitter["mrnd"].GetFloat()));
			emitter->setVelocity(
				Vector3(
					static_cast< float >(jBoundingBoxParticleEmitter["vx"].GetFloat()),
					static_cast< float >(jBoundingBoxParticleEmitter["vy"].GetFloat()),
					static_cast< float >(jBoundingBoxParticleEmitter["vz"].GetFloat())
				)
			);
			emitter->setVelocityRnd(
				Vector3(
					static_cast< float >(jBoundingBoxParticleEmitter["vrndx"].GetFloat()),
					static_cast< float >(jBoundingBoxParticleEmitter["vrndy"].GetFloat()),
					static_cast< float >(jBoundingBoxParticleEmitter["vrndz"].GetFloat())
				)
			);
			emitter->setColorStart(
				Color4(
					static_cast< float >(jBoundingBoxParticleEmitter["csr"].GetFloat()),
					static_cast< float >(jBoundingBoxParticleEmitter["csg"].GetFloat()),
					static_cast< float >(jBoundingBoxParticleEmitter["csb"].GetFloat()),
					static_cast< float >(jBoundingBoxParticleEmitter["csa"].GetFloat())
				)
			);
			emitter->setColorEnd(
				Color4(
					static_cast< float >(jBoundingBoxParticleEmitter["cer"].GetFloat()),
					static_cast< float >(jBoundingBoxParticleEmitter["ceg"].GetFloat()),
					static_cast< float >(jBoundingBoxParticleEmitter["ceb"].GetFloat()),
					static_cast< float >(jBoundingBoxParticleEmitter["cea"].GetFloat())
				)
			);
			emitter->setObbCenter(
				Vector3(
					static_cast< float >(jBoundingBoxParticleEmitter["ocx"].GetFloat()),
					static_cast< float >(jBoundingBoxParticleEmitter["ocy"].GetFloat()),
					static_cast< float >(jBoundingBoxParticleEmitter["ocz"].GetFloat())
				)
			);
			emitter->setObbHalfextension(
				Vector3(
					static_cast< float >(jBoundingBoxParticleEmitter["ohex"].GetFloat()),
					static_cast< float >(jBoundingBoxParticleEmitter["ohey"].GetFloat()),
					static_cast< float >(jBoundingBoxParticleEmitter["ohez"].GetFloat())
				)
			);
			emitter->setObbAxis0(
				Vector3(
					static_cast< float >(jBoundingBoxParticleEmitter["oa0x"].GetFloat()),
					static_cast< float >(jBoundingBoxParticleEmitter["oa0y"].GetFloat()),
					static_cast< float >(jBoundingBoxParticleEmitter["oa0z"].GetFloat())
				)
			);
			emitter->setObbAxis1(
				Vector3(
					static_cast< float >(jBoundingBoxParticleEmitter["oa1x"].GetFloat()),
					static_cast< float >(jBoundingBoxParticleEmitter["oa1y"].GetFloat()),
					static_cast< float >(jBoundingBoxParticleEmitter["oa1z"].GetFloat())
				)
			);
			emitter->setObbAxis2(
				Vector3(
					static_cast< float >(jBoundingBoxParticleEmitter["oa2x"].GetFloat()),
					static_cast< float >(jBoundingBoxParticleEmitter["oa2y"].GetFloat()),
					static_cast< float >(jBoundingBoxParticleEmitter["oa2z"].GetFloat())
				)
			);
		} else
		if (v == LevelEditorEntityParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER) {
			auto& jCircleParticleEmitter = jParticleSystem["cpe"];
			auto emitter = particleSystem->getCircleParticleEmitter();
			emitter->setCount(jCircleParticleEmitter["c"].GetInt());
			emitter->setLifeTime(jCircleParticleEmitter["lt"].GetInt());
			emitter->setLifeTimeRnd(jCircleParticleEmitter["ltrnd"].GetInt());
			emitter->setMass(static_cast< float >(jCircleParticleEmitter["m"].GetFloat()));
			emitter->setMassRnd(static_cast< float >(jCircleParticleEmitter["mrnd"].GetFloat()));
			emitter->setVelocity(
				Vector3(
					static_cast< float >(jCircleParticleEmitter["vx"].GetFloat()),
					static_cast< float >(jCircleParticleEmitter["vy"].GetFloat()),
					static_cast< float >(jCircleParticleEmitter["vz"].GetFloat())
				)
			);
			emitter->setVelocityRnd(
				Vector3(
					static_cast< float >(jCircleParticleEmitter["vrndx"].GetFloat()),
					static_cast< float >(jCircleParticleEmitter["vrndy"].GetFloat()),
					static_cast< float >(jCircleParticleEmitter["vrndz"].GetFloat())
				)
			);
			emitter->setColorStart(
				Color4(
					static_cast< float >(jCircleParticleEmitter["csr"].GetFloat()),
					static_cast< float >(jCircleParticleEmitter["csg"].GetFloat()),
					static_cast< float >(jCircleParticleEmitter["csb"].GetFloat()),
					static_cast< float >(jCircleParticleEmitter["csa"].GetFloat())
				)
			);
			emitter->setColorEnd(
				Color4(
					static_cast< float >(jCircleParticleEmitter["cer"].GetFloat()),
					static_cast< float >(jCircleParticleEmitter["ceg"].GetFloat()),
					static_cast< float >(jCircleParticleEmitter["ceb"].GetFloat()),
					static_cast< float >(jCircleParticleEmitter["cea"].GetFloat())
				)
			);
			emitter->setCenter(
				Vector3(
					static_cast< float >(jCircleParticleEmitter["cx"].GetFloat()),
					static_cast< float >(jCircleParticleEmitter["cy"].GetFloat()),
					static_cast< float >(jCircleParticleEmitter["cz"].GetFloat())
				)
			);
			emitter->setRadius(static_cast< float >(jCircleParticleEmitter["r"].GetFloat()));
			emitter->setAxis0(
				Vector3(
					static_cast< float >(jCircleParticleEmitter["a0x"].GetFloat()),
					static_cast< float >(jCircleParticleEmitter["a0y"].GetFloat()),
					static_cast< float >(jCircleParticleEmitter["a0z"].GetFloat())
				)
			);
			emitter->setAxis1(
				Vector3(
					static_cast< float >(jCircleParticleEmitter["a1x"].GetFloat()),
					static_cast< float >(jCircleParticleEmitter["a1y"].GetFloat()),
					static_cast< float >(jCircleParticleEmitter["a1z"].GetFloat())
				)
			);
		} else
		if (v == LevelEditorEntityParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER_PLANE_VELOCITY) {
			auto& jCircleParticleEmitterPlaneVelocity = jParticleSystem["cpeev"];
			auto emitter = particleSystem->getCircleParticleEmitterPlaneVelocity();
			emitter->setCount(jCircleParticleEmitterPlaneVelocity["c"].GetInt());
			emitter->setLifeTime(jCircleParticleEmitterPlaneVelocity["lt"].GetInt());
			emitter->setLifeTimeRnd(jCircleParticleEmitterPlaneVelocity["ltrnd"].GetInt());
			emitter->setMass(static_cast< float >(jCircleParticleEmitterPlaneVelocity["m"].GetFloat()));
			emitter->setMassRnd(static_cast< float >(jCircleParticleEmitterPlaneVelocity["mrnd"].GetFloat()));
			emitter->setVelocity(static_cast< float >(jCircleParticleEmitterPlaneVelocity["v"].GetFloat()));
			emitter->setVelocityRnd(static_cast< float >(jCircleParticleEmitterPlaneVelocity["vrnd"].GetFloat()));
			emitter->setColorStart(
				Color4(
					static_cast< float >(jCircleParticleEmitterPlaneVelocity["csr"].GetFloat()),
					static_cast< float >(jCircleParticleEmitterPlaneVelocity["csg"].GetFloat()),
					static_cast< float >(jCircleParticleEmitterPlaneVelocity["csb"].GetFloat()),
					static_cast< float >(jCircleParticleEmitterPlaneVelocity["csa"].GetFloat())
				)
			);
			emitter->setColorEnd(
				Color4(
					static_cast< float >(jCircleParticleEmitterPlaneVelocity["cer"].GetFloat()),
					static_cast< float >(jCircleParticleEmitterPlaneVelocity["ceg"].GetFloat()),
					static_cast< float >(jCircleParticleEmitterPlaneVelocity["ceb"].GetFloat()),
					static_cast< float >(jCircleParticleEmitterPlaneVelocity["cea"].GetFloat())
				)
			);
			emitter->setCenter(
				Vector3(
					static_cast< float >(jCircleParticleEmitterPlaneVelocity["cx"].GetFloat()),
					static_cast< float >(jCircleParticleEmitterPlaneVelocity["cy"].GetFloat()),
					static_cast< float >(jCircleParticleEmitterPlaneVelocity["cz"].GetFloat())
				)
			);
			emitter->setRadius(static_cast< float >(jCircleParticleEmitterPlaneVelocity["r"].GetFloat()));
			emitter->setAxis0(
				Vector3(
					static_cast< float >(jCircleParticleEmitterPlaneVelocity["a0x"].GetFloat()),
					static_cast< float >(jCircleParticleEmitterPlaneVelocity["a0y"].GetFloat()),
					static_cast< float >(jCircleParticleEmitterPlaneVelocity["a0z"].GetFloat())
				)
			);
			emitter->setAxis1(
				Vector3(
					static_cast< float >(jCircleParticleEmitterPlaneVelocity["a1x"].GetFloat()),
					static_cast< float >(jCircleParticleEmitterPlaneVelocity["a1y"].GetFloat()),
					static_cast< float >(jCircleParticleEmitterPlaneVelocity["a1z"].GetFloat())
				)
			);
		} else
		if (v == LevelEditorEntityParticleSystem_Emitter::SPHERE_PARTICLE_EMITTER) {
			auto& jSphereParticleEmitter = jParticleSystem["spe"];
			auto emitter = particleSystem->getSphereParticleEmitter();
			emitter->setCount(jSphereParticleEmitter["c"].GetInt());
			emitter->setLifeTime(jSphereParticleEmitter["lt"].GetInt());
			emitter->setLifeTimeRnd(jSphereParticleEmitter["ltrnd"].GetInt());
			emitter->setMass(static_cast< float >(jSphereParticleEmitter["m"].GetFloat()));
			emitter->setMassRnd(static_cast< float >(jSphereParticleEmitter["mrnd"].GetFloat()));
			emitter->setVelocity(
				Vector3(
					static_cast< float >(jSphereParticleEmitter["vx"].GetFloat()),
					static_cast< float >(jSphereParticleEmitter["vy"].GetFloat()),
					static_cast< float >(jSphereParticleEmitter["vz"].GetFloat())
				)
			);
			emitter->setVelocityRnd(
				Vector3(
					static_cast< float >(jSphereParticleEmitter["vrndx"].GetFloat()),
					static_cast< float >(jSphereParticleEmitter["vrndy"].GetFloat()),
					static_cast< float >(jSphereParticleEmitter["vrndz"].GetFloat())
				)
			);
			emitter->setColorStart(
				Color4(
					static_cast< float >(jSphereParticleEmitter["csr"].GetFloat()),
					static_cast< float >(jSphereParticleEmitter["csg"].GetFloat()),
					static_cast< float >(jSphereParticleEmitter["csb"].GetFloat()),
					static_cast< float >(jSphereParticleEmitter["csa"].GetFloat())
				)
			);
			emitter->setColorEnd(
				Color4(
					static_cast< float >(jSphereParticleEmitter["cer"].GetFloat()),
					static_cast< float >(jSphereParticleEmitter["ceg"].GetFloat()),
					static_cast< float >(jSphereParticleEmitter["ceb"].GetFloat()),
					static_cast< float >(jSphereParticleEmitter["cea"].GetFloat())
				)
			);
			emitter->setCenter(
				Vector3(
					static_cast< float >(jSphereParticleEmitter["cx"].GetFloat()),
					static_cast< float >(jSphereParticleEmitter["cy"].GetFloat()),
					static_cast< float >(jSphereParticleEmitter["cz"].GetFloat())
				)
			);
			emitter->setRadius(static_cast< float >(jSphereParticleEmitter["r"].GetFloat()));
		} else {
			Console::println(
				"ModelMetaDataFileExport::export(): unknown particle system emitter '" +
				particleSystem->getEmitter()->getName() +
				"'"
			 );
		}
	}
}
