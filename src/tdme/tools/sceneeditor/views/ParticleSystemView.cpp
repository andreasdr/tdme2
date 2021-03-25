#include <tdme/tools/sceneeditor/views/ParticleSystemView.h>

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
#include <tdme/tools/shared/views/fwd-tdme.h>
#include <tdme/tools/shared/views/SharedParticleSystemView.h>
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
using tdme::tools::sceneeditor::views::ParticleSystemView;
using tdme::tools::sceneeditor::TDMESceneEditor;
using tdme::utilities::StringTools;;

ParticleSystemView::ParticleSystemView(PopUps* popUps)
	: SharedParticleSystemView(popUps)
{
}

void ParticleSystemView::onSetEntityData()
{
	TDMESceneEditor::getInstance()->getSceneEditorLibraryScreenController()->setPrototypeLibrary();
}

void ParticleSystemView::onLoadParticleSystem(Prototype* oldEntity, Prototype* newEntity)
{
	TDMESceneEditor::getInstance()->getScene()->replacePrototype(oldEntity->getId(), newEntity->getId());
	TDMESceneEditor::getInstance()->getSceneLibrary()->removePrototype(oldEntity->getId());
	TDMESceneEditor::getInstance()->getSceneEditorLibraryScreenController()->setPrototypeLibrary();
}

Prototype* ParticleSystemView::loadParticleSystemPrototype(const string& name, const string& description, const string& pathName, const string& fileName)
{
	if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".tps") == true) {
		auto prototype = PrototypeReader::read(SceneLibrary::ID_ALLOCATE, pathName, fileName);
		prototype->setDefaultBoundingVolumes();
		TDMESceneEditor::getInstance()->getSceneLibrary()->addPrototype(prototype);
		return prototype;
	}
	return nullptr;
}

void ParticleSystemView::onInitAdditionalScreens()
{
	engine->getGUI()->addRenderScreen(TDMESceneEditor::getInstance()->getSceneEditorLibraryScreenController()->getScreenNode()->getId());
}
