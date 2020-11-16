#pragma once

#include <map>
#include <string>
#include <vector>

#include <tdme/gui/events/fwd-tdme.h>
#include <tdme/gui/events/GUIActionListener.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/tools/shared/controller/fwd-tdme.h>
#include <tdme/tools/shared/model/fwd-tdme.h>
#include <tdme/tools/shared/views/fwd-tdme.h>
#include <tdme/utilities/fwd-tdme.h>

using std::map;
using std::string;
using std::vector;

using tdme::gui::events::Action;
using tdme::gui::events::GUIActionListenerType;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::tools::shared::model::LevelEditorEntity;
using tdme::tools::shared::model::PropertyModelClass;
using tdme::tools::shared::views::EntityBaseView;
using tdme::tools::shared::views::PopUps;
using tdme::utilities::MutableString;

/**
 * Entity base sub screen controller
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::shared::controller::EntityBaseSubScreenController final
{
private:
	static MutableString TEXT_EMPTY;
	EntityBaseView* view { nullptr };
	PopUps* popUps { nullptr };
	GUIElementNode* entityName { nullptr };
	GUIElementNode* entityDescription { nullptr };
	GUIElementNode* entityApply { nullptr };
	GUIElementNode* entityPropertyName { nullptr };
	GUIElementNode* entityPropertyValue { nullptr };
	GUIElementNode* entityPropertySave { nullptr };
	GUIElementNode* entityPropertyAdd { nullptr };
	GUIElementNode* entityPropertyRemove { nullptr };
	GUIElementNode* entityPropertiesList { nullptr };
	GUIElementNode* entityPropertyPresetApply { nullptr };
	GUIElementNode* entityPropertiesPresets { nullptr };
	Action* onSetEntityDataAction { nullptr };

public:
	/**
	 * Public constructor
	 * @param popUps view
	 * @param onSetEntityDataAction on set entity data action
	 */
	EntityBaseSubScreenController(PopUps* popUps, Action* onSetEntityDataAction);

	/**
	 * Destructor
	 */
	~EntityBaseSubScreenController();

	/**
	 * Init
	 * @param screenNode screen node
	 */
	void initialize(GUIScreenNode* screenNode);

	/**
	 * Set up general entity data
	 * @param name name
	 * @param description description
	 */
	void setEntityData(const string& name, const string& description);

	/**
	 * Unset entity data
	 */
	void unsetEntityData();

	/**
	 * On entity data apply
	 * @param model model
	 */
	void onEntityDataApply(LevelEditorEntity* model);

	/**
	 * Set up entity property preset ids
	 * @param entityPresetIds entity property preset ids
	 */
	void setEntityPresetIds(const map<string, vector<PropertyModelClass*>>& entityPresetIds); // TODO: std container: maybe use call by reference

	/**
	 * Set up entity properties
	 * @param entity entity
	 * @param presetId preset id
	 * @param selectedName selected name
	 */
	void setEntityProperties(LevelEditorEntity* entity, const string& presetId, const string& selectedName);

	/**
	 * Unset entity properties
	 */
	void unsetEntityProperties();

	/**
	 * On entity property save
	 * @param entity entity
	 */
	void onEntityPropertySave(LevelEditorEntity* entity);

	/**
	 * On entity property add
	 * @param entity entity
	 */
	void onEntityPropertyAdd(LevelEditorEntity* entity);

	/**
	 * On entity property remove
	 * @param entity entity
	 */
	void onEntityPropertyRemove(LevelEditorEntity* entity);

	/**
	 * Shows the error pop up
	 */
	void showErrorPopUp(const string& caption, const string& message);

	/**
	 * On entity property preset apply
	 * @param model model
	 */
	void onEntityPropertyPresetApply(LevelEditorEntity* model);

	/**
	 * Event callback for entity properties selection
	 * @param entity
	 */
	void onEntityPropertiesSelectionChanged(LevelEditorEntity* entity);

	/**
	 * On value changed
	 * @param node node
	 * @param model model
	 */
	void onValueChanged(GUIElementNode* node, LevelEditorEntity* model);

	/**
	 * On action performed
	 * @param type type
	 * @param node node
	 * @param entity entity
	 */
	void onActionPerformed(GUIActionListenerType type, GUIElementNode* node, LevelEditorEntity* entity);

};
