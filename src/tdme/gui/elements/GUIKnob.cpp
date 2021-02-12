#include <tdme/gui/elements/GUIKnob.h>

#include <string>
#include <unordered_map>

#include <tdme/gui/elements/GUIKnobController.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemException.h>
#include <tdme/os/filesystem/FileSystemInterface.h>

using std::string;
using std::unordered_map;

using tdme::gui::elements::GUIKnob;
using tdme::gui::elements::GUIKnobController;
using tdme::gui::nodes::GUIScreenNode;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemException;
using tdme::os::filesystem::FileSystemInterface;

string GUIKnob::NAME = "knob";

GUIKnob::GUIKnob()
{
}

const string& GUIKnob::getName()
{
	return NAME;
}

const string GUIKnob::getTemplate(const string& applicationPathName, const string& applicationSubPathName, const string& fileName)
{
	return FileSystem::getInstance()->getContentAsString(applicationPathName + "/resources/" + applicationSubPathName + "/gui/definitions", fileName.empty() == true?"knob.xml":fileName);
}

unordered_map<string, string> GUIKnob::getAttributes(GUIScreenNode* screenNode)
{
	unordered_map<string, string> attributes;
	attributes["id"] = screenNode->allocateNodeId();
	attributes["disabled"] = "false";
	attributes["value"] = "0.0";
	return attributes;
}

GUINodeController* GUIKnob::createController(GUINode* node)
{
	return new GUIKnobController(node);
}

