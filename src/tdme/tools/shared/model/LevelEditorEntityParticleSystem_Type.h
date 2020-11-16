#pragma once

#include <string>

#include <tdme/tools/shared/model/fwd-tdme.h>
#include <tdme/utilities/Enum.h>

using std::string;

using tdme::utilities::Enum;

class tdme::tools::shared::model::LevelEditorEntityParticleSystem_Type final
	: public Enum
{
public:
	static LevelEditorEntityParticleSystem_Type* NONE;
	static LevelEditorEntityParticleSystem_Type* OBJECT_PARTICLE_SYSTEM;
	static LevelEditorEntityParticleSystem_Type* POINT_PARTICLE_SYSTEM;
	static LevelEditorEntityParticleSystem_Type* FOG_PARTICLE_SYSTEM;

	/**
	 * Public constructor
	 * @param name name
	 * @param ordinal ordinal
	 */
	LevelEditorEntityParticleSystem_Type(const string& name, int ordinal);

	/**
	 * Returns enum object given by name
	 * @param name name
	 * @return enum object
	 */
	static LevelEditorEntityParticleSystem_Type* valueOf(const string& name);

};
