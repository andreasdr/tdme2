#include <tdme/tools/sceneeditor/views/TerrainEditorView.h>

#include <string>

#include <tdme/engine/prototype/Prototype.h>
#include <tdme/engine/scene/Scene.h>
#include <tdme/engine/scene/SceneLibrary.h>
#include <tdme/engine/Engine.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/gui/GUI.h>
#include <tdme/tools/sceneeditor/controller/SceneEditorLibraryScreenController.h>
#include <tdme/tools/sceneeditor/TDMESceneEditor.h>

using std::string;

using tdme::engine::prototype::Prototype;
using tdme::engine::scene::Scene;
using tdme::engine::scene::SceneLibrary;
using tdme::engine::Engine;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::GUI;
using tdme::tools::sceneeditor::controller::SceneEditorLibraryScreenController;
using tdme::tools::sceneeditor::views::TerrainEditorView;
using tdme::tools::sceneeditor::TDMESceneEditor;

TerrainEditorView::TerrainEditorView(PopUps* popUps)
	: SharedTerrainEditorView(popUps)
{
}

void TerrainEditorView::onSetPrototypeData()
{
	TDMESceneEditor::getInstance()->getSceneEditorLibraryScreenController()->setPrototypeLibrary();
}

void TerrainEditorView::onInitAdditionalScreens()
{
	engine->getGUI()->addRenderScreen(TDMESceneEditor::getInstance()->getSceneEditorLibraryScreenController()->getScreenNode()->getId());
}
