
#pragma once

#include <tdme/tdme.h>
#include <tdme/gui/events/fwd-tdme.h>
#include <tdme/gui/events/GUIActionListener.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/tools/shared/controller/fwd-tdme.h>
#include <tdme/tools/shared/views/fwd-tdme.h>
#include <tdme/utilities/fwd-tdme.h>
#include <tdme/utilities/MutableString.h>

using tdme::gui::events::GUIActionListenerType;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::tools::shared::views::PrototypeDisplayView;
using tdme::tools::shared::views::PrototypePhysicsView;
using tdme::utilities::MutableString;

/**
 * Prototype display sub screen controller
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::shared::controller::PrototypeDisplaySubScreenController final
{
private:
	static MutableString CHECKBOX_CHECKED;
	static MutableString CHECKBOX_UNCHECKED;
	GUIElementNode* displayBoundingVolume { nullptr };
	GUIElementNode* displayShadowing { nullptr };
	GUIElementNode* displayGround { nullptr };
	PrototypeDisplayView* view { nullptr };
	PrototypePhysicsView* physicsView { nullptr };

public:
	/**
	 * Public constructor
	 * @param physicsView physics view
	 */
	PrototypeDisplaySubScreenController(PrototypePhysicsView* physicsView);

	/**
	 * Destructor
	 */
	~PrototypeDisplaySubScreenController();

	/**
	 * @return view
	 */
	PrototypeDisplayView* getView();

	/**
	 * Init
	 * @param screenNode screen node
	 */
	void initialize(GUIScreenNode* screenNode);

	/**
	 * Set up display section
	 */
	void setupDisplay();

	/**
	 * On display apply button event
	 */
	void onDisplayApply();

	/**
	 * @return display shadowing checked
	 */
	bool getDisplayShadowing();

	/**
	 * @return display ground checked
	 */
	bool getDisplayGround();

	/**
	 * @return display bounding volume checked
	 */
	bool getDisplayBoundingVolume();
	void onActionPerformed(GUIActionListenerType type, GUIElementNode* node);

};
