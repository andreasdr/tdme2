#include <tdme/gui/GUIParser.h>

#include <string>
#include <unordered_map>

#include <tdme/gui/GUIParserException.h>
#include <tdme/gui/effects/GUIColorEffect.h>
#include <tdme/gui/effects/GUIPositionEffect.h>
#include <tdme/gui/elements/GUIButton.h>
#include <tdme/gui/elements/GUICheckbox.h>
#include <tdme/gui/elements/GUIDropDown.h>
#include <tdme/gui/elements/GUIDropDownOption.h>
#include <tdme/gui/elements/GUIElement.h>
#include <tdme/gui/elements/GUIImageButton.h>
#include <tdme/gui/elements/GUIInput.h>
#include <tdme/gui/elements/GUIKnob.h>
#include <tdme/gui/elements/GUIProgressBar.h>
#include <tdme/gui/elements/GUIRadioButton.h>
#include <tdme/gui/elements/GUIScrollArea.h>
#include <tdme/gui/elements/GUIScrollAreaHorizontal.h>
#include <tdme/gui/elements/GUIScrollAreaVertical.h>
#include <tdme/gui/elements/GUISelectBox.h>
#include <tdme/gui/elements/GUISelectBoxMultiple.h>
#include <tdme/gui/elements/GUISelectBoxMultipleOption.h>
#include <tdme/gui/elements/GUISelectBoxOption.h>
#include <tdme/gui/elements/GUISliderH.h>
#include <tdme/gui/elements/GUISliderV.h>
#include <tdme/gui/elements/GUITab.h>
#include <tdme/gui/elements/GUITabContent.h>
#include <tdme/gui/elements/GUITabs.h>
#include <tdme/gui/elements/GUITabsContent.h>
#include <tdme/gui/elements/GUITabsHeader.h>
#include <tdme/gui/events/Action.h>
#include <tdme/gui/nodes/GUIColor.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUIHorizontalScrollbarInternalNode.h>
#include <tdme/gui/nodes/GUIImageNode.h>
#include <tdme/gui/nodes/GUIInputInternalNode.h>
#include <tdme/gui/nodes/GUILayoutNode.h>
#include <tdme/gui/nodes/GUIMultilineTextNode.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUINode_Alignments.h>
#include <tdme/gui/nodes/GUINode_AlignmentHorizontal.h>
#include <tdme/gui/nodes/GUINodeController.h>
#include <tdme/gui/nodes/GUIPanelNode.h>
#include <tdme/gui/nodes/GUIParentNode.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/gui/nodes/GUISpaceNode.h>
#include <tdme/gui/nodes/GUITextNode.h>
#include <tdme/gui/nodes/GUIVerticalScrollbarInternalNode.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemException.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/tools/shared/tools/Tools.h>
#include <tdme/utilities/Float.h>
#include <tdme/utilities/Integer.h>
#include <tdme/utilities/MutableString.h>
#include <tdme/utilities/StringTools.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>

#include <ext/tinyxml/tinyxml.h>

using std::string;
using std::unordered_map;

using tdme::gui::GUIParser;
using tdme::gui::GUIParserException;
using tdme::gui::effects::GUIColorEffect;
using tdme::gui::effects::GUIPositionEffect;
using tdme::gui::elements::GUIButton;
using tdme::gui::elements::GUICheckbox;
using tdme::gui::elements::GUIDropDown;
using tdme::gui::elements::GUIDropDownOption;
using tdme::gui::elements::GUIElement;
using tdme::gui::elements::GUIImageButton;
using tdme::gui::elements::GUIInput;
using tdme::gui::elements::GUIKnob;
using tdme::gui::elements::GUIProgressBar;
using tdme::gui::elements::GUIRadioButton;
using tdme::gui::elements::GUIScrollArea;
using tdme::gui::elements::GUIScrollAreaHorizontal;
using tdme::gui::elements::GUIScrollAreaVertical;
using tdme::gui::elements::GUISelectBox;
using tdme::gui::elements::GUISelectBoxMultiple;
using tdme::gui::elements::GUISelectBoxMultipleOption;
using tdme::gui::elements::GUISelectBoxOption;
using tdme::gui::elements::GUISliderH;
using tdme::gui::elements::GUISliderV;
using tdme::gui::elements::GUITab;
using tdme::gui::elements::GUITabContent;
using tdme::gui::elements::GUITabs;
using tdme::gui::elements::GUITabsContent;
using tdme::gui::elements::GUITabsHeader;
using tdme::gui::events::Action;
using tdme::gui::nodes::GUIColor;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUIHorizontalScrollbarInternalNode;
using tdme::gui::nodes::GUIImageNode;
using tdme::gui::nodes::GUIInputInternalNode;
using tdme::gui::nodes::GUILayoutNode;
using tdme::gui::nodes::GUIMultilineTextNode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeController;
using tdme::gui::nodes::GUIPanelNode;
using tdme::gui::nodes::GUIParentNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::nodes::GUISpaceNode;
using tdme::gui::nodes::GUITextNode;
using tdme::gui::nodes::GUIVerticalScrollbarInternalNode;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemException;
using tdme::os::filesystem::FileSystemInterface;
using tdme::tools::shared::tools::Tools;
using tdme::utilities::Float;
using tdme::utilities::Integer;
using tdme::utilities::MutableString;
using tdme::utilities::StringTools;
using tdme::utilities::Console;
using tdme::utilities::Exception;

using tinyxml::TiXmlDocument;
using tinyxml::TiXmlElement;
using tinyxml::TiXmlAttribute;

#define AVOID_NULLPTR_STRING(arg) (arg == nullptr?"":arg)

map<string, GUIElement*> GUIParser::elements;

GUIScreenNode* GUIParser::parse(const string& pathName, const string& fileName, const unordered_map<string, string>& parameters)
{
	return parse(FileSystem::getInstance()->getContentAsString(pathName, fileName), parameters, pathName);
}

