#include <tdme/gui/elements/GUITabs.h>

#include <string>
#include <unordered_map>

#include <tdme/gui/elements/GUITabsController.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemException.h>
#include <tdme/os/filesystem/FileSystemInterface.h>

using std::string;
using std::unordered_map;

using tdme::gui::elements::GUITabs;
using tdme::gui::elements::GUITabsController;
using tdme::gui::nodes::GUIScreenNode;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemException;
using tdme::os::filesystem::FileSystemInterface;

string GUITabs::NAME = "tabs";

GUITabs::GUITabs()
{
}

const string& GUITabs::getName()
{
	return NAME;
}

const string GUITabs::getTemplate(const string& pathName, const string& fileName)
{
	return FileSystem::getInstance()->getContentAsString(pathName + "/resources/engine/gui/definitions", fileName.empty() == true?"tabs.xml":fileName);
}

unordered_map<string, string> GUITabs::getAttributes(GUIScreenNode* screenNode)
{
	unordered_map<string, string> attributes;
	attributes["id"] = screenNode->allocateNodeId();
	attributes["width"] = "100%";
	attributes["height"] = "*";
	return attributes;
}

GUINodeController* GUITabs::createController(GUINode* node)
{
	return new GUITabsController(node);
}

