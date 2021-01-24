#pragma once

#include <array>
#include <map>
#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/prototype/fwd-tdme.h>
#include <tdme/engine/prototype/fwd-tdme.h>
#include <tdme/engine/scene/fwd-tdme.h>
#include <tdme/gui/events/fwd-tdme.h>
#include <tdme/gui/events/GUIActionListener.h>
#include <tdme/gui/events/GUIChangeListener.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/tools/sceneeditor/controller/fwd-tdme.h>
#include <tdme/tools/sceneeditor/views/fwd-tdme.h>
#include <tdme/tools/shared/controller/fwd-tdme.h>
#include <tdme/tools/shared/controller/ScreenController.h>
#include <tdme/utilities/fwd-tdme.h>
#include <tdme/utilities/MutableString.h>

using std::array;
using std::map;
using std::string;
using std::vector;

using tdme::engine::model::Color4;
using tdme::engine::prototype::PrototypeProperty;
using tdme::engine::scene::Scene;
using tdme::engine::scene::SceneEntity;
using tdme::engine::scene::SceneLight;
using tdme::gui::events::GUIActionListener;
using tdme::gui::events::GUIActionListenerType;
using tdme::gui::events::GUIChangeListener;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUIParentNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::nodes::GUITextNode;
using tdme::math::Vector3;
using tdme::math::Vector4;
using tdme::tools::sceneeditor::views::SceneEditorView;
using tdme::tools::shared::controller::FileDialogPath;
using tdme::tools::shared::controller::ScreenController;
using tdme::utilities::MutableString;

