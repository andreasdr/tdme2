#include <agui/gui/nodes/GUINode.h>

#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

#include <agui/agui.h>
#include <agui/gui/effects/GUIColorEffect.h>
#include <agui/gui/effects/GUIEffect.h>
#include <agui/gui/effects/GUIPositionEffect.h>
#include <agui/gui/events/GUIMouseEvent.h>
#include <agui/gui/nodes/GUIColor.h>
#include <agui/gui/nodes/GUIElementNode.h>
#include <agui/gui/nodes/GUINode_Alignments.h>
#include <agui/gui/nodes/GUINode_AlignmentHorizontal.h>
#include <agui/gui/nodes/GUINode_AlignmentVertical.h>
#include <agui/gui/nodes/GUINode_Border.h>
#include <agui/gui/nodes/GUINode_ComputedConstraints.h>
#include <agui/gui/nodes/GUINode_Flow.h>
#include <agui/gui/nodes/GUINode_Padding.h>
#include <agui/gui/nodes/GUINode_RequestedConstraints.h>
#include <agui/gui/nodes/GUINode_RequestedConstraints_RequestedConstraintsType.h>
#include <agui/gui/nodes/GUINode_Scale9Grid.h>
#include <agui/gui/nodes/GUINodeConditions.h>
#include <agui/gui/nodes/GUINodeController.h>
#include <agui/gui/nodes/GUIParentNode.h>
#include <agui/gui/nodes/GUIParentNode_Overflow.h>
#include <agui/gui/nodes/GUIScreenNode.h>
#include <agui/gui/renderer/GUIRenderer.h>
#include <agui/gui/textures/GUITexture.h>
#include <agui/gui/textures/GUITextureManager.h>
#include <agui/gui/GUI.h>
#include <agui/math/Math.h>
#include <agui/math/Vector2.h>
#include <agui/utilities/Action.h>
#include <agui/utilities/Console.h>
#include <agui/utilities/Integer.h>
#include <agui/utilities/StringTokenizer.h>
#include <agui/utilities/StringTools.h>

using std::array;
using std::find;
using std::make_unique;
using std::string;
using std::to_string;
using std::unique_ptr;
using std::unordered_set;
using std::vector;

using agui::gui::nodes::GUINode;

using agui::gui::effects::GUIColorEffect;
using agui::gui::effects::GUIEffect;
using agui::gui::effects::GUIPositionEffect;
using agui::gui::events::GUIMouseEvent;
using agui::gui::nodes::GUIColor;
using agui::gui::nodes::GUIElementNode;
using agui::gui::nodes::GUINode_Alignments;
using agui::gui::nodes::GUINode_AlignmentHorizontal;
using agui::gui::nodes::GUINode_AlignmentVertical;
using agui::gui::nodes::GUINode_Border;
using agui::gui::nodes::GUINode_ComputedConstraints;
using agui::gui::nodes::GUINode_Flow;
using agui::gui::nodes::GUINode_Padding;
using agui::gui::nodes::GUINode_RequestedConstraints;
using agui::gui::nodes::GUINode_RequestedConstraints_RequestedConstraintsType;
using agui::gui::nodes::GUINode_Scale9Grid;
using agui::gui::nodes::GUINodeConditions;
using agui::gui::nodes::GUINodeController;
using agui::gui::nodes::GUIParentNode;
using agui::gui::nodes::GUIParentNode_Overflow;
using agui::gui::nodes::GUIScreenNode;
using agui::gui::renderer::GUIRenderer;
using agui::gui::textures::GUITexture;
using agui::gui::textures::GUITextureManager;
using agui::gui::GUI;
using agui::math::Math;
using agui::math::Vector2;
using agui::utilities::Action;
using agui::utilities::Console;
using agui::utilities::Integer;
using agui::utilities::StringTokenizer;
using agui::utilities::StringTools;

GUINode::GUINode(
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
	const string& tooltip)
{
	this->screenNode = screenNode;
	this->parentNode = parentNode;
	this->id = id;
	this->flow = flow;
	this->alignments = alignments;
	this->requestedConstraints = requestedConstraints;
	this->computedConstraints.alignmentLeft = 0;
	this->computedConstraints.alignmentTop = 0;
	this->computedConstraints.contentAlignmentLeft = 0;
	this->computedConstraints.contentAlignmentTop = 0;
	this->backgroundColor = backgroundColor;
	this->backgroundTexture = nullptr;
	this->backgroundTextureId = 0;
	this->setBackgroundImage(backgroundImage);
	this->backgroundImageScale9Grid = backgroundImageScale9Grid;
	this->backgroundImageEffectColorMul = backgroundImageEffectColorMul;
	this->backgroundImageEffectColorAdd = backgroundImageEffectColorAdd;
	this->border = border;
	this->padding = padding;
	this->showOn = showOn;
	this->hideOn = hideOn;
	this->tooltip = tooltip;
	this->guiEffectOffsetX = 0;
	this->guiEffectOffsetY = 0;
	this->conditionsMet = false;
	this->layouted = false;
	this->haveOutEffect = false;
	// register this id with related element nodes
	vector<string> elementNodeDependencies;
	cfDetermineElementNodeDependencies(elementNodeDependencies);
	for (const auto& elementNodeId: elementNodeDependencies) screenNode->addNodeElementNodeDependency(elementNodeId, id);
}

GUINode::~GUINode() {
	// remove effects
	vector<string> effectsToRemove;
	for (const auto& [effectId, effect]: effects) {
		effectsToRemove.push_back(effectId);
	}
	for (const auto& effectToRemoveId: effectsToRemove) {
		removeEffect(effectToRemoveId);
	}
}

const string GUINode::getHierarchicalId() {
	string hierarchicalId;
	auto _parentNode = parentNode;
	while (_parentNode != nullptr) {
		hierarchicalId = _parentNode->id + "." + hierarchicalId;
		_parentNode = _parentNode->parentNode;
	}
	hierarchicalId = hierarchicalId + id;
	return hierarchicalId;
}

int GUINode::getAutoWidth()
{
	if (requestedConstraints.widthType == GUINode_RequestedConstraints_RequestedConstraintsType::AUTO) {
		return getContentWidth();
	} else {
		return computedConstraints.width;
	}
}

int GUINode::getAutoHeight()
{
	if (requestedConstraints.heightType == GUINode_RequestedConstraints_RequestedConstraintsType::AUTO) {
		return getContentHeight();
	} else {
		return computedConstraints.height;
	}
}

void GUINode::setLeft(int left)
{
	if (requestedConstraints.leftType == GUINode_RequestedConstraints_RequestedConstraintsType::PIXEL) left = requestedConstraints.left;
	computedConstraints.left = left;
}

void GUINode::setTop(int top)
{
	if (requestedConstraints.topType == GUINode_RequestedConstraints_RequestedConstraintsType::PIXEL) top = requestedConstraints.top;
	computedConstraints.top = top;
}

void GUINode::layout()
{
	if (conditionsMet == false) {
		computedConstraints = GUINode_ComputedConstraints();
		return;
	}
	auto parentNodeContentWidth = parentNode->computedConstraints.width - parentNode->border.left - parentNode->border.right - parentNode->padding.left - parentNode->padding.right;
	auto parentNodeContentHeight = parentNode->computedConstraints.height - parentNode->border.top - parentNode->border.bottom - parentNode->padding.top - parentNode->padding.bottom;
	computedConstraints.left = parentNode->computedConstraints.left + layoutConstraintPixel(requestedConstraints.leftType, 0, parentNodeContentWidth, requestedConstraints.left);
	computedConstraints.top = parentNode->computedConstraints.top + layoutConstraintPixel(requestedConstraints.topType, 0, parentNodeContentHeight, requestedConstraints.top);
	computedConstraints.width = layoutConstraintPixel(requestedConstraints.widthType, getAutoWidth(), parentNodeContentWidth, requestedConstraints.width);
	computedConstraints.height = layoutConstraintPixel(requestedConstraints.heightType, getAutoHeight(), parentNodeContentHeight, requestedConstraints.height);
	computeContentAlignment();
	layouted = true;
}

