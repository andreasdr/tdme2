#include "EntityRenderer_TransparentRenderFacesGroupPool.h"
#include <tdme/engine/subsystems/rendering/EntityRenderer.h>
#include <tdme/engine/subsystems/rendering/TransparentRenderFacesGroup.h>

using tdme::engine::subsystems::rendering::EntityRenderer;
using tdme::engine::subsystems::rendering::EntityRenderer_TransparentRenderFacesGroupPool;
using tdme::engine::subsystems::rendering::TransparentRenderFacesGroup;

EntityRenderer_TransparentRenderFacesGroupPool::EntityRenderer_TransparentRenderFacesGroupPool()
{
}

TransparentRenderFacesGroup* EntityRenderer_TransparentRenderFacesGroupPool::instantiate()
{
	return new TransparentRenderFacesGroup();
}
