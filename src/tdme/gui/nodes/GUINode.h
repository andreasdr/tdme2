#pragma once

#include <array>
#include <map>
#include <set>
#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/fileio/textures/fwd-tdme.h>
#include <tdme/gui/effects/fwd-tdme.h>
#include <tdme/gui/events/fwd-tdme.h>
#include <tdme/gui/fwd-tdme.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/gui/nodes/GUIColor.h>
#include <tdme/gui/nodes/GUINode_Alignments.h>
#include <tdme/gui/nodes/GUINode_Border.h>
#include <tdme/gui/nodes/GUINode_ComputedConstraints.h>
#include <tdme/gui/nodes/GUINode_Flow.h>
#include <tdme/gui/nodes/GUINode_Flow.h>
#include <tdme/gui/nodes/GUINode_Padding.h>
#include <tdme/gui/nodes/GUINode_RequestedConstraints.h>
#include <tdme/gui/nodes/GUINode_Scale9Grid.h>
#include <tdme/gui/nodes/GUINodeConditions.h>
#include <tdme/gui/renderer/fwd-tdme.h>
#include <tdme/utilities/fwd-tdme.h>

using std::array;
using std::map;
using std::set;
using std::string;
using std::vector;

using tdme::engine::fileio::textures::Texture;
using tdme::gui::effects::GUIEffect;
using tdme::gui::events::GUIKeyboardEvent;
using tdme::gui::events::GUIMouseEvent;
using tdme::gui::nodes::GUIColor;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINode_Alignments;
using tdme::gui::nodes::GUINode_AlignmentHorizontal;
using tdme::gui::nodes::GUINode_AlignmentVertical;
using tdme::gui::nodes::GUINode_Border;
using tdme::gui::nodes::GUINode_ComputedConstraints;
using tdme::gui::nodes::GUINode_Flow;
using tdme::gui::nodes::GUINode_Padding;
using tdme::gui::nodes::GUINode_RequestedConstraints;
using tdme::gui::nodes::GUINode_RequestedConstraints_RequestedConstraintsType;
using tdme::gui::nodes::GUINodeConditions;
using tdme::gui::nodes::GUINodeController;
using tdme::gui::nodes::GUIParentNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::renderer::GUIRenderer;

/**
 * GUI node base class
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::gui::nodes::GUINode
{
	friend class tdme::gui::GUI;
	friend class GUIElementNode;
	friend class GUIImageNode;
	friend class GUILayerNode;
	friend class GUILayoutNode;
	friend class GUIParentNode;
	friend class GUIScreenNode;
	friend class GUIHorizontalScrollbarInternalController;
	friend class GUIVerticalScrollbarInternalController;
	friend class GUIInputInternalController;
	friend class GUINodeConditions;
	friend class GUINode_Flow;
	friend class GUINode_AlignmentHorizontal;
	friend class GUINode_AlignmentVertical;
	friend class GUINode_RequestedConstraints;
	friend class GUINode_RequestedConstraints_RequestedConstraintsType;

private:
	GUINode_Flow* flow;

	/**
	 * Determine element node dependencies
	 * @param elementNodeDependencies element node dependencies
	 */
	void cfDetermineElementNodeDependencies(vector<string>& elementNodeDependencies);

	/**
	 * Parse condition function term
	 * @param term term
	 * @param function function
	 * @param arguments function arguments
	 */
	void cfParse(const string& term, string& function, vector<string>& arguments);

	/**
	 * Call condition function with arguments
	 * @param elementNode element node to work with
	 * @param function function to be called
	 * @param arguments function arguments
	 * @return condition met
	 */
	bool cfCall(GUIElementNode* elementNode, const string& function, const vector<string>& arguments);

	/**
	 * Determine element node dependencies - Call condition function with arguments
	 * @param function function to be called
	 * @param arguments function arguments
	 * @param elementNodeDependencies element node dependencies
	 */
	void cfCallDetermineElementNodeDependencies(const string& function, const vector<string>& arguments, vector<string>& elementNodeDependencies);

	/**
	 * Condition function: empty
	 * @param arguments arguments
	 * 	Argument should look like 'test', "test", '', "", 123, 0, 123.4, 0.0 for now
	 *	Arguments or OR connected
	 * @return if 1 argument has not been empty
	 */
	bool cfEmpty(const vector<string>& arguments);

	/**
	 * Condition function: has condition
	 * @param elementNode element node to work with
	 * @param arguments arguments
	 *	Format of arguments: [elementid.]condition
	 *	Arguments or OR connected
	 * @return if 1 condition has been met
	 */
	bool cfHasCondition(GUIElementNode* elementNode, const vector<string>& arguments);

	/**
	 * Determine element node dependencies - Condition function: has condition
	 * @param arguments arguments
	 *	Format of arguments: [elementid.]condition
	 *	Arguments or OR connected
	 */
	void cfHasConditionDetermineElementNodeDependencies(const vector<string>& arguments, vector<string>& elementNodeDependencies);

