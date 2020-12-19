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
#include <tdme/utilities/Console.h>

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
using tdme::utilities::Console;

PrototypeParticleSystem::PrototypeParticleSystem()
{
	type = PrototypeParticleSystem_Type::NONE;
	ops = nullptr;
	pps = nullptr;
	fps = nullptr;
	emitter = PrototypeParticleSystem_Emitter::NONE;
	ppe = nullptr;
	bbpe = nullptr;
	cpe = nullptr;
	cpepv = nullptr;
	spe = nullptr;
}

PrototypeParticleSystem::~PrototypeParticleSystem() {
	unsetType();
	unsetEmitter();
}

void PrototypeParticleSystem::unsetType() {
	auto v = this->type;
	if (v == PrototypeParticleSystem_Type::NONE) {
	} else
	if (v == PrototypeParticleSystem_Type::OBJECT_PARTICLE_SYSTEM) {
		delete ops;
		ops = nullptr;
	} else
	if (v == PrototypeParticleSystem_Type::POINT_PARTICLE_SYSTEM) {
		delete pps;
		pps = nullptr;
	} else
	if (v == PrototypeParticleSystem_Type::FOG_PARTICLE_SYSTEM) {
		delete fps;
		fps = nullptr;
	} else {
		Console::println(string("PrototypeParticleSystem::setType(): unknown type '" + this->type->getName() + "'"));
	}
}

void PrototypeParticleSystem::setType(PrototypeParticleSystem_Type* type)
{
	unsetType();
	this->type = type;
	{
		auto v = this->type;
		if (v == PrototypeParticleSystem_Type::NONE) {
			// no op
		} else
		if (v == PrototypeParticleSystem_Type::OBJECT_PARTICLE_SYSTEM) {
			ops = new PrototypeParticleSystem_ObjectParticleSystem();
		} else
		if (v == PrototypeParticleSystem_Type::POINT_PARTICLE_SYSTEM) {
			pps = new PrototypeParticleSystem_PointParticleSystem();
		} else
		if (v == PrototypeParticleSystem_Type::FOG_PARTICLE_SYSTEM) {
			fps = new PrototypeParticleSystem_FogParticleSystem();
		} else {
			Console::println(string("PrototypeParticleSystem::setType(): unknown type '" + this->type->getName() + "'"));
		}
	}

}

void PrototypeParticleSystem::unsetEmitter() {
	auto v = this->emitter;
	if (v == PrototypeParticleSystem_Emitter::NONE) {
		// no op
	} else
	if (v == PrototypeParticleSystem_Emitter::POINT_PARTICLE_EMITTER) {
		delete ppe;
		ppe = nullptr;
	} else
	if (v == PrototypeParticleSystem_Emitter::BOUNDINGBOX_PARTICLE_EMITTER) {
		delete bbpe;
		bbpe = nullptr;
	} else
	if (v == PrototypeParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER) {
		delete cpe;
		cpe = nullptr;
	} else
	if (v == PrototypeParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER_PLANE_VELOCITY) {
		delete cpepv;
		cpepv = nullptr;
	} else
	if (v == PrototypeParticleSystem_Emitter::SPHERE_PARTICLE_EMITTER) {
		delete spe;
		spe = nullptr;
	} else {
		Console::println(string("PrototypeParticleSystem::setEmitter(): unknown emitter '" + this->emitter->getName() + "'"));
	}
}

void PrototypeParticleSystem::setEmitter(PrototypeParticleSystem_Emitter* emitter)
{
	unsetEmitter();
	this->emitter = emitter;
	{
		auto v = this->emitter;
		if (v == PrototypeParticleSystem_Emitter::NONE) {
			// no op
		} else
		if (v == PrototypeParticleSystem_Emitter::POINT_PARTICLE_EMITTER) {
			ppe = new PrototypeParticleSystem_PointParticleEmitter();
		} else
		if (v == PrototypeParticleSystem_Emitter::BOUNDINGBOX_PARTICLE_EMITTER) {
			bbpe = new PrototypeParticleSystem_BoundingBoxParticleEmitter();
		} else
		if (v == PrototypeParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER) {
			cpe = new PrototypeParticleSystem_CircleParticleEmitter();
		} else
		if (v == PrototypeParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER_PLANE_VELOCITY) {
			cpepv = new PrototypeParticleSystem_CircleParticleEmitterPlaneVelocity();
		} else
		if (v == PrototypeParticleSystem_Emitter::SPHERE_PARTICLE_EMITTER) {
			spe = new PrototypeParticleSystem_SphereParticleEmitter();
		} else {
			Console::println(string("PrototypeParticleSystem::setEmitter(): unknown emitter '" + this->emitter->getName() + "'"));
		}
	}
}
