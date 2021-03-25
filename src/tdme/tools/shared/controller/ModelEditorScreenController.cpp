#include <tdme/tools/shared/controller/ModelEditorScreenController.h>

#include <string>
#include <vector>

#include <tdme/engine/fileio/models/ModelReader.h>
#include <tdme/engine/fileio/textures/TextureReader.h>
#include <tdme/engine/model/AnimationSetup.h>
#include <tdme/engine/model/Material.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/model/Node.h>
#include <tdme/engine/model/PBRMaterialProperties.h>
#include <tdme/engine/model/SpecularMaterialProperties.h>
#include <tdme/engine/prototype/Prototype.h>
#include <tdme/engine/prototype/PrototypeAudio.h>
#include <tdme/engine/prototype/PrototypeLODLevel.h>
#include <tdme/gui/events/Action.h>
#include <tdme/gui/events/GUIActionListener.h>
#include <tdme/gui/events/GUIChangeListener.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUINodeController.h>
#include <tdme/gui/nodes/GUIParentNode.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/gui/nodes/GUITextNode.h>
#include <tdme/gui/GUIParser.h>
#include <tdme/math/Vector3.h>
#include <tdme/tools/modeleditor/TDMEModelEditor.h>
#include <tdme/tools/shared/controller/FileDialogPath.h>
#include <tdme/tools/shared/controller/FileDialogScreenController.h>
#include <tdme/tools/shared/controller/InfoDialogScreenController.h>
#include <tdme/tools/shared/controller/PrototypeBaseSubScreenController.h>
#include <tdme/tools/shared/controller/PrototypeDisplaySubScreenController.h>
#include <tdme/tools/shared/controller/PrototypePhysicsSubScreenController.h>
#include <tdme/tools/shared/controller/PrototypeSoundsSubScreenController.h>
#include <tdme/tools/shared/tools/Tools.h>
#include <tdme/tools/shared/views/PopUps.h>
#include <tdme/tools/shared/views/SharedModelEditorView.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>
#include <tdme/utilities/ExceptionBase.h>
#include <tdme/utilities/Float.h>
#include <tdme/utilities/Integer.h>
#include <tdme/utilities/MutableString.h>
#include <tdme/utilities/StringTools.h>

using std::string;
using std::vector;

using tdme::tools::shared::controller::ModelEditorScreenController;

using tdme::engine::fileio::models::ModelReader;
using tdme::engine::fileio::textures::TextureReader;
using tdme::engine::model::AnimationSetup;
using tdme::engine::model::Material;
using tdme::engine::model::Model;
using tdme::engine::model::Node;
using tdme::engine::model::PBRMaterialProperties;
using tdme::engine::model::SpecularMaterialProperties;
using tdme::engine::prototype::Prototype;
using tdme::engine::prototype::PrototypeAudio;
using tdme::engine::prototype::PrototypeLODLevel;
using tdme::gui::events::Action;
using tdme::gui::events::GUIActionListenerType;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeController;
using tdme::gui::nodes::GUIParentNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::nodes::GUITextNode;
using tdme::gui::GUIParser;
using tdme::math::Vector3;
using tdme::tools::modeleditor::TDMEModelEditor;
using tdme::tools::shared::controller::FileDialogPath;
using tdme::tools::shared::controller::FileDialogScreenController;
using tdme::tools::shared::controller::InfoDialogScreenController;
using tdme::tools::shared::controller::PrototypeBaseSubScreenController;
using tdme::tools::shared::controller::PrototypeDisplaySubScreenController;
using tdme::tools::shared::controller::PrototypePhysicsSubScreenController;
using tdme::tools::shared::controller::PrototypeSoundsSubScreenController;
using tdme::tools::shared::tools::Tools;
using tdme::tools::shared::views::PopUps;
using tdme::tools::shared::views::SharedModelEditorView;
using tdme::utilities::Console;
using tdme::utilities::Exception;
using tdme::utilities::ExceptionBase;
using tdme::utilities::Float;
using tdme::utilities::Integer;
using tdme::utilities::MutableString;
using tdme::utilities::StringTools;

ModelEditorScreenController::ModelEditorScreenController(SharedModelEditorView* view)
{
	class OnSetPrototypeDataAction: public virtual Action
	{
	public:
		void performAction() override {
			finalView->updateGUIElements();
			finalView->onSetPrototypeData();
		}

		/**
		 * Public constructor
		 * @param modelEditorScreenController model editor screen controller
		 * @param finalView final view
		 */
		OnSetPrototypeDataAction(ModelEditorScreenController* modelEditorScreenController, SharedModelEditorView* finalView)
			: modelEditorScreenController(modelEditorScreenController)
			, finalView(finalView) {
		}


	private:
		ModelEditorScreenController* modelEditorScreenController;
		SharedModelEditorView* finalView;
	};

	this->view = view;
	auto const finalView = view;
	this->prototypeBaseSubScreenController = new PrototypeBaseSubScreenController(view->getPopUps(), new OnSetPrototypeDataAction(this, finalView));
	this->prototypePhysicsSubScreenController = new PrototypePhysicsSubScreenController(view->getPopUps(), &modelPath, true);
	this->prototypeSoundsSubScreenController = new PrototypeSoundsSubScreenController(view, view->getPopUps(), &audioPath);
	this->prototypeDisplaySubScreenController = new PrototypeDisplaySubScreenController(this->prototypePhysicsSubScreenController->getView());
}

ModelEditorScreenController::~ModelEditorScreenController() {
	delete prototypeBaseSubScreenController;
	delete prototypeDisplaySubScreenController;
	delete prototypePhysicsSubScreenController;
}

SharedModelEditorView* ModelEditorScreenController::getView() {
	return view;
}

PrototypeDisplaySubScreenController* ModelEditorScreenController::getPrototypeDisplaySubScreenController()
{
	return prototypeDisplaySubScreenController;
}

PrototypePhysicsSubScreenController* ModelEditorScreenController::getPrototypePhysicsSubScreenController()
{
	return prototypePhysicsSubScreenController;
}

PrototypeSoundsSubScreenController* ModelEditorScreenController::getPrototypeSoundsSubScreenController()
{
	return prototypeSoundsSubScreenController;
}

GUIScreenNode* ModelEditorScreenController::getScreenNode()
{
	return screenNode;
}

FileDialogPath* ModelEditorScreenController::getModelPath()
{
	return &modelPath;
}

FileDialogPath* ModelEditorScreenController::getAudioPath()
{
	return &audioPath;
}

