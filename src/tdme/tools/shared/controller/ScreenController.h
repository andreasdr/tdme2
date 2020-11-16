
#pragma once

#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/tools/shared/controller/fwd-tdme.h>

using tdme::gui::nodes::GUIScreenNode;

/**
 * Screen controller, which connects GUI screen definition with code
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::shared::controller::ScreenController
{
public:
	/**
	 * Destructor
	 */
	virtual ~ScreenController() {}

	/**
	 * @return screen node
	 */
	virtual GUIScreenNode* getScreenNode() = 0;

	/**
	 * Init
	 */
	virtual void initialize() = 0;

	/**
	 * Dispose
	 */
	virtual void dispose() = 0;

};
