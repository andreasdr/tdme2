
#pragma once

#include <tdme/tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/subsystems/particlesystem/fwd-tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Vector3.h>

using tdme::engine::model::Color4;
using tdme::math::Vector3;

/**
 * Particle entity
 * @author Andreas Drewke
 * @version $Id$
 */
struct tdme::engine::subsystems::particlesystem::Particle final
{
	bool active;
	float spriteIndex;
	Vector3 velocity;
	Vector3 position;
	float mass;
	int64_t lifeTimeMax;
	int64_t lifeTimeCurrent;
	Color4 color;
	Color4 colorAdd;
};
