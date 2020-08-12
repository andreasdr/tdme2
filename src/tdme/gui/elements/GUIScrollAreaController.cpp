#include <tdme/gui/elements/GUIScrollAreaController.h>

#include <tdme/gui/events/GUIActionListener.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUIParentNode.h>
#include <tdme/gui/nodes/GUIScreenNode.h>

using tdme::gui::elements::GUIScrollAreaController;
using tdme::gui::events::GUIActionListener;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUIParentNode;
using tdme::gui::nodes::GUIScreenNode;

GUIScrollAreaController::GUIScrollAreaController(GUINode* node) 
	: GUINodeController(node), actionListener(nullptr)
{
}

bool GUIScrollAreaController::isDisabled()
{
	return false;
}

void GUIScrollAreaController::setDisabled(bool disabled)
{
}

void GUIScrollAreaController::initialize()
{

	class GUIScrollAreaControllerActionListener: public virtual GUIActionListener
	{

	public:
		void onActionPerformed(GUIActionListener_Type* type, GUIElementNode* node) override {
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
			} else if (node == downArrowNode) {
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
			} else if (node == leftArrowNode) {
				float elementWidth = contentNode->getComputedConstraints().width;
				float contentWidth = contentNode->getContentWidth();
				auto scrollableWidth = contentWidth - elementWidth;
				if (scrollableWidth <= 0.0f)
					return;

				auto childrenRenderOffsetX = contentNode->getChildrenRenderOffsetX() - 1.0f;
				if (childrenRenderOffsetX < 0.0f)
					childrenRenderOffsetX = 0.0f;

				contentNode->setChildrenRenderOffsetX(childrenRenderOffsetX);
			} else if (node == rightArrowNode) {
				float elementWidth = contentNode->getComputedConstraints().width;
				float contentWidth = contentNode->getContentWidth();
				auto scrollableWidth = contentWidth - elementWidth;
				if (scrollableWidth <= 0.0f)
					return;

				auto childrenRenderOffsetX = contentNode->getChildrenRenderOffsetX() + 1.0f;
				if (childrenRenderOffsetX > contentWidth - contentNode->getComputedConstraints().width) {
					childrenRenderOffsetX = contentWidth - contentNode->getComputedConstraints().width;
				}
				contentNode->setChildrenRenderOffsetX(childrenRenderOffsetX);
			}
		}

		/**
		 * Public constructor
		 * @param guiScrollAreaController gui scroll area controller
		 * @param upArrowNode up arrow node
		 * @param contentNode content node
		 * @param downArrowNode down arrow node
		 * @param leftArrowNode left arrow node
		 * @param rightArrowNode right arrow node
		 */
		GUIScrollAreaControllerActionListener(GUIScrollAreaController* guiScrollAreaController, GUIElementNode* upArrowNode, GUIParentNode* contentNode, GUIElementNode* downArrowNode, GUIElementNode* leftArrowNode, GUIElementNode* rightArrowNode)
			: guiScrollAreaController(guiScrollAreaController)
			, upArrowNode(upArrowNode)
			, contentNode(contentNode)
			, downArrowNode(downArrowNode)
			, leftArrowNode(leftArrowNode)
			, rightArrowNode(rightArrowNode) {
		}

	private:
		GUIScrollAreaController* guiScrollAreaController;
		GUIElementNode* upArrowNode;
		GUIParentNode* contentNode;
		GUIElementNode* downArrowNode;
		GUIElementNode* leftArrowNode;
		GUIElementNode* rightArrowNode;
	};


	auto const contentNode = dynamic_cast< GUIParentNode* >(node->getScreenNode()->getNodeById(node->getId() + "_inner"));
	auto const upArrowNode = dynamic_cast< GUIElementNode* >(node->getScreenNode()->getNodeById(node->getId() + "_scrollbar_vertical_layout_up"));
	auto const downArrowNode = dynamic_cast< GUIElementNode* >(node->getScreenNode()->getNodeById(node->getId() + "_scrollbar_vertical_layout_down"));
	auto const leftArrowNode = dynamic_cast< GUIElementNode* >(node->getScreenNode()->getNodeById(node->getId() + "_scrollbar_horizontal_layout_left"));
	auto const rightArrowNode = dynamic_cast< GUIElementNode* >(node->getScreenNode()->getNodeById(node->getId() + "_scrollbar_horizontal_layout_right"));
	node->getScreenNode()->addActionListener(actionListener = new GUIScrollAreaControllerActionListener(this, upArrowNode, contentNode, downArrowNode, leftArrowNode, rightArrowNode));
}

void GUIScrollAreaController::dispose()
{
	if (actionListener != nullptr) {
		node->getScreenNode()->removeActionListener(actionListener);
		delete actionListener;
		actionListener = nullptr;
	}
}

void GUIScrollAreaController::postLayout()
{
}

void GUIScrollAreaController::handleMouseEvent(GUINode* node, GUIMouseEvent* event)
{
}

void GUIScrollAreaController::handleKeyboardEvent(GUINode* node, GUIKeyboardEvent* event)
{
}

void GUIScrollAreaController::tick()
{
}

void GUIScrollAreaController::onFocusGained()
{
}

void GUIScrollAreaController::onFocusLost()
{
}

bool GUIScrollAreaController::hasValue()
{
	return false;
}

const MutableString& GUIScrollAreaController::getValue()
{
	return value;
}

void GUIScrollAreaController::setValue(const MutableString& value)
{
}

