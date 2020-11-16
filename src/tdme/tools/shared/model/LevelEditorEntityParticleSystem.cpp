#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem.h>

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
#include <tdme/utilities/Console.h>

using tdme::tools::shared::model::LevelEditorEntityParticleSystem;
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
using tdme::utilities::Console;

LevelEditorEntityParticleSystem::LevelEditorEntityParticleSystem()
{
	type = LevelEditorEntityParticleSystem_Type::NONE;
	ops = nullptr;
	pps = nullptr;
	fps = nullptr;
	emitter = LevelEditorEntityParticleSystem_Emitter::NONE;
	ppe = nullptr;
	bbpe = nullptr;
	cpe = nullptr;
	cpepv = nullptr;
	spe = nullptr;
}

LevelEditorEntityParticleSystem::~LevelEditorEntityParticleSystem() {
	unsetType();
	unsetEmitter();
}

void LevelEditorEntityParticleSystem::unsetType() {
	auto v = this->type;
	if (v == LevelEditorEntityParticleSystem_Type::NONE) {
	} else
	if (v == LevelEditorEntityParticleSystem_Type::OBJECT_PARTICLE_SYSTEM) {
		delete ops;
		ops = nullptr;
	} else
	if (v == LevelEditorEntityParticleSystem_Type::POINT_PARTICLE_SYSTEM) {
		delete pps;
		pps = nullptr;
	} else
	if (v == LevelEditorEntityParticleSystem_Type::FOG_PARTICLE_SYSTEM) {
		delete fps;
		fps = nullptr;
	} else {
		Console::println(string("LevelEditorEntityParticleSystem::setType(): unknown type '" + this->type->getName() + "'"));
	}
}

void LevelEditorEntityParticleSystem::setType(LevelEditorEntityParticleSystem_Type* type)
{
	unsetType();
	this->type = type;
	{
		auto v = this->type;
		if (v == LevelEditorEntityParticleSystem_Type::NONE) {
			// no op
		} else
		if (v == LevelEditorEntityParticleSystem_Type::OBJECT_PARTICLE_SYSTEM) {
			ops = new LevelEditorEntityParticleSystem_ObjectParticleSystem();
		} else
		if (v == LevelEditorEntityParticleSystem_Type::POINT_PARTICLE_SYSTEM) {
			pps = new LevelEditorEntityParticleSystem_PointParticleSystem();
		} else
		if (v == LevelEditorEntityParticleSystem_Type::FOG_PARTICLE_SYSTEM) {
			fps = new LevelEditorEntityParticleSystem_FogParticleSystem();
		} else {
			Console::println(string("LevelEditorEntityParticleSystem::setType(): unknown type '" + this->type->getName() + "'"));
		}
	}

}

void LevelEditorEntityParticleSystem::unsetEmitter() {
	auto v = this->emitter;
	if (v == LevelEditorEntityParticleSystem_Emitter::NONE) {
		// no op
	} else
	if (v == LevelEditorEntityParticleSystem_Emitter::POINT_PARTICLE_EMITTER) {
		delete ppe;
		ppe = nullptr;
	} else
	if (v == LevelEditorEntityParticleSystem_Emitter::BOUNDINGBOX_PARTICLE_EMITTER) {
		delete bbpe;
		bbpe = nullptr;
	} else
	if (v == LevelEditorEntityParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER) {
		delete cpe;
		cpe = nullptr;
	} else
	if (v == LevelEditorEntityParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER_PLANE_VELOCITY) {
		delete cpepv;
		cpepv = nullptr;
	} else
	if (v == LevelEditorEntityParticleSystem_Emitter::SPHERE_PARTICLE_EMITTER) {
		delete spe;
		spe = nullptr;
	} else {
		Console::println(string("LevelEditorEntityParticleSystem::setEmitter(): unknown emitter '" + this->emitter->getName() + "'"));
	}
}

void LevelEditorEntityParticleSystem::setEmitter(LevelEditorEntityParticleSystem_Emitter* emitter)
{
	unsetEmitter();
	this->emitter = emitter;
	{
		auto v = this->emitter;
		if (v == LevelEditorEntityParticleSystem_Emitter::NONE) {
			// no op
		} else
		if (v == LevelEditorEntityParticleSystem_Emitter::POINT_PARTICLE_EMITTER) {
			ppe = new LevelEditorEntityParticleSystem_PointParticleEmitter();
		} else
		if (v == LevelEditorEntityParticleSystem_Emitter::BOUNDINGBOX_PARTICLE_EMITTER) {
			bbpe = new LevelEditorEntityParticleSystem_BoundingBoxParticleEmitter();
		} else
		if (v == LevelEditorEntityParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER) {
			cpe = new LevelEditorEntityParticleSystem_CircleParticleEmitter();
		} else
		if (v == LevelEditorEntityParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER_PLANE_VELOCITY) {
			cpepv = new LevelEditorEntityParticleSystem_CircleParticleEmitterPlaneVelocity();
		} else
		if (v == LevelEditorEntityParticleSystem_Emitter::SPHERE_PARTICLE_EMITTER) {
			spe = new LevelEditorEntityParticleSystem_SphereParticleEmitter();
		} else {
			Console::println(string("LevelEditorEntityParticleSystem::setEmitter(): unknown emitter '" + this->emitter->getName() + "'"));
		}
	}
}
