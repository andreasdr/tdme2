#include <tdme/tools/sceneeditor/controller/SceneEditorScreenController.h>

#include <array>
#include <string>
#include <vector>

#include <tdme/engine/fileio/models/ModelReader.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/prototype/Prototype.h>
#include <tdme/engine/prototype/PrototypeProperty.h>
#include <tdme/engine/scene/Scene.h>
#include <tdme/engine/scene/SceneEntity.h>
#include <tdme/engine/scene/SceneLight.h>
#include <tdme/engine/scene/ScenePropertyPresets.h>
#include <tdme/gui/events/Action.h>
#include <tdme/gui/events/GUIActionListener.h>
#include <tdme/gui/events/GUIChangeListener.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUINodeController.h>
#include <tdme/gui/nodes/GUIParentNode.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/gui/nodes/GUITextNode.h>
#include <tdme/gui/GUIParser.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Vector4.h>
#include <tdme/tools/sceneeditor/views/SceneEditorView.h>
#include <tdme/tools/sceneeditor/TDMESceneEditor.h>
#include <tdme/tools/shared/controller/FileDialogPath.h>
#include <tdme/tools/shared/controller/FileDialogScreenController.h>
#include <tdme/tools/shared/controller/InfoDialogScreenController.h>
#include <tdme/tools/shared/tools/Tools.h>
#include <tdme/tools/shared/views/PopUps.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>
#include <tdme/utilities/ExceptionBase.h>
#include <tdme/utilities/Float.h>
#include <tdme/utilities/MutableString.h>
#include <tdme/utilities/StringTokenizer.h>
#include <tdme/utilities/StringTools.h>

using std::array;
using std::string;
using std::to_string;
using std::vector;

using tdme::tools::sceneeditor::controller::SceneEditorScreenController;

using tdme::engine::fileio::models::ModelReader;
using tdme::engine::model::Color4;
using tdme::engine::prototype::Prototype;
using tdme::engine::prototype::PrototypeProperty;
using tdme::engine::scene::Scene;
using tdme::engine::scene::SceneEntity;
using tdme::engine::scene::SceneLight;
using tdme::engine::scene::ScenePropertyPresets;
using tdme::gui::events::GUIActionListenerType;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeController;
using tdme::gui::nodes::GUIParentNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::nodes::GUITextNode;
using tdme::gui::GUIParser;
using tdme::math::Vector3;
using tdme::math::Vector4;
using tdme::tools::sceneeditor::views::SceneEditorView;
using tdme::tools::sceneeditor::TDMESceneEditor;
using tdme::tools::shared::controller::FileDialogPath;
using tdme::tools::shared::controller::FileDialogScreenController;
using tdme::tools::shared::controller::InfoDialogScreenController;
using tdme::tools::shared::tools::Tools;
using tdme::tools::shared::views::PopUps;
using tdme::utilities::Console;
using tdme::utilities::Exception;
using tdme::utilities::ExceptionBase;
using tdme::utilities::Float;
using tdme::utilities::MutableString;
using tdme::utilities::StringTokenizer;
using tdme::utilities::StringTools;

MutableString SceneEditorScreenController::CHECKBOX_CHECKED = MutableString("1");
MutableString SceneEditorScreenController::CHECKBOX_UNCHECKED = MutableString("");
MutableString SceneEditorScreenController::TEXT_EMPTY = MutableString("");

SceneEditorScreenController::SceneEditorScreenController(SceneEditorView* view)
{
	this->view = view;
	this->scenePath = new FileDialogPath(".");
	this->modelPath = new FileDialogPath(".");
}

GUIScreenNode* SceneEditorScreenController::getScreenNode()
{
	return screenNode;
}

FileDialogPath* SceneEditorScreenController::getMapPath()
{
	return scenePath;
}

