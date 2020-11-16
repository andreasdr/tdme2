#include <tdme/tools/leveleditor/controller/LevelEditorScreenController.h>

#include <array>
#include <vector>
#include <string>

#include <tdme/engine/fileio/models/ModelReader.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/gui/GUIParser.h>
#include <tdme/gui/events/Action.h>
#include <tdme/gui/events/GUIActionListener.h>
#include <tdme/gui/events/GUIChangeListener.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUINodeController.h>
#include <tdme/gui/nodes/GUIParentNode.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/gui/nodes/GUITextNode.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Vector4.h>
#include <tdme/tools/leveleditor/TDMELevelEditor.h>
#include <tdme/tools/leveleditor/views/LevelEditorView.h>
#include <tdme/tools/shared/controller/FileDialogPath.h>
#include <tdme/tools/shared/controller/FileDialogScreenController.h>
#include <tdme/tools/shared/controller/InfoDialogScreenController.h>
#include <tdme/tools/shared/model/LevelEditorEntity.h>
#include <tdme/tools/shared/model/LevelEditorLevel.h>
#include <tdme/tools/shared/model/LevelEditorLight.h>
#include <tdme/tools/shared/model/LevelEditorObject.h>
#include <tdme/tools/shared/model/LevelPropertyPresets.h>
#include <tdme/tools/shared/model/PropertyModelClass.h>
#include <tdme/tools/shared/tools/Tools.h>
#include <tdme/tools/shared/views/PopUps.h>
#include <tdme/utilities/Float.h>
#include <tdme/utilities/MutableString.h>
#include <tdme/utilities/StringTools.h>
#include <tdme/utilities/StringTokenizer.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>
#include <tdme/utilities/ExceptionBase.h>

using std::array;
using std::vector;
using std::string;
using std::to_string;

using tdme::tools::leveleditor::controller::LevelEditorScreenController;

using tdme::engine::fileio::models::ModelReader;
using tdme::engine::model::Color4;
using tdme::gui::GUIParser;
using tdme::gui::events::GUIActionListenerType;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeController;
using tdme::gui::nodes::GUIParentNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::nodes::GUITextNode;
using tdme::math::Vector3;
using tdme::math::Vector4;
using tdme::tools::leveleditor::TDMELevelEditor;
using tdme::tools::leveleditor::views::LevelEditorView;
using tdme::tools::shared::controller::FileDialogPath;
using tdme::tools::shared::controller::FileDialogScreenController;
using tdme::tools::shared::controller::InfoDialogScreenController;
using tdme::tools::shared::model::LevelEditorEntity;
using tdme::tools::shared::model::LevelEditorLevel;
using tdme::tools::shared::model::LevelEditorLight;
using tdme::tools::shared::model::LevelEditorObject;
using tdme::tools::shared::model::LevelPropertyPresets;
using tdme::tools::shared::model::PropertyModelClass;
using tdme::tools::shared::tools::Tools;
using tdme::tools::shared::views::PopUps;
using tdme::utilities::Float;
using tdme::utilities::MutableString;
using tdme::utilities::StringTokenizer;
using tdme::utilities::StringTools;
using tdme::utilities::Exception;
using tdme::utilities::ExceptionBase;
using tdme::utilities::Console;

MutableString LevelEditorScreenController::CHECKBOX_CHECKED = MutableString("1");
MutableString LevelEditorScreenController::CHECKBOX_UNCHECKED = MutableString("");
MutableString LevelEditorScreenController::TEXT_EMPTY = MutableString("");

LevelEditorScreenController::LevelEditorScreenController(LevelEditorView* view)
{
	this->view = view;
	this->mapPath = new FileDialogPath(".");
}

GUIScreenNode* LevelEditorScreenController::getScreenNode()
{
	return screenNode;
}

FileDialogPath* LevelEditorScreenController::getMapPath()
{
	return mapPath;
}

