#pragma once

#include <tdme/tdme.h>
#include <tdme/tools/shared/views/fwd-tdme.h>

/**
 * Camera Input Handler Event Handler
 * @author Andreas Drewke
 * @version $Id$
 */
struct tdme::tools::shared::views::CameraInputHandlerEventHandler
{
	/**
	 * Destructor
	 */
	virtual ~CameraInputHandlerEventHandler() {}

	/**
	 * On translation event to be overloaded
	 */
	virtual void onCameraTranslation() = 0;

	/**
	 * On rotation event to be overloaded
	 */
	virtual void onCameraRotation() = 0;

	/**
	 * On scale event to be overloaded
	 */
	virtual void onCameraScale() = 0;


};
