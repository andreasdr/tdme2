#include <tdme/gui/GUI.h>

#if defined(__APPLE__)
	#include <Carbon/Carbon.h>
#endif

#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <vector>

#include <tdme/engine/Engine.h>
#include <tdme/engine/fileio/textures/Texture.h>
#include <tdme/engine/fileio/textures/TextureReader.h>
#include <tdme/gui/GUIParserException.h>
#include <tdme/gui/events/GUIInputEventHandler.h>
#include <tdme/gui/events/GUIKeyboardEvent.h>
#include <tdme/gui/events/GUIMouseEvent.h>
#include <tdme/gui/nodes/GUIColor.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUINode_Border.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUINodeConditions.h>
#include <tdme/gui/nodes/GUINodeController.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/gui/renderer/GUIFont.h>
#include <tdme/gui/renderer/GUIRenderer.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemException.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/utilities/Time.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>

using std::map;
using std::remove;
using std::set;
using std::string;
using std::to_string;
using std::vector;

using tdme::gui::GUI;
using tdme::engine::Engine;
using tdme::engine::fileio::textures::Texture;
using tdme::engine::fileio::textures::TextureReader;
using tdme::gui::GUIParserException;
using tdme::gui::events::GUIInputEventHandler;
using tdme::gui::events::GUIKeyboardEvent;
using tdme::gui::events::GUIMouseEvent;
using tdme::gui::nodes::GUIColor;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINode_Border;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeConditions;
using tdme::gui::nodes::GUINodeController;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::renderer::GUIFont;
using tdme::gui::renderer::GUIRenderer;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemException;
using tdme::os::filesystem::FileSystemInterface;
using tdme::utilities::Time;
using tdme::utilities::Console;
using tdme::utilities::Exception;

map<string, GUIFont*>* GUI::fontCache = new map<string, GUIFont*>();
map<string, Texture*>* GUI::imageCache = new map<string, Texture*>();

GUI::GUI(Engine* engine, GUIRenderer* guiRenderer)
{
	this->mouseButtonLast = 0;
	this->engine = engine;
	this->guiRenderer = guiRenderer;
	this->width = 0;
	this->height = 0;
	try {
		this->foccussedBorderColor = GUIColor("#8080FF");
	} catch (Exception& exception) {
		Console::print(string("GUI(): An error occurred: "));
		Console::println(string(exception.what()));
	}
}

GUI::~GUI() {
}

int32_t GUI::getWidth()
{
	return width;
}

int32_t GUI::getHeight()
{
	return height;
}

void GUI::initialize()
{
}

void GUI::reshape(int32_t width, int32_t height)
{
	this->width = width;
	this->height = height;
	for (auto screenIt: screens) screenIt.second->reshapeRequested = true;
}

void GUI::dispose()
{
	reset();
}

void GUI::lockEvents()
{
}

void GUI::unlockEvents()
{
}

vector<GUIMouseEvent>& GUI::getMouseEvents()
{
	return mouseEvents;
}

vector<GUIKeyboardEvent>& GUI::getKeyboardEvents()
{
	return keyboardEvents;
}

GUIFont* GUI::getFont(const string& applicationRootPath, const string& fileName)
{
	string canonicalFile;
	string path;
	string file;
	GUIFont* font = nullptr;
	try {
		canonicalFile = FileSystem::getInstance()->getCanonicalPath(applicationRootPath, fileName);
		path = FileSystem::getInstance()->getPathName(canonicalFile);
		file = FileSystem::getInstance()->getFileName(canonicalFile);
	} catch (Exception& exception) {
		Console::print(string("GUI::getFont(): An error occurred: "));
		Console::println(string(exception.what()));
		return nullptr;
	}

	auto fontCacheIt = fontCache->find(canonicalFile);
	if (fontCacheIt == fontCache->end()) {
		try {
			font = GUIFont::parse(path, file);
		} catch (Exception& exception) {
			Console::print(string("GUI::getFont(): An error occurred: "));
			Console::println(string(exception.what()));
			return nullptr;
		}
		(*fontCache)[canonicalFile] = font;
	} else {
		font = fontCacheIt->second;
	}
	return font;
}

Texture* GUI::getImage(const string& applicationRootPath, const string& fileName)
{
	// TODO: fix me, proper get path, filename
	string canonicalFile;
	string path;
	string file;
	try {
		canonicalFile = FileSystem::getInstance()->getCanonicalPath(applicationRootPath, fileName);
		path = FileSystem::getInstance()->getPathName(canonicalFile);
		file = FileSystem::getInstance()->getFileName(canonicalFile);
	} catch (Exception& exception) {
		Console::print(string("GUI::getImage(): An error occurred: "));
		Console::println(string(exception.what()));
		return nullptr;
	}

	auto imageIt = imageCache->find(canonicalFile);
	auto image = imageIt != imageCache->end() ? imageIt->second : nullptr;
	if (image == nullptr) {
		try {
			image = TextureReader::read(path, file, false, false);
			image->setUseMipMap(false);
			image->setRepeat(false);
		} catch (Exception& exception) {
			Console::print(string("GUI::getImage(): An error occurred: "));
			Console::println(string(exception.what()));
			throw;
		}
		(*imageCache)[canonicalFile] = image;
	}
	return image;
}

GUIScreenNode* GUI::getScreen(const string& id)
{
	auto screensIt = screens.find(id);
	if (screensIt == screens.end()) {
		return nullptr;
	}
	return screensIt->second;
}