void SceneEditorScreenController::initialize()
{
	try {
		screenNode = GUIParser::parse("resources/engine/gui", "screen_sceneeditor.xml");
		screenNode->addActionListener(this);
		screenNode->addChangeListener(this);
		screenCaption = dynamic_cast< GUITextNode* >(screenNode->getNodeById("screen_caption"));
		gridEnabled = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("grid_enabled"));
		gridYPosition = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("grid_y_position"));
		snappingX = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("snapping_x"));
		snappingZ = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("snapping_z"));
		snappingEnabled = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("snapping_enabled"));
		sceneWidth = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("map_width"));
		sceneDepth = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("map_depth"));
		sceneHeight = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("map_height"));
		scenePropertyName = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("map_property_name"));
		scenePropertyValue = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("map_property_value"));
		scenePropertySave = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_map_properties_save"));
		scenePropertyRemove = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_map_properties_remove"));
		scenePropertiesListBox = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("map_properties_listbox"));
		sceneSkyModel = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("map_sky_model"));
		btnSceneSkyModelLoad = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_map_sky_model_load"));
		btnSceneSkyModelClear = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_map_sky_model_clear"));
		sceneSkyModelScale = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("map_sky_model_scale"));
		btnSceneSkyApply = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_map_sky_apply"));
		entityName = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("object_name"));
		entityDescription = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("object_description"));
		entityModel = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("object_model"));
		entityCenter = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("object_center"));
		btnEntityDataApply = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_objectdata_apply"));
		btnEntityTranslationApply = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_translation_apply"));
		btnEntityScaleApply = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_scale_apply"));
		btnEntityRotationApply = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_rotation_apply"));
		btnEntityColor = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_object_color"));
		btnEntityCenter = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_object_center"));
		btnEntityRemove = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_object_remove"));
		entityTranslationX = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("object_translation_x"));
		entityTranslationY = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("object_translation_y"));
		entityTranslationZ = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("object_translation_z"));
		entityScaleX = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("object_scale_x"));
		entityScaleY = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("object_scale_y"));
		entityScaleZ = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("object_scale_z"));
		entityRotationX = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("object_rotation_x"));
		entityRotationY = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("object_rotation_y"));
		entityRotationZ = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("object_rotation_z"));
		entityPropertyName = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("object_property_name"));
		entityPropertyValue = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("object_property_value"));
		btnEntityPropertySave = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_object_properties_save"));
		btnEntityPropertyRemove = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_object_properties_remove"));
		btnEntityPropertyAdd = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_object_properties_add"));
		btnEntityPropertyRemove = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_object_properties_remove"));
		btnEntityPropertyPresetApply = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_object_properties_presetapply"));
		entityPropertiesListBox = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("object_properties_listbox"));
		entityPropertiesPresets = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("object_properties_presets"));
		entitiesListBox = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("objects_listbox"));
		entityReflectionsEnvironmentmappingDropDown = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("object_reflections_environmentmapping"));
		btnEntityReflectionsEnvironmentmappingApply = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_object_reflections_environmentmapping_apply"));
		sceneWidth->getController()->setDisabled(true);
		sceneDepth->getController()->setDisabled(true);
		sceneHeight->getController()->setDisabled(true);
		entityModel->getController()->setDisabled(true);
		entityCenter->getController()->setDisabled(true);
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
		Console::print(string("SceneEditorScreenController::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
}

void SceneEditorScreenController::dispose()
{
}

void SceneEditorScreenController::setScreenCaption(const string& text)
{
	screenCaption->setText(text);
}

void SceneEditorScreenController::setGrid(bool enabled, float gridY)
{
	gridEnabled->getController()->setValue(enabled == true ? CHECKBOX_CHECKED : CHECKBOX_UNCHECKED);
	gridYPosition->getController()->setValue(MutableString(Tools::formatFloat(gridY)));
}

void SceneEditorScreenController::setSnapping(bool snappingEnabled, float snappingX, float snappingZ) {
	this->snappingEnabled->getController()->setValue(snappingEnabled == true ? CHECKBOX_CHECKED : CHECKBOX_UNCHECKED);
	this->snappingX->getController()->setValue(MutableString(Tools::formatFloat(snappingX)));
	this->snappingZ->getController()->setValue(MutableString(Tools::formatFloat(snappingZ)));
}

void SceneEditorScreenController::setSceneSize(float width, float depth, float height)
{
	sceneWidth->getController()->setValue(MutableString(Tools::formatFloat(width)));
	sceneDepth->getController()->setValue(MutableString(Tools::formatFloat(depth)));
	sceneHeight->getController()->setValue(MutableString(Tools::formatFloat(height)));
}

void SceneEditorScreenController::unsetEntityProperties()
{
	entityPropertiesPresets->getController()->setValue(MutableString("none"));
	entityPropertiesPresets->getController()->setDisabled(true);
	btnEntityPropertyPresetApply->getController()->setDisabled(true);
	entityPropertiesListBox->getController()->setDisabled(true);
	btnEntityPropertyAdd->getController()->setDisabled(true);
	btnEntityPropertyRemove->getController()->setDisabled(true);
	btnEntityPropertySave->getController()->setDisabled(true);
	entityPropertyName->getController()->setValue(TEXT_EMPTY);
	entityPropertyName->getController()->setDisabled(true);
	entityPropertyValue->getController()->setValue(TEXT_EMPTY);
	entityPropertyValue->getController()->setDisabled(true);
	auto entityPropertiesListBoxInnerNode = dynamic_cast< GUIParentNode* >((entityPropertiesListBox->getScreenNode()->getNodeById(entityPropertiesListBox->getId() + "_inner")));
	entityPropertiesListBoxInnerNode->clearSubNodes();
}

const string SceneEditorScreenController::getEntityPropertyPresetSelection()
{
	return "";
}

void SceneEditorScreenController::setEntityData(const string& name, const string& description, const string& modelName, const Vector3& center)
{
	entityName->getController()->setDisabled(false);
	entityName->getController()->setValue(MutableString(name));
	entityDescription->getController()->setDisabled(false);
	entityDescription->getController()->setValue(MutableString(description));
	entityModel->getController()->setValue(MutableString(modelName));
	entityCenter->getController()->setValue(MutableString().append(Tools::formatFloat(center.getX())).append(", ").append(Tools::formatFloat(center.getY())).append(", ").append(Tools::formatFloat(center.getZ())));
	btnEntityDataApply->getController()->setDisabled(false);
}

void SceneEditorScreenController::unsetEntityData()
{
	entityName->getController()->setValue(TEXT_EMPTY);
	entityName->getController()->setDisabled(true);
	entityDescription->getController()->setValue(TEXT_EMPTY);
	entityDescription->getController()->setDisabled(true);
	entityModel->getController()->setValue(TEXT_EMPTY);
	entityModel->getController()->setDisabled(true);
	entityCenter->getController()->setValue(TEXT_EMPTY);
	entityCenter->getController()->setDisabled(true);
	btnEntityDataApply->getController()->setDisabled(true);
}

void SceneEditorScreenController::onEntityDataApply()
{
	if (view->entityDataApply(entityName->getController()->getValue().getString(), entityDescription->getController()->getValue().getString()) == false) {
		showErrorPopUp("Warning", "Changing object data failed");
	}
}

void SceneEditorScreenController::setEntityListbox(Scene& scene)
{
	auto selectedObjects = entitiesListBox->getController()->getValue();
	auto entitiesListBoxInnerNode = dynamic_cast< GUIParentNode* >((entitiesListBox->getScreenNode()->getNodeById(entitiesListBox->getId() + "_inner")));
	auto idx = 1;
	string entitiesListBoxSubNodesXML = "";
	entitiesListBoxSubNodesXML =
		entitiesListBoxSubNodesXML +
		"<scrollarea id=\"" +
		entitiesListBox->getId() +
		"_inner_scrollarea\" width=\"100%\" height=\"100%\">\n";
	auto entityIdx = 0;
	for (int i = 0; i < scene.getEntityCount(); i++) {
		if (entityIdx > 25000) {
			entitiesListBoxSubNodesXML =
				"<scrollarea id=\"" +
				entitiesListBox->getId() +
				"_inner_scrollarea\" width=\"100%\" height=\"100%\">\n";
			break;
		}
		auto entity = scene.getEntityAt(i);
		if (entity->getPrototype()->isRenderGroups() == true) continue;
		auto entityId = entity->getId();
		entitiesListBoxSubNodesXML =
			entitiesListBoxSubNodesXML +
			"<selectbox-option text=\"" +
			GUIParser::escapeQuotes(entityId) +
			"\" value=\"" +
			GUIParser::escapeQuotes(entityId) +
			"\" " +
			"/>\n";
		entityIdx++;
	}
	entitiesListBoxSubNodesXML =
		entitiesListBoxSubNodesXML +
		"</scrollarea>\n";
	try {
		entitiesListBoxInnerNode->replaceSubNodes(entitiesListBoxSubNodesXML, false);
	} catch (Exception& exception) {
		Console::print(string("SceneEditorScreenController::setEntityListbox(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	entitiesListBox->getController()->setValue(selectedObjects);
}

void SceneEditorScreenController::unselectEntitiesInEntityListBox(const string& entityId)
{
	auto selectedEntities = entitiesListBox->getController()->getValue();
	auto value = MutableString().set('|').append(entityId).append('|');
	auto pos = selectedEntities.indexOf(value);
	if (pos != -1) selectedEntities.delete_(pos, value.length());
	entitiesListBox->getController()->setValue(selectedEntities);
}

void SceneEditorScreenController::unselectEntitiesInEntityListBox()
{
	entitiesListBox->getController()->setValue(MutableString());
}

void SceneEditorScreenController::selectEntityInEntityListbox(const string& entityId)
{
	auto selectedEntities = entitiesListBox->getController()->getValue();
	auto value = MutableString().set('|').append(entityId).append('|');
	auto pos = selectedEntities.indexOf(value);
	if (pos == -1) selectedEntities.append(value);
	entitiesListBox->getController()->setValue(selectedEntities);
}

void SceneEditorScreenController::onEntitiesSelect()
{
	vector<string> selectedEntityList;
	StringTokenizer t;
	t.tokenize(entitiesListBox->getController()->getValue().getString(), "|");
	while (t.hasMoreTokens()) {
		selectedEntityList.push_back(t.nextToken());
	}
	if (selectedEntityList.empty() == false)
		view->selectEntities(selectedEntityList);

}

void SceneEditorScreenController::onEntitiesUnselect()
{
	view->unselectEntities();
}

void SceneEditorScreenController::setEntityTransformations(const Vector3& translation, const Vector3& scale, float rotationX, float rotationY, float rotationZ, bool disableRotation)
{
	btnEntityTranslationApply->getController()->setDisabled(false);
	btnEntityScaleApply->getController()->setDisabled(false);
	btnEntityRotationApply->getController()->setDisabled(false);
	btnEntityColor->getController()->setDisabled(false);
	btnEntityCenter->getController()->setDisabled(false);
	btnEntityRemove->getController()->setDisabled(false);
	entityTranslationX->getController()->setDisabled(false);
	entityTranslationY->getController()->setDisabled(false);
	entityTranslationZ->getController()->setDisabled(false);
	entityScaleX->getController()->setDisabled(false);
	entityScaleY->getController()->setDisabled(false);
	entityScaleZ->getController()->setDisabled(false);
	entityRotationX->getController()->setDisabled(disableRotation);
	entityRotationY->getController()->setDisabled(disableRotation);
	entityRotationZ->getController()->setDisabled(disableRotation);
	entityTranslationX->getController()->setValue(MutableString(Tools::formatFloat(translation.getX())));
	entityTranslationY->getController()->setValue(MutableString(Tools::formatFloat(translation.getY())));
	entityTranslationZ->getController()->setValue(MutableString(Tools::formatFloat(translation.getZ())));
	entityScaleX->getController()->setValue(MutableString(Tools::formatFloat(scale.getX())));
	entityScaleY->getController()->setValue(MutableString(Tools::formatFloat(scale.getY())));
	entityScaleZ->getController()->setValue(MutableString(Tools::formatFloat(scale.getZ())));
	entityRotationX->getController()->setValue(MutableString(Tools::formatFloat(rotationX)));
	entityRotationY->getController()->setValue(MutableString(Tools::formatFloat(rotationY)));
	entityRotationZ->getController()->setValue(MutableString(Tools::formatFloat(rotationZ)));
}

void SceneEditorScreenController::unsetEntityTransformations()
{
	btnEntityTranslationApply->getController()->setDisabled(true);
	btnEntityScaleApply->getController()->setDisabled(true);
	btnEntityRotationApply->getController()->setDisabled(true);
	btnEntityColor->getController()->setDisabled(true);
	btnEntityCenter->getController()->setDisabled(true);
	btnEntityRemove->getController()->setDisabled(true);
	entityTranslationX->getController()->setDisabled(true);
	entityTranslationY->getController()->setDisabled(true);
	entityTranslationZ->getController()->setDisabled(true);
	entityScaleX->getController()->setDisabled(true);
	entityScaleY->getController()->setDisabled(true);
	entityScaleZ->getController()->setDisabled(true);
	entityRotationX->getController()->setDisabled(true);
	entityRotationY->getController()->setDisabled(true);
	entityRotationZ->getController()->setDisabled(true);
	entityTranslationX->getController()->setValue(TEXT_EMPTY);
	entityTranslationY->getController()->setValue(TEXT_EMPTY);
	entityTranslationZ->getController()->setValue(TEXT_EMPTY);
	entityScaleX->getController()->setValue(TEXT_EMPTY);
	entityScaleY->getController()->setValue(TEXT_EMPTY);
	entityScaleZ->getController()->setValue(TEXT_EMPTY);
	entityRotationX->getController()->setValue(TEXT_EMPTY);
	entityRotationY->getController()->setValue(TEXT_EMPTY);
	entityRotationZ->getController()->setValue(TEXT_EMPTY);
}

void SceneEditorScreenController::onScenePropertiesSelectionChanged()
{
	scenePropertyName->getController()->setDisabled(true);
	scenePropertyName->getController()->setValue(TEXT_EMPTY);
	scenePropertyValue->getController()->setDisabled(true);
	scenePropertyValue->getController()->setValue(TEXT_EMPTY);
	scenePropertySave->getController()->setDisabled(true);
	scenePropertyRemove->getController()->setDisabled(true);
	auto mapProperty = view->getScene()->getProperty(scenePropertiesListBox->getController()->getValue().getString());
	if (mapProperty != nullptr) {
		scenePropertyName->getController()->setValue(MutableString(mapProperty->getName()));
		scenePropertyValue->getController()->setValue(MutableString(mapProperty->getValue()));
		scenePropertyName->getController()->setDisabled(false);
		scenePropertyValue->getController()->setDisabled(false);
		scenePropertySave->getController()->setDisabled(false);
		scenePropertyRemove->getController()->setDisabled(false);
	}
}

void SceneEditorScreenController::setSceneProperties(Scene& scene, const string& selectedName)
{
	scenePropertyName->getController()->setDisabled(true);
	scenePropertyValue->getController()->setDisabled(true);
	scenePropertySave->getController()->setDisabled(true);
	auto mapPropertiesListBoxInnerNode = dynamic_cast< GUIParentNode* >((scenePropertiesListBox->getScreenNode()->getNodeById(scenePropertiesListBox->getId() + "_inner")));
	auto idx = 1;
	string mapPropertiesListBoxSubNodesXML = "";
	mapPropertiesListBoxSubNodesXML =
		mapPropertiesListBoxSubNodesXML +
		"<scrollarea id=\"" +
		scenePropertiesListBox->getId() +
		"_inner_scrollarea\" width=\"100%\" height=\"100%\">\n";
	for (auto i = 0; i < scene.getPropertyCount(); i++) {
		PrototypeProperty* mapProperty = scene.getPropertyByIndex(i);
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
		"</scrollarea>\n";
	try {
		mapPropertiesListBoxInnerNode->replaceSubNodes(mapPropertiesListBoxSubNodesXML, false);
	} catch (Exception& exception) {
		Console::print(string("SceneEditorScreenController::setSceneProperties(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	onScenePropertiesSelectionChanged();
}

void SceneEditorScreenController::onScenePropertySave()
{
	if (view->scenePropertySave(
		scenePropertiesListBox->getController()->getValue().getString(),
		scenePropertyName->getController()->getValue().getString(),
		scenePropertyValue->getController()->getValue().getString()) == false) {
		showErrorPopUp("Warning", "Saving scene property failed");
	}
}

void SceneEditorScreenController::onScenePropertyAdd()
{
	if (view->scenePropertyAdd() == false) {
		showErrorPopUp("Warning", "Adding new scene property failed");
	}
}

void SceneEditorScreenController::onScenePropertyRemove()
{
	if (view->scenePropertyRemove(scenePropertiesListBox->getController()->getValue().getString()) == false) {
		showErrorPopUp("Warning", "Removing scene property failed");
	}
}

void SceneEditorScreenController::setEntityPresetIds(const map<string, vector<PrototypeProperty*>>& entityPresetIds)
{
	auto entitityPropertiesPresetsInnerNode = dynamic_cast< GUIParentNode* >((entityPropertiesPresets->getScreenNode()->getNodeById(entityPropertiesPresets->getId() + "_inner")));
	auto idx = 0;
	string entityPropertiesPresetsInnerNodeSubNodesXML = "";
	entityPropertiesPresetsInnerNodeSubNodesXML =
		entityPropertiesPresetsInnerNodeSubNodesXML +
		"<scrollarea id=\"" +
		entityPropertiesPresets->getId() +
		"_inner_scrollarea\" width=\"100%\" height=\"100\">\n";
	for (auto it: entityPresetIds) {
		auto entityPresetId = it.first;
		entityPropertiesPresetsInnerNodeSubNodesXML =
			entityPropertiesPresetsInnerNodeSubNodesXML +
			"<dropdown-option text=\"" +
			GUIParser::escapeQuotes(entityPresetId) +
			"\" value=\"" +
			GUIParser::escapeQuotes(entityPresetId) +
			"\" " +
			(idx == 0 ? "selected=\"true\" " : "") +
			" />\n";
		idx++;
	}
	entityPropertiesPresetsInnerNodeSubNodesXML =
		entityPropertiesPresetsInnerNodeSubNodesXML +
		"</scrollarea>\n";
	try {
		entitityPropertiesPresetsInnerNode->replaceSubNodes(entityPropertiesPresetsInnerNodeSubNodesXML, true);
	} catch (Exception& exception) {
		Console::print(string("SceneEditorScreenController::setEntityPresetIds(): An error occurred: "));
		Console::println(string(exception.what()));
	}
}

void SceneEditorScreenController::onEntityPropertiesSelectionChanged()
{
	entityPropertyName->getController()->setDisabled(true);
	entityPropertyName->getController()->setValue(TEXT_EMPTY);
	entityPropertyValue->getController()->setDisabled(true);
	entityPropertyValue->getController()->setValue(TEXT_EMPTY);
	btnEntityPropertySave->getController()->setDisabled(true);
	btnEntityPropertyRemove->getController()->setDisabled(true);
	auto sceneEntity = view->getSelectedSceneEntity();
	if (sceneEntity == nullptr)
		return;

	auto entityProperty = sceneEntity->getProperty(entityPropertiesListBox->getController()->getValue().getString());
	if (entityProperty != nullptr) {
		entityPropertyName->getController()->setValue(MutableString(entityProperty->getName()));
		entityPropertyValue->getController()->setValue(MutableString(entityProperty->getValue()));
		entityPropertyName->getController()->setDisabled(false);
		entityPropertyValue->getController()->setDisabled(false);
		btnEntityPropertySave->getController()->setDisabled(false);
		btnEntityPropertyRemove->getController()->setDisabled(false);
	}
}

void SceneEditorScreenController::setEntityProperties(const string& presetId, SceneEntity* entity, const string& selectedName)
{
	entityPropertiesPresets->getController()->setDisabled(false);
	btnEntityPropertyPresetApply->getController()->setDisabled(false);
	entityPropertiesListBox->getController()->setDisabled(false);
	btnEntityPropertyAdd->getController()->setDisabled(false);
	btnEntityPropertyRemove->getController()->setDisabled(false);
	btnEntityPropertySave->getController()->setDisabled(true);
	entityPropertyName->getController()->setDisabled(true);
	entityPropertyValue->getController()->setDisabled(true);
	entityPropertiesPresets->getController()->setValue(presetId.length() > 0 ? MutableString(presetId) : MutableString("none"));
	auto entityPropertiesListBoxInnerNode = dynamic_cast< GUIParentNode* >((entityPropertiesListBox->getScreenNode()->getNodeById(entityPropertiesListBox->getId() + "_inner")));
	auto idx = 1;
	string entityPropertiesListBoxSubNodesXML = "";
	entityPropertiesListBoxSubNodesXML =
		entityPropertiesListBoxSubNodesXML +
		"<scrollarea id=\"" +
		entityPropertiesListBox->getId() +
		"_inner_scrollarea\" width=\"100%\" height=\"100%\">\n";
	for (auto i = 0; i < entity->getPropertyCount(); i++) {
		PrototypeProperty* entityProperty = entity->getPropertyByIndex(i);
		entityPropertiesListBoxSubNodesXML =
			entityPropertiesListBoxSubNodesXML +
			"<selectbox-option text=\"" +
			GUIParser::escapeQuotes(entityProperty->getName()) +
			": " +
			GUIParser::escapeQuotes(entityProperty->getValue()) +
			"\" value=\"" +
			GUIParser::escapeQuotes(entityProperty->getName()) +
			"\" " +
			(selectedName.length() > 0 && entityProperty->getName() == selectedName? "selected=\"true\" " : "") +
			"/>\n";
	}
	entityPropertiesListBoxSubNodesXML =
		entityPropertiesListBoxSubNodesXML +
		"</scrollarea>\n";
	try {
		entityPropertiesListBoxInnerNode->replaceSubNodes(entityPropertiesListBoxSubNodesXML, false);
	} catch (Exception& exception) {
		Console::print(string("SceneEditorScreenController::setEntityProperties(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	onEntityPropertiesSelectionChanged();
}

void SceneEditorScreenController::onEntityPropertySave()
{
	if (view->entityPropertySave(
		entityPropertiesListBox->getController()->getValue().getString(),
		entityPropertyName->getController()->getValue().getString(),
		entityPropertyValue->getController()->getValue().getString()) == false) {
		showErrorPopUp("Warning", "Saving object property failed");
	}
}

void SceneEditorScreenController::onEntityPropertyAdd()
{
	if (view->entityPropertyAdd() == false) {
		showErrorPopUp("Warning", "Adding new entity property failed");
	}
}

void SceneEditorScreenController::onEntityPropertyRemove()
{
	if (view->entityPropertyRemove(entityPropertiesListBox->getController()->getValue().getString()) == false) {
		showErrorPopUp("Warning", "Removing entity property failed");
	}
}

void SceneEditorScreenController::onQuit()
{
	TDMESceneEditor::getInstance()->quit();
}

void SceneEditorScreenController::onEntityTranslationApply()
{
	try {
		auto x = Float::parseFloat(entityTranslationX->getController()->getValue().getString());
		auto y = Float::parseFloat(entityTranslationY->getController()->getValue().getString());
		auto z = Float::parseFloat(entityTranslationZ->getController()->getValue().getString());
		view->entityTranslationApply(x, y, z);
	} catch (Exception& exception) {
		showErrorPopUp("Warning", (exception.what()));
	}
}

void SceneEditorScreenController::onEntityScaleApply()
{
	try {
		auto x = Float::parseFloat(entityScaleX->getController()->getValue().getString());
		auto y = Float::parseFloat(entityScaleY->getController()->getValue().getString());
		auto z = Float::parseFloat(entityScaleZ->getController()->getValue().getString());
		if (x < -500.0f || x > 500.0f)
			throw ExceptionBase("x scale must be within -500 .. +500");

		if (y < -500.0f || y > 500.0f)
			throw ExceptionBase("y scale must be within -500 .. +500");

		if (z < -500.0f || z > 500.0f)
			throw ExceptionBase("z scale must be within -500 .. +500");

		view->entityScaleApply(x, y, z);
	} catch (Exception& exception) {
		showErrorPopUp("Warning", (exception.what()));
	}
}

void SceneEditorScreenController::onEntityRotationsApply()
{
	try {
		auto x = Float::parseFloat(entityRotationX->getController()->getValue().getString());
		auto y = Float::parseFloat(entityRotationY->getController()->getValue().getString());
		auto z = Float::parseFloat(entityRotationZ->getController()->getValue().getString());
		if (x < -360.0f || x > 360.0f)
			throw ExceptionBase("x axis rotation must be within -360 .. +360");

		if (y < -360.0f || y > 360.0f)
			throw ExceptionBase("y axis rotation must be within -360 .. +360");

		if (z < -360.0f || z > 360.0f)
			throw ExceptionBase("z axis rotation must be within -360 .. +360");

		view->entityRotationsApply(x, y, z);
	} catch (Exception& exception) {
		showErrorPopUp("Warning", (exception.what()));
	}
}

void SceneEditorScreenController::onEntityRemove()
{
	view->removeEntities();
}

void SceneEditorScreenController::onEntityColor()
{
	view->colorEntities();
}

void SceneEditorScreenController::onEntityCenter()
{
	view->centerEntity();
}

void SceneEditorScreenController::onSceneLoad()
{
	class OnSceneLoadAction: public virtual Action
	{
	public:
		void performAction() override {
			sceneEditorScreenController->view->loadScene(sceneEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getPathName(), sceneEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getFileName());
			sceneEditorScreenController->scenePath->setPath(sceneEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getPathName());
			sceneEditorScreenController->view->getPopUps()->getFileDialogScreenController()->close();
		}

		/**
		 * Public constructor
		 * @param sceneEditorScreenController scene editor screen controller
		 */
		OnSceneLoadAction(SceneEditorScreenController* sceneEditorScreenController): sceneEditorScreenController(sceneEditorScreenController) {
		}

	private:
		SceneEditorScreenController* sceneEditorScreenController;
	};

	vector<string> extensions = ModelReader::getModelExtensions();
	extensions.push_back("tl");
	view->getPopUps()->getFileDialogScreenController()->show(
		scenePath->getPath(),
		"Load from: ",
		extensions,
		view->getFileName(),
		true,
		new OnSceneLoadAction(this)
	);
}

void SceneEditorScreenController::onSceneSave()
{
	class OnSceneSaveAction: public virtual Action
	{
	public:
		void performAction() override {
			sceneEditorScreenController->view->saveScene(sceneEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getPathName(), sceneEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getFileName());
			sceneEditorScreenController->scenePath->setPath(sceneEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getPathName());
			sceneEditorScreenController->view->getPopUps()->getFileDialogScreenController()->close();
		}

		/**
		 * Public constructor
		 * @param sceneEditorScreenController scene editor screen controller
		 */
		OnSceneSaveAction(SceneEditorScreenController* sceneEditorScreenController): sceneEditorScreenController(sceneEditorScreenController) {
		}

	private:
		SceneEditorScreenController* sceneEditorScreenController;
	};

	vector<string> extensions = {
		"tl",
	};
	view->getPopUps()->getFileDialogScreenController()->show(
		scenePath->getPath(),
		"Save to: ",
		extensions,
		view->getFileName(),
		false,
		new OnSceneSaveAction(this)
	);
}

void SceneEditorScreenController::onGridApply()
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

void SceneEditorScreenController::onSnappingApply()
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

void SceneEditorScreenController::onEntityPropertyPresetApply()
{
	view->entityPropertiesPreset(entityPropertiesPresets->getController()->getValue().getString());
}

void SceneEditorScreenController::setLightPresetsIds(const map<string, SceneLight*>& lightPresetIds)
{
	for (auto i = 0; i < 4; i++) {
		auto lightPresetsInnerNode = dynamic_cast< GUIParentNode* >((lightsPresets[i]->getScreenNode()->getNodeById(lightsPresets[i]->getId() + "_inner")));
		auto idx = 0;
		string lightPresetsInnerNodeSubNodesXML = "";
		lightPresetsInnerNodeSubNodesXML =
			lightPresetsInnerNodeSubNodesXML +
			"<scrollarea id=\"" +
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
			"</scrollarea>\n";
		try {
			lightPresetsInnerNode->replaceSubNodes(lightPresetsInnerNodeSubNodesXML, true);
		} catch (Exception& exception) {
			Console::print(string("SceneEditorScreenController::setLightPresetsIds(): An error occurred: "));
			Console::println(string(exception.what()));
		}
	}
}

void SceneEditorScreenController::unselectLightPresets()
{
}

void SceneEditorScreenController::setLight(int i, const Color4& ambient, const Color4& diffuse, const Color4& specular, const Vector4& position, float constAttenuation, float linearAttenuation, float quadraticAttenuation, const Vector3& spotTo, const Vector3& spotDirection, float spotExponent, float spotCutoff, bool enabled)
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

void SceneEditorScreenController::onLight0Apply()
{
	onLightApply(0);
}

void SceneEditorScreenController::onLight1Apply()
{
	onLightApply(1);
}

void SceneEditorScreenController::onLight2Apply()
{
	onLightApply(2);
}

void SceneEditorScreenController::onLight3Apply()
{
	onLightApply(3);
}

void SceneEditorScreenController::onLightApply(int lightIdx)
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

void SceneEditorScreenController::onLight0PresetApply()
{
	onLightPresetApply(0);
}

void SceneEditorScreenController::onLight1PresetApply()
{
	onLightPresetApply(1);
}

void SceneEditorScreenController::onLight2PresetApply()
{
	onLightPresetApply(2);
}

void SceneEditorScreenController::onLight3PresetApply()
{
	onLightPresetApply(3);
}

void SceneEditorScreenController::onLightPresetApply(int lightIdx)
{
	auto lightPresets = ScenePropertyPresets::getInstance()->getLightPresets();
	SceneLight* lightPreset = nullptr;
	auto lightPresetIt = lightPresets.find(lightsPresets[lightIdx]->getController()->getValue().getString());
	if (lightPresetIt != lightPresets.end()) lightPreset = lightPresetIt->second;
	if (lightPreset == nullptr) return;

	view->applyLight(lightIdx, lightPreset->getAmbient(), lightPreset->getDiffuse(), lightPreset->getSpecular(), lightPreset->getPosition(), lightPreset->getConstantAttenuation(), lightPreset->getLinearAttenuation(), lightPreset->getQuadraticAttenuation(), lightPreset->getSpotTo(), lightPreset->getSpotDirection(), lightPreset->getSpotExponent(), lightPreset->getSpotCutOff(), lightPreset->isEnabled());
}

void SceneEditorScreenController::onLight0SpotDirectionCompute()
{
	onLightSpotDirectionCompute(0);
}

void SceneEditorScreenController::onLight1SpotDirectionCompute()
{
	onLightSpotDirectionCompute(1);
}

void SceneEditorScreenController::onLight2SpotDirectionCompute()
{
	onLightSpotDirectionCompute(2);
}

void SceneEditorScreenController::onLight3SpotDirectionCompute()
{
	onLightSpotDirectionCompute(3);
}

void SceneEditorScreenController::onLightSpotDirectionCompute(int lightIdx)
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

void SceneEditorScreenController::saveScene(const string& pathName, const string& fileName)
{
	view->saveScene(pathName, fileName);
}

void SceneEditorScreenController::loadScene(const string& pathName, const string& fileName)
{
	view->loadScene(pathName, fileName);
}

void SceneEditorScreenController::setSky(Scene& scene) {
	sceneSkyModel->getController()->setValue(MutableString(scene.getSkyModelFileName()));
	sceneSkyModelScale->getController()->setValue(MutableString(Tools::formatVector3(scene.getSkyModelScale())));
}

void SceneEditorScreenController::onMapSkyModelLoad() {
	class OnMapSkyModelLoad: public virtual Action
	{

	public:
		void performAction() override {
			sceneEditorScreenController->sceneSkyModel->getController()->setValue(
				sceneEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getPathName() +
				"/" +
				sceneEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getFileName()
			);
			sceneEditorScreenController->modelPath->setPath(
				sceneEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getPathName()
			);
			sceneEditorScreenController->view->getPopUps()->getFileDialogScreenController()->close();
		}

		/**
		 * Public constructor
		 * @param modelEditorScreenController model editor screen controller
		 */
		OnMapSkyModelLoad(SceneEditorScreenController* sceneEditorScreenController): sceneEditorScreenController(sceneEditorScreenController) {
		}

	private:
		SceneEditorScreenController* sceneEditorScreenController;
	};

	vector<string> extensions = ModelReader::getModelExtensions();
	view->getPopUps()->getFileDialogScreenController()->show(
		sceneSkyModel->getController()->getValue().getString().empty() == true?modelPath->getPath():Tools::getPathName(sceneSkyModel->getController()->getValue().getString()),
		"Load from: ",
		extensions,
		Tools::getFileName(sceneSkyModel->getController()->getValue().getString()),
		true,
		new OnMapSkyModelLoad(this)
	);
}

void SceneEditorScreenController::onMapSkyModelClear() {
	sceneSkyModel->getController()->setValue(MutableString());
}

void SceneEditorScreenController::onMapSkyApply() {
	try {
		auto skyModelScale = Tools::convertToVector3(sceneSkyModelScale->getController()->getValue().getString());
		if (skyModelScale.getX() < 0.01f || skyModelScale.getX() > 150.0f)
			throw ExceptionBase("x scale must be within 0.01 .. 150.0");

		if (skyModelScale.getY() < 0.01f || skyModelScale.getY() > 150.0f)
			throw ExceptionBase("y scale must be within 0.01 .. 150.0");

		if (skyModelScale.getZ() < 0.01f || skyModelScale.getZ() > 150.0f)
			throw ExceptionBase("z scale must be within 0.01 .. 150.0");

		view->getScene()->setSkyModelScale(skyModelScale);
		view->getScene()->setSkyModelFileName(sceneSkyModel->getController()->getValue().getString());
		auto model =
			sceneSkyModel->getController()->getValue().getString().empty() == true?
				nullptr:
				ModelReader::read(
					Tools::getPathName(sceneSkyModel->getController()->getValue().getString()),
					Tools::getFileName(sceneSkyModel->getController()->getValue().getString())
				);
		view->getScene()->setSkyModel(model);
		setSky(*view->getScene());
	} catch (Exception& exception) {
		showErrorPopUp("Warning", (exception.what()));
	}
	view->updateSky();
}

void SceneEditorScreenController::setEntityReflectionsEnvironmentMappings(Scene& scene, const string& selectedEnvironmentMappingId) {
	entityReflectionsEnvironmentmappingDropDown->getController()->setDisabled(false);
	entityReflectionsEnvironmentmappingDropDown->getController()->setValue(MutableString());
	btnEntityReflectionsEnvironmentmappingApply->getController()->setDisabled(false);
	auto environmentMappingIdsDropDownInnerNode = dynamic_cast< GUIParentNode* >((entityReflectionsEnvironmentmappingDropDown->getScreenNode()->getNodeById(entityReflectionsEnvironmentmappingDropDown->getId() + "_inner")));
	string environmentMappingIdsInnerNodeSubNodesXML = "";
	environmentMappingIdsInnerNodeSubNodesXML =
		environmentMappingIdsInnerNodeSubNodesXML +
		"<scrollarea id=\"" +
		entityReflectionsEnvironmentmappingDropDown->getId() +
		"_inner_scrollarea\" width=\"100%\" height=\"50\">\n";
	environmentMappingIdsInnerNodeSubNodesXML =
		environmentMappingIdsInnerNodeSubNodesXML +
		"<dropdown-option text=\"" +
		GUIParser::escapeQuotes("<None>") +
		"\" value=\"\" " +
		(selectedEnvironmentMappingId.empty() == true?"selected=\"true\" ":"") +
		" />\n";
	for (auto& environmentMappingId: scene.getEnvironmentMappingIds()) {
		environmentMappingIdsInnerNodeSubNodesXML =
			environmentMappingIdsInnerNodeSubNodesXML +
			"<dropdown-option text=\"" +
			GUIParser::escapeQuotes(environmentMappingId) +
			"\" value=\"" +
			GUIParser::escapeQuotes(environmentMappingId) +
			"\" " +
			(selectedEnvironmentMappingId == environmentMappingId?"selected=\"true\" ":"") +
			" />\n";
	}
	environmentMappingIdsInnerNodeSubNodesXML =
		environmentMappingIdsInnerNodeSubNodesXML +
		"</scrollarea>\n";
	try {
		environmentMappingIdsDropDownInnerNode->replaceSubNodes(environmentMappingIdsInnerNodeSubNodesXML, true);
	} catch (Exception& exception) {
		Console::print(string("SceneEditorScreenController::setObjectReflectionsEnvironmentMappings(): An error occurred: "));
		Console::println(string(exception.what()));
	}
}

void SceneEditorScreenController::unsetEntityReflectionsEnvironmentMappings() {
	entityReflectionsEnvironmentmappingDropDown->getController()->setDisabled(true);
	entityReflectionsEnvironmentmappingDropDown->getController()->setValue(MutableString());
	btnEntityReflectionsEnvironmentmappingApply->getController()->setDisabled(true);
}

void SceneEditorScreenController::onValueChanged(GUIElementNode* node)
{
	if (node->getId().compare("objects_listbox") == 0) {
	} else
	if (node->getId().compare("map_properties_listbox") == 0) {
		onScenePropertiesSelectionChanged();
	} else
	if (node->getId().compare("object_properties_listbox") == 0) {
		onEntityPropertiesSelectionChanged();
	} else {
		Console::println(string("SceneEditorScreenController::onValueChanged: " + node->getId()));
	}
}

void SceneEditorScreenController::onActionPerformed(GUIActionListenerType type, GUIElementNode* node)
{
	if (type == GUIActionListenerType::PERFORMED) {
		if (node->getId().compare("button_objects_select") == 0) {
			onEntitiesSelect();
		} else
		if (node->getId().compare("button_objects_unselect") == 0) {
			onEntitiesUnselect();
		} else
		if (node->getId().compare("button_grid_apply") == 0) {
			onGridApply();
		} else
		if (node->getId().compare("button_snapping_apply") == 0) {
			onSnappingApply();
		} else
		if (node->getId().compare("button_map_load") == 0) {
			onSceneLoad();
		} else
		if (node->getId().compare("button_map_save") == 0) {
			onSceneSave();
		} else
		if (node->getId().compare("button_map_properties_add") == 0) {
			onScenePropertyAdd();
		} else
		if (node->getId().compare("button_map_properties_remove") == 0) {
			onScenePropertyRemove();
		} else
		if (node->getId().compare("button_map_properties_save") == 0) {
			onScenePropertySave();
		} else
		if (node->getId().compare("button_objectdata_apply") == 0) {
			onEntityDataApply();
		} else
		if (node->getId().compare("button_translation_apply") == 0) {
			onEntityTranslationApply();
		} else
		if (node->getId().compare("button_scale_apply") == 0) {
			onEntityScaleApply();
		} else
		if (node->getId().compare("button_rotation_apply") == 0) {
			onEntityRotationsApply();
		} else
		if (node->getId().compare("button_object_color") == 0) {
			onEntityColor();
		} else
		if (node->getId().compare("button_object_center") == 0) {
			onEntityCenter();
		} else
		if (node->getId().compare("button_object_remove") == 0) {
			onEntityRemove();
		} else
		if (node->getId().compare("button_object_properties_presetapply") == 0) {
			onEntityPropertyPresetApply();
		} else
		if (node->getId().compare("button_object_properties_add") == 0) {
			onEntityPropertyAdd();
		} else
		if (node->getId().compare("button_object_properties_remove") == 0) {
			onEntityPropertyRemove();
		} else
		if (node->getId().compare("button_object_properties_save") == 0) {
			onEntityPropertySave();
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
		} else
		if (node->getId().compare("button_map_sky_model_load") == 0) {
			onMapSkyModelLoad();
		} else
		if (node->getId().compare("button_map_sky_model_clear") == 0) {
			onMapSkyModelClear();
		} else
		if (node->getId().compare("button_map_sky_apply") == 0) {
			onMapSkyApply();
		} else
		if (node->getId().compare("button_object_reflections_environmentmapping_apply") == 0) {
			view->applyReflectionEnvironmentMappingId(entityReflectionsEnvironmentmappingDropDown->getController()->getValue().getString());
		}
	}
}

void SceneEditorScreenController::showErrorPopUp(const string& caption, const string& message)
{
	Console::println(string(caption + ":" + message));
	view->getPopUps()->getInfoDialogScreenController()->show(caption, message);
}

void SceneEditorScreenController::getViewPort(int& left, int& top, int& width, int& height) {
	auto& constraints = viewPort->getComputedConstraints();
	left = constraints.left + constraints.alignmentLeft + constraints.contentAlignmentLeft;
	top = constraints.top + constraints.alignmentTop + constraints.contentAlignmentTop;
	width = constraints.width;
	height = constraints.height;
}