void GUINode::computeContentAlignment()
{
	if (isContentNode() == true) {
		{
			auto v = alignments.horizontal;
			if (v == GUINode_AlignmentHorizontal::LEFT) {
				computedConstraints.contentAlignmentLeft = border.left + padding.left;
			} else
			if (v == GUINode_AlignmentHorizontal::CENTER) {
				computedConstraints.contentAlignmentLeft = (computedConstraints.width - getContentWidth()) / 2 + border.left + padding.left;
			} else
			if (v == GUINode_AlignmentHorizontal::RIGHT) {
				computedConstraints.contentAlignmentLeft = computedConstraints.width - getContentWidth() + border.left + padding.left;
			}
		}

		{
			auto v = alignments.vertical;
			if (v == GUINode_AlignmentVertical::TOP) {
				computedConstraints.contentAlignmentTop = border.top + padding.top;
			} else
			if (v == GUINode_AlignmentVertical::CENTER) {
				computedConstraints.contentAlignmentTop = (computedConstraints.height - getContentHeight()) / 2 + border.top + padding.top;
			} else
			if (v == GUINode_AlignmentVertical::BOTTOM) {
				computedConstraints.contentAlignmentTop = computedConstraints.height - getContentHeight() + border.top + padding.top;
			}
		}

	}
}

int GUINode::layoutConstraintPixel(GUINode_RequestedConstraints_RequestedConstraintsType* type, int autoValue, int parentValue, int value)
{
	if (type->equals(GUINode_RequestedConstraints_RequestedConstraintsType::NONE)) {
		return 0;
	} else
	if (type->equals(GUINode_RequestedConstraints_RequestedConstraintsType::PIXEL)) {
		return value;
	} else
	if (type->equals(GUINode_RequestedConstraints_RequestedConstraintsType::PERCENT)) {
		return static_cast<int>((parentValue / 100.0 * value));
	} else
	if (type->equals(GUINode_RequestedConstraints_RequestedConstraintsType::AUTO)) {
		return autoValue;
	} else
	if (type->equals(GUINode_RequestedConstraints_RequestedConstraintsType::STAR)) {
		return value;
	} else
	if (type->equals(GUINode_RequestedConstraints_RequestedConstraintsType::TABLECELL)) {
		return value;
	}
	return -1;
}

GUINode_Alignments GUINode::createAlignments(const string& horizontal, const string& vertical)
{
	GUINode_Alignments alignments;
	alignments.horizontal = GUINode_AlignmentHorizontal::valueOf(horizontal.empty() == false && horizontal.length() > 0?StringTools::toUpperCase(horizontal):"LEFT");
	alignments.vertical = GUINode_AlignmentVertical::valueOf(vertical.empty() == false && vertical.length() > 0?StringTools::toUpperCase(vertical):"TOP");
	return alignments;
}

GUINode_RequestedConstraints GUINode::createRequestedConstraints(const string& left, const string& top, const string& width, const string& height, int factor)
{
	GUINode_RequestedConstraints constraints;
	constraints.leftType = getRequestedConstraintsType(StringTools::trim(left), GUINode_RequestedConstraints_RequestedConstraintsType::NONE);
	constraints.left = getRequestedConstraintsValue(StringTools::trim(left), 0);
	constraints.topType = getRequestedConstraintsType(StringTools::trim(top), GUINode_RequestedConstraints_RequestedConstraintsType::NONE);
	constraints.top = getRequestedConstraintsValue(StringTools::trim(top), 0);
	constraints.widthType = getRequestedConstraintsType(StringTools::trim(width), GUINode_RequestedConstraints_RequestedConstraintsType::AUTO);
	constraints.width = getRequestedConstraintsValue(StringTools::trim(width), -1);
	constraints.heightType = getRequestedConstraintsType(StringTools::trim(height), GUINode_RequestedConstraints_RequestedConstraintsType::AUTO);
	constraints.height = getRequestedConstraintsValue(StringTools::trim(height), -1);
	if (constraints.leftType == GUINode_RequestedConstraints_RequestedConstraintsType::PIXEL) constraints.left*= factor;
	if (constraints.topType == GUINode_RequestedConstraints_RequestedConstraintsType::PIXEL) constraints.top*= factor;
	if (constraints.widthType == GUINode_RequestedConstraints_RequestedConstraintsType::PIXEL) constraints.width*= factor;
	if (constraints.heightType == GUINode_RequestedConstraints_RequestedConstraintsType::PIXEL) constraints.height*= factor;
	return constraints;
}

GUINode_RequestedConstraints_RequestedConstraintsType* GUINode::getRequestedConstraintsType(const string& constraint, GUINode_RequestedConstraints_RequestedConstraintsType* defaultConstraintsType)
{
	if (constraint.empty() == true || constraint.length() == 0) {
		return defaultConstraintsType;
	} else
	if (constraint.compare("auto") == 0) {
		return GUINode_RequestedConstraints_RequestedConstraintsType::AUTO;
	} else
	if (constraint.compare("*") == 0) {
		return GUINode_RequestedConstraints_RequestedConstraintsType::STAR;
	} else
	if (StringTools::endsWith(constraint, "%")) {
		return GUINode_RequestedConstraints_RequestedConstraintsType::PERCENT;
	} else {
		return GUINode_RequestedConstraints_RequestedConstraintsType::PIXEL;
	}
}

int GUINode::getRequestedConstraintsValue(const string& constraint, int defaultConstraintsValue)
{
	if (constraint.empty() == true || constraint.length() == 0) {
		return defaultConstraintsValue;
	} else
	if (constraint.compare("auto") == 0) {
		return -1;
	} else
	if (constraint.compare("*") == 0) {
		return -1;
	} else
	if (StringTools::endsWith(constraint, "%")) {
		return (Integer::parse(constraint.substr(0, constraint.length() - 1)));
	} else {
		return (Integer::parse(constraint));
	}
}

int GUINode::getRequestedPixelValue(const string& value, int defaultValue)
{
	if (value.empty() == true || value.length() == 0) {
		return defaultValue;
	} else {
		return (Integer::parse(value));
	}
}

GUIColor GUINode::getRequestedColor(const string& color, const GUIColor& defaultColor)
{
	if (color.empty() == true || color.length() == 0) {
		return defaultColor;
	} else {
		return GUIColor(color);
	}
}

GUINode_Flow* GUINode::createFlow(const string& flow)
{
	return GUINode_Flow::valueOf(flow.empty() == false && flow.length() > 0 ? StringTools::toUpperCase(flow) : "INTEGRATED");
}

GUINode_Border GUINode::createBorder(const string& allBorder, const string& left, const string& top, const string& right, const string& bottom, const string& allBorderColor, const string& leftColor, const string& topColor, const string& rightColor, const string& bottomColor)
{
	GUINode_Border border;
	border.left = getRequestedPixelValue(allBorder, 0);
	border.top = getRequestedPixelValue(allBorder, 0);
	border.right = getRequestedPixelValue(allBorder, 0);
	border.bottom = getRequestedPixelValue(allBorder, 0);
	border.left = getRequestedPixelValue(left, border.left);
	border.top = getRequestedPixelValue(top, border.top);
	border.right = getRequestedPixelValue(right, border.right);
	border.bottom = getRequestedPixelValue(bottom, border.bottom);
	border.leftColor = getRequestedColor(allBorderColor, GUIColor::GUICOLOR_BLACK);
	border.topColor = getRequestedColor(allBorderColor, GUIColor::GUICOLOR_BLACK);
	border.rightColor = getRequestedColor(allBorderColor, GUIColor::GUICOLOR_BLACK);
	border.bottomColor = getRequestedColor(allBorderColor, GUIColor::GUICOLOR_BLACK);
	border.leftColor = getRequestedColor(leftColor, border.leftColor);
	border.topColor = getRequestedColor(topColor, border.topColor);
	border.rightColor = getRequestedColor(rightColor, border.rightColor);
	border.bottomColor = getRequestedColor(bottomColor, border.bottomColor);
	return border;
}

GUINode_Padding GUINode::createPadding(const string& allPadding, const string& left, const string& top, const string& right, const string& bottom)
{
	GUINode_Padding padding;
	padding.left = getRequestedPixelValue(allPadding, 0);
	padding.top = getRequestedPixelValue(allPadding, 0);
	padding.right = getRequestedPixelValue(allPadding, 0);
	padding.bottom = getRequestedPixelValue(allPadding, 0);
	padding.left = getRequestedPixelValue(left, padding.left);
	padding.top = getRequestedPixelValue(top, padding.top);
	padding.right = getRequestedPixelValue(right, padding.right);
	padding.bottom = getRequestedPixelValue(bottom, padding.bottom);
	return padding;
}

