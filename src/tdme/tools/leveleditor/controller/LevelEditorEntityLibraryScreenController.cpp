#include <tdme/tools/leveleditor/controller/LevelEditorEntityLibraryScreenController.h>

#include <string>

#include <tdme/engine/fileio/models/ModelReader.h>
#include <tdme/engine/fileio/models/TMWriter.h>
#include <tdme/utilities/ModelTools.h>
#include <tdme/gui/GUIParser.h>
#include <tdme/gui/events/Action.h>
#include <tdme/gui/events/GUIChangeListener.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUINodeController.h>
#include <tdme/gui/nodes/GUIParentNode.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/tools/leveleditor/TDMELevelEditor.h>
#include <tdme/tools/leveleditor/views/EmptyView.h>
#include <tdme/tools/leveleditor/views/LevelEditorView.h>
#include <tdme/tools/leveleditor/views/ModelEditorView.h>
#include <tdme/tools/leveleditor/views/ParticleSystemView.h>
#include <tdme/tools/leveleditor/views/TriggerView.h>
#include <tdme/tools/shared/controller/FileDialogScreenController.h>
#include <tdme/tools/shared/controller/InfoDialogScreenController.h>
#include <tdme/tools/shared/model/LevelEditorEntity_EntityType.h>
#include <tdme/tools/shared/model/LevelEditorEntity.h>
#include <tdme/tools/shared/model/LevelEditorEntityLibrary.h>
#include <tdme/tools/shared/model/LevelEditorEntityModel.h>
#include <tdme/tools/shared/model/LevelEditorObject.h>
#include <tdme/tools/shared/model/LevelEditorLevel.h>
#include <tdme/tools/shared/tools/Tools.h>
#include <tdme/tools/shared/views/PopUps.h>
#include <tdme/tools/shared/views/View.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>
#include <tdme/utilities/MutableString.h>
#include <tdme/utilities/StringTools.h>

using std::string;
using std::to_string;

using tdme::tools::leveleditor::controller::LevelEditorEntityLibraryScreenController;
using tdme::engine::fileio::models::ModelReader;
using tdme::engine::fileio::models::TMWriter;
using tdme::utilities::ModelTools;
using tdme::gui::GUIParser;
using tdme::gui::events::Action;
using tdme::gui::events::GUIActionListenerType;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeController;
using tdme::gui::nodes::GUIParentNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;
using tdme::tools::leveleditor::TDMELevelEditor;
using tdme::tools::leveleditor::views::EmptyView;
using tdme::tools::leveleditor::views::LevelEditorView;
using tdme::tools::leveleditor::views::ModelEditorView;
using tdme::tools::leveleditor::views::ParticleSystemView;
using tdme::tools::leveleditor::views::TriggerView;
using tdme::tools::shared::controller::FileDialogScreenController;
using tdme::tools::shared::controller::InfoDialogScreenController;
using tdme::tools::shared::model::LevelEditorEntity_EntityType;
using tdme::tools::shared::model::LevelEditorEntity;
using tdme::tools::shared::model::LevelEditorEntityLibrary;
using tdme::tools::shared::model::LevelEditorEntityModel;
using tdme::tools::shared::model::LevelEditorObject;
using tdme::tools::shared::model::LevelEditorLevel;
using tdme::tools::shared::tools::Tools;
using tdme::tools::shared::views::PopUps;
using tdme::tools::shared::views::View;
using tdme::utilities::Console;
using tdme::utilities::Exception;
using tdme::utilities::StringTools;
using tdme::utilities::MutableString;

LevelEditorEntityLibraryScreenController::LevelEditorEntityLibraryScreenController(PopUps* popUps)
{
	this->popUps = popUps;
	this->modelPath = ".";
}

GUIScreenNode* LevelEditorEntityLibraryScreenController::getScreenNode()
{
	return screenNode;
}

const string& LevelEditorEntityLibraryScreenController::getModelPath()
{
	return modelPath;
}

void LevelEditorEntityLibraryScreenController::setModelPath(const string& modelPath)
{
	this->modelPath = modelPath;
}

