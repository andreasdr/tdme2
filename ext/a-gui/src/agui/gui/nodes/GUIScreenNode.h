#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <minitscript/minitscript.h>
#include <minitscript/minitscript/Library.h>
#include <minitscript/minitscript/MinitScript.h>

#include <agui/agui.h>
#include <agui/gui/fileio/fwd-agui.h>
#include <agui/gui/events/fwd-agui.h>
#include <agui/gui/events/GUIActionListener.h>
#include <agui/gui/fwd-agui.h>
#include <agui/gui/nodes/fwd-agui.h>
#include <agui/gui/nodes/GUIColor.h>
#include <agui/gui/nodes/GUIParentNode.h>
#include <agui/gui/nodes/GUIScreenNode_SizeConstraints.h>
#include <agui/gui/renderer/fwd-agui.h>
#include <agui/gui/scripting/fwd-agui.h>
#include <agui/utilities/MutableString.h>

using std::string;
using std::to_string;
using std::unique_ptr;
using std::unordered_map;
using std::unordered_set;
using std::vector;

// namespaces
namespace agui {
namespace gui {
namespace nodes {
	using ::minitscript::minitscript::Library;
	using ::minitscript::minitscript::MinitScript;
	using ::agui::gui::textures::GUITexture;
	using ::agui::gui::events::GUIActionListener;
	using ::agui::gui::events::GUIActionListenerType;
	using ::agui::gui::events::GUIChangeListener;
	using ::agui::gui::events::GUIContextMenuRequestListener;
	using ::agui::gui::events::GUIDragRequestListener;
	using ::agui::gui::events::GUIFocusListener;
	using ::agui::gui::events::GUIInputEventHandler;
	using ::agui::gui::events::GUIKeyboardEvent;
	using ::agui::gui::events::GUIMouseEvent;
	using ::agui::gui::events::GUIMouseOverListener;
	using ::agui::gui::events::GUIMoveListener;
	using ::agui::gui::events::GUITooltipRequestListener;
	using ::agui::gui::renderer::GUIFont;
	using ::agui::gui::renderer::GUIRenderer;
	using ::agui::gui::scripting::GUIMinitScript;
	using ::agui::gui::GUI;
	using ::agui::utilities::MutableString;
}
}
}

/**
 * GUI screen node that represents a screen that can be rendered via GUI system
 * @author Andreas Drewke
 */
