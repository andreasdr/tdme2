#pragma once

#include <map>
#include <string>
#include <vector>

#include <tdme/engine/prototype/fwd-tdme.h>
#include <tdme/gui/events/fwd-tdme.h>
#include <tdme/gui/events/GUIActionListener.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/tools/shared/controller/fwd-tdme.h>
#include <tdme/tools/shared/views/fwd-tdme.h>
#include <tdme/utilities/fwd-tdme.h>

using std::map;
using std::string;
using std::vector;

using tdme::engine::prototype::Prototype;
using tdme::engine::prototype::PrototypeProperty;
using tdme::gui::events::Action;
using tdme::gui::events::GUIActionListenerType;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::tools::shared::views::PopUps;
using tdme::tools::shared::views::PrototypeBaseView;
using tdme::utilities::MutableString;

/**
 * Prototype base sub screen controller
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::shared::controller::PrototypeBaseSubScreenController final
{
private:
	static MutableString TEXT_EMPTY;
	PrototypeBaseView* view { nullptr };
	PopUps* popUps { nullptr };
	GUIElementNode* prototypeName { nullptr };
	GUIElementNode* prototypeDescription { nullptr };
	GUIElementNode* prototypeApply { nullptr };
	GUIElementNode* prototypePropertyName { nullptr };
	GUIElementNode* prototypePropertyValue { nullptr };
	GUIElementNode* prototypePropertySave { nullptr };
	GUIElementNode* prototypePropertyAdd { nullptr };
	GUIElementNode* prototypePropertyRemove { nullptr };
	GUIElementNode* prototypePropertiesList { nullptr };
	GUIElementNode* prototypePropertyPresetApply { nullptr };
	GUIElementNode* prototypePropertiesPresets { nullptr };
	Action* onSetPrototypeDataAction { nullptr };

public:
	/**
	 * Public constructor
	 * @param popUps view
	 * @param onSetEntityDataAction on set entity data action
	 */
	PrototypeBaseSubScreenController(PopUps* popUps, Action* onSetEntityDataAction);

	/**
	 * Destructor
	 */
	~PrototypeBaseSubScreenController();

	/**
	 * Init
	 * @param screenNode screen node
	 */
	void initialize(GUIScreenNode* screenNode);

	/**
	 * Set up general prototype data
	 * @param name name
	 * @param description description
	 */
	void setPrototypeData(const string& name, const string& description);

	/**
	 * Unset entity data
	 */
	void unsetPrototypeData();

	/**
	 * On prototype data apply
	 * @param model model
	 */
	void onPrototypeDataApply(Prototype* model);

	/**
	 * Set up prototype property preset ids
	 * @param prototypePresetIds prototype property preset ids
	 */
	void setPrototypePresetIds(const map<string, vector<PrototypeProperty*>>& prototypePresetIds);

	/**
	 * Set up prototype properties
	 * @param prototype prototype
	 * @param presetId preset id
	 * @param selectedName selected name
	 */
	void setPrototypeProperties(Prototype* prototype, const string& presetId, const string& selectedName);

	/**
	 * Unset prototype properties
	 */
	void unsetPrototypeProperties();

	/**
	 * On entity property save
	 * @param entity entity
	 */
	void onEntityPropertySave(Prototype* prototype);

	/**
	 * On prototype property add
	 * @param prototype prototype
	 */
	void onPrototypePropertyAdd(Prototype* prototype);

	/**
	 * On prototype property remove
	 * @param entity entity
	 */
	void onPrototypePropertyRemove(Prototype* prototype);

	/**
	 * On prototype property preset apply
	 * @param model model
	 */
	void onPrototypePropertyPresetApply(Prototype* prototype);

	/**
	 * Event callback for entity properties selection
	 * @param prototype prototype
	 */
	void onPrototypePropertiesSelectionChanged(Prototype* prototype);

	/**
	 * On value changed
	 * @param node node
	 * @param model model
	 */
	void onValueChanged(GUIElementNode* node, Prototype* model);

	/**
	 * On action performed
	 * @param type type
	 * @param node node
	 * @param entity entity
	 */
	void onActionPerformed(GUIActionListenerType type, GUIElementNode* node, Prototype* entity);

	/**
	 * Shows the error pop up
	 * @param caption caption
	 * @param message message
	 */
	void showErrorPopUp(const string& caption, const string& message);

};
