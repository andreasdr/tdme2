#include <tdme/gui/elements/GUIMenuItem.h>

#include <string>
#include <unordered_map>

#include <tdme/gui/elements/GUIMenuItemController.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemException.h>
#include <tdme/os/filesystem/FileSystemInterface.h>

using std::string;
using std::unordered_map;

using tdme::gui::elements::GUIMenuItem;
using tdme::gui::elements::GUIMenuItemController;
using tdme::gui::nodes::GUIScreenNode;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemException;
using tdme::os::filesystem::FileSystemInterface;

string GUIMenuItem::NAME = "menu-item";

GUIMenuItem::GUIMenuItem()
{
}

const string& GUIMenuItem::getName()
{
	return NAME;
}

const string GUIMenuItem::getTemplate(const string& pathName, const string& fileName)
{
	return FileSystem::getInstance()->getContentAsString(pathName + "/resources/gui-system/definitions/elements", fileName.empty() == true?"menu-item.xml":fileName);
}

unordered_map<string, string> GUIMenuItem::getAttributes(GUIScreenNode* screenNode)
{
	unordered_map<string, string> attributes;
	attributes["id"] = screenNode->allocateNodeId();
	return attributes;
}

GUINodeController* GUIMenuItem::createController(GUINode* node)
{
	return new GUIMenuItemController(node);
}
