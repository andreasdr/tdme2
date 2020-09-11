#include <tdme/tools/leveleditor/controller/EmptyScreenController.h>

#include <string>

#include <tdme/gui/GUIParser.h>
#include <tdme/gui/events/Action.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/gui/nodes/GUITextNode.h>
#include <tdme/tools/leveleditor/controller/LevelEditorEntityLibraryScreenController.h>
#include <tdme/tools/leveleditor/views/EmptyView.h>
#include <tdme/tools/shared/controller/EntityBaseSubScreenController.h>
#include <tdme/tools/shared/controller/InfoDialogScreenController.h>
#include <tdme/tools/shared/views/PopUps.h>
#include <tdme/tools/leveleditor/TDMELevelEditor.h>
#include <tdme/utilities/MutableString.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>

using std::string;

using tdme::tools::leveleditor::controller::EmptyScreenController;
using tdme::gui::GUIParser;
using tdme::gui::events::Action;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::nodes::GUITextNode;
using tdme::tools::leveleditor::controller::LevelEditorEntityLibraryScreenController;
using tdme::tools::leveleditor::views::EmptyView;
using tdme::tools::shared::controller::EntityBaseSubScreenController;
using tdme::tools::shared::controller::InfoDialogScreenController;
using tdme::tools::shared::views::PopUps;
using tdme::tools::leveleditor::TDMELevelEditor;
using tdme::utilities::MutableString;
using tdme::utilities::Console;
using tdme::utilities::Exception;

EmptyScreenController::EmptyScreenController(EmptyView* view) 
{
	class OnSetEntityDataAction: public virtual Action
	{
	public:
		void performAction() override {
			finalView->updateGUIElements();
			TDMELevelEditor::getInstance()->getLevelEditorEntityLibraryScreenController()->setEntityLibrary();
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
	this->entityBaseSubScreenController = new EntityBaseSubScreenController(view->getPopUpsViews(), new OnSetEntityDataAction(this, finalView));
}

GUIScreenNode* EmptyScreenController::getScreenNode()
{
	return screenNode;
}

void EmptyScreenController::initialize()
{
	try {
		screenNode = GUIParser::parse("resources/engine/tools/leveleditor/gui", "screen_empty.xml");
		screenNode->addActionListener(this);
		screenNode->addChangeListener(this);
		screenCaption = dynamic_cast< GUITextNode* >(screenNode->getNodeById("screen_caption"));
		viewPort = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("viewport"));
	} catch (Exception& exception) {
		Console::print(string("EmptyScreenController::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	entityBaseSubScreenController->initialize(screenNode);
}

void EmptyScreenController::dispose()
{
}

void EmptyScreenController::setScreenCaption(const string& text)
{
	screenCaption->setText(text);
}

void EmptyScreenController::setEntityData(const string& name, const string& description)
{
	entityBaseSubScreenController->setEntityData(name, description);
}

void EmptyScreenController::unsetEntityData()
{
	entityBaseSubScreenController->unsetEntityData();
}

void EmptyScreenController::setEntityProperties(const string& presetId, const string& selectedName)
{
	entityBaseSubScreenController->setEntityProperties(view->getEntity(), presetId, selectedName);
}

void EmptyScreenController::unsetEntityProperties()
{
	entityBaseSubScreenController->unsetEntityProperties();
}

void EmptyScreenController::onQuit()
{
	TDMELevelEditor::getInstance()->quit();
}

void EmptyScreenController::showErrorPopUp(const string& caption, const string& message)
{
	view->getPopUpsViews()->getInfoDialogScreenController()->show(caption, message);
}

void EmptyScreenController::onValueChanged(GUIElementNode* node)
{
	entityBaseSubScreenController->onValueChanged(node, view->getEntity());
}

void EmptyScreenController::onActionPerformed(GUIActionListener_Type* type, GUIElementNode* node)
{
	entityBaseSubScreenController->onActionPerformed(type, node, view->getEntity());
}

void EmptyScreenController::getViewPort(int& left, int& top, int& width, int& height) {
	auto& constraints = viewPort->getComputedConstraints();
	left = constraints.left + constraints.alignmentLeft + constraints.contentAlignmentLeft;
	top = constraints.top + constraints.alignmentTop + constraints.contentAlignmentTop;
	width = constraints.width;
	height = constraints.height;
}
