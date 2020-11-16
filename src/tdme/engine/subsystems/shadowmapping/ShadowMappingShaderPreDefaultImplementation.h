#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/engine/subsystems/shadowmapping/ShadowMappingShaderPreBaseImplementation.h>

using std::string;

using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::subsystems::shadowmapping::ShadowMappingShaderPreBaseImplementation;

/**
 * Shadow mapping default shader class to create shadow map
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::subsystems::shadowmapping::ShadowMappingShaderPreDefaultImplementation: public ShadowMappingShaderPreBaseImplementation
{
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
	ShadowMappingShaderPreDefaultImplementation(Renderer* renderer);

	/**
	 * Destructor
	 */
	~ShadowMappingShaderPreDefaultImplementation();

	// overridden methods
	virtual const string getId() override;
	virtual void initialize() override;

};
