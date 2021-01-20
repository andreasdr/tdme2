#include <tdme/gui/elements/GUICheckbox.h>

#include <string>
#include <unordered_map>

#include <tdme/gui/elements/GUICheckboxController.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemException.h>
#include <tdme/os/filesystem/FileSystemInterface.h>

using std::string;
using std::unordered_map;

using tdme::gui::elements::GUICheckbox;
using tdme::gui::elements::GUICheckboxController;
using tdme::gui::nodes::GUIScreenNode;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemException;
using tdme::os::filesystem::FileSystemInterface;

string GUICheckbox::NAME = "checkbox";

GUICheckbox::GUICheckbox()
{
}

const string& GUICheckbox::getName()
{
	return NAME;
}

const string GUICheckbox::getTemplate(const string& pathName, const string& fileName)
{
	return FileSystem::getInstance()->getContentAsString(pathName + "/resources/engine/gui/definitions", fileName.empty() == true?"checkbox.xml":fileName);
}

unordered_map<string, string> GUICheckbox::getAttributes(GUIScreenNode* screenNode)
{
	unordered_map<string, string> attributes;
	attributes["id"] = screenNode->allocateNodeId();
	return attributes;
}

GUINodeController* GUICheckbox::createController(GUINode* node)
{
	return new GUICheckboxController(node);
}

