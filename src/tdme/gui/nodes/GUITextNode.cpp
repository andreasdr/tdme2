#include <tdme/gui/nodes/GUITextNode.h>

#include <string>
#include <vector>

#include <tdme/gui/GUI.h>
#include <tdme/gui/nodes/GUIColor.h>
#include <tdme/gui/nodes/GUINode_Border.h>
#include <tdme/gui/nodes/GUINode_ComputedConstraints.h>
#include <tdme/gui/nodes/GUINode_Padding.h>
#include <tdme/gui/nodes/GUINode_Scale9Grid.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/gui/renderer/GUIFont.h>
#include <tdme/utilities/Exception.h>
#include <tdme/utilities/MutableString.h>

using std::vector;
using std::string;

using tdme::gui::nodes::GUITextNode;
using tdme::gui::GUI;
using tdme::gui::nodes::GUIColor;
using tdme::gui::nodes::GUINode_Border;
using tdme::gui::nodes::GUINode_ComputedConstraints;
using tdme::gui::nodes::GUINode_Padding;
using tdme::gui::nodes::GUINode_Scale9Grid;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::renderer::GUIFont;
using tdme::utilities::Exception;
using tdme::utilities::MutableString;

GUITextNode::GUITextNode(
	GUIScreenNode* screenNode,
	GUIParentNode* parentNode,
	const string& id,
	GUINode_Flow* flow,
	const GUINode_Alignments& alignments,
	const GUINode_RequestedConstraints& requestedConstraints,
	const GUIColor& backgroundColor,
	const string& backgroundImage,
	const GUINode_Scale9Grid& backgroundImageScale9Grid,
	const GUIColor& backgroundImageEffectColorMul,
	const GUIColor& backgroundImageEffectColorAdd,
	const GUINode_Border& border,
	const GUINode_Padding& padding,
	const GUINodeConditions& showOn,
	const GUINodeConditions& hideOn,
	const string& font,
	const string& color,
	const MutableString& text
):
	GUINode(screenNode, parentNode, id, flow, alignments, requestedConstraints, backgroundColor, backgroundImage, backgroundImageScale9Grid, backgroundImageEffectColorMul, backgroundImageEffectColorAdd, border, padding, showOn, hideOn)
{
	this->font = font.empty() == true?nullptr:GUI::getFont(screenNode->getApplicationRootPath(), font);
	this->color = color.empty() == true || color.length() == 0?GUIColor():GUIColor(color);
	this->text.set(text);
	if (this->font != nullptr) this->font->initialize();
}

const string GUITextNode::getNodeType()
{
	return "text";
}

bool GUITextNode::isContentNode()
{
	return true;
}

int GUITextNode::getContentWidth()
{
	return font != nullptr?font->getTextWidth(text) + border.left + border.right + padding.left + padding.right:0;
}

int GUITextNode::getContentHeight()
{
	return font != nullptr?font->getTextHeight(text) + border.top + border.bottom + padding.top + padding.bottom:0;
}

void GUITextNode::setText(const MutableString& text) {
	this->text = text;
	screenNode->layout(this);
}

void GUITextNode::dispose()
{
	if (font != nullptr) font->dispose();
	GUINode::dispose();
}

void GUITextNode::render(GUIRenderer* guiRenderer)
{
	if (shouldRender() == false) return;

	GUINode::render(guiRenderer);
	if (font != nullptr) {
		font->drawString(
			guiRenderer,
			computedConstraints.left + computedConstraints.alignmentLeft + computedConstraints.contentAlignmentLeft,
			computedConstraints.top + computedConstraints.alignmentTop + computedConstraints.contentAlignmentTop,
			text,
			0,
			0,
			color
		);
	}
}

