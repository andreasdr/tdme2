#pragma once

#include <string>
#include <vector>

#include <tdme/engine/fileio/textures/fwd-tdme.h>
#include <tdme/gui/events/fwd-tdme.h>
#include <tdme/gui/events/GUIActionListener.h>
#include <tdme/gui/events/GUIChangeListener.h>
#include <tdme/gui/events/GUIChangeListener.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/math/Vector3.h>
#include <tdme/tools/shared/controller/fwd-tdme.h>
#include <tdme/tools/shared/controller/ScreenController.h>
#include <tdme/tools/shared/views/fwd-tdme.h>
#include <tdme/utilities/Terrain.h>

using std::string;
using std::vector;

using tdme::engine::fileio::textures::Texture;
using tdme::gui::events::GUIActionListener;
using tdme::gui::events::GUIActionListenerType;
using tdme::gui::events::GUIChangeListener;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::nodes::GUITextNode;
using tdme::math::Vector3;
using tdme::tools::shared::controller::FileDialogPath;
using tdme::tools::shared::controller::PrototypeBaseSubScreenController;
using tdme::tools::shared::controller::ScreenController;
using tdme::tools::shared::views::SharedTerrainEditorView;
using tdme::utilities::Terrain;

/**
 * Terrain editor screen controller
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::shared::controller::TerrainEditorScreenController final
	: public ScreenController
	, public GUIActionListener
	, public GUIChangeListener
{
private:
	PrototypeBaseSubScreenController* prototypeBaseSubScreenController { nullptr };
	SharedTerrainEditorView* view { nullptr };
	GUIScreenNode* screenNode { nullptr };
	GUITextNode* screenCaption { nullptr };
	GUIElementNode* viewPort { nullptr };
	GUIElementNode* terrainDimensionWidth { nullptr };
	GUIElementNode* terrainDimensionDepth { nullptr };
	GUIElementNode* btnTerrainDimensionApply { nullptr };
	GUIElementNode* btnTerrainDimensionLoad { nullptr };
	GUIElementNode* btnTerrainDimensionSave { nullptr };
	GUIElementNode* brushScale { nullptr };
	GUIElementNode* brushStrength { nullptr };
	GUIElementNode* brushFile { nullptr };
	GUIElementNode* brushFileLoad { nullptr };
	GUIElementNode* brushFileClear { nullptr };
	GUIElementNode* btnBrushApply { nullptr };

	FileDialogPath* terrainPath { nullptr };

	bool haveCurrentBrushFlattenHeight { false };
	float currentBrushFlattenHeight { 0.0f };
	float currentBrushScale { 1.0f };
	float currentBrushStrength { 1.0f };
	Texture* currentBrushTexture { nullptr };
	Terrain::BrushOperation currentBrushOperation { Terrain::BRUSHOPERATION_ADD };

public:
	/**
	 * Public constructor
	 * @param view view
	 */
	TerrainEditorScreenController(SharedTerrainEditorView* view);

	// overridden methods
	GUIScreenNode* getScreenNode() override;
	void initialize() override;
	void dispose() override;

	/**
	 * Set screen caption
	 * @param text text
	 */
	void setScreenCaption(const string& text);

	/**
	 * Set up general prototype data
	 * @param name name
	 * @param description description
	 */
	void setPrototypeData(const string& name, const string& description);

	/**
	 * Unset prototype data
	 */
	void unsetPrototypeData();

	/**
	 * Set up prototype properties
	 * @param presetId preset id
	 * @param selectedName selected name
	 */
	void setPrototypeProperties(const string& presetId, const string& selectedName);

	/**
	 * Unset prototype properties
	 */
	void unsetPrototypeProperties();

	// overridden methods
	void onValueChanged(GUIElementNode* node) override;
	void onActionPerformed(GUIActionListenerType type, GUIElementNode* node) override;

	/**
	 * Set terrain dimension
	 * @param width width
	 * @param height height
	 */
	void setTerrainDimension(float width, float height);

	/**
	 * On terrain load
	 */
	void onTerrainLoad();

	/**
	 * On terrain save
	 */
	void onTerrainSave();

	/**
	 * On load terrain
	 */
	void onLoadTerrain();

	/**
	 * On apply terrain dimension
	 */
	void onApplyTerrainDimension();

	/**
	 * On apply brush
	 */
	void onApplyBrush();

	/**
	 * Apply current brush at given brush center position
	 * @param terrainBoundingBox terrain bounding box
	 * @param terrainModels terrain models
	 * @param brushCenterPosition brush center position
	 * @param deltaTime delta time between last frame and this frame
	 */
	void applyBrush(BoundingBox& terrainBoundingBox, vector<Model*> terrainModels, const Vector3& brushCenterPosition, int64_t deltaTime);

	/**
	 * Determine current brush flatten height
	 * @param terrainBoundingBox terrain bounding box
	 * @param terrainModels terrain models
	 * @param brushCenterPosition brush center position
	 * @return success
	 */
	bool determineCurrentBrushFlattenHeight(BoundingBox& terrainBoundingBox, vector<Model*> terrainModels, const Vector3& brushCenterPosition);

	/**
	 * Unset current brush flatten height
	 */
	void unsetCurrentBrushFlattenHeight();

	/**
	 * Get viewport rectangle
	 * @param left left
	 * @param top top
	 * @param width width
	 * @param height height
	 */
	void getViewPort(int& left, int& top, int& width, int& height);

	/**
	 * Shows the error pop up
	 * @param caption caption
	 * @param message message
	 */
	void showErrorPopUp(const string& caption, const string& message);

};
