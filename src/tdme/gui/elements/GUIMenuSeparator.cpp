#include <tdme/gui/elements/GUIMenuSeparator.h>

#include <string>
#include <unordered_map>

#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemException.h>
#include <tdme/os/filesystem/FileSystemInterface.h>

using std::string;
using std::unordered_map;

using tdme::gui::elements::GUIMenuSeparator;
using tdme::gui::nodes::GUIScreenNode;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemException;
using tdme::os::filesystem::FileSystemInterface;

string GUIMenuSeparator::NAME = "menu-separator";

GUIMenuSeparator::GUIMenuSeparator()
{
}

const string& GUIMenuSeparator::getName()
{
	return NAME;
}

const string GUIMenuSeparator::getTemplate(const string& pathName, const string& fileName)
{
	return FileSystem::getInstance()->getContentAsString(pathName + "/resources/engine/gui/definitions", fileName.empty() == true?"menu-separator.xml":fileName);
}

unordered_map<string, string> GUIMenuSeparator::getAttributes(GUIScreenNode* screenNode)
{
	unordered_map<string, string> attributes;
	attributes["id"] = screenNode->allocateNodeId();
	return attributes;
}

GUINodeController* GUIMenuSeparator::createController(GUINode* node)
{
	return nullptr;
}
