#include <tdme/engine/fileio/prototypes/PrototypeWriter.h>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include <tdme/engine/fileio/models/TMWriter.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/primitives/BoundingVolume.h>
#include <tdme/engine/primitives/Capsule.h>
#include <tdme/engine/primitives/ConvexMesh.h>
#include <tdme/engine/primitives/OrientedBoundingBox.h>
#include <tdme/engine/primitives/Sphere.h>
#include <tdme/engine/primitives/Triangle.h>
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
#include <tdme/engine/prototype/PrototypeProperty.h>
#include <tdme/engine/LODObject3D.h>
#include <tdme/math/Vector3.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/tools/shared/tools/Tools.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>
#include <tdme/utilities/StringTools.h>

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

using std::ostringstream;
using std::string;

using tdme::engine::fileio::prototypes::PrototypeWriter;

using tdme::engine::fileio::models::TMWriter;
using tdme::engine::model::Color4;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::primitives::BoundingVolume;
using tdme::engine::primitives::Capsule;
using tdme::engine::primitives::ConvexMesh;
using tdme::engine::primitives::OrientedBoundingBox;
using tdme::engine::primitives::Sphere;
using tdme::engine::primitives::Triangle;
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
using tdme::engine::prototype::PrototypeProperty;
using tdme::engine::LODObject3D;
using tdme::math::Vector3;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;
using tdme::tools::shared::tools::Tools;
using tdme::utilities::Console;
using tdme::utilities::Exception;
using tdme::utilities::StringTools;

using rapidjson::Document;
using rapidjson::StringBuffer;
using rapidjson::Value;
using rapidjson::Writer;

void PrototypeWriter::copyFile(const string& source, const string& dest)
{
}

void PrototypeWriter::write(const string& pathName, const string& fileName, Prototype* prototype)
{
	prototype->setFileName(FileSystem::getInstance()->getCanonicalPath(pathName, fileName));
	Document jRoot;
	jRoot.SetObject();
	write(jRoot, jRoot, prototype);

	StringBuffer strbuf;
	Writer<StringBuffer> writer(strbuf);
	jRoot.Accept(writer);

	FileSystem::getInstance()->setContentFromString(pathName, fileName, strbuf.GetString());
}

void PrototypeWriter::writeLODLevelToJSON(Document& jDocument, Value& jLodLevelRoot, PrototypeLODLevel* lodLevel) {
	auto& jAllocator = jDocument.GetAllocator();
	jLodLevelRoot.SetObject();
	jLodLevelRoot.AddMember("t", Value(lodLevel->getType()), jAllocator);
	if (lodLevel->getType() == LODObject3D::LODLEVELTYPE_MODEL) {
		//
		auto modelPathName = Tools::getPath(lodLevel->getFileName());
		auto modelFileName = Tools::removeFileEnding(Tools::getFileName(lodLevel->getFileName())) + ".tm";
		TMWriter::write(
			lodLevel->getModel(),
			modelPathName,
			modelFileName
		);
		jLodLevelRoot.AddMember("f", Value(modelPathName + "/" + modelFileName, jAllocator), jAllocator);
	}
	jLodLevelRoot.AddMember("d", Value(lodLevel->getMinDistance()), jAllocator);
	jLodLevelRoot.AddMember("cmr", Value(lodLevel->getColorMul().getRed()), jAllocator);
	jLodLevelRoot.AddMember("cmg", Value(lodLevel->getColorMul().getGreen()), jAllocator);
	jLodLevelRoot.AddMember("cmb", Value(lodLevel->getColorMul().getBlue()), jAllocator);
	jLodLevelRoot.AddMember("cma", Value(lodLevel->getColorMul().getAlpha()), jAllocator);
	jLodLevelRoot.AddMember("car", Value(lodLevel->getColorAdd().getRed()), jAllocator);
	jLodLevelRoot.AddMember("cag", Value(lodLevel->getColorAdd().getGreen()), jAllocator);
	jLodLevelRoot.AddMember("cab", Value(lodLevel->getColorAdd().getBlue()), jAllocator);
	jLodLevelRoot.AddMember("caa", Value(lodLevel->getColorAdd().getAlpha()), jAllocator);
}

