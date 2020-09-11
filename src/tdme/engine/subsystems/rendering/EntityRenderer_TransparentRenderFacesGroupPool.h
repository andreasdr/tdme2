#pragma once

#include <tdme/engine/subsystems/rendering/fwd-tdme.h>
#include <tdme/utilities/Pool.h>
#include <tdme/engine/subsystems/rendering/TransparentRenderFacesGroup.h>

using tdme::utilities::Pool;
using tdme::engine::subsystems::rendering::EntityRenderer;
using tdme::engine::subsystems::rendering::TransparentRenderFacesGroup;

/**
 * Entity renderer transparent render faces group pool
 * @author Andreas Drewke
 */
class tdme::engine::subsystems::rendering::EntityRenderer_TransparentRenderFacesGroupPool: public Pool<TransparentRenderFacesGroup*>
{
protected:
	/**
	 * Instantiate
	 */
	TransparentRenderFacesGroup* instantiate() override;
public:
	/**
	 * Public constructor
	 */
	EntityRenderer_TransparentRenderFacesGroupPool();
};
