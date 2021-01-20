#pragma once

#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/gui/nodes/GUIColor.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/renderer/fwd-tdme.h>
#include <tdme/utilities/fwd-tdme.h>

using std::string;
using std::vector;

using tdme::gui::nodes::GUIColor;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINode_Alignments;
using tdme::gui::nodes::GUINode_Border;
using tdme::gui::nodes::GUINode_Flow;
using tdme::gui::nodes::GUINode_Padding;
using tdme::gui::nodes::GUINode_RequestedConstraints;
using tdme::gui::nodes::GUINodeConditions;
using tdme::gui::nodes::GUIParentNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::renderer::GUIRenderer;

/**
 * GUI horizontal scroll bar internal node
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::gui::nodes::GUIHorizontalScrollbarInternalNode
	: public GUINode
{

private:
	GUIColor barColorNone;
	GUIColor barColorMouseOver;
	GUIColor barColorDragging;

protected:
	const string getNodeType() override;
	bool isContentNode() override;

public:
	/**
	 * Constructor
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
	 * @param barColorNone bar color none
	 * @param barColorMouseOver bar color mouse over
	 * @param barColorDragging bar color dragging
	 *
	 */
	GUIHorizontalScrollbarInternalNode(
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
		const GUINodeConditions& hideOn,
		const GUIColor& barColorNone,
		const GUIColor& barColorMouseOver,
		const GUIColor& barColorDragging
	);

	// overridden methods
	int32_t getContentWidth() override;
	int32_t getContentHeight() override;
	void render(GUIRenderer* guiRenderer) override;

};
