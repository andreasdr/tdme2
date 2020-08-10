#include <tdme/tools/shared/controller/ModelEditorScreenController.h>

#include <string>
#include <vector>

#include <tdme/engine/fileio/models/ModelReader.h>
#include <tdme/engine/fileio/textures/TextureReader.h>
#include <tdme/engine/model/AnimationSetup.h>
#include <tdme/engine/model/Group.h>
#include <tdme/engine/model/Material.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/model/PBRMaterialProperties.h>
#include <tdme/engine/model/SpecularMaterialProperties.h>
#include <tdme/gui/GUIParser.h>
#include <tdme/gui/events/Action.h>
#include <tdme/gui/events/GUIActionListener.h>
#include <tdme/gui/events/GUIActionListener_Type.h>
#include <tdme/gui/events/GUIChangeListener.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUINodeController.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/gui/nodes/GUITextNode.h>
#include <tdme/math/Vector3.h>
#include <tdme/tools/shared/controller/EntityBaseSubScreenController.h>
#include <tdme/tools/shared/controller/EntityDisplaySubScreenController.h>
#include <tdme/tools/shared/controller/EntityPhysicsSubScreenController.h>
#include <tdme/tools/shared/controller/EntitySoundsSubScreenController.h>
#include <tdme/tools/shared/controller/FileDialogPath.h>
#include <tdme/tools/shared/controller/FileDialogScreenController.h>
#include <tdme/tools/shared/controller/InfoDialogScreenController.h>
#include <tdme/tools/shared/model/LevelEditorEntity.h>
#include <tdme/tools/shared/model/LevelEditorEntityAudio.h>
#include <tdme/tools/shared/model/LevelEditorEntityLODLevel.h>
#include <tdme/tools/shared/model/LevelEditorEntityModel.h>
#include <tdme/tools/shared/tools/Tools.h>
#include <tdme/tools/shared/views/PopUps.h>
#include <tdme/tools/shared/views/SharedModelEditorView.h>
#include <tdme/tools/modeleditor/TDMEModelEditor.h>
#include <tdme/utils/Float.h>
#include <tdme/utils/Integer.h>
#include <tdme/utils/MutableString.h>
#include <tdme/utils/StringUtils.h>
#include <tdme/utils/Console.h>
#include <tdme/utils/Exception.h>
#include <tdme/utils/ExceptionBase.h>

using std::vector;
using std::string;

using tdme::tools::shared::controller::ModelEditorScreenController;

using tdme::engine::fileio::models::ModelReader;
using tdme::engine::fileio::textures::TextureReader;
using tdme::engine::model::AnimationSetup;
using tdme::engine::model::Group;
using tdme::engine::model::Material;
using tdme::engine::model::Model;
using tdme::engine::model::PBRMaterialProperties;
using tdme::engine::model::SpecularMaterialProperties;
using tdme::gui::GUIParser;
using tdme::gui::events::Action;
using tdme::gui::events::GUIActionListener_Type;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeController;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::nodes::GUITextNode;
using tdme::math::Vector3;
using tdme::tools::shared::controller::EntityBaseSubScreenController;
using tdme::tools::shared::controller::EntityDisplaySubScreenController;
using tdme::tools::shared::controller::EntityPhysicsSubScreenController;
using tdme::tools::shared::controller::EntitySoundsSubScreenController;
using tdme::tools::shared::controller::FileDialogPath;
using tdme::tools::shared::controller::FileDialogScreenController;
using tdme::tools::shared::controller::InfoDialogScreenController;
using tdme::tools::shared::model::LevelEditorEntity;
using tdme::tools::shared::model::LevelEditorEntityAudio;
using tdme::tools::shared::model::LevelEditorEntityLODLevel;
using tdme::tools::shared::model::LevelEditorEntityModel;
using tdme::tools::shared::tools::Tools;
using tdme::tools::shared::views::PopUps;
using tdme::tools::shared::views::SharedModelEditorView;
using tdme::tools::viewer::TDMEModelEditor;
using tdme::utils::Float;
using tdme::utils::Integer;
using tdme::utils::MutableString;
using tdme::utils::StringUtils;
using tdme::utils::Console;
using tdme::utils::Exception;
using tdme::utils::ExceptionBase;

ModelEditorScreenController::ModelEditorScreenController(SharedModelEditorView* view) 
{
	class OnSetEntityDataAction: public virtual Action
	{
	public:
		void performAction() override {
			finalView->updateGUIElements();
			finalView->onSetEntityData();
		}

		/**
		 * Public constructor
		 * @param modelEditorScreenController model editor screen controller
		 * @param finalView final view
		 */
		OnSetEntityDataAction(ModelEditorScreenController* modelEditorScreenController, SharedModelEditorView* finalView)
			: modelEditorScreenController(modelEditorScreenController)
			, finalView(finalView) {
		}


	private:
		ModelEditorScreenController* modelEditorScreenController;
		SharedModelEditorView* finalView;
	};

	this->modelPath = new FileDialogPath(".");
	this->audioPath = new FileDialogPath(".");
	this->view = view;
	auto const finalView = view;
	this->entityBaseSubScreenController = new EntityBaseSubScreenController(view->getPopUpsViews(), new OnSetEntityDataAction(this, finalView));
	this->entityPhysicsSubScreenController = new EntityPhysicsSubScreenController(view->getPopUpsViews(), modelPath, true);
	this->entitySoundsSubScreenController = new EntitySoundsSubScreenController(view, view->getPopUpsViews(), audioPath);
	this->entityDisplaySubScreenController = new EntityDisplaySubScreenController(this->entityPhysicsSubScreenController->getView());
}

ModelEditorScreenController::~ModelEditorScreenController() {
	delete modelPath;
	delete entityBaseSubScreenController;
	delete entityDisplaySubScreenController;
	delete entityPhysicsSubScreenController;
}

SharedModelEditorView* ModelEditorScreenController::getView() {
	return view;
}

EntityDisplaySubScreenController* ModelEditorScreenController::getEntityDisplaySubScreenController()
{
	return entityDisplaySubScreenController;
}

EntityPhysicsSubScreenController* ModelEditorScreenController::getEntityPhysicsSubScreenController()
{
	return entityPhysicsSubScreenController;
}

EntitySoundsSubScreenController* ModelEditorScreenController::getEntitySoundsSubScreenController()
{
	return entitySoundsSubScreenController;
}

GUIScreenNode* ModelEditorScreenController::getScreenNode()
{
	return screenNode;
}

FileDialogPath* ModelEditorScreenController::getModelPath()
{
	return modelPath;
}

FileDialogPath* ModelEditorScreenController::getAudioPath()
{
	return audioPath;
}

