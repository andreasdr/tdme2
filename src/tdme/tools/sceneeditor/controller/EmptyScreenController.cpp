#include <tdme/tools/sceneeditor/controller/EmptyScreenController.h>

#include <string>

#include <tdme/gui/events/Action.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/gui/nodes/GUITextNode.h>
#include <tdme/gui/GUIParser.h>
#include <tdme/tools/sceneeditor/controller/SceneEditorLibraryScreenController.h>
#include <tdme/tools/sceneeditor/views/EmptyView.h>
#include <tdme/tools/sceneeditor/TDMESceneEditor.h>
#include <tdme/tools/shared/controller/InfoDialogScreenController.h>
#include <tdme/tools/shared/controller/PrototypeBaseSubScreenController.h>
#include <tdme/tools/shared/views/PopUps.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>
#include <tdme/utilities/MutableString.h>

using std::string;

using tdme::gui::events::Action;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::nodes::GUITextNode;
using tdme::gui::GUIParser;
using tdme::tools::sceneeditor::controller::EmptyScreenController;
using tdme::tools::sceneeditor::controller::SceneEditorLibraryScreenController;
using tdme::tools::sceneeditor::views::EmptyView;
using tdme::tools::sceneeditor::TDMESceneEditor;
using tdme::tools::shared::controller::InfoDialogScreenController;
using tdme::tools::shared::controller::PrototypeBaseSubScreenController;
using tdme::tools::shared::views::PopUps;
using tdme::utilities::Console;
using tdme::utilities::Exception;
using tdme::utilities::MutableString;

EmptyScreenController::EmptyScreenController(EmptyView* view)
{
	class OnSetEntityDataAction: public virtual Action
	{
	public:
		void performAction() override {
			finalView->updateGUIElements();
			TDMESceneEditor::getInstance()->getSceneEditorLibraryScreenController()->setPrototypeLibrary();
		}

		/**
		 * Public constructor
		 * @param emptyScreenController empty screen controller
		 * @param finalView final view
		 */
		OnSetEntityDataAction(EmptyScreenController* emptyScreenController, EmptyView* finalView): emptyScreenController(emptyScreenController), finalView(finalView) {
		}

	private:
		EmptyScreenController* emptyScreenController;
		EmptyView* finalView;
	};

	this->view = view;
	auto const finalView = view;
	this->prototypeBaseSubScreenController = new PrototypeBaseSubScreenController(view->getPopUps(), new OnSetEntityDataAction(this, finalView));
}

GUIScreenNode* EmptyScreenController::getScreenNode()
{
	return screenNode;
}

void EmptyScreenController::initialize()
{
	try {
		screenNode = GUIParser::parse("resources/engine/gui", "screen_empty.xml");
		screenNode->addActionListener(this);
		screenNode->addChangeListener(this);
		screenCaption = dynamic_cast< GUITextNode* >(screenNode->getNodeById("screen_caption"));
		viewPort = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("viewport"));
	} catch (Exception& exception) {
		Console::print(string("EmptyScreenController::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	prototypeBaseSubScreenController->initialize(screenNode);
}

void EmptyScreenController::dispose()
{
}

void EmptyScreenController::setScreenCaption(const string& text)
{
	screenCaption->setText(text);
}

void EmptyScreenController::setPrototypeData(const string& name, const string& description)
{
	prototypeBaseSubScreenController->setPrototypeData(name, description);
}

void EmptyScreenController::unsetPrototypeData()
{
	prototypeBaseSubScreenController->unsetPrototypeData();
}

void EmptyScreenController::setPrototypeProperties(const string& presetId, const string& selectedName)
{
	prototypeBaseSubScreenController->setPrototypeProperties(view->getPrototype(), presetId, selectedName);
}

void EmptyScreenController::unsetPrototypeProperties()
{
	prototypeBaseSubScreenController->unsetPrototypeProperties();
}

void EmptyScreenController::onQuit()
{
	TDMESceneEditor::getInstance()->quit();
}

void EmptyScreenController::showErrorPopUp(const string& caption, const string& message)
{
	view->getPopUps()->getInfoDialogScreenController()->show(caption, message);
}

void EmptyScreenController::onValueChanged(GUIElementNode* node)
{
	prototypeBaseSubScreenController->onValueChanged(node, view->getPrototype());
}

void EmptyScreenController::onActionPerformed(GUIActionListenerType type, GUIElementNode* node)
{
	prototypeBaseSubScreenController->onActionPerformed(type, node, view->getPrototype());
}

void EmptyScreenController::getViewPort(int& left, int& top, int& width, int& height) {
	auto& constraints = viewPort->getComputedConstraints();
	left = constraints.left + constraints.alignmentLeft + constraints.contentAlignmentLeft;
	top = constraints.top + constraints.alignmentTop + constraints.contentAlignmentTop;
	width = constraints.width;
	height = constraints.height;
}
