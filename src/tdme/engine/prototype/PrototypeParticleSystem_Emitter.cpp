#include <tdme/engine/prototype/PrototypeParticleSystem_Emitter.h>

#include <string>

#include <tdme/utilities/Enum.h>

using std::string;

using tdme::engine::prototype::PrototypeParticleSystem_Emitter;
using tdme::utilities::Enum;

PrototypeParticleSystem_Emitter::PrototypeParticleSystem_Emitter(const string& name, int ordinal)
	: Enum(name, ordinal)
{
}

PrototypeParticleSystem_Emitter* tdme::engine::prototype::PrototypeParticleSystem_Emitter::NONE = new PrototypeParticleSystem_Emitter("NONE", 0);
PrototypeParticleSystem_Emitter* tdme::engine::prototype::PrototypeParticleSystem_Emitter::POINT_PARTICLE_EMITTER = new PrototypeParticleSystem_Emitter("POINT_PARTICLE_EMITTER", 1);
PrototypeParticleSystem_Emitter* tdme::engine::prototype::PrototypeParticleSystem_Emitter::BOUNDINGBOX_PARTICLE_EMITTER = new PrototypeParticleSystem_Emitter("BOUNDINGBOX_PARTICLE_EMITTER", 2);
PrototypeParticleSystem_Emitter* tdme::engine::prototype::PrototypeParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER = new PrototypeParticleSystem_Emitter("CIRCLE_PARTICLE_EMITTER", 3);
PrototypeParticleSystem_Emitter* tdme::engine::prototype::PrototypeParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER_PLANE_VELOCITY = new PrototypeParticleSystem_Emitter("CIRCLE_PARTICLE_EMITTER_PLANE_VELOCITY", 4);
PrototypeParticleSystem_Emitter* tdme::engine::prototype::PrototypeParticleSystem_Emitter::SPHERE_PARTICLE_EMITTER = new PrototypeParticleSystem_Emitter("SPHERE_PARTICLE_EMITTER", 5);

PrototypeParticleSystem_Emitter* PrototypeParticleSystem_Emitter::valueOf(const string& name)
{
	if (BOUNDINGBOX_PARTICLE_EMITTER->getName() == name) return BOUNDINGBOX_PARTICLE_EMITTER;
	if (CIRCLE_PARTICLE_EMITTER->getName() == name) return CIRCLE_PARTICLE_EMITTER;
	if (CIRCLE_PARTICLE_EMITTER_PLANE_VELOCITY->getName() == name) return CIRCLE_PARTICLE_EMITTER_PLANE_VELOCITY;
	if (NONE->getName() == name) return NONE;
	if (POINT_PARTICLE_EMITTER->getName() == name) return POINT_PARTICLE_EMITTER;
	if (SPHERE_PARTICLE_EMITTER->getName() == name) return SPHERE_PARTICLE_EMITTER;
	// TODO: throw exception here maybe
	return nullptr;
}