void GUI::addScreen(const string& id, GUIScreenNode* screen)
{
	if (id != screen->getId()) {
		Console::println("GUI::addScreen(): screen id '" + id + "' must be '" + screen->getId() + "'");
		return;
	}
	screens.emplace(id, screen);
	reshapeScreen(screen);
}

void GUI::removeScreen(const string& id)
{
	auto screensIt = screens.find(id);
	if (screensIt != screens.end()) {
		auto screen = screensIt->second;
		screens.erase(screensIt);
		mouseOutCandidateEventNodeIds.erase(id);
		mouseOutClickCandidateEventNodeIds.erase(id);
		mousePressedEventNodeIds.erase(id);
		mouseDraggingEventNodeIds.erase(id);
		mouseIsDragging.erase(id);
		delete screen;
	}
}

void GUI::reset()
{
	vector<string> entitiesToRemove;
	for (auto screenKeysIt: screens) {
		entitiesToRemove.push_back(screenKeysIt.first);
	}
	for (auto i = 0; i < entitiesToRemove.size(); i++) {
		removeScreen(entitiesToRemove[i]);
	}
	for (auto fontCacheIt: *fontCache) {
		delete fontCacheIt.second;
	}
	fontCache->clear();
	for (auto imageCacheIt: *imageCache) {
		imageCacheIt.second->releaseReference();
	}
	imageCache->clear();
}

void GUI::resetRenderScreens()
{
	// focussed node
	unfocusNode();
	focussedNodeScreenId.clear();
	focussedNodeNodeId.clear();

	// unset GUI
	for (auto i = 0; i < renderScreens.size(); i++) {
		renderScreens[i]->setGUI(nullptr);
	}

	// clear
	renderScreens.clear();
}

bool GUI::hasRenderScreen(const string& screenId) {
	for (auto renderScreen: renderScreens) {
		if (renderScreen->getId() == screenId) return true;
	}
	return false;
}

void GUI::addRenderScreen(const string& screenId)
{
	auto screenIt = screens.find(screenId);
	if (screenIt == screens.end()) return;

	//
	screenIt->second->setGUI(this);
	screenIt->second->setConditionsMet();
	renderScreens.push_back(screenIt->second);

	// focussed node
	focussedNodeScreenId.clear();
	focussedNodeNodeId.clear();
	determineFocussedNodes();
}

void GUI::removeRenderScreen(const string& screenId)
{
	auto screenIt = screens.find(screenId);
	if (screenIt == screens.end()) return;

	//
	screenIt->second->setGUI(nullptr);

	//
	renderScreens.erase(remove(renderScreens.begin(), renderScreens.end(), screenIt->second), renderScreens.end());
}

GUIColor& GUI::getFoccussedBorderColor()
{
	return foccussedBorderColor;
}

void GUI::invalidateFocussedNode()
{
	unfocusNode();
	focussedNodeScreenId.clear();
	focussedNodeNodeId.clear();
}

void GUI::determineFocussedNodes()
{
	focusableNodes.clear();
	focusableScreenNodes.clear();
	for (int32_t i = renderScreens.size() - 1; i >= 0; i--) {
		auto screen = renderScreens[i];
		if (screen->isVisible() == false)
			continue;

		focusableScreenNodes.push_back(screen);
		if (screen->isPopUp() == true)
			break;

	}
	for (int32_t i = focusableScreenNodes.size() - 1; i >= 0; i--) {
		auto screen = focusableScreenNodes[i];
		screen->determineFocussedNodes(screen, focusableNodes);
	}
}

GUIElementNode* GUI::getFocussedNode()
{
	if (focussedNodeScreenId.empty() == true || focussedNodeNodeId.empty() == true) return nullptr;
	auto focussedNodeScreen = getScreen(focussedNodeScreenId);
	auto focussedNode = dynamic_cast<GUIElementNode*>(focussedNodeScreen != nullptr?focussedNodeScreen->getNodeById(focussedNodeNodeId):nullptr);
	return focussedNode;
}

void GUI::unfocusNode()
{
	if (focussedNodeScreenId.empty() == true || focussedNodeNodeId.empty() == true) return;
	auto focussedNodeScreen = getScreen(focussedNodeScreenId);
	auto focussedNode = dynamic_cast<GUIElementNode*>(focussedNodeScreen != nullptr?focussedNodeScreen->getNodeById(focussedNodeNodeId):nullptr);
	if (focussedNode != nullptr) {
		focussedNode->getActiveConditions().remove(GUIElementNode::CONDITION_FOCUS);
		focussedNode->getBorder().topColor = unfocussedNodeBorderTopColor;
		focussedNode->getBorder().leftColor = unfocussedNodeBorderLeftColor;
		focussedNode->getBorder().bottomColor = unfocussedNodeBorderBottomColor;
		focussedNode->getBorder().rightColor = unfocussedNodeBorderRightColor;
		if (focussedNode->getController() != nullptr) focussedNode->getController()->onFocusLost();
		focussedNode->getScreenNode()->delegateUnfocus(focussedNode);
	}
}

