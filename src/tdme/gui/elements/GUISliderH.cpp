#include <tdme/gui/elements/GUISliderH.h>

#include <string>
#include <unordered_map>

#include <tdme/gui/elements/GUISliderHController.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemException.h>
#include <tdme/os/filesystem/FileSystemInterface.h>

using std::string;
using std::unordered_map;

using tdme::gui::elements::GUISliderH;
using tdme::gui::elements::GUISliderHController;
using tdme::gui::nodes::GUIScreenNode;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemException;
using tdme::os::filesystem::FileSystemInterface;

string GUISliderH::NAME = "slider-horizontal";

GUISliderH::GUISliderH()
{
}

const string& GUISliderH::getName()
{
	return NAME;
}

const string GUISliderH::getTemplate(const string& applicationPathName, const string& applicationSubPathName, const string& fileName)
{
	return FileSystem::getInstance()->getContentAsString(applicationPathName + "/resources/" + applicationSubPathName + "/gui/definitions", fileName.empty() == true?"slider-h.xml":fileName);
}

unordered_map<string, string> GUISliderH::getAttributes(GUIScreenNode* screenNode)
{
	unordered_map<string, string> attributes;
	attributes["id"] = screenNode->allocateNodeId();
	attributes["disabled"] = "false";
	attributes["value"] = "0.0";
	return attributes;
}

GUINodeController* GUISliderH::createController(GUINode* node)
{
	return new GUISliderHController(node);
}

