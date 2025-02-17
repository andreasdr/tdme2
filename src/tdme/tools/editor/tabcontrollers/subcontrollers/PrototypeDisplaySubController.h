#pragma once

#include <array>
#include <memory>

#include <agui/agui.h>
#include <agui/gui/events/fwd-agui.h>
#include <agui/gui/events/GUIActionListener.h>
#include <agui/gui/nodes/fwd-agui.h>
#include <agui/utilities/MutableString.h>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/prototype/fwd-tdme.h>
#include <tdme/engine/EntityShaderParameters.h>
#include <tdme/tools/editor/misc/fwd-tdme.h>
#include <tdme/tools/editor/tabcontrollers/subcontrollers/fwd-tdme.h>
#include <tdme/tools/editor/tabviews/fwd-tdme.h>
#include <tdme/tools/editor/tabviews/subviews/fwd-tdme.h>
#include <tdme/tools/editor/views/fwd-tdme.h>
#include <tdme/utilities/fwd-tdme.h>

using std::array;
using std::unique_ptr;

using agui::gui::events::GUIActionListenerType;
using agui::gui::nodes::GUIElementNode;
using agui::gui::nodes::GUIScreenNode;
using agui::utilities::MutableString;

using tdme::engine::prototype::Prototype;
using tdme::engine::Engine;
using tdme::engine::EntityShaderParameters;
using tdme::tools::editor::misc::PopUps;
using tdme::tools::editor::tabviews::subviews::PrototypeDisplaySubView;
using tdme::tools::editor::tabviews::subviews::PrototypePhysicsSubView;
using tdme::tools::editor::tabviews::TabView;
using tdme::tools::editor::views::EditorView;

/**
 * Prototype display sub screen controller
 * @author Andreas Drewke
 */
class tdme::tools::editor::tabcontrollers::subcontrollers::PrototypeDisplaySubController final
{
private:
	GUIScreenNode* screenNode { nullptr };
	EditorView* editorView { nullptr };
	TabView* tabView { nullptr };
	unique_ptr<PrototypeDisplaySubView> view;
	PrototypePhysicsSubView* physicsView { nullptr };
	PopUps* popUps { nullptr };

	bool displayShadowing { true };
	bool displayGround { true };
	bool displayBoundingVolumes { true };

	array<string, 6> applyDisplayNodes = {
		"rendering_shader",
		"rendering_distance_shader",
		"rendering_contributes_shadows",
		"rendering_receives_shadows",
		"rendering_render_groups"
	};

public:
	// forbid class copy
	FORBID_CLASS_COPY(PrototypeDisplaySubController)

	/**
	 * Public constructor
	 * @param editorView editor view
	 * @param tabView tab view
	 * @param physicsView physics view
	 */
	PrototypeDisplaySubController(EditorView* editorView, TabView* tabView, PrototypePhysicsSubView* physicsView);

	/**
	 * Destructor
	 */
	~PrototypeDisplaySubController();

	/**
	 * @returns view
	 */
	inline PrototypeDisplaySubView* getView() {
		return view.get();
	}

	/**
	 * Init
	 * @param screenNode screen node
	 */
	void initialize(GUIScreenNode* screenNode);

	/**
	 * @returns display shadowing checked
	 */
	inline bool getDisplayShadowing() {
		return displayShadowing;
	}

	/**
	 * @returns display ground checked
	 */
	inline bool getDisplayGround() {
		return displayGround;
	}

	/**
	 * @returns display bounding volume checked
	 */
	inline bool getDisplayBoundingVolume() {
		return displayBoundingVolumes;
	}

	/**
	 * Create display properties XML
	 * @param prototype prototype
	 * @param xml xml
	 */
	void createDisplayPropertiesXML(Prototype* prototype, string& xml);

	/**
	 * Set display details
	 * @param prototype prototype
	 */
	void setDisplayDetails(Prototype* prototype);

	/**
	 * Update details panel
	 * @param prototype prototype
	 * @param outlinerNode outliner node
	 */
	void updateDetails(Prototype* prototype, const string& outlinerNode);

	/**
	 * Apply display details
	 * @param prototype prototype
	 */
	void applyDisplayDetails(Prototype* prototype);

	/**
	 * Set display shader details
	 * @param prototype prototype
	 */
	void setDisplayShaderDetails(Prototype* prototype);

	/**
	 * Apply display shader details
	 * @param prototype prototype
	 * @param parameterName parameter name
	 * @param shaderParameters shader parameters
	 */
	void applyDisplayShaderDetails(Prototype* prototype, const string& parameterName, EntityShaderParameters& shaderParameters);

	/**
	 * On value changed
	 * @param node node
	 * @param prototype prototype
	 * @returns if this event has been handled
	 */
	bool onChange(GUIElementNode* node, Prototype* prototype);

	/**
	 * On action
	 * @param type type
	 * @param node element node
	 * @param prototype prototype
	 */
	bool onAction(GUIActionListenerType type, GUIElementNode* node, Prototype* prototype);

	/**
	 * Show the information pop up / modal
	 * @param caption caption
	 * @param message message
	 */
	void showInfoPopUp(const string& caption, const string& message);

};
