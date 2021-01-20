#include <tdme/gui/elements/GUIScrollArea.h>

#include <string>
#include <unordered_map>

#include <tdme/gui/elements/GUIScrollAreaController.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemException.h>
#include <tdme/os/filesystem/FileSystemInterface.h>

using std::string;
using std::unordered_map;

using tdme::gui::elements::GUIScrollArea;
using tdme::gui::elements::GUIScrollAreaController;
using tdme::gui::nodes::GUIScreenNode;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemException;
using tdme::os::filesystem::FileSystemInterface;

string GUIScrollArea::NAME = "scrollarea";

GUIScrollArea::GUIScrollArea()
{
}

const string& GUIScrollArea::getName()
{
	return NAME;
}

const string GUIScrollArea::getTemplate(const string& pathName, const string& fileName)
{
	return FileSystem::getInstance()->getContentAsString(pathName + "/resources/engine/gui/definitions", fileName.empty() == true?"scrollarea.xml":fileName);
}

unordered_map<string, string> GUIScrollArea::getAttributes(GUIScreenNode* screenNode)
{
	unordered_map<string, string> attributes;
	attributes["id"] = screenNode->allocateNodeId();
	attributes["width"] = "100%";
	attributes["height"] = "100%";
	attributes["horizontal-align"] = "left";
	attributes["vertical-align"] = "top";
	attributes["alignment"] = "vertical";
	attributes["background-color"] = "transparent";
	return attributes;
}

GUINodeController* GUIScrollArea::createController(GUINode* node)
{
	return new GUIScrollAreaController(node);
}