void ModelEditorScreenController::initialize()
{
	try {
		screenNode = GUIParser::parse("resources/engine/tools/modeleditor/gui", "screen_modeleditor.xml");
		screenNode->addActionListener(this);
		screenNode->addChangeListener(this);
		screenCaption = dynamic_cast< GUITextNode* >(screenNode->getNodeById("screen_caption"));
		modelReload = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_model_reload"));
		modelReimport = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_model_reimport"));
		modelSave = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_model_save"));
		pivotX = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("pivot_x"));
		pivotY = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("pivot_y"));
		pivotZ = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("pivot_z"));
		pivotApply = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_pivot_apply"));
		renderingContributesShadows = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("rendering_contributes_shadows"));
		renderingReceivesShadows = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("rendering_receives_shadows"));
		renderingRenderGroups = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("rendering_render_groups"));
		renderingShader = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("rendering_shader"));
		renderingDistanceShader = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("rendering_distance_shader"));
		renderingDistanceShaderDistance = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("rendering_distance_shader_distance"));
		renderingApply = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_rendering_apply"));
		lodLevel = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("lod_level"));
		lodLevelApply = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("lod_level_apply"));
		lodType = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("lod_type"));
		lodModelFile = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("lod_model_file"));
		lodModelFileLoad = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("lod_model_file_load"));
		lodModelFileClear = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("lod_model_file_clear"));
		lodMinDistance = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("lod_min_distance"));
		lodColorMul = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("lod_color_mul"));
		lodColorAdd = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("lod_color_add"));
		buttonLodApply = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_lod_apply"));
		materialsDropdown = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("materials_dropdown"));
		materialsDropdownApply = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_materials_dropdown_apply"));
		materialsMaterialName = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("materials_material_name"));
		materialsMaterialAmbient = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("materials_material_ambient"));
		materialsMaterialDiffuse = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("materials_material_diffuse"));
		materialsMaterialSpecular= dynamic_cast< GUIElementNode* >(screenNode->getNodeById("materials_material_specular"));
		materialsMaterialEmission = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("materials_material_emission"));
		materialsMaterialShininess = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("materials_material_shininess"));
		materialsMaterialDiffuseTexture = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("materials_material_diffuse_texture"));
		materialsMaterialDiffuseTransparencyTexture = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("materials_material_diffuse_transparency_texture"));
		materialsMaterialNormalTexture = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("materials_material_normal_texture"));
		materialsMaterialSpecularTexture = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("materials_material_specular_texture"));
		materialsMaterialDiffuseTextureLoad = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_materials_material_diffuse_texture_load"));;
		materialsMaterialDiffuseTransparencyTextureLoad = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_materials_material_diffuse_transparency_texture_load"));;
		materialsMaterialNormalTextureLoad = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_materials_material_normal_texture_load"));;
		materialsMaterialSpecularTextureLoad = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_materials_material_specular_texture_load"));;
		materialsMaterialDiffuseTextureClear = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_materials_material_diffuse_texture_clear"));;
		materialsMaterialDiffuseTransparencyTextureClear = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_materials_material_diffuse_transparency_texture_clear"));;
		materialsMaterialNormalTextureClear = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_materials_material_normal_texture_clear"));;
		materialsMaterialSpecularTextureClear = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_materials_material_specular_texture_clear"));;
		materialsMaterialPBREnabled = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("materials_material_pbr_enabled"));
		materialsMaterialPBRBaseColorFactor = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("materials_material_pbr_base_color_factor"));
		materialsMaterialPBRBaseColorTexture = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("materials_material_pbr_base_color_texture"));
		materialsMaterialPBRBaseColorTextureLoad = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_materials_material_pbr_base_color_texture_load"));
		materialsMaterialPBRBaseColorTextureClear = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_materials_material_pbr_base_color_texture_clear"));
		materialsMaterialPBRMetallicFactor = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("materials_material_pbr_metallic_factor"));
		materialsMaterialPBRRoughnessFactor = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("materials_material_pbr_roughness_factor"));
		materialsMaterialPBRMetallicRoughnessTexture = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("materials_material_pbr_metallic_roughness_texture"));
		materialsMaterialPBRMetallicRoughnessTextureLoad = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_materials_material_pbr_metallic_roughness_texture_load"));
		materialsMaterialPBRMetallicRoughnessTextureClear = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_materials_material_pbr_metallic_roughness_texture_clear"));
		materialsMaterialPBRNormalScale = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("materials_material_pbr_normal_scale"));
		materialsMaterialPBRNormalTexture = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("materials_material_pbr_normal_texture"));
		materialsMaterialPBRNormalTextureLoad = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_materials_material_pbr_normal_texture_load"));
		materialsMaterialPBRNormalTextureClear = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_materials_material_pbr_normal_texture_clear"));
		materialsMaterialPBRExposure = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("materials_material_pbr_exposure"));
		materialsMaterialUseMaskedTransparency = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("materials_material_use_masked_transparency"));;
		materialsMaterialMaskedTransparencyThreshold = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("materials_material_masked_transparency_threshold"));;
		materialsMaterialApply = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_materials_material_apply"));
		animationsDropDown = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("animations_dropdown"));
		animationsDropDownApply = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("animations_dropdown_apply"));
		animationsDropDownDelete = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("animations_dropdown_delete"));
		animationsAnimationStartFrame = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("animations_animation_startframe"));
		animationsAnimationEndFrame = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("animations_animation_endframe"));
		animationsAnimationOverlayFromGroupIdDropDown = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("animations_animation_overlayfromgroupidanimations_dropdown"));
		animationsAnimationLoop = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("animations_animation_loop"));
		animationsAnimationSpeed = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("animations_animation_speed"));
		animationsAnimationName = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("animations_animation_name"));
		animationsAnimationApply = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_animations_animation_apply"));
		statsOpaqueFaces = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("stats_opaque_faces"));
		buttonToolsComputeNormals = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_tools_computenormals"));
		statsTransparentFaces = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("stats_transparent_faces"));
		statsMaterialCount = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("stats_material_count"));
		viewPort = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("viewport"));
		statsOpaqueFaces->getController()->setDisabled(true);
		statsTransparentFaces->getController()->setDisabled(true);
		statsMaterialCount->getController()->setDisabled(true);
	} catch (Exception& exception) {
		Console::print(string("ModelEditorScreenController::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	entityBaseSubScreenController->initialize(screenNode);
	entityDisplaySubScreenController->initialize(screenNode);
	entityPhysicsSubScreenController->initialize(screenNode);
	entitySoundsSubScreenController->initialize(screenNode);
}

void ModelEditorScreenController::dispose()
{
}

void ModelEditorScreenController::setScreenCaption(const string& text)
{
	screenCaption->setText(text);
}

void ModelEditorScreenController::setEntityData(const string& name, const string& description)
{
	entityBaseSubScreenController->setEntityData(name, description);
	modelReload->getController()->setDisabled(false);
	modelReimport->getController()->setDisabled(false);
	modelSave->getController()->setDisabled(false);
}

void ModelEditorScreenController::unsetEntityData()
{
	entityBaseSubScreenController->unsetEntityData();
	modelReload->getController()->setDisabled(true);
	modelReimport->getController()->setDisabled(true);
	modelSave->getController()->setDisabled(true);
}

void ModelEditorScreenController::setEntityProperties(const string& presetId, LevelEditorEntity* entity, const string& selectedName)
{
	entityBaseSubScreenController->setEntityProperties(view->getEntity(), presetId, selectedName);
}

void ModelEditorScreenController::unsetEntityProperties()
{
	entityBaseSubScreenController->unsetEntityProperties();
}

void ModelEditorScreenController::setPivot(const Vector3& pivot)
{
	pivotX->getController()->setDisabled(false);
	pivotX->getController()->setValue(MutableString(Tools::formatFloat(pivot.getX())));
	pivotY->getController()->setDisabled(false);
	pivotY->getController()->setValue(MutableString(Tools::formatFloat(pivot.getY())));
	pivotZ->getController()->setDisabled(false);
	pivotZ->getController()->setValue(MutableString(Tools::formatFloat(pivot.getZ())));
	pivotApply->getController()->setDisabled(false);
}

void ModelEditorScreenController::unsetPivot()
{
	pivotX->getController()->setDisabled(true);
	pivotX->getController()->setValue(MutableString());
	pivotY->getController()->setDisabled(true);
	pivotY->getController()->setValue(MutableString());
	pivotZ->getController()->setDisabled(true);
	pivotZ->getController()->setValue(MutableString());
	pivotApply->getController()->setDisabled(true);
}

void ModelEditorScreenController::setRenderingShaders(const vector<string>& shaders) {
	auto idx = 1;
	string renderingShadersDropDownSubNodesXML = "";
	for (auto shader: shaders) {
		renderingShadersDropDownSubNodesXML =
			renderingShadersDropDownSubNodesXML +
			"<dropdown-option text=\"" +
			GUIParser::escapeQuotes(shader) +
			"\" value=\"" +
			GUIParser::escapeQuotes(shader) +
			"\" " +
			(shader == "default"?"selected=\"true\" " : "") +
			"/>\n";
	}
	try {
		dynamic_cast< GUIParentNode* >((renderingShader->getScreenNode()->getNodeById(renderingShader->getId() + "_inner")))->replaceSubNodes(
			"<scrollarea-vertical id=\"" +
				renderingShader->getId() +
				"_inner_scrollarea\" width=\"100%\" height=\"150\">\n" +
				renderingShadersDropDownSubNodesXML +
				"</scrollarea-vertical>\n",
			false
		);
		dynamic_cast< GUIParentNode* >((renderingShader->getScreenNode()->getNodeById(renderingDistanceShader->getId() + "_inner")))->replaceSubNodes(
			"<scrollarea-vertical id=\"" +
				renderingDistanceShader->getId() +
				"_inner_scrollarea\" width=\"100%\" height=\"150\">\n" +
				renderingShadersDropDownSubNodesXML +
				"</scrollarea-vertical>\n",
			false
		);
	} catch (Exception& exception) {
		Console::print(string("ModelEditorScreenController::setRenderingShaders(): An error occurred: "));
		Console::println(string(exception.what()));
	}
}

void ModelEditorScreenController::setRendering(LevelEditorEntity* entity)
{
	renderingContributesShadows->getController()->setDisabled(false);
	renderingContributesShadows->getController()->setValue(MutableString(entity->isContributesShadows() == true?"1":""));
	renderingReceivesShadows->getController()->setDisabled(false);
	renderingReceivesShadows->getController()->setValue(MutableString(entity->isReceivesShadows() == true?"1":""));
	renderingRenderGroups->getController()->setDisabled(false);
	renderingRenderGroups->getController()->setValue(MutableString(entity->isRenderGroups() == true?"1":""));
	renderingShader->getController()->setDisabled(false);
	renderingShader->getController()->setValue(MutableString(entity->getShader()));
	renderingDistanceShader->getController()->setDisabled(false);
	renderingDistanceShader->getController()->setValue(MutableString(entity->getDistanceShader()));
	renderingDistanceShaderDistance->getController()->setDisabled(false);
	renderingDistanceShaderDistance->getController()->setValue(MutableString(entity->getDistanceShaderDistance(), 4));
	renderingApply->getController()->setDisabled(false);
}

void ModelEditorScreenController::unsetRendering()
{
	renderingContributesShadows->getController()->setDisabled(true);
	renderingContributesShadows->getController()->setValue(MutableString("1"));
	renderingReceivesShadows->getController()->setDisabled(true);
	renderingReceivesShadows->getController()->setValue(MutableString("1"));
	renderingRenderGroups->getController()->setDisabled(true);
	renderingRenderGroups->getController()->setValue(MutableString("0"));
	renderingShader->getController()->setDisabled(true);
	renderingShader->getController()->setValue(MutableString("default"));
	renderingDistanceShader->getController()->setDisabled(true);
	renderingDistanceShader->getController()->setValue(MutableString("default"));
	renderingDistanceShaderDistance->getController()->setDisabled(true);
	renderingDistanceShaderDistance->getController()->setValue(MutableString(10000.0f, 4));
	renderingApply->getController()->setDisabled(true);
}

LevelEditorEntityLODLevel* ModelEditorScreenController::getLODLevel(int level) {
	auto entity = view->getEntity();
	switch(level) {
		case 2:
			{
				auto entityLodLevel = entity->getLODLevel2();
				if (entityLodLevel == nullptr) {
					entityLodLevel = new LevelEditorEntityLODLevel(
						LODObject3D::LODLEVELTYPE_NONE,
						"",
						nullptr,
						0.0f
					);
					entity->setLODLevel2(entityLodLevel);
				}
				return entityLodLevel;
			}
		case 3:
			{
				auto entityLodLevel = entity->getLODLevel3();
				if (entityLodLevel == nullptr) {
					entityLodLevel = new LevelEditorEntityLODLevel(
						LODObject3D::LODLEVELTYPE_NONE,
						"",
						nullptr,
						0.0f
					);
					entity->setLODLevel3(entityLodLevel);
				}
				return entityLodLevel;
			}
		default:
			{
				return nullptr;
			}
	}
}

void ModelEditorScreenController::setLODLevel(LevelEditorEntity* entity, int level) {
	auto entityLodLevel = getLODLevel(level);
	if (entityLodLevel == nullptr) {
		lodLevel->getController()->setValue(MutableString(to_string(level)));
		lodLevel->getController()->setDisabled(false);
		lodLevelApply->getController()->setDisabled(false);
		lodType->getController()->setValue(MutableString("1"));
		lodType->getController()->setDisabled(true);
		lodModelFile->getController()->setValue(MutableString(entity->getFileName()));
		lodModelFile->getController()->setDisabled(true);
		lodModelFileLoad->getController()->setDisabled(true);
		lodModelFileClear->getController()->setDisabled(true);
		lodMinDistance->getController()->setValue(MutableString("0.0"));
		lodMinDistance->getController()->setDisabled(true);
		lodColorMul->getController()->setValue(MutableString("1.0, 1.0, 1.0, 1.0"));
		lodColorMul->getController()->setDisabled(true);
		lodColorAdd->getController()->setValue(MutableString("0.0, 0.0, 0.0, 0.0"));
		lodColorAdd->getController()->setDisabled(true);
		buttonLodApply->getController()->setDisabled(true);
	} else {
		lodLevel->getController()->setValue(MutableString(to_string(level)));
		lodLevel->getController()->setDisabled(false);
		lodLevelApply->getController()->setDisabled(false);
		lodType->getController()->setValue(MutableString(to_string(entityLodLevel->getType())));
		lodType->getController()->setDisabled(false);
		lodModelFile->getController()->setValue(MutableString(entityLodLevel->getFileName()));
		lodModelFile->getController()->setDisabled(false);
		lodModelFileLoad->getController()->setDisabled(false);
		lodModelFileClear->getController()->setDisabled(false);
		lodMinDistance->getController()->setValue(MutableString(entityLodLevel->getMinDistance()));
		lodMinDistance->getController()->setDisabled(false);
		lodColorMul->getController()->setValue(MutableString(Tools::formatColor4(entityLodLevel->getColorMul())));
		lodColorMul->getController()->setDisabled(false);
		lodColorAdd->getController()->setValue(MutableString(Tools::formatColor4(entityLodLevel->getColorAdd())));
		lodColorAdd->getController()->setDisabled(false);
		buttonLodApply->getController()->setDisabled(false);
	}
	view->setLodLevel(level);
}

void ModelEditorScreenController::unsetLODLevel() {
	lodLevel->getController()->setValue(MutableString("1"));
	lodLevel->getController()->setDisabled(true);
	lodLevelApply->getController()->setDisabled(true);
	lodType->getController()->setValue(MutableString("0"));
	lodType->getController()->setDisabled(true);
	lodModelFile->getController()->setValue(MutableString());
	lodModelFile->getController()->setDisabled(true);
	lodModelFileLoad->getController()->setDisabled(true);
	lodModelFileClear->getController()->setDisabled(true);
	lodMinDistance->getController()->setValue(MutableString("0.0"));
	lodMinDistance->getController()->setDisabled(true);
	lodColorMul->getController()->setValue(MutableString("1.0, 1.0, 1.0, 1.0"));
	lodColorMul->getController()->setDisabled(true);
	lodColorAdd->getController()->setValue(MutableString("0.0, 0.0, 0.0, 0.0"));
	lodColorAdd->getController()->setDisabled(true);
	buttonLodApply->getController()->setDisabled(true);
}

void ModelEditorScreenController::onLODLevelApply() {
	auto entity = view->getEntity();
	auto lodLevelInt = Tools::convertToIntSilent(lodLevel->getController()->getValue().getString());
	setLODLevel(entity, lodLevelInt);
}

void ModelEditorScreenController::onLODLevelLoadModel() {
	class OnLODModelLoad: public virtual Action
	{
	public:
		void performAction() override {
			modelEditorScreenController->lodModelFile->getController()->setValue(
				MutableString(
					modelEditorScreenController->view->getPopUpsViews()->getFileDialogScreenController()->getPathName() +
					"/" +
					modelEditorScreenController->view->getPopUpsViews()->getFileDialogScreenController()->getFileName()
				)
			);
			modelEditorScreenController->modelPath->setPath(
				modelEditorScreenController->view->getPopUpsViews()->getFileDialogScreenController()->getPathName()
			);
			modelEditorScreenController->view->getPopUpsViews()->getFileDialogScreenController()->close();
		}

		/**
		 * Public constructor
		 * @param modelEditorScreenController model editor screen controller
		 */
		OnLODModelLoad(ModelEditorScreenController* modelEditorScreenController): modelEditorScreenController(modelEditorScreenController) {
		}

	private:
		ModelEditorScreenController *modelEditorScreenController;
	};

	auto lodLevelInt = Tools::convertToIntSilent(lodLevel->getController()->getValue().getString());
	auto entityLodLevel = getLODLevel(lodLevelInt);
	if (entityLodLevel == nullptr) return;
	auto extensions = ModelReader::getModelExtensions();
	view->getPopUpsViews()->getFileDialogScreenController()->show(
		entityLodLevel->getFileName() != ""?Tools::getPath(entityLodLevel->getFileName()):modelPath->getPath(),
		"Load from: ",
		extensions,
		Tools::getFileName(entityLodLevel->getFileName()),
		true,
		new OnLODModelLoad(this)
	);
}

void ModelEditorScreenController::onLODLevelClearModel() {
	lodModelFile->getController()->setValue(MutableString());
}

void ModelEditorScreenController::onLODLevelApplySettings() {
	view->resetEntity();
	auto lodLevelInt = Tools::convertToIntSilent(lodLevel->getController()->getValue().getString());
	LevelEditorEntityLODLevel* entityLodLevel = getLODLevel(lodLevelInt);
	try {
		entityLodLevel->setType(static_cast<LODObject3D::LODLevelType>(Tools::convertToIntSilent(lodType->getController()->getValue().getString())));
		entityLodLevel->setFileName(
			entityLodLevel->getType() == LODObject3D::LODLEVELTYPE_MODEL?
				lodModelFile->getController()->getValue().getString():
				""
			);
		entityLodLevel->setMinDistance(Tools::convertToFloat(lodMinDistance->getController()->getValue().getString()));
		entityLodLevel->setColorMul(Tools::convertToColor4(lodColorMul->getController()->getValue().getString()));
		entityLodLevel->setColorAdd(Tools::convertToColor4(lodColorAdd->getController()->getValue().getString()));
		entityLodLevel->setModel(
			entityLodLevel->getType() == LODObject3D::LODLEVELTYPE_MODEL?
				ModelReader::read(
					Tools::getPath(entityLodLevel->getFileName()),
					Tools::getFileName(entityLodLevel->getFileName())
				):
				nullptr
		);
	} catch (Exception& exception) {
		showErrorPopUp("Warning", (string(exception.what())));
	}
}

void ModelEditorScreenController::setMaterials(LevelEditorEntity* entity) {
	Model* model = view->getLodLevel() == 1?entity->getModel():getLODLevel(view->getLodLevel())->getModel();
	if (model == nullptr) {
		unsetMaterials();
		return;
	}

	{
		auto materialsDropDownInnerNode = dynamic_cast< GUIParentNode* >((materialsDropdown->getScreenNode()->getNodeById(materialsDropdown->getId() + "_inner")));
		auto idx = 0;
		string materialsDropDownInnerNodeSubNodesXML = "";
		materialsDropDownInnerNodeSubNodesXML =
				materialsDropDownInnerNodeSubNodesXML +
			"<scrollarea-vertical id=\"" +
			materialsDropdown->getId() +
			"_inner_scrollarea\" width=\"100%\" height=\"100\">\n";
		// materialsDropDownInnerNodeSubNodesXML = materialsDropDownInnerNodeSubNodesXML + "<dropdown-option text=\"<New animation>\" value=\"<New animation>\" />";
		for (auto it: model->getMaterials()) {
			auto materialId = it.second->getId();
			materialsDropDownInnerNodeSubNodesXML =
					materialsDropDownInnerNodeSubNodesXML + "<dropdown-option text=\"" +
				GUIParser::escapeQuotes(materialId) +
				"\" value=\"" +
				GUIParser::escapeQuotes(materialId) +
				"\" " +
				(idx == 0 ? "selected=\"true\" " : "") +
				" />\n";
			idx++;
		}
		materialsDropDownInnerNodeSubNodesXML = materialsDropDownInnerNodeSubNodesXML + "</scrollarea-vertical>";
		try {
			materialsDropDownInnerNode->replaceSubNodes(materialsDropDownInnerNodeSubNodesXML, true);
		} catch (Exception& exception) {
			Console::print(string("ModelEditorScreenController::setMaterials(): An error occurred: "));
			Console::println(string(exception.what()));
		}
	}

	//
	materialsDropdown->getController()->setDisabled(false);
	materialsDropdownApply->getController()->setDisabled(false);
	materialsMaterialAmbient->getController()->setDisabled(false);
	materialsMaterialDiffuse->getController()->setDisabled(false);
	materialsMaterialSpecular->getController()->setDisabled(false);
	materialsMaterialEmission->getController()->setDisabled(false);
	materialsMaterialShininess->getController()->setDisabled(false);
	materialsMaterialDiffuseTexture->getController()->setDisabled(false);
	materialsMaterialDiffuseTextureLoad->getController()->setDisabled(false);
	materialsMaterialDiffuseTextureClear->getController()->setDisabled(false);
	materialsMaterialDiffuseTransparencyTexture->getController()->setDisabled(false);
	materialsMaterialDiffuseTransparencyTextureLoad->getController()->setDisabled(false);
	materialsMaterialDiffuseTransparencyTextureClear->getController()->setDisabled(false);
	materialsMaterialNormalTexture->getController()->setDisabled(false);
	materialsMaterialNormalTextureLoad->getController()->setDisabled(false);
	materialsMaterialNormalTextureClear->getController()->setDisabled(false);
	materialsMaterialSpecularTexture->getController()->setDisabled(false);
	materialsMaterialSpecularTextureLoad->getController()->setDisabled(false);
	materialsMaterialSpecularTextureClear->getController()->setDisabled(false);
	materialsMaterialUseMaskedTransparency->getController()->setDisabled(false);
	materialsMaterialMaskedTransparencyThreshold->getController()->setDisabled(false);
	materialsMaterialApply->getController()->setDisabled(false);
	onMaterialDropDownApply();
}

void ModelEditorScreenController::unsetMaterials() {
	{
		auto materialsDropDownInnerNode = dynamic_cast< GUIParentNode* >((materialsDropdown->getScreenNode()->getNodeById(materialsDropdown->getId() + "_inner")));
		auto idx = 0;
		string materialsDropDownInnerNodeSubNodesXML = "";
		materialsDropDownInnerNodeSubNodesXML =
				materialsDropDownInnerNodeSubNodesXML +
			"<scrollarea-vertical id=\"" +
			materialsDropdown->getId() +
			"_inner_scrollarea\" width=\"100%\" height=\"100\">\n";
		materialsDropDownInnerNodeSubNodesXML = materialsDropDownInnerNodeSubNodesXML + "<dropdown-option text=\"None\" value=\"\" />";
		materialsDropDownInnerNodeSubNodesXML = materialsDropDownInnerNodeSubNodesXML + "</scrollarea-vertical>";
		try {
			materialsDropDownInnerNode->replaceSubNodes(materialsDropDownInnerNodeSubNodesXML, true);
		} catch (Exception& exception) {
			Console::print(string("ModelEditorScreenController::setMaterials(): An error occurred: "));
			Console::println(string(exception.what()));
		}
	}

	materialsDropdown->getController()->setValue(MutableString());
	materialsDropdown->getController()->setDisabled(true);
	materialsDropdownApply->getController()->setDisabled(true);
	materialsMaterialName->getController()->setValue(MutableString(""));
	materialsMaterialAmbient->getController()->setDisabled(true);
	materialsMaterialAmbient->getController()->setValue(MutableString(""));
	materialsMaterialDiffuse->getController()->setDisabled(true);
	materialsMaterialDiffuse->getController()->setValue(MutableString());
	materialsMaterialSpecular->getController()->setDisabled(true);
	materialsMaterialSpecular->getController()->setValue(MutableString());
	materialsMaterialEmission->getController()->setDisabled(true);
	materialsMaterialEmission->getController()->setValue(MutableString());
	materialsMaterialShininess->getController()->setDisabled(true);
	materialsMaterialShininess->getController()->setValue(MutableString());
	materialsMaterialApply->getController()->setDisabled(true);
	materialsMaterialDiffuseTexture->getController()->setValue(MutableString());
	materialsMaterialDiffuseTextureLoad->getController()->setDisabled(true);
	materialsMaterialDiffuseTextureClear->getController()->setDisabled(true);
	materialsMaterialDiffuseTransparencyTexture->getController()->setValue(MutableString());
	materialsMaterialDiffuseTransparencyTextureLoad->getController()->setDisabled(true);
	materialsMaterialDiffuseTransparencyTextureClear->getController()->setDisabled(true);
	materialsMaterialNormalTexture->getController()->setValue(MutableString());
	materialsMaterialNormalTextureLoad->getController()->setDisabled(true);
	materialsMaterialNormalTextureClear->getController()->setDisabled(true);
	materialsMaterialSpecularTexture->getController()->setValue(MutableString());
	materialsMaterialSpecularTextureLoad->getController()->setDisabled(true);
	materialsMaterialSpecularTextureClear->getController()->setDisabled(true);
	materialsMaterialShininess->getController()->setValue(MutableString());
	materialsMaterialPBREnabled->getController()->setDisabled(true);
	materialsMaterialPBREnabled->getController()->setValue(MutableString());
	materialsMaterialPBRBaseColorFactor->getController()->setDisabled(true);
	materialsMaterialPBRBaseColorFactor->getController()->setValue(MutableString());
	materialsMaterialPBRBaseColorTexture->getController()->setDisabled(true);
	materialsMaterialPBRBaseColorTexture->getController()->setValue(MutableString());
	materialsMaterialPBRBaseColorTextureLoad->getController()->setDisabled(true);
	materialsMaterialPBRBaseColorTextureClear->getController()->setDisabled(true);
	materialsMaterialPBRMetallicFactor->getController()->setDisabled(true);
	materialsMaterialPBRMetallicFactor->getController()->setValue(MutableString());
	materialsMaterialPBRRoughnessFactor->getController()->setDisabled(true);
	materialsMaterialPBRRoughnessFactor->getController()->setValue(MutableString());
	materialsMaterialPBRMetallicRoughnessTexture->getController()->setDisabled(true);
	materialsMaterialPBRMetallicRoughnessTexture->getController()->setValue(MutableString());
	materialsMaterialPBRMetallicRoughnessTextureLoad->getController()->setDisabled(true);
	materialsMaterialPBRMetallicRoughnessTextureClear->getController()->setDisabled(true);
	materialsMaterialPBRNormalScale->getController()->setDisabled(true);
	materialsMaterialPBRNormalScale->getController()->setValue(MutableString());
	materialsMaterialPBRNormalTexture->getController()->setDisabled(true);
	materialsMaterialPBRNormalTexture->getController()->setValue(MutableString());
	materialsMaterialPBRNormalTextureLoad->getController()->setDisabled(true);
	materialsMaterialPBRNormalTextureClear->getController()->setDisabled(true);
	materialsMaterialPBRExposure->getController()->setDisabled(true);
	materialsMaterialPBRExposure->getController()->setValue(MutableString());
	materialsMaterialUseMaskedTransparency->getController()->setValue(MutableString());
	materialsMaterialUseMaskedTransparency->getController()->setDisabled(true);
	materialsMaterialMaskedTransparencyThreshold->getController()->setValue(MutableString());
	materialsMaterialMaskedTransparencyThreshold->getController()->setDisabled(true);
}

void ModelEditorScreenController::onMaterialDropDownApply() {
	auto material = getSelectedMaterial();
	if (material == nullptr) return;
	SpecularMaterialProperties defaultSpecularMaterialProperties;
	auto specularMaterialProperties = material->getSpecularMaterialProperties();
	if (specularMaterialProperties == nullptr) specularMaterialProperties = &defaultSpecularMaterialProperties;
	auto pbrMaterialProperties = material->getPBRMaterialProperties();

	// specular
	materialsMaterialName->getController()->setValue(MutableString(material->getId()));
	materialsMaterialAmbient->getController()->setValue(MutableString(Tools::formatColor4(specularMaterialProperties->getAmbientColor())));
	materialsMaterialDiffuse->getController()->setValue(MutableString(Tools::formatColor4(specularMaterialProperties->getDiffuseColor())));
	materialsMaterialSpecular->getController()->setValue(MutableString(Tools::formatColor4(specularMaterialProperties->getSpecularColor())));
	materialsMaterialEmission->getController()->setValue(MutableString(Tools::formatColor4(specularMaterialProperties->getEmissionColor())));
	materialsMaterialShininess->getController()->setValue(MutableString(Tools::formatFloat(specularMaterialProperties->getShininess())));
	materialsMaterialDiffuseTexture->getController()->setValue(
		MutableString(specularMaterialProperties->getDiffuseTexturePathName()).append(specularMaterialProperties->getDiffuseTexturePathName() == ""?"":"/").append(specularMaterialProperties->getDiffuseTextureFileName())
	);
	materialsMaterialDiffuseTransparencyTexture->getController()->setValue(
		MutableString(specularMaterialProperties->getDiffuseTransparencyTexturePathName()).append(specularMaterialProperties->getDiffuseTransparencyTexturePathName() == ""?"":"/").append(specularMaterialProperties->getDiffuseTransparencyTextureFileName())
	);
	materialsMaterialNormalTexture->getController()->setValue(
		MutableString(specularMaterialProperties->getNormalTexturePathName()).append(specularMaterialProperties->getNormalTexturePathName() == ""?"":"/").append(specularMaterialProperties->getNormalTextureFileName())
	);
	materialsMaterialSpecularTexture->getController()->setValue(
		MutableString(specularMaterialProperties->getSpecularTexturePathName()).append(specularMaterialProperties->getSpecularTexturePathName() == ""?"":"/").append(specularMaterialProperties->getSpecularTextureFileName())
	);

	// pbr
	materialsMaterialPBREnabled->getController()->setDisabled(false);
	materialsMaterialPBREnabled->getController()->setValue(MutableString(pbrMaterialProperties != nullptr?"1":""));
	if (pbrMaterialProperties != nullptr) {
		materialsMaterialPBRBaseColorFactor->getController()->setDisabled(false);
		materialsMaterialPBRBaseColorFactor->getController()->setValue(MutableString(Tools::formatColor4(pbrMaterialProperties->getBaseColorFactor())));
		materialsMaterialPBRBaseColorTexture->getController()->setDisabled(false);
		materialsMaterialPBRBaseColorTexture->getController()->setValue(
			MutableString(
				pbrMaterialProperties->getBaseColorTexturePathName()).append(pbrMaterialProperties->getBaseColorTexturePathName() == ""?"":"/").append(pbrMaterialProperties->getBaseColorTextureFileName()
			)
		);
		materialsMaterialPBRBaseColorTextureLoad->getController()->setDisabled(false);
		materialsMaterialPBRBaseColorTextureClear->getController()->setDisabled(false);
		materialsMaterialPBRMetallicFactor->getController()->setDisabled(false);
		materialsMaterialPBRMetallicFactor->getController()->setValue(MutableString(Tools::formatFloat(pbrMaterialProperties->getMetallicFactor())));
		materialsMaterialPBRRoughnessFactor->getController()->setDisabled(false);
		materialsMaterialPBRRoughnessFactor->getController()->setValue(MutableString(Tools::formatFloat(pbrMaterialProperties->getRoughnessFactor())));
		materialsMaterialPBRMetallicRoughnessTexture->getController()->setDisabled(false);
		materialsMaterialPBRMetallicRoughnessTexture->getController()->setValue(
			MutableString(
				pbrMaterialProperties->getMetallicRoughnessTexturePathName()).append(pbrMaterialProperties->getMetallicRoughnessTexturePathName() == ""?"":"/").append(pbrMaterialProperties->getMetallicRoughnessTextureFileName()
			)
		);
		materialsMaterialPBRMetallicRoughnessTextureLoad->getController()->setDisabled(false);
		materialsMaterialPBRMetallicRoughnessTextureClear->getController()->setDisabled(false);
		materialsMaterialPBRNormalScale->getController()->setDisabled(false);
		materialsMaterialPBRNormalScale->getController()->setValue(MutableString(Tools::formatFloat(pbrMaterialProperties->getNormalScale())));
		materialsMaterialPBRNormalTexture->getController()->setDisabled(false);
		materialsMaterialPBRNormalTexture->getController()->setValue(
			MutableString(
				pbrMaterialProperties->getNormalTexturePathName()).append(pbrMaterialProperties->getNormalTexturePathName() == ""?"":"/").append(pbrMaterialProperties->getNormalTextureFileName()
			)
		);
		materialsMaterialPBRNormalTextureLoad->getController()->setDisabled(false);
		materialsMaterialPBRNormalTextureClear->getController()->setDisabled(false);
		materialsMaterialPBRExposure->getController()->setDisabled(false);
		materialsMaterialPBRExposure->getController()->setValue(MutableString(Tools::formatFloat(pbrMaterialProperties->getExposure())));
	} else {
		materialsMaterialPBRBaseColorFactor->getController()->setDisabled(true);
		materialsMaterialPBRBaseColorFactor->getController()->setValue(MutableString());
		materialsMaterialPBRBaseColorTexture->getController()->setDisabled(true);
		materialsMaterialPBRBaseColorTexture->getController()->setValue(MutableString());
		materialsMaterialPBRBaseColorTextureLoad->getController()->setDisabled(true);
		materialsMaterialPBRBaseColorTextureClear->getController()->setDisabled(true);
		materialsMaterialPBRMetallicFactor->getController()->setDisabled(true);
		materialsMaterialPBRMetallicFactor->getController()->setValue(MutableString());
		materialsMaterialPBRRoughnessFactor->getController()->setDisabled(true);
		materialsMaterialPBRRoughnessFactor->getController()->setValue(MutableString());
		materialsMaterialPBRMetallicRoughnessTexture->getController()->setDisabled(true);
		materialsMaterialPBRMetallicRoughnessTexture->getController()->setValue(MutableString());
		materialsMaterialPBRMetallicRoughnessTextureLoad->getController()->setDisabled(true);
		materialsMaterialPBRMetallicRoughnessTextureClear->getController()->setDisabled(true);
		materialsMaterialPBRNormalScale->getController()->setDisabled(true);
		materialsMaterialPBRNormalScale->getController()->setValue(MutableString());
		materialsMaterialPBRNormalTexture->getController()->setDisabled(true);
		materialsMaterialPBRNormalTexture->getController()->setValue(MutableString());
		materialsMaterialPBRNormalTextureLoad->getController()->setDisabled(true);
		materialsMaterialPBRNormalTextureClear->getController()->setDisabled(true);
		materialsMaterialPBRExposure->getController()->setDisabled(false);
		materialsMaterialPBRExposure->getController()->setValue(MutableString());
	}

	//
	materialsMaterialUseMaskedTransparency->getController()->setValue(MutableString(specularMaterialProperties->hasDiffuseTextureMaskedTransparency() == true || (pbrMaterialProperties != nullptr && pbrMaterialProperties->hasBaseColorTextureMaskedTransparency() == true)?"1":""));
	materialsMaterialMaskedTransparencyThreshold->getController()->setValue(MutableString(Tools::formatFloat(specularMaterialProperties->getDiffuseTextureMaskedTransparencyThreshold())));
}

Material* ModelEditorScreenController::getSelectedMaterial() {
	Model* model = view->getLodLevel() == 1?view->getEntity()->getModel():getLODLevel(view->getLodLevel())->getModel();
	if (model == nullptr) return nullptr;
	auto materialIt = model->getMaterials().find(materialsDropdown->getController()->getValue().getString());
	return materialIt != model->getMaterials().end()?materialIt->second:nullptr;
}

void ModelEditorScreenController::onMaterialApply() {
	auto material = getSelectedMaterial();
	if (material == nullptr) return;
	auto specularMaterialProperties = material->getSpecularMaterialProperties();
	if (specularMaterialProperties == nullptr) {
		specularMaterialProperties = new SpecularMaterialProperties();
		material->setSpecularMaterialProperties(specularMaterialProperties);
	}
	auto pbrMaterialProperties =
		materialsMaterialPBREnabled->getController()->getValue().getString() == "1"?
			(material->getPBRMaterialProperties() != nullptr?material->getPBRMaterialProperties():new PBRMaterialProperties()):
			nullptr;
	material->setPBRMaterialProperties(pbrMaterialProperties);
	try {
		view->resetEntity();
		// specular
		specularMaterialProperties->setAmbientColor(Tools::convertToColor4(materialsMaterialAmbient->getController()->getValue().getString()));
		specularMaterialProperties->setDiffuseColor(Tools::convertToColor4(materialsMaterialDiffuse->getController()->getValue().getString()));
		specularMaterialProperties->setSpecularColor(Tools::convertToColor4(materialsMaterialSpecular->getController()->getValue().getString()));
		specularMaterialProperties->setEmissionColor(Tools::convertToColor4(materialsMaterialEmission->getController()->getValue().getString()));
		specularMaterialProperties->setShininess(Tools::convertToFloat(materialsMaterialShininess->getController()->getValue().getString()));
		specularMaterialProperties->setDiffuseTexture(
			Tools::getPath(materialsMaterialDiffuseTexture->getController()->getValue().getString()),
			Tools::getFileName(materialsMaterialDiffuseTexture->getController()->getValue().getString()),
			Tools::getPath(materialsMaterialDiffuseTransparencyTexture->getController()->getValue().getString()),
			Tools::getFileName(materialsMaterialDiffuseTransparencyTexture->getController()->getValue().getString())
		);
		specularMaterialProperties->setNormalTexture(
			Tools::getPath(materialsMaterialNormalTexture->getController()->getValue().getString()),
			Tools::getFileName(materialsMaterialNormalTexture->getController()->getValue().getString())
		);
		specularMaterialProperties->setSpecularTexture(
			Tools::getPath(materialsMaterialSpecularTexture->getController()->getValue().getString()),
			Tools::getFileName(materialsMaterialSpecularTexture->getController()->getValue().getString())
		);
		specularMaterialProperties->setDiffuseTextureMaskedTransparency(materialsMaterialUseMaskedTransparency->getController()->getValue().getString() == "1"?true:false);
		specularMaterialProperties->setDiffuseTextureMaskedTransparencyThreshold(Tools::convertToFloat(materialsMaterialMaskedTransparencyThreshold->getController()->getValue().getString()));
		// pbr
		if (pbrMaterialProperties != nullptr) {
			pbrMaterialProperties->setBaseColorFactor(Tools::convertToColor4(materialsMaterialPBRBaseColorFactor->getController()->getValue().getString()));
			pbrMaterialProperties->setBaseColorTexture(
				Tools::getPath(materialsMaterialPBRBaseColorTexture->getController()->getValue().getString()),
				Tools::getFileName(materialsMaterialPBRBaseColorTexture->getController()->getValue().getString())
			);
			pbrMaterialProperties->setMetallicFactor(Tools::convertToFloat(materialsMaterialPBRMetallicFactor->getController()->getValue().getString()));
			pbrMaterialProperties->setRoughnessFactor(Tools::convertToFloat(materialsMaterialPBRRoughnessFactor->getController()->getValue().getString()));
			pbrMaterialProperties->setMetallicRoughnessTexture(
				Tools::getPath(materialsMaterialPBRMetallicRoughnessTexture->getController()->getValue().getString()),
				Tools::getFileName(materialsMaterialPBRMetallicRoughnessTexture->getController()->getValue().getString())
			);
			pbrMaterialProperties->setNormalScale(Tools::convertToFloat(materialsMaterialPBRNormalScale->getController()->getValue().getString()));
			pbrMaterialProperties->setNormalTexture(
				Tools::getPath(materialsMaterialPBRNormalTexture->getController()->getValue().getString()),
				Tools::getFileName(materialsMaterialPBRNormalTexture->getController()->getValue().getString())
			);
			pbrMaterialProperties->setExposure(Tools::convertToFloat(materialsMaterialPBRExposure->getController()->getValue().getString()));
			pbrMaterialProperties->setBaseColorTextureMaskedTransparency(materialsMaterialUseMaskedTransparency->getController()->getValue().getString() == "1"?true:false);
			pbrMaterialProperties->setBaseColorTextureMaskedTransparencyThreshold(Tools::convertToFloat(materialsMaterialMaskedTransparencyThreshold->getController()->getValue().getString()));
		} else {
			onMaterialDropDownApply();
		}
	} catch (Exception& exception) {
		showErrorPopUp("Warning", (string(exception.what())));
	}
}

void ModelEditorScreenController::onMaterialLoadDiffuseTexture() {
	auto material = getSelectedMaterial();
	if (material == nullptr) return;
	auto specularMaterialProperties = material->getSpecularMaterialProperties();
	if (specularMaterialProperties == nullptr) {
		specularMaterialProperties = new SpecularMaterialProperties();
		material->setSpecularMaterialProperties(specularMaterialProperties);
	}

	class OnLoadTexture: public virtual Action
	{
	public:
		void performAction() override {
			MutableString value;
			guiElementNode->getController()->setValue(
				MutableString().
					set(modelEditorScreenController->getView()->getPopUpsViews()->getFileDialogScreenController()->getPathName()).
					append("/").
					append(modelEditorScreenController->getView()->getPopUpsViews()->getFileDialogScreenController()->getFileName())
			);
			modelEditorScreenController->getModelPath()->setPath(
				modelEditorScreenController->getView()->getPopUpsViews()->getFileDialogScreenController()->getPathName()
			);
			modelEditorScreenController->getView()->getPopUpsViews()->getFileDialogScreenController()->close();
		}

		/**
		 * Public constructor
		 * @param modelEditorScreenController model editor screen controller
		 * @param guiElementNode gui element node
		 */
		OnLoadTexture(ModelEditorScreenController* modelEditorScreenController, GUIElementNode* guiElementNode)
			: modelEditorScreenController(modelEditorScreenController)
			, guiElementNode(guiElementNode) {
		}


	private:
		ModelEditorScreenController* modelEditorScreenController;
		GUIElementNode* guiElementNode;
	};

	auto extensions = TextureReader::getTextureExtensions();
	view->getPopUpsViews()->getFileDialogScreenController()->show(
		specularMaterialProperties->getDiffuseTextureFileName() != ""?specularMaterialProperties->getDiffuseTexturePathName():modelPath->getPath(),
		"Load from: ",
		extensions,
		specularMaterialProperties->getDiffuseTextureFileName(),
		true,
		new OnLoadTexture(this, materialsMaterialDiffuseTexture)
	);
}

void ModelEditorScreenController::onMaterialLoadDiffuseTransparencyTexture() {
	auto material = getSelectedMaterial();
	if (material == nullptr) return;
	auto specularMaterialProperties = material->getSpecularMaterialProperties();
	if (specularMaterialProperties == nullptr) {
		specularMaterialProperties = new SpecularMaterialProperties();
		material->setSpecularMaterialProperties(specularMaterialProperties);
	}

	class OnLoadTexture: public virtual Action
	{
	public:
		void performAction() override {
			MutableString value;
			guiElementNode->getController()->setValue(
				MutableString().
					set(modelEditorScreenController->getView()->getPopUpsViews()->getFileDialogScreenController()->getPathName()).
					append("/").
					append(modelEditorScreenController->getView()->getPopUpsViews()->getFileDialogScreenController()->getFileName())
			);
			modelEditorScreenController->getModelPath()->setPath(
				modelEditorScreenController->getView()->getPopUpsViews()->getFileDialogScreenController()->getPathName()
			);
			modelEditorScreenController->getView()->getPopUpsViews()->getFileDialogScreenController()->close();
		}

		/**
		 * Public constructor
		 * @param modelEditorScreenController model editor screen controller
		 * @param guiElementNode gui element node
		 */
		OnLoadTexture(ModelEditorScreenController* modelEditorScreenController, GUIElementNode* guiElementNode)
			: modelEditorScreenController(modelEditorScreenController)
			, guiElementNode(guiElementNode) {
		}


	private:
		ModelEditorScreenController* modelEditorScreenController;
		GUIElementNode* guiElementNode;
	};

	auto extensions = TextureReader::getTextureExtensions();
	view->getPopUpsViews()->getFileDialogScreenController()->show(
		specularMaterialProperties->getDiffuseTransparencyTextureFileName() != ""?specularMaterialProperties->getDiffuseTransparencyTexturePathName():modelPath->getPath(),
		"Load from: ",
		extensions,
		specularMaterialProperties->getDiffuseTransparencyTextureFileName(),
		true,
		new OnLoadTexture(this, materialsMaterialDiffuseTransparencyTexture)
	);
}

void ModelEditorScreenController::onMaterialLoadNormalTexture() {
	auto material = getSelectedMaterial();
	if (material == nullptr) return;
	auto specularMaterialProperties = material->getSpecularMaterialProperties();
	if (specularMaterialProperties == nullptr) {
		specularMaterialProperties = new SpecularMaterialProperties();
		material->setSpecularMaterialProperties(specularMaterialProperties);
	}

	class OnLoadTexture: public virtual Action
	{
	public:
		void performAction() override {
			MutableString value;
			guiElementNode->getController()->setValue(
				MutableString().
					set(modelEditorScreenController->getView()->getPopUpsViews()->getFileDialogScreenController()->getPathName()).
					append("/").
					append(modelEditorScreenController->getView()->getPopUpsViews()->getFileDialogScreenController()->getFileName())
			);
			modelEditorScreenController->getModelPath()->setPath(
				modelEditorScreenController->getView()->getPopUpsViews()->getFileDialogScreenController()->getPathName()
			);
			modelEditorScreenController->getView()->getPopUpsViews()->getFileDialogScreenController()->close();
		}

		/**
		 * Public constructor
		 * @param modelEditorScreenController model editor screen controller
		 * @param guiElementNode gui element node
		 */
		OnLoadTexture(ModelEditorScreenController* modelEditorScreenController, GUIElementNode* guiElementNode)
			: modelEditorScreenController(modelEditorScreenController)
			, guiElementNode(guiElementNode) {
		}


	private:
		ModelEditorScreenController* modelEditorScreenController;
		GUIElementNode* guiElementNode;
	};

	auto extensions = TextureReader::getTextureExtensions();
	view->getPopUpsViews()->getFileDialogScreenController()->show(
		specularMaterialProperties->getNormalTextureFileName() != ""?specularMaterialProperties->getNormalTexturePathName():modelPath->getPath(),
		"Load from: ",
		extensions,
		specularMaterialProperties->getNormalTextureFileName(),
		true,
		new OnLoadTexture(this, materialsMaterialNormalTexture)
	);
}

void ModelEditorScreenController::onMaterialLoadSpecularTexture() {
	auto material = getSelectedMaterial();
	if (material == nullptr) return;
	auto specularMaterialProperties = material->getSpecularMaterialProperties();
	if (specularMaterialProperties == nullptr) {
		specularMaterialProperties = new SpecularMaterialProperties();
		material->setSpecularMaterialProperties(specularMaterialProperties);
	}

	class OnLoadTexture: public virtual Action
	{
	public:
		void performAction() override {
			MutableString value;
			guiElementNode->getController()->setValue(
				MutableString().
					set(modelEditorScreenController->getView()->getPopUpsViews()->getFileDialogScreenController()->getPathName()).
					append("/").
					append(modelEditorScreenController->getView()->getPopUpsViews()->getFileDialogScreenController()->getFileName())
			);
			modelEditorScreenController->getModelPath()->setPath(
				modelEditorScreenController->getView()->getPopUpsViews()->getFileDialogScreenController()->getPathName()
			);
			modelEditorScreenController->getView()->getPopUpsViews()->getFileDialogScreenController()->close();
		}

		/**
		 * Public constructor
		 * @param modelEditorScreenController model editor screen controller
		 * @param guiElementNode gui element node
		 */
		OnLoadTexture(ModelEditorScreenController* modelEditorScreenController, GUIElementNode* guiElementNode)
			: modelEditorScreenController(modelEditorScreenController)
			, guiElementNode(guiElementNode) {
		}


	private:
		ModelEditorScreenController* modelEditorScreenController;
		GUIElementNode* guiElementNode;
	};

	auto extensions = TextureReader::getTextureExtensions();
	view->getPopUpsViews()->getFileDialogScreenController()->show(
		specularMaterialProperties->getSpecularTextureFileName() != ""?specularMaterialProperties->getSpecularTexturePathName():modelPath->getPath(),
		"Load from: ",
		extensions,
		specularMaterialProperties->getSpecularTextureFileName(),
		true,
		new OnLoadTexture(this, materialsMaterialNormalTexture)
	);
}

void ModelEditorScreenController::onMaterialLoadPBRBaseColorTexture() {
	auto material = getSelectedMaterial();
	if (material == nullptr) return;
	auto pbrMaterialProperties = material->getPBRMaterialProperties();

	class OnLoadTexture: public virtual Action
	{
	public:
		void performAction() override {
			MutableString value;
			guiElementNode->getController()->setValue(
				MutableString().
					set(modelEditorScreenController->getView()->getPopUpsViews()->getFileDialogScreenController()->getPathName()).
					append("/").
					append(modelEditorScreenController->getView()->getPopUpsViews()->getFileDialogScreenController()->getFileName())
			);
			modelEditorScreenController->getModelPath()->setPath(
				modelEditorScreenController->getView()->getPopUpsViews()->getFileDialogScreenController()->getPathName()
			);
			modelEditorScreenController->getView()->getPopUpsViews()->getFileDialogScreenController()->close();
		}

		/**
		 * Public constructor
		 * @param modelEditorScreenController model editor screen controller
		 * @param guiElementNode gui element node
		 */
		OnLoadTexture(ModelEditorScreenController* modelEditorScreenController, GUIElementNode* guiElementNode)
			: modelEditorScreenController(modelEditorScreenController)
			, guiElementNode(guiElementNode) {
		}


	private:
		ModelEditorScreenController* modelEditorScreenController;
		GUIElementNode* guiElementNode;
	};

	auto extensions = TextureReader::getTextureExtensions();
	view->getPopUpsViews()->getFileDialogScreenController()->show(
		pbrMaterialProperties != nullptr && pbrMaterialProperties->getBaseColorTextureFileName() != ""?pbrMaterialProperties->getBaseColorTexturePathName():modelPath->getPath(),
		"Load from: ",
		extensions,
		pbrMaterialProperties != nullptr?pbrMaterialProperties->getBaseColorTextureFileName():"",
		true,
		new OnLoadTexture(this, materialsMaterialPBRBaseColorTexture)
	);
}

void ModelEditorScreenController::onMaterialLoadPBRMetallicRoughnessTexture() {
	auto material = getSelectedMaterial();
	if (material == nullptr) return;
	auto pbrMaterialProperties = material->getPBRMaterialProperties();

	class OnLoadTexture: public virtual Action
	{
	public:
		void performAction() override {
			MutableString value;
			guiElementNode->getController()->setValue(
				MutableString().
					set(modelEditorScreenController->getView()->getPopUpsViews()->getFileDialogScreenController()->getPathName()).
					append("/").
					append(modelEditorScreenController->getView()->getPopUpsViews()->getFileDialogScreenController()->getFileName())
			);
			modelEditorScreenController->getModelPath()->setPath(
				modelEditorScreenController->getView()->getPopUpsViews()->getFileDialogScreenController()->getPathName()
			);
			modelEditorScreenController->getView()->getPopUpsViews()->getFileDialogScreenController()->close();
		}

		/**
		 * Public constructor
		 * @param modelEditorScreenController model editor screen controller
		 * @param guiElementNode gui element node
		 */
		OnLoadTexture(ModelEditorScreenController* modelEditorScreenController, GUIElementNode* guiElementNode)
			: modelEditorScreenController(modelEditorScreenController)
			, guiElementNode(guiElementNode) {
		}


	private:
		ModelEditorScreenController* modelEditorScreenController;
		GUIElementNode* guiElementNode;
	};

	auto extensions = TextureReader::getTextureExtensions();
	view->getPopUpsViews()->getFileDialogScreenController()->show(
		pbrMaterialProperties != nullptr && pbrMaterialProperties->getMetallicRoughnessTextureFileName() != ""?pbrMaterialProperties->getMetallicRoughnessTexturePathName():modelPath->getPath(),
		"Load from: ",
		extensions,
		pbrMaterialProperties != nullptr?pbrMaterialProperties->getMetallicRoughnessTextureFileName():"",
		true,
		new OnLoadTexture(this, materialsMaterialPBRMetallicRoughnessTexture)
	);
}

void ModelEditorScreenController::onMaterialLoadPBRNormalTexture() {
	auto material = getSelectedMaterial();
	if (material == nullptr) return;
	auto pbrMaterialProperties = material->getPBRMaterialProperties();

	class OnLoadTexture: public virtual Action
	{
	public:
		void performAction() override {
			MutableString value;
			guiElementNode->getController()->setValue(
				MutableString().
					set(modelEditorScreenController->getView()->getPopUpsViews()->getFileDialogScreenController()->getPathName()).
					append("/").
					append(modelEditorScreenController->getView()->getPopUpsViews()->getFileDialogScreenController()->getFileName())
			);
			modelEditorScreenController->getModelPath()->setPath(
				modelEditorScreenController->getView()->getPopUpsViews()->getFileDialogScreenController()->getPathName()
			);
			modelEditorScreenController->getView()->getPopUpsViews()->getFileDialogScreenController()->close();
		}

		/**
		 * Public constructor
		 * @param modelEditorScreenController model editor screen controller
		 * @param guiElementNode gui element node
		 */
		OnLoadTexture(ModelEditorScreenController* modelEditorScreenController, GUIElementNode* guiElementNode)
			: modelEditorScreenController(modelEditorScreenController)
			, guiElementNode(guiElementNode) {
		}


	private:
		ModelEditorScreenController* modelEditorScreenController;
		GUIElementNode* guiElementNode;
	};

	auto extensions = TextureReader::getTextureExtensions();
	view->getPopUpsViews()->getFileDialogScreenController()->show(
		pbrMaterialProperties != nullptr && pbrMaterialProperties->getNormalTextureFileName() != ""?pbrMaterialProperties->getNormalTexturePathName():modelPath->getPath(),
		"Load from: ",
		extensions,
		pbrMaterialProperties != nullptr?pbrMaterialProperties->getNormalTextureFileName():"",
		true,
		new OnLoadTexture(this, materialsMaterialPBRNormalTexture)
	);
}

void ModelEditorScreenController::onMaterialClearTexture(GUIElementNode* guiElementNode) {
	guiElementNode->getController()->setValue(MutableString(""));
}

void ModelEditorScreenController::setAnimations(LevelEditorEntity* entity) {
	Model* model = view->getLodLevel() == 1?view->getEntity()->getModel():getLODLevel(view->getLodLevel())->getModel();
	if (model == nullptr) {
		unsetAnimations();
		return;
	}

	{
		auto animationsDropDownInnerNode = dynamic_cast< GUIParentNode* >((animationsDropDown->getScreenNode()->getNodeById(animationsDropDown->getId() + "_inner")));
		auto idx = 0;
		string animationsDropDownInnerNodeSubNodesXML = "";
		animationsDropDownInnerNodeSubNodesXML =
			animationsDropDownInnerNodeSubNodesXML +
			"<scrollarea-vertical id=\"" +
			animationsDropDown->getId() +
			"_inner_scrollarea\" width=\"100%\" height=\"100\">\n";
		animationsDropDownInnerNodeSubNodesXML = animationsDropDownInnerNodeSubNodesXML + "<dropdown-option text=\"<New animation>\" value=\"<New animation>\" />";
		for (auto it: model->getAnimationSetups()) {
			auto animationSetupId = it.second->getId();
			animationsDropDownInnerNodeSubNodesXML =
				animationsDropDownInnerNodeSubNodesXML + "<dropdown-option text=\"" +
				GUIParser::escapeQuotes(animationSetupId) +
				"\" value=\"" +
				GUIParser::escapeQuotes(animationSetupId) +
				"\" " +
				(idx == 0 ? "selected=\"true\" " : "") +
				" />\n";
			idx++;
		}
		animationsDropDownInnerNodeSubNodesXML = animationsDropDownInnerNodeSubNodesXML + "</scrollarea-vertical>";
		try {
			animationsDropDownInnerNode->replaceSubNodes(animationsDropDownInnerNodeSubNodesXML, true);
		} catch (Exception& exception) {
			Console::print(string("ModelEditorScreenController::setAnimations(): An error occurred: "));
			Console::println(string(exception.what()));
		}
		// TODO: this usually works most of the time out of the box, so custom layouting is not required, but in this case not, need to find out whats going wrong there
		// animationsDropDown->getScreenNode()->layout(animationsDropDown);
	}

	{
		auto animationsAnimationOverlayFromGroupIdDropDownInnerNode = dynamic_cast< GUIParentNode* >((animationsAnimationOverlayFromGroupIdDropDown->getScreenNode()->getNodeById(animationsAnimationOverlayFromGroupIdDropDown->getId() + "_inner")));
		auto idx = 0;
		string animationsAnimationOverlayFromGroupIdDropDownInnerNodeSubNodesXML = "";
		animationsAnimationOverlayFromGroupIdDropDownInnerNodeSubNodesXML =
			animationsAnimationOverlayFromGroupIdDropDownInnerNodeSubNodesXML +
			"<scrollarea-vertical id=\"" +
			animationsAnimationOverlayFromGroupIdDropDown->getId() +
			"_inner_scrollarea\" width=\"100%\" height=\"70\">\n";
		animationsAnimationOverlayFromGroupIdDropDownInnerNodeSubNodesXML =
			animationsAnimationOverlayFromGroupIdDropDownInnerNodeSubNodesXML +
			"<dropdown-option text=\"\" value=\"\"" +
			(idx == 0 ? "selected=\"true\" " : "") +
			" />\n";
		for (auto it: model->getGroups()) {
			auto groupId = it.second->getId();
			animationsAnimationOverlayFromGroupIdDropDownInnerNodeSubNodesXML =
				animationsAnimationOverlayFromGroupIdDropDownInnerNodeSubNodesXML +
				"<dropdown-option text=\"" +
				GUIParser::escapeQuotes(groupId) +
				"\" value=\"" +
				GUIParser::escapeQuotes(groupId) +
				"\" " +
				(idx == 0 ? "selected=\"true\" " : "") +
				" />\n";
			idx++;
		}
		animationsAnimationOverlayFromGroupIdDropDownInnerNodeSubNodesXML = animationsAnimationOverlayFromGroupIdDropDownInnerNodeSubNodesXML + "</scrollarea-vertical>";
		try {
			animationsAnimationOverlayFromGroupIdDropDownInnerNode->replaceSubNodes(animationsAnimationOverlayFromGroupIdDropDownInnerNodeSubNodesXML, true);
		} catch (Exception& exception) {
			Console::print(string("ModelEditorScreenController::setAnimations(): An error occurred: "));
			Console::println(string(exception.what()));
		}
		// TODO: this usually works most of the time out of the box, so custom layouting is not required, but in this case not, need to find out whats going wrong there
		// animationsAnimationOverlayFromGroupIdDropDown->getScreenNode()->layout(animationsAnimationOverlayFromGroupIdDropDown);
	}

	// select default animation
	animationsDropDown->getController()->setValue(MutableString(Model::ANIMATIONSETUP_DEFAULT));

	// apply
	onAnimationDropDownApply();
}

void ModelEditorScreenController::onAnimationDropDownValueChanged() {
	Model* model = view->getLodLevel() == 1?view->getEntity()->getModel():getLODLevel(view->getLodLevel())->getModel();
	if (model == nullptr) return;

	auto animationSetup = model->getAnimationSetup(animationsDropDown->getController()->getValue().getString());
	auto defaultAnimation = animationSetup != nullptr && animationSetup->getId() == Model::ANIMATIONSETUP_DEFAULT;
	animationsDropDownDelete->getController()->setDisabled(defaultAnimation || animationSetup == nullptr);
}

void ModelEditorScreenController::onAnimationDropDownApply() {
	Model* model = view->getLodLevel() == 1?view->getEntity()->getModel():getLODLevel(view->getLodLevel())->getModel();
	if (model == nullptr) return;

	auto animationName = animationsDropDown->getController()->getValue().getString();

	auto animationSetup = model->getAnimationSetup(animationName);
	AnimationSetup newAnimationSetup(
		model,
		"New animation",
		model->getAnimationSetup(Model::ANIMATIONSETUP_DEFAULT)->getStartFrame(),
		model->getAnimationSetup(Model::ANIMATIONSETUP_DEFAULT)->getEndFrame(),
		true,
		""
	);
	if (animationSetup == nullptr) animationSetup = &newAnimationSetup;
	auto defaultAnimation = animationSetup->getId() == Model::ANIMATIONSETUP_DEFAULT;

	animationsDropDown->getController()->setDisabled(false);
	animationsDropDownApply->getController()->setDisabled(false);
	animationsDropDownDelete->getController()->setDisabled(defaultAnimation || animationSetup == &newAnimationSetup);
	animationsAnimationStartFrame->getController()->setValue(MutableString(animationSetup->getStartFrame()));
	animationsAnimationStartFrame->getController()->setDisabled(defaultAnimation);
	animationsAnimationEndFrame->getController()->setValue(MutableString(animationSetup->getEndFrame()));
	animationsAnimationEndFrame->getController()->setDisabled(defaultAnimation);
	animationsAnimationOverlayFromGroupIdDropDown->getController()->setValue(MutableString(animationSetup->getOverlayFromGroupId()));
	animationsAnimationOverlayFromGroupIdDropDown->getController()->setDisabled(defaultAnimation);
	animationsAnimationLoop->getController()->setValue(MutableString(animationSetup->isLoop() == true?"1":""));
	animationsAnimationLoop->getController()->setDisabled(defaultAnimation);
	animationsAnimationSpeed->getController()->setValue(MutableString(animationSetup->getSpeed(), 4));
	animationsAnimationSpeed->getController()->setDisabled(defaultAnimation);
	animationsAnimationName->getController()->setValue(MutableString(animationSetup->getId()));
	animationsAnimationName->getController()->setDisabled(defaultAnimation);
	animationsAnimationApply->getController()->setDisabled(defaultAnimation);
	if (animationSetup != &newAnimationSetup) view->playAnimation(animationSetup->getId());
}

void ModelEditorScreenController::onAnimationDropDownDelete() {
	Model* model = view->getLodLevel() == 1?view->getEntity()->getModel():getLODLevel(view->getLodLevel())->getModel();
	if (model == nullptr) return;

	auto animationName = animationsDropDown->getController()->getValue().getString();
	auto animationSetup = model->getAnimationSetup(animationName);
	auto it = model->getAnimationSetups().find(animationSetup->getId());
	it = model->getAnimationSetups().erase(it);
	delete animationSetup;

	setAnimations(view->getEntity());
	animationsDropDown->getController()->setValue(MutableString(it->second->getId()));
	onAnimationDropDownApply();
}

void ModelEditorScreenController::onAnimationApply() {
	Model* model = view->getLodLevel() == 1?view->getEntity()->getModel():getLODLevel(view->getLodLevel())->getModel();
	if (model == nullptr) return;

	auto animationName = animationsDropDown->getController()->getValue().getString();
	auto animationSetup = model->getAnimationSetup(animationName);
	try {
		if (animationSetup == nullptr) {
			if (model->getAnimationSetup(animationsAnimationName->getController()->getValue().getString()) != nullptr) {
				throw ExceptionBase("Name '" + animationsAnimationName->getController()->getValue().getString() + "' already in use");
			}
			animationSetup = model->addAnimationSetup(
				animationsAnimationName->getController()->getValue().getString(),
				0,
				0,
				false
			);
		} else
		if (animationSetup->getId() != animationsAnimationName->getController()->getValue().getString()) {
			if (model->getAnimationSetup(animationsAnimationName->getController()->getValue().getString()) != nullptr) {
				throw ExceptionBase("Name '" + animationsAnimationName->getController()->getValue().getString() + "' already in use");
			}
			model->getAnimationSetups().erase(animationSetup->getId());
			animationSetup = model->addAnimationSetup(
				animationsAnimationName->getController()->getValue().getString(),
				0,
				0,
				false
			);
		}
		animationSetup->setStartFrame(Integer::parseInt(animationsAnimationStartFrame->getController()->getValue().getString()));
		animationSetup->setEndFrame(Integer::parseInt(animationsAnimationEndFrame->getController()->getValue().getString()));
		animationSetup->setOverlayFromGroupId(animationsAnimationOverlayFromGroupIdDropDown->getController()->getValue().getString());
		animationSetup->setLoop(animationsAnimationLoop->getController()->getValue().getString() == "1");
		animationSetup->setSpeed(Float::parseFloat(animationsAnimationSpeed->getController()->getValue().getString()));
		setAnimations(view->getEntity());
		animationsDropDown->getController()->setValue(MutableString(animationSetup->getId()));
		onAnimationDropDownApply();
		view->playAnimation(animationSetup->getId());
	} catch (Exception& exception) {
		showErrorPopUp("Warning", (string(exception.what())));
	}
}

void ModelEditorScreenController::unsetAnimations() {

	dynamic_cast<GUIParentNode*>(animationsDropDown->getScreenNode()->getNodeById(animationsDropDown->getId() + "_inner"))->clearSubNodes();
	animationsDropDown->getController()->setValue(MutableString(""));
	animationsDropDown->getController()->setDisabled(true);
	animationsDropDownApply->getController()->setDisabled(true);
	animationsDropDownDelete->getController()->setDisabled(true);
	animationsAnimationStartFrame->getController()->setValue(MutableString(""));
	animationsAnimationStartFrame->getController()->setDisabled(true);
	animationsAnimationEndFrame->getController()->setValue(MutableString(""));
	animationsAnimationEndFrame->getController()->setDisabled(true);
	animationsAnimationOverlayFromGroupIdDropDown->getController()->setValue(MutableString(""));
	animationsAnimationOverlayFromGroupIdDropDown->getController()->setDisabled(true);
	animationsAnimationLoop->getController()->setValue(MutableString(""));
	animationsAnimationLoop->getController()->setDisabled(true);
	animationsAnimationSpeed->getController()->setValue(MutableString(""));
	animationsAnimationSpeed->getController()->setDisabled(true);
	animationsAnimationName->getController()->setValue(MutableString(""));
	animationsAnimationName->getController()->setDisabled(true);
	animationsAnimationApply->getController()->setDisabled(true);
}

void ModelEditorScreenController::setStatistics(int32_t statsOpaqueFaces, int32_t statsTransparentFaces, int32_t statsMaterialCount)
{
	this->statsOpaqueFaces->getController()->setValue(MutableString(statsOpaqueFaces));
	this->statsTransparentFaces->getController()->setValue(MutableString(statsTransparentFaces));
	this->statsMaterialCount->getController()->setValue(MutableString(statsMaterialCount));
}

void ModelEditorScreenController::unsetStatistics()
{
	this->statsOpaqueFaces->getController()->setValue(MutableString());
	this->statsTransparentFaces->getController()->setValue(MutableString());
	this->statsMaterialCount->getController()->setValue(MutableString());
}

void ModelEditorScreenController::setTools() {
	buttonToolsComputeNormals->getController()->setDisabled(false);
}

void ModelEditorScreenController::unsetTools() {
	buttonToolsComputeNormals->getController()->setDisabled(true);
}

void ModelEditorScreenController::onToolsComputeNormal() {
	view->computeNormals();
}

void ModelEditorScreenController::onQuit()
{
	TDMEModelEditor::getInstance()->quit();
}

void ModelEditorScreenController::onModelLoad()
{
	class OnModelLoad: public virtual Action
	{

	public:
		void performAction() override {
			modelEditorScreenController->view->loadFile(
				modelEditorScreenController->view->getPopUpsViews()->getFileDialogScreenController()->getPathName(),
				modelEditorScreenController->view->getPopUpsViews()->getFileDialogScreenController()->getFileName()
			);
			modelEditorScreenController->modelPath->setPath(
				modelEditorScreenController->view->getPopUpsViews()->getFileDialogScreenController()->getPathName()
			);
			modelEditorScreenController->view->getPopUpsViews()->getFileDialogScreenController()->close();
		}

		/**
		 * Public constructor
		 * @param modelEditorScreenController model editor screen controller
		 */
		OnModelLoad(ModelEditorScreenController* modelEditorScreenController): modelEditorScreenController(modelEditorScreenController) {
		}

	private:
		ModelEditorScreenController *modelEditorScreenController;
	};

	auto fileName = view->getEntity() != nullptr?view->getEntity()->getEntityFileName():"";
	if (fileName.length() == 0) {
		fileName = view->getFileName();
	}
	fileName = Tools::getFileName(fileName);
	vector<string> extensions = ModelReader::getModelExtensions();
	extensions.push_back("tmm");
	view->getPopUpsViews()->getFileDialogScreenController()->show(
		modelPath->getPath(),
		"Load from: ",
		extensions,
		view->getFileName(),
		true,
		new OnModelLoad(this)
	);
}

void ModelEditorScreenController::onModelSave()
{
	class OnModelSave: public virtual Action
	{
	public:
		void performAction() override {
			try {
				modelEditorScreenController->view->saveFile(
					modelEditorScreenController->view->getPopUpsViews()->getFileDialogScreenController()->getPathName(),
					modelEditorScreenController->view->getPopUpsViews()->getFileDialogScreenController()->getFileName()
				);
				modelEditorScreenController->modelPath->setPath(
					modelEditorScreenController->view->getPopUpsViews()->getFileDialogScreenController()->getPathName()
				);
				modelEditorScreenController->view->getPopUpsViews()->getFileDialogScreenController()->close();
			} catch (Exception& exception) {
				modelEditorScreenController->showErrorPopUp("Warning", (string(exception.what())));
			}
		}

		/**
		 * Public constructor
		 * @param modelEditorScreenController model editor screen controller
		 */
		OnModelSave(ModelEditorScreenController* modelEditorScreenController): modelEditorScreenController(modelEditorScreenController) {
		}

	private:
		ModelEditorScreenController* modelEditorScreenController;
	};

	auto fileName = view->getEntity() != nullptr?view->getEntity()->getEntityFileName():"";
	if (fileName.length() == 0) {
		fileName = view->getFileName();
		if (StringUtils::endsWith(StringUtils::toLowerCase(fileName), ".tmm") == false) {
			fileName = Tools::removeFileEnding(fileName) + ".tmm";
		}
	}
	vector<string> extensions = {
		"tmm"
	};
	fileName = Tools::getFileName(fileName);
	view->getPopUpsViews()->getFileDialogScreenController()->show(
		modelPath->getPath(),
		"Save to: ",
		extensions,
		fileName,
		false,
		new OnModelSave(this)
	);
}

void ModelEditorScreenController::onModelReload()
{
	view->reloadFile();
}

void ModelEditorScreenController::onModelReimport()
{
	class OnModelLoad: public virtual Action
	{

	public:
		void performAction() override {
			modelEditorScreenController->view->reimportModel(
				modelEditorScreenController->view->getPopUpsViews()->getFileDialogScreenController()->getPathName(),
				modelEditorScreenController->view->getPopUpsViews()->getFileDialogScreenController()->getFileName()
			);
			modelEditorScreenController->modelPath->setPath(
				modelEditorScreenController->view->getPopUpsViews()->getFileDialogScreenController()->getPathName()
			);
			modelEditorScreenController->view->getPopUpsViews()->getFileDialogScreenController()->close();
		}

		/**
		 * Public constructor
		 * @param modelEditorScreenController model editor screen controller
		 */
		OnModelLoad(ModelEditorScreenController* modelEditorScreenController): modelEditorScreenController(modelEditorScreenController) {
		}

	private:
		ModelEditorScreenController *modelEditorScreenController;
	};

	vector<string> extensions = ModelReader::getModelExtensions();
	view->getPopUpsViews()->getFileDialogScreenController()->show(
		modelPath->getPath(),
		"Reimport model: ",
		ModelReader::getModelExtensions(),
		view->getFileName(),
		true,
		new OnModelLoad(this)
	);
}

void ModelEditorScreenController::onPivotApply()
{
	try {
		auto x = Float::parseFloat(pivotX->getController()->getValue().getString());
		auto y = Float::parseFloat(pivotY->getController()->getValue().getString());
		auto z = Float::parseFloat(pivotZ->getController()->getValue().getString());
		view->pivotApply(x, y, z);
	} catch (Exception& exception) {
		showErrorPopUp("Warning", (string(exception.what())));
	}
}

void ModelEditorScreenController::onRenderingApply()
{
	if (view->getEntity() == nullptr) return;
	try {
		view->getEntity()->setContributesShadows(renderingContributesShadows->getController()->getValue().equals("1"));
		view->getEntity()->setReceivesShadows(renderingReceivesShadows->getController()->getValue().equals("1"));
		view->getEntity()->setRenderGroups(renderingRenderGroups->getController()->getValue().equals("1"));
		view->getEntity()->setShader(renderingShader->getController()->getValue().getString());
		view->getEntity()->setDistanceShader(renderingDistanceShader->getController()->getValue().getString());
		view->getEntity()->setDistanceShaderDistance(Float::parseFloat(renderingDistanceShaderDistance->getController()->getValue().getString()));
	} catch (Exception& exception) {
		showErrorPopUp("Warning", (string(exception.what())));
	}
	view->updateRendering();
}

void ModelEditorScreenController::onMaterialPBREnabledValueChanged() {
	auto material = getSelectedMaterial();
	if (material == nullptr) return;
	auto pbrMaterialProperties = material->getPBRMaterialProperties();
	auto disabled = materialsMaterialPBREnabled->getController()->getValue().getString() != "1";
	materialsMaterialPBRBaseColorFactor->getController()->setDisabled(disabled);
	materialsMaterialPBRBaseColorTexture->getController()->setDisabled(disabled);
	materialsMaterialPBRBaseColorTextureLoad->getController()->setDisabled(disabled);
	materialsMaterialPBRBaseColorTextureClear->getController()->setDisabled(disabled);
	materialsMaterialPBRMetallicFactor->getController()->setDisabled(disabled);
	materialsMaterialPBRRoughnessFactor->getController()->setDisabled(disabled);
	materialsMaterialPBRMetallicRoughnessTexture->getController()->setDisabled(disabled);
	materialsMaterialPBRMetallicRoughnessTextureLoad->getController()->setDisabled(disabled);
	materialsMaterialPBRMetallicRoughnessTextureClear->getController()->setDisabled(disabled);
	materialsMaterialPBRNormalScale->getController()->setDisabled(disabled);
	materialsMaterialPBRNormalTexture->getController()->setDisabled(disabled);
	materialsMaterialPBRNormalTextureLoad->getController()->setDisabled(disabled);
	materialsMaterialPBRNormalTextureClear->getController()->setDisabled(disabled);
	materialsMaterialPBRExposure->getController()->setDisabled(disabled);
	if (disabled == false && pbrMaterialProperties == nullptr) {
		materialsMaterialPBRBaseColorFactor->getController()->setValue(MutableString(Tools::formatColor4(Color4(1.0f, 1.0f, 1.0f, 1.0f))));
		materialsMaterialPBRBaseColorTexture->getController()->setValue(MutableString());
		materialsMaterialPBRMetallicFactor->getController()->setValue(MutableString(Tools::formatFloat(1.0f)));
		materialsMaterialPBRRoughnessFactor->getController()->setValue(MutableString(Tools::formatFloat(1.0f)));
		materialsMaterialPBRMetallicRoughnessTexture->getController()->setValue(MutableString());
		materialsMaterialPBRNormalScale->getController()->setValue(MutableString(Tools::formatFloat(1.0f)));
		materialsMaterialPBRNormalTexture->getController()->setValue(MutableString());
		materialsMaterialPBRExposure->getController()->setValue(MutableString(Tools::formatFloat(1.0f)));
	}
}

void ModelEditorScreenController::saveFile(const string& pathName, const string& fileName) /* throws(Exception) */
{
	view->saveFile(pathName, fileName);
}

void ModelEditorScreenController::loadFile(const string& pathName, const string& fileName) /* throws(Exception) */
{
	view->loadFile(pathName, fileName);
}

void ModelEditorScreenController::showErrorPopUp(const string& caption, const string& message)
{
	view->getPopUpsViews()->getInfoDialogScreenController()->show(caption, message);
}

void ModelEditorScreenController::onValueChanged(GUIElementNode* node)
{
	if (node->getId() == "animations_dropdown") {
		onAnimationDropDownValueChanged();
	} else
	if (node->getId() == "materials_material_pbr_enabled") {
		onMaterialPBREnabledValueChanged();
	} else {
		entityBaseSubScreenController->onValueChanged(node, view->getEntity());
		entityPhysicsSubScreenController->onValueChanged(node, view->getEntity());
		entitySoundsSubScreenController->onValueChanged(node, view->getEntity());
	}
}

void ModelEditorScreenController::onActionPerformed(GUIActionListener_Type* type, GUIElementNode* node)
{
	entityBaseSubScreenController->onActionPerformed(type, node, view->getEntity());
	entityDisplaySubScreenController->onActionPerformed(type, node);
	entityPhysicsSubScreenController->onActionPerformed(type, node, view->getEntity());
	entitySoundsSubScreenController->onActionPerformed(type, node, view->getEntity());
	{
		auto v = type;
		if (v == GUIActionListener_Type::PERFORMED) {
			if (node->getId().compare("button_model_load") == 0) {
				onModelLoad();
			} else
			if (node->getId().compare("button_model_reload") == 0) {
				onModelReload();
			} else
			if (node->getId().compare("button_model_reimport") == 0) {
				onModelReimport();
			} else
			if (node->getId().compare("button_model_save") == 0) {
				onModelSave();
			} else
			if (node->getId().compare("button_pivot_apply") == 0) {
				onPivotApply();
			} else
			if (node->getId().compare("button_rendering_apply") == 0) {
				onRenderingApply();
			} else
			if (node->getId().compare("lod_level_apply") == 0) {
				onLODLevelApply();
			} else
			if (node->getId().compare("lod_model_file_load") == 0) {
				onLODLevelLoadModel();
			} else
			if (node->getId().compare("lod_model_file_clear") == 0) {
				onLODLevelClearModel();
			} else
			if (node->getId().compare("button_lod_apply") == 0) {
				onLODLevelApplySettings();
			} else
			if (node->getId().compare("button_materials_dropdown_apply") == 0) {
				onMaterialDropDownApply();
			} else
			if (node->getId().compare("button_materials_material_apply") == 0) {
				onMaterialApply();
			} else
			if (node->getId().compare("button_materials_material_diffuse_texture_load") == 0) {
				onMaterialLoadDiffuseTexture();
			} else
			if (node->getId().compare("button_materials_material_diffuse_transparency_texture_load") == 0) {
				onMaterialLoadDiffuseTransparencyTexture();
			} else
			if (node->getId().compare("button_materials_material_normal_texture_load") == 0) {
				onMaterialLoadNormalTexture();
			} else
			if (node->getId().compare("button_materials_material_specular_texture_load") == 0) {
				onMaterialLoadSpecularTexture();
			} else
			if (node->getId().compare("button_materials_material_diffuse_texture_clear") == 0) {
				onMaterialClearTexture(materialsMaterialDiffuseTexture);
			} else
			if (node->getId().compare("button_materials_material_diffuse_transparency_texture_clear") == 0) {
				onMaterialClearTexture(materialsMaterialDiffuseTransparencyTexture);
			} else
			if (node->getId().compare("button_materials_material_normal_texture_clear") == 0) {
				onMaterialClearTexture(materialsMaterialNormalTexture);
			} else
			if (node->getId().compare("button_materials_material_specular_texture_clear") == 0) {
				onMaterialClearTexture(materialsMaterialSpecularTexture);
			} else
			if (node->getId().compare("button_materials_material_pbr_base_color_texture_load") == 0) {
				onMaterialLoadPBRBaseColorTexture();
			} else
			if (node->getId().compare("button_materials_material_pbr_base_color_texture_clear") == 0) {
				onMaterialClearTexture(materialsMaterialPBRBaseColorTexture);
			} else
			if (node->getId().compare("button_materials_material_pbr_metallic_roughness_texture_load") == 0) {
				onMaterialLoadPBRMetallicRoughnessTexture();
			} else
			if (node->getId().compare("button_materials_material_pbr_metallic_roughness_texture_clear") == 0) {
				onMaterialClearTexture(materialsMaterialPBRMetallicRoughnessTexture);
			} else
			if (node->getId().compare("button_materials_material_pbr_normal_texture_load") == 0) {
				onMaterialLoadPBRNormalTexture();
			} else
			if (node->getId().compare("button_materials_material_pbr_normal_texture_clear") == 0) {
				onMaterialClearTexture(materialsMaterialPBRNormalTexture);
			} else
			if (node->getId().compare("animations_dropdown_apply") == 0) {
				onAnimationDropDownApply();
			} else
			if (node->getId().compare("animations_dropdown_delete") == 0) {
				onAnimationDropDownDelete();
			} else
			if (node->getId().compare("button_animations_animation_apply") == 0) {
				onAnimationApply();
			} else
			if (node->getId().compare("button_tools_computenormals") == 0) {
				onToolsComputeNormal();
			}
		}
	}

}

void ModelEditorScreenController::getViewPort(int& left, int& top, int& width, int& height) {
	auto& constraints = viewPort->getComputedConstraints();
	left = constraints.left + constraints.alignmentLeft + constraints.contentAlignmentLeft;
	top = constraints.top + constraints.alignmentTop + constraints.contentAlignmentTop;
	width = constraints.width;
	height = constraints.height;
}