GUINode_Scale9Grid GUINode::createScale9Grid(const string& all, const string& left, const string& top, const string& right, const string& bottom)
{
	GUINode_Scale9Grid scale9Grid;
	scale9Grid.left = getRequestedPixelValue(all, 0);
	scale9Grid.top = getRequestedPixelValue(all, 0);
	scale9Grid.right = getRequestedPixelValue(all, 0);
	scale9Grid.bottom = getRequestedPixelValue(all, 0);
	scale9Grid.left = getRequestedPixelValue(left, scale9Grid.left);
	scale9Grid.top = getRequestedPixelValue(top, scale9Grid.top);
	scale9Grid.right = getRequestedPixelValue(right, scale9Grid.right);
	scale9Grid.bottom = getRequestedPixelValue(bottom, scale9Grid.bottom);
	return scale9Grid;
}

GUINodeConditions GUINode::createConditions(const string& conditions)
{
	auto arguments = 0;
	GUINodeConditions guiNodeConditions;
	string condition;
	for (auto i = 0; i < conditions.size(); i++) {
		auto c = conditions[i];
		if (c == '(') {
			condition+= c;
			arguments++;
		} else
		if (c == ')') {
			condition+= c;
			arguments--;
		} else
		if (arguments == 0 && c == ',') {
			guiNodeConditions.add(StringTools::trim(condition));
			condition.clear();
		} else {
			condition+= c;
		}
	}
	if (condition.empty() == false) {
		guiNodeConditions.add(StringTools::trim(condition));
		condition.clear();
	}
	return guiNodeConditions;
}

bool GUINode::checkConditions()
{
	const auto& showOn = this->showOn.conditions;
	const auto& hideOn = this->hideOn.conditions;

	if (showOn.empty() == true && hideOn.empty() == true) return true;

	for (auto i = 0; i < showOn.size(); i++) {
		if (showOn[i] == GUIElementNode::CONDITION_ALWAYS)
			return true;

	}
	for (auto i = 0; i < hideOn.size(); i++) {
		if (hideOn[i] == GUIElementNode::CONDITION_ALWAYS)
			return false;

	}

	GUIElementNode* elementNode = nullptr;
	for (GUINode* node = parentNode; node != nullptr && (elementNode = dynamic_cast<GUIElementNode*>(node)) == nullptr; node = node->parentNode);

	string function;
	vector<string> arguments;
	for (auto i = 0; i < hideOn.size(); i++) {
		auto conditionTerm = hideOn[i];
		cfParse(hideOn[i], function, arguments);
		if (cfCall(elementNode, function, arguments) == true) return false;
	}
	for (auto i = 0; i < showOn.size(); i++) {
		cfParse(showOn[i], function, arguments);
		if (cfCall(elementNode, function, arguments) == true) return true;
	}

	return showOn.empty() == true;
}

void GUINode::dispose()
{
	if (backgroundTexture != nullptr) GUI::getTextureManager()->removeTexture(backgroundTexture->getId());
	if (controller != nullptr) controller->dispose();
}

void GUINode::setConditionsMet()
{
	conditionsMet = checkConditions();
}

void GUINode::layoutOnDemand() {
	if (conditionsMet == false || layouted == true) return;
	if (conditionsMet == true && layouted == false) {
		screenNode->forceLayout(this);
	}
}

void GUINode::applyEffects(GUIRenderer* guiRenderer) {
	if (hasEffects() == false) return;
	vector<Action*> actions;
	vector<GUIEffect*> activeEffects;
	for (const auto& [effectId, effect]: effects) {
		if (effect->isActive() == true) {
			if (effect->update(screenNode->getGUI()) == true && effect->getAction() != nullptr) {
				actions.push_back(effect->getAction());
			} else {
				activeEffects.push_back(effect);
			}
		}
	}
	for (auto action: actions) action->performAction();
	if (activeEffects.empty() == true) {
		auto defaultEffect = getEffect("tdme.xmleffect.default");
		if (defaultEffect != nullptr) {
			defaultEffect->start();
			activeEffects.push_back(defaultEffect);
		}
	}
	guiRenderer->pushEffects(activeEffects);
}

void GUINode::undoEffects(GUIRenderer* guiRenderer) {
	if (hasEffects() == false) return;
	guiRenderer->popEffects();
}

