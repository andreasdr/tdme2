#pragma once

#include <array>
#include <map>
#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/gui/events/fwd-tdme.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/tools/leveleditor/controller/fwd-tdme.h>
#include <tdme/tools/leveleditor/views/fwd-tdme.h>
#include <tdme/tools/shared/controller/fwd-tdme.h>
#include <tdme/tools/shared/model/fwd-tdme.h>
#include <tdme/utilities/fwd-tdme.h>
#include <tdme/utilities/MutableString.h>
#include <tdme/tools/shared/controller/ScreenController.h>
#include <tdme/gui/events/GUIActionListener.h>
#include <tdme/gui/events/GUIChangeListener.h>

using std::array;
using std::map;
using std::string;
using std::vector;

using tdme::tools::shared::controller::ScreenController;
using tdme::gui::events::GUIActionListener;
using tdme::gui::events::GUIChangeListener;
using tdme::engine::model::Color4;
using tdme::gui::events::GUIActionListener_Type;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUIParentNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::nodes::GUITextNode;
using tdme::math::Vector3;
using tdme::math::Vector4;
using tdme::tools::leveleditor::views::LevelEditorView;
using tdme::tools::shared::controller::FileDialogPath;
using tdme::tools::shared::model::LevelEditorLevel;
using tdme::tools::shared::model::LevelEditorObject;
using tdme::tools::shared::model::LevelEditorLight;
using tdme::tools::shared::model::PropertyModelClass;
using tdme::utilities::MutableString;

