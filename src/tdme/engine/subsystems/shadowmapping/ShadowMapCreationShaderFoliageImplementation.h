#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/engine/subsystems/shadowmapping/ShadowMapCreationShaderBaseImplementation.h>

using std::string;

using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::subsystems::shadowmapping::ShadowMapCreationShaderBaseImplementation;

/**
 * Shadow mapping foliage shader class to create shadow map
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::subsystems::shadowmapping::ShadowMapCreationShaderFoliageImplementation: public ShadowMapCreationShaderBaseImplementation
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
	 * Constructor
	 * @param renderer renderer
	 */
	ShadowMapCreationShaderFoliageImplementation(Renderer* renderer);

	/**
	 * Destructor
	 */
	~ShadowMapCreationShaderFoliageImplementation();

	// overridden methods
	virtual const string getId() override;
	virtual void initialize() override;
	virtual void updateShaderParameters(Renderer* renderer, void* context) override;

};