void GUINode::render(GUIRenderer* guiRenderer)
{
	layoutOnDemand();

	if (shouldRender() == false) return;

	// floating nodes should always be in screen constraints
	// if this does not seem to be feasible we can add a property for this
	if (flow == GUINode_Flow::FLOATING) {
		if (computedConstraints.left < 0) {
			setLeft(0);
		} else
		if (computedConstraints.left + computedConstraints.width > screenNode->getScreenWidth()) {
			setLeft(screenNode->getScreenWidth() - computedConstraints.width);
		}
		if (computedConstraints.top < 0) {
			setTop(0);
		} else
		if (computedConstraints.top + computedConstraints.height > screenNode->getScreenHeight()) {
			setTop(screenNode->getScreenHeight() - computedConstraints.height);
		}
	}

	//
	auto screenWidth = screenNode->getScreenWidth();
	auto screenHeight = screenNode->getScreenHeight();
	float left = computedConstraints.left + computedConstraints.alignmentLeft + border.left;
	float top = computedConstraints.top + computedConstraints.alignmentTop + border.top;
	float width = computedConstraints.width - border.left - border.right;
	float height = computedConstraints.height - border.top - border.bottom;
	if (!backgroundColor.equals(GUIColor::GUICOLOR_TRANSPARENT)) {
		auto bgColorData = &backgroundColor.getArray();
		guiRenderer->addQuad(
			((left) / (screenWidth / 2.0f)) - 1.0f,
			((screenHeight - top) / (screenHeight / 2.0f)) - 1.0f,
			(*bgColorData)[0],
			(*bgColorData)[1],
			(*bgColorData)[2],
			(*bgColorData)[3],
			0.0f,
			1.0f,
			((left + width) / (screenWidth / 2.0f)) - 1.0f,
			((screenHeight - top) / (screenHeight / 2.0f)) - 1.0f,
			(*bgColorData)[0],
			(*bgColorData)[1],
			(*bgColorData)[2],
			(*bgColorData)[3],
			1.0f,
			1.0f,
			((left + width) / (screenWidth / 2.0f)) - 1.0f,
			((screenHeight - top - height) / (screenHeight / 2.0f)) - 1.0f,
			(*bgColorData)[0],
			(*bgColorData)[1],
			(*bgColorData)[2],
			(*bgColorData)[3],
			1.0f,
			0.0f,
			((left) / (screenWidth / 2.0f)) - 1.0f,
			((screenHeight - top - height) / (screenHeight / 2.0f)) - 1.0f,
			(*bgColorData)[0],
			(*bgColorData)[1],
			(*bgColorData)[2],
			(*bgColorData)[3],
			0.0f,
			0.0f
		);
		guiRenderer->render();
	}
	if (backgroundTexture != nullptr) {
		guiRenderer->bindTexture(backgroundTextureId);
		guiRenderer->setEffectColorMul(backgroundImageEffectColorMul);
		guiRenderer->setEffectColorAdd(backgroundImageEffectColorAdd);
		GUINode_Scale9Grid& scale9Grid = backgroundImageScale9Grid;
		if (scale9Grid.left == 0 &&
			scale9Grid.right == 0 &&
			scale9Grid.top == 0 &&
			scale9Grid.bottom == 0) {
			float left = computedConstraints.left + computedConstraints.alignmentLeft + border.left;
			float top = computedConstraints.top + computedConstraints.alignmentTop + border.top;
			float width = computedConstraints.width - border.left - border.right;
			float height = computedConstraints.height - border.top - border.bottom;
			guiRenderer->addQuad(
				((left) / (screenWidth / 2.0f)) - 1.0f,
				((screenHeight - top) / (screenHeight / 2.0f)) - 1.0f,
				1.0f, 1.0f, 1.0f, 1.0f,
				0.0f,
				0.0f,
				((left + width) / (screenWidth / 2.0f)) - 1.0f,
				((screenHeight - top) / (screenHeight / 2.0f)) - 1.0f,
				1.0f, 1.0f, 1.0f, 1.0f,
				1.0f,
				0.0f,
				((left + width) / (screenWidth / 2.0f)) - 1.0f,
				((screenHeight - top - height) / (screenHeight / 2.0f)) - 1.0f,
				1.0f, 1.0f, 1.0f, 1.0f,
				1.0f,
				1.0f,
				((left) / (screenWidth / 2.0f)) - 1.0f,
				((screenHeight - top - height) / (screenHeight / 2.0f)) - 1.0f,
				1.0f, 1.0f, 1.0f, 1.0f,
				0.0f,
				1.0f
			);
		} else {
			auto scaleX = 1.0f;
			auto scaleY = 1.0f;
			// we have a scale here, because we have a axis without "scale9grid"
			if (scale9Grid.top == 0 && scale9Grid.bottom == 0) {
				scaleY = (float)getContentHeight() / (float)backgroundTexture->getHeight();
				scaleX = scaleY;
			} else
			// we have a scale here, because we have a axis without "scale9grid"
			if (scale9Grid.left == 0 && scale9Grid.right == 0) {
				scaleX = (float)getContentWidth() / (float)backgroundTexture->getWidth();
				scaleY = scaleX;
			} else {
				// scale Y if content height is too small to fit scale 9 top and bottom
				if (getContentHeight() < scale9Grid.top + scale9Grid.bottom) {
					scaleY = getContentHeight() < Math::EPSILON?0.0f:(float)getContentHeight() / (float)(scale9Grid.top + scale9Grid.bottom);
				}
				// scale X if content width is too small to fit scale 9 left and top
				if (getContentWidth() < scale9Grid.left + scale9Grid.right) {
					scaleX = getContentWidth() < Math::EPSILON?0.0f:(float)getContentWidth() / (float)(scale9Grid.left + scale9Grid.right);
				}
			}
			// we have no certain scale, take original image size
			GUINode_Scale9Grid scaledScale9Grid;
			scaledScale9Grid.left = static_cast<int>(scale9Grid.left * scaleX);
			scaledScale9Grid.right = static_cast<int>(scale9Grid.right * scaleX);
			scaledScale9Grid.top = static_cast<int>(scale9Grid.top * scaleY);
			scaledScale9Grid.bottom = static_cast<int>(scale9Grid.bottom * scaleY);
			float imageScreenLeft = computedConstraints.left + computedConstraints.alignmentLeft + border.left;;
			float imageScreenTop = computedConstraints.top + computedConstraints.alignmentTop + border.top;;
			float imageScreenScale9Left = imageScreenLeft + scaledScale9Grid.left;
			float imageScreenScale9Top = imageScreenTop + scaledScale9Grid.top;
			float imageScreenWidth = computedConstraints.width - border.left - border.right;
			float imageScreenHeight = computedConstraints.height - border.top - border.bottom;
			float imageScreenScale9Right = imageScreenLeft + imageScreenWidth - scaledScale9Grid.right;
			float imageScreenScale9Bottom = imageScreenTop + imageScreenHeight - scaledScale9Grid.bottom;
			float imageTextureWidth = backgroundTexture->getWidth();
			float imageTextureHeight = backgroundTexture->getHeight();
			// left top
			if (scaledScale9Grid.left > 0 && scaledScale9Grid.top > 0) {
				float left = imageScreenLeft;
				float top = imageScreenTop;
				float width = scaledScale9Grid.left;
				float height = scaledScale9Grid.top;
				guiRenderer->addQuad(
					((left) / (screenWidth / 2.0f)) - 1.0f,
					((screenHeight - top) / (screenHeight / 2.0f)) - 1.0f,
					1.0f, 1.0f, 1.0f, 1.0f,
					0.0f,
					0.0f,
					((left + width) / (screenWidth / 2.0f)) - 1.0f,
					((screenHeight - top) / (screenHeight / 2.0f)) - 1.0f,
					1.0f, 1.0f, 1.0f, 1.0f,
					1.0f / imageTextureWidth * scale9Grid.left,
					0.0f,
					((left + width) / (screenWidth / 2.0f)) - 1.0f,
					((screenHeight - top - height) / (screenHeight / 2.0f)) - 1.0f,
					1.0f, 1.0f, 1.0f, 1.0f,
					1.0f / imageTextureWidth * scale9Grid.left,
					1.0f / imageTextureHeight * scale9Grid.top,
					((left) / (screenWidth / 2.0f)) - 1.0f,
					((screenHeight - top - height) / (screenHeight / 2.0f)) - 1.0f,
					1.0f, 1.0f, 1.0f, 1.0f,
					0.0f,
					1.0f / imageTextureHeight * scale9Grid.top
				);
			}
			// middle top
			if (scaledScale9Grid.top > 0) {
				float left = imageScreenScale9Left;
				float top = imageScreenTop;
				float width = imageScreenWidth - scaledScale9Grid.left - scaledScale9Grid.right;
				float height = scaledScale9Grid.top;
				guiRenderer->addQuad(
					((left) / (screenWidth / 2.0f)) - 1.0f,
					((screenHeight - top) / (screenHeight / 2.0f)) - 1.0f,
					1.0f, 1.0f, 1.0f, 1.0f,
					1.0f / imageTextureWidth * scale9Grid.left,
					0.0f,
					((left + width) / (screenWidth / 2.0f)) - 1.0f,
					((screenHeight - top) / (screenHeight / 2.0f)) - 1.0f,
					1.0f, 1.0f, 1.0f, 1.0f,
					1.0f / imageTextureWidth * (imageTextureWidth - scale9Grid.right),
					0.0f,
					((left + width) / (screenWidth / 2.0f)) - 1.0f,
					((screenHeight - top - height) / (screenHeight / 2.0f)) - 1.0f,
					1.0f, 1.0f, 1.0f, 1.0f,
					1.0f / imageTextureWidth * (imageTextureWidth - scale9Grid.right),
					1.0f / imageTextureHeight * scale9Grid.top,
					((left) / (screenWidth / 2.0f)) - 1.0f,
					((screenHeight - top - height) / (screenHeight / 2.0f)) - 1.0f,
					1.0f, 1.0f, 1.0f, 1.0f,
					1.0f / imageTextureWidth * scale9Grid.left,
					1.0f / imageTextureHeight * scale9Grid.top
				);
			}
			// right top
			if (scaledScale9Grid.right > 0 && scaledScale9Grid.top > 0) {
				float left = imageScreenScale9Right;
				float top = imageScreenTop;
				float width = scaledScale9Grid.right;
				float height = scaledScale9Grid.top;
				guiRenderer->addQuad(
					((left) / (screenWidth / 2.0f)) - 1.0f,
					((screenHeight - top) / (screenHeight / 2.0f)) - 1.0f,
					1.0f, 1.0f, 1.0f, 1.0f,
					1.0f / imageTextureWidth * (imageTextureWidth - scale9Grid.right),
					0.0f,
					((left + width) / (screenWidth / 2.0f)) - 1.0f,
					((screenHeight - top) / (screenHeight / 2.0f)) - 1.0f,
					1.0f, 1.0f, 1.0f, 1.0f,
					1.0f,
					0.0f,
					((left + width) / (screenWidth / 2.0f)) - 1.0f,
					((screenHeight - top - height) / (screenHeight / 2.0f)) - 1.0f,
					1.0f, 1.0f, 1.0f, 1.0f,
					1.0f,
					1.0f / imageTextureHeight * scale9Grid.top,
					((left) / (screenWidth / 2.0f)) - 1.0f,
					((screenHeight - top - height) / (screenHeight / 2.0f)) - 1.0f,
					1.0f, 1.0f, 1.0f, 1.0f,
					1.0f / imageTextureWidth * (imageTextureWidth - scale9Grid.right),
					1.0f / imageTextureHeight * scale9Grid.top
				);
			}
			// right bottom
			if (scaledScale9Grid.right > 0 && scaledScale9Grid.bottom > 0) {
				float left = imageScreenScale9Right;
				float top = imageScreenScale9Bottom;
				float width = scaledScale9Grid.right;
				float height = scaledScale9Grid.bottom;
				guiRenderer->addQuad(
					((left) / (screenWidth / 2.0f)) - 1.0f,
					((screenHeight - top) / (screenHeight / 2.0f)) - 1.0f,
					1.0f, 1.0f, 1.0f, 1.0f,
					1.0f / imageTextureWidth * (imageTextureWidth - scale9Grid.right),
					1.0f / imageTextureHeight * (imageTextureHeight - scale9Grid.bottom),
					((left + width) / (screenWidth / 2.0f)) - 1.0f,
					((screenHeight - top) / (screenHeight / 2.0f)) - 1.0f,
					1.0f, 1.0f, 1.0f, 1.0f,
					1.0f,
					1.0f / imageTextureHeight * (imageTextureHeight - scale9Grid.bottom),
					((left + width) / (screenWidth / 2.0f)) - 1.0f,
					((screenHeight - top - height) / (screenHeight / 2.0f)) - 1.0f,
					1.0f, 1.0f, 1.0f, 1.0f,
					1.0f,
					1.0f,
					((left) / (screenWidth / 2.0f)) - 1.0f,
					((screenHeight - top - height) / (screenHeight / 2.0f)) - 1.0f,
					1.0f, 1.0f, 1.0f, 1.0f,
					1.0f / imageTextureWidth * (imageTextureWidth - scale9Grid.right),
					1.0f
				);
			}
			// middle bottom
			if (scaledScale9Grid.bottom > 0) {
				float left = imageScreenScale9Left;
				float top = imageScreenScale9Bottom;
				float width = imageScreenWidth - scaledScale9Grid.left - scaledScale9Grid.right;
				float height = scaledScale9Grid.bottom;
				guiRenderer->addQuad(
					((left) / (screenWidth / 2.0f)) - 1.0f,
					((screenHeight - top) / (screenHeight / 2.0f)) - 1.0f,
					1.0f, 1.0f, 1.0f, 1.0f,
					1.0f / imageTextureWidth * scale9Grid.left,
					1.0f / imageTextureHeight * (imageTextureHeight - scale9Grid.bottom),
					((left + width) / (screenWidth / 2.0f)) - 1.0f,
					((screenHeight - top) / (screenHeight / 2.0f)) - 1.0f,
					1.0f, 1.0f, 1.0f, 1.0f,
					1.0f / imageTextureWidth * (imageTextureWidth - scale9Grid.right),
					1.0f / imageTextureHeight * (imageTextureHeight - scale9Grid.bottom),
					((left + width) / (screenWidth / 2.0f)) - 1.0f,
					((screenHeight - top - height) / (screenHeight / 2.0f)) - 1.0f,
					1.0f, 1.0f, 1.0f, 1.0f,
					1.0f / imageTextureWidth * (imageTextureWidth - scale9Grid.right),
					1.0f,
					((left) / (screenWidth / 2.0f)) - 1.0f,
					((screenHeight - top - height) / (screenHeight / 2.0f)) - 1.0f,
					1.0f, 1.0f, 1.0f, 1.0f,
					1.0f / imageTextureWidth * scale9Grid.left,
					1.0f
				);
			}
			// left bottom
			if (scaledScale9Grid.left > 0 && scaledScale9Grid.bottom > 0) {
				float left = imageScreenLeft;
				float top = imageScreenScale9Bottom;
				float width = scaledScale9Grid.left;
				float height = scaledScale9Grid.bottom;
				guiRenderer->addQuad(
					((left) / (screenWidth / 2.0f)) - 1.0f,
					((screenHeight - top) / (screenHeight / 2.0f)) - 1.0f,
					1.0f, 1.0f, 1.0f, 1.0f,
					0.0f,
					1.0f / imageTextureHeight * (imageTextureHeight - scale9Grid.bottom),
					((left + width) / (screenWidth / 2.0f)) - 1.0f,
					((screenHeight - top) / (screenHeight / 2.0f)) - 1.0f,
					1.0f, 1.0f, 1.0f, 1.0f,
					1.0f / imageTextureWidth * scale9Grid.left,
					1.0f / imageTextureHeight * (imageTextureHeight - scale9Grid.bottom),
					((left + width) / (screenWidth / 2.0f)) - 1.0f,
					((screenHeight - top - height) / (screenHeight / 2.0f)) - 1.0f,
					1.0f, 1.0f, 1.0f, 1.0f,
					1.0f / imageTextureWidth * scale9Grid.left,
					1.0f,
					((left) / (screenWidth / 2.0f)) - 1.0f,
					((screenHeight - top - height) / (screenHeight / 2.0f)) - 1.0f,
					1.0f, 1.0f, 1.0f, 1.0f,
					0.0f,
					1.0f
				);
			}
			// left middle
			if (scaledScale9Grid.left > 0) {
				float left = imageScreenLeft;
				float top = imageScreenScale9Top;
				float width = scaledScale9Grid.left;
				float height = imageScreenHeight - scaledScale9Grid.top - scaledScale9Grid.bottom;
				guiRenderer->addQuad(
					((left) / (screenWidth / 2.0f)) - 1.0f,
					((screenHeight - top) / (screenHeight / 2.0f)) - 1.0f,
					1.0f, 1.0f, 1.0f, 1.0f,
					0.0f,
					1.0f / imageTextureHeight * scale9Grid.top,
					((left + width) / (screenWidth / 2.0f)) - 1.0f,
					((screenHeight - top) / (screenHeight / 2.0f)) - 1.0f,
					1.0f, 1.0f, 1.0f, 1.0f,
					1.0f / imageTextureWidth * scale9Grid.left,
					1.0f / imageTextureHeight * scale9Grid.top,
					((left + width) / (screenWidth / 2.0f)) - 1.0f,
					((screenHeight - top - height) / (screenHeight / 2.0f)) - 1.0f,
					1.0f, 1.0f, 1.0f, 1.0f,
					1.0f / imageTextureWidth * scale9Grid.left,
					1.0f / imageTextureHeight * (imageTextureHeight - scale9Grid.bottom),
					((left) / (screenWidth / 2.0f)) - 1.0f,
					((screenHeight - top - height) / (screenHeight / 2.0f)) - 1.0f,
					1.0f, 1.0f, 1.0f, 1.0f,
					0.0f,
					1.0f / imageTextureHeight * (imageTextureHeight - scale9Grid.bottom)
				);
			}
			// right middle
			if (scaledScale9Grid.right > 0) {
				float left = imageScreenScale9Right;
				float top = imageScreenScale9Top;
				float width = scaledScale9Grid.right;
				float height = imageScreenHeight - scaledScale9Grid.top - scaledScale9Grid.bottom;
				guiRenderer->addQuad(
					((left) / (screenWidth / 2.0f)) - 1.0f,
					((screenHeight - top) / (screenHeight / 2.0f)) - 1.0f,
					1.0f, 1.0f, 1.0f, 1.0f,
					1.0f / imageTextureWidth * (imageTextureWidth - scale9Grid.right),
					1.0f / imageTextureHeight * scale9Grid.top,
					((left + width) / (screenWidth / 2.0f)) - 1.0f,
					((screenHeight - top) / (screenHeight / 2.0f)) - 1.0f,
					1.0f, 1.0f, 1.0f, 1.0f,
					1.0f,
					1.0f / imageTextureHeight * scale9Grid.top,
					((left + width) / (screenWidth / 2.0f)) - 1.0f,
					((screenHeight - top - height) / (screenHeight / 2.0f)) - 1.0f,
					1.0f, 1.0f, 1.0f, 1.0f,
					1.0f,
					1.0f / imageTextureHeight * (imageTextureHeight - scale9Grid.bottom),
					((left) / (screenWidth / 2.0f)) - 1.0f,
					((screenHeight - top - height) / (screenHeight / 2.0f)) - 1.0f,
					1.0f, 1.0f, 1.0f, 1.0f,
					1.0f / imageTextureWidth * (imageTextureWidth - scale9Grid.right),
					1.0f / imageTextureHeight * (imageTextureHeight - scale9Grid.bottom)
				);
			}
			// center
			{
				float left = imageScreenScale9Left;
				float top = imageScreenScale9Top;
				float width = imageScreenWidth - scaledScale9Grid.left - scaledScale9Grid.right;
				float height = imageScreenHeight - scaledScale9Grid.top - scaledScale9Grid.bottom;
				guiRenderer->addQuad(
					((left) / (screenWidth / 2.0f)) - 1.0f,
					((screenHeight - top) / (screenHeight / 2.0f)) - 1.0f,
					1.0f, 1.0f, 1.0f, 1.0f,
					1.0f / imageTextureWidth * scale9Grid.left,
					1.0f / imageTextureHeight * scale9Grid.top,
					((left + width) / (screenWidth / 2.0f)) - 1.0f,
					((screenHeight - top) / (screenHeight / 2.0f)) - 1.0f,
					1.0f, 1.0f, 1.0f, 1.0f,
					1.0f / imageTextureWidth * (imageTextureWidth - scale9Grid.right),
					1.0f / imageTextureHeight * scale9Grid.top,
					((left + width) / (screenWidth / 2.0f)) - 1.0f,
					((screenHeight - top - height) / (screenHeight / 2.0f)) - 1.0f,
					1.0f, 1.0f, 1.0f, 1.0f,
					1.0f / imageTextureWidth * (imageTextureWidth - scale9Grid.right),
					1.0f / imageTextureHeight * (imageTextureHeight - scale9Grid.bottom),
					((left) / (screenWidth / 2.0f)) - 1.0f,
					((screenHeight - top - height) / (screenHeight / 2.0f)) - 1.0f,
					1.0f, 1.0f, 1.0f, 1.0f,
					1.0f / imageTextureWidth * scale9Grid.left,
					1.0f / imageTextureHeight * (imageTextureHeight - scale9Grid.bottom)
				);
			}
		}
		guiRenderer->render();
		guiRenderer->bindTexture(0);
	}
	if (border.top > 0 || border.left > 0 || border.right > 0 || border.bottom > 0) {
		if (border.top > 0) {
			float left = computedConstraints.left + computedConstraints.alignmentLeft;
			float top = computedConstraints.top + computedConstraints.alignmentTop;
			float width = computedConstraints.width;
			float height = border.top;
			const auto& borderColor = border.topColor;
			guiRenderer->addQuad(((left) / (screenWidth / 2.0f)) - 1.0f, ((screenHeight - top) / (screenHeight / 2.0f)) - 1.0f, borderColor[0], borderColor[1], borderColor[2], borderColor[3], 0.0f, 1.0f, ((left + width) / (screenWidth / 2.0f)) - 1.0f, ((screenHeight - top) / (screenHeight / 2.0f)) - 1.0f, borderColor[0], borderColor[1], borderColor[2], borderColor[3], 1.0f, 1.0f, ((left + width) / (screenWidth / 2.0f)) - 1.0f, ((screenHeight - top - height) / (screenHeight / 2.0f)) - 1.0f, borderColor[0], borderColor[1], borderColor[2], borderColor[3], 1.0f, 0.0f, ((left) / (screenWidth / 2.0f)) - 1.0f, ((screenHeight - top - height) / (screenHeight / 2.0f)) - 1.0f, borderColor[0], borderColor[1], borderColor[2], borderColor[3], 0.0f, 0.0f);
		}
		if (border.bottom > 0) {
			float left = computedConstraints.left + computedConstraints.alignmentLeft;
			float top = computedConstraints.top + computedConstraints.alignmentTop + computedConstraints.height - border.bottom;
			float width = computedConstraints.width;
			float height = border.bottom;
			const auto& borderColor = border.bottomColor;
			guiRenderer->addQuad(((left) / (screenWidth / 2.0f)) - 1.0f, ((screenHeight - top) / (screenHeight / 2.0f)) - 1.0f, borderColor[0], borderColor[1], borderColor[2], borderColor[3], 0.0f, 1.0f, ((left + width) / (screenWidth / 2.0f)) - 1.0f, ((screenHeight - top) / (screenHeight / 2.0f)) - 1.0f, borderColor[0], borderColor[1], borderColor[2], borderColor[3], 1.0f, 1.0f, ((left + width) / (screenWidth / 2.0f)) - 1.0f, ((screenHeight - top - height) / (screenHeight / 2.0f)) - 1.0f, borderColor[0], borderColor[1], borderColor[2], borderColor[3], 1.0f, 0.0f, ((left) / (screenWidth / 2.0f)) - 1.0f, ((screenHeight - top - height) / (screenHeight / 2.0f)) - 1.0f, borderColor[0], borderColor[1], borderColor[2], borderColor[3], 0.0f, 0.0f);
		}
		if (border.left > 0) {
			float left = computedConstraints.left + computedConstraints.alignmentLeft;
			float top = computedConstraints.top + computedConstraints.alignmentTop;
			float width = border.left;
			float height = computedConstraints.height;
			const auto& borderColor = border.leftColor;
			guiRenderer->addQuad(((left) / (screenWidth / 2.0f)) - 1.0f, ((screenHeight - top) / (screenHeight / 2.0f)) - 1.0f, borderColor[0], borderColor[1], borderColor[2], borderColor[3], 0.0f, 1.0f, ((left + width) / (screenWidth / 2.0f)) - 1.0f, ((screenHeight - top) / (screenHeight / 2.0f)) - 1.0f, borderColor[0], borderColor[1], borderColor[2], borderColor[3], 1.0f, 1.0f, ((left + width) / (screenWidth / 2.0f)) - 1.0f, ((screenHeight - top - height) / (screenHeight / 2.0f)) - 1.0f, borderColor[0], borderColor[1], borderColor[2], borderColor[3], 1.0f, 0.0f, ((left) / (screenWidth / 2.0f)) - 1.0f, ((screenHeight - top - height) / (screenHeight / 2.0f)) - 1.0f, borderColor[0], borderColor[1], borderColor[2], borderColor[3], 0.0f, 0.0f);
		}
		if (border.right > 0) {
			float left = computedConstraints.left + computedConstraints.alignmentLeft + computedConstraints.width - border.right;
			float top = computedConstraints.top + computedConstraints.alignmentTop;
			float width = border.right;
			float height = computedConstraints.height;
			const auto& borderColor = border.rightColor;
			guiRenderer->addQuad(((left) / (screenWidth / 2.0f)) - 1.0f, ((screenHeight - top) / (screenHeight / 2.0f)) - 1.0f, borderColor[0], borderColor[1], borderColor[2], borderColor[3], 0.0f, 1.0f, ((left + width) / (screenWidth / 2.0f)) - 1.0f, ((screenHeight - top) / (screenHeight / 2.0f)) - 1.0f, borderColor[0], borderColor[1], borderColor[2], borderColor[3], 1.0f, 1.0f, ((left + width) / (screenWidth / 2.0f)) - 1.0f, ((screenHeight - top - height) / (screenHeight / 2.0f)) - 1.0f, borderColor[0], borderColor[1], borderColor[2], borderColor[3], 1.0f, 0.0f, ((left) / (screenWidth / 2.0f)) - 1.0f, ((screenHeight - top - height) / (screenHeight / 2.0f)) - 1.0f, borderColor[0], borderColor[1], borderColor[2], borderColor[3], 0.0f, 0.0f);
		}
		guiRenderer->render();
	}
}

