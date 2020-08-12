#include <tdme/gui/elements/GUIScrollAreaHorizontalController.h>

#include <tdme/gui/events/GUIActionListener.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUIParentNode.h>
#include <tdme/gui/nodes/GUIScreenNode.h>

using tdme::gui::elements::GUIScrollAreaHorizontalController;
using tdme::gui::events::GUIActionListener;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUIParentNode;
using tdme::gui::nodes::GUIScreenNode;

GUIScrollAreaHorizontalController::GUIScrollAreaHorizontalController(GUINode* node) 
	: GUINodeController(node), actionListener(nullptr)
{
}

bool GUIScrollAreaHorizontalController::isDisabled()
{
	return false;
}

void GUIScrollAreaHorizontalController::setDisabled(bool disabled)
{
}

void GUIScrollAreaHorizontalController::initialize()
{

	class GUIScrollAreaHorizontalControllerActionListener: public virtual GUIActionListener
	{

	public:
		void onActionPerformed(GUIActionListener_Type* type, GUIElementNode* node) override {
			if (node == leftArrowNode) {
				float elementWidth = contentNode->getComputedConstraints().width;
				float contentWidth = contentNode->getContentWidth();
				auto scrollableWidth = contentWidth - elementWidth;
				if (scrollableWidth <= 0.0f)
					return;

				auto childrenRenderOffsetX = contentNode->getChildrenRenderOffsetX() - 1.0f;
				if (childrenRenderOffsetX < 0.0f)
					childrenRenderOffsetX = 0.0f;

				contentNode->setChildrenRenderOffsetX(childrenRenderOffsetX);
			} else
			if (node == rightArrowNode) {
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
		 * @param guiScrollAreaHorizontalController gui scroll area horizontal controller
		 * @param leftArrowNode left arrow node
		 * @param contentNode content node
		 * @param rightArrowNode right arrow node
		 */
		GUIScrollAreaHorizontalControllerActionListener(GUIScrollAreaHorizontalController* guiScrollAreaHorizontalController, GUIElementNode* leftArrowNode, GUIParentNode* contentNode, GUIElementNode* rightArrowNode)
			: guiScrollAreaHorizontalController(guiScrollAreaHorizontalController)
			, leftArrowNode(leftArrowNode)
			, contentNode(contentNode)
			, rightArrowNode(rightArrowNode) {
		}

	private:
		GUIScrollAreaHorizontalController *guiScrollAreaHorizontalController;
		GUIElementNode* leftArrowNode;
		GUIParentNode* contentNode;
		GUIElementNode* rightArrowNode;
	};


	auto const contentNode = dynamic_cast< GUIParentNode* >(node->getScreenNode()->getNodeById(node->getId() + "_inner"));
	auto const leftArrowNode = dynamic_cast< GUIElementNode* >(node->getScreenNode()->getNodeById(node->getId() + "_scrollbar_horizontal_layout_left"));
	auto const rightArrowNode = dynamic_cast< GUIElementNode* >(node->getScreenNode()->getNodeById(node->getId() + "_scrollbar_horizontal_layout_right"));
	node->getScreenNode()->addActionListener(actionListener = new GUIScrollAreaHorizontalControllerActionListener(this, leftArrowNode, contentNode, rightArrowNode));
}

void GUIScrollAreaHorizontalController::dispose()
{
	if (actionListener != nullptr) {
		node->getScreenNode()->removeActionListener(actionListener);
		delete actionListener;
		actionListener = nullptr;
	}
}

void GUIScrollAreaHorizontalController::postLayout()
{
}

void GUIScrollAreaHorizontalController::handleMouseEvent(GUINode* node, GUIMouseEvent* event)
{
}

void GUIScrollAreaHorizontalController::handleKeyboardEvent(GUINode* node, GUIKeyboardEvent* event)
{
}

void GUIScrollAreaHorizontalController::tick()
{
}

void GUIScrollAreaHorizontalController::onFocusGained()
{
}

void GUIScrollAreaHorizontalController::onFocusLost()
{
}

bool GUIScrollAreaHorizontalController::hasValue()
{
	return false;
}

const MutableString& GUIScrollAreaHorizontalController::getValue()
{
	return value;
}

void GUIScrollAreaHorizontalController::setValue(const MutableString& value)
{
}

