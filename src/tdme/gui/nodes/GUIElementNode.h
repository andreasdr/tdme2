#pragma once

#include <set>
#include <string>

#include <tdme/tdme.h>
#include <tdme/gui/events/fwd-tdme.h>
#include <tdme/gui/fwd-tdme.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/gui/nodes/GUILayerNode.h>
#include <tdme/gui/nodes/GUINode_Alignments.h>
#include <tdme/gui/nodes/GUINodeConditions.h>

using std::set;
using std::string;

using tdme::gui::events::GUIKeyboardEvent;
using tdme::gui::events::GUIMouseEvent;
using tdme::gui::nodes::GUIColor;
using tdme::gui::nodes::GUILayerNode;
using tdme::gui::nodes::GUINode_Alignments;
using tdme::gui::nodes::GUINode_Border;
using tdme::gui::nodes::GUINode_Flow;
using tdme::gui::nodes::GUINode_Padding;
using tdme::gui::nodes::GUINode_RequestedConstraints;
using tdme::gui::nodes::GUINode_Scale9Grid;
using tdme::gui::nodes::GUINodeConditions;
using tdme::gui::nodes::GUIParentNode;
using tdme::gui::nodes::GUIParentNode_Overflow;
using tdme::gui::nodes::GUIScreenNode;

/**
 * GUI element node
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::gui::nodes::GUIElementNode final: public GUILayerNode
{
	friend class tdme::gui::GUIParser;
	friend class GUINode;
	friend class GUIScreenNode;

public:
	static string CONDITION_FOCUS;
	static string CONDITION_ALWAYS;
	static string CONDITION_ONMOUSEOVER;
	static string CONDITION_CLICK;

private:
	string name;
	string value;
	bool selected;
	bool disabled;
	GUINodeConditions activeConditions;
	bool focusable;
	bool ignoreEvents;
	string onInitializeExpression;
	string onMouseClickExpression;
	string onMouseDoubleClickExpression;
	string onMouseOverExpression;
	string onMouseOutExpression;
	string onChangeExpression;
	string parentElementId;
	vector<string> options;

protected:
	/**
	 * @return node type
	 */
	const string getNodeType() override;
	bool isContentNode() override;

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
	 * @param name name
	 * @param value value
	 * @param selected selected
	 * @param disabled disabled
	 * @param focusable focusable
	 * @param ignoreEvents ignore events
	 * @param onInitialize on initialize expression
	 * @param onMouseClick on mouse click expression
	 * @param onMouseDoubleClickExpression on mouse double click expression
	 * @param onMouseOver on mouse over expression
	 * @param onMouseOut on mouse out expression
	 * @param onChangeExpression on change expression
	 * @param parentElementId parent element id
	 * @param options options
	 */
	GUIElementNode(
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
		const GUINodeConditions& hideOn,
		const string& name,
		const string& value,
		bool selected,
		bool disabled,
		bool focusable,
		bool ignoreEvents,
		const string& onInitialize,
		const string& onMouseClick,
		const string& onMouseDoubleClickExpression,
		const string& onMouseOver,
		const string& onMouseOut,
		const string& onChangeExpression,
		const string& parentElementId,
		const string& options
	);

public:
	/**
	 * @return focusable
	 */
	bool isFocusable();

	/**
	 * @return name
	 */
	const string& getName();

	/**
	 * @return value
	 */
	const string& getValue();

	/**
	 * @return is selected
	 */
	bool isSelected();

	/**
	 * @return is disabled
	 */
	bool isDisabled();


	/**
	 * @return on initialize expression
	 */
	const string& getOnInitializeExpression();

	/**
	 * @return on mouse click expression
	 */
	const string& getOnMouseClickExpression();

	/**
	 * @return on mouse double click expression
	 */
	const string& getOnMouseDoubleClickExpression();

	/**
	 * @return on mouse over expression
	 */
	const string& getOnMouseOverExpression();

	/**
	 * @return on mouse out expression
	 */
	const string& getOnMouseOutExpression();

	/**
	 * @return on change expression
	 */
	const string& getOnChangeExpression();

	/**
	 * Execute on change expression
	 */
	void executeOnChangeExpression();

	/**
	 * Execute expression
	 * @param screenNode screen node
	 * @param expression expression
	 */
	static void executeExpression(GUIScreenNode* screenNode, const string& expression);

	/**
	 * @return parent element id
	 */
	const string& getParentElementNodeId();

	/**
	 * @returns if option is among given options
	 */
	bool hasOption(const string& option);

	/**
	 * @return active conditions
	 */
	GUINodeConditions& getActiveConditions();

	// overridden methods
	void handleKeyboardEvent(GUIKeyboardEvent* event) override;

};