float GUINode::computeParentChildrenRenderOffsetXTotal() {
	auto childrenRenderOffSetX = 0.0f;
	auto parentNode = this->parentNode;
	while (parentNode != nullptr) {
		childrenRenderOffSetX += parentNode->getChildrenRenderOffsetX();
		parentNode = parentNode->parentNode;
	}
	return childrenRenderOffSetX;
}

float GUINode::computeParentChildrenRenderOffsetYTotal() {
	auto childrenRenderOffSetY = 0.0f;
	auto parentNode = this->parentNode;
	while (parentNode != nullptr) {
		childrenRenderOffSetY += parentNode->getChildrenRenderOffsetY();
		parentNode = parentNode->parentNode;
	}
	return childrenRenderOffSetY;
}

void GUINode::getEventOffNodeRelativePosition(GUIMouseEvent* event, Vector2& position)
{
	auto eventXScreen = event->getX();
	auto eventYScreen = event->getY();
	auto eventX = eventXScreen + computeParentChildrenRenderOffsetXTotal();
	auto eventY = eventYScreen + computeParentChildrenRenderOffsetYTotal();
	float left = computedConstraints.left + computedConstraints.alignmentLeft;
	float right = computedConstraints.left + computedConstraints.alignmentLeft + computedConstraints.width - 1;
	float top = computedConstraints.top + computedConstraints.alignmentTop;
	float bottom = computedConstraints.top + computedConstraints.alignmentTop + computedConstraints.height - 1;
	if (eventX < left) {
		position[0] = static_cast<int>((eventX - left));
	} else if (eventX > right) {
		position[0] = static_cast<int>((eventX - right));
	} else {
		position[0] = 0;
	}
	if (eventY < top) {
		position[1] = static_cast<int>((eventY - top));
	} else if (eventY > bottom) {
		position[1] = static_cast<int>((eventY - bottom));
	} else {
		position[1] = 0;
	}
}

