#include <tdme/tools/terraineditor/TDMETerrainEditor.h>

#include <cstdlib>
#include <string>

#include <tdme/utilities/Time.h>

#include <tdme/engine/model/Color4.h>
#include <tdme/engine/prototype/Prototype.h>
#include <tdme/engine/prototype/Prototype_Type.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Version.h>
#include <tdme/gui/GUI.h>
#include <tdme/tools/shared/tools/Tools.h>
#include <tdme/tools/shared/views/PopUps.h>
#include <tdme/tools/shared/views/SharedTerrainEditorView.h>
#include <tdme/tools/shared/views/View.h>
#include <tdme/utilities/Console.h>

using std::string;

using tdme::tools::terraineditor::TDMETerrainEditor;

using tdme::utilities::Time;

using tdme::engine::model::Color4;
using tdme::engine::prototype::Prototype;
using tdme::engine::prototype::Prototype_Type;
using tdme::engine::Engine;
using tdme::engine::Version;
using tdme::gui::GUI;
using tdme::tools::shared::tools::Tools;
using tdme::tools::shared::views::PopUps;
using tdme::tools::shared::views::SharedTerrainEditorView;
using tdme::tools::shared::views::View;
using tdme::utilities::Console;

TDMETerrainEditor* TDMETerrainEditor::instance = nullptr;

TDMETerrainEditor::TDMETerrainEditor()
{
	Tools::loadSettings(this);
	TDMETerrainEditor::instance = this;
	engine = Engine::getInstance();
	view = nullptr;
	viewInitialized = false;
	viewNew = nullptr;
	popUps = new PopUps();
	terrainEditorView = nullptr;
	quitRequested = false;
}

TDMETerrainEditor::~TDMETerrainEditor() {
	delete popUps;
	delete terrainEditorView;
}

void TDMETerrainEditor::main(int argc, char** argv)
{
	Console::println(string("TDMETerrainEditor ") + Version::getVersion());
	Console::println(Version::getCopyright());
	Console::println();

	auto tdmeTerrainEditor = new TDMETerrainEditor();
	tdmeTerrainEditor->run(argc, argv, "TDMETerrainEditor");
}

TDMETerrainEditor* TDMETerrainEditor::getInstance()
{
	return instance;
}

void TDMETerrainEditor::setView(View* view)
{
	viewNew = view;
}

View* TDMETerrainEditor::getView()
{
	return view;
}

void TDMETerrainEditor::quit()
{
	quitRequested = true;
}

void TDMETerrainEditor::display()
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
	engine->getGUI()->handleEvents();
	engine->getGUI()->render();
	if (quitRequested == true) {
		if (view != nullptr) {
			view->deactivate();
			view->dispose();
		}
		Application::exit(0);
	}
}

void TDMETerrainEditor::dispose()
{
	if (view != nullptr && viewInitialized == true) {
		view->deactivate();
		view->dispose();
		delete terrainEditorView->getPrototype();
		view = nullptr;
	}
	engine->dispose();
	Tools::oseDispose();
}

void TDMETerrainEditor::initialize()
{
	engine->initialize();
	engine->setSceneColor(Color4(125.0f / 255.0f, 125.0f / 255.0f, 125.0f / 255.0f, 1.0f));
	setInputEventHandler(engine->getGUI());
	Tools::oseInit();
	popUps->initialize();
	setView(terrainEditorView = new SharedTerrainEditorView(popUps));
	terrainEditorView->setPrototype(
		new Prototype(
			-1,
			Prototype_Type::TERRAIN,
			"Untitled",
			"",
			"Untitled.tterrain",
			"",
			"",
			nullptr,
			Vector3()
		)
	);
}

void TDMETerrainEditor::reshape(int width, int height)
{
	engine->reshape(width, height);
}
