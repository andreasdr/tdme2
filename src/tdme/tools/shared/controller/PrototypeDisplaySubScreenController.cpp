#include <tdme/tools/shared/controller/PrototypeDisplaySubScreenController.h>

#include <tdme/gui/events/GUIActionListener.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUINodeController.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/tools/shared/views/PrototypeDisplayView.h>
#include <tdme/tools/shared/views/PrototypePhysicsView.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>
#include <tdme/utilities/MutableString.h>

using tdme::gui::events::GUIActionListenerType;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeController;
using tdme::gui::nodes::GUIScreenNode;
using tdme::tools::shared::controller::PrototypeDisplaySubScreenController;
using tdme::tools::shared::views::PrototypeDisplayView;
using tdme::tools::shared::views::PrototypePhysicsView;
using tdme::utilities::Console;
using tdme::utilities::Exception;
using tdme::utilities::MutableString;

MutableString PrototypeDisplaySubScreenController::CHECKBOX_CHECKED = MutableString("1");
MutableString PrototypeDisplaySubScreenController::CHECKBOX_UNCHECKED = MutableString("");

PrototypeDisplaySubScreenController::PrototypeDisplaySubScreenController(PrototypePhysicsView* physicsView)
{
	view = new PrototypeDisplayView(this);
	this->physicsView = physicsView;
}

PrototypeDisplaySubScreenController::~PrototypeDisplaySubScreenController() {
	delete view;
}

PrototypeDisplayView* PrototypeDisplaySubScreenController::getView()
{
	return view;
}

void PrototypeDisplaySubScreenController::initialize(GUIScreenNode* screenNode)
{
	try {
		displayBoundingVolume = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("display_boundingvolume"));
		displayShadowing = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("display_shadowing"));
		displayGround = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("display_ground"));
	} catch (Exception& exception) {
		Console::print(string("PrototypeDisplaySubScreenController::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
}

void PrototypeDisplaySubScreenController::setupDisplay()
{
	displayShadowing->getController()->setValue(view->isDisplayShadowing() == true ? CHECKBOX_CHECKED : CHECKBOX_UNCHECKED);
	displayGround->getController()->setValue(view->isDisplayGroundPlate() == true ? CHECKBOX_CHECKED : CHECKBOX_UNCHECKED);
	displayBoundingVolume->getController()->setValue(physicsView->isDisplayBoundingVolume() == true ? CHECKBOX_CHECKED : CHECKBOX_UNCHECKED);
}

void PrototypeDisplaySubScreenController::onDisplayApply()
{
	view->setDisplayShadowing(displayShadowing->getController()->getValue().equals(CHECKBOX_CHECKED));
	view->setDisplayGroundPlate(displayGround->getController()->getValue().equals(CHECKBOX_CHECKED));
	physicsView->setDisplayBoundingVolume(displayBoundingVolume->getController()->getValue().equals(CHECKBOX_CHECKED));
}

bool PrototypeDisplaySubScreenController::getDisplayShadowing()
{
	return displayShadowing->getController()->getValue().equals(CHECKBOX_CHECKED);
}

bool PrototypeDisplaySubScreenController::getDisplayGround()
{
	return displayGround->getController()->getValue().equals(CHECKBOX_CHECKED);
}

bool PrototypeDisplaySubScreenController::getDisplayBoundingVolume()
{
	return displayBoundingVolume->getController()->getValue().equals(CHECKBOX_CHECKED);
}

void PrototypeDisplaySubScreenController::onActionPerformed(GUIActionListenerType type, GUIElementNode* node)
{
	if (type == GUIActionListenerType::PERFORMED) {
		if (node->getId().compare("button_display_apply") == 0) {
			onDisplayApply();
		}
	}
}
