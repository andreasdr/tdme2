#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/Transformations.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/subsystems/particlesystem/fwd-tdme.h>

using std::string;

using tdme::engine::Transformations;
using tdme::engine::model::Color4;
using tdme::engine::subsystems::particlesystem::ParticleEmitter;

/**
 * Particle system entity internal interface
 * @author Andreas Drewke
 * @version $Id$
 */
struct tdme::engine::subsystems::particlesystem::ParticleSystemEntityInternal
{

	/**
	 * Public destructor
	 */
	virtual ~ParticleSystemEntityInternal() {}

	/**
	 * @return particle emitter
	 */
	virtual ParticleEmitter* getEmitter() = 0;

	/**
	 * @return object id
	 */
	virtual const string& getId() = 0;

	/**
	 * @return true if enabled to be rendered
	 */
	virtual bool isEnabled() = 0;

	/**
	 * @return true if active / particles available
	 */
	virtual bool isActive() = 0;

	/**
	 * Enable/disable rendering
	 * @param enabled enabled
	 */
	virtual void setEnabled(bool enabled) = 0;

	/**
	 * @return if object is pickable
	 */
	virtual bool isPickable() = 0;

	/**
	 * Set this object pickable
	 * @param pickable pickable
	 */
	virtual void setPickable(bool pickable) = 0;

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
	 * The effect color will be multiplied with fragment color
	 * @return effect color
	 */
	virtual const Color4& getEffectColorMul() const = 0;

	/**
	 * Set the effect color that will be multiplied with fragment color
	 * @param effectColorMul effect color
	 */
	virtual void setEffectColorMul(const Color4& effectColorMul) = 0;

	/**
	 * The effect color will be added to fragment color
	 * @return effect color
	 */
	virtual const Color4& getEffectColorAdd() const = 0;

	/**
	 * Set the effect color that will be added to fragment color
	 * @param effectColorAdd effect color
	 */
	virtual void setEffectColorAdd(const Color4& effectColorAdd) = 0;

	/**
	 * Updates the particle entity
	 */
	virtual void updateParticles() = 0;

	/**
	 * Adds particles to this particle entity at given position
	 */
	virtual int32_t emitParticles() = 0;

	/**
	 * Set up this transformations from given transformations
	 * @param transformations transformations
	 */
	virtual void fromTransformations(const Transformations& transformations) = 0;

	/**
	 * Update transformations
	 */
	virtual void update() = 0;

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
