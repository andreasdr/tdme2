#pragma once

#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/gui/events/fwd-tdme.h>
#include <tdme/gui/events/GUIActionListener.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/tools/shared/controller/fwd-tdme.h>
#include <tdme/tools/shared/model/fwd-tdme.h>
#include <tdme/tools/shared/views/fwd-tdme.h>
#include <tdme/utilities/fwd-tdme.h>
#include <tdme/tools/shared/controller/ScreenController.h>
#include <tdme/gui/events/GUIActionListener.h>
#include <tdme/gui/events/GUIChangeListener.h>

using std::string;
using std::vector;

using tdme::engine::model::Material;
using tdme::tools::shared::controller::ScreenController;
using tdme::gui::events::GUIActionListener;
using tdme::gui::events::GUIChangeListener;
using tdme::gui::events::GUIActionListenerType;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::nodes::GUITextNode;
using tdme::math::Vector3;
using tdme::tools::shared::controller::EntityBaseSubScreenController;
using tdme::tools::shared::controller::EntityDisplaySubScreenController;
using tdme::tools::shared::controller::EntityPhysicsSubScreenController;
using tdme::tools::shared::controller::EntitySoundsSubScreenController;
using tdme::tools::shared::controller::FileDialogPath;
using tdme::tools::shared::model::LevelEditorEntity;
using tdme::tools::shared::model::LevelEditorEntityLODLevel;
using tdme::tools::shared::views::SharedModelEditorView;
using tdme::utilities::MutableString;

/**
 * Model editor screen controller
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::shared::controller::ModelEditorScreenController final
	: public ScreenController
	, public GUIActionListener
	, public GUIChangeListener
{

private:
	EntityBaseSubScreenController* entityBaseSubScreenController { nullptr };
	EntityDisplaySubScreenController* entityDisplaySubScreenController { nullptr };
	EntityPhysicsSubScreenController* entityPhysicsSubScreenController { nullptr };
	EntitySoundsSubScreenController* entitySoundsSubScreenController { nullptr };
	SharedModelEditorView* view { nullptr };
	GUIScreenNode* screenNode { nullptr };
	GUITextNode* screenCaption { nullptr };
	GUIElementNode* modelReload { nullptr };
	GUIElementNode* modelReimport { nullptr };
	GUIElementNode* modelSave { nullptr };
	GUIElementNode* pivotX { nullptr };
	GUIElementNode* pivotY { nullptr };
	GUIElementNode* pivotZ { nullptr };
	GUIElementNode* pivotApply { nullptr };
	GUIElementNode* renderingContributesShadows { nullptr };
	GUIElementNode* renderingReceivesShadows { nullptr };
	GUIElementNode* renderingRenderGroups { nullptr };
	GUIElementNode* renderingShader { nullptr };
	GUIElementNode* renderingDistanceShader { nullptr };
	GUIElementNode* renderingDistanceShaderDistance { nullptr };
	GUIElementNode* renderingApply { nullptr };
	GUIElementNode* lodLevel { nullptr };
	GUIElementNode* lodLevelApply { nullptr };
	GUIElementNode* lodType { nullptr };
	GUIElementNode* lodModelFile { nullptr };
	GUIElementNode* lodModelFileLoad { nullptr };
	GUIElementNode* lodModelFileClear { nullptr };
	GUIElementNode* lodMinDistance { nullptr };
	GUIElementNode* lodColorMul { nullptr };
	GUIElementNode* lodColorAdd { nullptr };
	GUIElementNode* buttonLodApply { nullptr };
	GUIElementNode* materialsDropdown { nullptr };
	GUIElementNode* materialsDropdownApply { nullptr };
	GUIElementNode* materialsMaterialName { nullptr };
	GUIElementNode* materialsMaterialAmbient { nullptr };
	GUIElementNode* materialsMaterialDiffuse { nullptr };
	GUIElementNode* materialsMaterialSpecular { nullptr };
	GUIElementNode* materialsMaterialEmission { nullptr };
	GUIElementNode* materialsMaterialShininess { nullptr };
	GUIElementNode* materialsMaterialDiffuseTexture { nullptr };
	GUIElementNode* materialsMaterialDiffuseTransparencyTexture { nullptr };
	GUIElementNode* materialsMaterialNormalTexture { nullptr };
	GUIElementNode* materialsMaterialSpecularTexture { nullptr };
	GUIElementNode* materialsMaterialDiffuseTextureLoad { nullptr };
	GUIElementNode* materialsMaterialDiffuseTransparencyTextureLoad { nullptr };
	GUIElementNode* materialsMaterialNormalTextureLoad { nullptr };
	GUIElementNode* materialsMaterialSpecularTextureLoad { nullptr };
	GUIElementNode* materialsMaterialDiffuseTextureClear { nullptr };
	GUIElementNode* materialsMaterialDiffuseTransparencyTextureClear { nullptr };
	GUIElementNode* materialsMaterialNormalTextureClear { nullptr };
	GUIElementNode* materialsMaterialSpecularTextureClear { nullptr };
	GUIElementNode* materialsMaterialPBREnabled { nullptr };
	GUIElementNode* materialsMaterialPBRBaseColorFactor { nullptr };
	GUIElementNode* materialsMaterialPBRBaseColorTexture { nullptr };
	GUIElementNode* materialsMaterialPBRBaseColorTextureLoad { nullptr };
	GUIElementNode* materialsMaterialPBRBaseColorTextureClear { nullptr };
	GUIElementNode* materialsMaterialPBRMetallicFactor { nullptr };
	GUIElementNode* materialsMaterialPBRRoughnessFactor { nullptr };
	GUIElementNode* materialsMaterialPBRMetallicRoughnessTexture { nullptr };
	GUIElementNode* materialsMaterialPBRMetallicRoughnessTextureLoad { nullptr };
	GUIElementNode* materialsMaterialPBRMetallicRoughnessTextureClear { nullptr };
	GUIElementNode* materialsMaterialPBRNormalScale { nullptr };
	GUIElementNode* materialsMaterialPBRNormalTexture { nullptr };
	GUIElementNode* materialsMaterialPBRNormalTextureLoad { nullptr };
	GUIElementNode* materialsMaterialPBRNormalTextureClear { nullptr };
	GUIElementNode* materialsMaterialPBRExposure { nullptr };
	GUIElementNode* materialsMaterialUseMaskedTransparency { nullptr };
	GUIElementNode* materialsMaterialMaskedTransparencyThreshold { nullptr };
	GUIElementNode* materialsMaterialApply { nullptr };
	GUIElementNode* animationsDropDown { nullptr };
	GUIElementNode* animationsDropDownApply { nullptr };
	GUIElementNode* animationsDropDownDelete { nullptr };
	GUIElementNode* animationsAnimationStartFrame { nullptr };
	GUIElementNode* animationsAnimationEndFrame { nullptr };
	GUIElementNode* animationsAnimationOverlayFromNodeIdDropDown { nullptr };
	GUIElementNode* animationsAnimationLoop { nullptr };
	GUIElementNode* animationsAnimationSpeed { nullptr };
	GUIElementNode* animationsAnimationName { nullptr };
	GUIElementNode* animationsAnimationApply { nullptr };
	GUIElementNode* previewAnimationsBaseDropDown { nullptr };
	GUIElementNode* previewAnimationsOverlay1DropDown { nullptr };
	GUIElementNode* previewAnimationsOverlay2DropDown { nullptr };
	GUIElementNode* previewAnimationsOverlay3DropDown { nullptr };
	GUIElementNode* buttonPreviewApply { nullptr };
	GUIElementNode* buttonToolsComputeNormals { nullptr };
	GUIElementNode* buttonToolsOptimizeModel { nullptr };
	GUIElementNode* statsOpaqueFaces { nullptr };
	GUIElementNode* statsTransparentFaces { nullptr };
	GUIElementNode* statsMaterialCount { nullptr };
	GUIElementNode* viewPort { nullptr };

	FileDialogPath* modelPath { nullptr };
	FileDialogPath* audioPath { nullptr };

	/**
	 * @return level editor entity lod level or nullptr
	 */
	LevelEditorEntityLODLevel* getLODLevel(int level);

	/**
	 * @return current selected material
	 */
	Material* getSelectedMaterial();
