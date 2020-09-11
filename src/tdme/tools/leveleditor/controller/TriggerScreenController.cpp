#include <tdme/tools/leveleditor/controller/TriggerScreenController.h>

#include <string>

#include <tdme/gui/GUIParser.h>
#include <tdme/gui/events/Action.h>
#include <tdme/gui/events/GUIActionListener_Type.h>
#include <tdme/gui/events/GUIChangeListener.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUINodeController.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/gui/nodes/GUITextNode.h>
#include <tdme/tools/leveleditor/controller/LevelEditorEntityLibraryScreenController.h>
#include <tdme/tools/leveleditor/views/TriggerView.h>
#include <tdme/tools/shared/controller/EntityBaseSubScreenController.h>
#include <tdme/tools/shared/controller/EntityPhysicsSubScreenController.h>
#include <tdme/tools/shared/controller/FileDialogPath.h>
#include <tdme/tools/shared/controller/InfoDialogScreenController.h>
#include <tdme/tools/shared/tools/Tools.h>
#include <tdme/tools/shared/views/PopUps.h>
#include <tdme/tools/leveleditor/TDMELevelEditor.h>
#include <tdme/utilities/Float.h>
#include <tdme/utilities/MutableString.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>

using std::string;

using tdme::tools::leveleditor::controller::TriggerScreenController;
using tdme::gui::GUIParser;
using tdme::gui::events::Action;
using tdme::gui::events::GUIActionListener_Type;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeController;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::nodes::GUITextNode;
using tdme::tools::leveleditor::controller::LevelEditorEntityLibraryScreenController;
using tdme::tools::leveleditor::views::TriggerView;
using tdme::tools::shared::controller::EntityBaseSubScreenController;
using tdme::tools::shared::controller::EntityPhysicsSubScreenController;
using tdme::tools::shared::controller::FileDialogPath;
using tdme::tools::shared::controller::InfoDialogScreenController;
using tdme::tools::shared::tools::Tools;
using tdme::tools::shared::views::PopUps;
using tdme::tools::leveleditor::TDMELevelEditor;
using tdme::utilities::Float;
using tdme::utilities::MutableString;
using tdme::utilities::Console;
using tdme::utilities::Exception;

MutableString TriggerScreenController::TEXT_EMPTY = MutableString("");

TriggerScreenController::TriggerScreenController(TriggerView* view) 
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
		 * @param triggerScreenController trigger screen controller
		 * @param finalView final view
		 */
		OnSetEntityDataAction(TriggerScreenController* triggerScreenController, TriggerView* finalView): triggerScreenController(triggerScreenController), finalView(finalView) {
		}


	private:
		TriggerScreenController *triggerScreenController;
		TriggerView* finalView;

	};

	this->view = view;
	auto const finalView = view;
	this->modelPath = new FileDialogPath(".");
	this->entityBaseSubScreenController = new EntityBaseSubScreenController(view->getPopUpsViews(), new OnSetEntityDataAction(this, finalView));
	this->entityPhysicsSubScreenController = new EntityPhysicsSubScreenController(view->getPopUpsViews(), modelPath, false);
}

EntityPhysicsSubScreenController* TriggerScreenController::getEntityPhysicsSubScreenController()
{
	return entityPhysicsSubScreenController;
}

GUIScreenNode* TriggerScreenController::getScreenNode()
{
	return screenNode;
}

void TriggerScreenController::initialize()
{
	try {
		screenNode = GUIParser::parse("resources/engine/tools/leveleditor/gui", "screen_trigger.xml");
		screenNode->addActionListener(this);
		screenNode->addChangeListener(this);
		screenCaption = dynamic_cast< GUITextNode* >(screenNode->getNodeById("screen_caption"));
		viewPort = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("viewport"));
	} catch (Exception& exception) {
		Console::print(string("TriggerScreenController::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	entityBaseSubScreenController->initialize(screenNode);
	entityPhysicsSubScreenController->initialize(screenNode);
}

void TriggerScreenController::dispose()
{
}

void TriggerScreenController::setScreenCaption(const string& text)
{
	screenCaption->setText(text);
}

void TriggerScreenController::setEntityData(const string& name, const string& description)
{
	entityBaseSubScreenController->setEntityData(name, description);
}

void TriggerScreenController::unsetEntityData()
{
	entityBaseSubScreenController->unsetEntityData();
}

void TriggerScreenController::setEntityProperties(const string& presetId, const string& selectedName)
{
	entityBaseSubScreenController->setEntityProperties(view->getEntity(), presetId, selectedName);
}

void TriggerScreenController::unsetEntityProperties()
{
	entityBaseSubScreenController->unsetEntityProperties();
}

void TriggerScreenController::onQuit()
{
	TDMELevelEditor::getInstance()->quit();
}

void TriggerScreenController::showErrorPopUp(const string& caption, const string& message)
{
	view->getPopUpsViews()->getInfoDialogScreenController()->show(caption, message);
}

void TriggerScreenController::onValueChanged(GUIElementNode* node)
{
	entityBaseSubScreenController->onValueChanged(node, view->getEntity());
	entityPhysicsSubScreenController->onValueChanged(node, view->getEntity());
}

void TriggerScreenController::onActionPerformed(GUIActionListener_Type* type, GUIElementNode* node)
{
	entityBaseSubScreenController->onActionPerformed(type, node, view->getEntity());
	entityPhysicsSubScreenController->onActionPerformed(type, node, view->getEntity());
}

void TriggerScreenController::getViewPort(int& left, int& top, int& width, int& height) {
	auto& constraints = viewPort->getComputedConstraints();
	left = constraints.left + constraints.alignmentLeft + constraints.contentAlignmentLeft;
	top = constraints.top + constraints.alignmentTop + constraints.contentAlignmentTop;
	width = constraints.width;
	height = constraints.height;
}
