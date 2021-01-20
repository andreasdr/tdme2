
#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/application/Application.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/tools/shared/views/fwd-tdme.h>
#include <tdme/tools/shared/views/SharedTerrainEditorView.h>
#include <tdme/tools/terraineditor/fwd-tdme.h>

using std::string;

using tdme::application::Application;
using tdme::engine::Engine;
using tdme::tools::shared::views::PopUps;
using tdme::tools::shared::views::SharedTerrainEditorView;
using tdme::tools::shared::views::View;

/**
 * TDME terrain editor
 * @author andreas.drewke
 * @version $Id$
 */
class tdme::tools::terraineditor::TDMETerrainEditor final
	: public virtual Application
{
private:
	static TDMETerrainEditor* instance;
	Engine* engine { nullptr };
	View* view { nullptr };
	bool viewInitialized;
	View* viewNew { nullptr };
	bool quitRequested;
	PopUps* popUps { nullptr };
	SharedTerrainEditorView* terrainEditorView { nullptr };

public:

	/**
	 * Main
	 * @param argc argument count
	 * @param argv argument values
	 */
	static void main(int argc, char** argv);

public:

	/**
	 * @return particle system instance
	 */
	static TDMETerrainEditor* getInstance();

	/**
	 * Public constructor
	 */
	TDMETerrainEditor();

	/**
	 * Destructor
	 */
	~TDMETerrainEditor();

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
	 * Initialize tdme scene editor
	 */
	void initialize();

	/**
	 * Reshape tdme scene editor
	 * @param width width
	 * @param height height
	 */
	void reshape(int width, int height);

};
