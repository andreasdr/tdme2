#pragma once

#include <tdme/engine/subsystems/rendering/fwd-tdme.h>
#include <tdme/utilities/Pool.h>
#include <tdme/engine/subsystems/rendering/TransparentRenderFace.h>

using tdme::utilities::Pool;
using tdme::engine::subsystems::rendering::TransparentRenderFace;
using tdme::engine::subsystems::rendering::TransparentRenderFacesPool;

/**
 * Transparent render faces pool
 * @author Andreas Drewke
 */
class tdme::engine::subsystems::rendering::TransparentRenderFacesPool_TransparentRenderFacesPool
	: public Pool<TransparentRenderFace*>
{
protected:
	inline TransparentRenderFace* instantiate() override {
		return new TransparentRenderFace();
	}

public:
	TransparentRenderFacesPool_TransparentRenderFacesPool();
};
