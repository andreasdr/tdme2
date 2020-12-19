#include <tdme/tools/shared/views/PopUps.h>

#include <tdme/engine/Engine.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/gui/GUI.h>
#include <tdme/tools/shared/controller/FileDialogScreenController.h>
#include <tdme/tools/shared/controller/InfoDialogScreenController.h>
#include <tdme/tools/shared/controller/ProgressBarScreenController.h>

using tdme::engine::Engine;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::GUI;
using tdme::tools::shared::controller::FileDialogScreenController;
using tdme::tools::shared::controller::InfoDialogScreenController;
using tdme::tools::shared::controller::ProgressBarScreenController;
using tdme::tools::shared::views::PopUps;

PopUps::PopUps()
{
	fileDialogScreenController = new FileDialogScreenController();
	infoDialogScreenController = new InfoDialogScreenController();
	progressBarScreenController = new ProgressBarScreenController();
}

PopUps::~PopUps() {
	delete fileDialogScreenController;
	delete infoDialogScreenController;
	delete progressBarScreenController;
}

FileDialogScreenController* PopUps::getFileDialogScreenController()
{
	return fileDialogScreenController;
}

InfoDialogScreenController* PopUps::getInfoDialogScreenController()
{
	return infoDialogScreenController;
}

ProgressBarScreenController* PopUps::getProgressBarScreenController()
{
	return progressBarScreenController;
}

void PopUps::initialize()
{
	fileDialogScreenController->initialize();
	infoDialogScreenController->initialize();
	progressBarScreenController->initialize();
	Engine::getInstance()->getGUI()->addScreen(fileDialogScreenController->getScreenNode()->getId(), fileDialogScreenController->getScreenNode());
	Engine::getInstance()->getGUI()->addScreen(infoDialogScreenController->getScreenNode()->getId(), infoDialogScreenController->getScreenNode());
	Engine::getInstance()->getGUI()->addScreen(progressBarScreenController->getScreenNode()->getId(), progressBarScreenController->getScreenNode());
}

void PopUps::dispose()
{
	Engine::getInstance()->getGUI()->removeScreen(fileDialogScreenController->getScreenNode()->getId());
	Engine::getInstance()->getGUI()->removeScreen(infoDialogScreenController->getScreenNode()->getId());
	Engine::getInstance()->getGUI()->removeScreen(progressBarScreenController->getScreenNode()->getId());
	fileDialogScreenController->dispose();
	infoDialogScreenController->dispose();
	progressBarScreenController->dispose();
}
