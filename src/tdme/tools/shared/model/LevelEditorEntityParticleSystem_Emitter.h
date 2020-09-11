#pragma once

#include <string>

#include <tdme/tools/shared/model/fwd-tdme.h>
#include <tdme/utilities/Enum.h>

using std::string;

using tdme::utilities::Enum;

class tdme::tools::shared::model::LevelEditorEntityParticleSystem_Emitter final
	: public Enum
{
public:
	static LevelEditorEntityParticleSystem_Emitter *NONE;
	static LevelEditorEntityParticleSystem_Emitter *POINT_PARTICLE_EMITTER;
	static LevelEditorEntityParticleSystem_Emitter *BOUNDINGBOX_PARTICLE_EMITTER;
	static LevelEditorEntityParticleSystem_Emitter *CIRCLE_PARTICLE_EMITTER;
	static LevelEditorEntityParticleSystem_Emitter *CIRCLE_PARTICLE_EMITTER_PLANE_VELOCITY;
	static LevelEditorEntityParticleSystem_Emitter *SPHERE_PARTICLE_EMITTER;
	LevelEditorEntityParticleSystem_Emitter(const string& name, int ordinal);
	static LevelEditorEntityParticleSystem_Emitter* valueOf(const string& a0);
};
