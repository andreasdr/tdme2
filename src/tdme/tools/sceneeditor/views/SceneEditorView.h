#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/prototype/fwd-tdme.h>
#include <tdme/engine/scene/Scene.h>
#include <tdme/gui/events/GUIInputEventHandler.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Vector4.h>
#include <tdme/tools/sceneeditor/controller/fwd-tdme.h>
#include <tdme/tools/sceneeditor/views/fwd-tdme.h>
#include <tdme/tools/shared/views/fwd-tdme.h>
#include <tdme/tools/shared/views/Gizmo.h>
#include <tdme/tools/shared/views/View.h>
#include <tdme/utilities/fwd-tdme.h>

using std::map;
using std::set;
using std::string;
using std::vector;

using tdme::gui::events::GUIInputEventHandler;
using tdme::tools::shared::views::View;

using tdme::engine::model::Color4;
using tdme::engine::model::Model;
using tdme::engine::prototype::Prototype;
using tdme::engine::scene::Scene;
using tdme::engine::scene::SceneEntity;
using tdme::engine::Engine;
using tdme::engine::Entity;
using tdme::engine::EntityPickingFilter;
using tdme::engine::Object3D;
using tdme::engine::Rotation;
using tdme::math::Vector3;
using tdme::math::Vector4;
using tdme::tools::sceneeditor::controller::SceneEditorScreenController;
using tdme::tools::sceneeditor::views::SceneEditorView_EntityColor;
using tdme::tools::shared::views::Gizmo;
using tdme::tools::shared::views::PopUps;
using tdme::tools::shared::views::View;

/**
 * Scene editor view
 * @author andreas.drewke
 * @version $Id: 04313d20d0978eefc881024d6e0af748196c1425 $
 */
