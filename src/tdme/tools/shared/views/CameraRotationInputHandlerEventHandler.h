#pragma once

#include <tdme/tdme.h>
#include <tdme/tools/shared/views/fwd-tdme.h>

/**
 * Camera Rotation Input Handler Event Handler
 * @author Andreas Drewke
 * @version $Id$
 */
struct tdme::tools::shared::views::CameraRotationInputHandlerEventHandler
{
	/**
	 * Destructor
	 */
	virtual ~CameraRotationInputHandlerEventHandler() {}

	/**
	 * On rotation event to be overloaded
	 */
	virtual void onCameraRotation() = 0;

	/**
	 * On scale event to be overloaded
	 */
	virtual void onCameraScale() = 0;


};
