#include <tdme/tools/shared/controller/EntityBaseSubScreenController.h>

#include <map>
#include <string>
#include <vector>

#include <tdme/gui/GUIParser.h>
#include <tdme/gui/events/Action.h>
#include <tdme/gui/events/GUIActionListener_Type.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUINodeController.h>
#include <tdme/gui/nodes/GUIParentNode.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/tools/shared/controller/InfoDialogScreenController.h>
#include <tdme/tools/shared/model/LevelEditorEntity.h>
#include <tdme/tools/shared/model/LevelPropertyPresets.h>
#include <tdme/tools/shared/model/PropertyModelClass.h>
#include <tdme/tools/shared/views/EntityBaseView.h>
#include <tdme/tools/shared/views/PopUps.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>
#include <tdme/utilities/MutableString.h>

using std::map;
using std::vector;
using std::string;

using tdme::tools::shared::controller::EntityBaseSubScreenController;
using tdme::gui::GUIParser;
using tdme::gui::events::Action;
using tdme::gui::events::GUIActionListener_Type;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeController;
using tdme::gui::nodes::GUIParentNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::tools::shared::controller::InfoDialogScreenController;
using tdme::tools::shared::model::LevelEditorEntity;
using tdme::tools::shared::model::LevelPropertyPresets;
using tdme::tools::shared::model::PropertyModelClass;
using tdme::tools::shared::views::EntityBaseView;
using tdme::tools::shared::views::PopUps;
using tdme::utilities::MutableString;
using tdme::utilities::Console;
using tdme::utilities::Exception;

MutableString EntityBaseSubScreenController::TEXT_EMPTY = MutableString("");

EntityBaseSubScreenController::EntityBaseSubScreenController(PopUps* popUps, Action* onSetEntityDataAction) 
{
	this->view = new EntityBaseView(this);
	this->popUps = popUps;
	this->onSetEntityDataAction = onSetEntityDataAction;
}

EntityBaseSubScreenController::~EntityBaseSubScreenController() {
	delete view;
	delete onSetEntityDataAction;
}

