#pragma once

#include <set>
#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/gui/events/fwd-tdme.h>
#include <tdme/gui/fwd-tdme.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/renderer/fwd-tdme.h>
#include <tdme/gui/GUIParserException.h>
#include <tdme/utilities/fwd-tdme.h>

using std::set;
using std::string;
using std::vector;

using tdme::gui::events::GUIKeyboardEvent;
using tdme::gui::events::GUIMouseEvent;
using tdme::gui::nodes::GUIColor;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINode_Alignments;
using tdme::gui::nodes::GUINode_Border;
using tdme::gui::nodes::GUINode_Flow;
using tdme::gui::nodes::GUINode_Padding;
using tdme::gui::nodes::GUINode_RequestedConstraints;
using tdme::gui::nodes::GUINode_Scale9Grid;
using tdme::gui::nodes::GUINodeConditions;
using tdme::gui::nodes::GUIParentNode_Overflow;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::renderer::GUIRenderer;
using tdme::gui::GUIParserException;

/**
 * GUI parent node base class thats supporting child nodes
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::gui::nodes::GUIParentNode
	: public GUINode
{
	friend class tdme::gui::GUIParser;
	friend class GUIElementNode;
	friend class GUILayerNode;
	friend class GUILayoutNode;
	friend class GUINode;
	friend class GUINodeConditions;
	friend class GUIScreenNode;
	friend class GUIHorizontalScrollbarInternalController;
	friend class GUIVerticalScrollbarInternalController;
	friend class GUIParentNode_Overflow;
private:
	float childrenRenderOffsetX;
	float childrenRenderOffsetY;

protected:
	vector<GUINode*> subNodes;
	bool computeViewportCache;
	vector<GUINode*> vieportSubNodesCache;
	vector<GUINode*> floatingNodesCache;
	GUIParentNode_Overflow* overflowX;
	GUIParentNode_Overflow* overflowY;

	/**
	 * Constructor
	 * @param screenNode screen node
	 * @param parentNode parent node
	 * @param id id
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
	 * @param showOn show on
	 * @param hideOn hide on
	 * @throws tdme::gui::GUIParserException
	 */
	GUIParentNode(
		GUIScreenNode* screenNode,
		GUIParentNode* parentNode,
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
		const GUINodeConditions& showOn,
		const GUINodeConditions& hideOn
	);

	/**
	 * Layout
	 */
	void layout() override;

	/**
	 * Layout sub nodes
	 */
	virtual void layoutSubNodes();

	/**
	 * Compute horizontal children alignment
	 */
	virtual void computeHorizontalChildrenAlignment();

	/**
	 * Compute vertical children alignment
	 */
	virtual void computeVerticalChildrenAlignment();

public:

	/**
	 * Clear sub nodes
	 */
	virtual void clearSubNodes();

	/**
	 * Replace sub nodes with given XML
	 * @param xml xml
	 * @param resetScrollOffsets reset scroll offsets
	 * @throws tdme::gui::GUIParserException
	 */
	virtual void replaceSubNodes(const string& xml, bool resetScrollOffsets);

	/**
	 * Add sub node
	 * @param node node
	 * @throws tdme::gui::GUIParserException
	 */
	virtual void addSubNode(GUINode* node);

	/**
	 * @return overflow x
	 */
	virtual GUIParentNode_Overflow* getOverflowX();

	/**
	 * @return overflow y
	 */
	virtual GUIParentNode_Overflow* getOverflowY();

	/**
	 * Create over flow
	 * @param overflow over flow
	 * @return over flow
	 * @throws tdme::gui::GUIParserException
	 */
	static GUIParentNode_Overflow* createOverflow(const string& overflow);

	/**
	 * @return children render offset x
	 */
	virtual float getChildrenRenderOffsetX();

	/**
	 * Set children render offset x
	 * @param childrenRenderOffSetX children render offset x
	 */
	virtual void setChildrenRenderOffsetX(float childrenRenderOffSetX);

	/**
	 * @return children render offset y
	 */
	virtual float getChildrenRenderOffsetY();

	/**
	 * Set children render offset y
	 * @param childrenRenderOffSetY children render offset y
	 */
	virtual void setChildrenRenderOffsetY(float childrenRenderOffSetY);

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
	 * Get child controller nodes
	 * @param childControllerNodes child controller nodes
	 * @param requireConditionsMet require conditions met
	 */
	virtual void getChildControllerNodes(vector<GUINode*>& childControllerNodes, bool requireConditionsMet = false);

	// overridden methods
	void dispose() override;
	void setConditionsMet() override;
	void render(GUIRenderer* guiRenderer) override;
	void determineMouseEventNodes(GUIMouseEvent* event, bool floatingNode, set<string>& eventNodeIds, set<string>& eventFloatingNodeIds) override;
	void handleKeyboardEvent(GUIKeyboardEvent* event) override;

	/**
	 * Invalidate render caches
	 */
	void invalidateRenderCaches();

private:
	/**
	 * Get child controller nodes internal
	 * @param childControllerNodes child controller nodes
	 * @param requireConditionsMet require conditions met
	 */
	void getChildControllerNodesInternal(vector<GUINode*>& childControllerNodes, bool requireConditionsMet = false);
};