void PrototypeWriter::write(Document& jDocument, Value& jEntityRoot, Prototype* prototype)
{
	auto& jAllocator = jDocument.GetAllocator();
	if (prototype->getType() == Prototype_Type::MODEL && prototype->getModelFileName().length() > 0) {
		auto modelPathName = Tools::getPath(prototype->getModelFileName());
		auto modelFileName = Tools::removeFileEnding(Tools::getFileName(prototype->getModelFileName())) + ".tm";
		TMWriter::write(
			prototype->getModel(),
			modelPathName,
			modelFileName
		);
		jEntityRoot.AddMember("file", Value(modelPathName + "/" + modelFileName, jAllocator), jAllocator);
		/*
		try {
			auto thumbnail = modelFileName + ".png";
			jEntityRoot.AddMember("thumbnail] = (thumbnail));
			copyFile("./tmp/ + entity->getThumbnail(), Tools::getPath(entity->getFileName()) + thumbnail));
		} catch (Exception& exception) {
			Console::print(string("PrototypeWriter::export(): An error occurred: '));
			Console::print(entity->getFileName());
			Console::print(string(": "));
			Console::println(exception.what());
		}
		*/
		jEntityRoot.AddMember("tm", Value(prototype->isTerrainMesh()), jAllocator);
		int lodLevelIdx = 2;
		{
			auto lodLevel = prototype->getLODLevel2();
			if (lodLevel != nullptr &&
				(lodLevel->getType() == LODObject3D::LODLEVELTYPE_IGNORE ||
				((lodLevel->getType() == LODObject3D::LODLEVELTYPE_MODEL) &&
				lodLevel->getModel() != nullptr))) {
				//
				Value jLodLevel;
				jLodLevel.SetObject();
				writeLODLevelToJSON(jDocument, jLodLevel, lodLevel);
				jEntityRoot.AddMember(rapidjson::GenericStringRef<char>((string("ll") + to_string(lodLevelIdx++)).c_str()), jLodLevel, jAllocator);
			}
		}
		{
			auto lodLevel = prototype->getLODLevel3();
			if (lodLevel != nullptr &&
				(lodLevel->getType() == LODObject3D::LODLEVELTYPE_IGNORE ||
				((lodLevel->getType() == LODObject3D::LODLEVELTYPE_MODEL) &&
				lodLevel->getModel() != nullptr))) {
				//
				//
				Value jLodLevel;
				jLodLevel.SetObject();
				writeLODLevelToJSON(jDocument, jLodLevel, lodLevel);
				jEntityRoot.AddMember(rapidjson::GenericStringRef<char>((string("ll") + to_string(lodLevelIdx++)).c_str()), jLodLevel, jAllocator);
			}
		}
	}
	jEntityRoot.AddMember("version", Value("1.99", jAllocator), jAllocator);
	jEntityRoot.AddMember("type", Value(prototype->getType()->getName(), jAllocator), jAllocator);
	jEntityRoot.AddMember("name", Value(prototype->getName(), jAllocator), jAllocator);
	jEntityRoot.AddMember("descr", Value(prototype->getDescription(), jAllocator), jAllocator);
	jEntityRoot.AddMember("px", Value(prototype->getPivot().getX()), jAllocator);
	jEntityRoot.AddMember("py", Value(prototype->getPivot().getY()), jAllocator);
	jEntityRoot.AddMember("pz", Value(prototype->getPivot().getZ()), jAllocator);
	if (prototype->getSounds().size() > 0) {
		Value jSounds;
		jSounds.SetArray();
		for (auto sound: prototype->getSounds()) {
			if (sound->getFileName().length() == 0) continue;
			Value jSound;
			jSound.SetObject();
			jSound.AddMember("i", Value(sound->getId(), jAllocator), jAllocator);
			jSound.AddMember("a", Value(sound->getAnimation(), jAllocator), jAllocator);
			jSound.AddMember("file", Value(sound->getFileName(), jAllocator), jAllocator);
			jSound.AddMember("g", Value(sound->getGain()), jAllocator);
			jSound.AddMember("p", Value(sound->getPitch()), jAllocator);
			jSound.AddMember("o", Value(sound->getOffset()), jAllocator);
			jSound.AddMember("l", Value(sound->isLooping()), jAllocator);
			jSound.AddMember("f", Value(sound->isFixed()), jAllocator);
			jSounds.PushBack(jSound, jAllocator);
		}
		jEntityRoot.AddMember("sd", jSounds, jAllocator);
	}

	if (prototype->getType() == Prototype_Type::PARTICLESYSTEM) {
		Value jParticleSystems;
		jParticleSystems.SetArray();
		for (auto i = 0; i < prototype->getParticleSystemsCount(); i++) {
			auto particleSystem = prototype->getParticleSystemAt(i);
			Value jParticleSystem;
			jParticleSystem.SetObject();
			jParticleSystem.AddMember("t", Value(particleSystem->getType()->getName(), jAllocator), jAllocator);
			{
				auto v = particleSystem->getType();
				if (v == PrototypeParticleSystem_Type::NONE) {
					// no op
				} else
				if (v == PrototypeParticleSystem_Type::OBJECT_PARTICLE_SYSTEM)
				{
					Value jObjectParticleSystem;
					jObjectParticleSystem.SetObject();
					if (particleSystem->getObjectParticleSystem()->getModelFile().length() > 0) {
						auto modelPathName = Tools::getPath(particleSystem->getObjectParticleSystem()->getModelFile());
						auto modelFileName = Tools::removeFileEnding(Tools::getFileName(particleSystem->getObjectParticleSystem()->getModelFile())) + ".tm";
						TMWriter::write(
							particleSystem->getObjectParticleSystem()->getModel(),
							modelPathName,
							modelFileName
						);
						particleSystem->getObjectParticleSystem()->setModelFile(modelPathName + "/" + modelFileName);
					}
					jObjectParticleSystem.AddMember("mc", Value(particleSystem->getObjectParticleSystem()->getMaxCount()), jAllocator);
					jObjectParticleSystem.AddMember("sx", Value(particleSystem->getObjectParticleSystem()->getScale().getX()), jAllocator);
					jObjectParticleSystem.AddMember("sy", Value(particleSystem->getObjectParticleSystem()->getScale().getY()), jAllocator);
					jObjectParticleSystem.AddMember("sz", Value(particleSystem->getObjectParticleSystem()->getScale().getZ()), jAllocator);
					jObjectParticleSystem.AddMember("mf", Value(particleSystem->getObjectParticleSystem()->getModelFile(), jAllocator), jAllocator);
					jObjectParticleSystem.AddMember("ae", Value(particleSystem->getObjectParticleSystem()->isAutoEmit()), jAllocator);
					jParticleSystem.AddMember("ops", jObjectParticleSystem, jAllocator);
				} else
				if (v == PrototypeParticleSystem_Type::POINT_PARTICLE_SYSTEM)
				{
					Value jPointParticleSystem;
					jPointParticleSystem.SetObject();
					jPointParticleSystem.AddMember("mp", Value(particleSystem->getPointParticleSystem()->getMaxPoints()), jAllocator);
					jPointParticleSystem.AddMember("ps", Value(particleSystem->getPointParticleSystem()->getPointSize()), jAllocator);
					jPointParticleSystem.AddMember("t", Value(particleSystem->getPointParticleSystem()->getTextureFileName(), jAllocator), jAllocator);
					jPointParticleSystem.AddMember("tt", Value(particleSystem->getPointParticleSystem()->getTransparencyTextureFileName(), jAllocator), jAllocator);
					jPointParticleSystem.AddMember("ths", Value(particleSystem->getPointParticleSystem()->getTextureHorizontalSprites()), jAllocator);
					jPointParticleSystem.AddMember("tvs", Value(particleSystem->getPointParticleSystem()->getTextureVerticalSprites()), jAllocator);
					jPointParticleSystem.AddMember("fps", Value(particleSystem->getPointParticleSystem()->getTextureSpritesFPS()), jAllocator);
					jPointParticleSystem.AddMember("ae", Value(particleSystem->getPointParticleSystem()->isAutoEmit()), jAllocator);
					jParticleSystem.AddMember("pps", jPointParticleSystem, jAllocator);
				} else
				if (v == PrototypeParticleSystem_Type::FOG_PARTICLE_SYSTEM)
				{
					Value jFogParticleSystem;
					jFogParticleSystem.SetObject();
					jFogParticleSystem.AddMember("mp", Value(particleSystem->getFogParticleSystem()->getMaxPoints()), jAllocator);
					jFogParticleSystem.AddMember("ps", Value(particleSystem->getFogParticleSystem()->getPointSize()), jAllocator);
					jFogParticleSystem.AddMember("t", Value(particleSystem->getFogParticleSystem()->getTextureFileName(), jAllocator), jAllocator);
					jFogParticleSystem.AddMember("tt", Value(particleSystem->getFogParticleSystem()->getTransparencyTextureFileName(), jAllocator), jAllocator);
					jFogParticleSystem.AddMember("ths", Value(particleSystem->getFogParticleSystem()->getTextureHorizontalSprites()), jAllocator);
					jFogParticleSystem.AddMember("tvs", Value(particleSystem->getFogParticleSystem()->getTextureVerticalSprites()), jAllocator);
					jFogParticleSystem.AddMember("fps", Value(particleSystem->getFogParticleSystem()->getTextureSpritesFPS()), jAllocator);
					jParticleSystem.AddMember("fps", jFogParticleSystem, jAllocator);
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

			jParticleSystem.AddMember("e", Value(particleSystem->getEmitter()->getName(), jAllocator), jAllocator);
			{
				auto v = particleSystem->getEmitter();
				if (v == PrototypeParticleSystem_Emitter::NONE)
				{
				} else
				if (v == PrototypeParticleSystem_Emitter::POINT_PARTICLE_EMITTER)
				{
					Value jPointParticleEmitter;
					jPointParticleEmitter.SetObject();
					auto emitter = particleSystem->getPointParticleEmitter();
					jPointParticleEmitter.AddMember("c", Value(emitter->getCount()), jAllocator);
					jPointParticleEmitter.AddMember("lt", Value(emitter->getLifeTime()), jAllocator);
					jPointParticleEmitter.AddMember("ltrnd", Value(emitter->getLifeTimeRnd()), jAllocator);
					jPointParticleEmitter.AddMember("m", Value(emitter->getMass()), jAllocator);
					jPointParticleEmitter.AddMember("mrnd", Value(emitter->getMassRnd()), jAllocator);
					jPointParticleEmitter.AddMember("px", Value(emitter->getPosition().getX()), jAllocator);
					jPointParticleEmitter.AddMember("py", Value(emitter->getPosition().getY()), jAllocator);
					jPointParticleEmitter.AddMember("pz", Value(emitter->getPosition().getZ()), jAllocator);
					jPointParticleEmitter.AddMember("vx", Value(emitter->getVelocity().getX()), jAllocator);
					jPointParticleEmitter.AddMember("vy", Value(emitter->getVelocity().getY()), jAllocator);
					jPointParticleEmitter.AddMember("vz", Value(emitter->getVelocity().getZ()), jAllocator);
					jPointParticleEmitter.AddMember("vrndx", Value(emitter->getVelocityRnd().getX()), jAllocator);
					jPointParticleEmitter.AddMember("vrndy", Value(emitter->getVelocityRnd().getY()), jAllocator);
					jPointParticleEmitter.AddMember("vrndz", Value(emitter->getVelocityRnd().getZ()), jAllocator);
					jPointParticleEmitter.AddMember("csr", Value(emitter->getColorStart().getRed()), jAllocator);
					jPointParticleEmitter.AddMember("csg", Value(emitter->getColorStart().getGreen()), jAllocator);
					jPointParticleEmitter.AddMember("csb", Value(emitter->getColorStart().getBlue()), jAllocator);
					jPointParticleEmitter.AddMember("csa", Value(emitter->getColorStart().getAlpha()), jAllocator);
					jPointParticleEmitter.AddMember("cer", Value(emitter->getColorEnd().getRed()), jAllocator);
					jPointParticleEmitter.AddMember("ceg", Value(emitter->getColorEnd().getGreen()), jAllocator);
					jPointParticleEmitter.AddMember("ceb", Value(emitter->getColorEnd().getBlue()), jAllocator);
					jPointParticleEmitter.AddMember("cea", Value(emitter->getColorEnd().getAlpha()), jAllocator);
					jParticleSystem.AddMember("ppe", jPointParticleEmitter, jAllocator);
				} else
				if (v == PrototypeParticleSystem_Emitter::BOUNDINGBOX_PARTICLE_EMITTER)
				{
					Value jBoundingBoxParticleEmitter;
					jBoundingBoxParticleEmitter.SetObject();
					auto emitter = particleSystem->getBoundingBoxParticleEmitters();
					jBoundingBoxParticleEmitter.AddMember("c", Value(emitter->getCount()), jAllocator);
					jBoundingBoxParticleEmitter.AddMember("lt", Value(emitter->getLifeTime()), jAllocator);
					jBoundingBoxParticleEmitter.AddMember("ltrnd", Value(emitter->getLifeTimeRnd()), jAllocator);
					jBoundingBoxParticleEmitter.AddMember("m", Value(emitter->getMass()), jAllocator);
					jBoundingBoxParticleEmitter.AddMember("mrnd", Value(emitter->getMassRnd()), jAllocator);
					jBoundingBoxParticleEmitter.AddMember("vx", Value(emitter->getVelocity().getX()), jAllocator);
					jBoundingBoxParticleEmitter.AddMember("vy", Value(emitter->getVelocity().getY()), jAllocator);
					jBoundingBoxParticleEmitter.AddMember("vz", Value(emitter->getVelocity().getZ()), jAllocator);
					jBoundingBoxParticleEmitter.AddMember("vrndx", Value(emitter->getVelocityRnd().getX()), jAllocator);
					jBoundingBoxParticleEmitter.AddMember("vrndy", Value(emitter->getVelocityRnd().getY()), jAllocator);
					jBoundingBoxParticleEmitter.AddMember("vrndz", Value(emitter->getVelocityRnd().getZ()), jAllocator);
					jBoundingBoxParticleEmitter.AddMember("csr", Value(emitter->getColorStart().getRed()), jAllocator);
					jBoundingBoxParticleEmitter.AddMember("csg", Value(emitter->getColorStart().getGreen()), jAllocator);
					jBoundingBoxParticleEmitter.AddMember("csb", Value(emitter->getColorStart().getBlue()), jAllocator);
					jBoundingBoxParticleEmitter.AddMember("csa", Value(emitter->getColorStart().getAlpha()), jAllocator);
					jBoundingBoxParticleEmitter.AddMember("cer", Value(emitter->getColorEnd().getRed()), jAllocator);
					jBoundingBoxParticleEmitter.AddMember("ceg", Value(emitter->getColorEnd().getGreen()), jAllocator);
					jBoundingBoxParticleEmitter.AddMember("ceb", Value(emitter->getColorEnd().getBlue()), jAllocator);
					jBoundingBoxParticleEmitter.AddMember("cea", Value(emitter->getColorEnd().getAlpha()), jAllocator);
					jBoundingBoxParticleEmitter.AddMember("ocx", Value(emitter->getObbCenter().getX()), jAllocator);
					jBoundingBoxParticleEmitter.AddMember("ocy", Value(emitter->getObbCenter().getY()), jAllocator);
					jBoundingBoxParticleEmitter.AddMember("ocz", Value(emitter->getObbCenter().getZ()), jAllocator);
					jBoundingBoxParticleEmitter.AddMember("ohex", Value(emitter->getObbHalfextension().getX()), jAllocator);
					jBoundingBoxParticleEmitter.AddMember("ohey", Value(emitter->getObbHalfextension().getY()), jAllocator);
					jBoundingBoxParticleEmitter.AddMember("ohez", Value(emitter->getObbHalfextension().getZ()), jAllocator);
					jBoundingBoxParticleEmitter.AddMember("oa0x", Value(emitter->getObbAxis0().getX()), jAllocator);
					jBoundingBoxParticleEmitter.AddMember("oa0y", Value(emitter->getObbAxis0().getY()), jAllocator);
					jBoundingBoxParticleEmitter.AddMember("oa0z", Value(emitter->getObbAxis0().getZ()), jAllocator);
					jBoundingBoxParticleEmitter.AddMember("oa1x", Value(emitter->getObbAxis1().getX()), jAllocator);
					jBoundingBoxParticleEmitter.AddMember("oa1y", Value(emitter->getObbAxis1().getY()), jAllocator);
					jBoundingBoxParticleEmitter.AddMember("oa1z", Value(emitter->getObbAxis1().getZ()), jAllocator);
					jBoundingBoxParticleEmitter.AddMember("oa2x", Value(emitter->getObbAxis2().getX()), jAllocator);
					jBoundingBoxParticleEmitter.AddMember("oa2y", Value(emitter->getObbAxis2().getY()), jAllocator);
					jBoundingBoxParticleEmitter.AddMember("oa2z", Value(emitter->getObbAxis2().getZ()), jAllocator);
					jParticleSystem.AddMember("bbpe", jBoundingBoxParticleEmitter, jAllocator);
				} else
				if (v == PrototypeParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER)
				{
					Value jCircleParticleEmitter;
					jCircleParticleEmitter.SetObject();
					auto emitter = particleSystem->getCircleParticleEmitter();
					jCircleParticleEmitter.AddMember("c", Value(emitter->getCount()), jAllocator);
					jCircleParticleEmitter.AddMember("lt", Value(emitter->getLifeTime()), jAllocator);
					jCircleParticleEmitter.AddMember("ltrnd", Value(emitter->getLifeTimeRnd()), jAllocator);
					jCircleParticleEmitter.AddMember("m", Value(emitter->getMass()), jAllocator);
					jCircleParticleEmitter.AddMember("mrnd", Value(emitter->getMassRnd()), jAllocator);
					jCircleParticleEmitter.AddMember("vx", Value(emitter->getVelocity().getX()), jAllocator);
					jCircleParticleEmitter.AddMember("vy", Value(emitter->getVelocity().getY()), jAllocator);
					jCircleParticleEmitter.AddMember("vz", Value(emitter->getVelocity().getZ()), jAllocator);
					jCircleParticleEmitter.AddMember("vrndx", Value(emitter->getVelocityRnd().getX()), jAllocator);
					jCircleParticleEmitter.AddMember("vrndy", Value(emitter->getVelocityRnd().getY()), jAllocator);
					jCircleParticleEmitter.AddMember("vrndz", Value(emitter->getVelocityRnd().getZ()), jAllocator);
					jCircleParticleEmitter.AddMember("csr", Value(emitter->getColorStart().getRed()), jAllocator);
					jCircleParticleEmitter.AddMember("csg", Value(emitter->getColorStart().getGreen()), jAllocator);
					jCircleParticleEmitter.AddMember("csb", Value(emitter->getColorStart().getBlue()), jAllocator);
					jCircleParticleEmitter.AddMember("csa", Value(emitter->getColorStart().getAlpha()), jAllocator);
					jCircleParticleEmitter.AddMember("cer", Value(emitter->getColorEnd().getRed()), jAllocator);
					jCircleParticleEmitter.AddMember("ceg", Value(emitter->getColorEnd().getGreen()), jAllocator);
					jCircleParticleEmitter.AddMember("ceb", Value(emitter->getColorEnd().getBlue()), jAllocator);
					jCircleParticleEmitter.AddMember("cea", Value(emitter->getColorEnd().getAlpha()), jAllocator);
					jCircleParticleEmitter.AddMember("cx", Value(emitter->getCenter().getX()), jAllocator);
					jCircleParticleEmitter.AddMember("cy", Value(emitter->getCenter().getY()), jAllocator);
					jCircleParticleEmitter.AddMember("cz", Value(emitter->getCenter().getZ()), jAllocator);
					jCircleParticleEmitter.AddMember("r", Value(emitter->getRadius()), jAllocator);
					jCircleParticleEmitter.AddMember("a0x", Value(emitter->getAxis0().getX()), jAllocator);
					jCircleParticleEmitter.AddMember("a0y", Value(emitter->getAxis0().getY()), jAllocator);
					jCircleParticleEmitter.AddMember("a0z", Value(emitter->getAxis0().getZ()), jAllocator);
					jCircleParticleEmitter.AddMember("a1x", Value(emitter->getAxis1().getX()), jAllocator);
					jCircleParticleEmitter.AddMember("a1y", Value(emitter->getAxis1().getY()), jAllocator);
					jCircleParticleEmitter.AddMember("a1z", Value(emitter->getAxis1().getZ()), jAllocator);
					jParticleSystem.AddMember("cpe", jCircleParticleEmitter, jAllocator);
				} else
				if (v == PrototypeParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER_PLANE_VELOCITY) {
					Value jCircleParticleEmitterPlaneVelocity;
					jCircleParticleEmitterPlaneVelocity.SetObject();
					auto emitter = particleSystem->getCircleParticleEmitterPlaneVelocity();
					jCircleParticleEmitterPlaneVelocity.AddMember("c", Value(emitter->getCount()), jAllocator);
					jCircleParticleEmitterPlaneVelocity.AddMember("lt", Value(emitter->getLifeTime()), jAllocator);
					jCircleParticleEmitterPlaneVelocity.AddMember("ltrnd", Value(emitter->getLifeTimeRnd()), jAllocator);
					jCircleParticleEmitterPlaneVelocity.AddMember("m", Value(emitter->getMass()), jAllocator);
					jCircleParticleEmitterPlaneVelocity.AddMember("mrnd", Value(emitter->getMassRnd()), jAllocator);
					jCircleParticleEmitterPlaneVelocity.AddMember("v", Value(emitter->getVelocity()), jAllocator);
					jCircleParticleEmitterPlaneVelocity.AddMember("vrnd", Value(emitter->getVelocityRnd()), jAllocator);
					jCircleParticleEmitterPlaneVelocity.AddMember("csr", Value(emitter->getColorStart().getRed()), jAllocator);
					jCircleParticleEmitterPlaneVelocity.AddMember("csg", Value(emitter->getColorStart().getGreen()), jAllocator);
					jCircleParticleEmitterPlaneVelocity.AddMember("csb", Value(emitter->getColorStart().getBlue()), jAllocator);
					jCircleParticleEmitterPlaneVelocity.AddMember("csa", Value(emitter->getColorStart().getAlpha()), jAllocator);
					jCircleParticleEmitterPlaneVelocity.AddMember("cer", Value(emitter->getColorEnd().getRed()), jAllocator);
					jCircleParticleEmitterPlaneVelocity.AddMember("ceg", Value(emitter->getColorEnd().getGreen()), jAllocator);
					jCircleParticleEmitterPlaneVelocity.AddMember("ceb", Value(emitter->getColorEnd().getBlue()), jAllocator);
					jCircleParticleEmitterPlaneVelocity.AddMember("cea", Value(emitter->getColorEnd().getAlpha()), jAllocator);
					jCircleParticleEmitterPlaneVelocity.AddMember("cx", Value(emitter->getCenter().getX()), jAllocator);
					jCircleParticleEmitterPlaneVelocity.AddMember("cy", Value(emitter->getCenter().getY()), jAllocator);
					jCircleParticleEmitterPlaneVelocity.AddMember("cz", Value(emitter->getCenter().getZ()), jAllocator);
					jCircleParticleEmitterPlaneVelocity.AddMember("r", Value(emitter->getRadius()), jAllocator);
					jCircleParticleEmitterPlaneVelocity.AddMember("a0x", Value(emitter->getAxis0().getX()), jAllocator);
					jCircleParticleEmitterPlaneVelocity.AddMember("a0y", Value(emitter->getAxis0().getY()), jAllocator);
					jCircleParticleEmitterPlaneVelocity.AddMember("a0z", Value(emitter->getAxis0().getZ()), jAllocator);
					jCircleParticleEmitterPlaneVelocity.AddMember("a1x", Value(emitter->getAxis1().getX()), jAllocator);
					jCircleParticleEmitterPlaneVelocity.AddMember("a1y", Value(emitter->getAxis1().getY()), jAllocator);
					jCircleParticleEmitterPlaneVelocity.AddMember("a1z", Value(emitter->getAxis1().getZ()), jAllocator);
					jParticleSystem.AddMember("cpeev", jCircleParticleEmitterPlaneVelocity, jAllocator);
				} else
				if (v == PrototypeParticleSystem_Emitter::SPHERE_PARTICLE_EMITTER)
				{
					Value jSphereParticleEmitter;
					jSphereParticleEmitter.SetObject();
					auto emitter = particleSystem->getSphereParticleEmitter();
					jSphereParticleEmitter.AddMember("c", Value(emitter->getCount()), jAllocator);
					jSphereParticleEmitter.AddMember("lt", Value(emitter->getLifeTime()), jAllocator);
					jSphereParticleEmitter.AddMember("ltrnd", Value(emitter->getLifeTimeRnd()), jAllocator);
					jSphereParticleEmitter.AddMember("m", Value(emitter->getMass()), jAllocator);
					jSphereParticleEmitter.AddMember("mrnd", Value(emitter->getMassRnd()), jAllocator);
					jSphereParticleEmitter.AddMember("vx", Value(emitter->getVelocity().getX()), jAllocator);
					jSphereParticleEmitter.AddMember("vy", Value(emitter->getVelocity().getY()), jAllocator);
					jSphereParticleEmitter.AddMember("vz", Value(emitter->getVelocity().getZ()), jAllocator);
					jSphereParticleEmitter.AddMember("vrndx", Value(emitter->getVelocityRnd().getX()), jAllocator);
					jSphereParticleEmitter.AddMember("vrndy", Value(emitter->getVelocityRnd().getY()), jAllocator);
					jSphereParticleEmitter.AddMember("vrndz", Value(emitter->getVelocityRnd().getZ()), jAllocator);
					jSphereParticleEmitter.AddMember("csr", Value(emitter->getColorStart().getRed()), jAllocator);
					jSphereParticleEmitter.AddMember("csg", Value(emitter->getColorStart().getGreen()), jAllocator);
					jSphereParticleEmitter.AddMember("csb", Value(emitter->getColorStart().getBlue()), jAllocator);
					jSphereParticleEmitter.AddMember("csa", Value(emitter->getColorStart().getAlpha()), jAllocator);
					jSphereParticleEmitter.AddMember("cer", Value(emitter->getColorEnd().getRed()), jAllocator);
					jSphereParticleEmitter.AddMember("ceg", Value(emitter->getColorEnd().getGreen()), jAllocator);
					jSphereParticleEmitter.AddMember("ceb", Value(emitter->getColorEnd().getBlue()), jAllocator);
					jSphereParticleEmitter.AddMember("cea", Value(emitter->getColorEnd().getAlpha()), jAllocator);
					jSphereParticleEmitter.AddMember("cx", Value(emitter->getCenter().getX()), jAllocator);
					jSphereParticleEmitter.AddMember("cy", Value(emitter->getCenter().getY()), jAllocator);
					jSphereParticleEmitter.AddMember("cz", Value(emitter->getCenter().getZ()), jAllocator);
					jSphereParticleEmitter.AddMember("r", Value(emitter->getRadius()), jAllocator);
					jParticleSystem.AddMember("spe", jSphereParticleEmitter, jAllocator);
				} else
				{
					Console::println(
						string(
							"PrototypeWriter::export(): unknown particle system emitter '" +
							particleSystem->getEmitter()->getName() +
							"'"
						 )
					 );
				}
			}
			jParticleSystems.PushBack(jParticleSystem, jAllocator);
		}
		jEntityRoot.AddMember("pss", jParticleSystems, jAllocator);
	}
	Value jBoundingVolumes;
	jBoundingVolumes.SetArray();
	for (auto i = 0; i < prototype->getBoundingVolumeCount(); i++) {
		auto entityBoundingVolume = prototype->getBoundingVolume(i);
		auto bv = entityBoundingVolume->getBoundingVolume();
		if (bv == nullptr) continue;
		Value jBoundingVolume;
		jBoundingVolume.SetObject();
		if (bv == nullptr) {
			jBoundingVolume.AddMember("type", Value("none", jAllocator), jAllocator);
		} else
		if (dynamic_cast< Sphere* >(bv) != nullptr) {
			auto sphere = dynamic_cast< Sphere* >(bv);
			jBoundingVolume.AddMember("type", Value("sphere", jAllocator), jAllocator);
			jBoundingVolume.AddMember("cx", Value(sphere->getCenter().getX()), jAllocator);
			jBoundingVolume.AddMember("cy", Value(sphere->getCenter().getY()), jAllocator);
			jBoundingVolume.AddMember("cz", Value(sphere->getCenter().getZ()), jAllocator);
			jBoundingVolume.AddMember("r", Value(sphere->getRadius()), jAllocator);
		} else
		if (dynamic_cast< Capsule* >(bv) != nullptr) {
			auto capsule = dynamic_cast< Capsule* >(bv);
			jBoundingVolume.AddMember("type", Value("capsule", jAllocator), jAllocator);
			jBoundingVolume.AddMember("ax", Value(capsule->getA().getX()), jAllocator);
			jBoundingVolume.AddMember("ay", Value(capsule->getA().getY()), jAllocator);
			jBoundingVolume.AddMember("az", Value(capsule->getA().getZ()), jAllocator);
			jBoundingVolume.AddMember("bx", Value(capsule->getB().getX()), jAllocator);
			jBoundingVolume.AddMember("by", Value(capsule->getB().getY()), jAllocator);
			jBoundingVolume.AddMember("bz", Value(capsule->getB().getZ()), jAllocator);
			jBoundingVolume.AddMember("r", Value(capsule->getRadius()), jAllocator);
		} else
		if (dynamic_cast< BoundingBox* >(bv) != nullptr) {
			auto aabb = dynamic_cast< BoundingBox* >(bv);
			jBoundingVolume.AddMember("type", Value("aabb", jAllocator), jAllocator);
			jBoundingVolume.AddMember("mix", Value(aabb->getMin().getX()), jAllocator);
			jBoundingVolume.AddMember("miy", Value(aabb->getMin().getY()), jAllocator);
			jBoundingVolume.AddMember("miz", Value(aabb->getMin().getZ()), jAllocator);
			jBoundingVolume.AddMember("max", Value(aabb->getMax().getX()), jAllocator);
			jBoundingVolume.AddMember("may", Value(aabb->getMax().getY()), jAllocator);
			jBoundingVolume.AddMember("maz", Value(aabb->getMax().getZ()), jAllocator);
		} else
		if (dynamic_cast< OrientedBoundingBox* >(bv) != nullptr) {
			auto obb = dynamic_cast< OrientedBoundingBox* >(bv);
			jBoundingVolume.AddMember("type", Value("obb", jAllocator), jAllocator);
			jBoundingVolume.AddMember("cx", Value(obb->getCenter().getX()), jAllocator);
			jBoundingVolume.AddMember("cy", Value(obb->getCenter().getY()), jAllocator);
			jBoundingVolume.AddMember("cz", Value(obb->getCenter().getZ()), jAllocator);
			jBoundingVolume.AddMember("a0x", Value(obb->getAxes()[0].getX()), jAllocator);
			jBoundingVolume.AddMember("a0y", Value(obb->getAxes()[0].getY()), jAllocator);
			jBoundingVolume.AddMember("a0z", Value(obb->getAxes()[0].getZ()), jAllocator);
			jBoundingVolume.AddMember("a1x", Value(obb->getAxes()[1].getX()), jAllocator);
			jBoundingVolume.AddMember("a1y", Value(obb->getAxes()[1].getY()), jAllocator);
			jBoundingVolume.AddMember("a1z", Value(obb->getAxes()[1].getZ()), jAllocator);
			jBoundingVolume.AddMember("a2x", Value(obb->getAxes()[2].getX()), jAllocator);
			jBoundingVolume.AddMember("a2y", Value(obb->getAxes()[2].getY()), jAllocator);
			jBoundingVolume.AddMember("a2z", Value(obb->getAxes()[2].getZ()), jAllocator);
			jBoundingVolume.AddMember("hex", Value(obb->getHalfExtension().getX()), jAllocator);
			jBoundingVolume.AddMember("hey", Value(obb->getHalfExtension().getY()), jAllocator);
			jBoundingVolume.AddMember("hez", Value(obb->getHalfExtension().getZ()), jAllocator);
		} else
		if (dynamic_cast< ConvexMesh* >(bv) != nullptr) {
			jBoundingVolume.AddMember("type", Value("convexmesh", jAllocator), jAllocator);
			jBoundingVolume.AddMember("file", Value(entityBoundingVolume->getModelMeshFile(), jAllocator), jAllocator);
		}
		jBoundingVolumes.PushBack(jBoundingVolume, jAllocator);
	}
	jEntityRoot.AddMember("bvs", jBoundingVolumes, jAllocator);
	auto physics = prototype->getPhysics();
	if (physics != nullptr) {
		Value jPhysics;
		jPhysics.SetObject();
		jPhysics.AddMember("type", Value(physics->getType()->getName(), jAllocator), jAllocator);
		jPhysics.AddMember("mass", Value(physics->getMass()), jAllocator);
		jPhysics.AddMember("restitution", Value(physics->getRestitution()), jAllocator);
		jPhysics.AddMember("friction", Value(physics->getFriction()), jAllocator);
		jPhysics.AddMember("itx", Value(physics->getInertiaTensor().getX()), jAllocator);
		jPhysics.AddMember("ity", Value(physics->getInertiaTensor().getY()), jAllocator);
		jPhysics.AddMember("itz", Value(physics->getInertiaTensor().getZ()), jAllocator);
		jEntityRoot.AddMember("p", jPhysics, jAllocator);
	}
	Value jPrototypeProperties;
	jPrototypeProperties.SetArray();
	for (auto i = 0; i < prototype->getPropertyCount(); i++) {
		PrototypeProperty* modelProperty = prototype->getPropertyByIndex(i);
		Value jObjectProperty;
		jObjectProperty.SetObject();
		jObjectProperty.AddMember("name", Value(modelProperty->getName(), jAllocator), jAllocator);
		jObjectProperty.AddMember("value", Value(modelProperty->getValue(), jAllocator), jAllocator);
		jPrototypeProperties.PushBack(jObjectProperty, jAllocator);
	}
	jEntityRoot.AddMember("properties", jPrototypeProperties, jAllocator);
	jEntityRoot.AddMember("cs", Value(prototype->isContributesShadows()), jAllocator);
	jEntityRoot.AddMember("rs", Value(prototype->isReceivesShadows()), jAllocator);
	jEntityRoot.AddMember("rg", Value(prototype->isRenderGroups()), jAllocator);
	jEntityRoot.AddMember("s", Value(prototype->getShader(), jAllocator), jAllocator);
	jEntityRoot.AddMember("sds", Value(prototype->getDistanceShader(), jAllocator), jAllocator);
	jEntityRoot.AddMember("sdsd", Value(prototype->getDistanceShaderDistance()), jAllocator);
	if (prototype->getType() == Prototype_Type::ENVIRONMENTMAPPING) {
		jEntityRoot.AddMember("emrpm", Value(prototype->getEnvironmentMapRenderPassMask()), jAllocator);
		jEntityRoot.AddMember("emtf", Value(prototype->getEnvironmentMapTimeRenderUpdateFrequency()), jAllocator);
	}
}