void GUINode::getEventNodePosition(GUIMouseEvent* event, Vector2& position) {
	auto eventXScreen = event->getX();
	auto eventYScreen = event->getY();
	auto eventX = eventXScreen + computeParentChildrenRenderOffsetXTotal();
	auto eventY = eventYScreen + computeParentChildrenRenderOffsetYTotal();
	auto left = computedConstraints.left + computedConstraints.alignmentLeft;
	auto right = computedConstraints.left + computedConstraints.alignmentLeft + computedConstraints.width - 1;
	auto top = computedConstraints.top + computedConstraints.alignmentTop;
	auto bottom = computedConstraints.top + computedConstraints.alignmentTop + computedConstraints.height - 1;
	position[0] = Math::clamp(static_cast<int>(eventX), left, right) - left;
	position[1] = Math::clamp(static_cast<int>(eventY), top, bottom) - top;
}

GUIParentNode* GUINode::getParentControllerNode()
{
	auto node = this->parentNode;
	while (node != nullptr && node->controller == nullptr) {
		node = node->parentNode;
	}
	return node;
}

void GUINode::determineNodesByCoordinate(const Vector2& coordinate, unordered_set<string>& nodeIds) {
	if (conditionsMet == false)
		return;

	//
	if (isCoordinateBelongingToNode(coordinate) == true) {
		nodeIds.insert(id);
	}
}

