#include <tdme/tools/sceneeditor/views/ModelEditorView.h>

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
using tdme::tools::sceneeditor::views::ModelEditorView;
using tdme::tools::sceneeditor::TDMESceneEditor;

ModelEditorView::ModelEditorView(PopUps* popUps)
	: SharedModelEditorView(popUps)
{
}

void ModelEditorView::onSetPrototypeData()
{
	TDMESceneEditor::getInstance()->getSceneEditorLibraryScreenController()->setPrototypeLibrary();
}

void ModelEditorView::onLoadModel(Prototype* oldEntity, Prototype* entity)
{
	TDMESceneEditor::getInstance()->getScene()->replacePrototype(oldEntity->getId(), entity->getId());
	TDMESceneEditor::getInstance()->getSceneLibrary()->removePrototype(oldEntity->getId());
	TDMESceneEditor::getInstance()->getSceneEditorLibraryScreenController()->setPrototypeLibrary();
}

void ModelEditorView::onInitAdditionalScreens()
{
	engine->getGUI()->addRenderScreen(TDMESceneEditor::getInstance()->getSceneEditorLibraryScreenController()->getScreenNode()->getId());
}

Prototype* ModelEditorView::loadModel(const string& name, const string& description, const string& pathName, const string& fileName, const Vector3& pivot) /* throws(Exception) */
{
	return TDMESceneEditor::getInstance()->getSceneLibrary()->addModel(
		SceneLibrary::ID_ALLOCATE,
		name,
		description,
		pathName,
		fileName,
		pivot
	);
}
