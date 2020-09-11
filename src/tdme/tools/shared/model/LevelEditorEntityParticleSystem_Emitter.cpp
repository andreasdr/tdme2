#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem_Emitter.h>

#include <string>

#include <tdme/utilities/Enum.h>

using std::string;

using tdme::tools::shared::model::LevelEditorEntityParticleSystem_Emitter;
using tdme::utilities::Enum;

LevelEditorEntityParticleSystem_Emitter::LevelEditorEntityParticleSystem_Emitter(const string& name, int ordinal)
	: Enum(name, ordinal)
{
}

LevelEditorEntityParticleSystem_Emitter* tdme::tools::shared::model::LevelEditorEntityParticleSystem_Emitter::NONE = new LevelEditorEntityParticleSystem_Emitter("NONE", 0);
LevelEditorEntityParticleSystem_Emitter* tdme::tools::shared::model::LevelEditorEntityParticleSystem_Emitter::POINT_PARTICLE_EMITTER = new LevelEditorEntityParticleSystem_Emitter("POINT_PARTICLE_EMITTER", 1);
LevelEditorEntityParticleSystem_Emitter* tdme::tools::shared::model::LevelEditorEntityParticleSystem_Emitter::BOUNDINGBOX_PARTICLE_EMITTER = new LevelEditorEntityParticleSystem_Emitter("BOUNDINGBOX_PARTICLE_EMITTER", 2);
LevelEditorEntityParticleSystem_Emitter* tdme::tools::shared::model::LevelEditorEntityParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER = new LevelEditorEntityParticleSystem_Emitter("CIRCLE_PARTICLE_EMITTER", 3);
LevelEditorEntityParticleSystem_Emitter* tdme::tools::shared::model::LevelEditorEntityParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER_PLANE_VELOCITY = new LevelEditorEntityParticleSystem_Emitter("CIRCLE_PARTICLE_EMITTER_PLANE_VELOCITY", 4);
LevelEditorEntityParticleSystem_Emitter* tdme::tools::shared::model::LevelEditorEntityParticleSystem_Emitter::SPHERE_PARTICLE_EMITTER = new LevelEditorEntityParticleSystem_Emitter("SPHERE_PARTICLE_EMITTER", 5);

LevelEditorEntityParticleSystem_Emitter* LevelEditorEntityParticleSystem_Emitter::valueOf(const string& a0)
{
	if (BOUNDINGBOX_PARTICLE_EMITTER->getName() == a0) return BOUNDINGBOX_PARTICLE_EMITTER;
	if (CIRCLE_PARTICLE_EMITTER->getName() == a0) return CIRCLE_PARTICLE_EMITTER;
	if (CIRCLE_PARTICLE_EMITTER_PLANE_VELOCITY->getName() == a0) return CIRCLE_PARTICLE_EMITTER_PLANE_VELOCITY;
	if (NONE->getName() == a0) return NONE;
	if (POINT_PARTICLE_EMITTER->getName() == a0) return POINT_PARTICLE_EMITTER;
	if (SPHERE_PARTICLE_EMITTER->getName() == a0) return SPHERE_PARTICLE_EMITTER;
	// TODO: throw exception here maybe
	return nullptr;
}