void GUI::focusNode()
{
	auto focussedNodeScreen = getScreen(focussedNodeScreenId);
	auto focussedNode = dynamic_cast<GUIElementNode*>(focussedNodeScreen != nullptr?focussedNodeScreen->getNodeById(focussedNodeNodeId):nullptr);
	if (focussedNode != nullptr) {
		focussedNode->getActiveConditions().add(GUIElementNode::CONDITION_FOCUS);
		unfocussedNodeBorderTopColor = focussedNode->getBorder().topColor;
		unfocussedNodeBorderLeftColor = focussedNode->getBorder().leftColor;
		unfocussedNodeBorderBottomColor = focussedNode->getBorder().bottomColor;
		unfocussedNodeBorderRightColor = focussedNode->getBorder().rightColor;
		focussedNode->getBorder().topColor = foccussedBorderColor;
		focussedNode->getBorder().leftColor = foccussedBorderColor;
		focussedNode->getBorder().bottomColor = foccussedBorderColor;
		focussedNode->getBorder().rightColor = foccussedBorderColor;
		if (focussedNode->getController() != nullptr) focussedNode->getController()->onFocusGained();
		focussedNode->getScreenNode()->delegateFocus(focussedNode);
	}
}

void GUI::setFoccussedNode(GUIElementNode* newFoccussedNode)
{
	auto focussedNodeScreen = getScreen(focussedNodeScreenId);
	auto focussedNode = dynamic_cast<GUIElementNode*>(focussedNodeScreen != nullptr?focussedNodeScreen->getNodeById(focussedNodeNodeId):nullptr);
	if (focussedNode == newFoccussedNode) return;
	unfocusNode();
	this->focussedNodeScreenId = newFoccussedNode == nullptr?string():newFoccussedNode->getScreenNode()->getId();
	this->focussedNodeNodeId = newFoccussedNode == nullptr?string():newFoccussedNode->getId();
	if (newFoccussedNode != nullptr) focusNode();
	determineFocussedNodes();
}

void GUI::focusNextNode()
{
	determineFocussedNodes();
	unfocusNode();
	auto focussedNodeScreen = getScreen(focussedNodeScreenId);
	auto focussedNode = dynamic_cast<GUIElementNode*>(focussedNodeScreen != nullptr?focussedNodeScreen->getNodeById(focussedNodeNodeId):nullptr);
	if (focusableNodes.size() > 0) {
		auto focussedNodeIdx = -1;
		for (auto i = 0; i < focusableNodes.size(); i++) {
			if (focussedNode == focusableNodes[i]) {
				focussedNodeIdx = i;
			}
		}
		auto focussedNextNodeIdx = (focussedNodeIdx + 1) % focusableNodes.size();
		auto newFocussedNode = focusableNodes[focussedNextNodeIdx];
		setFoccussedNode(newFocussedNode);
		newFocussedNode->scrollToNodeX();
		newFocussedNode->scrollToNodeY();
	}
}

void GUI::focusPreviousNode()
{
	determineFocussedNodes();
	unfocusNode();
	auto focussedNodeScreen = getScreen(focussedNodeScreenId);
	auto focussedNode = dynamic_cast<GUIElementNode*>(focussedNodeScreen != nullptr?focussedNodeScreen->getNodeById(focussedNodeNodeId):nullptr);
	if (focusableNodes.size() > 0) {
		auto focussedNodeIdx = -1;
		for (auto i = 0; i < focusableNodes.size(); i++) {
			if (focussedNode == focusableNodes[i]) {
				focussedNodeIdx = i;
			}
		}
		int32_t focussedPreviousNodeIdx = (focussedNodeIdx - 1) % focusableNodes.size();
		if (focussedPreviousNodeIdx < 0) focussedPreviousNodeIdx += focusableNodes.size();
		auto newFocussedNode = focusableNodes[focussedPreviousNodeIdx];
		setFoccussedNode(newFocussedNode);
		newFocussedNode->scrollToNodeX();
		newFocussedNode->scrollToNodeY();
	}
}

void GUI::render()
{
	if (renderScreens.empty() == true)
		return;

	if (focussedNodeScreenId.empty() == true || focussedNodeNodeId.empty() == true) {
		focusNextNode();
	}

	guiRenderer->setGUI(this);
	engine->initGUIMode();
	guiRenderer->initRendering();
	for (auto i = 0; i < renderScreens.size(); i++) {
		auto screen = renderScreens[i];

		if (screen->isVisible() == false) continue;

		// reshape if requested
		if (screen->reshapeRequested == true) reshapeScreen(screen);

		screen->render(guiRenderer);
	}
	for (auto i = 0; i < renderScreens.size(); i++) {
		auto screen = renderScreens[i];
		if (screen->isVisible() == false) continue;

		screen->renderFloatingNodes(guiRenderer);
	}
	guiRenderer->doneRendering();
	engine->doneGUIMode();
}

bool GUI::isHavingMouseInteraction(GUINode* node) {
	auto nodeId = node->getId();
	auto screenNodeId = node->getScreenNode()->getId();
	return
		mousePressedEventNodeIds[screenNodeId].find(nodeId) != mousePressedEventNodeIds[screenNodeId].end() ||
		mouseDraggingEventNodeIds[screenNodeId].find(nodeId) != mouseDraggingEventNodeIds[screenNodeId].end();
}

