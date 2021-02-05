#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/engine/subsystems/shadowmapping/fwd-tdme.h>
#include <tdme/engine/subsystems/shadowmapping/ShadowMapRenderShaderBaseImplementation.h>

using std::string;

using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::subsystems::shadowmapping::ShadowMapRenderShaderBaseImplementation;
using tdme::engine::subsystems::shadowmapping::ShadowMapRenderShaderFoliageImplementation;

/**
 * Shadow mapping foliage shader to render shadow map
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::subsystems::shadowmapping::ShadowMapRenderShaderFoliageImplementation: public ShadowMapRenderShaderBaseImplementation
{
private:
	int32_t uniformSpeed { -1 };
	int32_t uniformAmplitudeDefault	{ -1 };
	int32_t uniformAmplitudeMax { -1 };

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
	ShadowMapRenderShaderFoliageImplementation(Renderer* renderer);

	/**
	 * Destructor
	 */
	virtual ~ShadowMapRenderShaderFoliageImplementation();

	// overridden methods
	virtual const string getId() override;
	virtual void initialize() override;
	virtual void updateShaderParameters(Renderer* renderer, void* context) override;

};
