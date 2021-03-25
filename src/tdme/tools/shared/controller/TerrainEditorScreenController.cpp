#include <tdme/tools/shared/controller/TerrainEditorScreenController.h>

#include <map>
#include <string>
#include <vector>

#include <tdme/engine/fileio/prototypes/PrototypeReader.h>
#include <tdme/engine/fileio/textures/Texture.h>
#include <tdme/engine/fileio/textures/TextureReader.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/prototype/Prototype.h>
#include <tdme/engine/prototype/PrototypeTerrain.h>
#include <tdme/gui/events/Action.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUINodeController.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/gui/nodes/GUITextNode.h>
#include <tdme/gui/GUIParser.h>
#include <tdme/tools/shared/controller/FileDialogPath.h>
#include <tdme/tools/shared/controller/FileDialogScreenController.h>
#include <tdme/tools/shared/controller/InfoDialogScreenController.h>
#include <tdme/tools/shared/controller/PrototypeBaseSubScreenController.h>
#include <tdme/tools/shared/tools/Tools.h>
#include <tdme/tools/shared/views/PopUps.h>
#include <tdme/tools/shared/views/SharedTerrainEditorView.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>
#include <tdme/utilities/Float.h>
#include <tdme/utilities/MutableString.h>
#include <tdme/utilities/Terrain.h>

using std::map;
using std::string;
using std::to_string;
using std::vector;

using tdme::engine::fileio::prototypes::PrototypeReader;
using tdme::engine::fileio::textures::Texture;
using tdme::engine::fileio::textures::TextureReader;
using tdme::engine::model::Model;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::prototype::Prototype;
using tdme::engine::prototype::PrototypeTerrain;
using tdme::gui::events::Action;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeController;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::nodes::GUITextNode;
using tdme::gui::GUIParser;
using tdme::tools::shared::controller::FileDialogPath;
using tdme::tools::shared::controller::FileDialogScreenController;
using tdme::tools::shared::controller::InfoDialogScreenController;
using tdme::tools::shared::controller::PrototypeBaseSubScreenController;
using tdme::tools::shared::controller::TerrainEditorScreenController;
using tdme::tools::shared::tools::Tools;
using tdme::tools::shared::views::PopUps;
using tdme::tools::shared::views::SharedTerrainEditorView;
using tdme::utilities::Console;
using tdme::utilities::Exception;
using tdme::utilities::Float;
using tdme::utilities::MutableString;
using tdme::utilities::Terrain;

TerrainEditorScreenController::TerrainEditorScreenController(SharedTerrainEditorView* view)
{
	class OnSetEntityDataAction: public virtual Action
	{
	public:
		void performAction() override {
			finalView->updateGUIElements();
			finalView->onSetPrototypeData();
		}

		/**
		 * Public constructor
		 * @param terrainEditorScreenController terrain editor screen controller
		 * @param finalView final view
		 */
		OnSetEntityDataAction(TerrainEditorScreenController* terrainEditorScreenController, SharedTerrainEditorView* finalView): terrainEditorScreenController(terrainEditorScreenController), finalView(finalView) {
		}

	private:
		TerrainEditorScreenController* terrainEditorScreenController;
		SharedTerrainEditorView* finalView;
	};

	this->view = view;
	auto const finalView = view;
	this->prototypeBaseSubScreenController = new PrototypeBaseSubScreenController(view->getPopUps(), new OnSetEntityDataAction(this, finalView));
}

GUIScreenNode* TerrainEditorScreenController::getScreenNode()
{
	return screenNode;
}