void GUI::handleMouseEvent(GUINode* node, GUIMouseEvent* event, const set<string>& mouseOutCandidateEventNodeIds, const set<string>& mouseOutClickCandidateEventNodeIds, set<string>& mousePressedEventNodeIds, bool floatingNodes)
{
	// handle each event
	set<string> mouseEventNodeIgnore;
	set<string> mouseEventNodeIds;

	//
	event->setX((float)event->getXUnscaled() * (float)node->getScreenNode()->getScreenWidth() / (float)width + node->getScreenNode()->getGUIEffectOffsetX());
	event->setY((float)event->getYUnscaled() * (float)node->getScreenNode()->getScreenHeight() / (float)height + node->getScreenNode()->getGUIEffectOffsetY());

	// if dragging only send events to dragging origin nodes
	if (event->getType() == GUIMouseEvent::MOUSEEVENT_DRAGGED) {
		mouseEventNodeIds = mouseDraggingEventNodeIds[node->getScreenNode()->getId()];
	} else
	if (event->getType() == GUIMouseEvent::MOUSEEVENT_RELEASED &&
		mouseIsDragging[node->getScreenNode()->getId()] == true) {
		mouseEventNodeIds = mouseDraggingEventNodeIds[node->getScreenNode()->getId()];
	} else {
		// otherwise continue with determining nodes that receive this event
		if (floatingNodes == true) {
			node->determineMouseEventNodes(event, node->flow == GUINode_Flow::FLOATING, mouseEventNodeIgnore, mouseEventNodeIds);
		} else {
			node->determineMouseEventNodes(event, node->flow == GUINode_Flow::FLOATING, mouseEventNodeIds, mouseEventNodeIgnore);
		}
	}

	// inject former mouse out candidates
	for (auto eventNodeId: mouseOutCandidateEventNodeIds) {
		mouseEventNodeIds.insert(eventNodeId);
	}

	// inject former mouse out click candidates
	for (auto eventNodeId: mouseOutClickCandidateEventNodeIds) {
		mouseEventNodeIds.insert(eventNodeId);
	}

	// handle mouse event for each node we collected
	for (auto eventNodeId: mouseEventNodeIds) {
		// node event occurred on
		auto eventNode = node->getScreenNode()->getNodeById(eventNodeId);
		if (eventNode == nullptr) continue;
		if (floatingNodes == false && eventNode->flow == GUINode_Flow::FLOATING) continue;

		// controller node
		auto controllerNode = eventNode;
		if (controllerNode->getController() == nullptr) {
			controllerNode = controllerNode->getParentControllerNode();
		}
		if (controllerNode == nullptr) continue;

		// handle event with determined controller
		controllerNode->getController()->handleMouseEvent(eventNode, event);
	}

	// compile list of gui node ids that received mouse PRESSED events
	if (event->getType() == GUIMouseEvent::MOUSEEVENT_PRESSED) {
		for (auto eventNodeId: mouseEventNodeIds) {
			mousePressedEventNodeIds.insert(eventNodeId);
		}
	}

	// clear event node id list
	mouseEventNodeIds.clear();
}

void GUI::handleKeyboardEvent(GUIKeyboardEvent* event) {
	// skip if already processed
	if (event->isProcessed() == true) return;

	//
	switch (event->getKeyCode()) {
		case (GUIKeyboardEvent::KEYCODE_TAB):
			{
				if (event->getType() == GUIKeyboardEvent::KEYBOARDEVENT_KEY_PRESSED) {
					focusNextNode();
				}
				event->setProcessed(true);
				break;
			}
		case (GUIKeyboardEvent::KEYCODE_TAB_SHIFT):
			{
				if (event->getType() == GUIKeyboardEvent::KEYBOARDEVENT_KEY_PRESSED) {
					focusPreviousNode();
				}
				event->setProcessed(true);
				break;
			}
		default:
			{
				break;
			}
	}

	// skip if processed
	if (event->isProcessed() == true) return;

	// otherwise dispatch event to focussed node
	if (focussedNodeScreenId.empty() == false && focussedNodeNodeId.empty() == false) {
		auto focussedNodeScreen = getScreen(focussedNodeScreenId);
		auto focussedNode = dynamic_cast<GUIElementNode*>(focussedNodeScreen != nullptr?focussedNodeScreen->getNodeById(focussedNodeNodeId):nullptr);
		if (focussedNode != nullptr) focussedNode->handleKeyboardEvent(event);
	}
}

