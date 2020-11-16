#include <tdme/tools/shared/controller/EntityDisplaySubScreenController.h>

#include <tdme/gui/events/GUIActionListener.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUINodeController.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/tools/shared/views/EntityDisplayView.h>
#include <tdme/tools/shared/views/EntityPhysicsView.h>
#include <tdme/utilities/MutableString.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>

using tdme::tools::shared::controller::EntityDisplaySubScreenController;
using tdme::gui::events::GUIActionListenerType;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeController;
using tdme::gui::nodes::GUIScreenNode;
using tdme::tools::shared::views::EntityDisplayView;
using tdme::tools::shared::views::EntityPhysicsView;
using tdme::utilities::MutableString;
using tdme::utilities::Console;
using tdme::utilities::Exception;

MutableString EntityDisplaySubScreenController::CHECKBOX_CHECKED = MutableString("1");
MutableString EntityDisplaySubScreenController::CHECKBOX_UNCHECKED = MutableString("");

EntityDisplaySubScreenController::EntityDisplaySubScreenController(EntityPhysicsView* physicsView)
{
	view = new EntityDisplayView(this);
	this->physicsView = physicsView;
}

EntityDisplaySubScreenController::~EntityDisplaySubScreenController() {
	delete view;
}

EntityDisplayView* EntityDisplaySubScreenController::getView()
{
	return view;
}

void EntityDisplaySubScreenController::initialize(GUIScreenNode* screenNode)
{
	try {
		displayBoundingVolume = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("display_boundingvolume"));
		displayShadowing = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("display_shadowing"));
		displayGround = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("display_ground"));
	} catch (Exception& exception) {
		Console::print(string("EntityDisplaySubScreenController::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
}

void EntityDisplaySubScreenController::setupDisplay()
{
	displayShadowing->getController()->setValue(view->isDisplayShadowing() == true ? CHECKBOX_CHECKED : CHECKBOX_UNCHECKED);
	displayGround->getController()->setValue(view->isDisplayGroundPlate() == true ? CHECKBOX_CHECKED : CHECKBOX_UNCHECKED);
	displayBoundingVolume->getController()->setValue(physicsView->isDisplayBoundingVolume() == true ? CHECKBOX_CHECKED : CHECKBOX_UNCHECKED);
}

void EntityDisplaySubScreenController::onDisplayApply()
{
	view->setDisplayShadowing(displayShadowing->getController()->getValue().equals(CHECKBOX_CHECKED));
	view->setDisplayGroundPlate(displayGround->getController()->getValue().equals(CHECKBOX_CHECKED));
	physicsView->setDisplayBoundingVolume(displayBoundingVolume->getController()->getValue().equals(CHECKBOX_CHECKED));
}

bool EntityDisplaySubScreenController::getDisplayShadowing()
{
	return displayShadowing->getController()->getValue().equals(CHECKBOX_CHECKED);
}

bool EntityDisplaySubScreenController::getDisplayGround()
{
	return displayGround->getController()->getValue().equals(CHECKBOX_CHECKED);
}

bool EntityDisplaySubScreenController::getDisplayBoundingVolume()
{
	return displayBoundingVolume->getController()->getValue().equals(CHECKBOX_CHECKED);
}

void EntityDisplaySubScreenController::onActionPerformed(GUIActionListenerType type, GUIElementNode* node)
{
	if (type == GUIActionListenerType::PERFORMED) {
		if (node->getId().compare("button_display_apply") == 0) {
			onDisplayApply();
		}
	}
}
