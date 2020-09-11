#include <tdme/tools/modeleditor/TDMEModelEditor.h>

#include <cstdlib>
#include <string>

#include <tdme/utilities/Time.h>

#include <tdme/engine/Engine.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/gui/GUI.h>
#include <tdme/tools/shared/tools/Tools.h>
#include <tdme/tools/shared/views/PopUps.h>
#include <tdme/tools/shared/views/SharedModelEditorView.h>
#include <tdme/tools/shared/views/View.h>
#include <tdme/utilities/Console.h>

using std::string;

using tdme::tools::viewer::TDMEModelEditor;

using tdme::utilities::Time;

using tdme::engine::Engine;
using tdme::engine::model::Color4;
using tdme::gui::GUI;
using tdme::tools::shared::tools::Tools;
using tdme::tools::shared::views::PopUps;
using tdme::tools::shared::views::SharedModelEditorView;
using tdme::tools::shared::views::View;
using tdme::utilities::Console;

string TDMEModelEditor::VERSION = "1.9.9";

TDMEModelEditor* TDMEModelEditor::instance = nullptr;

TDMEModelEditor::TDMEModelEditor()
{
	Tools::loadSettings(this);
	TDMEModelEditor::instance = this;
	engine = Engine::getInstance();
	view = nullptr;
	viewInitialized = false;
	viewNew = nullptr;
	popUps = new PopUps();
	modelEditorView = nullptr;
	quitRequested = false;
}

TDMEModelEditor::~TDMEModelEditor() {
	delete modelEditorView;
	delete popUps;
}

void TDMEModelEditor::main(int argc, char** argv)
{
	Console::println(string("TDMEModelEditor "+ VERSION));
	Console::println(string("Programmed 2014,...,2018 by Andreas Drewke, drewke.net."));
	Console::println();

	auto tdmeLevelEditor = new TDMEModelEditor();
	tdmeLevelEditor->run(argc, argv, "TDMEModelEditor");
}

TDMEModelEditor* TDMEModelEditor::getInstance()
{
	return instance;
}

void TDMEModelEditor::setView(View* view)
{
	viewNew = view;
}

View* TDMEModelEditor::getView()
{
	return view;
}

void TDMEModelEditor::quit()
{
	quitRequested = true;
}

void TDMEModelEditor::display()
{
	if (viewNew != nullptr) {
		if (view != nullptr && viewInitialized == true) {
			view->deactivate();
			view->dispose();
			viewInitialized = false;
		}
		view = viewNew;
		viewNew = nullptr;
	}
	if (view != nullptr) {
		if (viewInitialized == false) {
			view->initialize();
			view->activate();
			viewInitialized = true;
		}
		view->display();
	}
	engine->display();
	if (view != nullptr) view->display();
	if (quitRequested == true) {
		if (view != nullptr) {
			view->deactivate();
			view->dispose();
		}
		Application::exit(0);
	}
}

void TDMEModelEditor::dispose()
{
	if (view != nullptr && viewInitialized == true) {
		view->deactivate();
		view->dispose();
		view = nullptr;
	}
	engine->dispose();
	Tools::oseDispose();
}

void TDMEModelEditor::initialize()
{
	Console::println("initialize");
	engine->initialize();
	engine->setSceneColor(Color4(125.0f / 255.0f, 125.0f / 255.0f, 125.0f / 255.0f, 1.0f));
	setInputEventHandler(engine->getGUI());
	Tools::oseInit();
	popUps->initialize();
	setView(modelEditorView = new SharedModelEditorView(popUps));
}

void TDMEModelEditor::reshape(int32_t width, int32_t height)
{
	engine->reshape(width, height);
}
