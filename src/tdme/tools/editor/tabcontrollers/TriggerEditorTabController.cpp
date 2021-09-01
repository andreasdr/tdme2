#include <tdme/tools/editor/tabcontrollers/TriggerEditorTabController.h>

#include <string>

#include <tdme/engine/prototype/Prototype.h>
#include <tdme/gui/events/GUIActionListener.h>
#include <tdme/gui/events/GUIChangeListener.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/gui/GUI.h>
#include <tdme/gui/GUIParser.h>
#include <tdme/tools/editor/controllers/EditorScreenController.h>
#include <tdme/tools/editor/controllers/FileDialogScreenController.h>
#include <tdme/tools/editor/controllers/InfoDialogScreenController.h>
#include <tdme/tools/editor/misc/PopUps.h>
#include <tdme/tools/editor/misc/Tools.h>
#include <tdme/tools/editor/tabcontrollers/subcontrollers/BasePropertiesSubController.h>
#include <tdme/tools/editor/tabcontrollers/subcontrollers/PrototypeDisplaySubController.h>
#include <tdme/tools/editor/tabcontrollers/subcontrollers/PrototypePhysicsSubController.h>
#include <tdme/tools/editor/tabcontrollers/TabController.h>
#include <tdme/tools/editor/tabviews/subviews/PrototypePhysicsSubView.h>
#include <tdme/tools/editor/tabviews/TriggerEditorTabView.h>
#include <tdme/tools/editor/views/EditorView.h>
#include <tdme/utilities/Action.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>
#include <tdme/utilities/ExceptionBase.h>

using std::string;

using tdme::tools::editor::tabcontrollers::TriggerEditorTabController;

using tdme::engine::prototype::Prototype;
using tdme::gui::events::GUIActionListenerType;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::GUIParser;
using tdme::tools::editor::controllers::EditorScreenController;
using tdme::tools::editor::controllers::FileDialogScreenController;
using tdme::tools::editor::controllers::InfoDialogScreenController;
using tdme::tools::editor::misc::PopUps;
using tdme::tools::editor::misc::Tools;
using tdme::tools::editor::tabcontrollers::subcontrollers::BasePropertiesSubController;
using tdme::tools::editor::tabcontrollers::subcontrollers::PrototypeDisplaySubController;
using tdme::tools::editor::tabcontrollers::subcontrollers::PrototypePhysicsSubController;
using tdme::tools::editor::tabcontrollers::TabController;
using tdme::tools::editor::tabviews::subviews::PrototypePhysicsSubView;
using tdme::tools::editor::tabviews::TriggerEditorTabView;
using tdme::tools::editor::views::EditorView;
using tdme::utilities::Action;
using tdme::utilities::Console;
using tdme::utilities::Exception;
using tdme::utilities::ExceptionBase;

TriggerEditorTabController::TriggerEditorTabController(TriggerEditorTabView* view)
{
	this->view = view;
	this->popUps = view->getPopUps();
	this->basePropertiesSubController = new BasePropertiesSubController(view->getEditorView(), "prototype");
	this->prototypePhysicsSubController = new PrototypePhysicsSubController(view->getEditorView(), view, &modelPath, false);
	this->prototypeDisplaySubController = new PrototypeDisplaySubController(view->getEditorView(), view, this->prototypePhysicsSubController->getView());
}

TriggerEditorTabController::~TriggerEditorTabController() {
	delete prototypeDisplaySubController;
	delete prototypePhysicsSubController;
	delete basePropertiesSubController;
}

TriggerEditorTabView* TriggerEditorTabController::getView() {
	return view;
}

GUIScreenNode* TriggerEditorTabController::getScreenNode()
{
	return screenNode;
}

void TriggerEditorTabController::initialize(GUIScreenNode* screenNode)
{
	this->screenNode = screenNode;
	basePropertiesSubController->initialize(screenNode);
	prototypeDisplaySubController->initialize(screenNode);
	prototypePhysicsSubController->initialize(screenNode);
}

void TriggerEditorTabController::dispose()
{
}

void TriggerEditorTabController::save()
{
	auto fileName = view->getPrototype() != nullptr?view->getPrototype()->getFileName():"";
	try {
		if (fileName.empty() == true) throw ExceptionBase("Could not save file. No filename known");
		view->saveFile(
			Tools::getPathName(fileName),
			Tools::getFileName(fileName)
		);
	} catch (Exception& exception) {
		showErrorPopUp("Warning", (string(exception.what())));
	}
}