class agui::gui::nodes::GUIScreenNode final
	: public GUIParentNode
{
	friend class agui::gui::GUI;
	friend class agui::gui::GUIParser;
	friend class GUIElementNode;
	friend class GUINode;
	friend class GUINodeConditions;
	friend class GUIParentNode;
	friend class agui::gui::scripting::GUIMinitScript;

private:
	string applicationRootPathName;
	string applicationSubPathName;
	string fileName;
	GUI* gui { nullptr };
	int nodeCounter;
	int screenWidth;
	int screenHeight;
	unordered_map<string, GUINode*> nodesById;
	unordered_map<string, GUINode*> tickNodesById;
	vector<GUINode*> floatingNodes;
	vector<GUIActionListener*> actionListener;
	vector<GUIChangeListener*> changeListener;
	vector<GUIMouseOverListener*> mouseOverListener;
	vector<GUIContextMenuRequestListener*> contextMenuRequestListener;
	vector<GUIFocusListener*> focusListener;
	vector<GUIMoveListener*> moveListener;
	vector<GUITooltipRequestListener*> tooltipRequestListener;
	vector<GUIDragRequestListener*> dragRequestListener;
	GUIInputEventHandler* inputEventHandler;
	vector<GUINode*> childControllerNodes;
	GUIScreenNode_SizeConstraints sizeConstraints;
	unordered_set<string> invalidateLayoutNodeIds;
	unordered_map<string, unordered_set<string>> elementNodeToNodeMapping;

	bool enabled;
	bool popUp;

	unordered_map<int64_t, string> timedExpressions;

	struct ScrollToNodeStruct {
		ScrollToNodeStruct(
			const string& node,
			const string& toNode
		):
			node(node),
			toNode(toNode)
		{}
		string node;
		string toNode;
	};

	vector<ScrollToNodeStruct> scrollToNodesX;
	vector<ScrollToNodeStruct> scrollToNodesY;

	unordered_map<string, GUIFont*> fontCache;
	unordered_map<string, GUITexture*> imageCache;

	struct ForwardEvent {
		enum EventType {
			EVENTTYPE_NONE,
			EVENTTYPE_ACTION,
			EVENTTYPE_CHANGE,
			EVENTTYPE_MOUSEOVER,
			EVENTTYPE_CONTEXTMENUREQUEST,
			EVENTTYPE_FOCUS,
			EVENTTYPE_UNFOCUS,
			EVENTTYPE_MOVE,
			EVENTTYPE_MOVERELEASE,
			EVENTTYPE_TOOLTIPSHOWREQUEST,
			EVENTTYPE_TOOLTIPCLOSEREQUEST,
			EVENTTYPE_DRAGREQUEST
		};
		ForwardEvent(
			EventType eventType,
			const string& nodeId,
			int mouseX,
			int mouseY,
			int type
		):
			eventType(eventType),
			nodeId(nodeId),
			mouseX(mouseX),
			mouseY(mouseY),
			type(type)
		{}
		EventType eventType { EVENTTYPE_NONE };
		string nodeId;
		int mouseX { -1 };
		int mouseY { -1 };
		int type { -1 };
	};

	vector<ForwardEvent> forwardEventList;

	GUIColor foccussedBorderColor;

	unique_ptr<GUIMinitScript> script;
	bool scriptOnActionAvailable { false };
	bool scriptOnChangeAvailable { false };
	bool scriptOnMouseOverAvailable { false };
	bool scriptOnContextMenuRequestAvailable { false };
	bool scriptOnFocusAvailable { false };
	bool scriptOnUnfocusAvailable { false };
	bool scriptOnMoveAvailable { false };
	bool scriptOnMoveReleaseAvailable { false };
	bool scriptOnTooltipShowRequestAvailable { false };
	bool scriptOnTooltipCloseRequestAvailable { false };
	bool scriptOnDragRequestAvailable { false };
	bool scriptOnTickAvailable { false };

	MinitScript::Variable scriptArguments;

public:
	/**
	 * @returns screen filename or complete file path
	 */
	inline const string& getFileName() {
		return fileName;
	}

	/**
	 * @returns application root path name
	 */
	inline const string& getApplicationRootPathName() {
		return applicationRootPathName;
	}

	/**
	 * @returns application sub path path name
	 */
	inline const string& getApplicationSubPathName() {
		return applicationSubPathName;
	}

	/**
	 * @returns GUI
	 */
	GUI* getGUI();

	/**
	 * Set GUI
	 * @param gui gui
	 */
	void setGUI(GUI* gui);

	/**
	 * @returns screen width
	 */
	inline int getScreenWidth() {
		return screenWidth;
	}

	/**
	 * @returns screen height
	 */
	inline int getScreenHeight() {
		return screenHeight;
	}

	/**
	 * @returns is enabled
	 */
	inline bool isEnabled() {
		return enabled;
	}

	/**
	 * Set enabled
	 * @param enabled enabled
	 */
	void setEnabled(bool enabled);

	/**
	 * @returns is pop up
	 */
	inline bool isPopUp() {
		return popUp;
	}

	/**
	 * Set pop up
	 * @param popUp pop up
	 */
	void setPopUp(bool popUp);

	/**
	 * @returns floating nodes
	 */
	const vector<GUINode*>& getFloatingNodes();

	/**
	 * Register deferred scroll to node X
	 * @param node node
	 * @param toNode to node
	 */
	inline void scrollToNodeX(const string& node, const string& toNode) {
		scrollToNodesX.emplace_back(
			node,
			toNode
		);
	}

	/**
	 * Register deferred scroll to node Y
	 * @param node node
	 * @param toNode to node
	 */
	inline void scrollToNodeY(const string& node, const string& toNode) {
		scrollToNodesY.emplace_back(
			node,
			toNode
		);
	}

protected:

	// overridden methods
	bool isContentNode() override;
	const string getNodeType() override;

private:

	/**
	 * Initialize mini script
	 */
	void initializeMinitScript();

	/**
	 * Add node
	 * @param node node
	 * @returns success
	 */
	bool addNode(GUINode* node);

	/**
	 * Add node
	 * @param node node
	 * @returns success
	 */
	bool removeNode(GUINode* node);

	/**
	 * Calls registered tick nodes controller tick method
	 */
	void tick();

	/**
	 * Forward events
	 */
	void forwardEvents();

	/**
	 * Clear events
	 */
	inline void clearEvents() {
		forwardEventList.clear();
	}

public:
	// forbid class copy
	FORBID_CLASS_COPY(GUIScreenNode)

	/**
	 * Constructor
	 * @oaram fileName file name or complete file path
	 * @param applicationRootPath application root path
	 * @param applicationSubPathName application sub path name which is usually "engine" or "project"
	 * @param flow flow
	 * @param overflowX overflow x
	 * @param overflowY overflow y
	 * @param alignments alignments
	 * @param requestedConstraints requested constraints
	 * @param backgroundColor background color
	 * @param backgroundImage background image
	 * @param backgroundImageScale9Grid background image scale 9 grid
	 * @param backgroundImageEffectColorMul background image effect color mul
	 * @param backgroundImageEffectColorAdd background image effect color add
	 * @param border border
	 * @param padding padding
	 * @param sizeConstraints size constraints
	 * @param showOn show on
	 * @param hideOn hide on
	 * @param tooltip tooltip
	 * @param scrollable scrollable
	 * @param popUp pop up
	 * @param scriptLibrary script library
	 * @param scriptFileName script file name
	 * @param scriptArguments script arguments
	 * @throws agui::gui::GUIParserException
	 */
	GUIScreenNode(
		const string& fileName,
		const string& applicationRootPathName,
		const string& applicationSubPathName,
		const string& id,
		GUINode_Flow* flow,
		GUIParentNode_Overflow* overflowX,
		GUIParentNode_Overflow* overflowY,
		const GUINode_Alignments& alignments,
		const GUINode_RequestedConstraints& requestedConstraints,
		const GUIColor& backgroundColor,
		const string& backgroundImage,
		const GUINode_Scale9Grid& backgroundImageScale9Grid,
		const GUIColor& backgroundImageEffectColorMul,
		const GUIColor& backgroundImageEffectColorAdd,
		const GUINode_Border& border,
		const GUINode_Padding& padding,
		const GUIScreenNode_SizeConstraints& sizeConstraints,
		const GUINodeConditions& showOn,
		const GUINodeConditions& hideOn,
		const string& tooltip,
		bool scrollable,
		bool popUp,
		Library* scriptLibrary,
		const string& scriptFileName,
		const MinitScript::Variable& scriptArguments
	);

	/**
	 * Destructor
	 */
	~GUIScreenNode();

	/**
	 * @returns content width
	 */
	int getContentWidth() override;

	/**
	 * @returns content height
	 */
	int getContentHeight() override;

	/**
	 * Layout
	 */
	void layout() override;

	/**
	 * Mark a node to be invalidated regarding layout
	 * @param node node
	 * @returns first node that requires a layout in tree
	 */
	inline void invalidateLayout(GUINode* node) {
		invalidateLayoutNodeIds.insert(node->getId());
	}

	/**
	 * Actually do the invalidate layout
	 * @param node node
	 * @returns (parent)node thats need a layout
	 */
	GUINode* forceInvalidateLayout(GUINode* node);

	/**
	 * Actually do the nodes marked for layout invalidation
	 */
	void invalidateLayouts();

	/**
	 * Force layout node content (e.g. child nodes or content) without determining parent nodes to be layouted
	 * this does also does call layouted nodes post layout method
	 * @param node node
	 */
	void forceLayout(GUINode* node);

	/**
	 * Scroll to nodes
	 */
	void scrollToNodes();

	/**
	 * Set screen size
	 * @param width width
	 * @param height height
	 */
	void setScreenSize(int width, int height);

	/**
	 * Get GUI node by id
	 * @param nodeId nodeId
	 * @returns GUI node or null
	 */
	inline GUINode* getNodeById(const string& nodeId) {
		auto nodesByIdIt = nodesById.find(nodeId);
		if (nodesByIdIt == nodesById.end()) {
			return nullptr;
		}
		return nodesByIdIt->second;
	}

	/**
	 * Remove GUI node by id
	 * @param nodeId nodeId
	 * @param resetScrollOffsets reset scroll offsets
	 */
	void removeNodeById(const string& nodeId, bool resetScrollOffsets);

	/**
	 * Get inner GUI node by id
	 * @param nodeId nodeId
	 * @returns GUI node or null
	 */
	inline GUINode* getInnerNodeById(const string& nodeId) {
		return getNodeById(nodeId + "_inner");
	}

	/**
	 * Allocate node id
	 * @returns node id
	 */
	inline const string allocateNodeId() {
		return "<" + to_string(nodeCounter++) + ">";
	}

	/**
	 * Render screen
	 * @param guiRenderer gui renderer
	 */
	void render(GUIRenderer* guiRenderer) override;

	/**
	 * Render floating nodes
	 * @param guiRenderer gui renderer
	 */
	void renderFloatingNodes(GUIRenderer* guiRenderer);

	/**
	 * Determine focussed nodes
	 * @param parentNode parent node
	 * @param focusableNodes focusable nodes
	 */
	void determineFocussedNodes(GUIParentNode* parentNode, vector<GUIElementNode*>& focusableNodes);

	// overridden methods
	void determineMouseEventNodes(GUIMouseEvent* event, bool floatingNode, unordered_set<string>& eventNodeIds, unordered_set<string>& eventFloatingNodeIds, int flags = DETERMINEMOUSEEVENTNODES_FLAG_NONE) override;

	/**
	 * Add action listener
	 * @param listener listener
	 */
	void addActionListener(GUIActionListener* listener);

	/**
	 * Remove action listener
	 * @param listener listener
	 */
	void removeActionListener(GUIActionListener* listener);

	/**
	 * @returns input event handler
	 */
	GUIInputEventHandler* getInputEventHandler();

	/**
	 * Set input event handler
	 * @param inputEventHandler input event handler
	 */
	void setInputEventHandler(GUIInputEventHandler* inputEventHandler);

	/**
	 * Forward action event
	 * @param type type
	 * @param node node
	 */
	void forwardAction(GUIActionListenerType type, GUIElementNode* node);

	/**
	 * Add change listener
	 * @param listener listener
	 */
	void addChangeListener(GUIChangeListener* listener);

	/**
	 * Remove change listener
	 * @param listener listener
	 */
	void removeChangeListener(GUIChangeListener* listener);

	/**
	 * Forward change event
	 * @param node node
	 */
	void forwardChange(GUIElementNode* node);

	/**
	 * Add mouse over listener
	 * @param listener listener
	 */
	void addMouseOverListener(GUIMouseOverListener* listener);

	/**
	 * Remove mouse over listener
	 * @param listener listener
	 */
	void removeMouseOverListener(GUIMouseOverListener* listener);

	/**
	 * Forward mouse over event
	 * @param node node
	 */
	void forwardMouseOver(GUIElementNode* node);

	/**
	 * Add context menu request listener
	 * @param listener listener
	 */
	void addContextMenuRequestListener(GUIContextMenuRequestListener* listener);

	/**
	 * Remove context menu request listener
	 * @param listener listener
	 */
	void removeContextMenuRequestListener(GUIContextMenuRequestListener* listener);

	/**
	 * Forward context menu request event
	 * @param node node
	 * @param mouseX unscaled mouse X position
	 * @param mouseY unscaled mouse Y position
	 */
	void forwardContextMenuRequest(GUIElementNode* node, int mouseX, int mouseY);

	/**
	 * Add focus listener
	 * @param listener listener
	 */
	void addFocusListener(GUIFocusListener* listener);

	/**
	 * Remove focus listener
	 * @param listener listener
	 */
	void removeFocusListener(GUIFocusListener* listener);

	/**
	 * Forward focus event
	 * @param node node
	 */
	void forwardFocus(GUIElementNode* node);

	/**
	 * Forward unfocus event
	 * @param node node
	 */
	void forwardUnfocus(GUIElementNode* node);

	/**
	 * Add move listener
	 * @param listener listener
	 */
	void addMoveListener(GUIMoveListener* listener);

	/**
	 * Remove move listener
	 * @param listener listener
	 */
	void removeMoveListener(GUIMoveListener* listener);

	/**
	 * Returns if move is accepted by move listener
	 * @param node node
	 * @returns move is accepted
	 */
	bool isMoveAccepted(GUINode* node);

	/**
	 * Forward move event
	 * @param node node
	 */
	void forwardMove(GUINode* node);

	/**
	 * Forward move release event
	 * @param node node
	 * @param mouseX unscaled mouse x
	 * @param mouseY unscaled mouse y
	 */
	void forwardMoveRelease(GUINode* node, int mouseX, int mouseY);

	/**
	 * Add tooltip request listener
	 * @param listener listener
	 */
	void addTooltipRequestListener(GUITooltipRequestListener* listener);

	/**
	 * Remove tooltip request listener
	 * @param listener listener
	 */
	void removeTooltipRequestListener(GUITooltipRequestListener* listener);

	/**
	 * Forward tooltip show request event
	 * @param node node
	 * @param mouseX unscaled mouse X position
	 * @param mouseY unscaled mouse Y position
	 */
	void forwardTooltipShowRequest(GUINode* node, int mouseX, int mouseY);

	/**
	 * Forward tooltip close request event
	 */
	void forwardTooltipCloseRequest();

	/**
	 * Add drag request listener
	 * @param listener listener
	 */
	void addDragRequestListener(GUIDragRequestListener* listener);

	/**
	 * Remove drag request listener
	 * @param listener listener
	 */
	void removeDragRequestListener(GUIDragRequestListener* listener);

	/**
	 * Forward drag request event
	 * @param node node
	 * @param mouseX unscaled mouse X position
	 * @param mouseY unscaled mouse Y position
	 */
	void forwardDragRequest(GUIElementNode* node, int mouseX, int mouseY);

	/**
	 * @returns if haveing given node registered as tick node
	 */
	inline bool hasTickNode(GUINode* node) {
		return tickNodesById.find(node->getId()) != tickNodesById.end();
	}

	/**
	 * Add tick node, registered node controllers will have a tick once per frame
	 * @param node node
	 */
	inline void addTickNode(GUINode* node) {
		tickNodesById[node->getId()] = node;
	}

	/**
	 * Remove tick node
	 * @param node node
	 */
	inline void removeTickNode(GUINode* node) {
		tickNodesById.erase(node->getId());
	}

	/**
	 * Get values
	 * @param values values
	 */
	void getValues(unordered_map<string, MutableString>& values);

	/**
	 * Set values
	 * @param values values
	 */
	void setValues(const unordered_map<string, MutableString>& values);

	/**
	 * @returns size constraints
	 */
	inline GUIScreenNode_SizeConstraints& getSizeConstraints() {
		return sizeConstraints;
	}

	/**
	 * Create size constraints
	 * @param minWidth min width
	 * @param minHeight min height
	 * @param maxWidth max width
	 * @param maxHeight max height
	 */
	static GUIScreenNode_SizeConstraints createSizeConstraints(const string& minWidth, const string& minHeight, const string& maxWidth, const string& maxHeight);

	/**
	 * Add a timed expression
	 * @param time time
	 * @param expression expression
	 */
	inline void addTimedExpression(int64_t time, const string& expression) {
		timedExpressions[time]+= timedExpressions[time].empty() == false?";" + expression:expression;
	}

	/**
	 * Add node to element node dependency
	 * @param elementNodeId element node id
	 * @param nodeId node id that depends on element node condition changes
	 */
	inline void addNodeElementNodeDependency(const string& elementNodeId, const string& nodeId) {
		elementNodeToNodeMapping[elementNodeId].insert(nodeId);
	}

	/**
	 * Get font
	 * @param fileName file name
	 * @param size size
	 * @throws agui::os::filesystem::FileSystemException
	 * @returns font
	 */
	GUIFont* getFont(const string& fileName, int size);

	/**
	 * Get image
	 * @param fileName file name
	 * @throws agui::os::filesystem::FileSystemException
	 * @returns texture
	 */
	GUITexture* getImage(const string& fileName);

	/**
	 * @returns foccussed border color
	 */
	inline const GUIColor& getFoccussedBorderColor() {
		return foccussedBorderColor;
	}

	/**
	 * @returns mini script script attached to this screen
	 */
	inline GUIMinitScript* getMinitScript() {
		return script.get();
	}

	/**
	 * Get project path and filename of given file name
	 * @param fileName project fileName that can be relative to project path or absolute
	 * @param projectFilePathName computed project file path name
	 * @param projectFileFileName computed project file file name
	 */
	void getProjectFilePathNameAndFileName(const string &fileName, string& projectFilePathName, string& projectFileFileName);

};
