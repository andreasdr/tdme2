#include <tdme/engine/prototype/PrototypeParticleSystem_Type.h>

#include <string>

#include <tdme/utilities/Enum.h>

using std::string;

using tdme::engine::prototype::PrototypeParticleSystem_Type;

using tdme::utilities::Enum;

PrototypeParticleSystem_Type::PrototypeParticleSystem_Type(const string& name, int ordinal)
	: Enum(name, ordinal)
{
}

PrototypeParticleSystem_Type* tdme::engine::prototype::PrototypeParticleSystem_Type::NONE = new PrototypeParticleSystem_Type("NONE", 0);
PrototypeParticleSystem_Type* tdme::engine::prototype::PrototypeParticleSystem_Type::OBJECT_PARTICLE_SYSTEM = new PrototypeParticleSystem_Type("OBJECT_PARTICLE_SYSTEM", 1);
PrototypeParticleSystem_Type* tdme::engine::prototype::PrototypeParticleSystem_Type::POINT_PARTICLE_SYSTEM = new PrototypeParticleSystem_Type("POINT_PARTICLE_SYSTEM", 2);
PrototypeParticleSystem_Type* tdme::engine::prototype::PrototypeParticleSystem_Type::FOG_PARTICLE_SYSTEM = new PrototypeParticleSystem_Type("FOG_PARTICLE_SYSTEM", 3);

PrototypeParticleSystem_Type* PrototypeParticleSystem_Type::valueOf(const string& name)
{
	if (NONE->getName() == name) return NONE;
	if (OBJECT_PARTICLE_SYSTEM->getName() == name) return OBJECT_PARTICLE_SYSTEM;
	if (POINT_PARTICLE_SYSTEM->getName() == name) return POINT_PARTICLE_SYSTEM;
	if (FOG_PARTICLE_SYSTEM->getName() == name) return FOG_PARTICLE_SYSTEM;
	// TODO: throw exception here maybe
	return nullptr;
}
