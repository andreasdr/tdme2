#include <tdme/tools/sceneeditor/views/TerrainEditorView.h>

#include <string>

#include <tdme/engine/fileio/prototypes/PrototypeReader.h>
#include <tdme/engine/prototype/Prototype.h>
#include <tdme/engine/scene/Scene.h>
#include <tdme/engine/scene/SceneLibrary.h>
#include <tdme/engine/Engine.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/gui/GUI.h>
#include <tdme/tools/sceneeditor/controller/SceneEditorLibraryScreenController.h>
#include <tdme/tools/sceneeditor/TDMESceneEditor.h>
#include <tdme/utilities/StringTools.h>

using std::string;

using tdme::engine::fileio::prototypes::PrototypeReader;
using tdme::engine::prototype::Prototype;
using tdme::engine::scene::Scene;
using tdme::engine::scene::SceneLibrary;
using tdme::engine::Engine;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::GUI;
using tdme::tools::sceneeditor::controller::SceneEditorLibraryScreenController;
using tdme::tools::sceneeditor::views::TerrainEditorView;
using tdme::tools::sceneeditor::TDMESceneEditor;
using tdme::utilities::StringTools;

TerrainEditorView::TerrainEditorView(PopUps* popUps)
	: SharedTerrainEditorView(popUps)
{
}

void TerrainEditorView::onSetPrototypeData()
{
	TDMESceneEditor::getInstance()->getSceneEditorLibraryScreenController()->setPrototypeLibrary();
}

void TerrainEditorView::onLoadTerrain(Prototype* oldEntity, Prototype* entity)
{
	TDMESceneEditor::getInstance()->getScene()->replacePrototype(oldEntity->getId(), entity->getId());
	TDMESceneEditor::getInstance()->getSceneLibrary()->removePrototype(oldEntity->getId());
	TDMESceneEditor::getInstance()->getSceneEditorLibraryScreenController()->setPrototypeLibrary();
}

void TerrainEditorView::onInitAdditionalScreens()
{
	engine->getGUI()->addRenderScreen(TDMESceneEditor::getInstance()->getSceneEditorLibraryScreenController()->getScreenNode()->getId());
}

Prototype* TerrainEditorView::loadTerrainPrototype(const string& pathName, const string& fileName) {
	if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".tte") == true) {
		auto prototype = PrototypeReader::read(SceneLibrary::ID_ALLOCATE, pathName, fileName);
		TDMESceneEditor::getInstance()->getSceneLibrary()->addPrototype(prototype);
		return prototype;
	}
	return nullptr;
}
