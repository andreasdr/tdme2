#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Vector4.h>
#include <tdme/tools/leveleditor/controller/fwd-tdme.h>
#include <tdme/tools/leveleditor/views/fwd-tdme.h>
#include <tdme/tools/shared/model/fwd-tdme.h>
#include <tdme/tools/shared/views/fwd-tdme.h>
#include <tdme/utils/fwd-tdme.h>
#include <tdme/tools/shared/views/Gizmo.h>
#include <tdme/tools/shared/views/View.h>
#include <tdme/gui/events/GUIInputEventHandler.h>

using std::map;
using std::set;
using std::string;
using std::vector;

using tdme::tools::shared::views::View;
using tdme::gui::events::GUIInputEventHandler;

using tdme::engine::Engine;
using tdme::engine::Entity;
using tdme::engine::EntityPickingFilter;
using tdme::engine::Object3D;
using tdme::engine::Rotation;
using tdme::engine::model::Color4;
using tdme::engine::model::Model;
using tdme::math::Vector3;
using tdme::math::Vector4;
using tdme::tools::leveleditor::controller::LevelEditorScreenController;
using tdme::tools::leveleditor::views::LevelEditorView_ObjectColor;
using tdme::tools::shared::model::LevelEditorEntity;
using tdme::tools::shared::model::LevelEditorLevel;
using tdme::tools::shared::model::LevelEditorObject;
using tdme::tools::shared::views::Gizmo;
using tdme::tools::shared::views::PopUps;
using tdme::tools::shared::views::View;

/** 
 * TDME Level Editor View
 * @author andreas.drewke
 * @version $Id: 04313d20d0978eefc881024d6e0af748196c1425 $
 */
