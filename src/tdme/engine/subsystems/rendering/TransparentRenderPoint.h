#pragma once

#include <tdme/tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/subsystems/rendering/fwd-tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Vector3.h>

using tdme::engine::model::Color4;
using tdme::math::Vector3;

/**
 * Transparent point to be rendered
 * @author Andreas Drewke
 * @version $Id$
 */
struct tdme::engine::subsystems::rendering::TransparentRenderPoint final
{
	uint16_t spriteIndex;
	Vector3 point;
	Color4 color;
	int particleSystemType;
	void* particleSystem;

	/**
	 * Compare
	 * @param point1 point 1
	 * @param point2 point 2
	 */
	inline static bool compare(TransparentRenderPoint* point1, TransparentRenderPoint* point2) {
		return -point1->point.getZ() > -point2->point.getZ();
	}

};