void GUI::handleEvents()
{
	lockEvents();

	map<string, set<string>> _mouseOutCandidateEventNodeIds;
	map<string, set<string>> _mouseOutClickCandidateEventNodeIds;

	// handle mouse events
	for (auto& event: mouseEvents) {

		// fetch and clear last mouse out candidates as we now will actually send them
		if (event.getType() == GUIMouseEvent::MOUSEEVENT_MOVED) {
			_mouseOutCandidateEventNodeIds = mouseOutCandidateEventNodeIds;
			mouseOutCandidateEventNodeIds.clear();
		} else
		if (event.getType() == GUIMouseEvent::MOUSEEVENT_PRESSED || event.getType() == GUIMouseEvent::MOUSEEVENT_RELEASED) {
			_mouseOutClickCandidateEventNodeIds = mouseOutClickCandidateEventNodeIds;
			mouseOutClickCandidateEventNodeIds.clear();
		}


		// handle mouse dragged event
		if (event.getType() == GUIMouseEvent::MOUSEEVENT_DRAGGED) {
			for (int32_t i = renderScreens.size() - 1; i >= 0; i--) {
				auto screen = renderScreens[i];
				if (mouseIsDragging[screen->getId()] == false) {
					mouseIsDragging[screen->getId()] = true;
					mouseDraggingEventNodeIds[screen->getId()] = mousePressedEventNodeIds[screen->getId()];
				}
			}
		}

		// handle floating nodes first
		for (int32_t i = renderScreens.size() - 1; i >= 0; i--) {
			auto screen = renderScreens[i];

			// skip on invisible
			if (screen->isVisible() == false) continue;

			//
			auto& floatingNodes = screen->getFloatingNodes();
			for (auto j = 0; j < floatingNodes.size(); j++) {
				auto floatingNode = floatingNodes.at(j);

				handleMouseEvent(floatingNode, &event, _mouseOutCandidateEventNodeIds[screen->getId()], _mouseOutClickCandidateEventNodeIds[screen->getId()], mousePressedEventNodeIds[screen->getId()], true);

				// do not continue handling mouse events if already processed
				if (event.isProcessed() == true) break;
			}
			// do not continue handling mouse events if already processed
			if (event.isProcessed() == true) break;
			// skip on pop ups
			if (screen->isPopUp() == true) break;
		}

		// handle normal screen nodes if not processed already by floating node
		// 	Note: Different screens should not have UI elements that overlap and process events
		if (event.isProcessed() == false) {
			for (int32_t i = renderScreens.size() - 1; i >= 0; i--) {
				auto screen = renderScreens[i];
				if (screen->isVisible() == false) continue;
				handleMouseEvent(screen, &event, _mouseOutCandidateEventNodeIds[screen->getId()], _mouseOutClickCandidateEventNodeIds[screen->getId()], mousePressedEventNodeIds[screen->getId()], false);
				if (screen->isPopUp() == true) break;
			}
		}

		// handle mouse released event
		if (event.getType() == GUIMouseEvent::MOUSEEVENT_RELEASED) {
			for (int32_t i = renderScreens.size() - 1; i >= 0; i--) {
				auto screen = renderScreens[i];
				mouseIsDragging[screen->getId()] = false;
				mouseDraggingEventNodeIds.erase(screen->getId());
				mousePressedEventNodeIds.erase(screen->getId());
			}
		}
	}

	// handle keyboard events
	for (auto& event: keyboardEvents) {
		handleKeyboardEvent(&event);
	}

	// call tick and input event handler at very last
	for (int32_t i = renderScreens.size() - 1; i >= 0; i--) {
		auto screen = renderScreens[i];
		if (screen->isVisible() == false) continue;
		screen->tick();
		if (screen->getInputEventHandler() != nullptr) {
			screen->getInputEventHandler()->handleInputEvents();
		}
		if (screen->isPopUp() == true) break;
	}

	//
	mouseEvents.clear();
	keyboardEvents.clear();
	unlockEvents();

	// invalidate layouts
	for (int32_t i = renderScreens.size() - 1; i >= 0; i--) {
		auto screen = renderScreens[i];
		screen->invalidateLayouts();
	}
}

void GUI::onChar(unsigned int key, int x, int y) {
	fakeMouseMovedEvent();
	lockEvents();
	GUIKeyboardEvent guiKeyboardEvent;
	guiKeyboardEvent.setTime(Time::getCurrentMillis());
	guiKeyboardEvent.setType(GUIKeyboardEvent::KEYBOARDEVENT_KEY_TYPED);
	guiKeyboardEvent.setKeyCode(key);
	guiKeyboardEvent.setKeyChar(key);
	guiKeyboardEvent.setMetaDown(false);
	guiKeyboardEvent.setControlDown((InputEventHandler::getKeyboardModifiers() &  KEYBOARD_MODIFIER_CTRL) == KEYBOARD_MODIFIER_CTRL);
	guiKeyboardEvent.setAltDown((InputEventHandler::getKeyboardModifiers() &  KEYBOARD_MODIFIER_ALT) == KEYBOARD_MODIFIER_ALT);
	guiKeyboardEvent.setShiftDown((InputEventHandler::getKeyboardModifiers() &  KEYBOARD_MODIFIER_SHIFT) == KEYBOARD_MODIFIER_SHIFT);
	guiKeyboardEvent.setProcessed(false);
	keyboardEvents.push_back(guiKeyboardEvent);
	unlockEvents();
}

void GUI::onKeyDown (unsigned char key, int x, int y) {
	fakeMouseMovedEvent();
	lockEvents();
	GUIKeyboardEvent guiKeyboardEvent;
	guiKeyboardEvent.setTime(Time::getCurrentMillis());
	guiKeyboardEvent.setType(GUIKeyboardEvent::KEYBOARDEVENT_KEY_PRESSED);
	guiKeyboardEvent.setKeyCode(GUIKeyboardEvent::getKeyCodeFromChar(key));
	guiKeyboardEvent.setKeyChar(key);
	guiKeyboardEvent.setMetaDown(false);
	guiKeyboardEvent.setControlDown((InputEventHandler::getKeyboardModifiers() &  KEYBOARD_MODIFIER_CTRL) == KEYBOARD_MODIFIER_CTRL);
	guiKeyboardEvent.setAltDown((InputEventHandler::getKeyboardModifiers() &  KEYBOARD_MODIFIER_ALT) == KEYBOARD_MODIFIER_ALT);
	guiKeyboardEvent.setShiftDown((InputEventHandler::getKeyboardModifiers() &  KEYBOARD_MODIFIER_SHIFT) == KEYBOARD_MODIFIER_SHIFT);
	guiKeyboardEvent.setProcessed(false);
	keyboardEvents.push_back(guiKeyboardEvent);
	unlockEvents();
}