void TerrainEditorScreenController::initialize()
{
	try {
		screenNode = GUIParser::parse("resources/engine/gui", "screen_terraineditor.xml");
		screenNode->addActionListener(this);
		screenNode->addChangeListener(this);
		screenCaption = dynamic_cast< GUITextNode* >(screenNode->getNodeById("screen_caption"));
		viewPort = dynamic_cast<GUIElementNode*>(screenNode->getNodeById("viewport"));
		terrainDimensionWidth = dynamic_cast<GUIElementNode*>(screenNode->getNodeById("terrain_dimension_width"));
		terrainDimensionDepth = dynamic_cast<GUIElementNode*>(screenNode->getNodeById("terrain_dimension_depth"));
		btnTerrainDimensionApply = dynamic_cast<GUIElementNode*>(screenNode->getNodeById("button_terrain_dimension_apply"));
		btnTerrainDimensionLoad = dynamic_cast<GUIElementNode*>(screenNode->getNodeById("button_terrain_dimension_load"));
		btnTerrainDimensionSave = dynamic_cast<GUIElementNode*>(screenNode->getNodeById("button_terrain_dimension_save"));
		terrainBrushScale = dynamic_cast<GUIElementNode*>(screenNode->getNodeById("terrain_brush_scale"));
		terrainBrushStrength = dynamic_cast<GUIElementNode*>(screenNode->getNodeById("terrain_brush_strength"));
		terrainBrushFile = dynamic_cast<GUIElementNode*>(screenNode->getNodeById("terrain_brush_file"));
		terrainBrushFileLoad = dynamic_cast<GUIElementNode*>(screenNode->getNodeById("terrain_brush_file_load"));
		terrainBrushFileClear = dynamic_cast<GUIElementNode*>(screenNode->getNodeById("terrain_brush_file_clear"));
		btnTerrainBrushApply = dynamic_cast<GUIElementNode*>(screenNode->getNodeById("button_terrain_brush_apply"));

		foliageBrushScale = dynamic_cast<GUIElementNode*>(screenNode->getNodeById("foliage_brush_scale"));
		foliageBrushDensity = dynamic_cast<GUIElementNode*>(screenNode->getNodeById("foliage_brush_density"));
		foliageBrushFile = dynamic_cast<GUIElementNode*>(screenNode->getNodeById("foliage_brush_file"));
		foliageBrushFileLoad = dynamic_cast<GUIElementNode*>(screenNode->getNodeById("foliage_brush_file_load"));
		foliageBrushFileClear = dynamic_cast<GUIElementNode*>(screenNode->getNodeById("foliage_brush_file_clear"));
		btnFoliageBrushApply = dynamic_cast<GUIElementNode*>(screenNode->getNodeById("button_foliage_brush_apply"));

		for (auto i = 0; i < 5; i++) {
			foliageBrushPrototypeFileCount[i] = dynamic_cast<GUIElementNode*>(screenNode->getNodeById("foliage_brush_prototype_" + to_string(i + 1) + "_count"));
			foliageBrushPrototypeFile[i] = dynamic_cast<GUIElementNode*>(screenNode->getNodeById("foliage_brush_prototype_" + to_string(i + 1)));
			foliageBrushPrototypeFileLoad[i] = dynamic_cast<GUIElementNode*>(screenNode->getNodeById("foliage_brush_prototype_" + to_string(i + 1) + "_load"));
			foliageBrushPrototypeFileClear[i] = dynamic_cast<GUIElementNode*>(screenNode->getNodeById("foliage_brush_prototype_" + to_string(i + 1) + "_clear"));
			foliageBrushPrototypeScaleMin[i] = dynamic_cast<GUIElementNode*>(screenNode->getNodeById("foliage_brush_prototype_" + to_string(i + 1) + "_scale_min"));
			foliageBrushPrototypeScaleMax[i] = dynamic_cast<GUIElementNode*>(screenNode->getNodeById("foliage_brush_prototype_" + to_string(i + 1) + "_scale_max"));
			foliageBrushPrototypeRangeXMin[i] = dynamic_cast<GUIElementNode*>(screenNode->getNodeById("foliage_brush_prototype_" + to_string(i + 1) + "_range_xmin"));
			foliageBrushPrototypeRangeXMax[i] = dynamic_cast<GUIElementNode*>(screenNode->getNodeById("foliage_brush_prototype_" + to_string(i + 1) + "_range_xmax"));
			foliageBrushPrototypeRangeYMin[i] = dynamic_cast<GUIElementNode*>(screenNode->getNodeById("foliage_brush_prototype_" + to_string(i + 1) + "_range_ymin"));
			foliageBrushPrototypeRangeYMax[i] = dynamic_cast<GUIElementNode*>(screenNode->getNodeById("foliage_brush_prototype_" + to_string(i + 1) + "_range_ymax"));
			foliageBrushPrototypeRangeZMin[i] = dynamic_cast<GUIElementNode*>(screenNode->getNodeById("foliage_brush_prototype_" + to_string(i + 1) + "_range_zmin"));
			foliageBrushPrototypeRangeZMax[i] = dynamic_cast<GUIElementNode*>(screenNode->getNodeById("foliage_brush_prototype_" + to_string(i + 1) + "_range_zmax"));
			foliageBrushPrototypeSlopeMin[i] = dynamic_cast<GUIElementNode*>(screenNode->getNodeById("foliage_brush_prototype_" + to_string(i + 1) + "_slope_min"));
			foliageBrushPrototypeSlopeMax[i] = dynamic_cast<GUIElementNode*>(screenNode->getNodeById("foliage_brush_prototype_" + to_string(i + 1) + "_slope_max"));
			foliageBrushPrototypeHeightMin[i] = dynamic_cast<GUIElementNode*>(screenNode->getNodeById("foliage_brush_prototype_" + to_string(i + 1) + "_height_min"));
			foliageBrushPrototypeHeightMax[i] = dynamic_cast<GUIElementNode*>(screenNode->getNodeById("foliage_brush_prototype_" + to_string(i + 1) + "_height_max"));
			foliageBrushPrototypeNormalAlign[i] = dynamic_cast<GUIElementNode*>(screenNode->getNodeById("foliage_brush_prototype_" + to_string(i + 1) + "_normalalign"));
		}

	} catch (Exception& exception) {
		Console::print(string("TerrainEditorScreenController::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	prototypeBaseSubScreenController->initialize(screenNode);
	btnTerrainDimensionSave->getController()->setDisabled(true);
	onApplyTerrainBrush();
	for (auto i = 0; i < 5; i++) {
		foliageBrushPrototypeScaleMin[i]->getController()->setValue(MutableString(1.0f));
		foliageBrushPrototypeScaleMax[i]->getController()->setValue(MutableString(1.0f));
		foliageBrushPrototypeRangeXMin[i]->getController()->setValue(MutableString(0.0f));
		foliageBrushPrototypeRangeXMax[i]->getController()->setValue(MutableString(0.0f));
		foliageBrushPrototypeRangeYMin[i]->getController()->setValue(MutableString(0.0f));
		foliageBrushPrototypeRangeYMax[i]->getController()->setValue(MutableString(0.0f));
		foliageBrushPrototypeRangeZMin[i]->getController()->setValue(MutableString(0.0f));
		foliageBrushPrototypeRangeZMax[i]->getController()->setValue(MutableString(0.0f));
		foliageBrushPrototypeSlopeMin[i]->getController()->setValue(MutableString(0.0f));
		foliageBrushPrototypeSlopeMax[i]->getController()->setValue(MutableString(90.0f));
		foliageBrushPrototypeHeightMin[i]->getController()->setValue(MutableString(0.0f));
		foliageBrushPrototypeHeightMax[i]->getController()->setValue(MutableString(100.0f));
		foliageBrushPrototypeNormalAlign[i]->getController()->setValue(MutableString());
	}
}

void TerrainEditorScreenController::dispose()
{
	if (currentTerrainBrushTexture != nullptr) currentTerrainBrushTexture->releaseReference();
	delete prototypeBaseSubScreenController;
}

void TerrainEditorScreenController::setScreenCaption(const string& text)
{
	screenCaption->setText(text);
}

void TerrainEditorScreenController::setPrototypeData(const string& name, const string& description)
{
	prototypeBaseSubScreenController->setPrototypeData(name, description);
}

void TerrainEditorScreenController::unsetPrototypeData()
{
	prototypeBaseSubScreenController->unsetPrototypeData();
}

void TerrainEditorScreenController::setPrototypeProperties(const string& presetId, const string& selectedName)
{
	prototypeBaseSubScreenController->setPrototypeProperties(view->getPrototype(), presetId, selectedName);
}

void TerrainEditorScreenController::unsetPrototypeProperties()
{
	prototypeBaseSubScreenController->unsetPrototypeProperties();
}

void TerrainEditorScreenController::showErrorPopUp(const string& caption, const string& message)
{
	view->getPopUps()->getInfoDialogScreenController()->show(caption, message);
}

void TerrainEditorScreenController::onValueChanged(GUIElementNode* node)
{
	prototypeBaseSubScreenController->onValueChanged(node, view->getPrototype());
}

void TerrainEditorScreenController::onActionPerformed(GUIActionListenerType type, GUIElementNode* node)
{
	prototypeBaseSubScreenController->onActionPerformed(type, node, view->getPrototype());
	if (type == GUIActionListenerType::PERFORMED) {
		if (node->getId().compare(btnTerrainDimensionApply->getId()) == 0) {
			onApplyTerrainDimension();
		} else
		if (node->getId().compare(btnTerrainDimensionLoad->getId()) == 0) {
			onTerrainLoad();
		} else
		if (node->getId().compare(btnTerrainDimensionSave->getId()) == 0) {
			onTerrainSave();
		} else
		if (node->getId().compare(terrainBrushFileLoad->getId()) == 0) {
			onTerrainBrushFileLoad();
		} else
		if (node->getId().compare(terrainBrushFileClear->getId()) == 0) {
			onTerrainBrushFileClear();
		} else
		if (node->getId().compare(btnTerrainBrushApply->getId()) == 0) {
			onApplyTerrainBrush();
		} else
		if (node->getId().compare(foliageBrushFileLoad->getId()) == 0) {
			onFoliageBrushFileLoad();
		} else
		if (node->getId().compare(foliageBrushFileClear->getId()) == 0) {
			onFoliageBrushFileClear();
		} else
		if (node->getId().compare(foliageBrushPrototypeFileLoad[0]->getId()) == 0) {
			onFoliageBrushPrototypeLoad(0);
		} else
		if (node->getId().compare(foliageBrushPrototypeFileClear[0]->getId()) == 0) {
			onFoliageBrushPrototypeClear(0);
		} else
		if (node->getId().compare(foliageBrushPrototypeFileLoad[1]->getId()) == 0) {
			onFoliageBrushPrototypeLoad(1);
		} else
		if (node->getId().compare(foliageBrushPrototypeFileClear[1]->getId()) == 0) {
			onFoliageBrushPrototypeClear(1);
		} else
		if (node->getId().compare(foliageBrushPrototypeFileLoad[2]->getId()) == 0) {
			onFoliageBrushPrototypeLoad(2);
		} else
		if (node->getId().compare(foliageBrushPrototypeFileClear[2]->getId()) == 0) {
			onFoliageBrushPrototypeClear(2);
		} else
		if (node->getId().compare(foliageBrushPrototypeFileLoad[3]->getId()) == 0) {
			onFoliageBrushPrototypeLoad(3);
		} else
		if (node->getId().compare(foliageBrushPrototypeFileClear[3]->getId()) == 0) {
			onFoliageBrushPrototypeClear(3);
		} else
		if (node->getId().compare(foliageBrushPrototypeFileLoad[4]->getId()) == 0) {
			onFoliageBrushPrototypeLoad(4);
		} else
		if (node->getId().compare(foliageBrushPrototypeFileClear[4]->getId()) == 0) {
			onFoliageBrushPrototypeClear(4);
		} else
		if (node->getId().compare(btnFoliageBrushApply->getId()) == 0) {
			onApplyFoliageBrush();
		}
	}
}

void TerrainEditorScreenController::setTerrainDimension(float width, float height) {
	terrainDimensionWidth->getController()->setValue(MutableString(width));
	terrainDimensionDepth->getController()->setValue(MutableString(height));
}

void TerrainEditorScreenController::onLoadTerrain() {
	auto prototype = view->getPrototype();
	if (prototype == nullptr) return;

	// these ids get invalidated if loading a new terrain
	currentFoliageBrushIds.fill(-1);

	//
	try {
		auto width = prototype->getTerrain()->getWidth();
		auto depth = prototype->getTerrain()->getDepth();
		BoundingBox terrainBoundingBox;
		vector<Model*> terrainModels;
		Terrain::createTerrainModels(width, depth, 0.0f, prototype->getTerrain()->getHeightVector(), terrainBoundingBox, terrainModels);
		view->unsetWater();
		view->setTerrain(terrainBoundingBox, terrainModels);
		auto waterPositionMapsIndices = prototype->getTerrain()->getWaterPositionMapsIndices();
		for (auto waterPositionMapIdx: waterPositionMapsIndices) {
			vector<Model*> waterModels;
			Terrain::createWaterModels(
				terrainBoundingBox,
				prototype->getTerrain()->getWaterPositionMap(waterPositionMapIdx),
				prototype->getTerrain()->getWaterPositionMapHeight(waterPositionMapIdx),
				waterPositionMapIdx,
				waterModels
			);
			view->addWater(
				waterPositionMapIdx,
				waterModels,
				Terrain::computeWaterReflectionEnvironmentMappingPosition(
					prototype->getTerrain()->getWaterPositionMap(waterPositionMapIdx),
					prototype->getTerrain()->getWaterPositionMapHeight(waterPositionMapIdx)
				)
			);
		}
		view->addFoliage();
		Terrain::createFoliageMaps(terrainBoundingBox, newFoliageMaps);
		prototype->getTerrain()->setWidth(terrainBoundingBox.getDimensions().getX());
		prototype->getTerrain()->setDepth(terrainBoundingBox.getDimensions().getZ());
		btnTerrainDimensionSave->getController()->setDisabled(false);
		view->resetCamera();
	} catch (Exception& exception) {
		showErrorPopUp("Warning", (string(exception.what())));
	}
}

void TerrainEditorScreenController::onApplyTerrainDimension() {
	auto prototype = view->getPrototype();
	if (prototype == nullptr) return;

	// these ids get invalidated if loading a new terrain
	currentFoliageBrushIds.fill(-1);

	//
	try {
		auto width = Float::parseFloat(terrainDimensionWidth->getController()->getValue().getString());
		auto depth = Float::parseFloat(terrainDimensionDepth->getController()->getValue().getString());
		if (width < 1.0f || width > 4000.0f) throw ExceptionBase("Width must be within 1 .. 4000");
		if (depth < 1.0f || depth > 4000.0f) throw ExceptionBase("Depth must be within 1 .. 4000");
		prototype->getTerrain()->getHeightVector().clear();
		for (auto idx: prototype->getTerrain()->getWaterPositionMapsIndices()) prototype->getTerrain()->removeWaterPositionMap(idx);
		BoundingBox terrainBoundingBox;
		vector<Model*> terrainModels;
		Terrain::createTerrainModels(width, depth, 0.0f, prototype->getTerrain()->getHeightVector(), terrainBoundingBox, terrainModels);
		view->setTerrain(terrainBoundingBox, terrainModels);
		view->unsetWater();
		Terrain::createFoliageMaps(terrainBoundingBox, prototype->getTerrain()->getFoliageMaps());
		Terrain::createFoliageMaps(terrainBoundingBox, newFoliageMaps);
		prototype->getTerrain()->setWidth(terrainBoundingBox.getDimensions().getX());
		prototype->getTerrain()->setDepth(terrainBoundingBox.getDimensions().getZ());
		btnTerrainDimensionSave->getController()->setDisabled(false);
	} catch (Exception& exception) {
		showErrorPopUp("Warning", (string(exception.what())));
	}
}

void TerrainEditorScreenController::onTerrainLoad()
{
	class OnTerrainLoadAction: public virtual Action
	{
	public:
		void performAction() override {
			terrainEditorScreenController->view->loadTerrain(terrainEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getPathName(), terrainEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getFileName());
			terrainEditorScreenController->terrainPath.setPath(terrainEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getPathName());
			terrainEditorScreenController->view->getPopUps()->getFileDialogScreenController()->close();
		}

		/**
		 * Public constructor
		 * @param terrainEditorScreenController terrain editor screen controller
		 */
		OnTerrainLoadAction(TerrainEditorScreenController* terrainEditorScreenController): terrainEditorScreenController(terrainEditorScreenController) {
		}

	private:
		TerrainEditorScreenController* terrainEditorScreenController;
	};

	vector<string> extensions;
	extensions.push_back("tte");
	auto fileName = view->getPrototype() != nullptr?view->getPrototype()->getFileName():"";
	view->getPopUps()->getFileDialogScreenController()->show(
		terrainPath.getPath(),
		"Load from: ",
		extensions,
		fileName,
		true,
		new OnTerrainLoadAction(this)
	);
}

void TerrainEditorScreenController::onTerrainSave()
{
	class OnTerrainSave: public virtual Action
	{
	public:
		void performAction() override {
			try {
				terrainEditorScreenController->view->saveTerrain(
					terrainEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getPathName(),
					terrainEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getFileName()
				);
				terrainEditorScreenController->terrainPath.setPath(
						terrainEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getPathName()
				);
				terrainEditorScreenController->view->getPopUps()->getFileDialogScreenController()->close();
			} catch (Exception& exception) {
				terrainEditorScreenController->showErrorPopUp("Warning", (string(exception.what())));
			}
		}

		/**
		 * Public constructor
		 * @param modelEditorScreenController model editor screen controller
		 */
		OnTerrainSave(TerrainEditorScreenController* terrainEditorScreenController): terrainEditorScreenController(terrainEditorScreenController) {
		}

	private:
		TerrainEditorScreenController* terrainEditorScreenController;
	};

	auto fileName = view->getPrototype() != nullptr?view->getPrototype()->getFileName():"";
	vector<string> extensions = {
		"tte"
	};
	fileName = Tools::getFileName(fileName);
	view->getPopUps()->getFileDialogScreenController()->show(
		terrainPath.getPath(),
		"Save to: ",
		extensions,
		fileName,
		false,
		new OnTerrainSave(this)
	);
}

void TerrainEditorScreenController::onTerrainBrushFileLoad() {
	class OnTerrainBrushFileLoadAction: public virtual Action
	{
	public:
		void performAction() override {
			terrainEditorScreenController->terrainBrushFile->getController()->setValue(
				terrainEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getPathName() +
				"/" +
				terrainEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getFileName()
			);
			terrainEditorScreenController->brushTexturePath.setPath(
				terrainEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getPathName()
			);
			terrainEditorScreenController->view->getPopUps()->getFileDialogScreenController()->close();
		}

		/**
		 * Public constructor
		 * @param terrainEditorScreenController terrain editor screen controller
		 */
		OnTerrainBrushFileLoadAction(TerrainEditorScreenController* terrainEditorScreenController): terrainEditorScreenController(terrainEditorScreenController) {
		}

	private:
		TerrainEditorScreenController* terrainEditorScreenController;
	};

	vector<string> extensions = TextureReader::getTextureExtensions();
	view->getPopUps()->getFileDialogScreenController()->show(
		terrainBrushFile->getController()->getValue().getString().empty() == true?brushTexturePath.getPath():Tools::getPathName(terrainBrushFile->getController()->getValue().getString()),
		"Load from: ",
		extensions,
		Tools::getFileName(terrainBrushFile->getController()->getValue().getString()),
		true,
		new OnTerrainBrushFileLoadAction(this)
	);
}

void TerrainEditorScreenController::onTerrainBrushFileClear() {
	terrainBrushFile->getController()->setValue(MutableString());
}

void TerrainEditorScreenController::onApplyTerrainBrush() {
	view->unsetTerrainBrush();

	try {
		//
		currentFoliageBrushOperation = Terrain::BRUSHOPERATION_NONE;

		// texture
		if (currentTerrainBrushTexture != nullptr) currentTerrainBrushTexture->releaseReference();
		currentTerrainBrushTexture = nullptr;

		// operation
		map<string, MutableString> values;
		screenNode->getValues(values);
		auto brushOperationName = values["terrain_brush_operation"].getString();
		currentTerrainBrushOperation = Terrain::BRUSHOPERATION_NONE;
		if (brushOperationName == "add") {
			currentTerrainBrushOperation = Terrain::BRUSHOPERATION_ADD;
		} else
		if (brushOperationName == "subtract") {
			currentTerrainBrushOperation = Terrain::BRUSHOPERATION_SUBTRACT;
		} else
		if (brushOperationName == "flatten") {
			currentTerrainBrushOperation = Terrain::BRUSHOPERATION_FLATTEN;
		} else
		if (brushOperationName == "delete") {
			currentTerrainBrushOperation = Terrain::BRUSHOPERATION_DELETE;
		} else
		if (brushOperationName == "smooth") {
			currentTerrainBrushOperation = Terrain::BRUSHOPERATION_SMOOTH;
		} else
		if (brushOperationName == "water") {
			currentTerrainBrushOperation = Terrain::BRUSHOPERATION_WATER;
		}

		// scale, strength
		currentTerrainBrushScale = Float::parseFloat(terrainBrushScale->getController()->getValue().getString());
		currentTerrainBrushStrength = Float::parseFloat(terrainBrushStrength->getController()->getValue().getString());

		if (currentTerrainBrushScale < 0.1f || currentTerrainBrushScale > 100.0f) throw ExceptionBase("Brush scale must be within 0.1 .. 100");
		if (currentTerrainBrushStrength <= 0.0f || currentTerrainBrushStrength > 10.0f) throw ExceptionBase("Brush strength must be within 0 .. 10");

		// texture
		auto brushTextureFileName = terrainBrushFile->getController()->getValue().getString();
		currentTerrainBrushTexture = TextureReader::read(Tools::getPathName(brushTextureFileName), Tools::getFileName(brushTextureFileName), false, false);

		//
		view->setTerrainBrush(currentTerrainBrushTexture, currentTerrainBrushScale);
	} catch (Exception& exception) {
		Console::println(string("Terrain::onApplyBrush(): An error occurred: ") + exception.what());
		showErrorPopUp("Warning", (string(exception.what())));
	}
}

Terrain::BrushOperation TerrainEditorScreenController::getTerrainBrushOperation() {
	return currentTerrainBrushOperation;
}

Terrain::BrushOperation TerrainEditorScreenController::getFoliageBrushOperation() {
	return currentFoliageBrushOperation;
}

void TerrainEditorScreenController::applyTerrainBrush(BoundingBox& terrainBoundingBox, vector<Model*>& terrainModels, const Vector3& brushCenterPosition, int64_t deltaTime) {
	auto prototype = view->getPrototype();
	if (prototype == nullptr) return;
	if (terrainModels.empty() == true) return;

	// delete first
	Terrain::applyBrushToTerrainModels(
		terrainBoundingBox,
		terrainModels,
		prototype->getTerrain()->getHeightVector(),
		brushCenterPosition,
		currentTerrainBrushTexture,
		currentTerrainBrushScale,
		currentTerrainBrushStrength * static_cast<float>(deltaTime) / 200.0f, // if strength = 1.0f it will e.g. add to level 5 meters/second
		currentTerrainBrushOperation,
		currentTerrainBrushHeight
	);

	// and update
	Terrain::updateFoliageTerrainBrush(
		terrainBoundingBox,
		prototype->getTerrain()->getHeightVector(),
		brushCenterPosition,
		currentTerrainBrushTexture,
		currentTerrainBrushScale,
		prototype->getTerrain()->getFoliageMaps(),
		recreateFoliagePartitions
	);

	//
	view->updateTemporaryFoliage(recreateFoliagePartitions);
	recreateFoliagePartitions.clear();
}

void TerrainEditorScreenController::applyFoliageBrush(BoundingBox& terrainBoundingBox, const Vector3& brushCenterPosition, int64_t deltaTime) {
	auto prototype = view->getPrototype();
	if (prototype == nullptr) return;

	// check if having brush prototypes (ids)
	auto haveBrushPrototypeIds = false;
	for (auto brushBrushId: currentFoliageBrushIds) {
		if (brushBrushId != -1) haveBrushPrototypeIds = true;
	}
	if (haveBrushPrototypeIds == false) return;

	//
	switch(currentFoliageBrushOperation) {
		case Terrain::BRUSHOPERATION_ADD:
			//
			Terrain::applyFoliageDeleteBrush(
				terrainBoundingBox,
				brushCenterPosition,
				currentFoliageBrushTexture,
				currentFoliageBrushScale,
				currentFoliageBrushDensity,
				Terrain::BRUSHOPERATION_DELETE,
				prototype->getTerrain()->getFoliageMaps(),
				recreateFoliagePartitions
			);
			//
			Terrain::applyFoliageBrush(
				terrainBoundingBox,
				prototype->getTerrain()->getHeightVector(),
				brushCenterPosition,
				currentFoliageBrushTexture,
				currentFoliageBrushScale,
				currentFoliageBrushDensity,
				currentFoliageBrushIds,
				currentFoliageBrushCount,
				currentFoliageBrushPrototypeScale,
				currentFoliageBrushPrototypeRotations,
				currentFoliageBrushPrototypeSlope,
				currentFoliageBrushPrototypeHeight,
				currentFoliageBrushPrototypeNormalAlign,
				currentFoliageBrushOperation,
				prototype->getTerrain()->getFoliageMaps(),
				newFoliageMaps
			);
			break;
		case Terrain::BRUSHOPERATION_DELETE:
			//
			Terrain::applyFoliageDeleteBrush(
				terrainBoundingBox,
				brushCenterPosition,
				currentFoliageBrushTexture,
				currentFoliageBrushScale,
				currentFoliageBrushDensity,
				currentFoliageBrushOperation,
				prototype->getTerrain()->getFoliageMaps(),
				recreateFoliagePartitions
			);
			break;
	}

	//
	view->addTemporaryFoliage(newFoliageMaps);
	view->recreateTemporaryFoliage(recreateFoliagePartitions);

	//
	recreateFoliagePartitions.clear();
	Terrain::emptyFoliageMaps(newFoliageMaps);
}

void TerrainEditorScreenController::createWater(BoundingBox& terrainBoundingBox, const Vector3& brushCenterPosition, vector<Model*>& waterModels, Vector3& waterReflectionEnvironmentMappingPosition) {
	auto prototype = view->getPrototype();
	if (prototype == nullptr) return;
	auto waterPositionMapIdx = prototype->getTerrain()->allocateWaterPositionMapIdx();
	prototype->getTerrain()->setWaterPositionMapHeight(waterPositionMapIdx, currentTerrainBrushHeight);
	if (Terrain::computeWaterPositionMap(
		terrainBoundingBox,
		prototype->getTerrain()->getHeightVector(),
		brushCenterPosition,
		prototype->getTerrain()->getWaterPositionMapHeight(waterPositionMapIdx),
		prototype->getTerrain()->getWaterPositionMap(waterPositionMapIdx)) == true) {
		//
		Terrain::createWaterModels(
			terrainBoundingBox,
			prototype->getTerrain()->getWaterPositionMap(waterPositionMapIdx),
			prototype->getTerrain()->getWaterPositionMapHeight(waterPositionMapIdx),
			waterPositionMapIdx,
			waterModels
		);
		waterReflectionEnvironmentMappingPosition = Terrain::computeWaterReflectionEnvironmentMappingPosition(
			prototype->getTerrain()->getWaterPositionMap(waterPositionMapIdx),
			prototype->getTerrain()->getWaterPositionMapHeight(waterPositionMapIdx)
		);
		view->addWater(
			waterPositionMapIdx,
			waterModels,
			waterReflectionEnvironmentMappingPosition
		);
	}
}

void TerrainEditorScreenController::deleteWater(int waterPositionMapIdx) {
	auto prototype = view->getPrototype();
	if (prototype == nullptr) return;
	prototype->getTerrain()->removeWaterPositionMap(waterPositionMapIdx);
	view->removeWater(waterPositionMapIdx);
}

bool TerrainEditorScreenController::determineCurrentBrushHeight(BoundingBox& terrainBoundingBox, vector<Model*> terrainModels, const Vector3& brushCenterPosition) {
	auto prototype = view->getPrototype();
	if (prototype == nullptr) return false;
	if (currentTerrainBrushOperation != Terrain::BRUSHOPERATION_FLATTEN && currentTerrainBrushOperation != Terrain::BRUSHOPERATION_WATER) return true;
	if (haveCurrentTerrainBrushHeight == true) return true;
	if (terrainModels.empty() == true) return false;
	auto terrainModel = terrainModels[0];
	if (terrainModel == nullptr) return false;
	haveCurrentTerrainBrushHeight = Terrain::getTerrainModelsHeight(
		terrainBoundingBox,
		terrainModels,
		prototype->getTerrain()->getHeightVector(),
		brushCenterPosition,
		currentTerrainBrushHeight
	);
	return haveCurrentTerrainBrushHeight;
}

void TerrainEditorScreenController::unsetCurrentBrushFlattenHeight() {
	haveCurrentTerrainBrushHeight = false;
}

void TerrainEditorScreenController::onFoliageBrushFileLoad() {
	class OnFoliageBrushFileLoadAction: public virtual Action
	{
	public:
		void performAction() override {
			terrainEditorScreenController->foliageBrushFile->getController()->setValue(
				terrainEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getPathName() +
				"/" +
				terrainEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getFileName()
			);
			terrainEditorScreenController->brushTexturePath.setPath(
				terrainEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getPathName()
			);
			terrainEditorScreenController->view->getPopUps()->getFileDialogScreenController()->close();
		}

		/**
		 * Public constructor
		 * @param terrainEditorScreenController terrain editor screen controller
		 */
		OnFoliageBrushFileLoadAction(TerrainEditorScreenController* terrainEditorScreenController): terrainEditorScreenController(terrainEditorScreenController) {
		}

	private:
		TerrainEditorScreenController* terrainEditorScreenController;
	};

	vector<string> extensions = TextureReader::getTextureExtensions();
	view->getPopUps()->getFileDialogScreenController()->show(
		foliageBrushFile->getController()->getValue().getString().empty() == true?brushTexturePath.getPath():Tools::getPathName(foliageBrushFile->getController()->getValue().getString()),
		"Load from: ",
		extensions,
		Tools::getFileName(foliageBrushFile->getController()->getValue().getString()),
		true,
		new OnFoliageBrushFileLoadAction(this)
	);
}

void TerrainEditorScreenController::onFoliageBrushFileClear() {
	foliageBrushFile->getController()->setValue(MutableString());
}

void TerrainEditorScreenController::onFoliageBrushPrototypeLoad(int idx) {
	class OnFoliageBrushPrototypeLoadAction: public virtual Action
	{
	public:
		void performAction() override {
			terrainEditorScreenController->foliageBrushPrototypeFile[idx]->getController()->setValue(
				terrainEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getPathName() +
				"/" +
				terrainEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getFileName()
			);
			terrainEditorScreenController->prototypePath.setPath(
				terrainEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getPathName()
			);
			terrainEditorScreenController->view->getPopUps()->getFileDialogScreenController()->close();
		}

		/**
		 * Public constructor
		 * @param terrainEditorScreenController terrain editor screen controller
		 * @param idx index
		 */
		OnFoliageBrushPrototypeLoadAction(TerrainEditorScreenController* terrainEditorScreenController, int idx): terrainEditorScreenController(terrainEditorScreenController), idx(idx) {
		}

	private:
		TerrainEditorScreenController* terrainEditorScreenController;
		int idx;
	};

	vector<string> extensions = { "tmm" };
	view->getPopUps()->getFileDialogScreenController()->show(
		foliageBrushPrototypeFile[idx]->getController()->getValue().getString().empty() == true?prototypePath.getPath():Tools::getPathName(foliageBrushPrototypeFile[idx]->getController()->getValue().getString()),
		"Load from: ",
		extensions,
		Tools::getFileName(foliageBrushPrototypeFile[idx]->getController()->getValue().getString()),
		true,
		new OnFoliageBrushPrototypeLoadAction(this, idx)
	);
}

void TerrainEditorScreenController::onApplyFoliageBrush() {
	auto prototype = view->getPrototype();
	if (prototype == nullptr) return;

	//
	view->unsetTerrainBrush();

	//
	try {
		//
		currentTerrainBrushOperation = Terrain::BRUSHOPERATION_NONE;

		// texture
		if (currentFoliageBrushTexture != nullptr) currentFoliageBrushTexture->releaseReference();
		currentFoliageBrushTexture = nullptr;

		// operation
		map<string, MutableString> values;
		screenNode->getValues(values);
		auto brushOperationName = values["foliage_brush_operation"].getString();
		currentFoliageBrushOperation = Terrain::BRUSHOPERATION_NONE;
		if (brushOperationName == "add") {
			currentFoliageBrushOperation = Terrain::BRUSHOPERATION_ADD;
		} else
		if (brushOperationName == "subtract") {
			currentFoliageBrushOperation = Terrain::BRUSHOPERATION_SUBTRACT;
		} else
		if (brushOperationName == "flatten") {
			currentFoliageBrushOperation = Terrain::BRUSHOPERATION_FLATTEN;
		} else
		if (brushOperationName == "delete") {
			currentFoliageBrushOperation = Terrain::BRUSHOPERATION_DELETE;
		}

		// scale, strength
		currentFoliageBrushScale = Float::parseFloat(foliageBrushScale->getController()->getValue().getString());
		currentFoliageBrushDensity = Float::parseFloat(foliageBrushDensity->getController()->getValue().getString());

		if (currentFoliageBrushScale < 0.1f || currentFoliageBrushScale > 100.0f) throw ExceptionBase("Brush scale must be within 0.1 .. 100");
		if (currentFoliageBrushDensity <= 0.0f || currentFoliageBrushDensity> 10.0f) throw ExceptionBase("Brush strength must be within 0 .. 10");

		// texture
		auto brushTextureFileName = foliageBrushFile->getController()->getValue().getString();
		currentFoliageBrushTexture = TextureReader::read(Tools::getPathName(brushTextureFileName), Tools::getFileName(brushTextureFileName), false, false);

		// prototypes
		array<Prototype*, 5> currentFoliageBrushPrototypes { nullptr, nullptr, nullptr, nullptr, nullptr };
		for (auto i = 0; i < currentFoliageBrushPrototypes.size(); i++) {
			auto foliagePrototypeFileName = foliageBrushPrototypeFile[i]->getController()->getValue().getString();
			if (foliagePrototypeFileName.empty() == false) {
				currentFoliageBrushPrototypes[i] = PrototypeReader::read(
					PrototypeReader::getResourcePathName(Tools::getPathName(foliagePrototypeFileName), foliagePrototypeFileName),
					Tools::getFileName(foliagePrototypeFileName)
				);
			}
			currentFoliageBrushIds[i] = foliagePrototypeFileName.empty() == true?-1:prototype->getTerrain()->getFoliagePrototypeIndex(currentFoliageBrushPrototypes[i]);
			currentFoliageBrushCount[i] = Float::parseFloat(foliageBrushPrototypeFileCount[i]->getController()->getValue().getString());
			currentFoliageBrushPrototypeScale[i][0] = Float::parseFloat(foliageBrushPrototypeScaleMin[i]->getController()->getValue().getString());
			currentFoliageBrushPrototypeScale[i][1] = Float::parseFloat(foliageBrushPrototypeScaleMax[i]->getController()->getValue().getString());
			currentFoliageBrushPrototypeRotations[i][0] = Float::parseFloat(foliageBrushPrototypeRangeXMin[i]->getController()->getValue().getString());
			currentFoliageBrushPrototypeRotations[i][1] = Float::parseFloat(foliageBrushPrototypeRangeXMax[i]->getController()->getValue().getString());
			currentFoliageBrushPrototypeRotations[i][2] = Float::parseFloat(foliageBrushPrototypeRangeYMin[i]->getController()->getValue().getString());
			currentFoliageBrushPrototypeRotations[i][3] = Float::parseFloat(foliageBrushPrototypeRangeYMax[i]->getController()->getValue().getString());
			currentFoliageBrushPrototypeRotations[i][4] = Float::parseFloat(foliageBrushPrototypeRangeZMin[i]->getController()->getValue().getString());
			currentFoliageBrushPrototypeRotations[i][5] = Float::parseFloat(foliageBrushPrototypeRangeZMax[i]->getController()->getValue().getString());
			currentFoliageBrushPrototypeSlope[i][0] = Float::parseFloat(foliageBrushPrototypeSlopeMin[i]->getController()->getValue().getString());
			currentFoliageBrushPrototypeSlope[i][1] = Float::parseFloat(foliageBrushPrototypeSlopeMax[i]->getController()->getValue().getString());
			currentFoliageBrushPrototypeHeight[i][0] = Float::parseFloat(foliageBrushPrototypeHeightMin[i]->getController()->getValue().getString());
			currentFoliageBrushPrototypeHeight[i][1] = Float::parseFloat(foliageBrushPrototypeHeightMax[i]->getController()->getValue().getString());
			currentFoliageBrushPrototypeNormalAlign[i] = foliageBrushPrototypeNormalAlign[i]->getController()->getValue().getString() == "1";
		}

		//
		view->setTerrainBrush(currentFoliageBrushTexture, currentFoliageBrushScale);
	} catch (Exception& exception) {
		Console::println(string("Terrain::onApplyBrush(): An error occurred: ") + exception.what());
		showErrorPopUp("Warning", (string(exception.what())));
	}
}

void TerrainEditorScreenController::onFoliageBrushPrototypeClear(int idx) {
	foliageBrushPrototypeFile[idx]->getController()->setValue(MutableString());
}

void TerrainEditorScreenController::getViewPort(int& left, int& top, int& width, int& height) {
	auto& constraints = viewPort->getComputedConstraints();
	left = constraints.left + constraints.alignmentLeft + constraints.contentAlignmentLeft;
	top = constraints.top + constraints.alignmentTop + constraints.contentAlignmentTop;
	width = constraints.width;
	height = constraints.height;
}
