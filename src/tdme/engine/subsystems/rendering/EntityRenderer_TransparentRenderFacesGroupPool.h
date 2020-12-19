#pragma once

#include <tdme/engine/subsystems/rendering/fwd-tdme.h>
#include <tdme/engine/subsystems/rendering/TransparentRenderFacesGroup.h>
#include <tdme/utilities/Pool.h>

using tdme::engine::subsystems::rendering::EntityRenderer;
using tdme::engine::subsystems::rendering::TransparentRenderFacesGroup;
using tdme::utilities::Pool;

/**
 * Entity renderer transparent render faces node pool
 * @author Andreas Drewke
 */
class tdme::engine::subsystems::rendering::EntityRenderer_TransparentRenderFacesGroupPool: public Pool<TransparentRenderFacesGroup*>
{
public:
	/**
	 * Public constructor
	 */
	EntityRenderer_TransparentRenderFacesGroupPool();

protected:
	/**
	 * Instantiate
	 */
	TransparentRenderFacesGroup* instantiate() override;

};