void LevelEditorScreenController::initialize()
{
	try {
		screenNode = GUIParser::parse("resources/engine/tools/leveleditor/gui", "screen_leveleditor.xml");
		screenNode->addActionListener(this);
		screenNode->addChangeListener(this);
		screenCaption = dynamic_cast< GUITextNode* >(screenNode->getNodeById("screen_caption"));
		gridEnabled = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("grid_enabled"));
		gridYPosition = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("grid_y_position"));
		snappingX = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("snapping_x"));
		snappingZ = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("snapping_z"));
		snappingEnabled = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("snapping_enabled"));
		mapWidth = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("map_width"));
		mapDepth = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("map_depth"));
		mapHeight = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("map_height"));
		mapPropertyName = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("map_property_name"));
		mapPropertyValue = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("map_property_value"));
		mapPropertySave = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_map_properties_save"));
		mapPropertyRemove = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_map_properties_remove"));
		mapPropertiesListBox = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("map_properties_listbox"));
		objectName = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("object_name"));
		objectDescription = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("object_description"));
		objectModel = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("object_model"));
		objectCenter = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("object_center"));
		btnObjectDataApply = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_objectdata_apply"));
		btnObjectTranslationApply = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_translation_apply"));
		btnObjectScaleApply = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_scale_apply"));
		btnObjectRotationApply = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_rotation_apply"));
		btnObjectColor = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_object_color"));
		btnObjectCenter = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_object_center"));
		btnObjectRemove = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_object_remove"));
		objectTranslationX = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("object_translation_x"));
		objectTranslationY = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("object_translation_y"));
		objectTranslationZ = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("object_translation_z"));
		objectScaleX = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("object_scale_x"));
		objectScaleY = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("object_scale_y"));
		objectScaleZ = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("object_scale_z"));
		objectRotationX = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("object_rotation_x"));
		objectRotationY = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("object_rotation_y"));
		objectRotationZ = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("object_rotation_z"));
		objectPropertyName = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("object_property_name"));
		objectPropertyValue = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("object_property_value"));
		btnObjectPropertySave = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_object_properties_save"));
		btnObjectPropertyRemove = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_object_properties_remove"));
		btnObjectPropertyAdd = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_object_properties_add"));
		btnObjectPropertyRemove = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_object_properties_remove"));
		btnObjectPropertyPresetApply = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_object_properties_presetapply"));
		objectPropertiesListBox = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("object_properties_listbox"));
		objectPropertiesPresets = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("object_properties_presets"));
		objectsListBox = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("objects_listbox"));
		mapWidth->getController()->setDisabled(true);
		mapDepth->getController()->setDisabled(true);
		mapHeight->getController()->setDisabled(true);
		objectModel->getController()->setDisabled(true);
		objectCenter->getController()->setDisabled(true);
		for (auto i = 0; i < 4; i++) {
			lightsPresets[i] = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("presets_light" + to_string(i)));
			lightsAmbient[i] = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("light" + to_string(i) + "_ambient"));
			lightsDiffuse[i] = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("light" + to_string(i) + "_diffuse"));
			lightsSpecular[i] = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("light" + to_string(i) + "_specular"));
			lightsPosition[i] = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("light" + to_string(i) + "_position"));
			lightsLinAttenuation[i] = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("light" + to_string(i) + "_linear_attenuation"));
			lightsConstAttenuation[i] = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("light" + to_string(i) + "_constant_attenuation"));
			lightsQuadAttenuation[i] = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("light" + to_string(i) + "_quadratic_attenuation"));
			lightsSpotTo[i] = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("light" + to_string(i) + "_spot_to"));
			lightsSpotDirection[i] = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("light" + to_string(i) + "_spot_direction"));
			lightsSpotExponent[i] = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("light" + to_string(i) + "_spot_exponent"));
			lightsSpotCutoff[i] = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("light" + to_string(i) + "_spot_cutoff"));
			ligthsSpotDirectionCompute[i] = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_light" + to_string(i) + "_spotdirection_compute"));
			lightsEnabled[i] = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("light" + to_string(i) + "_enabled"));
		}
		viewPort = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("viewport"));
 	} catch (Exception& exception) {
		Console::print(string("LevelEditorScreenController::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
}

void LevelEditorScreenController::dispose()
{
}

void LevelEditorScreenController::setScreenCaption(const string& text)
{
	screenCaption->setText(text);
}

void LevelEditorScreenController::setGrid(bool enabled, float gridY)
{
	gridEnabled->getController()->setValue(enabled == true ? CHECKBOX_CHECKED : CHECKBOX_UNCHECKED);
	gridYPosition->getController()->setValue(MutableString(Tools::formatFloat(gridY)));
}

void LevelEditorScreenController::setSnapping(bool snappingEnabled, float snappingX, float snappingZ) {
	this->snappingEnabled->getController()->setValue(snappingEnabled == true ? CHECKBOX_CHECKED : CHECKBOX_UNCHECKED);
	this->snappingX->getController()->setValue(MutableString(Tools::formatFloat(snappingX)));
	this->snappingZ->getController()->setValue(MutableString(Tools::formatFloat(snappingZ)));
}

void LevelEditorScreenController::setLevelSize(float width, float depth, float height)
{
	mapWidth->getController()->setValue(MutableString(Tools::formatFloat(width)));
	mapDepth->getController()->setValue(MutableString(Tools::formatFloat(depth)));
	mapHeight->getController()->setValue(MutableString(Tools::formatFloat(height)));
}

void LevelEditorScreenController::unsetObjectProperties()
{
	objectPropertiesPresets->getController()->setValue(MutableString("none"));
	objectPropertiesPresets->getController()->setDisabled(true);
	btnObjectPropertyPresetApply->getController()->setDisabled(true);
	objectPropertiesListBox->getController()->setDisabled(true);
	btnObjectPropertyAdd->getController()->setDisabled(true);
	btnObjectPropertyRemove->getController()->setDisabled(true);
	btnObjectPropertySave->getController()->setDisabled(true);
	objectPropertyName->getController()->setValue(TEXT_EMPTY);
	objectPropertyName->getController()->setDisabled(true);
	objectPropertyValue->getController()->setValue(TEXT_EMPTY);
	objectPropertyValue->getController()->setDisabled(true);
	auto objectPropertiesListBoxInnerNode = dynamic_cast< GUIParentNode* >((objectPropertiesListBox->getScreenNode()->getNodeById(objectPropertiesListBox->getId() + "_inner")));
	objectPropertiesListBoxInnerNode->clearSubNodes();
}

const string LevelEditorScreenController::getObjectPropertyPresetSelection()
{
	return "";
}

void LevelEditorScreenController::setObjectData(const string& name, const string& description, const string& modelName, const Vector3& center)
{
	objectName->getController()->setDisabled(false);
	objectName->getController()->setValue(MutableString(name));
	objectDescription->getController()->setDisabled(false);
	objectDescription->getController()->setValue(MutableString(description));
	objectModel->getController()->setValue(MutableString(modelName));
	objectCenter->getController()->setValue(MutableString().append(Tools::formatFloat(center.getX())).append(", ").append(Tools::formatFloat(center.getY())).append(", ").append(Tools::formatFloat(center.getZ())));
	btnObjectDataApply->getController()->setDisabled(false);
}

void LevelEditorScreenController::unsetObjectData()
{
	objectName->getController()->setValue(TEXT_EMPTY);
	objectName->getController()->setDisabled(true);
	objectDescription->getController()->setValue(TEXT_EMPTY);
	objectDescription->getController()->setDisabled(true);
	objectModel->getController()->setValue(TEXT_EMPTY);
	objectModel->getController()->setDisabled(true);
	objectCenter->getController()->setValue(TEXT_EMPTY);
	objectCenter->getController()->setDisabled(true);
	btnObjectDataApply->getController()->setDisabled(true);
}

void LevelEditorScreenController::onObjectDataApply()
{
	if (view->objectDataApply(objectName->getController()->getValue().getString(), objectDescription->getController()->getValue().getString()) == false) {
		showErrorPopUp("Warning", "Changing object data failed");
	}
}

void LevelEditorScreenController::setObjectListbox(LevelEditorLevel* level)
{
	auto selectedObjects = objectsListBox->getController()->getValue();
	auto objectsListBoxInnerNode = dynamic_cast< GUIParentNode* >((objectsListBox->getScreenNode()->getNodeById(objectsListBox->getId() + "_inner")));
	auto idx = 1;
	string objectsListBoxSubNodesXML = "";
	objectsListBoxSubNodesXML =
		objectsListBoxSubNodesXML +
		"<scrollarea-vertical id=\"" +
		objectsListBox->getId() +
		"_inner_scrollarea\" width=\"100%\" height=\"100%\">\n";
	auto objectIdx = 0;
	for (int i = 0; i < level->getObjectCount(); i++) {
		if (objectIdx > 25000) {
			objectsListBoxSubNodesXML =
				"<scrollarea-vertical id=\"" +
				objectsListBox->getId() +
				"_inner_scrollarea\" width=\"100%\" height=\"100%\">\n";
			break;
		}
		auto object = level->getObjectAt(i);
		if (object->getEntity()->isRenderGroups() == true) continue;
		auto objectId = object->getId();
		objectsListBoxSubNodesXML =
			objectsListBoxSubNodesXML +
			"<selectbox-multiple-option text=\"" +
			GUIParser::escapeQuotes(objectId) +
			"\" value=\"" +
			GUIParser::escapeQuotes(objectId) +
			"\" " +
			"/>\n";
		objectIdx++;
	}
	objectsListBoxSubNodesXML =
		objectsListBoxSubNodesXML +
		"</scrollarea-vertical>\n";
	try {
		objectsListBoxInnerNode->replaceSubNodes(objectsListBoxSubNodesXML, false);
	} catch (Exception& exception) {
		Console::print(string("LevelEditorScreenController::setObjectListbox(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	objectsListBox->getController()->setValue(selectedObjects);
}

void LevelEditorScreenController::unselectObjectInObjectListBox(const string& objectId)
{
	auto selectedObjects = objectsListBox->getController()->getValue();
	auto value = MutableString().set(u'|').append(objectId).append(u'|');
	auto pos = selectedObjects.indexOf(value);
	if (pos != -1) selectedObjects.delete_(pos, value.length());
	objectsListBox->getController()->setValue(selectedObjects);
}

void LevelEditorScreenController::unselectObjectsInObjectListBox()
{
	objectsListBox->getController()->setValue(MutableString());
}

void LevelEditorScreenController::selectObjectInObjectListbox(const string& objectId)
{
	auto selectedObjects = objectsListBox->getController()->getValue();
	auto value = MutableString().set(u'|').append(objectId).append(u'|');
	auto pos = selectedObjects.indexOf(value);
	if (pos == -1) selectedObjects.append(value);
	objectsListBox->getController()->setValue(selectedObjects);
}

void LevelEditorScreenController::onObjectsSelect()
{
	vector<string> selectedObjectList;
	StringTokenizer t;
	t.tokenize(objectsListBox->getController()->getValue().getString(), "|");
	while (t.hasMoreTokens()) {
		selectedObjectList.push_back(t.nextToken());
	}
	if (selectedObjectList.empty() == false)
		view->selectObjects(selectedObjectList);

}

void LevelEditorScreenController::onObjectsUnselect()
{
	view->unselectObjects();
}

void LevelEditorScreenController::setObject(const Vector3& translation, const Vector3& scale, float rotationX, float rotationY, float rotationZ)
{
	btnObjectTranslationApply->getController()->setDisabled(false);
	btnObjectScaleApply->getController()->setDisabled(false);
	btnObjectRotationApply->getController()->setDisabled(false);
	btnObjectColor->getController()->setDisabled(false);
	btnObjectCenter->getController()->setDisabled(false);
	btnObjectRemove->getController()->setDisabled(false);
	objectTranslationX->getController()->setDisabled(false);
	objectTranslationY->getController()->setDisabled(false);
	objectTranslationZ->getController()->setDisabled(false);
	objectScaleX->getController()->setDisabled(false);
	objectScaleY->getController()->setDisabled(false);
	objectScaleZ->getController()->setDisabled(false);
	objectRotationX->getController()->setDisabled(false);
	objectRotationY->getController()->setDisabled(false);
	objectRotationZ->getController()->setDisabled(false);
	objectTranslationX->getController()->setValue(MutableString(Tools::formatFloat(translation.getX())));
	objectTranslationY->getController()->setValue(MutableString(Tools::formatFloat(translation.getY())));
	objectTranslationZ->getController()->setValue(MutableString(Tools::formatFloat(translation.getZ())));
	objectScaleX->getController()->setValue(MutableString(Tools::formatFloat(scale.getX())));
	objectScaleY->getController()->setValue(MutableString(Tools::formatFloat(scale.getY())));
	objectScaleZ->getController()->setValue(MutableString(Tools::formatFloat(scale.getZ())));
	objectRotationX->getController()->setValue(MutableString(Tools::formatFloat(rotationX)));
	objectRotationY->getController()->setValue(MutableString(Tools::formatFloat(rotationY)));
	objectRotationZ->getController()->setValue(MutableString(Tools::formatFloat(rotationZ)));
}

void LevelEditorScreenController::unsetObject()
{
	btnObjectTranslationApply->getController()->setDisabled(true);
	btnObjectScaleApply->getController()->setDisabled(true);
	btnObjectRotationApply->getController()->setDisabled(true);
	btnObjectColor->getController()->setDisabled(true);
	btnObjectCenter->getController()->setDisabled(true);
	btnObjectRemove->getController()->setDisabled(true);
	objectTranslationX->getController()->setDisabled(true);
	objectTranslationY->getController()->setDisabled(true);
	objectTranslationZ->getController()->setDisabled(true);
	objectScaleX->getController()->setDisabled(true);
	objectScaleY->getController()->setDisabled(true);
	objectScaleZ->getController()->setDisabled(true);
	objectRotationX->getController()->setDisabled(true);
	objectRotationY->getController()->setDisabled(true);
	objectRotationZ->getController()->setDisabled(true);
	objectTranslationX->getController()->setValue(TEXT_EMPTY);
	objectTranslationY->getController()->setValue(TEXT_EMPTY);
	objectTranslationZ->getController()->setValue(TEXT_EMPTY);
	objectScaleX->getController()->setValue(TEXT_EMPTY);
	objectScaleY->getController()->setValue(TEXT_EMPTY);
	objectScaleZ->getController()->setValue(TEXT_EMPTY);
	objectRotationX->getController()->setValue(TEXT_EMPTY);
	objectRotationY->getController()->setValue(TEXT_EMPTY);
	objectRotationZ->getController()->setValue(TEXT_EMPTY);
}

void LevelEditorScreenController::onMapPropertiesSelectionChanged()
{
	mapPropertyName->getController()->setDisabled(true);
	mapPropertyName->getController()->setValue(TEXT_EMPTY);
	mapPropertyValue->getController()->setDisabled(true);
	mapPropertyValue->getController()->setValue(TEXT_EMPTY);
	mapPropertySave->getController()->setDisabled(true);
	mapPropertyRemove->getController()->setDisabled(true);
	auto mapProperty = view->getLevel()->getProperty(mapPropertiesListBox->getController()->getValue().getString());
	if (mapProperty != nullptr) {
		mapPropertyName->getController()->setValue(MutableString(mapProperty->getName()));
		mapPropertyValue->getController()->setValue(MutableString(mapProperty->getValue()));
		mapPropertyName->getController()->setDisabled(false);
		mapPropertyValue->getController()->setDisabled(false);
		mapPropertySave->getController()->setDisabled(false);
		mapPropertyRemove->getController()->setDisabled(false);
	}
}

void LevelEditorScreenController::setMapProperties(LevelEditorLevel* level, const string& selectedName)
{
	mapPropertyName->getController()->setDisabled(true);
	mapPropertyValue->getController()->setDisabled(true);
	mapPropertySave->getController()->setDisabled(true);
	auto mapPropertiesListBoxInnerNode = dynamic_cast< GUIParentNode* >((mapPropertiesListBox->getScreenNode()->getNodeById(mapPropertiesListBox->getId() + "_inner")));
	auto idx = 1;
	string mapPropertiesListBoxSubNodesXML = "";
	mapPropertiesListBoxSubNodesXML =
		mapPropertiesListBoxSubNodesXML +
		"<scrollarea-vertical id=\"" +
		mapPropertiesListBox->getId() +
		"_inner_scrollarea\" width=\"100%\" height=\"100%\">\n";
	for (auto i = 0; i < level->getPropertyCount(); i++) {
		PropertyModelClass* mapProperty = level->getPropertyByIndex(i);
		mapPropertiesListBoxSubNodesXML =
			mapPropertiesListBoxSubNodesXML +
			"<selectbox-option text=\"" +
			GUIParser::escapeQuotes(mapProperty->getName()) +
			": " +
			GUIParser::escapeQuotes(mapProperty->getValue()) +
			"\" value=\"" +
			GUIParser::escapeQuotes(mapProperty->getName()) +
			"\" " +
			(selectedName.length() > 0 && mapProperty->getName() == selectedName ? "selected=\"true\" " : "") +
			"/>\n";
	}
	mapPropertiesListBoxSubNodesXML =
		mapPropertiesListBoxSubNodesXML +
		"</scrollarea-vertical>\n";
	try {
		mapPropertiesListBoxInnerNode->replaceSubNodes(mapPropertiesListBoxSubNodesXML, false);
	} catch (Exception& exception) {
		Console::print(string("LevelEditorScreenController::setMapProperties(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	onMapPropertiesSelectionChanged();
}

void LevelEditorScreenController::onMapPropertySave()
{
	if (view->mapPropertySave(
		mapPropertiesListBox->getController()->getValue().getString(),
		mapPropertyName->getController()->getValue().getString(),
		mapPropertyValue->getController()->getValue().getString()) == false) {
		showErrorPopUp("Warning", "Saving map property failed");
	}
}

void LevelEditorScreenController::onMapPropertyAdd()
{
	if (view->mapPropertyAdd() == false) {
		showErrorPopUp("Warning", "Adding new map property failed");
	}
}

void LevelEditorScreenController::onMapPropertyRemove()
{
	if (view->mapPropertyRemove(mapPropertiesListBox->getController()->getValue().getString()) == false) {
		showErrorPopUp("Warning", "Removing map property failed");
	}
}

void LevelEditorScreenController::setObjectPresetIds(const map<string, vector<PropertyModelClass*>>& objectPresetIds)
{
	auto objectPropertiesPresetsInnerNode = dynamic_cast< GUIParentNode* >((objectPropertiesPresets->getScreenNode()->getNodeById(objectPropertiesPresets->getId() + "_inner")));
	auto idx = 0;
	string objectPropertiesPresetsInnerNodeSubNodesXML = "";
	objectPropertiesPresetsInnerNodeSubNodesXML =
		objectPropertiesPresetsInnerNodeSubNodesXML +
		"<scrollarea-vertical id=\"" +
		objectPropertiesPresets->getId() +
		"_inner_scrollarea\" width=\"100%\" height=\"100\">\n";
	for (auto it: objectPresetIds) {
		auto modelPresetId = it.first;
		objectPropertiesPresetsInnerNodeSubNodesXML =
			objectPropertiesPresetsInnerNodeSubNodesXML +
			"<dropdown-option text=\"" +
			GUIParser::escapeQuotes(modelPresetId) +
			"\" value=\"" +
			GUIParser::escapeQuotes(modelPresetId) +
			"\" " +
			(idx == 0 ? "selected=\"true\" " : "") +
			" />\n";
		idx++;
	}
	objectPropertiesPresetsInnerNodeSubNodesXML =
		objectPropertiesPresetsInnerNodeSubNodesXML +
		"</scrollarea-vertical>\n";
	try {
		objectPropertiesPresetsInnerNode->replaceSubNodes(objectPropertiesPresetsInnerNodeSubNodesXML, true);
	} catch (Exception& exception) {
		Console::print(string("LevelEditorScreenController::setObjectPresetIds(): An error occurred: "));
		Console::println(string(exception.what()));
	}
}

void LevelEditorScreenController::onObjectPropertiesSelectionChanged()
{
	objectPropertyName->getController()->setDisabled(true);
	objectPropertyName->getController()->setValue(TEXT_EMPTY);
	objectPropertyValue->getController()->setDisabled(true);
	objectPropertyValue->getController()->setValue(TEXT_EMPTY);
	btnObjectPropertySave->getController()->setDisabled(true);
	btnObjectPropertyRemove->getController()->setDisabled(true);
	auto levelEditorObject = view->getSelectedObject();
	if (levelEditorObject == nullptr)
		return;

	auto modelProperty = levelEditorObject->getProperty(objectPropertiesListBox->getController()->getValue().getString());
	if (modelProperty != nullptr) {
		objectPropertyName->getController()->setValue(MutableString(modelProperty->getName()));
		objectPropertyValue->getController()->setValue(MutableString(modelProperty->getValue()));
		objectPropertyName->getController()->setDisabled(false);
		objectPropertyValue->getController()->setDisabled(false);
		btnObjectPropertySave->getController()->setDisabled(false);
		btnObjectPropertyRemove->getController()->setDisabled(false);
	}
}

void LevelEditorScreenController::setObjectProperties(const string& presetId, LevelEditorObject* object, const string& selectedName)
{
	objectPropertiesPresets->getController()->setDisabled(false);
	btnObjectPropertyPresetApply->getController()->setDisabled(false);
	objectPropertiesListBox->getController()->setDisabled(false);
	btnObjectPropertyAdd->getController()->setDisabled(false);
	btnObjectPropertyRemove->getController()->setDisabled(false);
	btnObjectPropertySave->getController()->setDisabled(true);
	objectPropertyName->getController()->setDisabled(true);
	objectPropertyValue->getController()->setDisabled(true);
	objectPropertiesPresets->getController()->setValue(presetId.length() > 0 ? MutableString(presetId) : MutableString("none"));
	auto objectPropertiesListBoxInnerNode = dynamic_cast< GUIParentNode* >((objectPropertiesListBox->getScreenNode()->getNodeById(objectPropertiesListBox->getId() + "_inner")));
	auto idx = 1;
	string objectPropertiesListBoxSubNodesXML = "";
	objectPropertiesListBoxSubNodesXML =
		objectPropertiesListBoxSubNodesXML +
		"<scrollarea-vertical id=\"" +
		objectPropertiesListBox->getId() +
		"_inner_scrollarea\" width=\"100%\" height=\"100%\">\n";
	for (auto i = 0; i < object->getPropertyCount(); i++) {
	PropertyModelClass* objectProperty = object->getPropertyByIndex(i);
		objectPropertiesListBoxSubNodesXML =
			objectPropertiesListBoxSubNodesXML +
			"<selectbox-option text=\"" +
			GUIParser::escapeQuotes(objectProperty->getName()) +
			": " +
			GUIParser::escapeQuotes(objectProperty->getValue()) +
			"\" value=\"" +
			GUIParser::escapeQuotes(objectProperty->getName()) +
			"\" " +
			(selectedName.length() > 0 && objectProperty->getName() == selectedName? "selected=\"true\" " : "") +
			"/>\n";
	}
	objectPropertiesListBoxSubNodesXML =
		objectPropertiesListBoxSubNodesXML +
		"</scrollarea-vertical>\n";
	try {
		objectPropertiesListBoxInnerNode->replaceSubNodes(objectPropertiesListBoxSubNodesXML, false);
	} catch (Exception& exception) {
		Console::print(string("LevelEditorScreenController::setObjectProperties(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	onObjectPropertiesSelectionChanged();
}

void LevelEditorScreenController::onObjectPropertySave()
{
	if (view->objectPropertySave(
		objectPropertiesListBox->getController()->getValue().getString(),
		objectPropertyName->getController()->getValue().getString(),
		objectPropertyValue->getController()->getValue().getString()) == false) {
		showErrorPopUp("Warning", "Saving object property failed");
	}
}

void LevelEditorScreenController::onObjectPropertyAdd()
{
	if (view->objectPropertyAdd() == false) {
		showErrorPopUp("Warning", "Adding new object property failed");
	}
}

void LevelEditorScreenController::onObjectPropertyRemove()
{
	if (view->objectPropertyRemove(objectPropertiesListBox->getController()->getValue().getString()) == false) {
		showErrorPopUp("Warning", "Removing object property failed");
	}
}

void LevelEditorScreenController::onQuit()
{
	TDMELevelEditor::getInstance()->quit();
}

void LevelEditorScreenController::onObjectTranslationApply()
{
	try {
		auto x = Float::parseFloat(objectTranslationX->getController()->getValue().getString());
		auto y = Float::parseFloat(objectTranslationY->getController()->getValue().getString());
		auto z = Float::parseFloat(objectTranslationZ->getController()->getValue().getString());
		view->objectTranslationApply(x, y, z);
	} catch (Exception& exception) {
		showErrorPopUp("Warning", (exception.what()));
	}
}

void LevelEditorScreenController::onObjectScaleApply()
{
	try {
		auto x = Float::parseFloat(objectScaleX->getController()->getValue().getString());
		auto y = Float::parseFloat(objectScaleY->getController()->getValue().getString());
		auto z = Float::parseFloat(objectScaleZ->getController()->getValue().getString());
		if (x < -10.0f || x > 10.0f)
			throw ExceptionBase("x scale must be within -10 .. +10");

		if (y < -10.0f || y > 10.0f)
			throw ExceptionBase("y scale must be within -10 .. +10");

		if (z < -10.0f || z > 10.0f)
			throw ExceptionBase("z scale must be within -10 .. +10");

		view->objectScaleApply(x, y, z);
	} catch (Exception& exception) {
		showErrorPopUp("Warning", (exception.what()));
	}
}

void LevelEditorScreenController::onObjectRotationsApply()
{
	try {
		auto x = Float::parseFloat(objectRotationX->getController()->getValue().getString());
		auto y = Float::parseFloat(objectRotationY->getController()->getValue().getString());
		auto z = Float::parseFloat(objectRotationZ->getController()->getValue().getString());
		if (x < -360.0f || x > 360.0f)
			throw ExceptionBase("x axis rotation must be within -360 .. +360");

		if (y < -360.0f || y > 360.0f)
			throw ExceptionBase("y axis rotation must be within -360 .. +360");

		if (z < -360.0f || z > 360.0f)
			throw ExceptionBase("z axis rotation must be within -360 .. +360");

		view->objectRotationsApply(x, y, z);
	} catch (Exception& exception) {
		showErrorPopUp("Warning", (exception.what()));
	}
}

void LevelEditorScreenController::onObjectRemove()
{
	view->removeObjects();
}

void LevelEditorScreenController::onObjectColor()
{
	view->colorObject();
}

void LevelEditorScreenController::onObjectCenter()
{
	view->centerObject();
}

void LevelEditorScreenController::onMapLoad()
{
	class OnMapLoadAction: public virtual Action
	{
	public:
		void performAction() override {
			levelEditorScreenController->view->loadMap(levelEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getPathName(), levelEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getFileName());
			levelEditorScreenController->mapPath->setPath(levelEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getPathName());
			levelEditorScreenController->view->getPopUps()->getFileDialogScreenController()->close();
		}

		/**
		 * Public constructor
		 * @param levelEditorScreenController level editor screen controller
		 */
		OnMapLoadAction(LevelEditorScreenController* levelEditorScreenController): levelEditorScreenController(levelEditorScreenController) {
		}

	private:
		LevelEditorScreenController* levelEditorScreenController;
	};

	vector<string> extensions = ModelReader::getModelExtensions();
	extensions.push_back("tl");
	view->getPopUps()->getFileDialogScreenController()->show(
		mapPath->getPath(),
		"Load from: ",
		extensions,
		view->getFileName(),
		true,
		new OnMapLoadAction(this)
	);
}

void LevelEditorScreenController::onMapSave()
{
	class OnMapSaveAction: public virtual Action
	{
	public:
		void performAction() override {
			levelEditorScreenController->view->saveMap(levelEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getPathName(), levelEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getFileName());
			levelEditorScreenController->mapPath->setPath(levelEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getPathName());
			levelEditorScreenController->view->getPopUps()->getFileDialogScreenController()->close();
		}

		/**
		 * Public constructor
		 * @param levelEditorScreenController level editor screen controller
		 */
		OnMapSaveAction(LevelEditorScreenController* levelEditorScreenController): levelEditorScreenController(levelEditorScreenController) {
		}

	private:
		LevelEditorScreenController* levelEditorScreenController;
	};

	vector<string> extensions = {
		"tl",
	};
	view->getPopUps()->getFileDialogScreenController()->show(
		mapPath->getPath(),
		"Save to: ",
		extensions,
		view->getFileName(),
		false,
		new OnMapSaveAction(this)
	);
}

void LevelEditorScreenController::onGridApply()
{
	try {
		auto gridY = Float::parseFloat(gridYPosition->getController()->getValue().getString());
		if (gridY < -5.0f || gridY > 5.0f)
			throw ExceptionBase("grid y position must be within -5 .. +5");

		view->setGridY(gridY);
		view->setGridEnabled(gridEnabled->getController()->getValue().equals(CHECKBOX_CHECKED));
	} catch (Exception& exception) {
		showErrorPopUp("Warning", (exception.what()));
	}
}

void LevelEditorScreenController::onSnappingApply()
{
	try {
		auto snappingXValue = Float::parseFloat(snappingX->getController()->getValue().getString());
		if (snappingXValue < 0.0f || snappingXValue > 5.0f) throw ExceptionBase("snapping x must be within 0 .. 5");
		auto snappingZValue = Float::parseFloat(snappingZ->getController()->getValue().getString());
		if (snappingZValue < 0.0f || snappingZValue > 5.0f) throw ExceptionBase("snapping x must be within 0 .. 5");
		view->setSnapping(snappingEnabled->getController()->getValue().equals(CHECKBOX_CHECKED), snappingXValue, snappingZValue);
	} catch (Exception& exception) {
		showErrorPopUp("Warning", (exception.what()));
	}
}

void LevelEditorScreenController::onObjectPropertyPresetApply()
{
	view->objectPropertiesPreset(objectPropertiesPresets->getController()->getValue().getString());
}

void LevelEditorScreenController::setLightPresetsIds(const map<string, LevelEditorLight*>& lightPresetIds)
{
	for (auto i = 0; i < 4; i++) {
		auto lightPresetsInnerNode = dynamic_cast< GUIParentNode* >((lightsPresets[i]->getScreenNode()->getNodeById(lightsPresets[i]->getId() + "_inner")));
		auto idx = 0;
		string lightPresetsInnerNodeSubNodesXML = "";
		lightPresetsInnerNodeSubNodesXML =
			lightPresetsInnerNodeSubNodesXML +
			"<scrollarea-vertical id=\"" +
			lightsPresets[i]->getId() +
			"_inner_scrollarea\" width=\"100%\" height=\"50\">\n";
		for (auto it: lightPresetIds) {
			string lightPresetId = it.first;
			lightPresetsInnerNodeSubNodesXML =
				lightPresetsInnerNodeSubNodesXML +
				"<dropdown-option text=\"" +
				GUIParser::escapeQuotes(lightPresetId) +
				"\" value=\"" +
				GUIParser::escapeQuotes(lightPresetId) +
				"\" " +
				(idx == 0 ? "selected=\"true\" " : "") +
				" />\n";
			idx++;
		}
		lightPresetsInnerNodeSubNodesXML =
			lightPresetsInnerNodeSubNodesXML +
			"</scrollarea-vertical>\n";
		try {
			lightPresetsInnerNode->replaceSubNodes(lightPresetsInnerNodeSubNodesXML, true);
		} catch (Exception& exception) {
			Console::print(string("LevelEditorScreenController::setLightPresetsIds(): An error occurred: "));
			Console::println(string(exception.what()));
		}
	}
}

void LevelEditorScreenController::unselectLightPresets()
{
}

void LevelEditorScreenController::setLight(int32_t i, const Color4& ambient, const Color4& diffuse, const Color4& specular, const Vector4& position, float constAttenuation, float linearAttenuation, float quadraticAttenuation, const Vector3& spotTo, const Vector3& spotDirection, float spotExponent, float spotCutoff, bool enabled)
{
	lightsAmbient[i]->getController()->setValue(MutableString(Tools::formatFloat(ambient.getRed())).append(", ").append(Tools::formatFloat(ambient.getGreen())).append(", ").append(Tools::formatFloat(ambient.getBlue())).append(", ").append(Tools::formatFloat(ambient.getAlpha())));
	lightsDiffuse[i]->getController()->setValue(MutableString(Tools::formatFloat(diffuse.getRed())).append(", ").append(Tools::formatFloat(diffuse.getGreen())).append(", ").append(Tools::formatFloat(diffuse.getBlue())).append(", ").append(Tools::formatFloat(diffuse.getAlpha())));
	lightsSpecular[i]->getController()->setValue(MutableString(Tools::formatFloat(specular.getRed())).append(", ").append(Tools::formatFloat(specular.getGreen())).append(", ").append(Tools::formatFloat(specular.getBlue())).append(", ").append(Tools::formatFloat(specular.getAlpha())));
	lightsPosition[i]->getController()->setValue(MutableString(Tools::formatFloat(position.getX())).append(", ").append(Tools::formatFloat(position.getY())).append(", ").append(Tools::formatFloat(position.getZ())).append(", ").append(Tools::formatFloat(position.getW())));
	lightsConstAttenuation[i]->getController()->setValue(MutableString(Tools::formatFloat(constAttenuation)));
	lightsLinAttenuation[i]->getController()->setValue(MutableString(Tools::formatFloat(linearAttenuation)));
	lightsQuadAttenuation[i]->getController()->setValue(MutableString(Tools::formatFloat(quadraticAttenuation)));
	lightsSpotTo[i]->getController()->setValue(MutableString(Tools::formatFloat(spotTo.getX())).append(", ").append(Tools::formatFloat(spotTo.getY())).append(", ").append(Tools::formatFloat(spotTo.getZ())));
	lightsSpotDirection[i]->getController()->setValue(MutableString(Tools::formatFloat(spotDirection.getX())).append(", ").append(Tools::formatFloat(spotDirection.getY())).append(", ").append(Tools::formatFloat(spotDirection.getZ())));
	lightsSpotExponent[i]->getController()->setValue(MutableString(Tools::formatFloat(spotExponent)));
	lightsSpotCutoff[i]->getController()->setValue(MutableString(Tools::formatFloat(spotCutoff)));
	lightsEnabled[i]->getController()->setValue(enabled == true ? CHECKBOX_CHECKED : CHECKBOX_UNCHECKED);
	lightsAmbient[i]->getController()->setDisabled(enabled == false);
	lightsDiffuse[i]->getController()->setDisabled(enabled == false);
	lightsSpecular[i]->getController()->setDisabled(enabled == false);
	lightsPosition[i]->getController()->setDisabled(enabled == false);
	lightsConstAttenuation[i]->getController()->setDisabled(enabled == false);
	lightsLinAttenuation[i]->getController()->setDisabled(enabled == false);
	lightsQuadAttenuation[i]->getController()->setDisabled(enabled == false);
	lightsSpotTo[i]->getController()->setDisabled(enabled == false);
	lightsSpotDirection[i]->getController()->setDisabled(enabled == false);
	lightsSpotExponent[i]->getController()->setDisabled(enabled == false);
	lightsSpotCutoff[i]->getController()->setDisabled(enabled == false);
	ligthsSpotDirectionCompute[i]->getController()->setDisabled(enabled == false);
}

void LevelEditorScreenController::onLight0Apply()
{
	onLightApply(0);
}

void LevelEditorScreenController::onLight1Apply()
{
	onLightApply(1);
}

void LevelEditorScreenController::onLight2Apply()
{
	onLightApply(2);
}

void LevelEditorScreenController::onLight3Apply()
{
	onLightApply(3);
}

void LevelEditorScreenController::onLightApply(int32_t lightIdx)
{
	try {
		auto enabled = lightsEnabled[lightIdx]->getController()->getValue().equals(CHECKBOX_CHECKED);
		view->applyLight(
			lightIdx,
			Tools::convertToColor4(lightsAmbient[lightIdx]->getController()->getValue().getString()),
			Tools::convertToColor4(lightsDiffuse[lightIdx]->getController()->getValue().getString()),
			Tools::convertToColor4(lightsSpecular[lightIdx]->getController()->getValue().getString()),
			Tools::convertToVector4(lightsPosition[lightIdx]->getController()->getValue().getString()),
			Tools::convertToFloat(lightsConstAttenuation[lightIdx]->getController()->getValue().getString()),
			Tools::convertToFloat(lightsLinAttenuation[lightIdx]->getController()->getValue().getString()),
			Tools::convertToFloat(lightsQuadAttenuation[lightIdx]->getController()->getValue().getString()),
			Tools::convertToVector3(lightsSpotTo[lightIdx]->getController()->getValue().getString()),
			Tools::convertToVector3(lightsSpotDirection[lightIdx]->getController()->getValue().getString()),
			Tools::convertToFloat(lightsSpotExponent[lightIdx]->getController()->getValue().getString()),
			Tools::convertToFloat(lightsSpotCutoff[lightIdx]->getController()->getValue().getString()),
			enabled
		);
		lightsAmbient[lightIdx]->getController()->setDisabled(enabled == false);
		lightsDiffuse[lightIdx]->getController()->setDisabled(enabled == false);
		lightsSpecular[lightIdx]->getController()->setDisabled(enabled == false);
		lightsPosition[lightIdx]->getController()->setDisabled(enabled == false);
		lightsConstAttenuation[lightIdx]->getController()->setDisabled(enabled == false);
		lightsLinAttenuation[lightIdx]->getController()->setDisabled(enabled == false);
		lightsQuadAttenuation[lightIdx]->getController()->setDisabled(enabled == false);
		lightsSpotTo[lightIdx]->getController()->setDisabled(enabled == false);
		lightsSpotDirection[lightIdx]->getController()->setDisabled(enabled == false);
		lightsSpotExponent[lightIdx]->getController()->setDisabled(enabled == false);
		lightsSpotCutoff[lightIdx]->getController()->setDisabled(enabled == false);
		ligthsSpotDirectionCompute[lightIdx]->getController()->setDisabled(enabled == false);
	} catch (Exception& exception) {
		showErrorPopUp("Warning", (exception.what()));
	}
}

void LevelEditorScreenController::onLight0PresetApply()
{
	onLightPresetApply(0);
}

void LevelEditorScreenController::onLight1PresetApply()
{
	onLightPresetApply(1);
}

void LevelEditorScreenController::onLight2PresetApply()
{
	onLightPresetApply(2);
}

void LevelEditorScreenController::onLight3PresetApply()
{
	onLightPresetApply(3);
}

void LevelEditorScreenController::onLightPresetApply(int32_t lightIdx)
{
	auto lightPresets = LevelPropertyPresets::getInstance()->getLightPresets();
	LevelEditorLight* lightPreset = nullptr;
	auto lightPresetIt = lightPresets.find(lightsPresets[lightIdx]->getController()->getValue().getString());
	if (lightPresetIt != lightPresets.end()) lightPreset = lightPresetIt->second;
	if (lightPreset == nullptr) return;

	view->applyLight(lightIdx, lightPreset->getAmbient(), lightPreset->getDiffuse(), lightPreset->getSpecular(), lightPreset->getPosition(), lightPreset->getConstantAttenuation(), lightPreset->getLinearAttenuation(), lightPreset->getQuadraticAttenuation(), lightPreset->getSpotTo(), lightPreset->getSpotDirection(), lightPreset->getSpotExponent(), lightPreset->getSpotCutOff(), lightPreset->isEnabled());
}

void LevelEditorScreenController::onLight0SpotDirectionCompute()
{
	onLightSpotDirectionCompute(0);
}

void LevelEditorScreenController::onLight1SpotDirectionCompute()
{
	onLightSpotDirectionCompute(1);
}

void LevelEditorScreenController::onLight2SpotDirectionCompute()
{
	onLightSpotDirectionCompute(2);
}

void LevelEditorScreenController::onLight3SpotDirectionCompute()
{
	onLightSpotDirectionCompute(3);
}

void LevelEditorScreenController::onLightSpotDirectionCompute(int32_t lightIdx)
{
	try {
		view->computeSpotDirection(
			lightIdx,
			Tools::convertToVector4(lightsPosition[lightIdx]->getController()->getValue().getString()),
			Tools::convertToVector3(lightsSpotTo[lightIdx]->getController()->getValue().getString())
		);
	} catch (Exception& exception) {
		showErrorPopUp("Warning", (exception.what()));
	}
}

void LevelEditorScreenController::saveFile(const string& pathName, const string& fileName) /* throws(Exception) */
{
	view->saveMap(pathName, fileName);
}

void LevelEditorScreenController::loadFile(const string& pathName, const string& fileName) /* throws(Exception) */
{
	view->loadMap(pathName, fileName);
}

void LevelEditorScreenController::onValueChanged(GUIElementNode* node)
{
	if (node->getId().compare("objects_listbox") == 0) {
	} else
	if (node->getId().compare("map_properties_listbox") == 0) {
		onMapPropertiesSelectionChanged();
	} else
	if (node->getId().compare("object_properties_listbox") == 0) {
		onObjectPropertiesSelectionChanged();
	} else {
		Console::println(string("LevelEditorScreenController::onValueChanged: " + node->getId()));
	}
}

void LevelEditorScreenController::onActionPerformed(GUIActionListenerType type, GUIElementNode* node)
{
	if (type == GUIActionListenerType::PERFORMED) {
		if (node->getId().compare("button_objects_select") == 0) {
			onObjectsSelect();
		} else
		if (node->getId().compare("button_objects_unselect") == 0) {
			onObjectsUnselect();
		} else
		if (node->getId().compare("button_grid_apply") == 0) {
			onGridApply();
		} else
		if (node->getId().compare("button_snapping_apply") == 0) {
			onSnappingApply();
		} else
		if (node->getId().compare("button_map_load") == 0) {
			onMapLoad();
		} else
		if (node->getId().compare("button_map_save") == 0) {
			onMapSave();
		} else
		if (node->getId().compare("button_map_properties_add") == 0) {
			onMapPropertyAdd();
		} else
		if (node->getId().compare("button_map_properties_remove") == 0) {
			onMapPropertyRemove();
		} else
		if (node->getId().compare("button_map_properties_save") == 0) {
			onMapPropertySave();
		} else
		if (node->getId().compare("button_objectdata_apply") == 0) {
			onObjectDataApply();
		} else
		if (node->getId().compare("button_translation_apply") == 0) {
			onObjectTranslationApply();
		} else
		if (node->getId().compare("button_scale_apply") == 0) {
			onObjectScaleApply();
		} else
		if (node->getId().compare("button_rotation_apply") == 0) {
			onObjectRotationsApply();
		} else
		if (node->getId().compare("button_object_color") == 0) {
			onObjectColor();
		} else
		if (node->getId().compare("button_object_center") == 0) {
			onObjectCenter();
		} else
		if (node->getId().compare("button_object_remove") == 0) {
			onObjectRemove();
		} else
		if (node->getId().compare("button_object_properties_presetapply") == 0) {
			onObjectPropertyPresetApply();
		} else
		if (node->getId().compare("button_object_properties_add") == 0) {
			onObjectPropertyAdd();
		} else
		if (node->getId().compare("button_object_properties_remove") == 0) {
			onObjectPropertyRemove();
		} else
		if (node->getId().compare("button_object_properties_save") == 0) {
			onObjectPropertySave();
		} else
		if (node->getId().compare("button_light0_presetapply") == 0) {
			onLight0PresetApply();
		} else
		if (node->getId().compare("button_light0_spotdirection_compute") == 0) {
			onLight0SpotDirectionCompute();
		} else
		if (node->getId().compare("button_light0_apply") == 0) {
			onLight0Apply();
		} else
		if (node->getId().compare("button_light1_presetapply") == 0) {
			onLight1PresetApply();
		} else
		if (node->getId().compare("button_light1_spotdirection_compute") == 0) {
			onLight1SpotDirectionCompute();
		} else
		if (node->getId().compare("button_light1_apply") == 0) {
			onLight1Apply();
		} else
		if (node->getId().compare("button_light2_presetapply") == 0) {
			onLight2PresetApply();
		} else
		if (node->getId().compare("button_light2_spotdirection_compute") == 0) {
			onLight2SpotDirectionCompute();
		} else
		if (node->getId().compare("button_light2_apply") == 0) {
			onLight2Apply();
		} else
		if (node->getId().compare("button_light3_presetapply") == 0) {
			onLight3PresetApply();
		} else
		if (node->getId().compare("button_light3_spotdirection_compute") == 0) {
			onLight3SpotDirectionCompute();
		} else
		if (node->getId().compare("button_light3_apply") == 0) {
			onLight3Apply();
		}
	}
}

void LevelEditorScreenController::showErrorPopUp(const string& caption, const string& message)
{
	Console::println(string(caption + ":" + message));
	view->getPopUps()->getInfoDialogScreenController()->show(caption, message);
}

void LevelEditorScreenController::getViewPort(int& left, int& top, int& width, int& height) {
	auto& constraints = viewPort->getComputedConstraints();
	left = constraints.left + constraints.alignmentLeft + constraints.contentAlignmentLeft;
	top = constraints.top + constraints.alignmentTop + constraints.contentAlignmentTop;
	width = constraints.width;
	height = constraints.height;
}
