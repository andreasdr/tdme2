#include <tdme/gui/elements/GUISelectBoxParentOptionController.h>

#include <tdme/gui/elements/GUISelectBoxController.h>
#include <tdme/gui/events/GUIActionListener.h>
#include <tdme/gui/nodes/GUIElementController.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUINodeConditions.h>
#include <tdme/gui/nodes/GUIParentNode.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/gui/GUI.h>
#include <tdme/utilities/Console.h>

using tdme::gui::elements::GUISelectBoxController;
using tdme::gui::elements::GUISelectBoxParentOptionController;
using tdme::gui::events::GUIActionListener;
using tdme::gui::events::GUIActionListenerType;
using tdme::gui::nodes::GUIElementController;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeConditions;
using tdme::gui::nodes::GUIParentNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::GUI;
using tdme::utilities::Console;

string GUISelectBoxParentOptionController::CONDITION_OPENED = "opened";
string GUISelectBoxParentOptionController::CONDITION_CLOSED = "closed";

GUISelectBoxParentOptionController::GUISelectBoxParentOptionController(GUINode* node)
	: GUISelectBoxOptionController(node), open(false)
{
}

void GUISelectBoxParentOptionController::initialize()
{
	class ArrowNodeActionListener: public virtual GUIActionListener
	{

	public:
		// overriden method
		void onActionPerformed(GUIActionListenerType type, GUIElementNode* node) override {
			if (node == selectBoxParentOptionController->arrowNode && type == GUIActionListenerType::PERFORMED) selectBoxParentOptionController->toggleOpenState();
		}

		/**
		 * Public constructor
		 * @param selectBoxParentOptionController select box parent option controller
		 */
		ArrowNodeActionListener(GUISelectBoxParentOptionController* selectBoxParentOptionController): selectBoxParentOptionController(selectBoxParentOptionController) {
		}

	private:
		GUISelectBoxParentOptionController* selectBoxParentOptionController { nullptr };
	};
	arrowNode = dynamic_cast<GUIElementNode*>(node->getScreenNode()->getNodeById(node->getId() + "_arrow"));
	arrowNode->getActiveConditions().add(open == true?CONDITION_OPENED:CONDITION_CLOSED);
	arrowNode->getScreenNode()->addActionListener(arrowNodeActionListener = new ArrowNodeActionListener(this));
	//
	GUISelectBoxOptionController::initialize();
}

void GUISelectBoxParentOptionController::dispose() {
	if (arrowNodeActionListener != nullptr) {
		node->getScreenNode()->removeActionListener(arrowNodeActionListener);
		delete arrowNodeActionListener;
		arrowNodeActionListener = nullptr;
	}
}

void GUISelectBoxParentOptionController::toggleOpenState()
{
	arrowNode->getActiveConditions().remove(open == true ?CONDITION_OPENED:CONDITION_CLOSED);
	open = open == true?false:true;
	arrowNode->getActiveConditions().add(open == true?CONDITION_OPENED:CONDITION_CLOSED);
}