void ModelEditorScreenController::initialize()
{
	try {
		screenNode = GUIParser::parse("resources/engine/gui", "screen_modeleditor.xml");
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
		shaderParametersContent = dynamic_cast< GUIParentNode* >(screenNode->getNodeById("shaderparameters_content"));
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
		materialsMaterialReflection = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("materials_material_reflection"));
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
		animationsAnimationOverlayFromNodeIdDropDown = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("animations_animation_overlayfromnodeidanimations_dropdown"));
		animationsAnimationLoop = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("animations_animation_loop"));
		animationsAnimationSpeed = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("animations_animation_speed"));
		animationsAnimationName = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("animations_animation_name"));
		animationsAnimationApply = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_animations_animation_apply"));
		previewAnimationsBaseDropDown = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("preview_animations_base_dropdown"));
		previewAnimationsOverlay1DropDown = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("preview_animations_overlay1_dropdown"));
		previewAnimationsOverlay2DropDown = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("preview_animations_overlay2_dropdown"));
		previewAnimationsOverlay3DropDown = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("preview_animations_overlay3_dropdown"));
		previewAnimationsAttachment1BoneDropdown = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("preview_animations_attachment1_bone_dropdown"));
		previewAnimationsAttachment1ModelModel = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("preview_animations_attachment1_model_model"));
		previewAnimationsAttachment1ModelLoad = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("preview_animations_attachment1_model_load"));
		previewAnimationsAttachment1ModelClear = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("preview_animations_attachment1_model_clear"));
		buttonPreviewApply = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_preview_apply"));
		statsOpaqueFaces = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("stats_opaque_faces"));
		buttonToolsComputeNormals = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_tools_computenormals"));
		buttonToolsOptimizeModel = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_tools_optimizemodel"));
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
	prototypeBaseSubScreenController->initialize(screenNode);
	prototypeDisplaySubScreenController->initialize(screenNode);
	prototypePhysicsSubScreenController->initialize(screenNode);
	prototypeSoundsSubScreenController->initialize(screenNode);
}

void ModelEditorScreenController::dispose()
{
}

void ModelEditorScreenController::setScreenCaption(const string& text)
{
	screenCaption->setText(text);
}

void ModelEditorScreenController::setPrototypeData(const string& name, const string& description)
{
	prototypeBaseSubScreenController->setPrototypeData(name, description);
	modelReload->getController()->setDisabled(false);
	modelReimport->getController()->setDisabled(false);
	modelSave->getController()->setDisabled(false);
}

void ModelEditorScreenController::unsetPrototypeData()
{
	prototypeBaseSubScreenController->unsetPrototypeData();
	modelReload->getController()->setDisabled(true);
	modelReimport->getController()->setDisabled(true);
	modelSave->getController()->setDisabled(true);
}

void ModelEditorScreenController::setPrototypeProperties(const string& presetId, const string& selectedName)
{
	auto prototype = view->getPrototype();
	if (prototype == nullptr) return;
	prototypeBaseSubScreenController->setPrototypeProperties(prototype, presetId, selectedName);
}

void ModelEditorScreenController::unsetPrototypeProperties()
{
	prototypeBaseSubScreenController->unsetPrototypeProperties();
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
			"<scrollarea id=\"" +
				renderingShader->getId() +
				"_inner_scrollarea\" width=\"100%\" height=\"150\">\n" +
				renderingShadersDropDownSubNodesXML +
				"</scrollarea>\n",
			false
		);
		dynamic_cast< GUIParentNode* >((renderingShader->getScreenNode()->getNodeById(renderingDistanceShader->getId() + "_inner")))->replaceSubNodes(
			"<scrollarea id=\"" +
				renderingDistanceShader->getId() +
				"_inner_scrollarea\" width=\"100%\" height=\"150\">\n" +
				renderingShadersDropDownSubNodesXML +
				"</scrollarea>\n",
			false
		);
	} catch (Exception& exception) {
		Console::print(string("ModelEditorScreenController::setRenderingShaders(): An error occurred: "));
		Console::println(string(exception.what()));
	}
}

