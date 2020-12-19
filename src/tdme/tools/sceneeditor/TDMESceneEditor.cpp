#include <tdme/tools/sceneeditor/TDMESceneEditor.h>

#include <cstdlib>
#include <string>

#include <tdme/utilities/Time.h>

#include <tdme/engine/model/Color4.h>
#include <tdme/engine/scene/Scene.h>
#include <tdme/engine/scene/SceneLibrary.h>
#include <tdme/engine/scene/ScenePropertyPresets.h>
#include <tdme/engine/Engine.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/gui/GUI.h>
#include <tdme/tools/sceneeditor/controller/SceneEditorLibraryScreenController.h>
#include <tdme/tools/sceneeditor/views/EmptyView.h>
#include <tdme/tools/sceneeditor/views/EnvironmentMappingView.h>
#include <tdme/tools/sceneeditor/views/ModelEditorView.h>
#include <tdme/tools/sceneeditor/views/ParticleSystemView.h>
#include <tdme/tools/sceneeditor/views/SceneEditorView.h>
#include <tdme/tools/sceneeditor/views/TriggerView.h>
#include <tdme/tools/shared/tools/Tools.h>
#include <tdme/tools/shared/views/PopUps.h>
#include <tdme/tools/shared/views/View.h>
#include <tdme/utilities/Console.h>

using std::string;

using tdme::tools::sceneeditor::TDMESceneEditor;
using tdme::utilities::Time;

using tdme::engine::model::Color4;
using tdme::engine::scene::Scene;
using tdme::engine::scene::SceneLibrary;
using tdme::engine::scene::ScenePropertyPresets;
using tdme::engine::Engine;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::GUI;
using tdme::tools::sceneeditor::controller::SceneEditorLibraryScreenController;
using tdme::tools::sceneeditor::views::EmptyView;
using tdme::tools::sceneeditor::views::EnvironmentMappingView;
using tdme::tools::sceneeditor::views::ModelEditorView;
using tdme::tools::sceneeditor::views::ParticleSystemView;
using tdme::tools::sceneeditor::views::SceneEditorView;
using tdme::tools::sceneeditor::views::TriggerView;
using tdme::tools::shared::tools::Tools;
using tdme::tools::shared::views::PopUps;
using tdme::tools::shared::views::View;
using tdme::utilities::Console;

string TDMESceneEditor::VERSION = "1.9.9";

TDMESceneEditor* TDMESceneEditor::instance = nullptr;

void TDMESceneEditor::main(int argc, char** argv) {
	Console::println(string("TDMESceneEditor " + VERSION));
	Console::println(string("Programmed 2014,...,2018 by Andreas Drewke, drewke.net."));
	Console::println();
	auto tdmeSceneEditor = new TDMESceneEditor();
	tdmeSceneEditor->run(argc, argv, "TDMESceneEditor");
}

TDMESceneEditor::TDMESceneEditor() {
	Tools::loadSettings(this);
	TDMESceneEditor::instance = this;
	engine = Engine::getInstance();
	view = nullptr;
	popUps = new PopUps();
	quitRequested = false;

}

TDMESceneEditor::~TDMESceneEditor() {
	delete sceneEditorView;
	delete modelEditorView;
	delete triggerView;
	delete emptyView;
	delete particleSystemView;
	delete popUps;
	delete prototypeLibraryScreenController;
}

TDMESceneEditor* TDMESceneEditor::getInstance() {
	return instance;
}

SceneEditorLibraryScreenController* TDMESceneEditor::getSceneEditorLibraryScreenController() {
	return prototypeLibraryScreenController;
}

SceneLibrary* TDMESceneEditor::getSceneLibrary() {
	return sceneEditorView->getScene()->getLibrary();
}

Scene* TDMESceneEditor::getScene() {
	return sceneEditorView->getScene();
}

void TDMESceneEditor::setView(View* view) {
	if (this->view != nullptr)
		this->view->deactivate();

	this->view = view;
	if (this->view != nullptr)
		this->view->activate();

}

View* TDMESceneEditor::getView() {
	return view;
}

void TDMESceneEditor::quit() {
	quitRequested = true;
}

void TDMESceneEditor::display() {
	engine->display();
	if (view != nullptr)
		view->display();
	if (quitRequested == true) {
		dispose();
		Application::exit(0);
	}
}

void TDMESceneEditor::dispose() {
	if (view != nullptr)
		view->deactivate();

	sceneEditorView->dispose();
	modelEditorView->dispose();
	triggerView->dispose();
	emptyView->dispose();
	particleSystemView->dispose();
	engine->dispose();
	Tools::oseDispose();
}

void TDMESceneEditor::initialize() {
	engine->initialize();
	engine->setSceneColor(Color4(125.0f / 255.0f, 125.0f / 255.0f, 125.0f / 255.0f, 1.0f));
	setInputEventHandler(engine->getGUI());
	Tools::oseInit();
	prototypeLibraryScreenController = new SceneEditorLibraryScreenController(popUps);
	prototypeLibraryScreenController->initialize();
	engine->getGUI()->addScreen(
		prototypeLibraryScreenController->getScreenNode()->getId(),
		prototypeLibraryScreenController->getScreenNode()
	);
	popUps->initialize();
	sceneEditorView = new SceneEditorView(popUps);
	sceneEditorView->initialize();
	ScenePropertyPresets::getInstance()->setDefaultSceneProperties(sceneEditorView->getScene());
	modelEditorView = new ModelEditorView(popUps);
	modelEditorView->initialize();
	triggerView = new TriggerView(popUps);
	triggerView->initialize();
	environmentMappingView = new EnvironmentMappingView(popUps);
	environmentMappingView->initialize();
	emptyView = new EmptyView(popUps);
	emptyView->initialize();
	particleSystemView = new ParticleSystemView(popUps);
	particleSystemView->initialize();
	setView(sceneEditorView);
}

void TDMESceneEditor::reshape(int width, int height) {
	engine->reshape(width, height);
}

void TDMESceneEditor::switchToSceneEditor() {
	setView(sceneEditorView);
}

void TDMESceneEditor::switchToModelEditor() {
	setView(modelEditorView);
}

void TDMESceneEditor::switchToTriggerView() {
	setView(triggerView);
}

void TDMESceneEditor::switchToEnvironmentMappingView() {
	setView(environmentMappingView);
}

void TDMESceneEditor::switchToEmptyView() {
	setView(emptyView);
}

void TDMESceneEditor::switchToParticleSystemView() {
	setView(particleSystemView);
}

