#include <tdme/gui/elements/GUIMenuHeaderItem.h>

#include <string>
#include <unordered_map>

#include <tdme/gui/elements/GUIMenuHeaderItemController.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemException.h>
#include <tdme/os/filesystem/FileSystemInterface.h>

using tdme::gui::elements::GUIMenuHeaderItem;

using std::string;
using std::unordered_map;

using tdme::gui::elements::GUIMenuHeaderItemController;
using tdme::gui::nodes::GUIScreenNode;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemException;
using tdme::os::filesystem::FileSystemInterface;

string GUIMenuHeaderItem::NAME = "menu-header-item";

GUIMenuHeaderItem::GUIMenuHeaderItem()
{
}

const string& GUIMenuHeaderItem::getName()
{
	return NAME;
}

const string GUIMenuHeaderItem::getTemplate(const string& pathName, const string& fileName)
{
	return FileSystem::getInstance()->getContentAsString(pathName + "/resources/engine/gui/definitions", fileName.empty() == true?"menu-header-item.xml":fileName);
}

unordered_map<string, string> GUIMenuHeaderItem::getAttributes(GUIScreenNode* screenNode)
{
	unordered_map<string, string> attributes;
	attributes["id"] = screenNode->allocateNodeId();
	attributes["text"] = "10";
	return attributes;
}

GUINodeController* GUIMenuHeaderItem::createController(GUINode* node)
{
	return new GUIMenuHeaderItemController(node);
}

