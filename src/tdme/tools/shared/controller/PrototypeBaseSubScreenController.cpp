#include <tdme/tools/shared/controller/PrototypeBaseSubScreenController.h>

#include <map>
#include <string>
#include <vector>

#include <tdme/engine/prototype/Prototype.h>
#include <tdme/engine/prototype/PrototypeProperty.h>
#include <tdme/engine/scene/ScenePropertyPresets.h>
#include <tdme/gui/events/Action.h>
#include <tdme/gui/events/GUIActionListener.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUINodeController.h>
#include <tdme/gui/nodes/GUIParentNode.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/gui/GUIParser.h>
#include <tdme/tools/shared/controller/InfoDialogScreenController.h>
#include <tdme/tools/shared/views/PopUps.h>
#include <tdme/tools/shared/views/PrototypeBaseView.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>
#include <tdme/utilities/MutableString.h>

using std::map;
using std::string;
using std::vector;

using tdme::engine::prototype::Prototype;
using tdme::engine::prototype::PrototypeProperty;
using tdme::engine::scene::ScenePropertyPresets;
using tdme::gui::events::Action;
using tdme::gui::events::GUIActionListenerType;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeController;
using tdme::gui::nodes::GUIParentNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::GUIParser;
using tdme::tools::shared::controller::InfoDialogScreenController;
using tdme::tools::shared::controller::PrototypeBaseSubScreenController;
using tdme::tools::shared::views::PopUps;
using tdme::tools::shared::views::PrototypeBaseView;
using tdme::utilities::Console;
using tdme::utilities::Exception;
using tdme::utilities::MutableString;

PrototypeBaseSubScreenController::PrototypeBaseSubScreenController(PopUps* popUps, Action* onSetEntityDataAction)
{
	this->view = new PrototypeBaseView(this);
	this->popUps = popUps;
	this->onSetPrototypeDataAction = onSetEntityDataAction;
}

PrototypeBaseSubScreenController::~PrototypeBaseSubScreenController() {
	delete view;
	delete onSetPrototypeDataAction;
}

