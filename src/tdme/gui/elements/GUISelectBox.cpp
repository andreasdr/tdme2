#include <tdme/gui/elements/GUISelectBox.h>

#include <string>
#include <unordered_map>

#include <tdme/gui/elements/GUISelectBoxController.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemException.h>
#include <tdme/os/filesystem/FileSystemInterface.h>

using std::string;
using std::unordered_map;

using tdme::gui::elements::GUISelectBox;
using tdme::gui::elements::GUISelectBoxController;
using tdme::gui::nodes::GUIScreenNode;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemException;
using tdme::os::filesystem::FileSystemInterface;

string GUISelectBox::NAME = "selectbox";

GUISelectBox::GUISelectBox()
{
}

const string& GUISelectBox::getName()
{
	return NAME;
}

const string GUISelectBox::getTemplate(const string& pathName, const string& fileName)
{
	return FileSystem::getInstance()->getContentAsString(pathName + "/resources/engine/gui/definitions", fileName.empty() == true?"selectbox.xml":fileName);
}

unordered_map<string, string> GUISelectBox::getAttributes(GUIScreenNode* screenNode)
{
	unordered_map<string, string> attributes;
	attributes["id"] = screenNode->allocateNodeId();
	attributes["width"] = "100%";
	attributes["height"] = "auto";
	return attributes;
}

GUINodeController* GUISelectBox::createController(GUINode* node)
{
	return new GUISelectBoxController(node);
}