void GUI::onKeyUp(unsigned char key, int x, int y) {
	fakeMouseMovedEvent();
	lockEvents();
	GUIKeyboardEvent guiKeyboardEvent;
	guiKeyboardEvent.setTime(Time::getCurrentMillis());
	guiKeyboardEvent.setType(GUIKeyboardEvent::KEYBOARDEVENT_KEY_RELEASED);
	guiKeyboardEvent.setKeyCode(GUIKeyboardEvent::getKeyCodeFromChar(key));
	guiKeyboardEvent.setKeyChar(key);
	guiKeyboardEvent.setMetaDown(false);
	guiKeyboardEvent.setControlDown((InputEventHandler::getKeyboardModifiers() &  KEYBOARD_MODIFIER_CTRL) == KEYBOARD_MODIFIER_CTRL);
	guiKeyboardEvent.setAltDown((InputEventHandler::getKeyboardModifiers() &  KEYBOARD_MODIFIER_ALT) == KEYBOARD_MODIFIER_ALT);
	guiKeyboardEvent.setShiftDown((InputEventHandler::getKeyboardModifiers() &  KEYBOARD_MODIFIER_SHIFT) == KEYBOARD_MODIFIER_SHIFT);
	guiKeyboardEvent.setProcessed(false);
	keyboardEvents.push_back(guiKeyboardEvent);
	unlockEvents();
}

void GUI::onSpecialKeyDown (int key, int x, int y) {
	fakeMouseMovedEvent();
	lockEvents();
	GUIKeyboardEvent guiKeyboardEvent;
	guiKeyboardEvent.setTime(Time::getCurrentMillis());
	guiKeyboardEvent.setType(GUIKeyboardEvent::KEYBOARDEVENT_KEY_PRESSED);
	guiKeyboardEvent.setKeyCode(key);
	guiKeyboardEvent.setKeyChar(-1);
	guiKeyboardEvent.setMetaDown(false);
	guiKeyboardEvent.setControlDown((InputEventHandler::getKeyboardModifiers() &  KEYBOARD_MODIFIER_CTRL) == KEYBOARD_MODIFIER_CTRL);
	guiKeyboardEvent.setAltDown((InputEventHandler::getKeyboardModifiers() &  KEYBOARD_MODIFIER_ALT) == KEYBOARD_MODIFIER_ALT);
	guiKeyboardEvent.setShiftDown((InputEventHandler::getKeyboardModifiers() &  KEYBOARD_MODIFIER_SHIFT) == KEYBOARD_MODIFIER_SHIFT);
	guiKeyboardEvent.setProcessed(false);
	keyboardEvents.push_back(guiKeyboardEvent);
	unlockEvents();
}

void GUI::onSpecialKeyUp(int key, int x, int y) {
	fakeMouseMovedEvent();
	lockEvents();
	GUIKeyboardEvent guiKeyboardEvent;
	guiKeyboardEvent.setTime(Time::getCurrentMillis());
	guiKeyboardEvent.setType(GUIKeyboardEvent::KEYBOARDEVENT_KEY_RELEASED);
	guiKeyboardEvent.setKeyCode(key);
	guiKeyboardEvent.setKeyChar(-1);
	guiKeyboardEvent.setMetaDown(false);
	guiKeyboardEvent.setControlDown((InputEventHandler::getKeyboardModifiers() &  KEYBOARD_MODIFIER_CTRL) == KEYBOARD_MODIFIER_CTRL);
	guiKeyboardEvent.setAltDown((InputEventHandler::getKeyboardModifiers() &  KEYBOARD_MODIFIER_ALT) == KEYBOARD_MODIFIER_ALT);
	guiKeyboardEvent.setShiftDown((InputEventHandler::getKeyboardModifiers() &  KEYBOARD_MODIFIER_SHIFT) == KEYBOARD_MODIFIER_SHIFT);
	guiKeyboardEvent.setProcessed(false);
	keyboardEvents.push_back(guiKeyboardEvent);
	unlockEvents();
}

void GUI::onMouseDragged(int x, int y) {
	fakeKeyboardModifierEvent();

	lockEvents();
	GUIMouseEvent guiMouseEvent;
	guiMouseEvent.setTime(Time::getCurrentMillis());
	guiMouseEvent.setType(GUIMouseEvent::MOUSEEVENT_DRAGGED);
	guiMouseEvent.setXUnscaled(x);
	guiMouseEvent.setYUnscaled(y);
	guiMouseEvent.setX(x);
	guiMouseEvent.setY(y);
	guiMouseEvent.setButton(mouseButtonLast);
	guiMouseEvent.setWheelX(0.0f);
	guiMouseEvent.setWheelY(0.0f);
	guiMouseEvent.setWheelZ(0.0f);
	#if defined(__APPLE__) && !defined(VULKAN) && !defined(GLFW3)
		guiMouseEvent.setControlDown(false);
		guiMouseEvent.setAltDown(false);
		guiMouseEvent.setShiftDown(false);
	#else
		guiMouseEvent.setControlDown((InputEventHandler::getKeyboardModifiers() &  KEYBOARD_MODIFIER_CTRL) == KEYBOARD_MODIFIER_CTRL);
		guiMouseEvent.setAltDown((InputEventHandler::getKeyboardModifiers() &  KEYBOARD_MODIFIER_ALT) == KEYBOARD_MODIFIER_ALT);
		guiMouseEvent.setShiftDown((InputEventHandler::getKeyboardModifiers() &  KEYBOARD_MODIFIER_SHIFT) == KEYBOARD_MODIFIER_SHIFT);
	#endif
	guiMouseEvent.setProcessed(false);
	mouseEvents.push_back(guiMouseEvent);
	unlockEvents();
}

