#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/engine/subsystems/earlyzrejection/EZRShaderPreBaseImplementation.h>

using std::string;

using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::subsystems::earlyzrejection::EZRShaderPreBaseImplementation;

/**
 * Early z rejection default shader class
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::subsystems::earlyzrejection::EZRShaderPreDefaultImplementation: public EZRShaderPreBaseImplementation
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
	EZRShaderPreDefaultImplementation(Renderer* renderer);

	/**
	 * Destructor
	 */
	~EZRShaderPreDefaultImplementation();

	// overridden methods
	virtual const string getId() override;
	virtual void initialize() override;

};
