#pragma once

#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/fileio/textures/Texture.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/prototype/fwd-tdme.h>
#include <tdme/gui/events/GUIInputEventHandler.h>
#include <tdme/math/Vector3.h>
#include <tdme/tools/shared/controller/fwd-tdme.h>
#include <tdme/tools/shared/views/fwd-tdme.h>
#include <tdme/tools/shared/views/View.h>

using std::map;
using std::unordered_map;
using std::unordered_set;
using std::vector;

using tdme::engine::model::Model;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::fileio::textures::Texture;
using tdme::engine::prototype::Prototype;
using tdme::engine::Engine;
using tdme::gui::events::GUIInputEventHandler;
using tdme::math::Vector3;
using tdme::tools::shared::controller::TerrainEditorScreenController;
using tdme::tools::shared::views::CameraInputHandler;
using tdme::tools::shared::views::PopUps;
using tdme::tools::shared::views::View;

/**
 * Shared terrain editor View
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::shared::views::SharedTerrainEditorView
	: public View
	, public GUIInputEventHandler
{
protected:
	Engine* engine { nullptr };

private:
	PopUps* popUps { nullptr };
	TerrainEditorScreenController* terrainEditorScreenController { nullptr };
	Prototype* prototype { nullptr };
	bool initModelRequested { false };
	bool initCameraRequested { true };
	CameraInputHandler* cameraInputHandler { nullptr };
	bool brushingEnabled { false };
	bool brushMoved { false };
	Vector3 brushCenterPosition;
	Texture* brushTexture { nullptr };

	float skyDomeTranslation { 0.0f };

	Prototype* skySpherePrototype { nullptr };
	Prototype* skyDomePrototype { nullptr };
	Prototype* skyPanoramaPrototype { nullptr };
	Prototype* spherePrototype { nullptr };

	BoundingBox terrainBoundingBox;
	vector<Model*> terrainModels;

	struct Water {
		Vector3 waterReflectionEnvironmentMappingPosition;
		vector<Model*> waterModels;
	};
	map<int, Water> waters;

	vector<int> partitionFoliageIdx;
	unordered_set<int> temporaryPartitionIdxs;

	int rampMode { -1 };
	array<Vector3,2> rampVertices;
	array<float,2> rampHeight;
	float brushScale { 1.0f };
	float brushDensityStrength { 1.0f };

	/**
	 * Init entity
	 */
	void initModel();

	/**
	 * Init sky
	 */
	void initSky();

	/**
	 * Update sky
	 */
	void updateSky();

	/**
	 * Load terrain prototype
	 * @param pathName path name
	 * @param fileName file name
	 */
	virtual Prototype* loadTerrainPrototype(const string& pathName, const string& fileName);

public:
	/**
	 * Public constructor
	 * @param popUps pop ups view
	 */
	SharedTerrainEditorView(PopUps* popUps);

	/**
	 * Destructor
	 */
	~SharedTerrainEditorView();

	/**
	 * @return pop ups
	 */
	PopUps* getPopUps();

	/**
	 * @return prototype
	 */
	Prototype* getPrototype();

	/**
	 * Set prototype
	 * @param prototype prototype
	 */
	void setPrototype(Prototype* entity);

	/**
	 * Set terrain brush
	 * @param texture brush texture
	 * @param scale scale
	 * @param densityStrength density strength
	 */
	void setTerrainBrush(Texture* texture, float scale, float densityStrength);

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
	 * Unset terrain brush
	 */
	void unsetTerrainBrush();

	/**
	 * Set terrain models
	 * @param terrainBoundingBox terrain bounding box
	 * @param terrainModels terrain models
	 */
	void setTerrain(BoundingBox& terrainBoundingBox, vector<Model*> terrainModels);

	/**
	 * Unset all water
	 */
	void unsetWater();

	/**
	 * Remove water
	 * @param waterIdx water index
	 */
	void removeWater(int waterIdx);

	/**
	 * Add water
	 * @param waterIdx water index
	 * @param waterModels water models
	 * @param waterReflectionEnvironmentMappingPosition water reflection environment mapping position
	 */
	void addWater(int waterIdx, vector<Model*> waterModels, const Vector3& waterReflectionEnvironmentMappingPosition);

	/**
	 * Add temporary foliage
	 * @param newFoliageMaps new foliage maps
	 */
	void addTemporaryFoliage(const vector<unordered_map<int, vector<Transformations>>>& newFoliageMaps);

	/**
	 * Update temporary foliage
	 */
	void updateTemporaryFoliage(const unordered_set<int>& partitionIdxSet);

	/**
	 * Recreate temporary foliage at given partition indices
	 * @param partitionIdxSet partition indices set
	 */
	inline void recreateTemporaryFoliage(const unordered_set<int>& partitionIdxSet) {
		for (auto partitionIdx: partitionIdxSet) recreateTemporaryFoliage(partitionIdx);
	}

	/**
	 * Recreate temporary foliage at given partition index
	 * @param partitionIdx partition index
	 */
	void recreateTemporaryFoliage(int partitionIdx);

	/**
	 * Add foliage using render groups at given partition indices
	 */
	void addFoliage();

	/**
	 * Recreate foliage using render groups at given partition indices that has been transformed to temporary foliage
	 * @param partitionIdxSet partition index set
	 */
	void recreateFoliage(const unordered_set<int>& partitionIdxSet);

	/**
	 * Reset camera
	 */
	void resetCamera();

	/**
	 * Init GUI elements
	 */
	void updateGUIElements();

	// overridden methods
	void handleInputEvents() override;
	void display() override;
	void initialize() override;
	void activate() override;
	void deactivate() override;
	void dispose() override;

	/**
	 * On init additional screens
	 */
	virtual void onInitAdditionalScreens();

	/**
	 * On load terrain
	 * @param oldEntity old entity
	 * @param entity entity
	 */
	virtual void onLoadTerrain(Prototype* oldEntity, Prototype* entity);

	/**
	 * On set prototype data hook
	 */
	virtual void onSetPrototypeData();

	/**
	 * Load terrain
	 * @param pathName path name
	 * @param fileName file name
	 */
	virtual void loadTerrain(const string& pathName, const string& fileName);

	/**
	 * Save terrain
	 * @param pathName path name
	 * @param fileName file name
	 */
	void saveTerrain(const string& pathName, const string& fileName);

};
