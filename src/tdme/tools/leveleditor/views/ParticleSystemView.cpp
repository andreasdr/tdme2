#include <tdme/tools/leveleditor/views/ParticleSystemView.h>

#include <string>

#include <tdme/engine/Engine.h>
#include <tdme/gui/GUI.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/tools/leveleditor/TDMELevelEditor.h>
#include <tdme/tools/leveleditor/controller/LevelEditorEntityLibraryScreenController.h>
#include <tdme/tools/shared/files/ModelMetaDataFileImport.h>
#include <tdme/tools/shared/model/LevelEditorEntity.h>
#include <tdme/tools/shared/model/LevelEditorEntityLibrary.h>
#include <tdme/tools/shared/model/LevelEditorLevel.h>
#include <tdme/tools/shared/views/fwd-tdme.h>
#include <tdme/tools/shared/views/SharedParticleSystemView.h>
#include <tdme/utilities/StringTools.h>

using std::string;

using tdme::tools::leveleditor::views::ParticleSystemView;
using tdme::engine::Engine;
using tdme::gui::GUI;
using tdme::gui::nodes::GUIScreenNode;
using tdme::tools::leveleditor::TDMELevelEditor;
using tdme::tools::leveleditor::controller::LevelEditorEntityLibraryScreenController;
using tdme::tools::shared::files::ModelMetaDataFileImport;
using tdme::tools::shared::model::LevelEditorEntity;
using tdme::tools::shared::model::LevelEditorEntityLibrary;
using tdme::tools::shared::model::LevelEditorLevel;
using tdme::utilities::StringTools;;

ParticleSystemView::ParticleSystemView(PopUps* popUps) 
	: SharedParticleSystemView(popUps)
{
}

void ParticleSystemView::onSetEntityData()
{
	TDMELevelEditor::getInstance()->getLevelEditorEntityLibraryScreenController()->setEntityLibrary();
}

void ParticleSystemView::onLoadParticleSystem(LevelEditorEntity* oldEntity, LevelEditorEntity* newEntity)
{
	TDMELevelEditor::getInstance()->getLevel()->replaceEntity(oldEntity->getId(), newEntity->getId());
	TDMELevelEditor::getInstance()->getEntityLibrary()->removeEntity(oldEntity->getId());
	TDMELevelEditor::getInstance()->getLevelEditorEntityLibraryScreenController()->setEntityLibrary();
}

LevelEditorEntity* ParticleSystemView::loadParticleSystem(const string& name, const string& description, const string& pathName, const string& fileName) /* throws(Exception) */
{
	if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".tps") == true) {
		auto levelEditorEntity = ModelMetaDataFileImport::doImport(LevelEditorEntityLibrary::ID_ALLOCATE, pathName, fileName);
		levelEditorEntity->setDefaultBoundingVolumes();
		TDMELevelEditor::getInstance()->getEntityLibrary()->addEntity(levelEditorEntity);
		return levelEditorEntity;
	}
	return nullptr;
}

void ParticleSystemView::onInitAdditionalScreens()
{
	engine->getGUI()->addRenderScreen(TDMELevelEditor::getInstance()->getLevelEditorEntityLibraryScreenController()->getScreenNode()->getId());
}
