#include <tdme/gui/elements/GUISelectBoxMultipleParentOption.h>

#include <string>
#include <unordered_map>

#include <tdme/gui/elements/GUISelectBoxMultipleParentOptionController.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemException.h>
#include <tdme/os/filesystem/FileSystemInterface.h>

using std::string;
using std::unordered_map;

using tdme::gui::elements::GUISelectBoxMultipleParentOption;
using tdme::gui::elements::GUISelectBoxMultipleParentOptionController;
using tdme::gui::nodes::GUIScreenNode;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemException;
using tdme::os::filesystem::FileSystemInterface;

string GUISelectBoxMultipleParentOption::NAME = "selectbox-multiple-parent-option";

GUISelectBoxMultipleParentOption::GUISelectBoxMultipleParentOption()
{
}

const string& GUISelectBoxMultipleParentOption::getName()
{
	return NAME;
}

const string GUISelectBoxMultipleParentOption::getTemplate(const string& pathName, const string& fileName)
{
	return FileSystem::getInstance()->getContentAsString(pathName + "/resources/gui-system/definitions/elements", fileName.empty() == true?"selectbox-multiple-parent-option.xml":fileName);
}

unordered_map<string, string> GUISelectBoxMultipleParentOption::getAttributes(GUIScreenNode* screenNode)
{
	unordered_map<string, string> attributes;
	attributes["id"] = screenNode->allocateNodeId();
	return attributes;
}

GUINodeController* GUISelectBoxMultipleParentOption::createController(GUINode* node)
{
	return new GUISelectBoxMultipleParentOptionController(node);
}