void GUINode::determineMouseEventNodes(GUIMouseEvent* event, bool floatingNode, unordered_set<string>& eventNodeIds, unordered_set<string>& eventFloatingNodeIds, int flags)
{
	if (conditionsMet == false)
		return;

	// node belongs to event?
	if (isEventBelongingToNode(event) == true) {
		if ((flags & DETERMINEMOUSEEVENTNODES_FLAG_TOOLTIP) == DETERMINEMOUSEEVENTNODES_FLAG_TOOLTIP && tooltip.empty() == true) {
			// no op
		} else {
			// yep insert
			if (floatingNode == true || flow == GUINode_Flow::FLOATING) eventFloatingNodeIds.insert(id); else eventNodeIds.insert(id);
		}
	}
}

void GUINode::setController(GUINodeController* controller)
{
	if (this->getController() != nullptr) {
		screenNode->removeTickNode(this);
		this->getController()->dispose();
	}
	this->controller = unique_ptr<GUINodeController>(controller);
}

void GUINode::_scrollToNodeX(GUIParentNode* toNode)
{
	if (layouted == false) return;
	auto scrollXParentNode = this->parentNode;
	while (true == true) {
		if (scrollXParentNode == toNode || scrollXParentNode == nullptr)
			return;

		if (scrollXParentNode->overflowX == GUIParentNode_Overflow::SCROLL) {
			break;
		}
		scrollXParentNode = scrollXParentNode->parentNode;
	}
	if (computedConstraints.left < scrollXParentNode->getChildrenRenderOffsetX() + scrollXParentNode->computedConstraints.left) {
		scrollXParentNode->setChildrenRenderOffsetX(computedConstraints.left - scrollXParentNode->computedConstraints.left);
	}
	if (computedConstraints.left + computedConstraints.width > scrollXParentNode->getChildrenRenderOffsetX() + scrollXParentNode->computedConstraints.left + scrollXParentNode->computedConstraints.width) {
		scrollXParentNode->setChildrenRenderOffsetX(computedConstraints.left + computedConstraints.width - scrollXParentNode->computedConstraints.left - scrollXParentNode->computedConstraints.width);
	}
	scrollXParentNode->_scrollToNodeX(toNode);
}

void GUINode::_scrollToNodeY(GUIParentNode* toNode)
{
	if (layouted == false) return;
	auto scrollYParentNode = this->parentNode;
	while (true == true) {
		if (scrollYParentNode == toNode || scrollYParentNode == nullptr) return;
		if (scrollYParentNode->overflowY == GUIParentNode_Overflow::SCROLL) break;
		scrollYParentNode = scrollYParentNode->parentNode;
		if (scrollYParentNode == nullptr) return;
	}
	if (computedConstraints.top < scrollYParentNode->getChildrenRenderOffsetY() + scrollYParentNode->computedConstraints.top) {
		scrollYParentNode->setChildrenRenderOffsetY(computedConstraints.top - scrollYParentNode->computedConstraints.top);
	}
	if (computedConstraints.top + computedConstraints.height > scrollYParentNode->getChildrenRenderOffsetY() + scrollYParentNode->computedConstraints.top + scrollYParentNode->computedConstraints.height) {
		scrollYParentNode->setChildrenRenderOffsetY(computedConstraints.top + computedConstraints.height - scrollYParentNode->computedConstraints.top - scrollYParentNode->computedConstraints.height);
	}
	scrollYParentNode->_scrollToNodeY(toNode);
}

void GUINode::scrollToNodeX(GUIParentNode* toNode) {
	screenNode->scrollToNodeX(getId(), toNode != nullptr?toNode->getId():string());
}

void GUINode::scrollToNodeY(GUIParentNode* toNode) {
	screenNode->scrollToNodeY(getId(), toNode != nullptr?toNode->getId():string());
}

void GUINode::dumpNode(GUINode* node, int depth, int indent, int depthIdx) {
	string indentString;
	for (auto i = 0; i < indent; i++) indentString+= "  ";
	Console::printLine(
		indentString +
		node->id + ": " +
		node->getNodeType() + ": constaints: " +
		to_string(node->computedConstraints.left) + ", " +
		to_string(node->computedConstraints.top) + "; " +
		to_string(node->computedConstraints.width) + ", " +
		to_string(node->computedConstraints.height) + "; alignment: " +
		to_string(node->computedConstraints.alignmentLeft) + ", " +
		to_string(node->computedConstraints.alignmentTop) + "; content alignment: " +
		to_string(node->computedConstraints.contentAlignmentLeft) + ", " +
		to_string(node->computedConstraints.contentAlignmentTop) + "; " +
		StringTools::substring(node->requestedConstraints.leftType->getName(), 0, 2) + "/" +
		StringTools::substring(node->requestedConstraints.topType->getName(), 0, 2) + "/" +
		StringTools::substring(node->requestedConstraints.widthType->getName(), 0, 2) + "/" +
		StringTools::substring(node->requestedConstraints.heightType->getName(), 0, 2) + ";" +
		": conditions met: " +
		to_string(node->conditionsMet) + "; layouted: " +
		to_string(node->layouted) +
		(dynamic_cast<GUIParentNode*>(node) != nullptr?"; child count: " + to_string(dynamic_cast<GUIParentNode*>(node)->subNodes.size()):"") +
		(node->getController() != nullptr?"; controller attached":"; no controller")
	);
	if (dynamic_cast<GUIParentNode*>(node) != nullptr && (depth == 0 || depthIdx + 1 < depth)) {
		auto parentNode = required_dynamic_cast<GUIParentNode*>(node);
		for (auto subNode: parentNode->subNodes) {
			dumpNode(subNode, depth, indent + 1, depthIdx + 1);
		}
	}
}

void GUINode::dumpParentNodes(GUINode* node, int indent) {
	string indentString;
	for (auto i = 0; i < indent; i++) indentString+= "  ";
	Console::printLine(
		indentString +
		node->id + ": " +
		node->getNodeType() + ": constaints: " +
		to_string(node->computedConstraints.left) + ", " +
		to_string(node->computedConstraints.top) + ", " +
		to_string(node->computedConstraints.width) + "; " +
		to_string(node->computedConstraints.height) + ", alignment: " +
		to_string(node->computedConstraints.alignmentLeft) + ", " +
		to_string(node->computedConstraints.alignmentTop) + "; content alignment: " +
		to_string(node->computedConstraints.contentAlignmentLeft) + ", " +
		to_string(node->computedConstraints.contentAlignmentTop) + "; " +
		StringTools::substring(node->requestedConstraints.leftType->getName(), 0, 2) + "/" +
		StringTools::substring(node->requestedConstraints.topType->getName(), 0, 2) + "/" +
		StringTools::substring(node->requestedConstraints.widthType->getName(), 0, 2) + "/" +
		StringTools::substring(node->requestedConstraints.heightType->getName(), 0, 2) + ";" +
		": conditions met: " +
		to_string(node->conditionsMet) + "; layouted: " +
		to_string(node->layouted) +
		(dynamic_cast<GUIParentNode*>(node) != nullptr?"; child count: " + to_string(dynamic_cast<GUIParentNode*>(node)->subNodes.size()):"")
	);
	if (node->parentNode != nullptr) dumpParentNodes(node->parentNode, indent + 1);
}

void GUINode::cfDetermineElementNodeDependencies(vector<string>& elementNodeDependencies) {
	const auto& showOn = this->showOn.conditions;
	const auto& hideOn = this->hideOn.conditions;

	StringTokenizer t;
	string function;
	vector<string> arguments;
	for (auto i = 0; i < hideOn.size(); i++) {
		auto conditionTerm = hideOn[i];
		cfParse(hideOn[i], function, arguments);
		cfCallDetermineElementNodeDependencies(function, arguments, elementNodeDependencies);
	}
	for (auto i = 0; i < showOn.size(); i++) {
		cfParse(showOn[i], function, arguments);
		cfCallDetermineElementNodeDependencies(function, arguments, elementNodeDependencies);
	}

}