class tdme::tools::sceneeditor::views::SceneEditorView final
	: public View
	, public GUIInputEventHandler
	, protected Gizmo
{
private:
	static vector<string> ENTITYCOLOR_NAMES;
	static constexpr int MOUSE_DOWN_LAST_POSITION_NONE { -1 };
	static constexpr int MOUSE_PANNING_NONE { 0 };
	static constexpr int MOUSE_ROTATION_NONE { 0 };
	float snappingX;
	float snappingZ;
	bool snappingEnabled;
	SceneEditorScreenController* sceneEditorScreenController { nullptr };
	Engine* engine { nullptr };
	Prototype* selectedPrototype { nullptr };
	bool reloadEntityLibrary;
	map<string, SceneEditorView_EntityColor*> entityColors;
	Rotation* camLookRotationX { nullptr };
	Rotation* camLookRotationY { nullptr };
	float camScale;
	float camScaleMax;
	float camScaleMin;
	int placeEntityMouseX;
	int placeEntityMouseY;
	int mouseDownLastX;
	int mouseDownLastY;
	bool mouseDragging;
	Entity* mouseDraggingLastEntity;
	int mousePanningSide;
	int mousePanningForward;
	int mouseRotationX;
	int mouseRotationY;
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
	Scene scene;
	vector<string> selectedEntityIds;
	set<string> selectedEntityIdsById;
	vector<SceneEntity*> copiedEntities;
	PopUps* popUps;
	EntityPickingFilter* entityPickingFilterNoGrid { nullptr };
	EntityPickingFilter* entityPickingFilterPlacing { nullptr };
	bool needGizmoUpdate { false };

public:
	/**
	 * Public constructor
	 * @param popUps pop ups
	 */
	SceneEditorView(PopUps* popUps);

	/**
	 * Destructor
	 */
	~SceneEditorView();

	/**
	 * @return pop ups
	 */
	PopUps* getPopUps();

	/**
	 * @return scene file name
	 */
	const string getFileName();

	/**
	 * @return scene
	 */
	Scene* getScene();

	/**
	 * @return selected entity
	 */
	Prototype* getSelectedPrototype();

	/**
	 * @return selected scene editor object
	 */
	SceneEntity* getSelectedSceneEntity();

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
	void selectPrototypeFromLibrary(int id);
	void handleInputEvents() override;

	/**
	 * Renders the scene
	 */
	void display() override;

	/**
	 * Select entities
	 * @param entityIds object ids
	 */
	void selectEntities(const vector<string>& entityIds);

	/**
	 * Select objects by id
	 */
	void unselectEntities();

	/**
	 * Updates entity list box
	 */
	void setEntityListBox();

	/**
	 * Unselect light presets
	 */
	void unselectLightPresets();

	// overridden methods
	void initialize() override;
	void activate() override;
	void deactivate() override;
	void dispose() override;

	/**
	 * Loads a scene from internal scene representation to tdme
	 */
	void loadScene();

	/**
	 * On object data apply
	 * @param name name
	 * @param description description
	 */
	bool entityDataApply(const string& name, const string& description);

	/**
	 * Initialize place entity mode
	 */
	void setPlaceEntityMode();

	/**
	 * Finish place entity mode
	 */
	void unsetPlaceEntityMode();

	/**
	 * Places selected entity on selected object
	 */
	void placeEntity();

	/**
	 * Removes selected entities
	 */
	void removeEntities();

	/**
	 * Centers selected entities
	 */
	void colorEntities();

	/**
	 * Centers selected entities
	 */
	void centerEntity();

	/**
	 * Apply entity translation
	 * @param x x
	 * @param y y
	 * @param z z
	 */
	void entityTranslationApply(float x, float y, float z);

	/**
	 * Apply entity scale
	 * @param x x
	 * @param y y
	 * @param z z
	 */
	void entityScaleApply(float x, float y, float z);

	/**
	 * Apply entity rotations
	 * @param x x
	 * @param y y
	 * @param z z
	 */
	void entityRotationsApply(float x, float y, float z);

	/**
	 * Save a scene property
	 * @param oldName old name
	 * @param name name
	 * @param value value
	 * @return success
	 */
	bool scenePropertySave(const string& oldName, const string& name, const string& value);

	/**
	 * Add a scene property
	 * @return success
	 */
	bool scenePropertyAdd();

	/**
	 * Remove a scene property from model properties
	 * @param name name
	 * @return success
	 */
	bool scenePropertyRemove(const string& name);

	/**
	 * Remove a entity property from object properties
	 * @param name name
	 * @return success
	 */
	bool entityPropertyRemove(const string& name);

	/**
	 * Apply entity property preset
	 * @param presetId preset id
	 */
	void entityPropertiesPreset(const string& presetId);

	/**
	 * Save a entity property
	 * @param oldName old name
	 * @param name name
	 * @param value value
	 * @return success
	 */
	bool entityPropertySave(const string& oldName, const string& name, const string& value);

	/**
	 * Add a entity property
	 * @return success
	 */
	bool entityPropertyAdd();

	/**
	 * Triggers loading a scene
	 */
	void loadScene(const string& path, const string& file);

	/**
	 * Triggers saving a scene
	 */
	void saveScene(const string& pathName, const string& fileName);


	/**
	 * Compute spot direction
	 * @param i light index
	 * @param position position
	 * @param spotTo spot to
	 */
	void computeSpotDirection(int i, const Vector4& position, const Vector3& spotTo);

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
	void applyLight(int i, const Color4& ambient, const Color4& diffuse, const Color4& specular, const Vector4& position, float constantAttenuation, float linearAttenuation, float quadraticAttenuation, const Vector3& spotTo, const Vector3& spotDirection, float spotExponent, float spotCutoff, bool enabled);

	/**
	 * Update sky
	 */
	void updateSky();

	/**
	 * Update sky position
	 */
	void updateSkyPosition();

	/**
	 * Apply reflection environment mapping id
	 */
	void applyReflectionEnvironmentMappingId(const string& reflectionEnvironmentMappingId);

private:
	/**
	 * Update GUI elements
	 * screen caption
	 * scene size
	 * selected object
	 * object properties
	 * object 3d transformations
	 * object data
	 * ..
	 */
	void updateGUIElements();

	/**
	 * Update GUI transformations elements
	 * scene size
	 * object 3d transformations
	 * ..
	 */
	void updateGUITransformationsElements();

	/**
	 * Load settings
	 */
	void loadSettings();

	/**
	 * Store settings
	 */
	void storeSettings();

	/**
	 * Set highlight entity color effect
	 * @param object object
	 */
	void setHighlightEntityColorEffect(Entity* object);

	/**
	 * Set standard entity color effect
	 * @param object object
	 */
	void setStandardEntityColorEffect(Entity* object);

	/**
	 * Reset scale to scene editor object scale
	 * @param object object
	 */
	void resetEntity(Entity* object);

	/**
	 * Update dynamic grid
	 */
	void updateGrid();

	/**
	 * Remove grid
	 */
	void removeGrid();

	/**
	 * Creates a scene editor ground plate
	 * @return ground
	 */
	Model* createSceneEditorGroundPlateModel();

	/**
	 * Copy current selected entities
	 */
	void copyEntities();

	/**
	 * Set paste mode
	 */
	void setPasteMode();

	/**
	 * cancel paste
	 */
	void unsetPasteMode();

	/**
	 * Paste entities
	 * @param displayOnly display only
	 */
	void pasteEntities(bool displayOnly);

	/**
	 * Update gizmo
	 */
	void updateGizmo();

};
