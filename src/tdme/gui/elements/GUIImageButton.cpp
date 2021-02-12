#include <tdme/gui/elements/GUIImageButton.h>

#include <string>
#include <unordered_map>

#include <tdme/gui/elements/GUIButtonController.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemException.h>
#include <tdme/os/filesystem/FileSystemInterface.h>

using std::string;
using std::unordered_map;

using tdme::gui::elements::GUIButtonController;
using tdme::gui::elements::GUIImageButton;
using tdme::gui::nodes::GUIScreenNode;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemException;
using tdme::os::filesystem::FileSystemInterface;

string GUIImageButton::NAME = "image-button";

GUIImageButton::GUIImageButton()
{
}

const string& GUIImageButton::getName()
{
	return NAME;
}

const string GUIImageButton::getTemplate(const string& applicationPathName, const string& applicationSubPathName, const string& fileName)
{
	return FileSystem::getInstance()->getContentAsString(applicationPathName + "/resources/" + applicationSubPathName + "/gui/definitions", fileName.empty() == true?"image-button.xml":fileName);
}

unordered_map<string, string> GUIImageButton::getAttributes(GUIScreenNode* screenNode)
{
	unordered_map<string, string> attributes;
	attributes["id"] = screenNode->allocateNodeId();
	attributes["width"] = "auto";
	attributes["height"] = "auto";
	return attributes;
}

GUINodeController* GUIImageButton::createController(GUINode* node)
{
	return new GUIButtonController(node);
}