void GUI::onMouseMoved(int x, int y) {
	fakeKeyboardModifierEvent();

	lockEvents();
	GUIMouseEvent guiMouseEvent;
	guiMouseEvent.setTime(Time::getCurrentMillis());
	guiMouseEvent.setType(GUIMouseEvent::MOUSEEVENT_MOVED);
	guiMouseEvent.setXUnscaled(x);
	guiMouseEvent.setYUnscaled(y);
	guiMouseEvent.setX(x);
	guiMouseEvent.setY(y);
	guiMouseEvent.setButton(0);
	guiMouseEvent.setWheelX(0.0f);
	guiMouseEvent.setWheelY(0.0f);
	guiMouseEvent.setWheelZ(0.0f);
	#if defined(__APPLE__) && !defined(VULKAN) && !defined(GLFW3)
		guiMouseEvent.setControlDown(false);
		guiMouseEvent.setAltDown(false);
		guiMouseEvent.setShiftDown(false);
	#else
		guiMouseEvent.setControlDown((InputEventHandler::getKeyboardModifiers() &  KEYBOARD_MODIFIER_CTRL) == KEYBOARD_MODIFIER_CTRL);
		guiMouseEvent.setAltDown((InputEventHandler::getKeyboardModifiers() &  KEYBOARD_MODIFIER_ALT) == KEYBOARD_MODIFIER_ALT);
		guiMouseEvent.setShiftDown((InputEventHandler::getKeyboardModifiers() &  KEYBOARD_MODIFIER_SHIFT) == KEYBOARD_MODIFIER_SHIFT);
	#endif
	guiMouseEvent.setProcessed(false);
	mouseEvents.push_back(guiMouseEvent);
	unlockEvents();
}

void GUI::onMouseButton(int button, int state, int x, int y) {
	fakeKeyboardModifierEvent();

	lockEvents();
	mouseButtonLast = button + 1;
	GUIMouseEvent guiMouseEvent;
	guiMouseEvent.setTime(Time::getCurrentMillis());
	guiMouseEvent.setType(state == MOUSE_BUTTON_DOWN?GUIMouseEvent::MOUSEEVENT_PRESSED:GUIMouseEvent::MOUSEEVENT_RELEASED);
	guiMouseEvent.setXUnscaled(x);
	guiMouseEvent.setYUnscaled(y);
	guiMouseEvent.setX(x);
	guiMouseEvent.setY(y);
	guiMouseEvent.setButton(mouseButtonLast);
	guiMouseEvent.setWheelX(0.0f);
	guiMouseEvent.setWheelY(0.0f);
	guiMouseEvent.setWheelZ(0.0f);
	#if defined(__APPLE__) && !defined(VULKAN) && !defined(GLFW3)
		guiMouseEvent.setControlDown(false);
		guiMouseEvent.setAltDown(false);
		guiMouseEvent.setShiftDown(false);
	#else
		guiMouseEvent.setControlDown((InputEventHandler::getKeyboardModifiers() &  KEYBOARD_MODIFIER_CTRL) == KEYBOARD_MODIFIER_CTRL);
		guiMouseEvent.setAltDown((InputEventHandler::getKeyboardModifiers() &  KEYBOARD_MODIFIER_ALT) == KEYBOARD_MODIFIER_ALT);
		guiMouseEvent.setShiftDown((InputEventHandler::getKeyboardModifiers() &  KEYBOARD_MODIFIER_SHIFT) == KEYBOARD_MODIFIER_SHIFT);
	#endif
	guiMouseEvent.setProcessed(false);
	mouseEvents.push_back(guiMouseEvent);
	unlockEvents();
}

void GUI::onMouseWheel(int button, int direction, int x, int y) {
	fakeKeyboardModifierEvent();

	lockEvents();
	mouseButtonLast = button + 1;
	GUIMouseEvent guiMouseEvent;
	guiMouseEvent.setTime(Time::getCurrentMillis());
	guiMouseEvent.setType(GUIMouseEvent::MOUSEEVENT_WHEEL_MOVED);
	guiMouseEvent.setXUnscaled(x);
	guiMouseEvent.setYUnscaled(y);
	guiMouseEvent.setX(x);
	guiMouseEvent.setY(y);
	guiMouseEvent.setButton(mouseButtonLast);
	guiMouseEvent.setWheelX(0.0f);
	guiMouseEvent.setWheelY(direction * 1.0f);
	guiMouseEvent.setWheelZ(0.0f);
	#if defined(__APPLE__) && !defined(VULKAN) && !defined(GLFW3)
		guiMouseEvent.setControlDown(false);
		guiMouseEvent.setAltDown(false);
		guiMouseEvent.setShiftDown(false);
	#else
		guiMouseEvent.setControlDown((InputEventHandler::getKeyboardModifiers() &  KEYBOARD_MODIFIER_CTRL) == KEYBOARD_MODIFIER_CTRL);
		guiMouseEvent.setAltDown((InputEventHandler::getKeyboardModifiers() &  KEYBOARD_MODIFIER_ALT) == KEYBOARD_MODIFIER_ALT);
		guiMouseEvent.setShiftDown((InputEventHandler::getKeyboardModifiers() &  KEYBOARD_MODIFIER_SHIFT) == KEYBOARD_MODIFIER_SHIFT);
	#endif
	guiMouseEvent.setProcessed(false);
	mouseEvents.push_back(guiMouseEvent);
	unlockEvents();
}

void GUI::fakeMouseMovedEvent()
{
	lockEvents();
	GUIMouseEvent guiMouseEvent;
	guiMouseEvent.setTime(Time::getCurrentMillis());
	guiMouseEvent.setType(GUIMouseEvent::MOUSEEVENT_MOVED);
	guiMouseEvent.setXUnscaled(-10000);
	guiMouseEvent.setYUnscaled(-10000);
	guiMouseEvent.setX(-10000);
	guiMouseEvent.setY(-10000);
	guiMouseEvent.setButton(0);
	guiMouseEvent.setWheelX(0.0f);
	guiMouseEvent.setWheelY(0.0f);
	guiMouseEvent.setWheelZ(0.0f);
	guiMouseEvent.setProcessed(false);
	mouseEvents.push_back(guiMouseEvent);
	unlockEvents();
}