public:
	/**
	 * Public constructor
	 * @param view view
	 */
	ModelEditorScreenController(SharedModelEditorView* view);

	/**
	 * Destructor
	 */
	virtual ~ModelEditorScreenController();

	/**
	 * Get view
	 */
	SharedModelEditorView* getView();

	/**
	 * @return entity display sub screen controller
	 */
	EntityDisplaySubScreenController* getEntityDisplaySubScreenController();

	/**
	 * @return entity bounding volume sub screen controller
	 */
	EntityPhysicsSubScreenController* getEntityPhysicsSubScreenController();

	/**
	 * @return entity sounds sub screen controller
	 */
	EntitySoundsSubScreenController* getEntitySoundsSubScreenController();

	// overridden method
	GUIScreenNode* getScreenNode() override;

	/**
	 * @return model path
	 */
	FileDialogPath* getModelPath();

	/**
	 * @return audio path
	 */
	FileDialogPath* getAudioPath();

	// overridden methods
	void initialize() override;
	void dispose() override;

	/**
	 * Set screen caption
	 * @param text text
	 */
	void setScreenCaption(const string& text);

	/**
	 * Set up general entity data
	 * @param name name
	 * @param description description
	 */
	void setEntityData(const string& name, const string& description);

	/**
	 * Unset entity data
	 */
	void unsetEntityData();

	/**
	 * Set up entity properties
	 * @param presetId preset id
	 * @param entity entity properties
	 * @param selectedName selected name
	 */
	void setEntityProperties(const string& presetId, LevelEditorEntity* entity, const string& selectedName);

	/**
	 * Unset entity properties
	 */
	void unsetEntityProperties();

	/**
	 * Set pivot tab
	 * @param pivot pivot
	 */
	void setPivot(const Vector3& pivot);

	/**
	 * Unset pivot tab
	 */
	void unsetPivot();

	/**
	 * Set rendering shader
	 * @param shaders shaders
	 */
	void setRenderingShaders(const vector<string>& shaders);

	/**
	 * Set renering options
	 * @param entity entity
	 */
	void setRendering(LevelEditorEntity* entity);

	/**
	 * Unset rendering
	 */
	void unsetRendering();

	/**
	 * Set lod level
	 * @param entity entity
	 * @param level lod level
	 */
	void setLODLevel(LevelEditorEntity* entity, int level);

	/**
	 * Unset LOD level
	 */
	void unsetLODLevel();

	/**
	 * On LOD level apply
	 */
	void onLODLevelApply();

	/**
	 * On LOD level load model
	 */
	void onLODLevelLoadModel();

	/**
	 * On LOD level clear model
	 */
	void onLODLevelClearModel();

	/**
	 * On LOD level apply settings
	 */
	void onLODLevelApplySettings();

	/**
	 * Set materials
	 * @param entity entity
	 */
	void setMaterials(LevelEditorEntity* entity);

	/**
	 * Unset materials
	 */
	void unsetMaterials();

	/**
	 * On material drop down apply
	 */
	void onMaterialDropDownApply();

	Material* getCurrentMaterial();

	/**
	 * On material apply
	 */
	void onMaterialApply();

	/**
	 * On material load diffuse texture
	 */
	void onMaterialLoadDiffuseTexture();

	/**
	 * On material load diffuse transparency texture
	 */
	void onMaterialLoadDiffuseTransparencyTexture();

	/**
	 * On material load normal texture
	 */
	void onMaterialLoadNormalTexture();

	/**
	 * On material load specular texture
	 */
	void onMaterialLoadSpecularTexture();

	/**
	 * On material load PBR base color texture
	 */
	void onMaterialLoadPBRBaseColorTexture();

	/**
	 * On material load PBR metallic roughness texture
	 */
	void onMaterialLoadPBRMetallicRoughnessTexture();

	/**
	 * On material load PBR normal texture
	 */
	void onMaterialLoadPBRNormalTexture();

	/**
	 * On material PBR enabled value changed
	 */
	void onMaterialPBREnabledValueChanged();

	/**
	 * On material clear texture
	 */
	void onMaterialClearTexture(GUIElementNode* guiElementNode);

	/**
	 * Set animations
	 */
	void setAnimations(LevelEditorEntity* entity);

	/**
	 * On animation drop down value changed
	 */
	void onAnimationDropDownValueChanged();

	/**
	 * On animation drop down apply
	 */
	void onAnimationDropDownApply();

	/**
	 * On animation drop down delete
	 */
	void onAnimationDropDownDelete();

	/**
	 * On animation apply
	 */
	void onAnimationApply();

	/**
	 * Unset animations
	 */
	void unsetAnimations();

	/**
	 * Set preview
	 */
	void setPreview();

	/**
	 * On preview apply
	 */
	void onPreviewApply();

	/**
	 * Unset preview
	 */
	void unsetPreview();

	/**
	 * Set up model statistics
	 * @param statsOpaqueFaces stats opaque faces
	 * @param statsTransparentFaces stats transparent faces
	 * @param statsMaterialCount stats material count
	 */
	void setStatistics(int32_t statsOpaqueFaces, int32_t statsTransparentFaces, int32_t statsMaterialCount);

	/**
	 * Unset statistics
	 */
	void unsetStatistics();

	/**
	 * Set up tool
	 */
	void setTools();

	/**
	 * Unset tools
	 */
	void unsetTools();

	/**
	 * On quit
	 */
	void onQuit();

	/**
	 * On model load
	 */
	void onModelLoad();

	/**
	 * On model save
	 */
	void onModelSave();

	/**
	 * On model reload
	 */
	void onModelReload();

	/**
	 * On model reload
	 */
	void onModelReimport();

	/**
	 * On pivot apply
	 */
	void onPivotApply();

	/**
	 * On rendering apply
	 */
	void onRenderingApply();

	/**
	 * On tools compute normals
	 */
	void onToolsComputeNormal();

	/**
	 * On tools optimize model
	 */
	void onToolsOptimizeModel();

	/**
	 * Save file
	 * @param pathName path name
	 * @param fileName file name
	 */
	void saveFile(const string& pathName, const string& fileName) /* throws(Exception) */;

	/**
	 * Load file
	 * @param pathName path name
	 * @param fileName file name
	 */
	void loadFile(const string& pathName, const string& fileName) /* throws(Exception) */;

	/**
	 * Shows the error pop up
	 */
	void showErrorPopUp(const string& caption, const string& message);

	/**
	 * On value changed
	 * @param node node
	 */
	void onValueChanged(GUIElementNode* node) override;

	/**
	 * On action performed
	 * @param type type
	 * @param node node
	 */
	void onActionPerformed(GUIActionListenerType type, GUIElementNode* node) override;

	/**
	 * Get viewport rectangle
	 * @param left left
	 * @param top top
	 * @param width width
	 * @param height height
	 */
	void getViewPort(int& left, int& top, int& width, int& height);

};