/** 
 * Level Editor Screen Controller
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::leveleditor::controller::LevelEditorScreenController final
	: public ScreenController
	, public GUIActionListener
	, public GUIChangeListener
{
private:
	static MutableString CHECKBOX_CHECKED;
	static MutableString CHECKBOX_UNCHECKED;
	static MutableString TEXT_EMPTY;
	LevelEditorView* view { nullptr };
	GUIScreenNode* screenNode { nullptr };
	GUITextNode* screenCaption { nullptr };
	GUIElementNode* btnObjectTranslationApply { nullptr };
	GUIElementNode* btnObjectScaleApply { nullptr };
	GUIElementNode* btnObjectRotationApply { nullptr };
	GUIElementNode* btnObjectColor { nullptr };
	GUIElementNode* btnObjectCenter { nullptr };
	GUIElementNode* btnObjectRemove { nullptr };
	GUIElementNode* gridYPosition { nullptr };
	GUIElementNode* gridEnabled { nullptr };
	GUIElementNode* snappingX { nullptr };
	GUIElementNode* snappingZ { nullptr };
	GUIElementNode* snappingEnabled { nullptr };
	GUIElementNode* mapWidth { nullptr };
	GUIElementNode* mapDepth { nullptr };
	GUIElementNode* mapHeight { nullptr };
	GUIElementNode* mapPropertyName { nullptr };
	GUIElementNode* mapPropertyValue { nullptr };
	GUIElementNode* mapPropertySave { nullptr };
	GUIElementNode* mapPropertyRemove { nullptr };
	GUIElementNode* mapPropertiesListBox { nullptr };
	GUIElementNode* objectName { nullptr };
	GUIElementNode* objectDescription { nullptr };
	GUIElementNode* objectModel { nullptr };
	GUIElementNode* objectCenter { nullptr };
	GUIElementNode* btnObjectDataApply { nullptr };
	GUIElementNode* objectTranslationX { nullptr };
	GUIElementNode* objectTranslationY { nullptr };
	GUIElementNode* objectTranslationZ { nullptr };
	GUIElementNode* objectScaleX { nullptr };
	GUIElementNode* objectScaleY { nullptr };
	GUIElementNode* objectScaleZ { nullptr };
	GUIElementNode* objectRotationX { nullptr };
	GUIElementNode* objectRotationY { nullptr };
	GUIElementNode* objectRotationZ { nullptr };
	GUIElementNode* objectPropertyName { nullptr };
	GUIElementNode* objectPropertyValue { nullptr };
	GUIElementNode* btnObjectPropertySave { nullptr };
	GUIElementNode* btnObjectPropertyAdd { nullptr };
	GUIElementNode* btnObjectPropertyRemove { nullptr };
	GUIElementNode* btnObjectPropertyPresetApply { nullptr };
	GUIElementNode* objectPropertiesListBox { nullptr };
	GUIElementNode* objectPropertiesPresets { nullptr };
	GUIElementNode* objectsListBox { nullptr };
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
	FileDialogPath* mapPath { nullptr };

public:
	GUIScreenNode* getScreenNode() override;

	/** 
	 * @return map path
	 */
	FileDialogPath* getMapPath();
	void initialize() override;
	void dispose() override;

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
	 * Set up level size
	 * @param width width
	 * @param depth depth
	 * @param height height
	 */
	void setLevelSize(float width, float depth, float height);

	/** 
	 * Unset object properties
	 */
	void unsetObjectProperties();

	/** 
	 * @return object property preset selection
	 */
	const string getObjectPropertyPresetSelection();

	/** 
	 * Set up general object data
	 * @param name name
	 * @param description description
	 * @param modelName model name
	 * @param center center
	 */
	void setObjectData(const string& name, const string& description, const string& modelName, const Vector3& center);

	/** 
	 * Unset model data
	 */
	void unsetObjectData();

	/** 
	 * On object data apply
	 */
	void onObjectDataApply();

	/** 
	 * Set up object list box
	 * @param level object by ids hash map
	 */
	void setObjectListbox(LevelEditorLevel* level);

	/** 
	 * Unselect objects in object list box
	 */
	void unselectObjectInObjectListBox(const string& objectId);

	/** 
	 * Unselect objects in object list box
	 */
	void unselectObjectsInObjectListBox();

	/** 
	 * Select a object in object list box
	 * @param objectId object id
	 */
	void selectObjectInObjectListbox(const string& objectId);

	/** 
	 * On objects select button click event
	 */
	void onObjectsSelect();

	/** 
	 * On objects unselect button click event
	 */
	void onObjectsUnselect();

	/** 
	 * Set up object
	 * @param translation translation
	 * @param scale scale
	 * @param rotationX rotation x
	 * @param rotationY rotation y
	 * @param rotationZ rotation z
	 */
	void setObject(const Vector3& translation, const Vector3& scale, float rotationX, float rotationY, float rotationZ);

	/** 
	 * Unset current object
	 */
	void unsetObject();

	/** 
	 * Event callback for map properties selection
	 */
	void onMapPropertiesSelectionChanged();

	/** 
	 * Set up map properties
	 * @param level map properties
	 * @param selectedName selected name
	 */
	void setMapProperties(LevelEditorLevel* level, const string& selectedName);

	/** 
	 * On map property save
	 */
	void onMapPropertySave();

	/** 
	 * On model property add
	 */
	void onMapPropertyAdd();

	/** 
	 * On model property remove
	 */
	void onMapPropertyRemove();

	/** 
	 * Set up object property preset ids
	 * @param objectPresetIds object property preset ids
	 */
	void setObjectPresetIds(const map<string, vector<PropertyModelClass*>>& objectPresetIds);

	/** 
	 * Event callback for object properties selection
	 */
	void onObjectPropertiesSelectionChanged();

	/** 
	 * Set up object properties
	 * @param presetId preset id
	 * @param object object properties
	 * @param selectedName selected name
	 */
	void setObjectProperties(const string& presetId, LevelEditorObject* object, const string& selectedName);

	/** 
	 * On object property save
	 */
	void onObjectPropertySave();

	/** 
	 * On object property add
	 */
	void onObjectPropertyAdd();

	/** 
	 * On object property remove
	 */
	void onObjectPropertyRemove();

	/** 
	 * On quit action
	 */
	void onQuit();

	/** 
	 * On object translation apply action
	 */
	void onObjectTranslationApply();

	/** 
	 * On object scale apply action
	 */
	void onObjectScaleApply();

	/** 
	 * On object rotations apply action
	 */
	void onObjectRotationsApply();

	/** 
	 * On object remove action
	 */
	void onObjectRemove();

	/** 
	 * On object color action
	 */
	void onObjectColor();

	/** 
	 * On object center action
	 */
	void onObjectCenter();

	/** 
	 * On map load action
	 */
	void onMapLoad();

	/** 
	 * On map save action
	 */
	void onMapSave();

	/** 
	 * On grid apply button
	 */
	void onGridApply();

	/** 
	 * On snapping apply button
	 */
	void onSnappingApply();

	/**
	 * On object property preset apply 
	 */
	void onObjectPropertyPresetApply();

	/** 
	 * Set up light presets
	 * @param lightPresetIds light presets
	 */
	void setLightPresetsIds(const map<string, LevelEditorLight*>& lightPresetIds);

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
	void setLight(int32_t i, const Color4& ambient, const Color4& diffuse, const Color4& specular, const Vector4& position, float constAttenuation, float linearAttenuation, float quadraticAttenuation, const Vector3& spotTo, const Vector3& spotDirection, float spotExponent, float spotCutoff, bool enabled);

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
	void onLightApply(int32_t lightIdx);

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
	void onLightPresetApply(int32_t lightIdx);

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
	void onLightSpotDirectionCompute(int32_t lightIdx);
	void saveFile(const string& pathName, const string& fileName) /* throws(Exception) */;
	void loadFile(const string& pathName, const string& fileName) /* throws(Exception) */;
	void onValueChanged(GUIElementNode* node) override;
	void onActionPerformed(GUIActionListener_Type* type, GUIElementNode* node) override;

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

	/**
	 * Public constructor
	 * @param view view
	 */
	LevelEditorScreenController(LevelEditorView* view);
};