void GUI::fakeKeyboardModifierEvent() {
	bool isControlDown = false;
	bool isAltDown = false;
	bool isShiftDown = false;
	#if defined(__linux__) || defined(_WIN32)
		isControlDown = (InputEventHandler::getKeyboardModifiers() &  KEYBOARD_MODIFIER_CTRL) == KEYBOARD_MODIFIER_CTRL;
		isAltDown = (InputEventHandler::getKeyboardModifiers() &  KEYBOARD_MODIFIER_ALT) == KEYBOARD_MODIFIER_ALT;
		isShiftDown = (InputEventHandler::getKeyboardModifiers() &  KEYBOARD_MODIFIER_SHIFT) == KEYBOARD_MODIFIER_SHIFT;
	#elif defined(__APPLE__)
		KeyMap keys;
		GetKeys(keys);
		#define IS_KEY_DOWN(key, var) \
		{ \
			uint8_t index = key / 32 ; \
			uint8_t shift = key % 32 ; \
			var = keys[index].bigEndianValue & (1 << shift); \
		}
		IS_KEY_DOWN(kVK_Command, isControlDown);
		IS_KEY_DOWN(kVK_Option, isAltDown);
		IS_KEY_DOWN(kVK_Shift, isShiftDown);
	#endif

	if (isControlDown == false &&
		isAltDown == false &&
		isShiftDown == false) {
		return;
	}

	lockEvents();
	GUIKeyboardEvent guiKeyboardEvent;
	guiKeyboardEvent.setTime(Time::getCurrentMillis());
	guiKeyboardEvent.setType(GUIKeyboardEvent::KEYBOARDEVENT_KEY_PRESSED);
	guiKeyboardEvent.setKeyCode(-1);
	guiKeyboardEvent.setKeyChar(-1);
	guiKeyboardEvent.setMetaDown(false);
	guiKeyboardEvent.setControlDown(isControlDown);
	guiKeyboardEvent.setAltDown(isAltDown);
	guiKeyboardEvent.setShiftDown(isShiftDown);
	guiKeyboardEvent.setProcessed(false);
	keyboardEvents.push_back(guiKeyboardEvent);
	unlockEvents();
}

void GUI::reshapeScreen(GUIScreenNode* screenNode) {
	// TODO: maybe move logic into GUIScreenNode
	// FIXME: always keep aspect ratio
	auto screenNodeWidthConstrained = width;
	auto screenNodeHeightConstrained = height;

	auto minRatio = 1.0f;

	if ((screenNode->sizeConstraints.maxWidth > 0 && screenNodeWidthConstrained > screenNode->sizeConstraints.maxWidth) ||
		(screenNode->sizeConstraints.maxHeight > 0 && screenNodeHeightConstrained > screenNode->sizeConstraints.maxHeight)) {
		if (screenNode->sizeConstraints.maxWidth > 0 && screenNodeWidthConstrained > screenNode->sizeConstraints.maxWidth) {
			minRatio = (float)screenNode->sizeConstraints.maxWidth / (float)width;
			screenNodeWidthConstrained = screenNode->sizeConstraints.maxWidth;
			screenNodeHeightConstrained = (int)((float)screenNodeHeightConstrained * minRatio);
		}
		if (screenNode->sizeConstraints.maxHeight > 0 && screenNodeHeightConstrained > screenNode->sizeConstraints.maxHeight) {
			minRatio = (float)screenNode->sizeConstraints.maxHeight / (float)height;
			screenNodeHeightConstrained = screenNode->sizeConstraints.maxHeight;
			screenNodeWidthConstrained = (int)((float)screenNodeWidthConstrained * minRatio);
		}
	}
	if ((screenNode->sizeConstraints.minWidth > 0 && screenNodeWidthConstrained < screenNode->sizeConstraints.minWidth) ||
		(screenNode->sizeConstraints.minHeight > 0 && screenNodeHeightConstrained < screenNode->sizeConstraints.minHeight)) {
		if (screenNode->sizeConstraints.minWidth > 0 && screenNodeWidthConstrained < screenNode->sizeConstraints.minWidth) {
			auto ratio = (float)screenNode->sizeConstraints.minWidth / (float)width;
			screenNodeWidthConstrained = screenNode->sizeConstraints.minWidth;
			screenNodeHeightConstrained = (int)((float)screenNodeHeightConstrained * ratio / minRatio);
		}
		if (screenNode->sizeConstraints.minHeight > 0 && screenNodeHeightConstrained < screenNode->sizeConstraints.minHeight) {
			auto ratio = (float)screenNode->sizeConstraints.minHeight / (float)height;
			screenNodeHeightConstrained = screenNode->sizeConstraints.minHeight;
			screenNodeWidthConstrained = (int)((float)screenNodeWidthConstrained * ratio / minRatio);
		}
	}

	screenNode->setScreenSize(screenNodeWidthConstrained, screenNodeHeightConstrained);
	screenNode->reshapeRequested = false;
}

void GUI::addMouseOutCandidateElementNode(GUINode* node) {
	mouseOutCandidateEventNodeIds[node->getScreenNode()->getId()].insert(node->getId());
}

void GUI::addMouseOutClickCandidateElementNode(GUINode* node) {
	mouseOutClickCandidateEventNodeIds[node->getScreenNode()->getId()].insert(node->getId());
}
