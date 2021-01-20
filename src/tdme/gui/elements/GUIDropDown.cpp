#include <tdme/gui/elements/GUIDropDown.h>

#include <string>
#include <unordered_map>

#include <tdme/gui/elements/GUIDropDownController.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemException.h>
#include <tdme/os/filesystem/FileSystemInterface.h>

using tdme::gui::elements::GUIDropDown;

using std::string;
using std::unordered_map;

using tdme::gui::elements::GUIDropDownController;
using tdme::gui::nodes::GUIScreenNode;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemException;
using tdme::os::filesystem::FileSystemInterface;

string GUIDropDown::NAME = "dropdown";

GUIDropDown::GUIDropDown()
{
}

const string& GUIDropDown::getName()
{
	return NAME;
}

const string GUIDropDown::getTemplate(const string& pathName, const string& fileName)
{
	return FileSystem::getInstance()->getContentAsString(pathName + "/resources/engine/gui/definitions", fileName.empty() == true?"dropdown.xml":fileName);
}

unordered_map<string, string> GUIDropDown::getAttributes(GUIScreenNode* screenNode)
{
	unordered_map<string, string> attributes;
	attributes["id"] = screenNode->allocateNodeId();
	attributes["width"] = "100%";
	attributes["height"] = "auto";
	attributes["text"] = "10";
	return attributes;
}

GUINodeController* GUIDropDown::createController(GUINode* node)
{
	return new GUIDropDownController(node);
}

