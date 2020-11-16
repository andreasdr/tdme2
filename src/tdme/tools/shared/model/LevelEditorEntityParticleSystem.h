#pragma once

#include <tdme/tools/shared/model/fwd-tdme.h>

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

/**
 * Level editor entity particle system
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::shared::model::LevelEditorEntityParticleSystem final
{
private:
	LevelEditorEntityParticleSystem_Type* type { nullptr };
	LevelEditorEntityParticleSystem_ObjectParticleSystem* ops { nullptr };
	LevelEditorEntityParticleSystem_PointParticleSystem* pps { nullptr };
	LevelEditorEntityParticleSystem_FogParticleSystem* fps { nullptr };
	LevelEditorEntityParticleSystem_Emitter* emitter { nullptr };
	LevelEditorEntityParticleSystem_PointParticleEmitter* ppe { nullptr };
	LevelEditorEntityParticleSystem_BoundingBoxParticleEmitter* bbpe { nullptr };
	LevelEditorEntityParticleSystem_CircleParticleEmitter* cpe { nullptr };
	LevelEditorEntityParticleSystem_CircleParticleEmitterPlaneVelocity* cpepv { nullptr };
	LevelEditorEntityParticleSystem_SphereParticleEmitter* spe { nullptr };

	/**
	 * Unset particle system type
	 */
	void unsetType();

	/**
	 * Unset emitter
	 */
	void unsetEmitter();

public:
	/**
	 * Public constructor
	 */
	LevelEditorEntityParticleSystem();

	/**
	 * Destructor
	 */
	~LevelEditorEntityParticleSystem();

	/**
	 * @return particle system type
	 */
	inline LevelEditorEntityParticleSystem_Type* getType() {
		return type;
	}

	/**
	 * Set particle system type
	 * @param type type
	 */
	void setType(LevelEditorEntityParticleSystem_Type* type);

	/**
	 * @return object particle system
	 */
	inline LevelEditorEntityParticleSystem_ObjectParticleSystem* getObjectParticleSystem() {
		return ops;
	}

	/**
	 * @return point particle system
	 */
	inline LevelEditorEntityParticleSystem_PointParticleSystem* getPointParticleSystem() {
		return pps;
	}

	/**
	 * @return fog particle system
	 */
	inline LevelEditorEntityParticleSystem_FogParticleSystem* getFogParticleSystem() {
		return fps;
	}

	/**
	 * @return particle system emitter
	 */
	inline LevelEditorEntityParticleSystem_Emitter* getEmitter() {
		return emitter;
	}

	/**
	 * Set emitter
	 * @param emitter emitter
	 */
	void setEmitter(LevelEditorEntityParticleSystem_Emitter* emitter);

	/**
	 * @return point particle emitter
	 */
	inline LevelEditorEntityParticleSystem_PointParticleEmitter* getPointParticleEmitter() {
		return ppe;
	}

	/**
	 * @return bounding box particle emitter
	 */
	inline LevelEditorEntityParticleSystem_BoundingBoxParticleEmitter* getBoundingBoxParticleEmitters() {
		return bbpe;
	}

	/**
	 * @return circle particle emitter
	 */
	inline LevelEditorEntityParticleSystem_CircleParticleEmitter* getCircleParticleEmitter() {
		return cpe;
	}

	/**
	 * @return circle particle emitter plane velocity
	 */
	inline LevelEditorEntityParticleSystem_CircleParticleEmitterPlaneVelocity* getCircleParticleEmitterPlaneVelocity() {
		return cpepv;
	}

	/**
	 * @return sphere particle emitter
	 */
	inline LevelEditorEntityParticleSystem_SphereParticleEmitter* getSphereParticleEmitter() {
		return spe;
	}

};
