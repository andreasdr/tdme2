#pragma once

#include <set>
#include <string>

#include <tdme/tdme.h>
#include <tdme/gui/fwd-tdme.h>
#include <tdme/gui/events/fwd-tdme.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/gui/nodes/GUINode_AlignmentHorizontal.h>
#include <tdme/gui/nodes/GUINode_AlignmentVertical.h>
#include <tdme/gui/nodes/GUINode_Alignments.h>
#include <tdme/gui/nodes/GUINodeConditions.h>
#include <tdme/gui/nodes/GUIParentNode.h>

using std::set;
using std::string;

using tdme::gui::nodes::GUIParentNode;
using tdme::gui::nodes::GUIColor;
using tdme::gui::nodes::GUINode_Alignments;
using tdme::gui::nodes::GUINode_Border;
using tdme::gui::nodes::GUINode_Flow;
using tdme::gui::nodes::GUINode_Padding;
using tdme::gui::nodes::GUINode_RequestedConstraints;
using tdme::gui::nodes::GUINode_Scale9Grid;
using tdme::gui::nodes::GUINodeConditions;
using tdme::gui::nodes::GUIParentNode_Overflow;
using tdme::gui::nodes::GUIScreenNode;

/**
 * GUI layer node
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::gui::nodes::GUILayerNode: public GUIParentNode
{
	friend class tdme::gui::GUIParser;
	friend class GUINode;
	friend class GUIScreenNode;

protected:
	/**
	 * @return node type
	 */
	const string getNodeType() override;
	bool isContentNode() override;

	/**
	 * Set computed top
	 * @param top top
	 */
	void setTop(int32_t top) override;

	/**
	 * Set computed left
	 * @param left left
	 */
	void setLeft(int32_t left) override;
	void layoutSubNodes() override;
	void layout() override;

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
	 */
	GUILayerNode(
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

public:
	int32_t getContentWidth() override;
	int32_t getContentHeight() override;

};
