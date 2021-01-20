#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/fileio/textures/fwd-tdme.h>
#include <tdme/gui/fwd-tdme.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/gui/nodes/GUIColor.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUINode_Clipping.h>
#include <tdme/gui/nodes/GUINode_Scale9Grid.h>
#include <tdme/gui/renderer/fwd-tdme.h>
#include <tdme/math/Matrix2D3x3.h>

using std::string;

using tdme::engine::fileio::textures::Texture;
using tdme::gui::nodes::GUIColor;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINode_Alignments;
using tdme::gui::nodes::GUINode_Border;
using tdme::gui::nodes::GUINode_Clipping;
using tdme::gui::nodes::GUINode_Flow;
using tdme::gui::nodes::GUINode_Padding;
using tdme::gui::nodes::GUINode_RequestedConstraints;
using tdme::gui::nodes::GUINode_Scale9Grid;
using tdme::gui::nodes::GUINodeConditions;
using tdme::gui::nodes::GUIParentNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::renderer::GUIRenderer;
using tdme::math::Matrix2D3x3;

/**
 * GUI image node
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::gui::nodes::GUIImageNode final
	: public GUINode
{
	friend class tdme::gui::GUIParser;

private:
	Texture* texture { nullptr };
	int32_t textureId { 0 };
	GUIColor effectColorMul;
	GUIColor effectColorAdd;
	GUINode_Clipping clipping;
	Texture* maskTexture { nullptr };
	float maskMaxValue;
	int32_t maskTextureId { 0 };

	string source;
	Matrix2D3x3 textureMatrix;
	GUINode_Scale9Grid scale9Grid;
	string mask;

protected:
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
	 * @param source source
	 * @param effectColorMul effect color mul
	 * @param effectColorAdd effect color add
	 * @param scale9Grid scale 9 grid
	 * @param clipping clipping
	 * @param mask mask image
	 * @param maskMaxValue maximum value of mask to display image
	 * @throws tdme::gui::GUIParserException
	 */
	GUIImageNode(
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
		const string& source,
		const GUIColor& effectColorMul,
		const GUIColor& effectColorAdd,
		const GUINode_Scale9Grid& scale9Grid,
		const GUINode_Clipping& clipping,
		const string& mask,
		float maskMaxValue
	);

	/**
	 * @return node type
	 */
	const string getNodeType() override;
	bool isContentNode() override;

public:
	// overridden methods
	int32_t getContentWidth() override;
	int32_t getContentHeight() override;
	void dispose() override;
	void render(GUIRenderer* guiRenderer) override;

	/**
	 * @return image source
	 */
	const string& getSource();

	/**
	 * Set image source
	 * @param source source
	 */
	void setSource(const string& source);

	/**
	 * Set texture matrix
	 * @param textureMatrix texture matrix
	 */
	void setTextureMatrix(const Matrix2D3x3& textureMatrix);

	/**
	 * @return effect color mul
	 */
	const GUIColor& getEffectColorMul();

	/**
	 * Set effect color mul
	 * @param effectColorMul effect color mul
	 */
	void setEffectColorMul(const GUIColor& effectColorMul);

	/**
	 * @return effect color add
	 */
	const GUIColor& getEffectColorAdd();

	/**
	 * Set effect color add
	 * @param effectColorAdd effect color add
	 */
	void setEffectColorAdd(const GUIColor& effectColorAdd);

	/**
	 * @return clipping
	 */
	GUINode_Clipping& getClipping();

	/**
	 * Create clipping
	 * @param allClipping all sides
	 * @param left left
	 * @param top top
	 * @param right right
	 * @param bottom bottom
	 */
	static GUINode_Clipping createClipping(const string& allClipping, const string& left, const string& top, const string& right, const string& bottom);

	/**
	 * @return mask source
	 */
	const string& getMask();

	/**
	 * Set mask source
	 * @param mask mask source
	 */
	void setMask(const string& mask);

	/**
	 * @return maximum value of mask to display image
	 */
	float getMaskMaxValue() {
		return maskMaxValue;
	}

	/**
	 * Set maximum value of mask to display image
	 * @param maskMinValue value of mask to display image
	 */
	void setMaskMaxValue(float maskMaxValue) {
		this->maskMaxValue = maskMaxValue;
	}

};