void LevelEditorEntityLibraryScreenController::initialize()
{
	try {
		screenNode = GUIParser::parse("resources/engine/tools/leveleditor/gui", "screen_leveleditor_entitylibrary.xml");
		screenNode->addActionListener(this);
		screenNode->addChangeListener(this);
		entityLibraryListBox = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("entity_library_listbox"));
		buttonEntityPlace = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_entity_place"));
		buttonLevelEdit = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_level_edit"));
	} catch (Exception& exception) {
		Console::print(string("LevelEditorEntityLibraryScreenController::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	buttonEntityPlace->getController()->setDisabled(false);
	buttonLevelEdit->getController()->setDisabled(true);
}

void LevelEditorEntityLibraryScreenController::dispose()
{
}

void LevelEditorEntityLibraryScreenController::selectEntity(int32_t entityId)
{
	MutableString entityLibraryListBoxSelection;
	entityLibraryListBoxSelection.set(entityId);
	entityLibraryListBox->getController()->setValue(entityLibraryListBoxSelection);
}

void LevelEditorEntityLibraryScreenController::setEntityLibrary()
{
	MutableString entityLibraryListBoxSelection;
	auto entityLibrary = TDMELevelEditor::getInstance()->getEntityLibrary();
	entityLibraryListBoxSelection.set(entityLibraryListBox->getController()->getValue());
	auto entityLibraryListBoxInnerNode = dynamic_cast< GUIParentNode* >((entityLibraryListBox->getScreenNode()->getNodeById(entityLibraryListBox->getId() + "_inner")));
	auto idx = 1;
	string entityLibraryListBoxSubNodesXML;
	entityLibraryListBoxSubNodesXML =
		entityLibraryListBoxSubNodesXML +
		"<scrollarea-vertical id=\"" +
		entityLibraryListBox->getId() +
		"_inner_scrollarea\" width=\"100%\" height=\"100%\">\n";
	for (auto i = 0; i < entityLibrary->getEntityCount(); i++) {
		auto objectId = entityLibrary->getEntityAt(i)->getId();
		auto objectName = entityLibrary->getEntityAt(i)->getName();
		entityLibraryListBoxSubNodesXML =
			entityLibraryListBoxSubNodesXML +
			"<selectbox-option text=\"" +
			GUIParser::escapeQuotes(objectName) +
			"\" value=\"" +
			to_string(objectId) +
			"\" " +
			(i == 0 ? "selected=\"true\" " : "") +
			"/>\n";
	}
	entityLibraryListBoxSubNodesXML = entityLibraryListBoxSubNodesXML + "</scrollarea-vertical>\n";
	try {
		entityLibraryListBoxInnerNode->replaceSubNodes(entityLibraryListBoxSubNodesXML, false);
	} catch (Exception& exception) {
		Console::print(string("LevelEditorEntityLibraryScreenController::setEntityLibrary(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	if (entityLibraryListBoxSelection.length() > 0) {
		entityLibraryListBox->getController()->setValue(entityLibraryListBoxSelection);
	}
	onEntitySelectionChanged();
	buttonEntityPlace->getController()->setDisabled(entityLibrary->getEntityCount() == 0);
}

void LevelEditorEntityLibraryScreenController::onEntitySelectionChanged()
{
	auto view = TDMELevelEditor::getInstance()->getView();
	if (dynamic_cast< LevelEditorView* >(view) != nullptr) {
		auto entity = TDMELevelEditor::getInstance()->getEntityLibrary()->getEntity(Tools::convertToIntSilent(entityLibraryListBox->getController()->getValue().getString()));
		if (entity != nullptr) {
			(dynamic_cast< LevelEditorView* >(view))->loadEntityFromLibrary(entity->getId());
		}
	}
}

void LevelEditorEntityLibraryScreenController::onEditEntity()
{
	auto entity = TDMELevelEditor::getInstance()->getEntityLibrary()->getEntity(Tools::convertToIntSilent(entityLibraryListBox->getController()->getValue().getString()));
	if (entity == nullptr) return;
	{
		auto v = entity->getType();
		if (v == LevelEditorEntity_EntityType::MODEL) {
			if (dynamic_cast< ModelEditorView* >(TDMELevelEditor::getInstance()->getView()) != nullptr == false) {
				TDMELevelEditor::getInstance()->switchToModelEditor();
			}
			(dynamic_cast< ModelEditorView* >(TDMELevelEditor::getInstance()->getView()))->setEntity(entity);
		} else
		if (v == LevelEditorEntity_EntityType::TRIGGER) {
			if (dynamic_cast< TriggerView* >(TDMELevelEditor::getInstance()->getView()) != nullptr == false) {
				TDMELevelEditor::getInstance()->switchToTriggerView();
			}
			(dynamic_cast< TriggerView* >(TDMELevelEditor::getInstance()->getView()))->setEntity(entity);
		} else
		if (v == LevelEditorEntity_EntityType::EMPTY) {
			if (dynamic_cast< EmptyView* >(TDMELevelEditor::getInstance()->getView()) != nullptr == false) {
				TDMELevelEditor::getInstance()->switchToEmptyView();
			}
			(dynamic_cast< EmptyView* >(TDMELevelEditor::getInstance()->getView()))->setEntity(entity);
		} else
		if (v == LevelEditorEntity_EntityType::PARTICLESYSTEM) {
			if (dynamic_cast< ParticleSystemView* >(TDMELevelEditor::getInstance()->getView()) != nullptr == false) {
				TDMELevelEditor::getInstance()->switchToParticleSystemView();
			}
			(dynamic_cast< ParticleSystemView* >(TDMELevelEditor::getInstance()->getView()))->setEntity(entity);
		}
	}

	buttonEntityPlace->getController()->setDisabled(true);
	buttonLevelEdit->getController()->setDisabled(false);
}

void LevelEditorEntityLibraryScreenController::onEditLevel()
{
	TDMELevelEditor::getInstance()->switchToLevelEditor();
	buttonEntityPlace->getController()->setDisabled(false);
	buttonLevelEdit->getController()->setDisabled(true);
}

void LevelEditorEntityLibraryScreenController::onPlaceEntity()
{
	auto entity = TDMELevelEditor::getInstance()->getEntityLibrary()->getEntity(Tools::convertToIntSilent(entityLibraryListBox->getController()->getValue().getString()));
	if (entity == nullptr) return;
	auto view = TDMELevelEditor::getInstance()->getView();
	if (dynamic_cast< LevelEditorView* >(view) != nullptr) {
		(dynamic_cast< LevelEditorView* >(view))->setPlaceObjectMode();
	}
}

void LevelEditorEntityLibraryScreenController::onDeleteEntity()
{
	auto entity = TDMELevelEditor::getInstance()->getEntityLibrary()->getEntity(Tools::convertToIntSilent(entityLibraryListBox->getController()->getValue().getString()));
	if (entity == nullptr) return;
	TDMELevelEditor::getInstance()->getLevel()->removeObjectsByEntityId(entity->getId());
	auto view = TDMELevelEditor::getInstance()->getView();
	if (dynamic_cast< LevelEditorView* >(view) != nullptr) {
		(dynamic_cast< LevelEditorView* >(view))->loadLevel();
	} else {
		TDMELevelEditor::getInstance()->switchToLevelEditor();
	}
	TDMELevelEditor::getInstance()->getLevel()->getEntityLibrary()->removeEntity(entity->getId());
	setEntityLibrary();
}

void LevelEditorEntityLibraryScreenController::onPartitionEntity()
{
	// check if we have a entity
	auto entity = TDMELevelEditor::getInstance()->getEntityLibrary()->getEntity(Tools::convertToIntSilent(entityLibraryListBox->getController()->getValue().getString()));
	if (entity == nullptr || entity->getType() != LevelEditorEntity_EntityType::MODEL) return;
	// TODO: there can always be the tdme default animation, do not do skinned objects
	if (/*entity->getModel()->hasAnimations() == true || */entity->getModel()->hasSkinning() == true) {
		popUps->getInfoDialogScreenController()->show("Warning", "This model has animations or skinning");
		return;
	}

	// check if entity exists only once
	vector<string> objectsByEntityId;
	TDMELevelEditor::getInstance()->getLevel()->getObjectsByEntityId(entity->getId(), objectsByEntityId);
	if (objectsByEntityId.size() != 1) {
		popUps->getInfoDialogScreenController()->show("Warning", "This model has several object instances");
		return;
	}

	//
	auto level = TDMELevelEditor::getInstance()->getLevel();
	auto levelEntityLibrary = level->getEntityLibrary();
	auto levelEditorObject = level->getObjectById(objectsByEntityId[0]);

	// partition object
	map<string, Model*> modelsByPartition;
	map<string, Vector3> modelsPosition;
	ModelTools::partition(
		levelEditorObject->getEntity()->getModel(),
		levelEditorObject->getTransformations(),
		modelsByPartition,
		modelsPosition
	);

	try {
		// add partitions to level
		auto pathName = Tools::getPath(entity->getFileName());
		auto fileName = Tools::getFileName(entity->getFileName());
		for (auto modelsByPartitionIt: modelsByPartition) {
			auto key = modelsByPartitionIt.first;
			auto model = modelsByPartitionIt.second;
			auto fileNamePartition = StringTools::substring(fileName, 0, StringTools::lastIndexOf(fileName, '.') - 1) + "." + key + ".tm";

			// create entity
			TMWriter::write(
				model,
				pathName,
				fileNamePartition
			);
			auto levelEditorEntityPartition = levelEntityLibrary->addModel(
				LevelEditorEntityLibrary::ID_ALLOCATE,
				model->getName(),
				model->getName(),
				pathName,
				fileNamePartition,
				Vector3(0.0f, 0.0f, 0.0f)
			);
			levelEditorEntityPartition->setShader(entity->getShader());
			levelEditorEntityPartition->setDistanceShader(entity->getDistanceShader());
			levelEditorEntityPartition->setDistanceShaderDistance(entity->getDistanceShaderDistance());

			// avoid name collision
			auto objectName = model->getName();
			while (level->getObjectById(objectName) != nullptr) {
				objectName+= ".p";
			}

			// add to level
			auto levelEditorObjectPartition = new LevelEditorObject(
				objectName,
				"",
				levelEditorObject->getTransformations(),
				levelEditorEntityPartition
			);
			levelEditorEntityPartition->getModelSettings()->setTerrainMesh(levelEditorObject->getEntity()->getModelSettings()->isTerrainMesh());

			// add to objects
			level->addObject(levelEditorObjectPartition);
		}
	} catch (Exception& exception) {
		popUps->getInfoDialogScreenController()->show("Warning", exception.what());
	}

	// remove original object
	level->removeObjectsByEntityId(entity->getId());
	// TODO: check if to delete original model
	//	as long as .tl has not been saved it is still required to have this file
	// FileSystem::getInstance()->removeFile(pathName, fileName);

	// (re-)load level editor view
	auto view = TDMELevelEditor::getInstance()->getView();
	if (dynamic_cast< LevelEditorView* >(view) != nullptr) {
		(dynamic_cast< LevelEditorView* >(view))->loadLevel();
	} else {
		TDMELevelEditor::getInstance()->switchToLevelEditor();
	}

	// remove original entity from entity library
	// TODO: delete file
	level->getEntityLibrary()->removeEntity(entity->getId());

	//
	setEntityLibrary();
}

void LevelEditorEntityLibraryScreenController::onValueChanged(GUIElementNode* node)
{
	if (node->getId().compare("entity_library_listbox") == 0) {
		onEntitySelectionChanged();
	} else
	if (node->getId().compare("dropdown_entity_action") == 0) {
		if (node->getController()->getValue().getString() == "edit") {
			onEditEntity();
		} else
		if (node->getController()->getValue().getString() == "delete") {
			onDeleteEntity();
		} else
		if (node->getController()->getValue().getString() == "partition") {
			onPartitionEntity();
		} else
		if (node->getController()->getValue().getString() == "create_model") {
			class OnCreateModel: public virtual Action
			{
			public:
				void performAction() override {
					try {
						auto entity = entityLibrary->addModel(
							LevelEditorEntityLibrary::ID_ALLOCATE,
							levelEditorEntityLibraryScreenController->popUps->getFileDialogScreenController()->getFileName(),
							"",
							levelEditorEntityLibraryScreenController->popUps->getFileDialogScreenController()->getPathName(),
							levelEditorEntityLibraryScreenController->popUps->getFileDialogScreenController()->getFileName(),
							Vector3(0.0f, 0.0f, 0.0f)
						);
						entity->setDefaultBoundingVolumes();
						levelEditorEntityLibraryScreenController->setEntityLibrary();
						levelEditorEntityLibraryScreenController->entityLibraryListBox->getController()->setValue(MutableString(entity->getId()));
						levelEditorEntityLibraryScreenController->onEditEntity();
					} catch (Exception& exception) {
						levelEditorEntityLibraryScreenController->popUps->getInfoDialogScreenController()->show(
							"Error",
							"An error occurred: " + string(exception.what())
						);
					}
					levelEditorEntityLibraryScreenController->modelPath = levelEditorEntityLibraryScreenController->popUps->getFileDialogScreenController()->getPathName();
					levelEditorEntityLibraryScreenController->popUps->getFileDialogScreenController()->close();
				}

				/**
				 * Public constructor
				 * @param levelEditorEntityLibraryScreenController level editor entity library screen controller
				 * @param entityLibrary entity library
				 */
				OnCreateModel(LevelEditorEntityLibraryScreenController* levelEditorEntityLibraryScreenController, LevelEditorEntityLibrary* entityLibrary)
					: levelEditorEntityLibraryScreenController(levelEditorEntityLibraryScreenController)
					, entityLibrary(entityLibrary) {
				}

			private:
				LevelEditorEntityLibraryScreenController *levelEditorEntityLibraryScreenController;
				LevelEditorEntityLibrary* entityLibrary;
			};

			auto const entityLibrary = TDMELevelEditor::getInstance()->getEntityLibrary();
			vector<string> extensions = ModelReader::getModelExtensions();
			extensions.push_back("tmm");
			popUps->getFileDialogScreenController()->show(
				modelPath,
				"Load from: ",
				extensions,
				"",
				true,
				new OnCreateModel(this, entityLibrary)
			);
		} else
		if (node->getController()->getValue().getString() == "create_trigger") {
			try {
				auto model = TDMELevelEditor::getInstance()->getEntityLibrary()->addTrigger(LevelEditorEntityLibrary::ID_ALLOCATE, "New trigger", "", 1.0f, 1.0f, 1.0f);
				setEntityLibrary();
				entityLibraryListBox->getController()->setValue(MutableString(model->getId()));
				onEditEntity();
			} catch (Exception& exception) {
				popUps->getInfoDialogScreenController()->show(
					"Error",
					"An error occurred: " + (string(exception.what()))
				);
			}
		} else
		if (node->getController()->getValue().getString() == "create_empty") {
			try {
				auto model = TDMELevelEditor::getInstance()->getEntityLibrary()->addEmpty(LevelEditorEntityLibrary::ID_ALLOCATE, "New empty", "");
				setEntityLibrary();
				entityLibraryListBox->getController()->setValue(MutableString(model->getId()));
				onEditEntity();
			} catch (Exception& exception) {
				popUps->getInfoDialogScreenController()->show(
					"Error",
					"An error occurred: " + (string(exception.what()))
				 );
			}
		} else
		if (node->getController()->getValue().getString() == "create_light") {
		} else
		if (node->getController()->getValue().getString() == "create_particlesystem") {
			try {
				auto model = TDMELevelEditor::getInstance()->getEntityLibrary()->addParticleSystem(LevelEditorEntityLibrary::ID_ALLOCATE, "New particle system", "");
				setEntityLibrary();
				entityLibraryListBox->getController()->setValue(MutableString(model->getId()));
				onEditEntity();
			} catch (Exception& exception) {
				popUps->getInfoDialogScreenController()->show(
					"Error",
					"An error occurred: " + (string(exception.what()))
				 );
			}
		} else {
			Console::println("LevelEditorEntityLibraryScreenController::onValueChanged: dropdown_model_create: " + node->getController()->getValue().getString());
		}
		node->getController()->setValue(MutableString("action"));
	}
}

void LevelEditorEntityLibraryScreenController::onActionPerformed(GUIActionListenerType type, GUIElementNode* node)
{
	if (type == GUIActionListenerType::PERFORMED) {
		if (node->getId().compare("button_entity_place") == 0) {
			onPlaceEntity();
		} else if (node->getId().compare("button_level_edit") == 0) {
			onEditLevel();
		}
	}
}
