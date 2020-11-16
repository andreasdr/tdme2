
#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/application/Application.h>
#include <tdme/tools/leveleditor/fwd-tdme.h>
#include <tdme/tools/leveleditor/controller/fwd-tdme.h>
#include <tdme/tools/leveleditor/views/fwd-tdme.h>
#include <tdme/tools/shared/model/fwd-tdme.h>
#include <tdme/tools/shared/model/LevelEditorLevel.h>
#include <tdme/tools/shared/views/fwd-tdme.h>

using std::string;

using tdme::application::Application;
using tdme::engine::Engine;
using tdme::tools::leveleditor::controller::LevelEditorEntityLibraryScreenController;
using tdme::tools::leveleditor::views::EmptyView;
using tdme::tools::leveleditor::views::LevelEditorView;
using tdme::tools::leveleditor::views::ModelEditorView;
using tdme::tools::leveleditor::views::ParticleSystemView;
using tdme::tools::leveleditor::views::TriggerView;
using tdme::tools::shared::model::LevelEditorEntityLibrary;
using tdme::tools::shared::model::LevelEditorLevel;
using tdme::tools::shared::views::PopUps;
using tdme::tools::shared::views::View;

/**
 * TDME Level Editor
 * @author andreas.drewke
 * @version $Id$
 */
class tdme::tools::leveleditor::TDMELevelEditor final
	: public virtual Application
{
private:
	static string VERSION;
	static TDMELevelEditor* instance;
	Engine* engine { nullptr };
	View* view { nullptr };
	bool quitRequested;
	LevelEditorLevel level;
	LevelEditorEntityLibraryScreenController* levelEditorEntityLibraryScreenController { nullptr };
	PopUps* popUps { nullptr };
	LevelEditorView* levelEditorView { nullptr };
	ModelEditorView* modelEditorView { nullptr };
	TriggerView* triggerView { nullptr };
	EmptyView* emptyView { nullptr };
	ParticleSystemView* particleSystemView { nullptr };

public:

	/**
	 * Main
	 * @param argc argument count
	 * @param argv argument values
	 */
	static void main(int argc, char** argv);

public:
	/**
	 * @return level editor instance
	 */
	static TDMELevelEditor* getInstance();

	/**
	 * Public constructor
	 */
	TDMELevelEditor();

	/**
	 * Destructor
	 */
	~TDMELevelEditor();

	/**
	 * @return level editor entity library screen controller
	 */
	LevelEditorEntityLibraryScreenController* getLevelEditorEntityLibraryScreenController();

	/**
	 * @return entity library
	 */
	LevelEditorEntityLibrary* getEntityLibrary();

	/**
	 * @return level
	 */
	LevelEditorLevel* getLevel();

	/**
	 * Set up new view
	 * @param view view
	 */
	void setView(View* view);

	/**
	 * @return current view
	 */
	View* getView();

	/**
	 * Request to exit the viewer
	 */
	void quit();

	/**
	 * Renders the scene
	 */
	void display();

	/**
	 * Shutdown tdme viewer
	 */
	void dispose();

	/**
	 * Initialize tdme level editor
	 */
	void initialize();

	/**
	 * reshape tdme level editor
	 */
	void reshape(int32_t width, int32_t height);

	/**
	 * Switch to level editor
	 */
	void switchToLevelEditor();

	/**
	 * Switch to model editor
	 */
	void switchToModelEditor();

	/**
	 * Switch to trigger view
	 */
	void switchToTriggerView();

	/**
	 * Switch to empty view
	 */
	void switchToEmptyView();

	/**
	 * Switch to particle system view
	 */
	void switchToParticleSystemView();

};
