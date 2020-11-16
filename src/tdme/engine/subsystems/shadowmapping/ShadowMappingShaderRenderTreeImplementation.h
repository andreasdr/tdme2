#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/engine/subsystems/shadowmapping/fwd-tdme.h>
#include <tdme/engine/subsystems/shadowmapping/ShadowMappingShaderRenderBaseImplementation.h>

using std::string;

using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::subsystems::shadowmapping::ShadowMappingShaderRenderBaseImplementation;
using tdme::engine::subsystems::shadowmapping::ShadowMappingShaderRenderTreeImplementation;

/**
 * Shadow mapping foliage shader to render shadow map
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::subsystems::shadowmapping::ShadowMappingShaderRenderTreeImplementation: public ShadowMappingShaderRenderBaseImplementation
{
public:
	/**
	 * @return if supported by renderer
	 * @param renderer renderer
	 */
	static bool isSupported(Renderer* renderer);

	/**
	 * Public constructor
	 * @param renderer renderer
	 */
	ShadowMappingShaderRenderTreeImplementation(Renderer* renderer);

	/**
	 * Destructor
	 */
	virtual ~ShadowMappingShaderRenderTreeImplementation();

	// overridden methods
	virtual const string getId() override;
	virtual void initialize() override;

};
