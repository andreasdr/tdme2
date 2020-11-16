#pragma once

#include <tdme/tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/subsystems/rendering/fwd-tdme.h>

using tdme::engine::model::AnimationSetup;

/**
 * Animation state entity
 * @author Andreas Drewke
 * @version $Id$
 */
struct tdme::engine::subsystems::rendering::AnimationState final {
	AnimationSetup* setup { nullptr };
	int64_t endAtTime { -1LL };
	int64_t currentAtTime { -1LL };
	int64_t lastAtTime { -1LL };
	bool finished { true };
	float time { -1LL };
	float speed { 1.0f };
};