GUIScreenNode* GUIParser::parse(const string& xml, const unordered_map<string, string>& parameters, const string& pathName)
{
	// replace attributes from element
	auto newXML = xml;
	for (auto parametersIt: parameters) {
		newXML = StringTools::replace(newXML, "{$" + parametersIt.first + "}", escapeQuotes(parametersIt.second));
	}

	//
	TiXmlDocument xmlDocument;
	xmlDocument.Parse(newXML.c_str());
	if (xmlDocument.Error() == true) {
		throw GUIParserException(
			string("GUIParser::parse():: Could not parse XML. Error='") + string(xmlDocument.ErrorDesc()) + "': \n\n" + newXML
		);
	}
	TiXmlElement* xmlRoot = xmlDocument.RootElement();
	GUIScreenNode* guiScreenNode = nullptr;
	if (string(xmlRoot->Value()) != string("screen")) {
		throw GUIParserException("XML root node must be <screen>");
	}

	auto applicationRootPath = Tools::getGameRootPath(pathName);
	guiScreenNode = new GUIScreenNode(
		applicationRootPath.empty() == true?".":FileSystem::getInstance()->getCanonicalPath(applicationRootPath, ""),
		string(AVOID_NULLPTR_STRING(xmlRoot->Attribute("id"))),
		GUINode::createFlow(string(AVOID_NULLPTR_STRING(xmlRoot->Attribute("flow")))),
		GUIParentNode::createOverflow(string(AVOID_NULLPTR_STRING(xmlRoot->Attribute("overflow-x")))),
		GUIParentNode::createOverflow(string(AVOID_NULLPTR_STRING(xmlRoot->Attribute("overflow-y")))),
		GUINode::createAlignments(
			string(AVOID_NULLPTR_STRING(xmlRoot->Attribute("horizontal-align"))),
			string(AVOID_NULLPTR_STRING(xmlRoot->Attribute("vertical-align")))
		),
		GUINode::createRequestedConstraints(
			string(AVOID_NULLPTR_STRING(xmlRoot->Attribute("left"))),
			string(AVOID_NULLPTR_STRING(xmlRoot->Attribute("top"))),
			string(AVOID_NULLPTR_STRING(xmlRoot->Attribute("width"))),
			string(AVOID_NULLPTR_STRING(xmlRoot->Attribute("height")))
		),
		GUINode::getRequestedColor(string(AVOID_NULLPTR_STRING(xmlRoot->Attribute("background-color"))), GUIColor::GUICOLOR_TRANSPARENT),
		"",
		GUINode::createScale9Grid(
			string(AVOID_NULLPTR_STRING(xmlRoot->Attribute("background-image-scale9"))),
			string(AVOID_NULLPTR_STRING(xmlRoot->Attribute("background-image-scale9-left"))),
			string(AVOID_NULLPTR_STRING(xmlRoot->Attribute("background-image-scale9-top"))),
			string(AVOID_NULLPTR_STRING(xmlRoot->Attribute("background-image-scale9-right"))),
			string(AVOID_NULLPTR_STRING(xmlRoot->Attribute("background-image-scale9-bottom")))
		),
		GUINode::getRequestedColor(string(AVOID_NULLPTR_STRING(xmlRoot->Attribute("background-image-effect-color-mul"))), GUIColor::GUICOLOR_EFFECT_COLOR_MUL),
		GUINode::getRequestedColor(string(AVOID_NULLPTR_STRING(xmlRoot->Attribute("background-image-effect-color-add"))), GUIColor::GUICOLOR_EFFECT_COLOR_ADD),
		GUINode::createBorder(
			string(AVOID_NULLPTR_STRING(xmlRoot->Attribute("border"))),
			string(AVOID_NULLPTR_STRING(xmlRoot->Attribute("border-left"))),
			string(AVOID_NULLPTR_STRING(xmlRoot->Attribute("border-top"))),
			string(AVOID_NULLPTR_STRING(xmlRoot->Attribute("border-right"))),
			string(AVOID_NULLPTR_STRING(xmlRoot->Attribute("border-bottom"))),
			string(AVOID_NULLPTR_STRING(xmlRoot->Attribute("border-color"))),
			string(AVOID_NULLPTR_STRING(xmlRoot->Attribute("border-color-left"))),
			string(AVOID_NULLPTR_STRING(xmlRoot->Attribute("border-color-top"))),
			string(AVOID_NULLPTR_STRING(xmlRoot->Attribute("border-color-right"))),
			string(AVOID_NULLPTR_STRING(xmlRoot->Attribute("border-color-bottom")))
		),
		GUINode::createPadding(
			string(AVOID_NULLPTR_STRING(xmlRoot->Attribute("padding"))),
			string(AVOID_NULLPTR_STRING(xmlRoot->Attribute("padding-left"))),
			string(AVOID_NULLPTR_STRING(xmlRoot->Attribute("padding-top"))),
			string(AVOID_NULLPTR_STRING(xmlRoot->Attribute("padding-right"))),
			string(AVOID_NULLPTR_STRING(xmlRoot->Attribute("padding-bottom")))
		),
		GUIScreenNode::createSizeConstraints(
			string(AVOID_NULLPTR_STRING(xmlRoot->Attribute("min-width"))),
			string(AVOID_NULLPTR_STRING(xmlRoot->Attribute("min-height"))),
			string(AVOID_NULLPTR_STRING(xmlRoot->Attribute("max-width"))),
			string(AVOID_NULLPTR_STRING(xmlRoot->Attribute("max-height")))
		),
		GUINode::createConditions(string(AVOID_NULLPTR_STRING(xmlRoot->Attribute("show-on")))),
		GUINode::createConditions(string(AVOID_NULLPTR_STRING(xmlRoot->Attribute("hide-on")))),
		StringTools::equalsIgnoreCase(StringTools::trim(string(AVOID_NULLPTR_STRING(xmlRoot->Attribute("scrollable")))), "true"),
		StringTools::equalsIgnoreCase(StringTools::trim(string(AVOID_NULLPTR_STRING(xmlRoot->Attribute("popup")))), "true")
	);
	// workaround for having GUINode constructor to be called before GUIScreenNode constructor
	// so GUIScreenNode::applicationRootPath is not available at GUIScreenNode::GUINode construction time
	guiScreenNode->setBackgroundImage(string(AVOID_NULLPTR_STRING(xmlRoot->Attribute("background-image"))));
	parseGUINode(guiScreenNode, xmlRoot, nullptr);
	guiScreenNode->setConditionsMet();
	return guiScreenNode;
}

void GUIParser::parse(GUIParentNode* parentNode, const string& pathName, const string& fileName)
{
	string xml = FileSystem::getInstance()->getContentAsString(pathName, fileName);
	parse(parentNode, xml);
}

void GUIParser::parse(GUIParentNode* parentNode, const string& xml)
{
	TiXmlDocument xmlDocument;
	xmlDocument.Parse((string("<gui-element>") + xml + string("</gui-element>")).c_str());
	if (xmlDocument.Error() == true) {
		throw GUIParserException(
			"GUIParser::parse():: Could not parse XML. Error='" + string(xmlDocument.ErrorDesc()) + "': \n\n" + xml
		);
	}
	TiXmlElement* xmlNode = xmlDocument.RootElement();
	parseGUINode(parentNode, xmlNode, nullptr);
}