void PrototypeBaseSubScreenController::initialize(GUIScreenNode* screenNode)
{
	try {
		prototypeName = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("entity_name"));
		prototypeDescription = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("entity_description"));
		prototypeApply = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_entity_apply"));
		prototypePropertyName = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("entity_property_name"));
		prototypePropertyValue = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("entity_property_value"));
		prototypePropertySave = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_entity_properties_save"));
		prototypePropertyAdd = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_entity_properties_add"));
		prototypePropertyRemove = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_entity_properties_remove"));
		prototypePropertiesList = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("entity_properties_listbox"));
		prototypePropertyPresetApply = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_entity_properties_presetapply"));
		prototypePropertiesPresets = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("entity_properties_presets"));
	} catch (Exception& exception) {
		Console::print(string("PrototypeBaseSubScreenController::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	setPrototypePresetIds(ScenePropertyPresets::getInstance()->getEntityPropertiesPresets());
}

void PrototypeBaseSubScreenController::setPrototypeData(const string& name, const string& description)
{
	prototypeName->getController()->setDisabled(false);
	prototypeName->getController()->setValue(name);
	prototypeDescription->getController()->setDisabled(false);
	prototypeDescription->getController()->setValue(description);
	prototypeApply->getController()->setDisabled(false);
}

void PrototypeBaseSubScreenController::unsetPrototypeData()
{
	prototypeName->getController()->setValue(MutableString());
	prototypeName->getController()->setDisabled(true);
	prototypeDescription->getController()->setValue(MutableString());
	prototypeDescription->getController()->setDisabled(true);
	prototypeApply->getController()->setDisabled(true);
}

void PrototypeBaseSubScreenController::onPrototypeDataApply(Prototype* prototype)
{
	if (prototype == nullptr)
		return;

	view->setPrototypeData(prototype, prototypeName->getController()->getValue().getString(), prototypeDescription->getController()->getValue().getString());
	onSetPrototypeDataAction->performAction();
}

void PrototypeBaseSubScreenController::setPrototypePresetIds(const map<string, vector<PrototypeProperty*>>& prototypePresetIds)
{
	auto prototypePropertiesPresetsInnerNode = dynamic_cast<GUIParentNode*>((prototypePropertiesPresets->getScreenNode()->getNodeById(prototypePropertiesPresets->getId() + "_inner")));
	auto idx = 0;
	string prototypePropertiesPresetsInnerNodeSubNodesXML = "";
	prototypePropertiesPresetsInnerNodeSubNodesXML =
		prototypePropertiesPresetsInnerNodeSubNodesXML +
		"<scrollarea id=\"" +
		prototypePropertiesPresets->getId() +
		"_inner_scrollarea\" width=\"100%\" height=\"100\">\n";
	for (auto it: prototypePresetIds) {
		auto prototypePresetId = it.first;
		prototypePropertiesPresetsInnerNodeSubNodesXML =
			prototypePropertiesPresetsInnerNodeSubNodesXML + "<dropdown-option text=\"" +
			GUIParser::escapeQuotes(prototypePresetId) +
			"\" value=\"" +
			GUIParser::escapeQuotes(prototypePresetId) +
			"\" " +
			(idx == 0 ? "selected=\"true\" " : "") +
			" />\n";
		idx++;
	}
	prototypePropertiesPresetsInnerNodeSubNodesXML = prototypePropertiesPresetsInnerNodeSubNodesXML + "</scrollarea>";
	try {
		prototypePropertiesPresetsInnerNode->replaceSubNodes(prototypePropertiesPresetsInnerNodeSubNodesXML, true);
	} catch (Exception& exception) {
		Console::print(string("PrototypeBaseSubScreenController::setPrototypePresetIds(): An error occurred: "));
		Console::println(string(exception.what()));
	}
}

void PrototypeBaseSubScreenController::setPrototypeProperties(Prototype* prototype, const string& presetId, const string& selectedName)
{
	prototypePropertiesPresets->getController()->setDisabled(false);
	prototypePropertyPresetApply->getController()->setDisabled(false);
	prototypePropertiesList->getController()->setDisabled(false);
	prototypePropertyAdd->getController()->setDisabled(false);
	prototypePropertyRemove->getController()->setDisabled(false);
	prototypePropertySave->getController()->setDisabled(true);
	prototypePropertyName->getController()->setDisabled(true);
	prototypePropertyValue->getController()->setDisabled(true);
	prototypePropertiesPresets->getController()->setValue(presetId.length() > 0 ? MutableString(presetId) : MutableString("none"));
	auto prototypePropertiesListBoxInnerNode = dynamic_cast<GUIParentNode*>((prototypePropertiesList->getScreenNode()->getNodeById(prototypePropertiesList->getId() + "_inner")));
	auto idx = 1;
	string prototypePropertiesListBoxSubNodesXML = "";
	prototypePropertiesListBoxSubNodesXML =
		prototypePropertiesListBoxSubNodesXML +
		"<scrollarea id=\"" +
		prototypePropertiesList->getId() +
		"_inner_scrollarea\" width=\"100%\" height=\"100%\">\n";
	for (auto i = 0; i < prototype->getPropertyCount(); i++) {
		PrototypeProperty* entityProperty = prototype->getPropertyByIndex(i);
		prototypePropertiesListBoxSubNodesXML =
			prototypePropertiesListBoxSubNodesXML +
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
	prototypePropertiesListBoxSubNodesXML = prototypePropertiesListBoxSubNodesXML + "</scrollarea>\n";
	try {
		prototypePropertiesListBoxInnerNode->replaceSubNodes(prototypePropertiesListBoxSubNodesXML, false);
	} catch (Exception& exception) {
		Console::print(string("PrototypeBaseSubScreenController::setEntityProperties(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	onPrototypePropertiesSelectionChanged(prototype);
}

void PrototypeBaseSubScreenController::unsetPrototypeProperties()
{
	auto prototypePropertiesListBoxInnerNode = dynamic_cast<GUIParentNode*>((prototypePropertiesList->getScreenNode()->getNodeById(prototypePropertiesList->getId() + "_inner")));
	prototypePropertiesListBoxInnerNode->clearSubNodes();
	prototypePropertiesPresets->getController()->setValue(MutableString("none"));
	prototypePropertiesPresets->getController()->setDisabled(true);
	prototypePropertyPresetApply->getController()->setDisabled(true);
	prototypePropertiesList->getController()->setDisabled(true);
	prototypePropertyAdd->getController()->setDisabled(true);
	prototypePropertyRemove->getController()->setDisabled(true);
	prototypePropertySave->getController()->setDisabled(true);
	prototypePropertyName->getController()->setValue(MutableString());
	prototypePropertyName->getController()->setDisabled(true);
	prototypePropertyValue->getController()->setValue(MutableString());
	prototypePropertyValue->getController()->setDisabled(true);
}

void PrototypeBaseSubScreenController::onEntityPropertySave(Prototype* prototype)
{
	if (view->prototypePropertySave(
		prototype,
		prototypePropertiesList->getController()->getValue().getString(),
		prototypePropertyName->getController()->getValue().getString(),
		prototypePropertyValue->getController()->getValue().getString()) == false) {
		showErrorPopUp("Warning", "Saving prototype property failed");
	}
}

void PrototypeBaseSubScreenController::onPrototypePropertyAdd(Prototype* prototype)
{
	if (view->prototypePropertyAdd(prototype) == false) {
		showErrorPopUp("Warning", "Adding new prototype property failed");
	}
}

void PrototypeBaseSubScreenController::onPrototypePropertyRemove(Prototype* prototype)
{
	if (view->prototypePropertyRemove(prototype, prototypePropertiesList->getController()->getValue().getString()) == false) {
		showErrorPopUp("Warning", "Removing prototype property failed");
	}
}

void PrototypeBaseSubScreenController::onPrototypePropertyPresetApply(Prototype* prototype)
{
	view->prototypePropertiesPreset(prototype, prototypePropertiesPresets->getController()->getValue().getString());
}

void PrototypeBaseSubScreenController::onPrototypePropertiesSelectionChanged(Prototype* entity)
{
	prototypePropertyName->getController()->setDisabled(true);
	prototypePropertyName->getController()->setValue(MutableString());
	prototypePropertyValue->getController()->setDisabled(true);
	prototypePropertyValue->getController()->setValue(MutableString());
	prototypePropertySave->getController()->setDisabled(true);
	prototypePropertyRemove->getController()->setDisabled(true);
	auto entityProperty = entity->getProperty(prototypePropertiesList->getController()->getValue().getString());
	if (entityProperty != nullptr) {
		prototypePropertyName->getController()->setValue(MutableString(entityProperty->getName()));
		prototypePropertyValue->getController()->setValue(MutableString(entityProperty->getValue()));
		prototypePropertyName->getController()->setDisabled(false);
		prototypePropertyValue->getController()->setDisabled(false);
		prototypePropertySave->getController()->setDisabled(false);
		prototypePropertyRemove->getController()->setDisabled(false);
	}
}

void PrototypeBaseSubScreenController::onValueChanged(GUIElementNode* node, Prototype* model)
{
	if (node == prototypePropertiesList) {
		onPrototypePropertiesSelectionChanged(model);
	} else {
	}
}

void PrototypeBaseSubScreenController::onActionPerformed(GUIActionListenerType type, GUIElementNode* node, Prototype* entity)
{
	if (type == GUIActionListenerType::PERFORMED)
	{
		if (node->getId().compare("button_entity_apply") == 0) {
			onPrototypeDataApply(entity);
		} else
		if (node->getId().compare("button_entity_properties_presetapply") == 0) {
			onPrototypePropertyPresetApply(entity);
		} else
		if (node->getId().compare("button_entity_properties_add") == 0) {
			onPrototypePropertyAdd(entity);
		} else
		if (node->getId().compare("button_entity_properties_remove") == 0) {
			onPrototypePropertyRemove(entity);
		} else
		if (node->getId().compare("button_entity_properties_save") == 0) {
			onEntityPropertySave(entity);
		}
	}
}

void PrototypeBaseSubScreenController::showErrorPopUp(const string& caption, const string& message)
{
	popUps->getInfoDialogScreenController()->show(caption, message);
}
