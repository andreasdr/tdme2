#include <tdme/tools/shared/controller/ProgressBarScreenController.h>

#include <string>

#include <tdme/application/Application.h>
#include <tdme/engine/Engine.h>
#include <tdme/gui/GUI.h>
#include <tdme/gui/GUIParser.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUINodeController.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/utilities/MutableString.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>

using std::string;

using tdme::application::Application;
using tdme::engine::Engine;
using tdme::tools::shared::controller::ProgressBarScreenController;
using tdme::gui::GUI;
using tdme::gui::GUIParser;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUIMultilineTextNode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeController;
using tdme::gui::nodes::GUIScreenNode;
using tdme::utilities::Console;
using tdme::utilities::Exception;
using tdme::utilities::MutableString;

ProgressBarScreenController::ProgressBarScreenController()
{
}

ProgressBarScreenController::~ProgressBarScreenController() {
}

GUIScreenNode* ProgressBarScreenController::getScreenNode()
{
	return screenNode;
}

void ProgressBarScreenController::initialize()
{
	try {
		screenNode = GUIParser::parse("resources/engine/tools/shared/gui", "progressbar.xml");
		screenNode->setVisible(false);
		progressBarNode = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("progressbar"));
	} catch (Exception& exception) {
		Console::print(string("ProgressBarScreenController::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
}

void ProgressBarScreenController::dispose()
{
	screenNode = nullptr;
}

void ProgressBarScreenController::show()
{
	screenNode->setVisible(true);
	progress(0.0f);
}

void ProgressBarScreenController::progress(float value) {
	progressBarNode->getController()->setValue(MutableString(value, 4));
	Engine::getInstance()->display();
	Engine::getInstance()->getGUI()->render();
	Engine::getInstance()->getGUI()->handleEvents();
	Application::swapBuffers();
}

void ProgressBarScreenController::close()
{
	screenNode->setVisible(false);
}