/**
 * Scene Editor Screen Controller
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::sceneeditor::controller::SceneEditorScreenController final
	: public ScreenController
	, public GUIActionListener
	, public GUIChangeListener
{
private:
	SceneEditorView* view { nullptr };
	GUIScreenNode* screenNode { nullptr };
	GUITextNode* screenCaption { nullptr };
	GUIElementNode* btnEntityTranslationApply { nullptr };
	GUIElementNode* btnEntityScaleApply { nullptr };
	GUIElementNode* btnEntityRotationApply { nullptr };
	GUIElementNode* btnEntityColor { nullptr };
	GUIElementNode* btnEntityCenter { nullptr };
	GUIElementNode* btnEntityRemove { nullptr };
	GUIElementNode* gridYPosition { nullptr };
	GUIElementNode* gridEnabled { nullptr };
	GUIElementNode* snappingX { nullptr };
	GUIElementNode* snappingZ { nullptr };
	GUIElementNode* snappingEnabled { nullptr };
	GUIElementNode* sceneWidth { nullptr };
	GUIElementNode* sceneDepth { nullptr };
	GUIElementNode* sceneHeight { nullptr };
	GUIElementNode* scenePropertyName { nullptr };
	GUIElementNode* scenePropertyValue { nullptr };
	GUIElementNode* scenePropertySave { nullptr };
	GUIElementNode* scenePropertyRemove { nullptr };
	GUIElementNode* scenePropertiesListBox { nullptr };
	GUIElementNode* sceneSkyModel { nullptr };
	GUIElementNode* btnSceneSkyModelLoad { nullptr };
	GUIElementNode* btnSceneSkyModelClear { nullptr };
	GUIElementNode* sceneSkyModelScale { nullptr };
	GUIElementNode* btnSceneSkyApply { nullptr };
	GUIElementNode* entityName { nullptr };
	GUIElementNode* entityDescription { nullptr };
	GUIElementNode* entityModel { nullptr };
	GUIElementNode* entityCenter { nullptr };
	GUIElementNode* btnEntityDataApply { nullptr };
	GUIElementNode* entityTranslationX { nullptr };
	GUIElementNode* entityTranslationY { nullptr };
	GUIElementNode* entityTranslationZ { nullptr };
	GUIElementNode* entityScaleX { nullptr };
	GUIElementNode* entityScaleY { nullptr };
	GUIElementNode* entityScaleZ { nullptr };
	GUIElementNode* entityRotationX { nullptr };
	GUIElementNode* entityRotationY { nullptr };
	GUIElementNode* entityRotationZ { nullptr };
	GUIElementNode* entityPropertyName { nullptr };
	GUIElementNode* entityPropertyValue { nullptr };
	GUIElementNode* btnEntityPropertySave { nullptr };
	GUIElementNode* btnEntityPropertyAdd { nullptr };
	GUIElementNode* btnEntityPropertyRemove { nullptr };
	GUIElementNode* btnEntityPropertyPresetApply { nullptr };
	GUIElementNode* entityPropertiesListBox { nullptr };
	GUIElementNode* entityPropertiesPresets { nullptr };
	GUIElementNode* entitiesListBox { nullptr };
	GUIElementNode* entityReflectionsEnvironmentmappingDropDown { nullptr };
	GUIElementNode* btnEntityReflectionsEnvironmentmappingApply { nullptr };
	array<GUIElementNode*, 4> lightsPresets;
	array<GUIElementNode*, 4> lightsAmbient;
	array<GUIElementNode*, 4> lightsDiffuse;
	array<GUIElementNode*, 4> lightsSpecular;
	array<GUIElementNode*, 4> lightsPosition;
	array<GUIElementNode*, 4> lightsLinAttenuation;
	array<GUIElementNode*, 4> lightsConstAttenuation;
	array<GUIElementNode*, 4> lightsQuadAttenuation;
	array<GUIElementNode*, 4> lightsSpotTo;
	array<GUIElementNode*, 4> lightsSpotDirection;
	array<GUIElementNode*, 4> lightsSpotExponent;
	array<GUIElementNode*, 4> lightsSpotCutoff;
	array<GUIElementNode*, 4> ligthsSpotDirectionCompute;
	array<GUIElementNode*, 4> lightsEnabled;
	GUIElementNode* viewPort { nullptr };
	FileDialogPath* scenePath { nullptr };
	FileDialogPath* modelPath { nullptr };

public:
	/**
	 * Public constructor
	 * @param view view
	 */
	SceneEditorScreenController(SceneEditorView* view);

	/**
	 * @return map path
	 */
	FileDialogPath* getMapPath();

	// overridden methods
	GUIScreenNode* getScreenNode() override;
	void initialize() override;
	void dispose() override;
	void onValueChanged(GUIElementNode* node) override;
	void onActionPerformed(GUIActionListenerType type, GUIElementNode* node) override;

	/**
	 * Set up screen caption
	 * @param text text
	 */
	void setScreenCaption(const string& text);

	/**
	 * Set grid
	 * @param enabled enabled
	 * @param gridY grid y position
	 */
	void setGrid(bool enabled, float gridY);

	/**
	 * Set snapping
	 * @param snappingEnabled snapping enabled
	 * @param snappingX snapping along X axis
	 * @param snappingZ snapping along Z axis
	 */
	void setSnapping(bool snappingEnabled, float snappingX, float snappingZ);

	/**
	 * Set up scene size
	 * @param width width
	 * @param depth depth
	 * @param height height
	 */
	void setSceneSize(float width, float depth, float height);

	/**
	 * Unset entity properties
	 */
	void unsetEntityProperties();

	/**
	 * @return entity property preset selection
	 */
	const string getEntityPropertyPresetSelection();

	/**
	 * Set up general entity data
	 * @param name name
	 * @param description description
	 * @param modelName model name
	 * @param center center
	 */
	void setEntityData(const string& name, const string& description, const string& modelName, const Vector3& center);

	/**
	 * Unset entity data
	 */
	void unsetEntityData();

	/**
	 * On entity data apply
	 */
	void onEntityDataApply();

	/**
	 * Set up entity list box
	 * @param scene scene
	 */
	void setEntityListbox(Scene& scene);

	/**
	 * Unselect entities in object list box
	 */
	void unselectEntitiesInEntityListBox(const string& entityId);

	/**
	 * Unselect entities in object list box
	 */
	void unselectEntitiesInEntityListBox();

	/**
	 * Select a entity in object list box
	 * @param objectId object id
	 */
	void selectEntityInEntityListbox(const string& entityId);

	/**
	 * On entities select button click event
	 */
	void onEntitiesSelect();

	/**
	 * On entities unselect button click event
	 */
	void onEntitiesUnselect();

	/**
	 * Set up entity transformations
	 * @param translation translation
	 * @param scale scale
	 * @param rotationX rotation x
	 * @param rotationY rotation y
	 * @param rotationZ rotation z
	 * @oaram disableRotation disable rotation input fields
	 */
	void setEntityTransformations(const Vector3& translation, const Vector3& scale, float rotationX, float rotationY, float rotationZ, bool disableRotation);

	/**
	 * Unset current entity
	 */
	void unsetEntityTransformations();

	/**
	 * Event callback for scebe properties selection
	 */
	void onScenePropertiesSelectionChanged();

	/**
	 * Set up scene properties
	 * @param scene map properties
	 * @param selectedName selected name
	 */
	void setSceneProperties(Scene& scene, const string& selectedName);

	/**
	 * On scene property save
	 */
	void onScenePropertySave();

	/**
	 * On scene property add
	 */
	void onScenePropertyAdd();

	/**
	 * On scene property remove
	 */
	void onScenePropertyRemove();

	/**
	 * Set up entity property preset ids
	 * @param objectPresetIds object property preset ids
	 */
	void setEntityPresetIds(const map<string, vector<PrototypeProperty*>>& entityPresetIds);

	/**
	 * Event callback for entity properties selection
	 */
	void onEntityPropertiesSelectionChanged();

	/**
	 * Set up entity properties
	 * @param presetId preset id
	 * @param entity entity
	 * @param selectedName selected name
	 */
	void setEntityProperties(const string& presetId, SceneEntity* entity, const string& selectedName);

	/**
	 * On entity property save
	 */
	void onEntityPropertySave();

	/**
	 * On entity property add
	 */
	void onEntityPropertyAdd();

	/**
	 * On entity property remove
	 */
	void onEntityPropertyRemove();

	/**
	 * On quit action
	 */
	void onQuit();

	/**
	 * On entity translation apply action
	 */
	void onEntityTranslationApply();

	/**
	 * On entity scale apply action
	 */
	void onEntityScaleApply();

	/**
	 * On entity rotations apply action
	 */
	void onEntityRotationsApply();

	/**
	 * On entity remove action
	 */
	void onEntityRemove();

	/**
	 * On entity color action
	 */
	void onEntityColor();

	/**
	 * On entity center action
	 */
	void onEntityCenter();

	/**
	 * On scene load action
	 */
	void onSceneLoad();

	/**
	 * On scene save action
	 */
	void onSceneSave();

	/**
	 * On grid apply button
	 */
	void onGridApply();

	/**
	 * On snapping apply button
	 */
	void onSnappingApply();

	/**
	 * On entity property preset apply
	 */
	void onEntityPropertyPresetApply();

	/**
	 * Set up light presets
	 * @param lightPresetIds light presets
	 */
	void setLightPresetsIds(const map<string, SceneLight*>& lightPresetIds);

	/**
	 * Unselect light presets
	 */
	void unselectLightPresets();

	/**
	 * Set up light indexed by i
	 * @param i i
	 * @param ambient ambient
	 * @param diffuse diffuse
	 * @param specular specular
	 * @param position position
	 * @param constAttenuation const attenuation
	 * @param linearAttenuation linear attenuation
	 * @param quadraticAttenuation quadratic attenuation
	 * @param spotTo spot to
	 * @param spotDirection spot direction
	 * @param spotExponent spot exponent
	 * @param spotCutoff spot cutoff
	 * @param enabled enabled
	 */
	void setLight(int i, const Color4& ambient, const Color4& diffuse, const Color4& specular, const Vector4& position, float constAttenuation, float linearAttenuation, float quadraticAttenuation, const Vector3& spotTo, const Vector3& spotDirection, float spotExponent, float spotCutoff, bool enabled);

	/**
	 * On light 0 apply
	 */
	void onLight0Apply();

	/**
	 * On light 1 apply
	 */
	void onLight1Apply();

	/**
	 * On light 2 apply
	 */
	void onLight2Apply();

	/**
	 * On light 3 apply
	 */
	void onLight3Apply();

	/**
	 * On light 3 apply
	 * @param lightIdx light idx
	 */
	void onLightApply(int lightIdx);

	/**
	 * On light 0 preset apply
	 */
	void onLight0PresetApply();

	/**
	 * On light 1 preset apply
	 */
	void onLight1PresetApply();

	/**
	 * On light 2 preset apply
	 */
	void onLight2PresetApply();

	/**
	 * On light 3 preset apply
	 */
	void onLight3PresetApply();

	/**
	 * On light preset apply for light
	 * @param lightIdx i
	 */
	void onLightPresetApply(int lightIdx);

	/**
	 * On Light 0 spot direction compute
	 */
	void onLight0SpotDirectionCompute();

	/**
	 * On Light 1 spot direction compute
	 */
	void onLight1SpotDirectionCompute();

	/**
	 * On Light 2 spot direction compute
	 */
	void onLight2SpotDirectionCompute();

	/**
	 * On Light 3 spot direction compute
	 */
	void onLight3SpotDirectionCompute();

	/**
	 * On Light spot direction compute for given light idx
	 */
	void onLightSpotDirectionCompute(int lightIdx);

	/**
	 * Save file
	 * @param pathName path name
	 * @param fileName file name
	 */
	void saveScene(const string& pathName, const string& fileName);

	/**
	 * Load file
	 * @param pathName path name
	 * @param fileName file name
	 */
	void loadScene(const string& pathName, const string& fileName);

	/**
	 * Set sky
	 * @param scene scene
	 */
	void setSky(Scene& scene);

	/**
	 * On map sky model load
	 */
	void onMapSkyModelLoad();

	/**
	 * On map sky model clear
	 */
	void onMapSkyModelClear();

	/**
	 * On map sky apply
	 */
	void onMapSkyApply();

	/**
	 * Set object reflections environment mappings
	 * @param scene scene
	 * @param selectedEnvironmentMappingId selected environment mapping id
	 */
	void setEntityReflectionsEnvironmentMappings(Scene& scene, const string& selectedEnvironmentMappingId);

	/**
	 * Unset object reflections environment mappings
	 */
	void unsetEntityReflectionsEnvironmentMappings();

	/**
	 * Shows the error pop up
	 */
	void showErrorPopUp(const string& caption, const string& message);

	/**
	 * Get viewport rectangle
	 * @param left left
	 * @param top top
	 * @param width width
	 * @param height height
	 */
	void getViewPort(int& left, int& top, int& width, int& height);

};