void ModelEditorScreenController::setRendering()
{
	auto prototype = view->getPrototype();
	if (prototype == nullptr) return;
	renderingContributesShadows->getController()->setDisabled(false);
	renderingContributesShadows->getController()->setValue(MutableString(prototype->isContributesShadows() == true?"1":""));
	renderingReceivesShadows->getController()->setDisabled(false);
	renderingReceivesShadows->getController()->setValue(MutableString(prototype->isReceivesShadows() == true?"1":""));
	renderingRenderGroups->getController()->setDisabled(false);
	renderingRenderGroups->getController()->setValue(MutableString(prototype->isRenderGroups() == true?"1":""));
	renderingShader->getController()->setDisabled(false);
	renderingShader->getController()->setValue(MutableString(prototype->getShader()));
	renderingDistanceShader->getController()->setDisabled(false);
	renderingDistanceShader->getController()->setValue(MutableString(prototype->getDistanceShader()));
	renderingDistanceShaderDistance->getController()->setDisabled(false);
	renderingDistanceShaderDistance->getController()->setValue(MutableString(prototype->getDistanceShaderDistance(), 4));
	renderingApply->getController()->setDisabled(false);

	// shader parameters
	setShaderParameters();
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

void ModelEditorScreenController::setShaderParameters() {
	unsetShaderParameters();

	auto prototype = view->getPrototype();
	if (prototype == nullptr) return;

	auto shaderParameters = Engine::getShaderParameterDefaults(view->getPrototype()->getShader());
	auto distanceShaderParameters = Engine::getShaderParameterDefaults(view->getPrototype()->getDistanceShader());
	string shadersParametersContentSubNodesXML = "";
	if (shaderParameters.empty() == false) {
		shadersParametersContentSubNodesXML+= string("<space height=\"20\" />");
		shadersParametersContentSubNodesXML+= string("<text font=\"{$font.default}\" text=\"'" + prototype->getShader() + "' - Shader\" width=\"auto\" height=\"auto\"/>\n");
		shadersParametersContentSubNodesXML+= string("<space height=\"10\" />");
		for (auto& parameterIt: shaderParameters) {
			auto& parameterName = parameterIt.first;
			auto parameterValue = prototype->getShaderParameters().getShaderParameter(parameterName).toString();
			shadersParametersContentSubNodesXML+=
				string("<layout alignment=\"horizontal\" width=\"auto\" height=\"auto\">\n") +
				string("	<layout width=\"auto\" height=\"auto\" alignment=\"horizontal\" vertical-align=\"center\">\n") +
				string("		<text font=\"{$font.default}\" text=\"'" + GUIParser::escapeQuotes(parameterName) + "'\" width=\"145\" height=\"auto\"/>\n") +
				string("		<space width=\"5\" />\n") +
				string("		<input id=\"shaderparameters_" + GUIParser::escapeQuotes(parameterName) + "\" name=\"shaderparameters_" + GUIParser::escapeQuotes(parameterName) + "\" width=\"450\" height=\"auto\" text=\"" + GUIParser::escapeQuotes(parameterValue) + "\" disabled=\"false\" />\n") +
				string("	</layout>\n") +
				string("</layout>\n");
		}
	}

	if (distanceShaderParameters.empty() == false) {
		shadersParametersContentSubNodesXML+= string("<space height=\"20\" />");
		shadersParametersContentSubNodesXML+= string("<text font=\"{$font.default}\" text=\"'" + prototype->getDistanceShader() + "' - Distance Shader\" width=\"auto\" height=\"auto\"/>\n");
		shadersParametersContentSubNodesXML+= string("<space height=\"10\" />");
		for (auto& parameterIt: distanceShaderParameters) {
			auto& parameterName = parameterIt.first;
			auto parameterValue = prototype->getDistanceShaderParameters().getShaderParameter(parameterName).toString();
			shadersParametersContentSubNodesXML+=
				string("<layout alignment=\"horizontal\" width=\"auto\" height=\"auto\">\n") +
				string("	<layout width=\"auto\" height=\"auto\" alignment=\"horizontal\" vertical-align=\"center\">\n") +
				string("		<text font=\"{$font.default}\" text=\"'" + GUIParser::escapeQuotes(parameterName) + "'\" width=\"145\" height=\"auto\"/>\n") +
				string("		<space width=\"5\" />\n") +
				string("		<input id=\"shaderparameters_distance_" + GUIParser::escapeQuotes(parameterName) + "\" name=\"shaderparameters_distance_" + GUIParser::escapeQuotes(parameterName) + "\" width=\"450\" height=\"auto\" text=\"" + GUIParser::escapeQuotes(parameterValue) + "\" disabled=\"false\" />\n") +
				string("	</layout>\n") +
				string("</layout>\n");
		}
	}

	if (shaderParameters.empty() == false || distanceShaderParameters.empty() == false) {
		shadersParametersContentSubNodesXML+=
			string("<space height=\"20\" />\n") +
			string("<layout width=\"100%\" height=\"auto\" alignment=\"none\" horizontal-align=\"center\">\n") +
			string("	<button id=\"button_shaderparameters_apply\" name=\"button_shaderparameters_apply\" text=\"Apply\" width=\"auto\" height=\"auto\" />\n") +
			string("</layout>\n") +
			string("<space height=\"20\" />\n");
	}

	if (shadersParametersContentSubNodesXML.empty() == true) {
		shadersParametersContentSubNodesXML+=
			string("<space height=\"20\" />\n") +
			string("<text font=\"{$font.default}\" text=\"No shader parameters available\" width=\"auto\" height=\"auto\"/>\n") +
			string("<space height=\"20\" />\n");
	}

	try {
		shaderParametersContent->replaceSubNodes(
			shadersParametersContentSubNodesXML,
			false
		);
	} catch (Exception& exception) {
		Console::print(string("ModelEditorScreenController::setRenderingShaders(): An error occurred: "));
		Console::println(string(exception.what()));
	}
}

void ModelEditorScreenController::unsetShaderParameters() {
	shaderParametersContent->clearSubNodes();
}

PrototypeLODLevel* ModelEditorScreenController::getLODLevel(int level) {
	auto prototype = view->getPrototype();
	if (prototype == nullptr) return nullptr;
	switch(level) {
		case 2:
			{
				auto prototypeLodLevel = prototype->getLODLevel2();
				if (prototypeLodLevel == nullptr) {
					prototypeLodLevel = new PrototypeLODLevel(
						LODObject3D::LODLEVELTYPE_NONE,
						"",
						nullptr,
						0.0f
					);
					prototype->setLODLevel2(prototypeLodLevel);
				}
				return prototypeLodLevel;
			}
		case 3:
			{
				auto prototypeLodLevel = prototype->getLODLevel3();
				if (prototypeLodLevel == nullptr) {
					prototypeLodLevel = new PrototypeLODLevel(
						LODObject3D::LODLEVELTYPE_NONE,
						"",
						nullptr,
						0.0f
					);
					prototype->setLODLevel3(prototypeLodLevel);
				}
				return prototypeLodLevel;
			}
		default:
			{
				return nullptr;
			}
	}
}

void ModelEditorScreenController::setLODLevel(int level) {
	auto prototype = view->getPrototype();
	if (prototype == nullptr) return;

	auto prototypeLodLevel = getLODLevel(level);
	if (prototypeLodLevel == nullptr) {
		lodLevel->getController()->setValue(MutableString(to_string(level)));
		lodLevel->getController()->setDisabled(false);
		lodLevelApply->getController()->setDisabled(false);
		lodType->getController()->setValue(MutableString("1"));
		lodType->getController()->setDisabled(true);
		lodModelFile->getController()->setValue(MutableString(prototype->getModelFileName()));
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
		lodType->getController()->setValue(MutableString(to_string(prototypeLodLevel->getType())));
		lodType->getController()->setDisabled(false);
		lodModelFile->getController()->setValue(MutableString(prototypeLodLevel->getFileName()));
		lodModelFile->getController()->setDisabled(false);
		lodModelFileLoad->getController()->setDisabled(false);
		lodModelFileClear->getController()->setDisabled(false);
		lodMinDistance->getController()->setValue(MutableString(prototypeLodLevel->getMinDistance()));
		lodMinDistance->getController()->setDisabled(false);
		lodColorMul->getController()->setValue(MutableString(Tools::formatColor4(prototypeLodLevel->getColorMul())));
		lodColorMul->getController()->setDisabled(false);
		lodColorAdd->getController()->setValue(MutableString(Tools::formatColor4(prototypeLodLevel->getColorAdd())));
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
	auto prototype = view->getPrototype();
	auto lodLevelInt = Tools::convertToIntSilent(lodLevel->getController()->getValue().getString());
	setLODLevel(lodLevelInt);
}

void ModelEditorScreenController::onLODLevelLoadModel() {
	class OnLODModelLoad: public virtual Action
	{
	public:
		void performAction() override {
			modelEditorScreenController->lodModelFile->getController()->setValue(
				MutableString(
					modelEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getPathName() +
					"/" +
					modelEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getFileName()
				)
			);
			modelEditorScreenController->modelPath.setPath(
				modelEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getPathName()
			);
			modelEditorScreenController->view->getPopUps()->getFileDialogScreenController()->close();
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
	auto prototypeLodLevel = getLODLevel(lodLevelInt);
	if (prototypeLodLevel == nullptr) return;
	auto extensions = ModelReader::getModelExtensions();
	view->getPopUps()->getFileDialogScreenController()->show(
		prototypeLodLevel->getFileName() != ""?Tools::getPathName(prototypeLodLevel->getFileName()):modelPath.getPath(),
		"Load from: ",
		extensions,
		Tools::getFileName(prototypeLodLevel->getFileName()),
		true,
		new OnLODModelLoad(this)
	);
}

void ModelEditorScreenController::onLODLevelClearModel() {
	lodModelFile->getController()->setValue(MutableString());
}

void ModelEditorScreenController::onLODLevelApplySettings() {
	view->resetPrototype();
	auto lodLevelInt = Tools::convertToIntSilent(lodLevel->getController()->getValue().getString());
	PrototypeLODLevel* prototypeLodLevel = getLODLevel(lodLevelInt);
	try {
		prototypeLodLevel->setType(static_cast<LODObject3D::LODLevelType>(Tools::convertToIntSilent(lodType->getController()->getValue().getString())));
		prototypeLodLevel->setFileName(
			prototypeLodLevel->getType() == LODObject3D::LODLEVELTYPE_MODEL?
				lodModelFile->getController()->getValue().getString():
				""
			);
		prototypeLodLevel->setMinDistance(Tools::convertToFloat(lodMinDistance->getController()->getValue().getString()));
		prototypeLodLevel->setColorMul(Tools::convertToColor4(lodColorMul->getController()->getValue().getString()));
		prototypeLodLevel->setColorAdd(Tools::convertToColor4(lodColorAdd->getController()->getValue().getString()));
		prototypeLodLevel->setModel(
			prototypeLodLevel->getType() == LODObject3D::LODLEVELTYPE_MODEL?
				ModelReader::read(
					Tools::getPathName(prototypeLodLevel->getFileName()),
					Tools::getFileName(prototypeLodLevel->getFileName())
				):
				nullptr
		);
	} catch (Exception& exception) {
		showErrorPopUp("Warning", (string(exception.what())));
	}
}

void ModelEditorScreenController::setMaterials() {
	auto prototype = view->getPrototype();
	if (prototype == nullptr) return;

	Model* model = view->getLodLevel() == 1?prototype->getModel():getLODLevel(view->getLodLevel())->getModel();
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
			"<scrollarea id=\"" +
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
		materialsDropDownInnerNodeSubNodesXML = materialsDropDownInnerNodeSubNodesXML + "</scrollarea>";
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
	materialsMaterialReflection->getController()->setDisabled(false);
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
			"<scrollarea id=\"" +
			materialsDropdown->getId() +
			"_inner_scrollarea\" width=\"100%\" height=\"100\">\n";
		materialsDropDownInnerNodeSubNodesXML = materialsDropDownInnerNodeSubNodesXML + "<dropdown-option text=\"None\" value=\"\" />";
		materialsDropDownInnerNodeSubNodesXML = materialsDropDownInnerNodeSubNodesXML + "</scrollarea>";
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
	materialsMaterialReflection->getController()->setDisabled(true);
	materialsMaterialReflection->getController()->setValue(MutableString());
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
	materialsMaterialReflection->getController()->setValue(MutableString(Tools::formatFloat(specularMaterialProperties->getReflection())));
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
	Model* model = view->getLodLevel() == 1?view->getPrototype()->getModel():getLODLevel(view->getLodLevel())->getModel();
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
		view->resetPrototype();
		// specular
		specularMaterialProperties->setAmbientColor(Tools::convertToColor4(materialsMaterialAmbient->getController()->getValue().getString()));
		specularMaterialProperties->setDiffuseColor(Tools::convertToColor4(materialsMaterialDiffuse->getController()->getValue().getString()));
		specularMaterialProperties->setSpecularColor(Tools::convertToColor4(materialsMaterialSpecular->getController()->getValue().getString()));
		specularMaterialProperties->setEmissionColor(Tools::convertToColor4(materialsMaterialEmission->getController()->getValue().getString()));
		specularMaterialProperties->setShininess(Tools::convertToFloat(materialsMaterialShininess->getController()->getValue().getString()));
		specularMaterialProperties->setReflection(Tools::convertToFloat(materialsMaterialReflection->getController()->getValue().getString()));
		specularMaterialProperties->setDiffuseTexture(
			Tools::getPathName(materialsMaterialDiffuseTexture->getController()->getValue().getString()),
			Tools::getFileName(materialsMaterialDiffuseTexture->getController()->getValue().getString()),
			Tools::getPathName(materialsMaterialDiffuseTransparencyTexture->getController()->getValue().getString()),
			Tools::getFileName(materialsMaterialDiffuseTransparencyTexture->getController()->getValue().getString())
		);
		specularMaterialProperties->setNormalTexture(
			Tools::getPathName(materialsMaterialNormalTexture->getController()->getValue().getString()),
			Tools::getFileName(materialsMaterialNormalTexture->getController()->getValue().getString())
		);
		specularMaterialProperties->setSpecularTexture(
			Tools::getPathName(materialsMaterialSpecularTexture->getController()->getValue().getString()),
			Tools::getFileName(materialsMaterialSpecularTexture->getController()->getValue().getString())
		);
		specularMaterialProperties->setDiffuseTextureMaskedTransparency(materialsMaterialUseMaskedTransparency->getController()->getValue().getString() == "1"?true:false);
		specularMaterialProperties->setDiffuseTextureMaskedTransparencyThreshold(Tools::convertToFloat(materialsMaterialMaskedTransparencyThreshold->getController()->getValue().getString()));
		// pbr
		if (pbrMaterialProperties != nullptr) {
			pbrMaterialProperties->setBaseColorFactor(Tools::convertToColor4(materialsMaterialPBRBaseColorFactor->getController()->getValue().getString()));
			pbrMaterialProperties->setBaseColorTexture(
				Tools::getPathName(materialsMaterialPBRBaseColorTexture->getController()->getValue().getString()),
				Tools::getFileName(materialsMaterialPBRBaseColorTexture->getController()->getValue().getString())
			);
			pbrMaterialProperties->setMetallicFactor(Tools::convertToFloat(materialsMaterialPBRMetallicFactor->getController()->getValue().getString()));
			pbrMaterialProperties->setRoughnessFactor(Tools::convertToFloat(materialsMaterialPBRRoughnessFactor->getController()->getValue().getString()));
			pbrMaterialProperties->setMetallicRoughnessTexture(
				Tools::getPathName(materialsMaterialPBRMetallicRoughnessTexture->getController()->getValue().getString()),
				Tools::getFileName(materialsMaterialPBRMetallicRoughnessTexture->getController()->getValue().getString())
			);
			pbrMaterialProperties->setNormalScale(Tools::convertToFloat(materialsMaterialPBRNormalScale->getController()->getValue().getString()));
			pbrMaterialProperties->setNormalTexture(
				Tools::getPathName(materialsMaterialPBRNormalTexture->getController()->getValue().getString()),
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
					set(modelEditorScreenController->getView()->getPopUps()->getFileDialogScreenController()->getPathName()).
					append("/").
					append(modelEditorScreenController->getView()->getPopUps()->getFileDialogScreenController()->getFileName())
			);
			modelEditorScreenController->getModelPath()->setPath(
				modelEditorScreenController->getView()->getPopUps()->getFileDialogScreenController()->getPathName()
			);
			modelEditorScreenController->getView()->getPopUps()->getFileDialogScreenController()->close();
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
	view->getPopUps()->getFileDialogScreenController()->show(
		specularMaterialProperties->getDiffuseTextureFileName() != ""?specularMaterialProperties->getDiffuseTexturePathName():modelPath.getPath(),
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
					set(modelEditorScreenController->getView()->getPopUps()->getFileDialogScreenController()->getPathName()).
					append("/").
					append(modelEditorScreenController->getView()->getPopUps()->getFileDialogScreenController()->getFileName())
			);
			modelEditorScreenController->getModelPath()->setPath(
				modelEditorScreenController->getView()->getPopUps()->getFileDialogScreenController()->getPathName()
			);
			modelEditorScreenController->getView()->getPopUps()->getFileDialogScreenController()->close();
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
	view->getPopUps()->getFileDialogScreenController()->show(
		specularMaterialProperties->getDiffuseTransparencyTextureFileName() != ""?specularMaterialProperties->getDiffuseTransparencyTexturePathName():modelPath.getPath(),
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
					set(modelEditorScreenController->getView()->getPopUps()->getFileDialogScreenController()->getPathName()).
					append("/").
					append(modelEditorScreenController->getView()->getPopUps()->getFileDialogScreenController()->getFileName())
			);
			modelEditorScreenController->getModelPath()->setPath(
				modelEditorScreenController->getView()->getPopUps()->getFileDialogScreenController()->getPathName()
			);
			modelEditorScreenController->getView()->getPopUps()->getFileDialogScreenController()->close();
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
	view->getPopUps()->getFileDialogScreenController()->show(
		specularMaterialProperties->getNormalTextureFileName() != ""?specularMaterialProperties->getNormalTexturePathName():modelPath.getPath(),
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
					set(modelEditorScreenController->getView()->getPopUps()->getFileDialogScreenController()->getPathName()).
					append("/").
					append(modelEditorScreenController->getView()->getPopUps()->getFileDialogScreenController()->getFileName())
			);
			modelEditorScreenController->getModelPath()->setPath(
				modelEditorScreenController->getView()->getPopUps()->getFileDialogScreenController()->getPathName()
			);
			modelEditorScreenController->getView()->getPopUps()->getFileDialogScreenController()->close();
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
	view->getPopUps()->getFileDialogScreenController()->show(
		specularMaterialProperties->getSpecularTextureFileName() != ""?specularMaterialProperties->getSpecularTexturePathName():modelPath.getPath(),
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
					set(modelEditorScreenController->getView()->getPopUps()->getFileDialogScreenController()->getPathName()).
					append("/").
					append(modelEditorScreenController->getView()->getPopUps()->getFileDialogScreenController()->getFileName())
			);
			modelEditorScreenController->getModelPath()->setPath(
				modelEditorScreenController->getView()->getPopUps()->getFileDialogScreenController()->getPathName()
			);
			modelEditorScreenController->getView()->getPopUps()->getFileDialogScreenController()->close();
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
	view->getPopUps()->getFileDialogScreenController()->show(
		pbrMaterialProperties != nullptr && pbrMaterialProperties->getBaseColorTextureFileName() != ""?pbrMaterialProperties->getBaseColorTexturePathName():modelPath.getPath(),
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
					set(modelEditorScreenController->getView()->getPopUps()->getFileDialogScreenController()->getPathName()).
					append("/").
					append(modelEditorScreenController->getView()->getPopUps()->getFileDialogScreenController()->getFileName())
			);
			modelEditorScreenController->getModelPath()->setPath(
				modelEditorScreenController->getView()->getPopUps()->getFileDialogScreenController()->getPathName()
			);
			modelEditorScreenController->getView()->getPopUps()->getFileDialogScreenController()->close();
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
	view->getPopUps()->getFileDialogScreenController()->show(
		pbrMaterialProperties != nullptr && pbrMaterialProperties->getMetallicRoughnessTextureFileName() != ""?pbrMaterialProperties->getMetallicRoughnessTexturePathName():modelPath.getPath(),
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
					set(modelEditorScreenController->getView()->getPopUps()->getFileDialogScreenController()->getPathName()).
					append("/").
					append(modelEditorScreenController->getView()->getPopUps()->getFileDialogScreenController()->getFileName())
			);
			modelEditorScreenController->getModelPath()->setPath(
				modelEditorScreenController->getView()->getPopUps()->getFileDialogScreenController()->getPathName()
			);
			modelEditorScreenController->getView()->getPopUps()->getFileDialogScreenController()->close();
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
	view->getPopUps()->getFileDialogScreenController()->show(
		pbrMaterialProperties != nullptr && pbrMaterialProperties->getNormalTextureFileName() != ""?pbrMaterialProperties->getNormalTexturePathName():modelPath.getPath(),
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

void ModelEditorScreenController::setAnimations() {
	auto prototype = view->getPrototype();
	if (prototype == nullptr) return;

	Model* model = view->getLodLevel() == 1?view->getPrototype()->getModel():getLODLevel(view->getLodLevel())->getModel();
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
			"<scrollarea id=\"" +
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
		animationsDropDownInnerNodeSubNodesXML = animationsDropDownInnerNodeSubNodesXML + "</scrollarea>";
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
		auto animationsAnimationOverlayFromNodeIdDropDownInnerNode = dynamic_cast< GUIParentNode* >((animationsAnimationOverlayFromNodeIdDropDown->getScreenNode()->getNodeById(animationsAnimationOverlayFromNodeIdDropDown->getId() + "_inner")));
		auto idx = 0;
		string animationsAnimationOverlayFromNodeIdDropDownInnerNodeSubNodesXML = "";
		animationsAnimationOverlayFromNodeIdDropDownInnerNodeSubNodesXML =
			animationsAnimationOverlayFromNodeIdDropDownInnerNodeSubNodesXML +
			"<scrollarea id=\"" +
			animationsAnimationOverlayFromNodeIdDropDown->getId() +
			"_inner_scrollarea\" width=\"100%\" height=\"70\">\n";
		animationsAnimationOverlayFromNodeIdDropDownInnerNodeSubNodesXML =
			animationsAnimationOverlayFromNodeIdDropDownInnerNodeSubNodesXML +
			"<dropdown-option text=\"\" value=\"\"" +
			(idx == 0 ? "selected=\"true\" " : "") +
			" />\n";
		for (auto it: model->getNodes()) {
			auto nodeId = it.second->getId();
			animationsAnimationOverlayFromNodeIdDropDownInnerNodeSubNodesXML =
				animationsAnimationOverlayFromNodeIdDropDownInnerNodeSubNodesXML +
				"<dropdown-option text=\"" +
				GUIParser::escapeQuotes(nodeId) +
				"\" value=\"" +
				GUIParser::escapeQuotes(nodeId) +
				"\" " +
				(idx == 0 ? "selected=\"true\" " : "") +
				" />\n";
			idx++;
		}
		animationsAnimationOverlayFromNodeIdDropDownInnerNodeSubNodesXML = animationsAnimationOverlayFromNodeIdDropDownInnerNodeSubNodesXML + "</scrollarea>";
		try {
			animationsAnimationOverlayFromNodeIdDropDownInnerNode->replaceSubNodes(animationsAnimationOverlayFromNodeIdDropDownInnerNodeSubNodesXML, true);
		} catch (Exception& exception) {
			Console::print(string("ModelEditorScreenController::setAnimations(): An error occurred: "));
			Console::println(string(exception.what()));
		}
		// TODO: this usually works most of the time out of the box, so custom layouting is not required, but in this case not, need to find out whats going wrong there
		// animationsAnimationOverlayFromNodeIdDropDown->getScreenNode()->layout(animationsAnimationOverlayFromNodeIdDropDown);
	}

	// select default animation
	animationsDropDown->getController()->setValue(MutableString(Model::ANIMATIONSETUP_DEFAULT));

	// apply
	onAnimationDropDownApply();
}

void ModelEditorScreenController::onAnimationDropDownValueChanged() {
	Model* model = view->getLodLevel() == 1?view->getPrototype()->getModel():getLODLevel(view->getLodLevel())->getModel();
	if (model == nullptr) return;

	auto animationSetup = model->getAnimationSetup(animationsDropDown->getController()->getValue().getString());
	auto defaultAnimation = animationSetup != nullptr && animationSetup->getId() == Model::ANIMATIONSETUP_DEFAULT;
	animationsDropDownDelete->getController()->setDisabled(defaultAnimation || animationSetup == nullptr);
}

void ModelEditorScreenController::onAnimationDropDownApply() {
	Model* model = view->getLodLevel() == 1?view->getPrototype()->getModel():getLODLevel(view->getLodLevel())->getModel();
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
	animationsAnimationOverlayFromNodeIdDropDown->getController()->setValue(MutableString(animationSetup->getOverlayFromNodeId()));
	animationsAnimationOverlayFromNodeIdDropDown->getController()->setDisabled(defaultAnimation);
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
	Model* model = view->getLodLevel() == 1?view->getPrototype()->getModel():getLODLevel(view->getLodLevel())->getModel();
	if (model == nullptr) return;

	auto animationName = animationsDropDown->getController()->getValue().getString();
	auto animationSetup = model->getAnimationSetup(animationName);
	auto it = model->getAnimationSetups().find(animationSetup->getId());
	it = model->getAnimationSetups().erase(it);
	delete animationSetup;

	setAnimations();
	animationsDropDown->getController()->setValue(MutableString(it->second->getId()));
	onAnimationDropDownApply();
}

void ModelEditorScreenController::onAnimationApply() {
	Model* model = view->getLodLevel() == 1?view->getPrototype()->getModel():getLODLevel(view->getLodLevel())->getModel();
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
		animationSetup->setOverlayFromNodeId(animationsAnimationOverlayFromNodeIdDropDown->getController()->getValue().getString());
		animationSetup->setLoop(animationsAnimationLoop->getController()->getValue().getString() == "1");
		animationSetup->setSpeed(Float::parseFloat(animationsAnimationSpeed->getController()->getValue().getString()));
		setAnimations();
		animationsDropDown->getController()->setValue(MutableString(animationSetup->getId()));
		onAnimationDropDownApply();
		setPreview();
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
	animationsAnimationOverlayFromNodeIdDropDown->getController()->setValue(MutableString(""));
	animationsAnimationOverlayFromNodeIdDropDown->getController()->setDisabled(true);
	animationsAnimationLoop->getController()->setValue(MutableString(""));
	animationsAnimationLoop->getController()->setDisabled(true);
	animationsAnimationSpeed->getController()->setValue(MutableString(""));
	animationsAnimationSpeed->getController()->setDisabled(true);
	animationsAnimationName->getController()->setValue(MutableString(""));
	animationsAnimationName->getController()->setDisabled(true);
	animationsAnimationApply->getController()->setDisabled(true);
}

void ModelEditorScreenController::setPreview() {
	previewAnimationsBaseDropDown->getController()->setDisabled(false);
	previewAnimationsOverlay1DropDown->getController()->setDisabled(false);
	previewAnimationsOverlay2DropDown->getController()->setDisabled(false);
	previewAnimationsOverlay3DropDown->getController()->setDisabled(false);
	previewAnimationsAttachment1BoneDropdown->getController()->setDisabled(false);
	previewAnimationsAttachment1ModelModel->getController()->setDisabled(false);
	previewAnimationsAttachment1ModelLoad->getController()->setDisabled(false);
	previewAnimationsAttachment1ModelClear->getController()->setDisabled(false);
	buttonPreviewApply->getController()->setDisabled(false);

	Model* model = view->getLodLevel() == 1?view->getPrototype()->getModel():getLODLevel(view->getLodLevel())->getModel();
	if (model == nullptr) {
		unsetPreview();
		return;
	}

	{
		auto animationsDropDownInnerNode = dynamic_cast< GUIParentNode* >((previewAnimationsBaseDropDown->getScreenNode()->getNodeById(previewAnimationsBaseDropDown->getId() + "_inner")));
		string animationsDropDownInnerNodeSubNodesXML = "";
		animationsDropDownInnerNodeSubNodesXML =
			animationsDropDownInnerNodeSubNodesXML +
			"<scrollarea id=\"" +
			previewAnimationsBaseDropDown->getId() +
			"_inner_scrollarea\" width=\"100%\" height=\"100\">\n";
		animationsDropDownInnerNodeSubNodesXML = animationsDropDownInnerNodeSubNodesXML + "<dropdown-option text=\"<No animation>\" value=\"\" selected=\"true\" />";
		for (auto it: model->getAnimationSetups()) {
			auto animationSetup = it.second;
			if (animationSetup->isOverlayAnimationSetup() == true) continue;
			animationsDropDownInnerNodeSubNodesXML =
				animationsDropDownInnerNodeSubNodesXML + "<dropdown-option text=\"" +
				GUIParser::escapeQuotes(animationSetup->getId()) +
				"\" value=\"" +
				GUIParser::escapeQuotes(animationSetup->getId()) +
				"\" " +
				" />\n";
		}
		animationsDropDownInnerNodeSubNodesXML = animationsDropDownInnerNodeSubNodesXML + "</scrollarea>";
		try {
			animationsDropDownInnerNode->replaceSubNodes(animationsDropDownInnerNodeSubNodesXML, true);
		} catch (Exception& exception) {
			Console::print(string("ModelEditorScreenController::setPreview(): An error occurred: "));
			Console::println(string(exception.what()));
		}
	}

	{
		auto animationsDropDownInnerNode1 = dynamic_cast< GUIParentNode* >((previewAnimationsOverlay1DropDown->getScreenNode()->getNodeById(previewAnimationsOverlay1DropDown->getId() + "_inner")));
		auto animationsDropDownInnerNode2 = dynamic_cast< GUIParentNode* >((previewAnimationsOverlay2DropDown->getScreenNode()->getNodeById(previewAnimationsOverlay2DropDown->getId() + "_inner")));
		auto animationsDropDownInnerNode3 = dynamic_cast< GUIParentNode* >((previewAnimationsOverlay3DropDown->getScreenNode()->getNodeById(previewAnimationsOverlay3DropDown->getId() + "_inner")));
		string animationsDropDownInnerNodeSubNodesXML = "";
		animationsDropDownInnerNodeSubNodesXML = animationsDropDownInnerNodeSubNodesXML + "<dropdown-option text=\"<No animation>\" value=\"\" selected=\"true\" />";
		for (auto it: model->getAnimationSetups()) {
			auto animationSetup = it.second;
			if (animationSetup->isOverlayAnimationSetup() == false) continue;
			animationsDropDownInnerNodeSubNodesXML =
				animationsDropDownInnerNodeSubNodesXML + "<dropdown-option text=\"" +
				GUIParser::escapeQuotes(animationSetup->getId()) +
				"\" value=\"" +
				GUIParser::escapeQuotes(animationSetup->getId()) +
				"\" " +
				" />\n";
		}
		try {
			animationsDropDownInnerNode1->replaceSubNodes(
				"<scrollarea id=\"" +
				previewAnimationsOverlay1DropDown->getId() +
				"_inner_scrollarea\" width=\"100%\" height=\"100\">\n" +
				animationsDropDownInnerNodeSubNodesXML +
				"</scrollarea>",
				true
			);
		} catch (Exception& exception) {
			Console::print(string("ModelEditorScreenController::setPreview(): An error occurred: "));
			Console::println(string(exception.what()));
		}
		try {
			animationsDropDownInnerNode2->replaceSubNodes(
				"<scrollarea id=\"" +
				previewAnimationsOverlay2DropDown->getId() +
				"_inner_scrollarea\" width=\"100%\" height=\"100\">\n" +
				animationsDropDownInnerNodeSubNodesXML +
				"</scrollarea>",
				true
			);
		} catch (Exception& exception) {
			Console::print(string("ModelEditorScreenController::setPreview(): An error occurred: "));
			Console::println(string(exception.what()));
		}
		try {
			animationsDropDownInnerNode3->replaceSubNodes(
				"<scrollarea id=\"" +
				previewAnimationsOverlay3DropDown->getId() +
				"_inner_scrollarea\" width=\"100%\" height=\"100\">\n" +
				animationsDropDownInnerNodeSubNodesXML +
				"</scrollarea>",
				true
			);
		} catch (Exception& exception) {
			Console::print(string("ModelEditorScreenController::setPreview(): An error occurred: "));
			Console::println(string(exception.what()));
		}
	}

	{
		auto animationsAttachment1BoneDropDownInnerNode = dynamic_cast< GUIParentNode* >((previewAnimationsAttachment1BoneDropdown->getScreenNode()->getNodeById(previewAnimationsAttachment1BoneDropdown->getId() + "_inner")));
		string animationsAttachment1BoneDropDownInnerNodeSubNodesXML = "";
		animationsAttachment1BoneDropDownInnerNodeSubNodesXML =
			animationsAttachment1BoneDropDownInnerNodeSubNodesXML +
			"<scrollarea id=\"" +
			previewAnimationsAttachment1BoneDropdown->getId() +
			"_inner_scrollarea\" width=\"100%\" height=\"100\">\n";
		animationsAttachment1BoneDropDownInnerNodeSubNodesXML = animationsAttachment1BoneDropDownInnerNodeSubNodesXML + "<dropdown-option text=\"<No bone>\" value=\"\" selected=\"true\" />";
		for (auto it: model->getNodes()) {
			auto node = it.second;
			animationsAttachment1BoneDropDownInnerNodeSubNodesXML =
				animationsAttachment1BoneDropDownInnerNodeSubNodesXML + "<dropdown-option text=\"" +
				GUIParser::escapeQuotes(node->getId()) +
				"\" value=\"" +
				GUIParser::escapeQuotes(node->getId()) +
				"\" " +
				" />\n";
		}
		animationsAttachment1BoneDropDownInnerNodeSubNodesXML = animationsAttachment1BoneDropDownInnerNodeSubNodesXML + "</scrollarea>";
		try {
			animationsAttachment1BoneDropDownInnerNode->replaceSubNodes(animationsAttachment1BoneDropDownInnerNodeSubNodesXML, true);
		} catch (Exception& exception) {
			Console::print(string("ModelEditorScreenController::setPreview(): An error occurred: "));
			Console::println(string(exception.what()));
		}
	}

}

void ModelEditorScreenController::onPreviewAnimationsAttachment1ModelLoad() {
	class OnPreviewAnimationsAttachment1ModelLoad: public virtual Action
	{

	public:
		void performAction() override {
			modelEditorScreenController->previewAnimationsAttachment1ModelModel->getController()->setValue(
				modelEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getPathName() +
				"/" +
				modelEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getFileName()
			);
			modelEditorScreenController->modelPath.setPath(
				modelEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getPathName()
			);
			modelEditorScreenController->view->getPopUps()->getFileDialogScreenController()->close();
		}

		/**
		 * Public constructor
		 * @param modelEditorScreenController model editor screen controller
		 */
		OnPreviewAnimationsAttachment1ModelLoad(ModelEditorScreenController* modelEditorScreenController): modelEditorScreenController(modelEditorScreenController) {
		}

	private:
		ModelEditorScreenController *modelEditorScreenController;
	};

	vector<string> extensions = ModelReader::getModelExtensions();
	view->getPopUps()->getFileDialogScreenController()->show(
		previewAnimationsAttachment1ModelModel->getController()->getValue().getString().empty() == true?modelPath.getPath():Tools::getPathName(previewAnimationsAttachment1ModelModel->getController()->getValue().getString()),
		"Load from: ",
		extensions,
		Tools::getFileName(previewAnimationsAttachment1ModelModel->getController()->getValue().getString()),
		true,
		new OnPreviewAnimationsAttachment1ModelLoad(this)
	);
}

void ModelEditorScreenController::onPreviewAnimationsAttachment1ModelClear() {
	previewAnimationsAttachment1ModelModel->getController()->setValue(MutableString());
}

void ModelEditorScreenController::onPreviewApply() {
	Model* model = view->getLodLevel() == 1?view->getPrototype()->getModel():getLODLevel(view->getLodLevel())->getModel();
	if (model == nullptr) return;

	auto baseAnimationName = previewAnimationsBaseDropDown->getController()->getValue().getString();
	auto overlay1AnimationName = previewAnimationsOverlay1DropDown->getController()->getValue().getString();
	auto overlay2AnimationName = previewAnimationsOverlay2DropDown->getController()->getValue().getString();
	auto overlay3AnimationName = previewAnimationsOverlay3DropDown->getController()->getValue().getString();
	try {
		view->playAnimation(baseAnimationName, overlay1AnimationName, overlay2AnimationName, overlay3AnimationName);
		view->addAttachment1(previewAnimationsAttachment1BoneDropdown->getController()->getValue().getString(), previewAnimationsAttachment1ModelModel->getController()->getValue().getString());
	} catch (Exception& exception) {
		showErrorPopUp("Warning", (string(exception.what())));
	}
}

void ModelEditorScreenController::unsetPreview() {
	previewAnimationsBaseDropDown->getController()->setDisabled(true);
	previewAnimationsOverlay1DropDown->getController()->setDisabled(true);
	previewAnimationsOverlay2DropDown->getController()->setDisabled(true);
	previewAnimationsOverlay3DropDown->getController()->setDisabled(true);
	previewAnimationsAttachment1BoneDropdown->getController()->setDisabled(true);
	previewAnimationsAttachment1BoneDropdown->getController()->setValue(MutableString());
	previewAnimationsAttachment1ModelModel->getController()->setDisabled(true);
	previewAnimationsAttachment1ModelModel->getController()->setValue(MutableString());
	previewAnimationsAttachment1ModelLoad->getController()->setDisabled(true);
	previewAnimationsAttachment1ModelClear->getController()->setDisabled(true);
	buttonPreviewApply->getController()->setDisabled(true);
}


void ModelEditorScreenController::setStatistics(int statsOpaqueFaces, int statsTransparentFaces, int statsMaterialCount)
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
	buttonToolsOptimizeModel->getController()->setDisabled(false);
}

void ModelEditorScreenController::unsetTools() {
	buttonToolsComputeNormals->getController()->setDisabled(true);
	buttonToolsOptimizeModel->getController()->setDisabled(true);
}

void ModelEditorScreenController::onToolsComputeNormal() {
	view->computeNormals();
}

void ModelEditorScreenController::onToolsOptimizeModel() {
	view->optimizeModel();
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
				modelEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getPathName(),
				modelEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getFileName()
			);
			modelEditorScreenController->modelPath.setPath(
				modelEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getPathName()
			);
			modelEditorScreenController->view->getPopUps()->getFileDialogScreenController()->close();
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

	auto fileName = view->getPrototype() != nullptr?view->getPrototype()->getFileName():"";
	if (fileName.length() == 0) {
		fileName = view->getFileName();
	}
	fileName = Tools::getFileName(fileName);
	vector<string> extensions = ModelReader::getModelExtensions();
	extensions.push_back("tmm");
	view->getPopUps()->getFileDialogScreenController()->show(
		modelPath.getPath(),
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
					modelEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getPathName(),
					modelEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getFileName()
				);
				modelEditorScreenController->modelPath.setPath(
					modelEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getPathName()
				);
				modelEditorScreenController->view->getPopUps()->getFileDialogScreenController()->close();
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

	auto fileName = view->getPrototype() != nullptr?view->getPrototype()->getFileName():"";
	if (fileName.length() == 0) {
		fileName = view->getFileName();
		if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".tmm") == false) {
			fileName = Tools::removeFileEnding(fileName) + ".tmm";
		}
	}
	vector<string> extensions = {
		"tmm"
	};
	fileName = Tools::getFileName(fileName);
	view->getPopUps()->getFileDialogScreenController()->show(
		modelPath.getPath(),
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
				modelEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getPathName(),
				modelEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getFileName()
			);
			modelEditorScreenController->modelPath.setPath(
				modelEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getPathName()
			);
			modelEditorScreenController->view->getPopUps()->getFileDialogScreenController()->close();
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
	view->getPopUps()->getFileDialogScreenController()->show(
		modelPath.getPath(),
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
	if (view->getPrototype() == nullptr) return;
	try {
		view->getPrototype()->setContributesShadows(renderingContributesShadows->getController()->getValue().equals("1"));
		view->getPrototype()->setReceivesShadows(renderingReceivesShadows->getController()->getValue().equals("1"));
		view->getPrototype()->setRenderGroups(renderingRenderGroups->getController()->getValue().equals("1"));
		view->getPrototype()->setShader(renderingShader->getController()->getValue().getString());
		view->getPrototype()->setDistanceShader(renderingDistanceShader->getController()->getValue().getString());
		view->getPrototype()->setDistanceShaderDistance(Float::parseFloat(renderingDistanceShaderDistance->getController()->getValue().getString()));
	} catch (Exception& exception) {
		showErrorPopUp("Warning", (string(exception.what())));
	}
	view->updateRendering();

	// shader parameters
	setShaderParameters();
}

void ModelEditorScreenController::onShaderParametersApply() {
	auto prototype = view->getPrototype();
	if (prototype == nullptr) return;

	auto shaderParametersDefault = Engine::getShaderParameterDefaults(view->getPrototype()->getShader());
	auto distanceShaderParametersDefault = Engine::getShaderParameterDefaults(view->getPrototype()->getDistanceShader());
	auto shaderParameters = prototype->getShaderParameters();
	auto distanceShaderParameters = prototype->getDistanceShaderParameters();
	if (shaderParametersDefault.empty() == false) {
		for (auto& parameterIt: shaderParametersDefault) {
			auto& parameterName = parameterIt.first;
			auto parameterValueElement = dynamic_cast<GUIElementNode*>(shaderParametersContent->getScreenNode()->getNodeById("shaderparameters_" + GUIParser::unescapeQuotes(parameterName)));
			if (parameterValueElement == nullptr) continue;
			auto parameterValue = parameterValueElement->getController()->getValue().getString();
			shaderParameters.setShaderParameter(parameterName, parameterValue);
		}
	}

	if (distanceShaderParametersDefault.empty() == false) {
		for (auto& parameterIt: distanceShaderParametersDefault) {
			auto& parameterName = parameterIt.first;
			auto parameterValueElement = dynamic_cast<GUIElementNode*>(shaderParametersContent->getScreenNode()->getNodeById("shaderparameters_distance_" + GUIParser::unescapeQuotes(parameterName)));
			if (parameterValueElement == nullptr) continue;
			auto parameterValue = parameterValueElement->getController()->getValue().getString();
			distanceShaderParameters.setShaderParameter(parameterName, parameterValue);
		}
	}

	//
	prototype->setShaderParameters(shaderParameters);
	prototype->setDistanceShaderParameters(distanceShaderParameters);

	//
	view->updateShaderParemeters();
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

void ModelEditorScreenController::saveFile(const string& pathName, const string& fileName)
{
	view->saveFile(pathName, fileName);
}

void ModelEditorScreenController::loadFile(const string& pathName, const string& fileName)
{
	view->loadFile(pathName, fileName);
}

void ModelEditorScreenController::showErrorPopUp(const string& caption, const string& message)
{
	view->getPopUps()->getInfoDialogScreenController()->show(caption, message);
}

void ModelEditorScreenController::onValueChanged(GUIElementNode* node)
{
	if (node->getId() == "animations_dropdown") {
		onAnimationDropDownValueChanged();
	} else
	if (node->getId() == "materials_material_pbr_enabled") {
		onMaterialPBREnabledValueChanged();
	} else {
		prototypeBaseSubScreenController->onValueChanged(node, view->getPrototype());
		prototypePhysicsSubScreenController->onValueChanged(node, view->getPrototype());
		prototypeSoundsSubScreenController->onValueChanged(node, view->getPrototype());
	}
}

void ModelEditorScreenController::onActionPerformed(GUIActionListenerType type, GUIElementNode* node)
{
	prototypeBaseSubScreenController->onActionPerformed(type, node, view->getPrototype());
	prototypeDisplaySubScreenController->onActionPerformed(type, node);
	prototypePhysicsSubScreenController->onActionPerformed(type, node, view->getPrototype());
	prototypeSoundsSubScreenController->onActionPerformed(type, node, view->getPrototype());
	if (type == GUIActionListenerType::PERFORMED) {
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
		if (node->getId().compare("button_shaderparameters_apply") == 0) {
			onShaderParametersApply();
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
		if (node->getId().compare("preview_animations_attachment1_model_load") == 0) {
			onPreviewAnimationsAttachment1ModelLoad();
		} else
		if (node->getId().compare("preview_animations_attachment1_model_clear") == 0) {
			onPreviewAnimationsAttachment1ModelClear();
		} else
		if (node->getId().compare("button_preview_apply") == 0) {
			onPreviewApply();
		} else
		if (node->getId().compare("button_tools_computenormals") == 0) {
			onToolsComputeNormal();
		} else
		if (node->getId().compare("button_tools_optimizemodel") == 0) {
			onToolsOptimizeModel();
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
