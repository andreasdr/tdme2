#include <tdme/tools/particlesystem/TDMEParticleSystem.h>

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
#include <tdme/tools/shared/views/SharedParticleSystemView.h>
#include <tdme/tools/shared/views/View.h>
#include <tdme/utilities/Console.h>

using std::string;

using tdme::tools::particlesystem::TDMEParticleSystem;

using tdme::utilities::Time;

using tdme::engine::model::Color4;
using tdme::engine::prototype::Prototype;
using tdme::engine::prototype::Prototype_Type;
using tdme::engine::Engine;
using tdme::engine::Version;
using tdme::gui::GUI;
using tdme::tools::shared::tools::Tools;
using tdme::tools::shared::views::PopUps;
using tdme::tools::shared::views::SharedParticleSystemView;
using tdme::tools::shared::views::View;
using tdme::utilities::Console;

TDMEParticleSystem* TDMEParticleSystem::instance = nullptr;

TDMEParticleSystem::TDMEParticleSystem()
{
	Tools::loadSettings(this);
	TDMEParticleSystem::instance = this;
	engine = Engine::getInstance();
	view = nullptr;
	viewInitialized = false;
	viewNew = nullptr;
	popUps = new PopUps();
	particleSystemView = nullptr;
	quitRequested = false;
}

TDMEParticleSystem::~TDMEParticleSystem() {
	delete popUps;
	delete particleSystemView;
}

void TDMEParticleSystem::main(int argc, char** argv)
{
	Console::println(string("TDMEParticleSystem ") + Version::getVersion());
	Console::println(Version::getCopyright());
	Console::println();

	auto tdmeParticleSystem = new TDMEParticleSystem();
	tdmeParticleSystem->run(argc, argv, "TDMEParticleSystem");
}

TDMEParticleSystem* TDMEParticleSystem::getInstance()
{
	return instance;
}

void TDMEParticleSystem::setView(View* view)
{
	viewNew = view;
}

View* TDMEParticleSystem::getView()
{
	return view;
}

void TDMEParticleSystem::quit()
{
	quitRequested = true;
}

void TDMEParticleSystem::display()
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

void TDMEParticleSystem::dispose()
{
	if (view != nullptr && viewInitialized == true) {
		view->deactivate();
		view->dispose();
		delete particleSystemView->getPrototype();
		view = nullptr;
	}
	engine->dispose();
	Tools::oseDispose();
}

void TDMEParticleSystem::initialize()
{
	engine->initialize();
	engine->setSceneColor(Color4(125.0f / 255.0f, 125.0f / 255.0f, 125.0f / 255.0f, 1.0f));
	setInputEventHandler(engine->getGUI());
	Tools::oseInit();
	popUps->initialize();
	setView(particleSystemView = new SharedParticleSystemView(popUps));
	particleSystemView->setPrototype(
		new Prototype(
			-1,
			Prototype_Type::PARTICLESYSTEM,
			"Untitled",
			"",
			"Untitled.tps",
			"",
			"",
			nullptr,
			Vector3()
		)
	);
}

void TDMEParticleSystem::reshape(int width, int height)
{
	engine->reshape(width, height);
}