void GUIParser::parseGUINode(GUIParentNode* guiParentNode, TiXmlElement* xmlParentNode, GUIElement* guiElement)
{
	GUINodeController* guiElementController = nullptr;
	auto guiElementControllerInstalled = false;
	for (auto *node = xmlParentNode->FirstChildElement(); node != nullptr; node = node->NextSiblingElement()) {
		{
			string nodeTagName = string(node->Value());
			if (nodeTagName == "effect-in") {
				// TODO: Refactor this and next sub block into a method
				GUIEffect* effect = nullptr;
				auto type = string(AVOID_NULLPTR_STRING(node->Attribute("type")));
				if (type == "color") {
					effect = new GUIColorEffect();
					static_cast<GUIColorEffect*>(effect)->setColorMulStart(GUIColor(AVOID_NULLPTR_STRING(node->Attribute("effect-color-mul-start"))));
					static_cast<GUIColorEffect*>(effect)->setColorMulEnd(GUIColor(AVOID_NULLPTR_STRING(node->Attribute("effect-color-mul-end"))));
					static_cast<GUIColorEffect*>(effect)->setTimeTotal(Float::parseFloat(node->Attribute("time")));
					guiParentNode->addEffect(
						string("tdme.xmleffect.in." + type + ".on.") + AVOID_NULLPTR_STRING(node->Attribute("on")),
						effect
					);
				} else
				if (type == "position") {
					effect = new GUIPositionEffect();
					static_cast<GUIPositionEffect*>(effect)->setPositionXStart(Integer::parseInt(AVOID_NULLPTR_STRING(node->Attribute("effect-position-x-start"))));
					static_cast<GUIPositionEffect*>(effect)->setPositionXEnd(Integer::parseInt(AVOID_NULLPTR_STRING(node->Attribute("effect-position-x-end"))));
					static_cast<GUIPositionEffect*>(effect)->setPositionYStart(Integer::parseInt(AVOID_NULLPTR_STRING(node->Attribute("effect-position-y-start"))));
					static_cast<GUIPositionEffect*>(effect)->setPositionYEnd(Integer::parseInt(AVOID_NULLPTR_STRING(node->Attribute("effect-position-y-end"))));
					static_cast<GUIPositionEffect*>(effect)->setTimeTotal(Float::parseFloat(node->Attribute("time")));
					guiParentNode->addEffect(
						string("tdme.xmleffect.in." + type + ".on.") + AVOID_NULLPTR_STRING(node->Attribute("on")),
						effect
					);
				}
				auto action = string(AVOID_NULLPTR_STRING(node->Attribute("action")));
				if (effect != nullptr && action.empty() == false) {
					class EffectAction: public virtual Action
					{
					public:
						EffectAction(GUIScreenNode* guiScreenNode, const string& expression): guiScreenNode(guiScreenNode), expression(expression) {
						}
						void performAction() override {
							GUIElementNode::executeExpression(guiScreenNode, expression);
						}
					private:
						GUIScreenNode* guiScreenNode;
						string expression;
					};
					effect->setAction(new EffectAction(guiParentNode->getScreenNode(), action));
				}
			} else
			if (nodeTagName == "effect-out") {
				GUIEffect* effect = nullptr;
				auto type = string(AVOID_NULLPTR_STRING(node->Attribute("type")));
				if (type == "color") {
					effect = new GUIColorEffect();
					static_cast<GUIColorEffect*>(effect)->setColorMulStart(GUIColor(AVOID_NULLPTR_STRING(node->Attribute("effect-color-mul-start"))));
					static_cast<GUIColorEffect*>(effect)->setColorMulEnd(GUIColor(AVOID_NULLPTR_STRING(node->Attribute("effect-color-mul-end"))));
					static_cast<GUIColorEffect*>(effect)->setTimeTotal(Float::parseFloat(node->Attribute("time")));
					guiParentNode->addEffect(
						string("tdme.xmleffect.out." + type + ".on.") + AVOID_NULLPTR_STRING(node->Attribute("on")),
						effect
					);
				} else
				if (type == "position") {
					effect = new GUIPositionEffect();
					static_cast<GUIPositionEffect*>(effect)->setPositionXStart(Integer::parseInt(AVOID_NULLPTR_STRING(node->Attribute("effect-position-x-start"))));
					static_cast<GUIPositionEffect*>(effect)->setPositionXEnd(Integer::parseInt(AVOID_NULLPTR_STRING(node->Attribute("effect-position-x-end"))));
					static_cast<GUIPositionEffect*>(effect)->setPositionYStart(Integer::parseInt(AVOID_NULLPTR_STRING(node->Attribute("effect-position-y-start"))));
					static_cast<GUIPositionEffect*>(effect)->setPositionYEnd(Integer::parseInt(AVOID_NULLPTR_STRING(node->Attribute("effect-position-y-end"))));
					static_cast<GUIPositionEffect*>(effect)->setTimeTotal(Float::parseFloat(node->Attribute("time")));
					guiParentNode->addEffect(
						string("tdme.xmleffect.out." + type + ".on.") + AVOID_NULLPTR_STRING(node->Attribute("on")),
						effect
					);
				}
				auto action = string(AVOID_NULLPTR_STRING(node->Attribute("action")));
				if (effect != nullptr && action.empty() == false) {
					class EffectAction: public virtual Action
					{
					public:
						EffectAction(GUIScreenNode* guiScreenNode, const string& expression): guiScreenNode(guiScreenNode), expression(expression) {
						}
						void performAction() override {
							GUIElementNode::executeExpression(guiScreenNode, expression);
						}
					private:
						GUIScreenNode* guiScreenNode;
						string expression;
					};
					effect->setAction(new EffectAction(guiParentNode->getScreenNode(), action));
				}
			} else
			if (nodeTagName == "panel") {
				auto guiPanelNode = new GUIPanelNode(
					guiParentNode->getScreenNode(),
					guiParentNode,
					string(AVOID_NULLPTR_STRING(node->Attribute("id"))),
					GUINode::createFlow(string(AVOID_NULLPTR_STRING(node->Attribute("flow")))),
					GUIParentNode::createOverflow(string(AVOID_NULLPTR_STRING(node->Attribute("overflow-x")))),
					GUIParentNode::createOverflow(string(AVOID_NULLPTR_STRING(node->Attribute("overflow-y")))),
					GUINode::createAlignments(
						string(AVOID_NULLPTR_STRING(node->Attribute("horizontal-align"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("vertical-align")))
					),
					GUIParentNode::createRequestedConstraints(
						string(AVOID_NULLPTR_STRING(node->Attribute("left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("width"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("height")))
					),
					GUINode::getRequestedColor(string(AVOID_NULLPTR_STRING(node->Attribute("background-color"))), GUIColor("#F0F0F0")),
					string(AVOID_NULLPTR_STRING(node->Attribute("background-image"))),
					GUINode::createScale9Grid(
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9-left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9-top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9-right"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9-bottom")))
					),
					GUINode::getRequestedColor(string(AVOID_NULLPTR_STRING(node->Attribute("background-image-effect-color-mul"))), GUIColor::GUICOLOR_EFFECT_COLOR_MUL),
					GUINode::getRequestedColor(string(AVOID_NULLPTR_STRING(node->Attribute("background-image-effect-color-add"))), GUIColor::GUICOLOR_EFFECT_COLOR_ADD),
					GUINode::createBorder(
						string(AVOID_NULLPTR_STRING(node->Attribute("border"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-right"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-bottom"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color-left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color-top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color-right"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color-bottom")))
					),
					GUINode::createPadding(
						string(AVOID_NULLPTR_STRING(node->Attribute("padding"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("padding-left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("padding-top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("padding-right"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("padding-bottom")))
					),
					GUINode::createConditions(string(AVOID_NULLPTR_STRING(node->Attribute("show-on")))),
					GUINode::createConditions(string(AVOID_NULLPTR_STRING(node->Attribute("hide-on")))),
					GUILayoutNode::createAlignment(string(AVOID_NULLPTR_STRING(node->Attribute("alignment"))))
				);
				guiParentNode->addSubNode(guiPanelNode);
				if (guiElement != nullptr && guiElementControllerInstalled == false) {
					guiElementController = guiElement->createController(guiPanelNode);
					if (guiElementController != nullptr) {
						guiPanelNode->setController(guiElementController);
					}
					guiElementControllerInstalled = true;
				}
				parseGUINode(guiPanelNode, node, nullptr);
			} else
			if (nodeTagName == "layout") {
				auto guiLayoutNode = new GUILayoutNode(
					guiParentNode->getScreenNode(),
					guiParentNode,
					string(AVOID_NULLPTR_STRING(node->Attribute("id"))),
					GUINode::createFlow(string(AVOID_NULLPTR_STRING(node->Attribute("flow")))),
					GUIParentNode::createOverflow(string(AVOID_NULLPTR_STRING(node->Attribute("overflow-x")))),
					GUIParentNode::createOverflow(string(AVOID_NULLPTR_STRING(node->Attribute("overflow-y")))),
					GUINode::createAlignments(
						string(AVOID_NULLPTR_STRING(node->Attribute("horizontal-align"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("vertical-align")))
					),
					GUIParentNode::createRequestedConstraints(
						string(AVOID_NULLPTR_STRING(node->Attribute("left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("width"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("height")))
					),
					GUINode::getRequestedColor(string(AVOID_NULLPTR_STRING(node->Attribute("background-color"))), GUIColor::GUICOLOR_TRANSPARENT),
					string(AVOID_NULLPTR_STRING(node->Attribute("background-image"))),
					GUINode::createScale9Grid(
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9-left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9-top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9-right"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9-bottom")))
					),
					GUINode::getRequestedColor(string(AVOID_NULLPTR_STRING(node->Attribute("background-image-effect-color-mul"))), GUIColor::GUICOLOR_EFFECT_COLOR_MUL),
					GUINode::getRequestedColor(string(AVOID_NULLPTR_STRING(node->Attribute("background-image-effect-color-add"))), GUIColor::GUICOLOR_EFFECT_COLOR_ADD),
					GUINode::createBorder(
						string(AVOID_NULLPTR_STRING(node->Attribute("border"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-right"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-bottom"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color-left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color-top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color-right"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color-bottom")))
					),
					GUINode::createPadding(
						string(AVOID_NULLPTR_STRING(node->Attribute("padding"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("padding-left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("padding-top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("padding-right"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("padding-bottom")))
					),
					GUINode::createConditions(string(AVOID_NULLPTR_STRING(node->Attribute("show-on")))),
					GUINode::createConditions(string(AVOID_NULLPTR_STRING(node->Attribute("hide-on")))),
					GUILayoutNode::createAlignment(string(AVOID_NULLPTR_STRING(node->Attribute("alignment"))))
				);
				guiParentNode->addSubNode(guiLayoutNode);
				if (guiElement != nullptr && guiElementControllerInstalled == false) {
					guiElementController = guiElement->createController(guiLayoutNode);
					if (guiElementController != nullptr) {
						guiLayoutNode->setController(guiElementController);
					}
					guiElementControllerInstalled = true;
				}
				parseGUINode(guiLayoutNode, node, nullptr);
			} else
			if (nodeTagName == "space") {
				auto guiSpaceNode = new GUISpaceNode(
					guiParentNode->getScreenNode(),
					guiParentNode,
					string(AVOID_NULLPTR_STRING(node->Attribute("id"))),
					GUINode::createFlow(string(AVOID_NULLPTR_STRING(node->Attribute("flow")))),
					GUINode::createAlignments(
						string(AVOID_NULLPTR_STRING(node->Attribute("horizontal-align"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("vertical-align")))
					),
					GUIParentNode::createRequestedConstraints(
						string(AVOID_NULLPTR_STRING(node->Attribute("left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("width"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("height")))
					),
					GUINode::getRequestedColor(string(AVOID_NULLPTR_STRING(node->Attribute("background-color"))), GUIColor::GUICOLOR_TRANSPARENT),
					string(AVOID_NULLPTR_STRING(node->Attribute("background-image"))),
					GUINode::createScale9Grid(
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9-left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9-top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9-right"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9-bottom")))
					),
					GUINode::getRequestedColor(string(AVOID_NULLPTR_STRING(node->Attribute("background-image-effect-color-mul"))), GUIColor::GUICOLOR_EFFECT_COLOR_MUL),
					GUINode::getRequestedColor(string(AVOID_NULLPTR_STRING(node->Attribute("background-image-effect-color-add"))), GUIColor::GUICOLOR_EFFECT_COLOR_ADD),
					GUINode::createBorder(
						string(AVOID_NULLPTR_STRING(node->Attribute("border"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-right"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-bottom"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color-left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color-top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color-right"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color-bottom")))
					),
					GUINode::createPadding(
						string(AVOID_NULLPTR_STRING(node->Attribute("padding"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("padding-left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("padding-top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("padding-right"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("padding-bottom")))
					),
					GUINode::createConditions(string(AVOID_NULLPTR_STRING(node->Attribute("show-on")))),
					GUINode::createConditions(string(AVOID_NULLPTR_STRING(node->Attribute("hide-on"))))
				);
				guiParentNode->addSubNode(guiSpaceNode);
				if (guiElement != nullptr && guiElementControllerInstalled == false) {
					guiElementController = guiElement->createController(guiSpaceNode);
					if (guiElementController != nullptr) {
						guiSpaceNode->setController(guiElementController);
					}
					guiElementControllerInstalled = true;
				}
			} else
			if (nodeTagName == "element") {
				auto guiElementNode = new GUIElementNode(
					guiParentNode->getScreenNode(),
					guiParentNode,
					string(AVOID_NULLPTR_STRING(node->Attribute("id"))),
					GUINode::createFlow(string(AVOID_NULLPTR_STRING(node->Attribute("flow")))),
					GUIParentNode::createOverflow(string(AVOID_NULLPTR_STRING(node->Attribute("overflow-x")))),
					GUIParentNode::createOverflow(string(AVOID_NULLPTR_STRING(node->Attribute("overflow-y")))),
					GUIElementNode::createAlignments(
						string(AVOID_NULLPTR_STRING(node->Attribute("horizontal-align"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("vertical-align")))
					),
					GUIParentNode::createRequestedConstraints(
						string(AVOID_NULLPTR_STRING(node->Attribute("left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("width"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("height")))
					),
					GUINode::getRequestedColor(string(AVOID_NULLPTR_STRING(node->Attribute("background-color"))), GUIColor::GUICOLOR_TRANSPARENT),
					string(AVOID_NULLPTR_STRING(node->Attribute("background-image"))),
					GUINode::createScale9Grid(
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9-left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9-top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9-right"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9-bottom")))
					),
					GUINode::getRequestedColor(string(AVOID_NULLPTR_STRING(node->Attribute("background-image-effect-color-mul"))), GUIColor::GUICOLOR_EFFECT_COLOR_MUL),
					GUINode::getRequestedColor(string(AVOID_NULLPTR_STRING(node->Attribute("background-image-effect-color-add"))), GUIColor::GUICOLOR_EFFECT_COLOR_ADD),
					GUINode::createBorder(
						string(AVOID_NULLPTR_STRING(node->Attribute("border"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-right"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-bottom"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color-left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color-top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color-right"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color-bottom")))
					),
					GUINode::createPadding(
						string(AVOID_NULLPTR_STRING(node->Attribute("padding"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("padding-left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("padding-top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("padding-right"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("padding-bottom")))
					),
					GUINode::createConditions(string(AVOID_NULLPTR_STRING(node->Attribute("show-on")))),
					GUINode::createConditions(string(AVOID_NULLPTR_STRING(node->Attribute("hide-on")))),
					unescapeQuotes(string(AVOID_NULLPTR_STRING(node->Attribute("name")))),
					unescapeQuotes(string(AVOID_NULLPTR_STRING(node->Attribute("value")))),
					StringTools::equalsIgnoreCase(StringTools::trim(string(AVOID_NULLPTR_STRING(node->Attribute("selected")))), "true"),
					StringTools::equalsIgnoreCase(StringTools::trim(string(AVOID_NULLPTR_STRING(node->Attribute("disabled")))), "true"),
					StringTools::equalsIgnoreCase(StringTools::trim(string(AVOID_NULLPTR_STRING(node->Attribute("focusable")))), "true"),
					StringTools::equalsIgnoreCase(StringTools::trim(string(AVOID_NULLPTR_STRING(node->Attribute("ignore-events")))), "true"),
					string(AVOID_NULLPTR_STRING(node->Attribute("on-initialize"))),
					string(AVOID_NULLPTR_STRING(node->Attribute("on-mouse-click"))),
					string(AVOID_NULLPTR_STRING(node->Attribute("on-mouse-doubleclick"))),
					string(AVOID_NULLPTR_STRING(node->Attribute("on-mouse-over"))),
					string(AVOID_NULLPTR_STRING(node->Attribute("on-mouse-out"))),
					string(AVOID_NULLPTR_STRING(node->Attribute("on-change")))
				);
				guiParentNode->addSubNode(guiElementNode);
				if (guiElement != nullptr && guiElementControllerInstalled == false) {
					guiElementController = guiElement->createController(guiElementNode);
					if (guiElementController != nullptr) {
						guiElementNode->setController(guiElementController);
					}
					guiElementControllerInstalled = true;
				}
				parseGUINode(guiElementNode, node, nullptr);
			} else
			if (nodeTagName == "image") {
				auto guiImageNode = new GUIImageNode(
					guiParentNode->getScreenNode(),
					guiParentNode,
					string(AVOID_NULLPTR_STRING(node->Attribute("id"))),
					GUINode::createFlow(string(AVOID_NULLPTR_STRING(node->Attribute("flow")))),
					GUINode::createAlignments(
						string(AVOID_NULLPTR_STRING(node->Attribute("horizontal-align"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("vertical-align")))
					),
					GUIParentNode::createRequestedConstraints(
						string(AVOID_NULLPTR_STRING(node->Attribute("left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("width"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("height")))
					),
					GUINode::getRequestedColor(string(AVOID_NULLPTR_STRING(node->Attribute("background-color"))), GUIColor::GUICOLOR_TRANSPARENT),
					string(AVOID_NULLPTR_STRING(node->Attribute("background-image"))),
					GUINode::createScale9Grid(
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9-left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9-top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9-right"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9-bottom")))
					),
					GUINode::getRequestedColor(string(AVOID_NULLPTR_STRING(node->Attribute("background-image-effect-color-mul"))), GUIColor::GUICOLOR_EFFECT_COLOR_MUL),
					GUINode::getRequestedColor(string(AVOID_NULLPTR_STRING(node->Attribute("background-image-effect-color-add"))), GUIColor::GUICOLOR_EFFECT_COLOR_ADD),
					GUINode::createBorder(
						string(AVOID_NULLPTR_STRING(node->Attribute("border"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-right"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-bottom"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color-left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color-top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color-right"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color-bottom")))
					),
					GUINode::createPadding(
						string(AVOID_NULLPTR_STRING(node->Attribute("padding"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("padding-left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("padding-top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("padding-right"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("padding-bottom")))
					),
					GUINode::createConditions(string(AVOID_NULLPTR_STRING(node->Attribute("show-on")))),
					GUINode::createConditions(string(AVOID_NULLPTR_STRING(node->Attribute("hide-on")))),
					StringTools::trim(unescapeQuotes(string(AVOID_NULLPTR_STRING(node->Attribute("src"))))),
					GUINode::getRequestedColor(string(AVOID_NULLPTR_STRING(node->Attribute("effect-color-mul"))), GUIColor::GUICOLOR_EFFECT_COLOR_MUL),
					GUINode::getRequestedColor(string(AVOID_NULLPTR_STRING(node->Attribute("effect-color-add"))), GUIColor::GUICOLOR_EFFECT_COLOR_ADD),
					GUINode::createScale9Grid(
						string(AVOID_NULLPTR_STRING(node->Attribute("scale9"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("scale9-left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("scale9-top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("scale9-right"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("scale9-bottom")))
					),
					GUIImageNode::createClipping(
						string(AVOID_NULLPTR_STRING(node->Attribute("clipping"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("clipping-left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("clipping-top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("clipping-right"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("clipping-bottom")))
					),
					StringTools::trim(unescapeQuotes(string(AVOID_NULLPTR_STRING(node->Attribute("mask"))))),
					Float::parseFloat(string(AVOID_NULLPTR_STRING(node->Attribute("mask-max-value"))))
				);
				guiParentNode->addSubNode(guiImageNode);
				if (guiElement != nullptr && guiElementControllerInstalled == false) {
					guiElementController = guiElement->createController(guiImageNode);
					if (guiElementController != nullptr) {
						guiImageNode->setController(guiElementController);
					}
					guiElementControllerInstalled = true;
				}
			} else
			if (nodeTagName == "text") {
				auto guiTextNode = new GUITextNode(
					guiParentNode->getScreenNode(),
					guiParentNode,
					string(AVOID_NULLPTR_STRING(node->Attribute("id"))),
					GUINode::createFlow(string(AVOID_NULLPTR_STRING(node->Attribute("flow")))),
					GUINode::createAlignments(
						string(AVOID_NULLPTR_STRING(node->Attribute("horizontal-align"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("vertical-align")))
					),
					GUIParentNode::createRequestedConstraints(
						string(AVOID_NULLPTR_STRING(node->Attribute("left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("width"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("height")))
					),
					GUINode::getRequestedColor(string(AVOID_NULLPTR_STRING(node->Attribute("background-color"))), GUIColor::GUICOLOR_TRANSPARENT),
					string(AVOID_NULLPTR_STRING(node->Attribute("background-image"))),
					GUINode::createScale9Grid(
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9-left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9-top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9-right"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9-bottom")))
					),
					GUINode::getRequestedColor(string(AVOID_NULLPTR_STRING(node->Attribute("background-image-effect-color-mul"))), GUIColor::GUICOLOR_EFFECT_COLOR_MUL),
					GUINode::getRequestedColor(string(AVOID_NULLPTR_STRING(node->Attribute("background-image-effect-color-add"))), GUIColor::GUICOLOR_EFFECT_COLOR_ADD),
					GUINode::createBorder(
						string(AVOID_NULLPTR_STRING(node->Attribute("border"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-right"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-bottom"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color-left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color-top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color-right"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color-bottom")))
					),
					GUINode::createPadding(
						string(AVOID_NULLPTR_STRING(node->Attribute("padding"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("padding-left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("padding-top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("padding-right"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("padding-bottom")))
					),
					GUINode::createConditions(string(AVOID_NULLPTR_STRING(node->Attribute("show-on")))),
					GUINode::createConditions(string(AVOID_NULLPTR_STRING(node->Attribute("hide-on")))),
					StringTools::trim(string(AVOID_NULLPTR_STRING(node->Attribute("font")))),
					string(AVOID_NULLPTR_STRING(node->Attribute("color"))),
					MutableString(unescapeQuotes(string(StringTools::trim(AVOID_NULLPTR_STRING(node->Attribute("text"))))))
				);
				guiParentNode->addSubNode(guiTextNode);
				if (guiElement != nullptr && guiElementControllerInstalled == false) {
					guiElementController = guiElement->createController(guiTextNode);
					if (guiElementController != nullptr) {
						guiTextNode->setController(guiElementController);
					}
					guiElementControllerInstalled = true;
				}
			} else
			if (nodeTagName == "multiline-text") {
				auto guiTextNode = new GUIMultilineTextNode(
					guiParentNode->getScreenNode(),
					guiParentNode,
					string(AVOID_NULLPTR_STRING(node->Attribute("id"))),
					GUINode::createFlow(string(AVOID_NULLPTR_STRING(node->Attribute("flow")))),
					GUINode::createAlignments(
						string(AVOID_NULLPTR_STRING(node->Attribute("horizontal-align"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("vertical-align")))
					),
					GUIParentNode::createRequestedConstraints(
						string(AVOID_NULLPTR_STRING(node->Attribute("left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("width"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("height")))
					),
					GUINode::getRequestedColor(string(AVOID_NULLPTR_STRING(node->Attribute("background-color"))), GUIColor::GUICOLOR_TRANSPARENT),
					string(AVOID_NULLPTR_STRING(node->Attribute("background-image"))),
					GUINode::createScale9Grid(
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9-left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9-top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9-right"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9-bottom")))
					),
					GUINode::getRequestedColor(string(AVOID_NULLPTR_STRING(node->Attribute("background-image-effect-color-mul"))), GUIColor::GUICOLOR_EFFECT_COLOR_MUL),
					GUINode::getRequestedColor(string(AVOID_NULLPTR_STRING(node->Attribute("background-image-effect-color-add"))), GUIColor::GUICOLOR_EFFECT_COLOR_ADD),
					GUINode::createBorder(
						string(AVOID_NULLPTR_STRING(node->Attribute("border"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-right"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-bottom"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color-left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color-top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color-right"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color-bottom")))
					),
					GUINode::createPadding(
						string(AVOID_NULLPTR_STRING(node->Attribute("padding"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("padding-left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("padding-top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("padding-right"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("padding-bottom")))
					),
					GUINode::createConditions(string(AVOID_NULLPTR_STRING(node->Attribute("show-on")))),
					GUINode::createConditions(string(AVOID_NULLPTR_STRING(node->Attribute("hide-on")))),
					StringTools::trim(string(AVOID_NULLPTR_STRING(node->Attribute("font")))),
					string(AVOID_NULLPTR_STRING(node->Attribute("color"))),
					MutableString(StringTools::trim(AVOID_NULLPTR_STRING(node->GetText())))
				);
				guiParentNode->addSubNode(guiTextNode);
				if (guiElement != nullptr && guiElementControllerInstalled == false) {
					guiElementController = guiElement->createController(guiTextNode);
					if (guiElementController != nullptr) {
						guiTextNode->setController(guiElementController);
					}
					guiElementControllerInstalled = true;
				}
			} else
			if (nodeTagName == "input-internal") {
				auto guiInputInternalNode = new GUIInputInternalNode(
					guiParentNode->getScreenNode(),
					guiParentNode,
					string(AVOID_NULLPTR_STRING(node->Attribute("id"))),
					GUINode::createFlow(string(AVOID_NULLPTR_STRING(node->Attribute("flow")))),
					GUINode::createAlignments(
						string(AVOID_NULLPTR_STRING(node->Attribute("horizontal-align"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("vertical-align")))
					),
					GUIParentNode::createRequestedConstraints(
						string(AVOID_NULLPTR_STRING(node->Attribute("left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("width"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("height")))
					),
					GUINode::getRequestedColor(string(AVOID_NULLPTR_STRING(node->Attribute("background-color"))), GUIColor::GUICOLOR_TRANSPARENT),
					string(AVOID_NULLPTR_STRING(node->Attribute("background-image"))),
					GUINode::createScale9Grid(
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9-left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9-top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9-right"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9-bottom")))
					),
					GUINode::getRequestedColor(string(AVOID_NULLPTR_STRING(node->Attribute("background-image-effect-color-mul"))), GUIColor::GUICOLOR_EFFECT_COLOR_MUL),
					GUINode::getRequestedColor(string(AVOID_NULLPTR_STRING(node->Attribute("background-image-effect-color-add"))), GUIColor::GUICOLOR_EFFECT_COLOR_ADD),
					GUINode::createBorder(
						string(AVOID_NULLPTR_STRING(node->Attribute("border"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-right"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-bottom"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color-left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color-top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color-right"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color-bottom")))
					),
					GUINode::createPadding(
						string(AVOID_NULLPTR_STRING(node->Attribute("padding"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("padding-left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("padding-top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("padding-right"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("padding-bottom")))
					),
					GUINode::createConditions(string(AVOID_NULLPTR_STRING(node->Attribute("show-on")))),
					GUINode::createConditions(string(AVOID_NULLPTR_STRING(node->Attribute("hide-on")))),
					StringTools::trim(string(AVOID_NULLPTR_STRING(node->Attribute("font")))),
					string(AVOID_NULLPTR_STRING(node->Attribute("color"))),
					string(AVOID_NULLPTR_STRING(node->Attribute("color-disabled"))),
					MutableString(unescapeQuotes(string(AVOID_NULLPTR_STRING(node->Attribute("text"))))),
					GUIInputInternalNode::createMaxLength(string(AVOID_NULLPTR_STRING(node->Attribute("maxlength"))))
				);
				guiParentNode->addSubNode(guiInputInternalNode);
				if (guiElement != nullptr && guiElementControllerInstalled == false) {
					guiElementController = guiElement->createController(guiInputInternalNode);
					if (guiElementController != nullptr) {
						guiInputInternalNode->setController(guiElementController);
					}
					guiElementControllerInstalled = true;
				}
			} else
			if (nodeTagName == "vertical-scrollbar-internal") {
				auto guiVerticalScrollbarInternalNode = new GUIVerticalScrollbarInternalNode(
					guiParentNode->getScreenNode(),
					guiParentNode,
					string(AVOID_NULLPTR_STRING(node->Attribute("id"))),
					GUINode::createFlow(string(AVOID_NULLPTR_STRING(node->Attribute("flow")))),
					GUINode::createAlignments(
						string(AVOID_NULLPTR_STRING(node->Attribute("horizontal-align"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("vertical-align")))
					),
					GUIParentNode::createRequestedConstraints(
						string(AVOID_NULLPTR_STRING(node->Attribute("left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("width"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("height")))
					),
					GUINode::getRequestedColor(string(AVOID_NULLPTR_STRING(node->Attribute("background-color"))), GUIColor::GUICOLOR_TRANSPARENT),
					string(AVOID_NULLPTR_STRING(node->Attribute("background-image"))),
					GUINode::createScale9Grid(
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9-left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9-top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9-right"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9-bottom")))
					),
					GUINode::getRequestedColor(string(AVOID_NULLPTR_STRING(node->Attribute("background-image-effect-color-mul"))), GUIColor::GUICOLOR_EFFECT_COLOR_MUL),
					GUINode::getRequestedColor(string(AVOID_NULLPTR_STRING(node->Attribute("background-image-effect-color-add"))), GUIColor::GUICOLOR_EFFECT_COLOR_ADD),
					GUINode::createBorder(
						string(AVOID_NULLPTR_STRING(node->Attribute("border"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-right"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-bottom"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color-left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color-top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color-right"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color-bottom")))
					),
					GUINode::createPadding(
						string(AVOID_NULLPTR_STRING(node->Attribute("padding"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("padding-left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("padding-top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("padding-right"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("padding-bottom")))
					),
					GUINode::createConditions(string(AVOID_NULLPTR_STRING(node->Attribute("show-on")))),
					GUINode::createConditions(string(AVOID_NULLPTR_STRING(node->Attribute("hide-on")))),
					GUINode::getRequestedColor(string(AVOID_NULLPTR_STRING(node->Attribute("color-none"))), GUIColor::GUICOLOR_BLACK),
					GUINode::getRequestedColor(string(AVOID_NULLPTR_STRING(node->Attribute("color-mouseover"))), GUIColor::GUICOLOR_BLACK),
					GUINode::getRequestedColor(string(AVOID_NULLPTR_STRING(node->Attribute("color-dragging"))), GUIColor::GUICOLOR_BLACK)
				);
				guiParentNode->addSubNode(guiVerticalScrollbarInternalNode);
				if (guiElement != nullptr && guiElementControllerInstalled == false) {
					guiElementController = guiElement->createController(guiVerticalScrollbarInternalNode);
					if (guiElementController != nullptr) {
						guiVerticalScrollbarInternalNode->setController(guiElementController);
					}
					guiElementControllerInstalled = true;
				}
			} else
			if (nodeTagName == "horizontal-scrollbar-internal") {
				auto guiHorizontalScrollbarInternalNode = new GUIHorizontalScrollbarInternalNode(
					guiParentNode->getScreenNode(),
					guiParentNode,
					string(AVOID_NULLPTR_STRING(node->Attribute("id"))),
					GUINode::createFlow(string(AVOID_NULLPTR_STRING(node->Attribute("flow")))),
					GUINode::createAlignments(
						string(AVOID_NULLPTR_STRING(node->Attribute("horizontal-align"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("vertical-align")))
					),
					GUIParentNode::createRequestedConstraints(
						string(AVOID_NULLPTR_STRING(node->Attribute("left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("width"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("height")))
					),
					GUINode::getRequestedColor(string(AVOID_NULLPTR_STRING(node->Attribute("background-color"))), GUIColor::GUICOLOR_TRANSPARENT),
					string(AVOID_NULLPTR_STRING(node->Attribute("background-image"))),
					GUINode::createScale9Grid(
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9-left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9-top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9-right"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("background-image-scale9-bottom")))
					),
					GUINode::getRequestedColor(string(AVOID_NULLPTR_STRING(node->Attribute("background-image-effect-color-mul"))), GUIColor::GUICOLOR_EFFECT_COLOR_MUL),
					GUINode::getRequestedColor(string(AVOID_NULLPTR_STRING(node->Attribute("background-image-effect-color-add"))), GUIColor::GUICOLOR_EFFECT_COLOR_ADD),
					GUINode::createBorder(
						string(AVOID_NULLPTR_STRING(node->Attribute("border"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-right"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-bottom"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color-left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color-top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color-right"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("border-color-bottom")))
					),
					GUINode::createPadding(
						string(AVOID_NULLPTR_STRING(node->Attribute("padding"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("padding-left"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("padding-top"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("padding-right"))),
						string(AVOID_NULLPTR_STRING(node->Attribute("padding-bottom")))
					),
					GUINode::createConditions(string(AVOID_NULLPTR_STRING(node->Attribute("show-on")))),
					GUINode::createConditions(string(AVOID_NULLPTR_STRING(node->Attribute("hide-on")))),
					GUINode::getRequestedColor(string(AVOID_NULLPTR_STRING(node->Attribute("color-none"))), GUIColor::GUICOLOR_BLACK),
					GUINode::getRequestedColor(string(AVOID_NULLPTR_STRING(node->Attribute("color-mouseover"))), GUIColor::GUICOLOR_BLACK),
					GUINode::getRequestedColor(string(AVOID_NULLPTR_STRING(node->Attribute("color-dragging"))), GUIColor::GUICOLOR_BLACK)
				);
				guiParentNode->addSubNode(guiHorizontalScrollbarInternalNode);
				if (guiElement != nullptr && guiElementControllerInstalled == false) {
					guiElementController = guiElement->createController(guiHorizontalScrollbarInternalNode);
					if (guiElementController != nullptr) {
						guiHorizontalScrollbarInternalNode->setController(guiElementController);
					}
					guiElementControllerInstalled = true;
				}
			} else
			if (nodeTagName == "template") {
				auto src = string(AVOID_NULLPTR_STRING(node->Attribute("src")));
				unordered_map<string, string> attributes;
				parseTemplate(
					guiParentNode,
					node,
					FileSystem::getInstance()->getContentAsString(
						guiParentNode->getScreenNode()->getApplicationRootPath() + "/" +
						FileSystem::getInstance()->getPathName(src),
						FileSystem::getInstance()->getFileName(src)
					),
					attributes,
					nullptr
				);
			} else {
				auto nodeTagNameString = nodeTagName;
				const auto guiElementIt = elements.find(nodeTagNameString);
				if (guiElementIt == elements.end()) {
					throw GUIParserException(
						"Unknown element '" +
						(nodeTagNameString) +
						"'"
					);
				}
				parseTemplate(
					guiParentNode,
					node,
					guiElementIt->second->getTemplate(guiParentNode->getScreenNode()->getApplicationRootPath(),
					AVOID_NULLPTR_STRING(node->Attribute("template"))),
					guiElementIt->second->getAttributes(guiParentNode->screenNode),
					guiElementIt->second
				);
			}
		}
	}
	if (guiElementController != nullptr) {
		guiElementController->initialize();
	}
}

void GUIParser::parseTemplate(GUIParentNode* parentNode, TiXmlElement* node, const string& templateXML, const unordered_map<string, string>& attributes, GUIElement* guiElement) {
	auto newGuiElementTemplateXML = templateXML;

	// replace attributes given
	for (TiXmlAttribute* attribute = node->FirstAttribute(); attribute != nullptr; attribute = attribute->Next()) {
		auto attributeKey = string(attribute->Name());
		auto attributeValue = string(attribute->Value());
		newGuiElementTemplateXML = StringTools::replace(newGuiElementTemplateXML, "{$" + attributeKey + "}", escapeQuotes(attributeValue));
	}

	// replace attributes from element
	for (auto newGuiElementAttributesIt : attributes) {
		auto guiElementAttributeValue = escapeQuotes(newGuiElementAttributesIt.second);
		newGuiElementTemplateXML = StringTools::replace(newGuiElementTemplateXML, "{$" + newGuiElementAttributesIt.first + "}", guiElementAttributeValue);
	}

	// replace remaining unset parameters with empty spaces
	newGuiElementTemplateXML = StringTools::regexReplace(newGuiElementTemplateXML, "\\{\\$[a-zA-Z\\-_0-9]{1,}\\}", "");

	// replace inner XML
	newGuiElementTemplateXML = StringTools::replace(newGuiElementTemplateXML, "{__InnerXML__}", getInnerXml(node));

	// add root tag
	newGuiElementTemplateXML =  "<gui-element>\n" + newGuiElementTemplateXML + "</gui-element>\n";

	// parse
	TiXmlDocument newGuiElementDocument;
	newGuiElementDocument.Parse(newGuiElementTemplateXML.c_str());
	if (newGuiElementDocument.Error() == true) {
		throw GUIParserException(
			"GUIParser::parse():: Could not parse XML. Error='" + string(newGuiElementDocument.ErrorDesc())
		);
	}
	parseGUINode(parentNode, newGuiElementDocument.RootElement(), guiElement);
}

const vector<TiXmlElement*> GUIParser::getChildrenByTagName(TiXmlElement* parent, const char* name)
{
	vector<TiXmlElement*> elementList;
	for (auto *child = parent->FirstChildElement(name); child != nullptr; child = child->NextSiblingElement(name)) {
		elementList.push_back(child);
	}
	return elementList;
}

const string GUIParser::getInnerXml(TiXmlElement* node)
{
	std::stringstream ss;
	for (auto *childNode = node->FirstChildElement(); childNode != nullptr; childNode = childNode->NextSiblingElement()) {
		ss << (*childNode);
	}
	return ss.str();
}

const string GUIParser::unescapeQuotes(const string& str)
{
	string result;
	result = StringTools::replace(str, "&quot;", "\"");
	result = StringTools::replace(result, "&#39;", "'");
	return result;
}

const string GUIParser::escapeQuotes(const string& str)
{
	string result;
	result = StringTools::replace(str, "\"", "&quot;");
	result = StringTools::replace(result, "'", "&#39;");
	return result;
}

void GUIParser::addElement(GUIElement* guiElement)
{
	if (elements.find(guiElement->getName()) != elements.end()) {
		throw GUIParserException(
			"Element with given name '" +
			(guiElement->getName()) +
			"' already exists"
		);
	}
	elements.emplace(guiElement->getName(), guiElement);
}

void GUIParser::initialize()
{
	try {
		GUIElement* guiElement = new GUICheckbox();
		addElement(guiElement);
	} catch (Exception& exception) {
		Console::print(string("GUIParser::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	try {
		GUIElement* guiElement = new GUIRadioButton();
		addElement(guiElement);
	} catch (Exception& exception) {
		Console::print(string("GUIParser::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	try {
		GUIElement* guiElement = new GUISelectBox();
		addElement(guiElement);
	} catch (Exception& exception) {
		Console::print(string("GUIParser::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	try {
		GUIElement* guiElement = new GUISelectBoxOption();
		addElement(guiElement);
	} catch (Exception& exception) {
		Console::print(string("GUIParser::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	try {
		GUIElement* guiElement = new GUISelectBoxMultiple();
		addElement(guiElement);
	} catch (Exception& exception) {
		Console::print(string("GUIParser::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	try {
		GUIElement* guiElement = new GUISelectBoxMultipleOption();
		addElement(guiElement);
	} catch (Exception& exception) {
		Console::print(string("GUIParser::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	try {
		GUIElement* guiElement = new GUIDropDown();
		addElement(guiElement);
	} catch (Exception& exception) {
		Console::print(string("GUIParser::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	try {
		GUIElement* guiElement = new GUIDropDownOption();
		addElement(guiElement);
	} catch (Exception& exception) {
		Console::print(string("GUIParser::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	try {
		GUIElement* guiElement = new GUITabs();
		addElement(guiElement);
	} catch (Exception& exception) {
		Console::print(string("GUIParser::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	try {
		GUIElement* guiElement = new GUITabsHeader();
		addElement(guiElement);
	} catch (Exception& exception) {
		Console::print(string("GUIParser::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	try {
		GUIElement* guiElement = new GUITab();
		addElement(guiElement);
	} catch (Exception& exception) {
		Console::print(string("GUIParser::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	try {
		GUIElement* guiElement = new GUITabsContent();
		addElement(guiElement);
	} catch (Exception& exception) {
		Console::print(string("GUIParser::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	try {
		GUIElement* guiElement = new GUITabContent();
		addElement(guiElement);
	} catch (Exception& exception) {
		Console::print(string("GUIParser::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	try {
		GUIElement* guiElement = new GUIButton();
		addElement(guiElement);
	} catch (Exception& exception) {
		Console::print(string("GUIParser::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	try {
		GUIElement* guiElement = new GUIInput();
		addElement(guiElement);
	} catch (Exception& exception) {
		Console::print(string("GUIParser::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	try {
		GUIElement* guiElement = new GUIScrollAreaVertical();
		addElement(guiElement);
	} catch (Exception& exception) {
		Console::print(string("GUIParser::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	try {
		GUIElement* guiElement = new GUIScrollAreaHorizontal();
		addElement(guiElement);
	} catch (Exception& exception) {
		Console::print(string("GUIParser::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	try {
		GUIElement* guiElement = new GUIScrollArea();
		addElement(guiElement);
	} catch (Exception& exception) {
		Console::print(string("GUIParser::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	try {
		GUIElement* guiElement = new GUISliderH();
		addElement(guiElement);
	} catch (Exception& exception) {
		Console::print(string("GUIParser::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	try {
		GUIElement* guiElement = new GUISliderV();
		addElement(guiElement);
	} catch (Exception& exception) {
		Console::print(string("GUIParser::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	try {
		GUIElement* guiElement = new GUIKnob();
		addElement(guiElement);
	} catch (Exception& exception) {
		Console::print(string("GUIParser::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	try {
		GUIElement* guiElement = new GUIImageButton();
		addElement(guiElement);
	} catch (Exception& exception) {
		Console::print(string("GUIParser::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	try {
		GUIElement* guiElement = new GUIProgressBar();
		addElement(guiElement);
	} catch (Exception& exception) {
		Console::print(string("GUIParser::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
}

void GUIParser::dispose() {
	for (auto& elementIt: elements) {
		delete elementIt.second;
	}
	elements.clear();
}
