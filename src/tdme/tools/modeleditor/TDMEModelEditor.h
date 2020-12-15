
#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/application/Application.h>
#include <tdme/tools/shared/views/fwd-tdme.h>
#include <tdme/tools/shared/views/SharedModelEditorView.h>
#include <tdme/tools/modeleditor/fwd-tdme.h>

using std::string;

using tdme::application::Application;
using tdme::engine::Engine;
using tdme::tools::shared::views::PopUps;
using tdme::tools::shared::views::View;
using tdme::tools::shared::views::SharedModelEditorView;

/**
 * TDME model editor
 * @author andreas.drewke
 * @version $Id$
 */
class tdme::tools::viewer::TDMEModelEditor final
	: public Application
{

private:
	static string VERSION;
	static TDMEModelEditor* instance;
	Engine* engine { nullptr };
	View* view { nullptr };
	bool viewInitialized;
	View* viewNew { nullptr };
	bool quitRequested;
	PopUps* popUps { nullptr };
	SharedModelEditorView* modelEditorView { nullptr };

public:
	/**
	 * @param argc argument count
	 * @param argv argument values
	 */
	static void main(int argc, char** argv);

public:

	/**
	 * @return level editor instance
	 */
	static TDMEModelEditor* getInstance();

	/**
	 * Public constructor
	 */
	TDMEModelEditor();

	/**
	 * Destructor
	 */
	~TDMEModelEditor();

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
	 * Reshape tdme level editor
	 * @param width width
	 * @param height height
	 */
	void reshape(int width, int height);

};