void TriggerEditorTabController::saveAs()
{
	class OnTriggerSave: public virtual Action
	{
	public:
		void performAction() override {
			try {
				triggerEditorTabController->view->saveFile(
					triggerEditorTabController->popUps->getFileDialogScreenController()->getPathName(),
					triggerEditorTabController->popUps->getFileDialogScreenController()->getFileName()
				);
				triggerEditorTabController->triggerPath.setPath(
					triggerEditorTabController->popUps->getFileDialogScreenController()->getPathName()
				);
			} catch (Exception& exception) {
				triggerEditorTabController->showErrorPopUp("Warning", (string(exception.what())));
			}
			triggerEditorTabController->popUps->getFileDialogScreenController()->close();
		}
		OnTriggerSave(TriggerEditorTabController* triggerEditorTabController): triggerEditorTabController(triggerEditorTabController) {
		}
	private:
		TriggerEditorTabController* triggerEditorTabController;
	};

	auto fileName = view->getPrototype() != nullptr?view->getPrototype()->getFileName():"";
	vector<string> extensions = {
		"tempty"
	};
	popUps->getFileDialogScreenController()->show(
		fileName.empty() == false?Tools::getPathName(fileName):triggerPath.getPath(),
		"Save to: ",
		extensions,
		Tools::getFileName(fileName),
		false,
		new OnTriggerSave(this)
	);
}

void TriggerEditorTabController::onValueChanged(GUIElementNode* node)
{
	if (node->getId() == "selectbox_outliner") {
		auto outlinerNode = view->getEditorView()->getScreenController()->getOutlinerSelection();
		updateDetails(outlinerNode);
	}
	basePropertiesSubController->onValueChanged(node, view->getPrototype());
	prototypeDisplaySubController->onValueChanged(node, view->getPrototype());
	prototypePhysicsSubController->onValueChanged(node, view->getPrototype());
}

void TriggerEditorTabController::onFocus(GUIElementNode* node) {
	basePropertiesSubController->onFocus(node, view->getPrototype());
}

void TriggerEditorTabController::onUnfocus(GUIElementNode* node) {
	basePropertiesSubController->onUnfocus(node, view->getPrototype());
}

void TriggerEditorTabController::onContextMenuRequested(GUIElementNode* node, int mouseX, int mouseY) {
	basePropertiesSubController->onContextMenuRequested(node, mouseX, mouseY, view->getPrototype());
	prototypePhysicsSubController->onContextMenuRequested(node, mouseX, mouseY, view->getPrototype());
}

void TriggerEditorTabController::onActionPerformed(GUIActionListenerType type, GUIElementNode* node)
{
	basePropertiesSubController->onActionPerformed(type, node, view->getPrototype());
	prototypePhysicsSubController->onActionPerformed(type, node, view->getPrototype());
}

void TriggerEditorTabController::setOutlinerContent() {
	string xml;
	xml+= "<selectbox-parent-option image=\"resources/engine/images/folder.png\" text=\"" + GUIParser::escapeQuotes("Prototype") + "\" value=\"" + GUIParser::escapeQuotes("prototype") + "\">\n";
	auto prototype = view->getPrototype();
	if (prototype != nullptr) {
		basePropertiesSubController->createBasePropertiesXML(prototype, xml);
		prototypePhysicsSubController->createOutlinerPhysicsXML(prototype, xml);
	}
	xml+= "</selectbox-parent-option>\n";
	view->getEditorView()->setOutlinerContent(xml);}

void TriggerEditorTabController::setOutlinerAddDropDownContent() {
	view->getEditorView()->setOutlinerAddDropDownContent(
		string("<dropdown-option text=\"Property\" value=\"property\" />\n") +
		string("<dropdown-option text=\"BV\" value=\"boundingvolume\" />\n")
	);
}

void TriggerEditorTabController::updateDetails(const string& outlinerNode) {
	view->getEditorView()->setDetailsContent(string());
	basePropertiesSubController->updateDetails(view->getPrototype(), outlinerNode);
	prototypeDisplaySubController->updateDetails(view->getPrototype(), outlinerNode);
	prototypePhysicsSubController->updateDetails(view->getPrototype(), outlinerNode);
	prototypePhysicsSubController->getView()->setDisplayBoundingVolume(true);
}

void TriggerEditorTabController::showErrorPopUp(const string& caption, const string& message)
{
	popUps->getInfoDialogScreenController()->show(caption, message);
}