protected:
	GUIScreenNode* screenNode { nullptr };
	GUIParentNode* parentNode { nullptr };
	string id;
	GUINode_Alignments alignments;
	GUINode_RequestedConstraints requestedConstraints;
	GUINode_ComputedConstraints computedConstraints;
	GUIColor backgroundColor;
	Texture* backgroundTexture { nullptr };
	int32_t backgroundTextureId;
	GUINode_Scale9Grid backgroundImageScale9Grid;
	GUIColor backgroundImageEffectColorMul;
	GUIColor backgroundImageEffectColorAdd;
	GUINode_Padding padding;
	GUINode_Border border;
	GUINodeConditions showOn;
	GUINodeConditions hideOn;
	GUINodeController* controller { nullptr };
	map<string, GUIEffect*> effects;
	int guiEffectOffsetX;
	int guiEffectOffsetY;
	bool conditionsMet;
	bool layouted;
	bool haveOutEffect;

	/**
	 * Public constructor
	 * @param screenNode screen node
	 * @param parentNode parent node
	 * @param id id
	 * @param flow flow
	 * @param alignments alignments
	 * @param requestedConstraints requested constraints
	 * @param backgroundColor background color
	 * @param backgroundImage background image
	 * @param backgroundImageScale9Grid background image scale 9 grid
	 * @param backgroundImageEffectColorMul background image effect color mul
	 * @param backgroundImageEffectColorAdd background image effect color add
	 * @param border border
	 * @param padding padding
	 * @param showOn show on
	 * @param hideOn hide on
	 */
	GUINode(
		GUIScreenNode* screenNode,
		GUIParentNode* parentNode,
		const string& id,
		GUINode_Flow* flow,
		const GUINode_Alignments& alignments,
		const GUINode_RequestedConstraints& requestedConstraints,
		const GUIColor& backgroundColor,
		const string& backgroundImage,
		const GUINode_Scale9Grid& backgroundImageScale9Grid,
		const GUIColor& backgroundImageEffectColorMul,
		const GUIColor& backgroundImageEffectColorAdd,
		const GUINode_Border& border,
		const GUINode_Padding& padding,
		const GUINodeConditions& showOn,
		const GUINodeConditions& hideOn
	);

	/**
	 * Destructor
	 */
	virtual ~GUINode();
	/**
	 * @return node type
	 */
	virtual const string getNodeType() = 0;

	/**
	 * @return is content node
	 */
	virtual bool isContentNode() = 0;

	/**
	 * Set computed left
	 * @param left left
	 */
	virtual void setLeft(int left);

	/**
	 * Set computed top
	 * @param top top
	 */
	virtual void setTop(int top);

	/**
	 * Layout
	 */
	virtual void layout();

	/**
	 * Do content alignment
	 */
	virtual void computeContentAlignment();

	/**
	 * Layout constraint
	 * @param type type
	 * @param autoValue auto value
	 * @param parentValue parent value
	 * @param value value
	 * @return pixel
	 */
	virtual int layoutConstraintPixel(GUINode_RequestedConstraints_RequestedConstraintsType* type, int autoValue, int parentValue, int value);

	/**
	 * Get requested constraints type
	 * @param constraint constraint
	 * @param defaultConstraintsType default constraints type
	 * @return requested constraints type
	 */
	static GUINode_RequestedConstraints_RequestedConstraintsType* getRequestedConstraintsType(const string& constraint, GUINode_RequestedConstraints_RequestedConstraintsType* defaultConstraintsType);

	/**
	 * Get requested constraints value
	 * @param constraint constraint
	 * @param defaultConstraintsValue default constraints value
	 * @return requested constraints value
	 */
	static int getRequestedConstraintsValue(const string& constraint, int defaultConstraintsValue);

	/**
	 * Get requested pixel value
	 * @param value value
	 * @param defaultValue default value
	 * @return value
	 */
	static int getRequestedPixelValue(const string& value, int defaultValue);

	/**
	 * Check if conditions are met
	 * @return conditions met
	 */
	virtual bool checkConditions();

	/**
	 * @return compute parent children render offset X total
	 */
	virtual float computeParentChildrenRenderOffsetXTotal();

	/**
	 * @return compute children render offset Y total
	 */
	virtual float computeParentChildrenRenderOffsetYTotal();

	/**
	 * On set condition
	 * @param conditions conditions
	 */
	virtual void onSetConditions(const vector<string>& conditions);

	/**
	 * Determine if we have a out effect active
	 */
	virtual bool haveActiveOutEffect();

	/**
	 * Determine if to render
	 * @return if node will be rendered
	 */
	inline virtual bool shouldRender() {
		return conditionsMet == true || haveActiveOutEffect() == true;
	}

