#pragma once

#include <array>

#include <tdme/tdme.h>
#include <tdme/engine/physics/fwd-tdme.h>
#include <tdme/engine/primitives/fwd-tdme.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Vector3.h>

using tdme::engine::primitives::BoundingBox;

/**
 * Collision detection
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::physics::CollisionDetection final
{
public:

	/**
	 * Returns if axis aligned bounding boxes do collide
	 * Will not provide hit points
	 * @param b1 axis aligned bounding box 1
	 * @param b2 axis aligned bounding box 2
	 * @return collision
	 */
	inline static bool doCollideAABBvsAABBFast(BoundingBox* b1, BoundingBox* b2) {
		// see
		//      http://www.gamedev.net/topic/567310-platform-game-collision-detection/
		auto& b1Min = b1->getMin();
		auto& b1Max = b1->getMax();
		auto& b2Min = b2->getMin();
		auto& b2Max = b2->getMax();

		// face distances
		if (b2Max[0] - b1Min[0] < 0.0f) return false; // b2 collides into b1 on x
		if (b2Max[1] - b1Min[1] < 0.0f) return false; // b2 collides into b1 on y
		if (b2Max[2] - b1Min[2] < 0.0f) return false; // b2 collides into b1 on z
		if (b1Max[0] - b2Min[0] < 0.0f) return false; // b1 collides into b2 on x
		if (b1Max[1] - b2Min[1] < 0.0f) return false; // b1 collides into b2 on y
		if (b1Max[2] - b2Min[2] < 0.0f) return false; // b1 collides into b2 on z
		return true;
	}

};
