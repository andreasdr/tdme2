#include <tdme/gui/elements/GUISelectBoxParentOption.h>

#include <string>
#include <unordered_map>

#include <tdme/gui/elements/GUISelectBoxParentOptionController.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemException.h>
#include <tdme/os/filesystem/FileSystemInterface.h>

using std::string;
using std::unordered_map;

using tdme::gui::elements::GUISelectBoxParentOption;
using tdme::gui::elements::GUISelectBoxParentOptionController;
using tdme::gui::nodes::GUIScreenNode;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemException;
using tdme::os::filesystem::FileSystemInterface;

string GUISelectBoxParentOption::NAME = "selectbox-parent-option";

GUISelectBoxParentOption::GUISelectBoxParentOption()
{
}

const string& GUISelectBoxParentOption::getName()
{
	return NAME;
}

const string GUISelectBoxParentOption::getTemplate(const string& pathName, const string& fileName)
{
	return FileSystem::getInstance()->getContentAsString(pathName + "/resources/engine/gui/definitions", fileName.empty() == true?"selectbox-parent-option.xml":fileName);
}

unordered_map<string, string> GUISelectBoxParentOption::getAttributes(GUIScreenNode* screenNode)
{
	unordered_map<string, string> attributes;
	attributes["id"] = screenNode->allocateNodeId();
	return attributes;
}

GUINodeController* GUISelectBoxParentOption::createController(GUINode* node)
{
	return new GUISelectBoxParentOptionController(node);
}

