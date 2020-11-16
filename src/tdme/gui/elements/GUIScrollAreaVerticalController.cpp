#include <tdme/gui/elements/GUIScrollAreaVerticalController.h>

#include <tdme/gui/events/GUIActionListener.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUIParentNode.h>
#include <tdme/gui/nodes/GUIScreenNode.h>

using tdme::gui::elements::GUIScrollAreaVerticalController;
using tdme::gui::events::GUIActionListener;
using tdme::gui::events::GUIActionListenerType;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUIParentNode;
using tdme::gui::nodes::GUIScreenNode;

GUIScrollAreaVerticalController::GUIScrollAreaVerticalController(GUINode* node)
	: GUINodeController(node), actionListener(nullptr)
{
}

bool GUIScrollAreaVerticalController::isDisabled()
{
	return false;
}

void GUIScrollAreaVerticalController::setDisabled(bool disabled)
{
}

void GUIScrollAreaVerticalController::initialize()
{

	class GUIScrollAreaVerticalControllerActionListener: public virtual GUIActionListener
	{

	public:
		void onActionPerformed(GUIActionListenerType type, GUIElementNode* node) override {
			if (node == upArrowNode) {
				float elementHeight = contentNode->getComputedConstraints().height;
				float contentHeight = contentNode->getContentHeight();
				auto scrollableHeight = contentHeight - elementHeight;
				if (scrollableHeight <= 0.0f)
					return;

				auto childrenRenderOffsetY = contentNode->getChildrenRenderOffsetY() - 1.0f;
				if (childrenRenderOffsetY < 0.0f)
					childrenRenderOffsetY = 0.0f;

				contentNode->setChildrenRenderOffsetY(childrenRenderOffsetY);
			} else
			if (node == downArrowNode) {
				float elementHeight = contentNode->getComputedConstraints().height;
				float contentHeight = contentNode->getContentHeight();
				auto scrollableHeight = contentHeight - elementHeight;
				if (scrollableHeight <= 0.0f)
					return;

				auto childrenRenderOffsetY = contentNode->getChildrenRenderOffsetY() + 1.0f;
				if (childrenRenderOffsetY > contentHeight - contentNode->getComputedConstraints().height) {
					childrenRenderOffsetY = contentHeight - contentNode->getComputedConstraints().height;
				}
				contentNode->setChildrenRenderOffsetY(childrenRenderOffsetY);
			}
		}

		/**
		 * Public constructor
		 * @param guiScrollAreaVerticalController gui scroll area vertical controller
		 * @param upArrowNode up arrow node
		 * @param contentNode content node
		 * @param downArrowNode down arrow node
		 */
		GUIScrollAreaVerticalControllerActionListener(GUIScrollAreaVerticalController* guiScrollAreaVerticalController, GUIElementNode* upArrowNode, GUIParentNode* contentNode, GUIElementNode* downArrowNode)
			: guiScrollAreaVerticalController(guiScrollAreaVerticalController)
			, upArrowNode(upArrowNode)
			, contentNode(contentNode)
			, downArrowNode(downArrowNode) {
		}


	private:
		GUIScrollAreaVerticalController *guiScrollAreaVerticalController;
		GUIElementNode* upArrowNode;
		GUIParentNode* contentNode;
		GUIElementNode* downArrowNode;
	};


	auto const contentNode = dynamic_cast< GUIParentNode* >(node->getScreenNode()->getNodeById(node->getId() + "_inner"));
	auto const upArrowNode = dynamic_cast< GUIElementNode* >(node->getScreenNode()->getNodeById(node->getId() + "_scrollbar_vertical_layout_up"));
	auto const downArrowNode = dynamic_cast< GUIElementNode* >(node->getScreenNode()->getNodeById(node->getId() + "_scrollbar_vertical_layout_down"));
	node->getScreenNode()->addActionListener(actionListener = new GUIScrollAreaVerticalControllerActionListener(this, upArrowNode, contentNode, downArrowNode));
}

void GUIScrollAreaVerticalController::dispose()
{
	if (actionListener != nullptr) {
		node->getScreenNode()->removeActionListener(actionListener);
		delete actionListener;
		actionListener = nullptr;
	}
}

void GUIScrollAreaVerticalController::postLayout()
{
}

void GUIScrollAreaVerticalController::handleMouseEvent(GUINode* node, GUIMouseEvent* event)
{
}

void GUIScrollAreaVerticalController::handleKeyboardEvent(GUINode* node, GUIKeyboardEvent* event)
{
}

void GUIScrollAreaVerticalController::tick()
{
}

void GUIScrollAreaVerticalController::onFocusGained()
{
}

void GUIScrollAreaVerticalController::onFocusLost()
{
}

bool GUIScrollAreaVerticalController::hasValue()
{
	return false;
}

const MutableString& GUIScrollAreaVerticalController::getValue()
{
	return value;
}

void GUIScrollAreaVerticalController::setValue(const MutableString& value)
{
}

