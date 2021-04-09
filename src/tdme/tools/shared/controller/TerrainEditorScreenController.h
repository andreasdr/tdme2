#pragma once

#include <array>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <tdme/engine/fileio/textures/fwd-tdme.h>
#include <tdme/engine/prototype/fwd-tdme.h>
#include <tdme/engine/Transformations.h>
#include <tdme/gui/events/fwd-tdme.h>
#include <tdme/gui/events/GUIActionListener.h>
#include <tdme/gui/events/GUIChangeListener.h>
#include <tdme/gui/events/GUIChangeListener.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/math/Vector2.h>
#include <tdme/math/Vector3.h>
#include <tdme/tools/shared/controller/fwd-tdme.h>
#include <tdme/tools/shared/controller/FileDialogPath.h>
#include <tdme/tools/shared/controller/ScreenController.h>
#include <tdme/tools/shared/views/fwd-tdme.h>
#include <tdme/utilities/Terrain.h>

using std::array;
using std::string;
using std::unordered_map;
using std::unordered_set;
using std::vector;

using tdme::engine::fileio::textures::Texture;
using tdme::engine::prototype::Prototype;
using tdme::engine::Transformations;
using tdme::gui::events::GUIActionListener;
using tdme::gui::events::GUIActionListenerType;
using tdme::gui::events::GUIChangeListener;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::nodes::GUITextNode;
using tdme::math::Vector2;
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
	GUIElementNode* terrainBrushScale { nullptr };
	GUIElementNode* terrainBrushStrength { nullptr };
	GUIElementNode* terrainBrushFile { nullptr };
	GUIElementNode* terrainBrushFileLoad { nullptr };
	GUIElementNode* terrainBrushFileClear { nullptr };
	GUIElementNode* btnTerrainBrushApply { nullptr };

	GUIElementNode* foliageBrushScale { nullptr };
	GUIElementNode* foliageBrushDensity { nullptr };
	GUIElementNode* foliageBrushFile { nullptr };
	GUIElementNode* foliageBrushFileLoad { nullptr };
	GUIElementNode* foliageBrushFileClear { nullptr };

	array<GUIElementNode*, 5> foliageBrushPrototypeFileCount;
	array<GUIElementNode*, 5> foliageBrushPrototypeFile;
	array<GUIElementNode*, 5> foliageBrushPrototypeFileLoad;
	array<GUIElementNode*, 5> foliageBrushPrototypeFileClear;
	array<GUIElementNode*, 5> foliageBrushPrototypeScaleMin;
	array<GUIElementNode*, 5> foliageBrushPrototypeScaleMax;
	array<GUIElementNode*, 5> foliageBrushPrototypeRangeXMin;
	array<GUIElementNode*, 5> foliageBrushPrototypeRangeXMax;
	array<GUIElementNode*, 5> foliageBrushPrototypeRangeYMin;
	array<GUIElementNode*, 5> foliageBrushPrototypeRangeYMax;
	array<GUIElementNode*, 5> foliageBrushPrototypeRangeZMin;
	array<GUIElementNode*, 5> foliageBrushPrototypeRangeZMax;
	array<GUIElementNode*, 5> foliageBrushPrototypeSlopeMin;
	array<GUIElementNode*, 5> foliageBrushPrototypeSlopeMax;
	array<GUIElementNode*, 5> foliageBrushPrototypeHeightMin;
	array<GUIElementNode*, 5> foliageBrushPrototypeHeightMax;
	array<GUIElementNode*, 5> foliageBrushPrototypeNormalAlign;

	GUIElementNode* btnFoliageBrushApply { nullptr };

	GUIElementNode* mirrorXAxis { nullptr };
	GUIElementNode* mirrorZAxis { nullptr };
	GUIElementNode* mirrorFlip { nullptr };
	GUIElementNode* btnMirrorApply { nullptr };

	FileDialogPath terrainPath;
	FileDialogPath brushTexturePath;
	FileDialogPath prototypePath;

	bool haveCurrentTerrainBrushHeight { false };
	float currentTerrainBrushHeight { 0.0f };
	float currentTerrainBrushScale { 1.0f };
	float currentTerrainBrushStrength { 1.0f };
	Texture* currentTerrainBrushTexture { nullptr };
	Terrain::BrushOperation currentTerrainBrushOperation { Terrain::BRUSHOPERATION_NONE };

	Texture* currentFoliageBrushTexture { nullptr };
	float currentFoliageBrushScale { 1.0f };
	float currentFoliageBrushDensity { 1.0f };
	array<int, 5> currentFoliageBrushIds { -1, -1, -1, -1, -1 };
	array<float, 5> currentFoliageBrushCount { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
	array<array<float, 2>, 5> currentFoliageBrushPrototypeScale {{
		{ 1.0f, 1.0f },
		{ 1.0f, 1.0f },
		{ 1.0f, 1.0f },
		{ 1.0f, 1.0f },
		{ 1.0f, 1.0f }
	}};
	array<array<float, 6>, 5> currentFoliageBrushPrototypeRotations {{
		{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f }
	}};
	array<array<float, 2>, 5> currentFoliageBrushPrototypeSlope {{
		{ 0.0f, 90.0f },
		{ 0.0f, 90.0f },
		{ 0.0f, 90.0f },
		{ 0.0f, 90.0f },
		{ 0.0f, 90.0f }
	}};
	array<array<float, 2>, 5> currentFoliageBrushPrototypeHeight {{
		{ 0.0f, 100.0f },
		{ 0.0f, 100.0f },
		{ 0.0f, 100.0f },
		{ 0.0f, 100.0f },
		{ 0.0f, 100.0f }
	}};
	array<bool, 5> currentFoliageBrushPrototypeNormalAlign { false, false, false, false, false };
	Terrain::BrushOperation currentFoliageBrushOperation { Terrain::BRUSHOPERATION_NONE };
	vector<unordered_map<int, vector<Transformations>>> newFoliageMaps;
	unordered_set<int> recreateFoliagePartitions;

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
	 * On terrain brush load
	 */
	void onTerrainBrushFileLoad();

	/**
	 * On terrain brush clear
	 */
	void onTerrainBrushFileClear();

	/**
	 * On apply terrain brush
	 */
	void onApplyTerrainBrush();

	/**
	 * Set brush scale
	 * @param scale scale
	 */
	void setBrushScale(float scale);

	/**
	 * Set brush density/strength
	 * @param densityStrength density/strength
	 */
	void setBrushDensityStrength(float densityStrength);

	/**
	 * @return current terrain brush operation
	 */
	Terrain::BrushOperation getTerrainBrushOperation();

	/**
	 * @return current foliage brush operation
	 */
	Terrain::BrushOperation getFoliageBrushOperation();

	/**
	 * Apply current brush to terrain at given brush center position
	 * @param terrainBoundingBox terrain bounding box
	 * @param terrainModels terrain models
	 * @param brushCenterPosition brush center position
	 * @param deltaTime delta time between last frame and this frame
	 */
	void applyTerrainBrush(BoundingBox& terrainBoundingBox, vector<Model*>& terrainModels, const Vector3& brushCenterPosition, int64_t deltaTime);

	/**
	 * Apply current brush to terrain at given brush center position
	 * @param terrainBoundingBox terrain bounding box
	 * @param terrainModels terrain models
	 * @param position position
	 * @param rotation rotation
	 * @param scale scale
	 * @param minHeight min height
	 * @param maxHeigth max heigth
	 */
	void applyRampTerrainBrush(BoundingBox& terrainBoundingBox, vector<Model*>& terrainModels, const Vector3& position, float rotation, const Vector2& scale, float minHeight, float maxHeight);

	/**
	 * Apply current brush to foliage at given brush center position
	 * @param terrainBoundingBox terrain bounding box
	 * @param brushCenterPosition brush center position
	 * @param deltaTime delta time between last frame and this frame
	 */
	void applyFoliageBrush(BoundingBox& terrainBoundingBox, const Vector3& brushCenterPosition, int64_t deltaTime);

	/**
	 * Create water using a auto fill like algorithm
	 * @param terrainBoundingBox terrain bounding box
	 * @param brushCenterPosition brush center position
	 * @param waterModels water models
	 * @param waterReflectionEnvironmentMappingPosition water reflection environment mapping position
	 */
	void createWater(BoundingBox& terrainBoundingBox, const Vector3& brushCenterPosition, vector<Model*>& waterModels, Vector3& waterReflectionEnvironmentMappingPosition);

	/**
	 * Delete water models
	 * @param waterPositionMapIdx water position map index
	 */
	void deleteWater(int waterPositionMapIdx);

	/**
	 * Determine current brush flatten height
	 * @param terrainBoundingBox terrain bounding box
	 * @param terrainModels terrain models
	 * @param brushCenterPosition brush center position
	 * @return success
	 */
	bool determineCurrentBrushHeight(BoundingBox& terrainBoundingBox, vector<Model*> terrainModels, const Vector3& brushCenterPosition);

	/**
	 * Determine ramp height
	 * @param terrainBoundingBox terrain bounding box
	 * @param terrainModels terrain models
	 * @param position position
	 * @param height height
	 * @return success
	 */
	bool determineRampHeight(BoundingBox& terrainBoundingBox, vector<Model*> terrainModels, const Vector3& position, float& height);

	/**
	 * Unset current brush flatten height
	 */
	void unsetCurrentBrushFlattenHeight();

	/**
	 * On foliage brush load
	 */
	void onFoliageBrushFileLoad();

	/**
	 * On foliage brush clear
	 */
	void onFoliageBrushFileClear();

	/**
	 * On foliage brush prototype load
	 * @param idx index
	 */
	void onFoliageBrushPrototypeLoad(int idx);

	/**
	 * On foliage brush prototype clear
	 * @param idx index
	 */
	void onFoliageBrushPrototypeClear(int idx);

	/**
	 * On apply foliage brush
	 */
	void onApplyFoliageBrush();

	/**
	 * On apply mirror
	 */
	void onApplyMirror();

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