void GUINode::cfParse(const string& term, string& function, vector<string>& arguments) {
	auto leftParenthesis = term.find('(');
	auto rightParenthesis = term.find_last_of(')');
	function = "hasCondition";
	if (leftParenthesis != string::npos && rightParenthesis != string::npos && leftParenthesis < rightParenthesis) {
		function = StringTools::trim(StringTools::substring(term, 0, leftParenthesis));
	}
	arguments.clear();
	auto argumentStartIdx = leftParenthesis != string::npos?leftParenthesis + 1:0;
	auto argumentEndIdx = rightParenthesis != string::npos?rightParenthesis:term.size();
	auto quote = false;
	auto doubleQuote = false;
	string argument;
	for (auto i = argumentStartIdx; i < argumentEndIdx; i++) {
		auto c = term[i];
		if (c == '\'') {
			argument+= c;
			if (quote == true) {
				quote = false;
				arguments.push_back(StringTools::trim(argument));
				argument.clear();
			}
		} else
		if (c == '\"') {
			argument+= c;
			if (doubleQuote == true) {
				doubleQuote = false;
				arguments.push_back(argument);
				argument.clear();
			}
		} else
		if (quote == false && doubleQuote == false && c == ',') {
			arguments.push_back(StringTools::trim(argument));
			argument.clear();
		} else {
			argument+= c;
		}
	}
	if (argument.empty() == false) {
		arguments.push_back(StringTools::trim(argument));
		argument.clear();
	}
}

bool GUINode::cfCall(GUIElementNode* elementNode, const string& function, const vector<string>& arguments) {
	if (function == "empty") {
		return cfEmpty(arguments);
	} else
	if (function == "notEmpty") {
		return cfEmpty(arguments) == false;
	} else
	if (function == "hasCondition") {
		return cfHasCondition(elementNode, arguments);
	} else {
		Console::printLine("GUINode::cfCall(): Unknown function: " + function + ": returning false");
		return false;
	}
}

void GUINode::cfCallDetermineElementNodeDependencies(const string& function, const vector<string>& arguments, vector<string>& elementNodeDependencies) {
	if (function == "empty") {
		// no op
	} else
	if (function == "notEmpty") {
		// no op
	} else
	if (function == "hasCondition") {
		cfHasConditionDetermineElementNodeDependencies(arguments, elementNodeDependencies);
	} else {
		Console::printLine("GUINode::cfCallDetermineElementNodeDependencies(): Unknown function: " + function + ": returning false");
	}

}

bool GUINode::cfHasCondition(GUIElementNode* elementNode, const vector<string>& arguments) {
	StringTokenizer t;
	for (const auto& argument: arguments) {
		string elementNodeId;
		auto condition = argument;
		if (condition.find('.') != -1) {
			t.tokenize(condition, ".");
			elementNodeId = t.nextToken();
			condition = t.nextToken();
		}
		auto elementNodeToCheck = elementNodeId.size() == 0?elementNode:dynamic_cast<GUIElementNode*>(screenNode->getNodeById(elementNodeId));
		if (elementNodeToCheck == nullptr) {
			Console::printLine("GUINode::checkConditions(): element node '" + elementNodeId + "': not found");
			continue;
		}
		if (elementNodeToCheck->activeConditions.has(condition) == true) return true;
	}
	return false;
}

void GUINode::cfHasConditionDetermineElementNodeDependencies(const vector<string>& arguments, vector<string>& elementNodeDependencies) {
	StringTokenizer t;
	for (const auto& argument: arguments) {
		string elementNodeId;
		auto condition = argument;
		if (condition.find('.') != string::npos) {
			t.tokenize(condition, ".");
			elementNodeId = t.nextToken();
			condition = t.nextToken();
		}
		if (elementNodeId.empty() == false) {
			elementNodeDependencies.push_back(elementNodeId);
		}
	}
}

bool GUINode::cfEmpty(const vector<string>& arguments) {
	for (const auto& argument: arguments) {
		if (argument == "false" ||
			argument == "0" ||
			argument == "0.0" ||
			argument == "\"\"" ||
			argument == "''") return true;
	}
	return false;
}

void GUINode::setBackgroundImage(const string& backgroundImage) {
	if (backgroundTexture != nullptr) {
		GUI::getTextureManager()->removeTexture(backgroundTexture->getId());
		backgroundTexture = nullptr;
		backgroundTextureId = 0;
	}
	if (backgroundImage.length() > 0) {
		backgroundTexture = screenNode->getImage(backgroundImage);
		backgroundTextureId = GUI::getTextureManager()->addTexture(backgroundTexture, 0);
	}
}

void GUINode::addEffect(const string& id, GUIEffect* effect)
{
	removeEffect(id);
	if (effectState == nullptr && effects.empty() == true) effectState = make_unique<GUIEffectState>();
	effects[id] = effect;
}

GUIEffect* GUINode::getEffect(const string& id)
{
	auto effectIt = effects.find(id);
	if (effectIt == effects.end()) return nullptr;
	return effectIt->second;
}

void GUINode::removeEffect(const string& id)
{
	auto effectIt = effects.find(id);
	if (effectIt == effects.end()) return;
	delete effectIt->second;
	effects.erase(effectIt);
	if (effectState != nullptr && effects.empty() == true) {
		effectState = nullptr;
	}
}

void GUINode::onSetConditions(const vector<string>& conditions) {
	// no op if no effects
	if (hasEffects() == false) return;

	//
	auto defaultEffect = getEffect("tdme.xmleffect.default");

	//
	auto haveInEffect = false;
	auto issuedOutEffect = false;

	//
	for (const auto& condition: conditions) {
		{
			auto effect = getEffect("tdme.xmleffect.in.color.on." + condition);
			if (effect != nullptr && effect->isActive() == false) {
				haveInEffect = true;
				effect->start();
			}
		}
		{
			auto effect = getEffect("tdme.xmleffect.in.position.on." + condition);
			if (effect != nullptr && effect->isActive() == false) {
				haveInEffect = true;
				effect->start();
			}
		}
	}
	if (haveInEffect == true) {
		if (defaultEffect != nullptr) defaultEffect->stop();
		for (const auto& [effectId, effect]: effects) {
			if (StringTools::startsWith(effectId, "tdme.xmleffect.out.") == true && effect->isActive() == true) {
				effect->stop();
			}
		}
	} else {
		for (const auto& condition: lastConditions) {
			if (find(conditions.begin(), conditions.end(), condition) != conditions.end()) continue;
			{
				auto effect = getEffect("tdme.xmleffect.out.color.on." + condition);
				if (effect != nullptr && effect->isActive() == false) {
					issuedOutEffect = true;
					haveOutEffect = true;
					effect->start();
				}
			}
			{
				auto effect = getEffect("tdme.xmleffect.out.position.on." + condition);
				if (effect != nullptr && effect->isActive() == false) {
					issuedOutEffect = true;
					haveOutEffect = true;
					effect->start();
				}
			}
		}
		if (issuedOutEffect == true) {
			if (defaultEffect != nullptr && defaultEffect->isActive() == true) defaultEffect->stop();
			for (const auto& [effectId, effect]: effects) {
				if (StringTools::startsWith(effectId, "tdme.xmleffect.in.") == true && effect->isActive() == true) {
					effect->stop();
				}
			}
		}
	}
	lastConditions = conditions;

	// check if we need to start default effect
	auto haveColorEffect = false;
	auto havePositionEffect = false;
	for (const auto& [effectId, effect]: effects) {
		if (effect->isActive() == true) {
			switch (effect->getType()) {
				case GUIEffect::EFFECTTYPE_COLOR:
					haveColorEffect = true;
					break;
				case GUIEffect::EFFECTTYPE_POSITION:
					havePositionEffect = true;
					break;
				case GUIEffect::EFFECTTYPE_NONE:
					break;
			}
		}
	}
	if (haveColorEffect == false || havePositionEffect == false) {
		if (defaultEffect != nullptr) {
			switch (defaultEffect->getType()) {
				case GUIEffect::EFFECTTYPE_COLOR:
					haveColorEffect = true;
					break;
				case GUIEffect::EFFECTTYPE_POSITION:
					havePositionEffect = true;
					break;
				case GUIEffect::EFFECTTYPE_NONE:
					break;
			}
		}
		// also reset color effect if not applied
		if (haveColorEffect == false) {
			GUIColorEffect::resetEffectState(effectState.get());
		} else
		// also position effect if not applied
		if (havePositionEffect == false) {
			GUIPositionEffect::resetEffectState(effectState.get());
		}
		// finally start default effect if we have none
		if (defaultEffect != nullptr && haveColorEffect == false && havePositionEffect == false) {
			defaultEffect->start();
		}
	}
}

bool GUINode::haveActiveOutEffect() {
	if (haveOutEffect == false) return false;
	// do not change condition met if we have a active effect
	haveOutEffect = false;
	for (const auto& [effectId, effect]: effects) {
		if (effect->isActive() == true) {
			haveOutEffect = true;
			break;
		}
	}
	return haveOutEffect;
}