public:
	/**
	 * @return scren node
	 */
	virtual GUIScreenNode* getScreenNode();

	/**
	 * @return parent node
	 */
	virtual GUIParentNode* getParentNode();

	/**
	 * @return id
	 */
	virtual const string& getId();

	/**
	 * @return content width including border, margin
	 */
	virtual int getContentWidth() = 0;

	/**
	 * @return content height including border, margin
	 */
	virtual int getContentHeight() = 0;

	/**
	 * @return auto width if auto width requested or content width
	 */
	virtual int getAutoWidth();

	/**
	 * @return auto height if auto height requested or content height
	 */
	virtual int getAutoHeight();

	/**
	 * @return border
	 */
	virtual GUINode_Border& getBorder();

	/**
	 * @return padding
	 */
	virtual GUINode_Padding& getPadding();

	/**
	 * @return requested constraints
	 */
	virtual GUINode_RequestedConstraints& getRequestsConstraints();

	/**
	 * @return computed constraints
	 */
	virtual GUINode_ComputedConstraints& getComputedConstraints();

	/**
	 * Create alignments
	 * @param horizontal horizontal
	 * @param vertical vertical
	 * @return alignments
	 */
	static GUINode_Alignments createAlignments(const string& horizontal, const string& vertical);

	/**
	 * Create requested constraints
	 * @param left left
	 * @param top top
	 * @param width width
	 * @param height height
	 * @return requested constraints
	 */
	static GUINode_RequestedConstraints createRequestedConstraints(const string& left, const string& top, const string& width, const string& height);

	/**
	 * Get color
	 * @param color color
	 * @param defaultColor default color
	 * @throws tdme::gui::GUIParserException
	 * @return value
	 */
	static GUIColor getRequestedColor(const string& color, const GUIColor& defaultColor);

	/**
	 * Create flow
	 * @param flow flow
	 * @return flow
	 */
	static GUINode_Flow* createFlow(const string& flow);

	/**
	 * Create border
	 * @param allBorder all border
	 * @param left left
	 * @param top top
	 * @param right right
	 * @param bottom bottom
	 * @param allBorderColor all border color
	 * @param leftColor left color
	 * @param topColor top color
	 * @param rightColor right color
	 * @param bottomColor bottom color
	 * @return border
	 */
	static GUINode_Border createBorder(const string& allBorder, const string& left, const string& top, const string& right, const string& bottom, const string& allBorderColor, const string& leftColor, const string& topColor, const string& rightColor, const string& bottomColor);

	/**
	 * Create padding
	 * @param allPadding all padding
	 * @param left left
	 * @param top top
	 * @param right right
	 * @param bottom bottom
	 * @return padding
	 */
	static GUINode_Padding createPadding(const string& allPadding, const string& left, const string& top, const string& right, const string& bottom);

	/**
	 * Create scale 9 grid
	 * @param all all
	 * @param left left
	 * @param top top
	 * @param right right
	 * @param bottom bottom
	 * @return scale 9 grid
	 */
	static GUINode_Scale9Grid createScale9Grid(const string& all, const string& left, const string& top, const string& right, const string& bottom);

	/**
	 * Create conditions
	 * @param conditions conditions
	 */
	static GUINodeConditions createConditions(const string& conditions);

	/**
	 * Dispose node
	 */
	virtual void dispose();

	/**
	 * Determine if conditions are set
	 * @return if conditions are set
	 */
	inline bool isConditionsMet() {
		return conditionsMet;
	}

	/**
	 * Set conditions met for this node and its subnodes
	 */
	virtual void setConditionsMet();

	/**
	 * Layout on demand
	 */
	virtual void layoutOnDemand();

	/**
	 * Render
	 * @param guiRenderer gui renderer
	 * @param floatingNodes floating nodes
	 */
	virtual void render(GUIRenderer* guiRenderer);

	/**
	 * Is event belonging to node
	 * @param event event
	 * @param position x,y position in node coordinate system
	 * @return boolean
	 */
	virtual bool isEventBelongingToNode(GUIMouseEvent* event, array<float, 2>& position);

	/**
	 * Is event belonging to node
	 * @param event event
	 * @return boolean
	 */
	virtual bool isEventBelongingToNode(GUIMouseEvent* event);

	/**
	 * Get event off node relative position
	 * 	TODO: use Vector2 instead of array<float, 2>
	 * @param event event
	 * @param position x,y position (will return x = 0 if in node on x axis, will return x < 0  (-pixel) if on the left of element, x > 0 (+pixel) if on the right of element, y behaves analogous to x)
	 * @return void
	 */
	virtual void getEventOffNodeRelativePosition(GUIMouseEvent* event, array<float, 2>& position);

	/**
	 * @return first parent node in tree with controller node attached
	 */
	virtual GUIParentNode* getParentControllerNode();

	/**
	 * Determine mouse event nodes
	 * @param event event
	 * @param floatingNode if node is floating node
	 * @param eventNodeIds event node ids
	 * @param eventFloatingNodeIds event floating node ids
	 */
	virtual void determineMouseEventNodes(GUIMouseEvent* event, bool floatingNode, set<string>& eventNodeIds, set<string>& eventFloatingNodeIds);

	/**
	 * Handle keyboard event
	 * @param event event
	 */
	virtual void handleKeyboardEvent(GUIKeyboardEvent* event);

	/**
	 * @return controller
	 */
	virtual GUINodeController* getController();

	/**
	 * Set up node controller
	 * @param controller controller
	 */
	virtual void setController(GUINodeController* controller);

	/**
	 * Scroll to node Y
	 */
	virtual void scrollToNodeY();

	/**
	 * Scroll to node Y
	 * @param toNode stop at node to node
	 */
	virtual void scrollToNodeY(GUIParentNode* toNode);

	/**
	 * Scroll to node X
	 */
	virtual void scrollToNodeX();

	/**
	 * Scroll to node X
	 * @param toNode stop at node to node
	 */
	virtual void scrollToNodeX(GUIParentNode* toNode);

	/**
	 * Set background image
	 * @param backgroundImage background image
	 */
	virtual void setBackgroundImage(const string& backgroundImage);

	/**
	 * @return GUI effect offset X
	 */
	int getGUIEffectOffsetX();

	/**
	 * Set GUI effect offset X
	 * @param guiEffectOffsetX gui effect offset X
	 */
	void setGUIEffectOffsetX(int guiEffectOffsetX);

	/**
	 * @return GUI effect offset Y
	 */
	int getGUIEffectOffsetY();

	/**
	 * Set GUI effect offset Y
	 * @param guiEffectOffsetY gui effect offset Y
	 */
	void setGUIEffectOffsetY(int guiEffectOffsetY);

	/**
	 * Add effect, effect already registered with the is will be removed
	 * @param id id
	 * @param effect effect
	 */
	void addEffect(const string& id, GUIEffect* effect);

	/**
	 * Get effect
	 * @param id id
	 * @return effect or null
	 */
	GUIEffect* getEffect(const string& id);

	/**
	 * Remove effect
	 * @param id id
	 */
	void removeEffect(const string& id);

	/**
	 * Dump node
	 * @param node node to dump
	 * @param depth depth
	 * @param indent indention
	 * @param depthIdx depth index
	 */
	static void dumpNode(GUINode* node, int depth = 0, int indent = 0, int depthIdx = 0);

	/**
	 * Dump parent nodes
	 * @param node node to dump from
	 * @param indent indention
	 */
	static void dumpParentNodes(GUINode* node, int indent = 0);

};
