#include <tdme/gui/elements/GUISliderV.h>

#include <string>
#include <unordered_map>

#include <tdme/gui/elements/GUISliderVController.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemException.h>
#include <tdme/os/filesystem/FileSystemInterface.h>

using std::string;
using std::unordered_map;

using tdme::gui::elements::GUISliderV;
using tdme::gui::elements::GUISliderVController;
using tdme::gui::nodes::GUIScreenNode;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemException;
using tdme::os::filesystem::FileSystemInterface;

string GUISliderV::NAME = "slider-vertical";

GUISliderV::GUISliderV()
{
}

const string& GUISliderV::getName()
{
	return NAME;
}

const string GUISliderV::getTemplate(const string& applicationPathName, const string& applicationSubPathName, const string& fileName)
{
	return FileSystem::getInstance()->getContentAsString(applicationPathName + "/resources/" + applicationSubPathName + "/gui/definitions", fileName.empty() == true?"slider-v.xml":fileName);
}

unordered_map<string, string> GUISliderV::getAttributes(GUIScreenNode* screenNode)
{
	unordered_map<string, string> attributes;
	attributes["id"] = screenNode->allocateNodeId();
	attributes["disabled"] = "false";
	attributes["value"] = "0.0";
	return attributes;
}

GUINodeController* GUISliderV::createController(GUINode* node)
{
	return new GUISliderVController(node);
}