void EntityBaseSubScreenController::initialize(GUIScreenNode* screenNode)
{
	try {
		entityName = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("entity_name"));
		entityDescription = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("entity_description"));
		entityApply = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_entity_apply"));
		entityPropertyName = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("entity_property_name"));
		entityPropertyValue = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("entity_property_value"));
		entityPropertySave = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_entity_properties_save"));
		entityPropertyAdd = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_entity_properties_add"));
		entityPropertyRemove = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_entity_properties_remove"));
		entityPropertiesList = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("entity_properties_listbox"));
		entityPropertyPresetApply = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_entity_properties_presetapply"));
		entityPropertiesPresets = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("entity_properties_presets"));
	} catch (Exception& exception) {
		Console::print(string("EntityBaseSubScreenController::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	setEntityPresetIds(LevelPropertyPresets::getInstance()->getObjectPropertiesPresets());
}

void EntityBaseSubScreenController::setEntityData(const string& name, const string& description)
{
	entityName->getController()->setDisabled(false);
	entityName->getController()->setValue(name);
	entityDescription->getController()->setDisabled(false);
	entityDescription->getController()->setValue(description);
	entityApply->getController()->setDisabled(false);
}

void EntityBaseSubScreenController::unsetEntityData()
{
	entityName->getController()->setValue(TEXT_EMPTY);
	entityName->getController()->setDisabled(true);
	entityDescription->getController()->setValue(TEXT_EMPTY);
	entityDescription->getController()->setDisabled(true);
	entityApply->getController()->setDisabled(true);
}

void EntityBaseSubScreenController::onEntityDataApply(LevelEditorEntity* model)
{
	if (model == nullptr)
		return;

	view->setEntityData(model, entityName->getController()->getValue().getString(), entityDescription->getController()->getValue().getString());
	onSetEntityDataAction->performAction();
}

void EntityBaseSubScreenController::setEntityPresetIds(const map<string, vector<PropertyModelClass*>>& entityPresetIds)
{
	auto entityPropertiesPresetsInnerNode = dynamic_cast< GUIParentNode* >((entityPropertiesPresets->getScreenNode()->getNodeById(entityPropertiesPresets->getId() + "_inner")));
	auto idx = 0;
	string entityPropertiesPresetsInnerNodeSubNodesXML = "";
	entityPropertiesPresetsInnerNodeSubNodesXML =
		entityPropertiesPresetsInnerNodeSubNodesXML +
		"<scrollarea-vertical id=\"" +
		entityPropertiesPresets->getId() +
		"_inner_scrollarea\" width=\"100%\" height=\"100\">\n";
	for (auto it: entityPresetIds) {
		auto entityPresetId = it.first;
		entityPropertiesPresetsInnerNodeSubNodesXML =
			entityPropertiesPresetsInnerNodeSubNodesXML + "<dropdown-option text=\"" +
			GUIParser::escapeQuotes(entityPresetId) +
			"\" value=\"" +
			GUIParser::escapeQuotes(entityPresetId) +
			"\" " +
			(idx == 0 ? "selected=\"true\" " : "") +
			" />\n";
		idx++;
	}
	entityPropertiesPresetsInnerNodeSubNodesXML = entityPropertiesPresetsInnerNodeSubNodesXML + "</scrollarea-vertical>";
	try {
		entityPropertiesPresetsInnerNode->replaceSubNodes(entityPropertiesPresetsInnerNodeSubNodesXML, true);
	} catch (Exception& exception) {
		Console::print(string("EntityBaseSubScreenController::setEntityPresetIds(): An error occurred: "));
		Console::println(string(exception.what()));
	}
}

void EntityBaseSubScreenController::setEntityProperties(LevelEditorEntity* entity, const string& presetId, const string& selectedName)
{
	entityPropertiesPresets->getController()->setDisabled(false);
	entityPropertyPresetApply->getController()->setDisabled(false);
	entityPropertiesList->getController()->setDisabled(false);
	entityPropertyAdd->getController()->setDisabled(false);
	entityPropertyRemove->getController()->setDisabled(false);
	entityPropertySave->getController()->setDisabled(true);
	entityPropertyName->getController()->setDisabled(true);
	entityPropertyValue->getController()->setDisabled(true);
	entityPropertiesPresets->getController()->setValue(presetId.length() > 0 ? MutableString(presetId) : MutableString("none"));
	auto entityPropertiesListBoxInnerNode = dynamic_cast< GUIParentNode* >((entityPropertiesList->getScreenNode()->getNodeById(entityPropertiesList->getId() + "_inner")));
	auto idx = 1;
	string entityPropertiesListBoxSubNodesXML = "";
	entityPropertiesListBoxSubNodesXML =
		entityPropertiesListBoxSubNodesXML +
		"<scrollarea-vertical id=\"" +
		entityPropertiesList->getId() +
		"_inner_scrollarea\" width=\"100%\" height=\"100%\">\n";
	for (auto i = 0; i < entity->getPropertyCount(); i++) {
		PropertyModelClass* entityProperty = entity->getPropertyByIndex(i);
		entityPropertiesListBoxSubNodesXML =
			entityPropertiesListBoxSubNodesXML +
			"<selectbox-option text=\"" +
			GUIParser::escapeQuotes(entityProperty->getName()) +
			": " +
			GUIParser::escapeQuotes(entityProperty->getValue()) +
			"\" value=\"" +
			GUIParser::escapeQuotes(entityProperty->getName()) +
			"\" " +
			(selectedName.length() > 0 && entityProperty->getName() == selectedName ? "selected=\"true\" " : "") +
			"/>\n";
	}
	entityPropertiesListBoxSubNodesXML = entityPropertiesListBoxSubNodesXML + "</scrollarea-vertical>\n";
	try {
		entityPropertiesListBoxInnerNode->replaceSubNodes(entityPropertiesListBoxSubNodesXML, false);
	} catch (Exception& exception) {
		Console::print(string("EntityBaseSubScreenController::setEntityProperties(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	onEntityPropertiesSelectionChanged(entity);
}

void EntityBaseSubScreenController::unsetEntityProperties()
{
	auto modelPropertiesListBoxInnerNode = dynamic_cast< GUIParentNode* >((entityPropertiesList->getScreenNode()->getNodeById(entityPropertiesList->getId() + "_inner")));
	modelPropertiesListBoxInnerNode->clearSubNodes();
	entityPropertiesPresets->getController()->setValue(MutableString("none"));
	entityPropertiesPresets->getController()->setDisabled(true);
	entityPropertyPresetApply->getController()->setDisabled(true);
	entityPropertiesList->getController()->setDisabled(true);
	entityPropertyAdd->getController()->setDisabled(true);
	entityPropertyRemove->getController()->setDisabled(true);
	entityPropertySave->getController()->setDisabled(true);
	entityPropertyName->getController()->setValue(TEXT_EMPTY);
	entityPropertyName->getController()->setDisabled(true);
	entityPropertyValue->getController()->setValue(TEXT_EMPTY);
	entityPropertyValue->getController()->setDisabled(true);
}

void EntityBaseSubScreenController::onEntityPropertySave(LevelEditorEntity* entity)
{
	if (view->entityPropertySave(
		entity,
		entityPropertiesList->getController()->getValue().getString(),
		entityPropertyName->getController()->getValue().getString(),
		entityPropertyValue->getController()->getValue().getString()) == false) {
		showErrorPopUp("Warning", "Saving entity property failed");
	}
}

void EntityBaseSubScreenController::onEntityPropertyAdd(LevelEditorEntity* entity)
{
	if (view->entityPropertyAdd(entity) == false) {
		showErrorPopUp("Warning", "Adding new entity property failed");
	}
}

void EntityBaseSubScreenController::onEntityPropertyRemove(LevelEditorEntity* entity)
{
	if (view->entityPropertyRemove(entity, entityPropertiesList->getController()->getValue().getString()) == false) {
		showErrorPopUp("Warning", "Removing entity property failed");
	}
}

void EntityBaseSubScreenController::showErrorPopUp(const string& caption, const string& message)
{
	popUps->getInfoDialogScreenController()->show(caption, message);
}

void EntityBaseSubScreenController::onEntityPropertyPresetApply(LevelEditorEntity* model)
{
	view->entityPropertiesPreset(model, entityPropertiesPresets->getController()->getValue().getString());
}

void EntityBaseSubScreenController::onEntityPropertiesSelectionChanged(LevelEditorEntity* entity)
{
	entityPropertyName->getController()->setDisabled(true);
	entityPropertyName->getController()->setValue(TEXT_EMPTY);
	entityPropertyValue->getController()->setDisabled(true);
	entityPropertyValue->getController()->setValue(TEXT_EMPTY);
	entityPropertySave->getController()->setDisabled(true);
	entityPropertyRemove->getController()->setDisabled(true);
	auto entityProperty = entity->getProperty(entityPropertiesList->getController()->getValue().getString());
	if (entityProperty != nullptr) {
		entityPropertyName->getController()->setValue(MutableString(entityProperty->getName()));
		entityPropertyValue->getController()->setValue(MutableString(entityProperty->getValue()));
		entityPropertyName->getController()->setDisabled(false);
		entityPropertyValue->getController()->setDisabled(false);
		entityPropertySave->getController()->setDisabled(false);
		entityPropertyRemove->getController()->setDisabled(false);
	}
}

void EntityBaseSubScreenController::onValueChanged(GUIElementNode* node, LevelEditorEntity* model)
{
	if (node == entityPropertiesList) {
		onEntityPropertiesSelectionChanged(model);
	} else {
	}
}

void EntityBaseSubScreenController::onActionPerformed(GUIActionListener_Type* type, GUIElementNode* node, LevelEditorEntity* entity)
{
	{
		auto v = type;
		if (v == GUIActionListener_Type::PERFORMED)
		{
			if (node->getId().compare("button_entity_apply") == 0) {
				onEntityDataApply(entity);
			} else
			if (node->getId().compare("button_entity_properties_presetapply") == 0) {
				onEntityPropertyPresetApply(entity);
			} else
			if (node->getId().compare("button_entity_properties_add") == 0) {
				onEntityPropertyAdd(entity);
			} else
			if (node->getId().compare("button_entity_properties_remove") == 0) {
				onEntityPropertyRemove(entity);
			} else
			if (node->getId().compare("button_entity_properties_save") == 0) {
				onEntityPropertySave(entity);
			}
		}
	}

}
