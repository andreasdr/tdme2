#pragma once

#include <tdme/tdme.h>
#include <tdme/engine/subsystems/rendering/fwd-tdme.h>

using tdme::engine::subsystems::rendering::ModelUtilitiesInternal;

/**
 * Model statistics entity
 * @author Andreas Drewke
 * @version $Id$
 */
struct tdme::engine::subsystems::rendering::ModelStatistics
{	int32_t opaqueFaceCount { 0 };
	int32_t transparentFaceCount { 0 };
	int32_t materialCount { 0 };
};
