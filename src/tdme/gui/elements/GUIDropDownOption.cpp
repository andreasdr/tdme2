#include <tdme/gui/elements/GUIDropDownOption.h>

#include <string>
#include <unordered_map>

#include <tdme/gui/elements/GUIDropDownOptionController.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemException.h>
#include <tdme/os/filesystem/FileSystemInterface.h>

using std::string;
using std::unordered_map;

using tdme::gui::elements::GUIDropDownOption;
using tdme::gui::elements::GUIDropDownOptionController;
using tdme::gui::nodes::GUIScreenNode;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemException;
using tdme::os::filesystem::FileSystemInterface;

string GUIDropDownOption::NAME = "dropdown-option";

GUIDropDownOption::GUIDropDownOption()
{
}

const string& GUIDropDownOption::getName()
{
	return NAME;
}

const string GUIDropDownOption::getTemplate(const string& pathName, const string& fileName)
{
	return FileSystem::getInstance()->getContentAsString(pathName + "/resources/engine/gui/definitions", fileName.empty() == true?"dropdown-option.xml":fileName);
}

unordered_map<string, string> GUIDropDownOption::getAttributes(GUIScreenNode* screenNode)
{
	unordered_map<string, string> attributes;
	attributes["id"] = screenNode->allocateNodeId();
	return attributes;
}

GUINodeController* GUIDropDownOption::createController(GUINode* node)
{
	return new GUIDropDownOptionController(node);
}
