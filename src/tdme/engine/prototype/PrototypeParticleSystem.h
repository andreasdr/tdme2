#pragma once

#include <tdme/engine/prototype/fwd-tdme.h>

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

/**
 * Prototype particle system definition
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::prototype::PrototypeParticleSystem final
{
private:
	PrototypeParticleSystem_Type* type { nullptr };
	PrototypeParticleSystem_ObjectParticleSystem* ops { nullptr };
	PrototypeParticleSystem_PointParticleSystem* pps { nullptr };
	PrototypeParticleSystem_FogParticleSystem* fps { nullptr };
	PrototypeParticleSystem_Emitter* emitter { nullptr };
	PrototypeParticleSystem_PointParticleEmitter* ppe { nullptr };
	PrototypeParticleSystem_BoundingBoxParticleEmitter* bbpe { nullptr };
	PrototypeParticleSystem_CircleParticleEmitter* cpe { nullptr };
	PrototypeParticleSystem_CircleParticleEmitterPlaneVelocity* cpepv { nullptr };
	PrototypeParticleSystem_SphereParticleEmitter* spe { nullptr };

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
	PrototypeParticleSystem();

	/**
	 * Destructor
	 */
	~PrototypeParticleSystem();

	/**
	 * @return particle system type
	 */
	inline PrototypeParticleSystem_Type* getType() {
		return type;
	}

	/**
	 * Set particle system type
	 * @param type type
	 */
	void setType(PrototypeParticleSystem_Type* type);

	/**
	 * @return object particle system
	 */
	inline PrototypeParticleSystem_ObjectParticleSystem* getObjectParticleSystem() {
		return ops;
	}

	/**
	 * @return point particle system
	 */
	inline PrototypeParticleSystem_PointParticleSystem* getPointParticleSystem() {
		return pps;
	}

	/**
	 * @return fog particle system
	 */
	inline PrototypeParticleSystem_FogParticleSystem* getFogParticleSystem() {
		return fps;
	}

	/**
	 * @return particle system emitter
	 */
	inline PrototypeParticleSystem_Emitter* getEmitter() {
		return emitter;
	}

	/**
	 * Set emitter
	 * @param emitter emitter
	 */
	void setEmitter(PrototypeParticleSystem_Emitter* emitter);

	/**
	 * @return point particle emitter
	 */
	inline PrototypeParticleSystem_PointParticleEmitter* getPointParticleEmitter() {
		return ppe;
	}

	/**
	 * @return bounding box particle emitter
	 */
	inline PrototypeParticleSystem_BoundingBoxParticleEmitter* getBoundingBoxParticleEmitters() {
		return bbpe;
	}

	/**
	 * @return circle particle emitter
	 */
	inline PrototypeParticleSystem_CircleParticleEmitter* getCircleParticleEmitter() {
		return cpe;
	}

	/**
	 * @return circle particle emitter plane velocity
	 */
	inline PrototypeParticleSystem_CircleParticleEmitterPlaneVelocity* getCircleParticleEmitterPlaneVelocity() {
		return cpepv;
	}

	/**
	 * @return sphere particle emitter
	 */
	inline PrototypeParticleSystem_SphereParticleEmitter* getSphereParticleEmitter() {
		return spe;
	}

};
