#include <tdme/gui/elements/GUITabContent.h>

#include <string>
#include <unordered_map>

#include <tdme/gui/elements/GUITabContentController.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemException.h>
#include <tdme/os/filesystem/FileSystemInterface.h>

using std::string;
using std::unordered_map;

using tdme::gui::elements::GUITabContent;
using tdme::gui::elements::GUITabContentController;
using tdme::gui::nodes::GUIScreenNode;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemException;
using tdme::os::filesystem::FileSystemInterface;

string GUITabContent::NAME = "tab-content";

GUITabContent::GUITabContent()
{
}

const string& GUITabContent::getName()
{
	return NAME;
}

const string GUITabContent::getTemplate(const string& pathName, const string& fileName)
{
	return FileSystem::getInstance()->getContentAsString(pathName + "/resources/engine/gui/definitions", fileName.empty() == true?"tab-content.xml":fileName);
}

unordered_map<string, string> GUITabContent::getAttributes(GUIScreenNode* screenNode)
{
	unordered_map<string, string> attributes;
	attributes["id"] = screenNode->allocateNodeId();
	attributes["horizontal-align"] = "center";
	attributes["vertical-align"] = "center";
	return attributes;
}

GUINodeController* GUITabContent::createController(GUINode* node)
{
	return new GUITabContentController(node);
}

