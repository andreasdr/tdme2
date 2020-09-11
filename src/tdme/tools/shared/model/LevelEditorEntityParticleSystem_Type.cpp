#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem_Type.h>

#include <string>

#include <tdme/utilities/Enum.h>

using std::string;

using tdme::tools::shared::model::LevelEditorEntityParticleSystem_Type;
using tdme::utilities::Enum;

LevelEditorEntityParticleSystem_Type::LevelEditorEntityParticleSystem_Type(const string& name, int ordinal)
	: Enum(name, ordinal)
{
}

LevelEditorEntityParticleSystem_Type* tdme::tools::shared::model::LevelEditorEntityParticleSystem_Type::NONE = new LevelEditorEntityParticleSystem_Type("NONE", 0);
LevelEditorEntityParticleSystem_Type* tdme::tools::shared::model::LevelEditorEntityParticleSystem_Type::OBJECT_PARTICLE_SYSTEM = new LevelEditorEntityParticleSystem_Type("OBJECT_PARTICLE_SYSTEM", 1);
LevelEditorEntityParticleSystem_Type* tdme::tools::shared::model::LevelEditorEntityParticleSystem_Type::POINT_PARTICLE_SYSTEM = new LevelEditorEntityParticleSystem_Type("POINT_PARTICLE_SYSTEM", 2);
LevelEditorEntityParticleSystem_Type* tdme::tools::shared::model::LevelEditorEntityParticleSystem_Type::FOG_PARTICLE_SYSTEM = new LevelEditorEntityParticleSystem_Type("FOG_PARTICLE_SYSTEM", 3);

LevelEditorEntityParticleSystem_Type* LevelEditorEntityParticleSystem_Type::valueOf(const string& a0)
{
	if (NONE->getName() == a0) return NONE;
	if (OBJECT_PARTICLE_SYSTEM->getName() == a0) return OBJECT_PARTICLE_SYSTEM;
	if (POINT_PARTICLE_SYSTEM->getName() == a0) return POINT_PARTICLE_SYSTEM;
	if (FOG_PARTICLE_SYSTEM->getName() == a0) return FOG_PARTICLE_SYSTEM;
	// TODO: throw exception here maybe
	return nullptr;
}
