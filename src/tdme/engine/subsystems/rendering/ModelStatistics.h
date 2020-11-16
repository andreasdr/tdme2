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
{	int32_t opaqueFaceCount {  };
	int32_t transparentFaceCount {  };
	int32_t materialCount {  };
};
