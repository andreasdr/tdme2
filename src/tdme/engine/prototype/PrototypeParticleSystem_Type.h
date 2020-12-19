#pragma once

#include <string>

#include <tdme/engine/prototype/fwd-tdme.h>
#include <tdme/utilities/Enum.h>

using std::string;

using tdme::utilities::Enum;

/**
 * Prototype particle system type enum
 * @author Andreas Drewke
 */
class tdme::engine::prototype::PrototypeParticleSystem_Type final: public Enum
{
public:
	static PrototypeParticleSystem_Type* NONE;
	static PrototypeParticleSystem_Type* OBJECT_PARTICLE_SYSTEM;
	static PrototypeParticleSystem_Type* POINT_PARTICLE_SYSTEM;
	static PrototypeParticleSystem_Type* FOG_PARTICLE_SYSTEM;

	/**
	 * Public constructor
	 * @param name name
	 * @param ordinal ordinal
	 */
	PrototypeParticleSystem_Type(const string& name, int ordinal);

	/**
	 * Returns enum object given by name
	 * @param name name
	 * @return enum object
	 */
	static PrototypeParticleSystem_Type* valueOf(const string& name);

};
