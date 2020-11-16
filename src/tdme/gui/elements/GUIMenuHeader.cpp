#include <tdme/gui/elements/GUIMenuHeader.h>

#include <string>
#include <unordered_map>

#include <tdme/gui/elements/GUIMenuHeaderController.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemException.h>
#include <tdme/os/filesystem/FileSystemInterface.h>

using std::string;
using std::unordered_map;

using tdme::gui::elements::GUIMenuHeader;
using tdme::gui::elements::GUIMenuHeaderController;
using tdme::gui::nodes::GUIScreenNode;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemException;
using tdme::os::filesystem::FileSystemInterface;

string GUIMenuHeader::NAME = "menu-header";

GUIMenuHeader::GUIMenuHeader()
{
}

const string& GUIMenuHeader::getName()
{
	return NAME;
}

const string GUIMenuHeader::getTemplate(const string& pathName, const string& fileName)
{
	return FileSystem::getInstance()->getContentAsString(pathName + "/resources/gui-system/definitions/elements", fileName.empty() == true?"menu-header.xml":fileName);
}

unordered_map<string, string> GUIMenuHeader::getAttributes(GUIScreenNode* screenNode)
{
	unordered_map<string, string> attributes;
	attributes["id"] = screenNode->allocateNodeId();
	return attributes;
}

GUINodeController* GUIMenuHeader::createController(GUINode* node)
{
	return new GUIMenuHeaderController(node);
}