class tdme::tools::leveleditor::views::LevelEditorView final
	: public View
	, public GUIInputEventHandler
	, protected Gizmo
{
private:
	static vector<string> OBJECTCOLOR_NAMES;
	static constexpr int32_t MOUSE_DOWN_LAST_POSITION_NONE { -1 };
	static constexpr int32_t MOUSE_PANNING_NONE { 0 };
	static constexpr int32_t MOUSE_ROTATION_NONE { 0 };
	float snappingX;
	float snappingZ;
	bool snappingEnabled;
	LevelEditorScreenController* levelEditorScreenController { nullptr };
	Engine* engine { nullptr };
	LevelEditorEntity* selectedEntity { nullptr };
	bool reloadEntityLibrary;
	map<string, LevelEditorView_ObjectColor*> objectColors;
	Rotation* camLookRotationX { nullptr };
	Rotation* camLookRotationY { nullptr };
	float camScale;
	float camScaleMax;
	float camScaleMin;
	int32_t placeEntityMouseX;
	int32_t placeEntityMouseY;
	int32_t mouseDownLastX;
	int32_t mouseDownLastY;
	bool mouseDragging;
	Entity* mouseDraggingLastObject;
	int32_t mousePanningSide;
	int32_t mousePanningForward;
	int32_t mouseRotationX;
	int32_t mouseRotationY;
	Vector3 camLookAt;
	Vector3 gridCenter;
	bool gridEnabled;
	float gridY;
	bool keyLeft;
	bool keyRight;
	bool keyUp;
	bool keyDown;
	bool keyA;
	bool keyD;
	bool keyW;
	bool keyS;
	bool keyPlus;
	bool keyMinus;
	bool keyR;
	bool keyControl;
	bool keyShift;
	bool keyEscape;

	bool placeEntityMode;
	bool placeEntityValid;
	int placeEntityYRotation;
	bool pasteMode;
	bool pasteModeValid;
	Vector3 placeEntityTranslation;

private:
	Model* levelEditorGround { nullptr };
	LevelEditorLevel* level { nullptr };
	vector<string> selectedEntityIds;
	set<string> selectedEntityIdsById;
	vector<LevelEditorObject*> pasteObjects_;
	PopUps* popUps;
	EntityPickingFilter* entityPickingFilterNoGrid { nullptr };
	EntityPickingFilter* entityPickingFilterPlacing { nullptr };

public:

	/** 
	 * @return pop ups
	 */
	PopUps* getPopUps();

	/** 
	 * @return level file name
	 */
	const string getFileName();

	/** 
	 * @return level
	 */
	LevelEditorLevel* getLevel();

	/** 
	 * @return selected entity
	 */
	LevelEditorEntity* getSelectedEntity();

	/** 
	 * @return selected level editor object
	 */
	LevelEditorObject* getSelectedObject();

	/** 
	 * @return grid enabled
	 */
	bool isGridEnabled();

	/** 
	 * @param gridEnabled grid enabled
	 */
	void setGridEnabled(bool gridEnabled);

	/** 
	 * @return grid y
	 */
	float getGridY();

	/**
	 * Set grid y position 
	 * @param gridY grid y
	 */
	void setGridY(float gridY);

	/** 
	 * Get snapping
	 * @param snappingEnabled snapping enabled
	 * @param snappingX snapping along X axis
	 * @param snappingZ snapping along Z axis
	 */
	void getSnapping(bool& snappingEnabled, float& snappingX, float& snappingZ);

	/**
	 * Set snapping
	 * @param snappingEnabled snapping enabled
	 * @param snappingX snapping along X axis
	 * @param snappingZ snapping along Z axis
	 */
	void setSnapping(bool snappingEnabled, float snappingX, float snappingZ);

	/**
	 * Load selected entity from library
	 * @param id id
	 */
	void loadEntityFromLibrary(int32_t id);
	void handleInputEvents() override;

	/** 
	 * Renders the scene 
	 */
	void display() override;

	/** 
	 * Select objects
	 * @param entityIds object ids
	 */
	void selectObjects(const vector<string>& entityIds);

	/** 
	 * Select objects by id
	 */
	void unselectObjects();

private:

	/** 
	 * Update GUI elements
	 * screen caption
	 * level size
	 * selected object
	 * object properties
	 * object 3d transformations
	 * object data  
	 * ..
	 */
	void updateGUIElements();

	/**
	 * Update GUI transformations elements
	 * level size
	 * object 3d transformations
	 * ..
	 */
	void updateGUITransformationsElements();

public:

	/** 
	 * Updates objects list box
	 */
	void setObjectsListBox();

	/** 
	 * Unselect light presets
	 */
	void unselectLightPresets();

private:

	/** 
	 * Load settings
	 */
	void loadSettings();

public:
	void initialize() override;
	void activate() override;
	void deactivate() override;

private:

	/** 
	 * Store settings
	 */
	void storeSettings();

public:
	void dispose() override;

private:

	/** 
	 * Set highlight object color effect
	 * @param object object
	 */
	void setHighlightObjectColorEffect(Entity* object);

	/** 
	 * Set standard object color effect
	 * @param object object
	 */
	void setStandardObjectColorEffect(Entity* object);

public:

	/** 
	 * Loads a level from internal level representation to tdme
	 */
	void loadLevel();

private:

	/** 
	 * Update dynamic grid
	 */
	void updateGrid();

	/** 
	 * Remove grid
	 */
	void removeGrid();

	/** 
	 * Creates a level editor ground plate
	 * @return ground
	 */
	Model* createLevelEditorGroundPlateModel();

public:

	/** 
	 * On object data apply
	 * @param name name
	 * @param description description
	 */
	bool objectDataApply(const string& name, const string& description);

	/** 
	 * Initialize place entity mode
	 */
	void setPlaceObjectMode();

	/**
	 * Finish place entity mode
	 */
	void unsetPlaceObjectMode();

	/**
	 * Places selected model on selected object
	 */
	void placeObject();

	/** 
	 * Removes selected object
	 */
	void removeObjects();

	/** 
	 * Centers selected objects
	 */
	void colorObject();

	/** 
	 * Centers selected objects
	 */
	void centerObject();

	/** 
	 * Apply object translation
	 * @param x x
	 * @param y y
	 * @param z z
	 */
	void objectTranslationApply(float x, float y, float z);

	/** 
	 * Apply object scale
	 * @param x x
	 * @param y y
	 * @param z z
	 */
	void objectScaleApply(float x, float y, float z);

	/** 
	 * Apply object rotations
	 * @param x x
	 * @param y y
	 * @param z z
	 */
	void objectRotationsApply(float x, float y, float z);

	/** 
	 * Save a map property
	 * @param oldName old name
	 * @param name name
	 * @param value value
	 * @return success
	 */
	bool mapPropertySave(const string& oldName, const string& name, const string& value);

	/** 
	 * Add a map property
	 * @return success
	 */
	bool mapPropertyAdd();

	/** 
	 * Remove a map property from model properties
	 * @param name name
	 * @return success
	 */
	bool mapPropertyRemove(const string& name);

	/** 
	 * Remove a object property from object properties
	 * @param name name
	 * @return success
	 */
	bool objectPropertyRemove(const string& name);

	/** 
	 * Apply object property preset
	 * @param presetId preset id
	 */
	void objectPropertiesPreset(const string& presetId);

	/** 
	 * Save a model property
	 * @param oldName old name
	 * @param name name
	 * @param value value
	 * @return success
	 */
	bool objectPropertySave(const string& oldName, const string& name, const string& value);

	/** 
	 * Add a model property
	 * @return success
	 */
	bool objectPropertyAdd();

	/** 
	 * Triggers loading a map
	 */
	void loadMap(const string& path, const string& file);

	/** 
	 * Triggers saving a map
	 */
	void saveMap(const string& pathName, const string& fileName);

private:

	/** 
	 * Copy current selected objects
	 */
	void copyObjects();

	/**
	 * Set paste mode
	 */
	void setPasteMode();

	/**
	 * cancel paste
	 */
	void unsetPasteMode();

	/** 
	 * Paste objects
	 * @param displayOnly display only
	 */
	void pasteObjects(bool displayOnly);

	/**
	 * Update gizmo
	 */
	void updateGizmo();

public:

	/** 
	 * Compute spot direction
	 * @param i light index
	 * @param position position
	 * @param spotTo spot to
	 */
	void computeSpotDirection(int32_t i, const Vector4& position, const Vector3& spotTo);

	/** 
	 * Apply light with index i
	 * @param i light index
	 * @param ambient ambient
	 * @param diffuse diffuse
	 * @param specular specular
	 * @param position position
	 * @param constantAttenuation constant attenuation
	 * @param linearAttenuation linear attenuation
	 * @param quadraticAttenuation quadratic attenuation
	 * @param spotTo spot to
	 * @param spotDirection spot direction
	 * @param spotExponent spot exponent
	 * @param spotCutoff spot cutoff
	 * @param enabled enabled
	 */
	void applyLight(int32_t i, const Color4& ambient, const Color4& diffuse, const Color4& specular, const Vector4& position, float constantAttenuation, float linearAttenuation, float quadraticAttenuation, const Vector3& spotTo, const Vector3& spotDirection, float spotExponent, float spotCutoff, bool enabled);

	/**
	 * Public constructor
	 * @param popUps pop ups
	 */
	LevelEditorView(PopUps* popUps);

	/**
	 * Destructor
	 */
	~LevelEditorView();

};
