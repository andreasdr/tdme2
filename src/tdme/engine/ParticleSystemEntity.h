#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/Entity.h>
#include <tdme/engine/Transformations.h>
#include <tdme/engine/subsystems/particlesystem/fwd-tdme.h>

using std::string;

using tdme::engine::Entity;
using tdme::engine::Transformations;
using tdme::engine::subsystems::particlesystem::ParticleEmitter;

/**
 * Particle system entity interface
 * @author Andreas Drewke
 * @version $Id$
 */
struct tdme::engine::ParticleSystemEntity: public Entity
{
	/**
	 * Public destructor
	 */
	virtual ~ParticleSystemEntity() {}

	/**
	 * @return particle emitter
	 */
	virtual ParticleEmitter* getEmitter() = 0;

	/**
	 * @return true if active / particles available
	 */
	virtual bool isActive() = 0;

	/**
	 * @return if auto emit is enabled
	 */
	virtual bool isAutoEmit() = 0;

	/**
	 * Set auto emit
	 * @param autoEmit auto emit
	 */
	virtual void setAutoEmit(bool autoEmit) = 0;

	/**
	 * Updates the particle entity
	 */
	virtual void updateParticles() = 0;

	/**
	 * Adds particles to this particle entity at given position
	 */
	virtual int32_t emitParticles() = 0;

	/**
	 * @return local transformations
	 */
	virtual const Transformations& getLocalTransformations() = 0;

	/**
	 * Set local transformations
	 * @param transformations local transformations
	 */
	virtual void setLocalTransformations(const Transformations& transformations) = 0;

};
