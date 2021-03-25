#include <tdme/tools/shared/controller/ParticleSystemScreenController.h>

#include <string>
#include <vector>

#include <tdme/engine/fileio/models/ModelReader.h>
#include <tdme/engine/fileio/textures/TextureReader.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Color4Base.h>
#include <tdme/engine/primitives/OrientedBoundingBox.h>
#include <tdme/engine/prototype/Prototype.h>
#include <tdme/engine/prototype/PrototypeParticleSystem.h>
#include <tdme/engine/prototype/PrototypeParticleSystem_BoundingBoxParticleEmitter.h>
#include <tdme/engine/prototype/PrototypeParticleSystem_CircleParticleEmitter.h>
#include <tdme/engine/prototype/PrototypeParticleSystem_CircleParticleEmitterPlaneVelocity.h>
#include <tdme/engine/prototype/PrototypeParticleSystem_Emitter.h>
#include <tdme/engine/prototype/PrototypeParticleSystem_FogParticleSystem.h>
#include <tdme/engine/prototype/PrototypeParticleSystem_ObjectParticleSystem.h>
#include <tdme/engine/prototype/PrototypeParticleSystem_PointParticleEmitter.h>
#include <tdme/engine/prototype/PrototypeParticleSystem_PointParticleSystem.h>
#include <tdme/engine/prototype/PrototypeParticleSystem_SphereParticleEmitter.h>
#include <tdme/engine/prototype/PrototypeParticleSystem_Type.h>
#include <tdme/engine/Transformations.h>
#include <tdme/gui/events/Action.h>
#include <tdme/gui/events/GUIActionListener.h>
#include <tdme/gui/events/GUIChangeListener.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUINodeConditions.h>
#include <tdme/gui/nodes/GUINodeController.h>
#include <tdme/gui/nodes/GUIParentNode.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/gui/nodes/GUITextNode.h>
#include <tdme/gui/GUIParser.h>
#include <tdme/math/Matrix4x4.h>
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
#include <tdme/tools/shared/views/SharedParticleSystemView.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>
#include <tdme/utilities/Integer.h>
#include <tdme/utilities/MutableString.h>

using std::string;
using std::to_string;
using std::vector;

using tdme::engine::fileio::models::ModelReader;
using tdme::engine::fileio::textures::TextureReader;
using tdme::engine::model::Color4;
using tdme::engine::model::Color4Base;
using tdme::engine::primitives::OrientedBoundingBox;
using tdme::engine::prototype::Prototype;
using tdme::engine::prototype::PrototypeParticleSystem;
using tdme::engine::prototype::PrototypeParticleSystem_BoundingBoxParticleEmitter;
using tdme::engine::prototype::PrototypeParticleSystem_CircleParticleEmitter;
using tdme::engine::prototype::PrototypeParticleSystem_CircleParticleEmitterPlaneVelocity;
using tdme::engine::prototype::PrototypeParticleSystem_Emitter;
using tdme::engine::prototype::PrototypeParticleSystem_FogParticleSystem;
using tdme::engine::prototype::PrototypeParticleSystem_ObjectParticleSystem;
using tdme::engine::prototype::PrototypeParticleSystem_PointParticleEmitter;
using tdme::engine::prototype::PrototypeParticleSystem_PointParticleSystem;
using tdme::engine::prototype::PrototypeParticleSystem_SphereParticleEmitter;
using tdme::engine::prototype::PrototypeParticleSystem_Type;
using tdme::engine::Transformations;
using tdme::gui::events::Action;
using tdme::gui::events::GUIActionListenerType;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeConditions;
using tdme::gui::nodes::GUINodeController;
using tdme::gui::nodes::GUIParentNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::nodes::GUITextNode;
using tdme::gui::GUIParser;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;
using tdme::tools::modeleditor::TDMEModelEditor;
using tdme::tools::shared::controller::FileDialogPath;
using tdme::tools::shared::controller::FileDialogScreenController;
using tdme::tools::shared::controller::InfoDialogScreenController;
using tdme::tools::shared::controller::ParticleSystemScreenController;
using tdme::tools::shared::controller::PrototypeBaseSubScreenController;
using tdme::tools::shared::controller::PrototypeDisplaySubScreenController;
using tdme::tools::shared::controller::PrototypePhysicsSubScreenController;
using tdme::tools::shared::controller::PrototypeSoundsSubScreenController;
using tdme::tools::shared::tools::Tools;
using tdme::tools::shared::views::PopUps;
using tdme::tools::shared::views::SharedParticleSystemView;
using tdme::utilities::Console;
using tdme::utilities::Exception;
using tdme::utilities::Integer;
using tdme::utilities::MutableString;

string ParticleSystemScreenController::TYPE_NONE = "None";
string ParticleSystemScreenController::TYPE_OBJECTPARTICLESYSTEM = "Object Particle System";
string ParticleSystemScreenController::TYPE_POINTSPARTICLESYSTEM = "Points Particle System";
string ParticleSystemScreenController::TYPE_FOGPARTICLESYSTEM = "Fog Particle System";
string ParticleSystemScreenController::EMITTER_NONE = "None";
string ParticleSystemScreenController::EMITTER_POINTPARTICLEEMITTER = "Point Particle Emitter";
string ParticleSystemScreenController::EMITTER_BOUNDINGBOXPARTICLEEMITTER = "BoundingBox Particle Emitter";
string ParticleSystemScreenController::EMITTER_CIRCLEPARTICLEEMITTER = "Circle Particle Emitter";
string ParticleSystemScreenController::EMITTER_CIRCLEPARTICLEEMITTERPLANEVELOCITY = "Circle Particle Emitter Plane Velocity";
string ParticleSystemScreenController::EMITTER_SPHEREPARTICLEEMITTER = "Sphere Particle Emitter";

ParticleSystemScreenController::ParticleSystemScreenController(SharedParticleSystemView* view)
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
		 * @param particleSystemScreenController particle system screen controller
		 * @param finalView final view
		 */
		OnSetEntityDataAction(ParticleSystemScreenController* particleSystemScreenController, SharedParticleSystemView* finalView)
			: particleSystemScreenController(particleSystemScreenController)
			, finalView(finalView) {
		}

	private:
		ParticleSystemScreenController* particleSystemScreenController;
		SharedParticleSystemView* finalView;
	};

	this->view = view;
	auto const finalView = view;
	this->prototypeBaseSubScreenController = new PrototypeBaseSubScreenController(view->getPopUps(), new OnSetEntityDataAction(this, finalView));
	this->prototypePhysicsSubScreenController = new PrototypePhysicsSubScreenController(view->getPopUps(), &particleSystemPath, false);
	this->prototypeSoundsSubScreenController = new PrototypeSoundsSubScreenController(view, view->getPopUps(), &audioPath);
	this->prototypeDisplaySubScreenController = new PrototypeDisplaySubScreenController(this->prototypePhysicsSubScreenController->getView());
}

ParticleSystemScreenController::~ParticleSystemScreenController() {
	delete prototypeBaseSubScreenController;
	delete prototypeDisplaySubScreenController;
	delete prototypePhysicsSubScreenController;
}

PrototypeDisplaySubScreenController* ParticleSystemScreenController::getPrototypeDisplaySubScreenController()
{
	return prototypeDisplaySubScreenController;
}

PrototypePhysicsSubScreenController* ParticleSystemScreenController::getPrototypePhysicsSubScreenController()
{
	return prototypePhysicsSubScreenController;
}

PrototypeSoundsSubScreenController* ParticleSystemScreenController::getPrototypeSoundsSubScreenController()
{
	return prototypeSoundsSubScreenController;
}

GUIScreenNode* ParticleSystemScreenController::getScreenNode()
{
	return screenNode;
}

FileDialogPath* ParticleSystemScreenController::getParticleSystemPath()
{
	return &particleSystemPath;
}

FileDialogPath* ParticleSystemScreenController::getModelPath()
{
	return &modelPath;
}

void ParticleSystemScreenController::initialize()
{
	try {
		screenNode = GUIParser::parse("resources/engine/gui", "screen_particlesystem.xml");
		screenNode->addActionListener(this);
		screenNode->addChangeListener(this);
		screenCaption = dynamic_cast< GUITextNode* >(screenNode->getNodeById("screen_caption"));
		particleSystemReload = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_entity_reload"));
		particleSystemSave = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_entity_save"));
		particleSystemTypes = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("ps_types"));
		particleSystemType = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("ps_type"));
		particleSystemTypeApply = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_emitter_apply"));
		particleSystemEmitters = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("ps_emitters"));
		particleSystemEmitter = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("ps_emitter"));
		particleSystemEmitterApply = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_ps_type_apply"));;
		opsScale = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("ops_scale"));
		opsMaxCount = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("ops_maxcount"));
		opsModel = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("ops_model"));
		opsAutoEmit = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("ops_auto_emit"));
		ppsMaxPoints = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("pps_maxpoints"));
		ppsPointSize = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("pps_pointsize"));
		ppsTexture = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("pps_texture"));
		ppsTransparencyTexture = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("pps_transparency_texture"));
		ppsHorizontalSprites = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("pps_horizontal_sprites"));
		ppsVerticalSprites = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("pps_vertical_sprites"));
		ppsFPS = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("pps_fps"));
		ppsAutoEmit = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("pps_auto_emit"));
		fpsMaxPoints = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("fps_maxpoints"));
		fpsPointSize = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("fps_pointsize"));
		fpsTexture = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("fps_texture"));
		fpsTransparencyTexture = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("fps_transparency_texture"));
		fpsHorizontalSprites = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("fps_horizontal_sprites"));
		fpsVerticalSprites = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("fps_vertical_sprites"));
		fpsFPS = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("fps_fps"));
		ppeCount = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("ppe_count"));
		ppeLifeTime = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("ppe_lifetime"));
		ppeLifeTimeRnd = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("ppe_lifetimernd"));
		ppeMass = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("ppe_mass"));
		ppeMassRnd = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("ppe_massrnd"));
		ppePosition = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("ppe_position"));
		ppeVelocity = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("ppe_velocity"));
		ppeVelocityRnd = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("ppe_velocityrnd"));
		ppeColorStart = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("ppe_colorstart"));
		ppeColorEnd = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("ppe_colorend"));
		bbpeCount = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("bbpe_count"));
		bbpeLifeTime = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("bbpe_lifetime"));
		bbpeLifeTimeRnd = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("bbpe_lifetimernd"));
		bbpeMass = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("bbpe_mass"));
		bbpeMassRnd = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("bbpe_massrnd"));
		bbpeVelocity = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("bbpe_velocity"));
		bbpeVelocityRnd = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("bbpe_velocityrnd"));
		bbpeColorStart = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("bbpe_colorstart"));
		bbpeColorEnd = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("bbpe_colorend"));
		bbpeObbCenter = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("bbpe_obb_center"));
		bbpeObbHalfextension = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("bbpe_obb_halfextension"));
		bbpeObbRotationX = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("bbpe_obb_rotation_x"));
		bbpeObbRotationY = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("bbpe_obb_rotation_y"));
		bbpeObbRotationZ = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("bbpe_obb_rotation_z"));
		cpeCount = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpe_count"));
		cpeLifeTime = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpe_lifetime"));
		cpeLifeTimeRnd = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpe_lifetimernd"));
		cpeMass = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpe_mass"));
		cpeMassRnd = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpe_massrnd"));
		cpeVelocity = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpe_velocity"));
		cpeVelocityRnd = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpe_velocityrnd"));
		cpeColorStart = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpe_colorstart"));
		cpeColorEnd = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpe_colorend"));
		cpeCenter = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpe_center"));
		cpeRadius = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpe_radius"));
		cpeRotationX = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpe_rotation_x"));
		cpeRotationY = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpe_rotation_y"));
		cpeRotationZ = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpe_rotation_z"));
		cpepvCount = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpepv_count"));
		cpepvLifeTime = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpepv_lifetime"));
		cpepvLifeTimeRnd = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpepv_lifetimernd"));
		cpepvMass = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpepv_mass"));
		cpepvMassRnd = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpepv_massrnd"));
		cpepvVelocity = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpepv_velocity"));
		cpepvVelocityRnd = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpepv_velocityrnd"));
		cpepvColorStart = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpepv_colorstart"));
		cpepvColorEnd = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpepv_colorend"));
		cpepvCenter = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpepv_center"));
		cpepvRadius = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpepv_radius"));
		cpepvRotationX = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpepv_rotation_x"));
		cpepvRotationY = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpepv_rotation_y"));
		cpepvRotationZ = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpepv_rotation_z"));
		speCount = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("spe_count"));
		speLifeTime = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("spe_lifetime"));
		speLifeTimeRnd = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("spe_lifetimernd"));
		speMass = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("spe_mass"));
		speMassRnd = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("spe_massrnd"));
		speVelocity = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("spe_velocity"));
		speVelocityRnd = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("spe_velocityrnd"));
		speColorStart = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("spe_colorstart"));
		speColorEnd = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("spe_colorend"));
		speCenter = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("spe_center"));
		speRadius = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("spe_radius"));
		particleSystemsListbox = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("particlesystems_listbox"));
		particleSystemAddButton = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_particlesystem_add"));
		particleSystemRemoveButton = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_particlesystem_remove"));
		viewPort = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("viewport"));
	} catch (Exception& exception) {
		Console::print(string("ParticleSystemScreenController::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	prototypeBaseSubScreenController->initialize(screenNode);
	prototypeDisplaySubScreenController->initialize(screenNode);
	prototypePhysicsSubScreenController->initialize(screenNode);
	prototypeSoundsSubScreenController->initialize(screenNode);
}

void ParticleSystemScreenController::dispose()
{
}

void ParticleSystemScreenController::setScreenCaption(const string& text)
{
	screenCaption->setText(text);
}

void ParticleSystemScreenController::setPrototypeData(const string& name, const string& description)
{
	prototypeBaseSubScreenController->setPrototypeData(name, description);
	particleSystemReload->getController()->setDisabled(false);
	particleSystemSave->getController()->setDisabled(false);
}

void ParticleSystemScreenController::unsetPrototypeData()
{
	prototypeBaseSubScreenController->unsetPrototypeData();
	particleSystemReload->getController()->setDisabled(true);
	particleSystemSave->getController()->setDisabled(true);
}

void ParticleSystemScreenController::setPrototypeProperties(const string& presetId, Prototype* entity, const string& selectedName)
{
	prototypeBaseSubScreenController->setPrototypeProperties(view->getPrototype(), presetId, selectedName);
}

void ParticleSystemScreenController::unsetPrototypeProperties()
{
	prototypeBaseSubScreenController->unsetPrototypeProperties();
}

void ParticleSystemScreenController::setParticleSystemTypes(const vector<string>& particleSystemTypesCollection)
{
	auto particleSystemTypesInnerNode = dynamic_cast< GUIParentNode* >(particleSystemTypes->getScreenNode()->getNodeById(particleSystemTypes->getId() + "_inner"));
	auto idx = 0;
	string particleSystemTypesInnerNodeSubNodesXML = "";
	particleSystemTypesInnerNodeSubNodesXML =
		particleSystemTypesInnerNodeSubNodesXML +
		"<scrollarea id=\"" +
		particleSystemTypes->getId() +
		"_inner_scrollarea\" width=\"100%\" height=\"100\">\n";
	for (auto particleSystem: particleSystemTypesCollection) {
		particleSystemTypesInnerNodeSubNodesXML =
			particleSystemTypesInnerNodeSubNodesXML +
			"<dropdown-option text=\"" +
			GUIParser::escapeQuotes(particleSystem) +
			"\" value=\"" +
			GUIParser::escapeQuotes(particleSystem) +
			"\" " +
			(idx == 0 ? "selected=\"true\" " : "") +
			" />\n";
		idx++;
	}
	particleSystemTypesInnerNodeSubNodesXML =
		particleSystemTypesInnerNodeSubNodesXML +
		"</scrollarea>";
	try {
		particleSystemTypesInnerNode->replaceSubNodes(particleSystemTypesInnerNodeSubNodesXML, true);
	} catch (Exception& exception) {
		Console::print(string("ParticleSystemScreenController::setParticleSystemTypes(): An error occurred: "));
		Console::println(string(exception.what()));
	}
}

void ParticleSystemScreenController::setParticleSystemEmitters(const vector<string>& emittersCollection)
{
	auto particleSystemEmittersInnerNode = dynamic_cast< GUIParentNode* >((particleSystemEmitters->getScreenNode()->getNodeById(particleSystemEmitters->getId() + "_inner")));
	auto idx = 0;
	string particleSystemEmittersInnerNodeSubNodesXML = "";
	particleSystemEmittersInnerNodeSubNodesXML =
		particleSystemEmittersInnerNodeSubNodesXML +
		"<scrollarea id=\"" +
		particleSystemEmitters->getId() +
		"_inner_scrollarea\" width=\"100%\" height=\"100\">\n";
	for (auto particleSystemEmitter: emittersCollection) {
		particleSystemEmittersInnerNodeSubNodesXML =
			particleSystemEmittersInnerNodeSubNodesXML +
			"<dropdown-option text=\"" +
			GUIParser::escapeQuotes(particleSystemEmitter) +
			"\" value=\"" +
			GUIParser::escapeQuotes(particleSystemEmitter) +
			"\" " +
			(idx == 0 ? "selected=\"true\" " : "") +
			" />\n";
		idx++;
	}
	particleSystemEmittersInnerNodeSubNodesXML =
		particleSystemEmittersInnerNodeSubNodesXML +
		"</scrollarea>";
	try {
		particleSystemEmittersInnerNode->replaceSubNodes(particleSystemEmittersInnerNodeSubNodesXML, true);
	} catch (Exception& exception) {
		Console::print(string("ParticleSystemScreenController::setParticleSystemEmitters: An error occurred: "));
		Console::println(string(exception.what()));
	}
}

void ParticleSystemScreenController::onQuit()
{
	TDMEModelEditor::getInstance()->quit();
}

void ParticleSystemScreenController::unsetParticleSystemType()
{
	particleSystemTypes->getController()->setDisabled(true);
	particleSystemTypeApply->getController()->setDisabled(true);
}

void ParticleSystemScreenController::setParticleSystemType()
{
	particleSystemTypes->getController()->setDisabled(false);
	particleSystemTypeApply->getController()->setDisabled(false);
	auto particleSystem = view->getPrototype()->getParticleSystemAt(view->getParticleSystemIndex());
	particleSystemType->getActiveConditions().removeAll();
	{
		auto v = particleSystem->getType();
		if (v == PrototypeParticleSystem_Type::NONE) {
			particleSystemTypes->getController()->setValue(MutableString(TYPE_NONE));
			particleSystemType->getActiveConditions().add(TYPE_NONE);
		} else
		if (v == PrototypeParticleSystem_Type::OBJECT_PARTICLE_SYSTEM) {
			particleSystemTypes->getController()->setValue(MutableString(TYPE_OBJECTPARTICLESYSTEM));
			particleSystemType->getActiveConditions().add(TYPE_OBJECTPARTICLESYSTEM);
			opsMaxCount->getController()->setValue(MutableString(particleSystem->getObjectParticleSystem()->getMaxCount()));
			opsScale->getController()->setValue(MutableString(Tools::formatVector3(particleSystem->getObjectParticleSystem()->getScale())));
			opsModel->getController()->setValue(MutableString(particleSystem->getObjectParticleSystem()->getModelFile()));
			opsAutoEmit->getController()->setValue(MutableString(particleSystem->getObjectParticleSystem()->isAutoEmit() == true ? "1" : ""));
		} else
		if (v == PrototypeParticleSystem_Type::POINT_PARTICLE_SYSTEM) {
			particleSystemTypes->getController()->setValue(MutableString(TYPE_POINTSPARTICLESYSTEM));
			particleSystemType->getActiveConditions().add(TYPE_POINTSPARTICLESYSTEM);
			ppsMaxPoints->getController()->setValue(MutableString(particleSystem->getPointParticleSystem()->getMaxPoints()));
			ppsPointSize->getController()->setValue(MutableString(particleSystem->getPointParticleSystem()->getPointSize(), 4));
			ppsTexture->getController()->setValue(MutableString(particleSystem->getPointParticleSystem()->getTextureFileName()));
			ppsTransparencyTexture->getController()->setValue(MutableString(particleSystem->getPointParticleSystem()->getTransparencyTextureFileName()));
			ppsAutoEmit->getController()->setValue(MutableString(particleSystem->getPointParticleSystem()->isAutoEmit() == true ? "1" : ""));
			ppsHorizontalSprites->getController()->setValue(MutableString(particleSystem->getPointParticleSystem()->getTextureHorizontalSprites()));
			ppsVerticalSprites->getController()->setValue(MutableString(particleSystem->getPointParticleSystem()->getTextureVerticalSprites()));
			ppsFPS->getController()->setValue(MutableString(particleSystem->getPointParticleSystem()->getTextureSpritesFPS()));
		} else
		if (v == PrototypeParticleSystem_Type::FOG_PARTICLE_SYSTEM) {
			particleSystemTypes->getController()->setValue(MutableString(TYPE_FOGPARTICLESYSTEM));
			particleSystemType->getActiveConditions().add(TYPE_FOGPARTICLESYSTEM);
			fpsMaxPoints->getController()->setValue(MutableString(particleSystem->getFogParticleSystem()->getMaxPoints()));
			fpsPointSize->getController()->setValue(MutableString(particleSystem->getFogParticleSystem()->getPointSize(), 4));
			fpsTexture->getController()->setValue(MutableString(particleSystem->getFogParticleSystem()->getTextureFileName()));
			fpsTransparencyTexture->getController()->setValue(MutableString(particleSystem->getFogParticleSystem()->getTransparencyTextureFileName()));
			fpsHorizontalSprites->getController()->setValue(MutableString(particleSystem->getFogParticleSystem()->getTextureHorizontalSprites()));
			fpsVerticalSprites->getController()->setValue(MutableString(particleSystem->getFogParticleSystem()->getTextureVerticalSprites()));
			fpsFPS->getController()->setValue(MutableString(particleSystem->getFogParticleSystem()->getTextureSpritesFPS()));
		} else {
			Console::println(
				string(
					"ParticleSystemScreenController::setParticleSystemType(): unknown particle system type '" +
					particleSystem->getType()->getName() +
					"'"
				)
			);
		}
	}
}

void ParticleSystemScreenController::onParticleSystemTypeDataApply()
{
	try {
		auto particleSystem = view->getPrototype()->getParticleSystemAt(view->getParticleSystemIndex());
		{
			auto v = particleSystem->getType();
			if (v == PrototypeParticleSystem_Type::NONE) {
			} else
			if (v == PrototypeParticleSystem_Type::OBJECT_PARTICLE_SYSTEM) {
				particleSystem->getObjectParticleSystem()->setMaxCount(Tools::convertToInt(opsMaxCount->getController()->getValue().getString()));
				particleSystem->getObjectParticleSystem()->getScale().set(Tools::convertToVector3(opsScale->getController()->getValue().getString()));
				particleSystem->getObjectParticleSystem()->setAutoEmit(opsAutoEmit->getController()->getValue().getString() == "1");
				try {
					particleSystem->getObjectParticleSystem()->setModelFile(opsModel->getController()->getValue().getString());
				} catch (Exception& exception) {
					view->getPopUps()->getInfoDialogScreenController()->show("Error", "An error occurred: " + (string(exception.what())));
				}
			} else
			if (v == PrototypeParticleSystem_Type::POINT_PARTICLE_SYSTEM) {
				particleSystem->getPointParticleSystem()->setMaxPoints(Tools::convertToInt(ppsMaxPoints->getController()->getValue().getString()));
				particleSystem->getPointParticleSystem()->setPointSize(Tools::convertToFloat(ppsPointSize->getController()->getValue().getString()));
				particleSystem->getPointParticleSystem()->setTextureFileName(ppsTexture->getController()->getValue().getString(), ppsTransparencyTexture->getController()->getValue().getString());
				particleSystem->getPointParticleSystem()->setAutoEmit(ppsAutoEmit->getController()->getValue().getString() == "1");
				particleSystem->getPointParticleSystem()->setTextureHorizontalSprites(Tools::convertToInt(ppsHorizontalSprites->getController()->getValue().getString()));
				particleSystem->getPointParticleSystem()->setTextureVerticalSprites(Tools::convertToInt(ppsVerticalSprites->getController()->getValue().getString()));
				particleSystem->getPointParticleSystem()->setTextureSpritesFPS(Tools::convertToFloat(ppsFPS->getController()->getValue().getString()));
			} else
			if (v == PrototypeParticleSystem_Type::FOG_PARTICLE_SYSTEM) {
				particleSystem->getFogParticleSystem()->setMaxPoints(Tools::convertToInt(fpsMaxPoints->getController()->getValue().getString()));
				particleSystem->getFogParticleSystem()->setPointSize(Tools::convertToFloat(fpsPointSize->getController()->getValue().getString()));
				particleSystem->getFogParticleSystem()->setTextureFileName(fpsTexture->getController()->getValue().getString(), fpsTransparencyTexture->getController()->getValue().getString());
				particleSystem->getFogParticleSystem()->setTextureHorizontalSprites(Tools::convertToInt(fpsHorizontalSprites->getController()->getValue().getString()));
				particleSystem->getFogParticleSystem()->setTextureVerticalSprites(Tools::convertToInt(fpsVerticalSprites->getController()->getValue().getString()));
				particleSystem->getFogParticleSystem()->setTextureSpritesFPS(Tools::convertToFloat(fpsFPS->getController()->getValue().getString()));
			} else {
				Console::println(
					string(
						"ParticleSystemScreenController::setParticleSystemType(): unknown particle system type '" +
						particleSystem->getType()->getName() +
						"'"
					)
				);
			}
		}
	} catch (Exception& exception) {
		showErrorPopUp("Warning", (string(exception.what())));
	}
}

void ParticleSystemScreenController::onParticleSystemTypeApply()
{
	auto particleSystemTypeString = particleSystemTypes->getController()->getValue().getString();
	particleSystemType->getActiveConditions().removeAll();
	particleSystemType->getActiveConditions().add(particleSystemTypeString);
	if (particleSystemTypeString == TYPE_NONE) {
		view->getPrototype()->getParticleSystemAt(view->getParticleSystemIndex())->setType(PrototypeParticleSystem_Type::NONE);
	} else
	if (particleSystemTypeString == TYPE_OBJECTPARTICLESYSTEM) {
		view->getPrototype()->getParticleSystemAt(view->getParticleSystemIndex())->setType(PrototypeParticleSystem_Type::OBJECT_PARTICLE_SYSTEM);
	} else
	if (particleSystemTypeString == TYPE_POINTSPARTICLESYSTEM) {
		view->getPrototype()->getParticleSystemAt(view->getParticleSystemIndex())->setType(PrototypeParticleSystem_Type::POINT_PARTICLE_SYSTEM);
	} else
	if (particleSystemTypeString == TYPE_FOGPARTICLESYSTEM) {
		view->getPrototype()->getParticleSystemAt(view->getParticleSystemIndex())->setType(PrototypeParticleSystem_Type::FOG_PARTICLE_SYSTEM);
	} else {
		Console::println(
			string(
				"ParticleSystemScreenController::onParticleSystemTypeApply(): unknown particle system type '" +
				particleSystemTypeString +
				"'"
			)
		);
	}

	setParticleSystemType();
}

void ParticleSystemScreenController::onParticleSystemEmitterApply()
{
	auto particleSystem = view->getPrototype()->getParticleSystemAt(view->getParticleSystemIndex());
	auto particleSystemEmitterString = particleSystemEmitters->getController()->getValue().getString();
	particleSystemEmitter->getActiveConditions().removeAll();
	particleSystemEmitter->getActiveConditions().add(particleSystemEmitterString);
	if (particleSystemEmitterString == EMITTER_NONE) {
		particleSystem->setEmitter(PrototypeParticleSystem_Emitter::NONE);
	} else
	if (particleSystemEmitterString == EMITTER_POINTPARTICLEEMITTER) {
		particleSystem->setEmitter(PrototypeParticleSystem_Emitter::POINT_PARTICLE_EMITTER);
	} else
	if (particleSystemEmitterString == EMITTER_BOUNDINGBOXPARTICLEEMITTER) {
		particleSystem->setEmitter(PrototypeParticleSystem_Emitter::BOUNDINGBOX_PARTICLE_EMITTER);
	} else
	if (particleSystemEmitterString == EMITTER_CIRCLEPARTICLEEMITTER) {
		particleSystem->setEmitter(PrototypeParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER);
	} else
	if (particleSystemEmitterString == EMITTER_CIRCLEPARTICLEEMITTERPLANEVELOCITY) {
		particleSystem->setEmitter(PrototypeParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER_PLANE_VELOCITY);
	} else
	if (particleSystemEmitterString == EMITTER_SPHEREPARTICLEEMITTER) {
		particleSystem->setEmitter(PrototypeParticleSystem_Emitter::SPHERE_PARTICLE_EMITTER);
	} else {
		Console::println(
			string(
				"ParticleSystemScreenController::onParticleSystemEmitterApply(): unknown particle system emitter '" +
				particleSystemEmitterString +
				"'"
			)
		);
	}
}

void ParticleSystemScreenController::onParticleSystemEmitterDataApply()
{
	try {
		auto particleSystem = view->getPrototype()->getParticleSystemAt(view->getParticleSystemIndex());;
		{
			auto v = particleSystem->getEmitter();
			if (v == PrototypeParticleSystem_Emitter::NONE)
			{
				particleSystemEmitters->getController()->setValue(MutableString(EMITTER_NONE));
			} else
			if (v == PrototypeParticleSystem_Emitter::POINT_PARTICLE_EMITTER)
			{
				particleSystemEmitters->getController()->setValue(MutableString(EMITTER_POINTPARTICLEEMITTER));
				auto emitter = particleSystem->getPointParticleEmitter();
				emitter->setCount(Tools::convertToInt(ppeCount->getController()->getValue().getString()));
				emitter->setLifeTime(Tools::convertToInt(ppeLifeTime->getController()->getValue().getString()));
				emitter->setLifeTimeRnd(Tools::convertToInt(ppeLifeTimeRnd->getController()->getValue().getString()));
				emitter->setMass(Tools::convertToFloat(ppeMass->getController()->getValue().getString()));
				emitter->setMassRnd(Tools::convertToFloat(ppeMassRnd->getController()->getValue().getString()));
				emitter->setPosition(Tools::convertToVector3(ppePosition->getController()->getValue().getString()));
				emitter->setVelocity(Tools::convertToVector3(ppeVelocity->getController()->getValue().getString()));
				emitter->setVelocityRnd(Tools::convertToVector3(ppeVelocityRnd->getController()->getValue().getString()));
				emitter->setColorStart(Tools::convertToColor4(ppeColorStart->getController()->getValue().getString()));
				emitter->setColorEnd(Tools::convertToColor4(ppeColorEnd->getController()->getValue().getString()));
			} else
			if (v == PrototypeParticleSystem_Emitter::BOUNDINGBOX_PARTICLE_EMITTER)
			{
				particleSystemEmitters->getController()->setValue(MutableString(EMITTER_BOUNDINGBOXPARTICLEEMITTER));
				auto emitter = particleSystem->getBoundingBoxParticleEmitters();
				emitter->setCount(Tools::convertToInt(bbpeCount->getController()->getValue().getString()));
				emitter->setLifeTime(Tools::convertToInt(bbpeLifeTime->getController()->getValue().getString()));
				emitter->setLifeTimeRnd(Tools::convertToInt(bbpeLifeTimeRnd->getController()->getValue().getString()));
				emitter->setMass(Tools::convertToFloat(bbpeMass->getController()->getValue().getString()));
				emitter->setMassRnd(Tools::convertToFloat(bbpeMassRnd->getController()->getValue().getString()));
				emitter->setVelocity(Tools::convertToVector3(bbpeVelocity->getController()->getValue().getString()));
				emitter->setVelocityRnd(Tools::convertToVector3(bbpeVelocityRnd->getController()->getValue().getString()));
				emitter->setColorStart(Tools::convertToColor4(bbpeColorStart->getController()->getValue().getString()));
				emitter->setColorEnd(Tools::convertToColor4(bbpeColorEnd->getController()->getValue().getString()));
				emitter->setObbCenter(Tools::convertToVector3(bbpeObbCenter->getController()->getValue().getString()));
				emitter->setObbHalfextension(Tools::convertToVector3(bbpeObbHalfextension->getController()->getValue().getString()));
				Transformations transformations;
				transformations.addRotation(OrientedBoundingBox::AABB_AXIS_Z, Tools::convertToFloat(bbpeObbRotationZ->getController()->getValue().getString()));
				transformations.addRotation(OrientedBoundingBox::AABB_AXIS_Y, Tools::convertToFloat(bbpeObbRotationY->getController()->getValue().getString()));
				transformations.addRotation(OrientedBoundingBox::AABB_AXIS_X, Tools::convertToFloat(bbpeObbRotationX->getController()->getValue().getString()));
				transformations.update();
				Vector3 obbAxis0;
				Vector3 obbAxis1;
				Vector3 obbAxis2;
				transformations.getTransformationsMatrix().getAxes(obbAxis0, obbAxis1, obbAxis2);
				emitter->setObbAxis0(obbAxis0);
				emitter->setObbAxis1(obbAxis1);
				emitter->setObbAxis2(obbAxis2);
			} else
			if (v == PrototypeParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER)
			{
				particleSystemEmitters->getController()->setValue(MutableString(EMITTER_CIRCLEPARTICLEEMITTER));
				auto emitter = particleSystem->getCircleParticleEmitter();
				emitter->setCount(Tools::convertToInt(cpeCount->getController()->getValue().getString()));
				emitter->setLifeTime(Tools::convertToInt(cpeLifeTime->getController()->getValue().getString()));
				emitter->setLifeTimeRnd(Tools::convertToInt(cpeLifeTimeRnd->getController()->getValue().getString()));
				emitter->setMass(Tools::convertToFloat(cpeMass->getController()->getValue().getString()));
				emitter->setMassRnd(Tools::convertToFloat(cpeMassRnd->getController()->getValue().getString()));
				emitter->setVelocity(Tools::convertToVector3(cpeVelocity->getController()->getValue().getString()));
				emitter->setVelocityRnd(Tools::convertToVector3(cpeVelocityRnd->getController()->getValue().getString()));
				emitter->setColorStart(Tools::convertToColor4(cpeColorStart->getController()->getValue().getString()));
				emitter->setColorEnd(Tools::convertToColor4(cpeColorEnd->getController()->getValue().getString()));
				emitter->setCenter(Tools::convertToVector3(cpeCenter->getController()->getValue().getString()));
				emitter->setRadius(Tools::convertToFloat(cpeRadius->getController()->getValue().getString()));
				Transformations transformations;
				transformations.addRotation(OrientedBoundingBox::AABB_AXIS_Z, Tools::convertToFloat(cpeRotationZ->getController()->getValue().getString()));
				transformations.addRotation(OrientedBoundingBox::AABB_AXIS_Y, Tools::convertToFloat(cpeRotationY->getController()->getValue().getString()));
				transformations.addRotation(OrientedBoundingBox::AABB_AXIS_X, Tools::convertToFloat(cpeRotationX->getController()->getValue().getString()));
				transformations.update();
				Vector3 tmpVector3;
				Vector3 axis0;
				Vector3 axis1;
				transformations.getTransformationsMatrix().getAxes(axis0, tmpVector3, axis1);
				emitter->setAxis0(axis0);
				emitter->setAxis1(axis1);
			} else
			if (v == PrototypeParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER_PLANE_VELOCITY)
			{
				particleSystemEmitters->getController()->setValue(MutableString(EMITTER_CIRCLEPARTICLEEMITTERPLANEVELOCITY));
				auto emitter = particleSystem->getCircleParticleEmitterPlaneVelocity();
				emitter->setCount(Tools::convertToInt(cpepvCount->getController()->getValue().getString()));
				emitter->setLifeTime(Tools::convertToInt(cpepvLifeTime->getController()->getValue().getString()));
				emitter->setLifeTimeRnd(Tools::convertToInt(cpepvLifeTimeRnd->getController()->getValue().getString()));
				emitter->setMass(Tools::convertToFloat(cpepvMass->getController()->getValue().getString()));
				emitter->setMassRnd(Tools::convertToFloat(cpepvMassRnd->getController()->getValue().getString()));
				emitter->setVelocity(Tools::convertToFloat(cpepvVelocity->getController()->getValue().getString()));
				emitter->setVelocityRnd(Tools::convertToFloat(cpepvVelocityRnd->getController()->getValue().getString()));
				emitter->setColorStart(Tools::convertToColor4(cpepvColorStart->getController()->getValue().getString()));
				emitter->setColorEnd(Tools::convertToColor4(cpepvColorEnd->getController()->getValue().getString()));
				emitter->setCenter(Tools::convertToVector3(cpepvCenter->getController()->getValue().getString()));
				emitter->setRadius(Tools::convertToFloat(cpepvRadius->getController()->getValue().getString()));
				Transformations transformations;
				transformations.addRotation(OrientedBoundingBox::AABB_AXIS_Z, Tools::convertToFloat(cpepvRotationZ->getController()->getValue().getString()));
				transformations.addRotation(OrientedBoundingBox::AABB_AXIS_Y, Tools::convertToFloat(cpepvRotationY->getController()->getValue().getString()));
				transformations.addRotation(OrientedBoundingBox::AABB_AXIS_X, Tools::convertToFloat(cpepvRotationX->getController()->getValue().getString()));
				transformations.update();
				Vector3 tmpVector3;
				Vector3 axis0;
				Vector3 axis1;
				transformations.getTransformationsMatrix().getAxes(axis0, tmpVector3, axis1);
				emitter->setAxis0(axis0);
				emitter->setAxis1(axis1);
			} else
			if (v == PrototypeParticleSystem_Emitter::SPHERE_PARTICLE_EMITTER)
			{
				particleSystemEmitters->getController()->setValue(MutableString(EMITTER_SPHEREPARTICLEEMITTER));
				auto emitter = particleSystem->getSphereParticleEmitter();
				emitter->setCount(Tools::convertToInt(speCount->getController()->getValue().getString()));
				emitter->setLifeTime(Tools::convertToInt(speLifeTime->getController()->getValue().getString()));
				emitter->setLifeTimeRnd(Tools::convertToInt(speLifeTimeRnd->getController()->getValue().getString()));
				emitter->setMass(Tools::convertToFloat(speMass->getController()->getValue().getString()));
				emitter->setMassRnd(Tools::convertToFloat(speMassRnd->getController()->getValue().getString()));
				emitter->setVelocity(Tools::convertToVector3(speVelocity->getController()->getValue().getString()));
				emitter->setVelocityRnd(Tools::convertToVector3(speVelocityRnd->getController()->getValue().getString()));
				emitter->setColorStart(Tools::convertToColor4(speColorStart->getController()->getValue().getString()));
				emitter->setColorEnd(Tools::convertToColor4(speColorEnd->getController()->getValue().getString()));
				emitter->setCenter(Tools::convertToVector3(speCenter->getController()->getValue().getString()));
				emitter->setRadius(Tools::convertToFloat(speRadius->getController()->getValue().getString()));
			} else {
				Console::println(
					string(
						"ParticleSystemScreenController::onParticleSystemEmitterApply(): unknown particle system emitter '" +
						particleSystem->getEmitter()->getName() +
						"'"
					)
				);
			}
		}

	} catch (Exception& exception) {
		showErrorPopUp("Warning", (string(exception.what())));
	}
}

void ParticleSystemScreenController::unsetParticleSystemEmitter() {
	particleSystemEmitters->getController()->setDisabled(true);
	particleSystemEmitterApply->getController()->setDisabled(true);
}

void ParticleSystemScreenController::updatePointParticleSystemEmitter(const Vector3& position) {
	ppePosition->getController()->setValue(MutableString(Tools::formatVector3(position)));
}

void ParticleSystemScreenController::updateBoundingBoxParticleSystemEmitter(const Vector3& center, const Vector3& axis0, const Vector3& axis1, const Vector3& axis2, const Vector3& halfExtension) {
	bbpeObbCenter->getController()->setValue(MutableString(Tools::formatVector3(center)));
	bbpeObbHalfextension->getController()->setValue(MutableString(Tools::formatVector3(halfExtension)));
	Vector3 rotation;
	Matrix4x4 rotationMatrix;
	rotationMatrix.identity();
	rotationMatrix.setAxes(axis0, axis1, axis2);
	rotationMatrix.computeEulerAngles(rotation);
	bbpeObbRotationX->getController()->setValue(MutableString(Tools::formatFloat(rotation.getX())));
	bbpeObbRotationY->getController()->setValue(MutableString(Tools::formatFloat(rotation.getY())));
	bbpeObbRotationZ->getController()->setValue(MutableString(Tools::formatFloat(rotation.getZ())));
}

void ParticleSystemScreenController::updateCircleParticleSystemEmitter(const Vector3& center, const Vector3& axis0, const Vector3& axis1, float radius) {
	cpeCenter->getController()->setValue(MutableString(Tools::formatVector3(center)));
	cpeRadius->getController()->setValue(MutableString(radius, 4));
	Vector3 tmpAxis;
	Vector3 rotation;
	Matrix4x4 rotationMatrix;
	rotationMatrix.identity();
	rotationMatrix.setAxes(axis0, Vector3::computeCrossProduct(axis0, axis1, tmpAxis), axis1);
	rotationMatrix.computeEulerAngles(rotation);
	cpeRotationX->getController()->setValue(MutableString(Tools::formatFloat(rotation.getX())));
	cpeRotationY->getController()->setValue(MutableString(Tools::formatFloat(rotation.getY())));
	cpeRotationZ->getController()->setValue(MutableString(Tools::formatFloat(rotation.getZ())));
}

void ParticleSystemScreenController::updateCirclePlaneVelocityParticleSystemEmitter(const Vector3& center, const Vector3& axis0, const Vector3& axis1, float radius) {
	cpepvCenter->getController()->setValue(MutableString(Tools::formatVector3(center)));
	cpepvRadius->getController()->setValue(MutableString(radius, 4));
	Vector3 tmpAxis;
	Vector3 rotation;
	Matrix4x4 rotationMatrix;
	rotationMatrix.identity();
	rotationMatrix.setAxes(axis0, Vector3::computeCrossProduct(axis0, axis1, tmpAxis), axis1);
	rotationMatrix.computeEulerAngles(rotation);
	cpepvRotationX->getController()->setValue(MutableString(Tools::formatFloat(rotation.getX())));
	cpepvRotationY->getController()->setValue(MutableString(Tools::formatFloat(rotation.getY())));
	cpepvRotationZ->getController()->setValue(MutableString(Tools::formatFloat(rotation.getZ())));
}

void ParticleSystemScreenController::updateSphereParticleSystemEmitter(const Vector3& center, float radius) {
	speCenter->getController()->setValue(MutableString(Tools::formatVector3(center)));
	speRadius->getController()->setValue(MutableString(radius, 4));
}

void ParticleSystemScreenController::setParticleSystemEmitter()
{
	particleSystemEmitters->getController()->setDisabled(false);
	particleSystemEmitterApply->getController()->setDisabled(false);
	particleSystemEmitter->getActiveConditions().removeAll();
	auto particleSystem = view->getPrototype()->getParticleSystemAt(view->getParticleSystemIndex());
	{
		auto v = particleSystem->getEmitter();
		if (v == PrototypeParticleSystem_Emitter::NONE) {
			particleSystemEmitters->getController()->setValue(MutableString(EMITTER_NONE));
			particleSystemEmitter->getActiveConditions().add(EMITTER_NONE);
		} else
		if (v == PrototypeParticleSystem_Emitter::POINT_PARTICLE_EMITTER) {
			particleSystemEmitters->getController()->setValue(MutableString(EMITTER_POINTPARTICLEEMITTER));
			particleSystemEmitter->getActiveConditions().add(EMITTER_POINTPARTICLEEMITTER);
			auto emitter = particleSystem->getPointParticleEmitter();
			ppeCount->getController()->setValue(MutableString(emitter->getCount()));
			ppeLifeTime->getController()->setValue(MutableString(static_cast<int>(emitter->getLifeTime())));
			ppeLifeTimeRnd->getController()->setValue(MutableString(static_cast<int>(emitter->getLifeTimeRnd())));
			ppeMass->getController()->setValue(MutableString(emitter->getMass(), 4));
			ppeMassRnd->getController()->setValue(MutableString(emitter->getMassRnd(), 4));
			ppePosition->getController()->setValue(MutableString(Tools::formatVector3(emitter->getPosition())));
			ppeVelocity->getController()->setValue(MutableString(Tools::formatVector3(emitter->getVelocity())));
			ppeVelocityRnd->getController()->setValue(MutableString(Tools::formatVector3(emitter->getVelocityRnd())));
			ppeColorStart->getController()->setValue(MutableString(Tools::formatColor4(emitter->getColorStart())));
			ppeColorEnd->getController()->setValue(MutableString(Tools::formatColor4(emitter->getColorEnd())));
		} else
		if (v == PrototypeParticleSystem_Emitter::BOUNDINGBOX_PARTICLE_EMITTER) {
			particleSystemEmitters->getController()->setValue(MutableString(EMITTER_BOUNDINGBOXPARTICLEEMITTER));
			particleSystemEmitter->getActiveConditions().add(EMITTER_BOUNDINGBOXPARTICLEEMITTER);
			auto emitter = particleSystem->getBoundingBoxParticleEmitters();
			bbpeCount->getController()->setValue(MutableString(emitter->getCount()));
			bbpeLifeTime->getController()->setValue(MutableString(static_cast<int>(emitter->getLifeTime())));
			bbpeLifeTimeRnd->getController()->setValue(MutableString(static_cast<int>(emitter->getLifeTimeRnd())));
			bbpeMass->getController()->setValue(MutableString(emitter->getMass(), 4));
			bbpeMassRnd->getController()->setValue(MutableString(emitter->getMassRnd(), 4));
			bbpeVelocity->getController()->setValue(MutableString(Tools::formatVector3(emitter->getVelocity())));
			bbpeVelocityRnd->getController()->setValue(MutableString(Tools::formatVector3(emitter->getVelocityRnd())));
			bbpeColorStart->getController()->setValue(MutableString(Tools::formatColor4(emitter->getColorStart())));
			bbpeColorEnd->getController()->setValue(MutableString(Tools::formatColor4(emitter->getColorEnd())));
			bbpeObbCenter->getController()->setValue(MutableString(Tools::formatVector3(emitter->getObbCenter())));
			bbpeObbHalfextension->getController()->setValue(MutableString(Tools::formatVector3(emitter->getObbHalfextension())));
			Vector3 rotation;
			Matrix4x4 rotationMatrix;
			rotationMatrix.identity();
			rotationMatrix.setAxes(emitter->getObbAxis0(), emitter->getObbAxis1(), emitter->getObbAxis2());
			rotationMatrix.computeEulerAngles(rotation);
			bbpeObbRotationX->getController()->setValue(MutableString(Tools::formatFloat(rotation.getX())));
			bbpeObbRotationY->getController()->setValue(MutableString(Tools::formatFloat(rotation.getY())));
			bbpeObbRotationZ->getController()->setValue(MutableString(Tools::formatFloat(rotation.getZ())));
		} else
		if (v == PrototypeParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER) {
			particleSystemEmitters->getController()->setValue(MutableString(EMITTER_CIRCLEPARTICLEEMITTER));
			particleSystemEmitter->getActiveConditions().add(EMITTER_CIRCLEPARTICLEEMITTER);
			auto emitter = particleSystem->getCircleParticleEmitter();
			cpeCount->getController()->setValue(MutableString(emitter->getCount()));
			cpeLifeTime->getController()->setValue(MutableString(static_cast<int>(emitter->getLifeTime())));
			cpeLifeTimeRnd->getController()->setValue(MutableString(static_cast<int>(emitter->getLifeTimeRnd())));
			cpeMass->getController()->setValue(MutableString(emitter->getMass(), 4));
			cpeMassRnd->getController()->setValue(MutableString(emitter->getMassRnd(), 4));
			cpeVelocity->getController()->setValue(MutableString(Tools::formatVector3(emitter->getVelocity())));
			cpeVelocityRnd->getController()->setValue(MutableString(Tools::formatVector3(emitter->getVelocityRnd())));
			cpeColorStart->getController()->setValue(MutableString(Tools::formatColor4(emitter->getColorStart())));
			cpeColorEnd->getController()->setValue(MutableString(Tools::formatColor4(emitter->getColorEnd())));
			cpeCenter->getController()->setValue(MutableString(Tools::formatVector3(emitter->getCenter())));
			cpeRadius->getController()->setValue(MutableString(emitter->getRadius(), 4));
			Vector3 tmpAxis;
			Vector3 rotation;
			Matrix4x4 rotationMatrix;
			rotationMatrix.identity();
			rotationMatrix.setAxes(emitter->getAxis0(), Vector3::computeCrossProduct(emitter->getAxis0(), emitter->getAxis1(), tmpAxis), emitter->getAxis1());
			rotationMatrix.computeEulerAngles(rotation);
			cpeRotationX->getController()->setValue(MutableString(Tools::formatFloat(rotation.getX())));
			cpeRotationY->getController()->setValue(MutableString(Tools::formatFloat(rotation.getY())));
			cpeRotationZ->getController()->setValue(MutableString(Tools::formatFloat(rotation.getZ())));
		} else
		if (v == PrototypeParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER_PLANE_VELOCITY) {
			particleSystemEmitters->getController()->setValue(MutableString(EMITTER_CIRCLEPARTICLEEMITTERPLANEVELOCITY));
			particleSystemEmitter->getActiveConditions().add(EMITTER_CIRCLEPARTICLEEMITTERPLANEVELOCITY);
			auto emitter = particleSystem->getCircleParticleEmitterPlaneVelocity();
			cpepvCount->getController()->setValue(MutableString(emitter->getCount()));
			cpepvLifeTime->getController()->setValue(MutableString(static_cast<int>(emitter->getLifeTime())));
			cpepvLifeTimeRnd->getController()->setValue(MutableString(static_cast<int>(emitter->getLifeTimeRnd())));
			cpepvMass->getController()->setValue(MutableString(emitter->getMass(), 4));
			cpepvMassRnd->getController()->setValue(MutableString(emitter->getMassRnd(), 4));
			cpepvVelocity->getController()->setValue(MutableString(emitter->getVelocity(), 4));
			cpepvVelocityRnd->getController()->setValue(MutableString(emitter->getVelocityRnd(), 4));
			cpepvColorStart->getController()->setValue(MutableString(Tools::formatColor4(emitter->getColorStart())));
			cpepvColorEnd->getController()->setValue(MutableString(Tools::formatColor4(emitter->getColorEnd())));
			cpepvCenter->getController()->setValue(MutableString(Tools::formatVector3(emitter->getCenter())));
			cpepvRadius->getController()->setValue(MutableString(emitter->getRadius(), 4));
			Vector3 tmpAxis;
			Vector3 rotation;
			Matrix4x4 rotationMatrix;
			rotationMatrix.identity();
			rotationMatrix.setAxes(emitter->getAxis0(), Vector3::computeCrossProduct(emitter->getAxis0(), emitter->getAxis1(), tmpAxis), emitter->getAxis1());
			rotationMatrix.computeEulerAngles(rotation);
			cpepvRotationX->getController()->setValue(MutableString(Tools::formatFloat(rotation.getX())));
			cpepvRotationY->getController()->setValue(MutableString(Tools::formatFloat(rotation.getY())));
			cpepvRotationZ->getController()->setValue(MutableString(Tools::formatFloat(rotation.getZ())));
		} else
		if (v == PrototypeParticleSystem_Emitter::SPHERE_PARTICLE_EMITTER) {
			particleSystemEmitters->getController()->setValue(MutableString(EMITTER_SPHEREPARTICLEEMITTER));
			particleSystemEmitter->getActiveConditions().add(EMITTER_SPHEREPARTICLEEMITTER);
			auto emitter = particleSystem->getSphereParticleEmitter();
			speCount->getController()->setValue(MutableString(emitter->getCount()));
			speLifeTime->getController()->setValue(MutableString(static_cast<int>(emitter->getLifeTime())));
			speLifeTimeRnd->getController()->setValue(MutableString(static_cast<int>(emitter->getLifeTimeRnd())));
			speMass->getController()->setValue(MutableString(emitter->getMass(), 4));
			speMassRnd->getController()->setValue(MutableString(emitter->getMassRnd(), 4));
			speVelocity->getController()->setValue(MutableString(Tools::formatVector3(emitter->getVelocity())));
			speVelocityRnd->getController()->setValue(MutableString(Tools::formatVector3(emitter->getVelocityRnd())));
			speColorStart->getController()->setValue(MutableString(Tools::formatColor4(emitter->getColorStart())));
			speColorEnd->getController()->setValue(MutableString(Tools::formatColor4(emitter->getColorEnd())));
			speCenter->getController()->setValue(MutableString(Tools::formatVector3(emitter->getCenter())));
			speRadius->getController()->setValue(MutableString(emitter->getRadius(), 4));
		} else {
			Console::println(
				string(
					"ParticleSystemScreenController::onParticleSystemEmitterApply(): unknown particle system emitter '" +
					particleSystem->getEmitter()->getName() +
					"'"
				)
			);
		}
	}
}

void ParticleSystemScreenController::unsetParticleSystemListBox() {
	particleSystemsListbox->getController()->setDisabled(true);
	particleSystemAddButton->getController()->setDisabled(false);
	particleSystemRemoveButton->getController()->setDisabled(true);
}

void ParticleSystemScreenController::setParticleSystemListBox(int count, int selectionIdx) {
	particleSystemsListbox->getController()->setDisabled(false);
	particleSystemAddButton->getController()->setDisabled(false);
	particleSystemRemoveButton->getController()->setDisabled(false);
	auto particleSystemListBoxInnerNode = dynamic_cast< GUIParentNode* >((particleSystemsListbox->getScreenNode()->getNodeById(particleSystemsListbox->getId() + "_inner")));
	auto idx = 0;
	string particleSystemListBoxInnerNodeSubNodesXML = "";
	particleSystemListBoxInnerNodeSubNodesXML =
		particleSystemListBoxInnerNodeSubNodesXML +
		"<scrollarea id=\"" +
		particleSystemsListbox->getId() +
		"_inner_scrollarea\" width=\"100%\" height=\"100%\">\n";
	for (auto i = 0; i < count; i++) {
		particleSystemListBoxInnerNodeSubNodesXML =
			particleSystemListBoxInnerNodeSubNodesXML +
			"<selectbox-option text=\"" +
			GUIParser::escapeQuotes("Particle System " + to_string(i + 1)) +
			"\" value=\"" +
			GUIParser::escapeQuotes(to_string(i)) +
			"\" " +
			(idx == selectionIdx ? "selected=\"true\" " : "") +
			" />\n";
		idx++;
	}
	particleSystemListBoxInnerNodeSubNodesXML =
		particleSystemListBoxInnerNodeSubNodesXML +
		"</scrollarea>";
	try {
		particleSystemListBoxInnerNode->replaceSubNodes(particleSystemListBoxInnerNodeSubNodesXML, true);
	} catch (Exception& exception) {
		Console::print(string("ParticleSystemScreenController::setParticleSystemListBox: An error occurred: "));
		Console::println(string(exception.what()));
	}
}


void ParticleSystemScreenController::onParticleSystemLoad()
{
	class OnParticleSystemLoad: public virtual Action
	{
	public:
		void performAction() override {
			particleSystemScreenController->view->loadFile(particleSystemScreenController->view->getPopUps()->getFileDialogScreenController()->getPathName(), particleSystemScreenController->view->getPopUps()->getFileDialogScreenController()->getFileName());
			particleSystemScreenController->particleSystemPath.setPath(particleSystemScreenController->view->getPopUps()->getFileDialogScreenController()->getPathName());
			particleSystemScreenController->view->getPopUps()->getFileDialogScreenController()->close();
		}

		/**
		 * Public constructor
		 * @param particleSystemScreenController particle system screen controller
		 */
		OnParticleSystemLoad(ParticleSystemScreenController* particleSystemScreenController): particleSystemScreenController(particleSystemScreenController) {
		}

	private:
		ParticleSystemScreenController* particleSystemScreenController;
	};

	vector<string> extensions = {
		"tps"
	};
	view->getPopUps()->getFileDialogScreenController()->show(
		particleSystemPath.getPath(),
		"Load from: ",
		extensions,
		view->getFileName(),
		true,
		new OnParticleSystemLoad(this)
	);
}

void ParticleSystemScreenController::onPrototypeSave()
{
	class OnEntitySave: public virtual Action
	{
	public:
		void performAction() override {
			try {
				particleSystemScreenController->view->saveFile(particleSystemScreenController->view->getPopUps()->getFileDialogScreenController()->getPathName(), particleSystemScreenController->view->getPopUps()->getFileDialogScreenController()->getFileName());
				particleSystemScreenController->particleSystemPath.setPath(particleSystemScreenController->view->getPopUps()->getFileDialogScreenController()->getPathName());
				particleSystemScreenController->view->getPopUps()->getFileDialogScreenController()->close();
			} catch (Exception& exception) {
				particleSystemScreenController->showErrorPopUp("Warning", (exception.what()));
			}
		}

		/**
		 * Public constructor
		 * @param particleSystemScreenController particle system screen controller
		 */
		OnEntitySave(ParticleSystemScreenController* particleSystemScreenController): particleSystemScreenController(particleSystemScreenController) {
		}

	private:
		ParticleSystemScreenController* particleSystemScreenController;
	};

	auto fileName = view->getPrototype()->getFileName();
	if (fileName.length() == 0) {
		fileName = "untitle.tps";
	}
	fileName = Tools::getFileName(fileName);
	vector<string> extensions = {
		"tps"
	};
	view->getPopUps()->getFileDialogScreenController()->show(
		particleSystemPath.getPath(),
		"Save to: ",
		extensions,
		fileName,
		false,
		new OnEntitySave(this)
);
}

void ParticleSystemScreenController::onParticleSystemReload()
{
	view->reloadFile();
}

void ParticleSystemScreenController::saveParticleSystem(const string& pathName, const string& fileName)
{
	view->saveFile(pathName, fileName);
}

void ParticleSystemScreenController::loadParticleSystem(const string& pathName, const string& fileName)
{
	view->loadFile(pathName, fileName);
}

void ParticleSystemScreenController::showErrorPopUp(const string& caption, const string& message)
{
	view->getPopUps()->getInfoDialogScreenController()->show(caption, message);
}

void ParticleSystemScreenController::onValueChanged(GUIElementNode* node)
{
	if (node->getId() == "particlesystems_listbox") {
		view->setParticleSystemIndex(Integer::parseInt(particleSystemsListbox->getController()->getValue().getString()));
	} else {
		prototypeBaseSubScreenController->onValueChanged(node, view->getPrototype());
		prototypePhysicsSubScreenController->onValueChanged(node, view->getPrototype());
		prototypeSoundsSubScreenController->onValueChanged(node, view->getPrototype());
	}
}

void ParticleSystemScreenController::onActionPerformed(GUIActionListenerType type, GUIElementNode* node)
{
	prototypeBaseSubScreenController->onActionPerformed(type, node, view->getPrototype());
	prototypeDisplaySubScreenController->onActionPerformed(type, node);
	prototypePhysicsSubScreenController->onActionPerformed(type, node, view->getPrototype());
	prototypeSoundsSubScreenController->onActionPerformed(type, node, view->getPrototype());
	if (type == GUIActionListenerType::PERFORMED) {
		if (node->getId().compare("button_entity_load") == 0) {
			onParticleSystemLoad();
		} else
		if (node->getId().compare("button_entity_reload") == 0) {
			onParticleSystemReload();
		} else
		if (node->getId().compare("button_entity_save") == 0) {
			onPrototypeSave();
		} else
		if (node->getId().compare("button_ps_type_apply") == 0) {
			view->reset();
			onParticleSystemTypeApply();
			view->updateParticleSystemRequest();
		} else
		if (node->getId().compare("button_ops_apply") == 0 ||
			node->getId().compare("button_pps_type_apply") == 0 ||
			node->getId().compare("button_fps_type_apply") == 0) {
			view->reset();
			onParticleSystemTypeDataApply();
			view->updateParticleSystemRequest();
		} else
		if (node->getId().compare("button_emitter_apply") == 0) {
			view->reset();
			onParticleSystemEmitterApply();
			view->updateParticleSystemRequest();
		} else
		if (node->getId().compare("button_ppe_emitter_apply") == 0 ||
			node->getId().compare("button_bbpe_emitter_apply") == 0 ||
			node->getId().compare("button_cpe_emitter_apply") == 0 ||
			node->getId().compare("button_cpepv_emitter_apply") == 0 ||
			node->getId().compare("button_spe_emitter_apply") == 0) {
			view->reset();
			onParticleSystemEmitterDataApply();
			view->updateParticleSystemRequest();
		} else
		if (node->getId().compare("button_ops_model_file") == 0) {
			class OnLoadModelFile: public virtual Action
			{
			public:
				void performAction() override {
					particleSystemScreenController->opsModel->getController()->setValue(MutableString(particleSystemScreenController->view->getPopUps()->getFileDialogScreenController()->getPathName() + "/" + particleSystemScreenController->view->getPopUps()->getFileDialogScreenController()->getFileName()));
					particleSystemScreenController->modelPath.setPath(particleSystemScreenController->view->getPopUps()->getFileDialogScreenController()->getPathName());
					particleSystemScreenController->view->getPopUps()->getFileDialogScreenController()->close();
				}

				/**
				 * Public constructor
				 * @param particleSystemScreenController particle system screen controller
				 */
				OnLoadModelFile(ParticleSystemScreenController* particleSystemScreenController): particleSystemScreenController(particleSystemScreenController) {
				}

			private:
				ParticleSystemScreenController* particleSystemScreenController;
			};

			vector<string> extensions = ModelReader::getModelExtensions();
			view->getPopUps()->getFileDialogScreenController()->show(
				modelPath.getPath(),
				"Load from: ",
				extensions,
				opsModel->getController()->getValue().getString(),
				true,
				new OnLoadModelFile(this)
			);
		} else
		if (node->getId().compare("button_pps_texture_file") == 0) {
			class OnLoadTextureFile: public virtual Action
			{
			public:
				void performAction() override {
					particleSystemScreenController->ppsTexture->getController()->setValue(MutableString(particleSystemScreenController->view->getPopUps()->getFileDialogScreenController()->getPathName() + "/" + particleSystemScreenController->view->getPopUps()->getFileDialogScreenController()->getFileName()));
					particleSystemScreenController->modelPath.setPath(particleSystemScreenController->view->getPopUps()->getFileDialogScreenController()->getPathName());
					particleSystemScreenController->view->getPopUps()->getFileDialogScreenController()->close();
				}

				/**
				 * Public constructor
				 * @param particleSystemScreenController particle system screen controller
				 */
				OnLoadTextureFile(ParticleSystemScreenController* particleSystemScreenController): particleSystemScreenController(particleSystemScreenController) {
				}

			private:
				ParticleSystemScreenController* particleSystemScreenController;
			};

			vector<string> extensions = TextureReader::getTextureExtensions();
			view->getPopUps()->getFileDialogScreenController()->show(
				modelPath.getPath(),
				"Load from: ",
				extensions,
				ppsTexture->getController()->getValue().getString(),
				true,
				new OnLoadTextureFile(this)
			);
		} else
		if (node->getId().compare("button_pps_transparency_texture_file") == 0) {
			class OnLoadTextureFile: public virtual Action
			{
			public:
				void performAction() override {
					particleSystemScreenController->ppsTransparencyTexture->getController()->setValue(MutableString(particleSystemScreenController->view->getPopUps()->getFileDialogScreenController()->getPathName() + "/" + particleSystemScreenController->view->getPopUps()->getFileDialogScreenController()->getFileName()));
					particleSystemScreenController->modelPath.setPath(particleSystemScreenController->view->getPopUps()->getFileDialogScreenController()->getPathName());
					particleSystemScreenController->view->getPopUps()->getFileDialogScreenController()->close();
				}

				/**
				 * Public constructor
				 * @param particleSystemScreenController particle system screen controller
				 */
				OnLoadTextureFile(ParticleSystemScreenController* particleSystemScreenController): particleSystemScreenController(particleSystemScreenController) {
				}

			private:
				ParticleSystemScreenController* particleSystemScreenController;
			};

			vector<string> extensions = TextureReader::getTextureExtensions();
			view->getPopUps()->getFileDialogScreenController()->show(
				modelPath.getPath(),
				"Load from: ",
				extensions,
				ppsTransparencyTexture->getController()->getValue().getString(),
				true,
				new OnLoadTextureFile(this)
			);
		} else
		if (node->getId().compare("button_pps_transparency_texture_clear") == 0) {
			ppsTransparencyTexture->getController()->setValue(MutableString());
		} else
		if (node->getId().compare("button_fps_texture_file") == 0) {
			class OnLoadTextureFile: public virtual Action
			{
			public:
				void performAction() override {
					particleSystemScreenController->fpsTexture->getController()->setValue(MutableString(particleSystemScreenController->view->getPopUps()->getFileDialogScreenController()->getPathName() + "/" + particleSystemScreenController->view->getPopUps()->getFileDialogScreenController()->getFileName()));
					particleSystemScreenController->modelPath.setPath(particleSystemScreenController->view->getPopUps()->getFileDialogScreenController()->getPathName());
					particleSystemScreenController->view->getPopUps()->getFileDialogScreenController()->close();
				}

				/**
				 * Public constructor
				 * @param particleSystemScreenController particle system screen controller
				 */
				OnLoadTextureFile(ParticleSystemScreenController* particleSystemScreenController): particleSystemScreenController(particleSystemScreenController) {
				}

			private:
				ParticleSystemScreenController* particleSystemScreenController;
			};

			vector<string> extensions = TextureReader::getTextureExtensions();
			view->getPopUps()->getFileDialogScreenController()->show(
				modelPath.getPath(),
				"Load from: ",
				extensions,
				ppsTexture->getController()->getValue().getString(),
				true,
				new OnLoadTextureFile(this)
			);
		} else
		if (node->getId().compare("button_fps_transparency_texture_file") == 0) {
			class OnLoadTextureFile: public virtual Action
			{
			public:
				void performAction() override {
					particleSystemScreenController->fpsTransparencyTexture->getController()->setValue(MutableString(particleSystemScreenController->view->getPopUps()->getFileDialogScreenController()->getPathName() + "/" + particleSystemScreenController->view->getPopUps()->getFileDialogScreenController()->getFileName()));
					particleSystemScreenController->modelPath.setPath(particleSystemScreenController->view->getPopUps()->getFileDialogScreenController()->getPathName());
					particleSystemScreenController->view->getPopUps()->getFileDialogScreenController()->close();
				}

				/**
				 * Public constructor
				 * @param particleSystemScreenController particle system screen controller
				 */
				OnLoadTextureFile(ParticleSystemScreenController* particleSystemScreenController): particleSystemScreenController(particleSystemScreenController) {
				}

			private:
				ParticleSystemScreenController* particleSystemScreenController;
			};

			vector<string> extensions = TextureReader::getTextureExtensions();
			view->getPopUps()->getFileDialogScreenController()->show(
				modelPath.getPath(),
				"Load from: ",
				extensions,
				ppsTransparencyTexture->getController()->getValue().getString(),
				true,
				new OnLoadTextureFile(this)
			);
		} else
		if (node->getId().compare("button_fps_transparency_texture_clear") == 0) {
			fpsTransparencyTexture->getController()->setValue(MutableString());
		} else
		if (node->getId() == "button_particlesystem_add") {
			view->getPrototype()->addParticleSystem();
			view->setParticleSystemIndex(view->getPrototype()->getParticleSystemsCount() - 1);
		} else
		if (node->getId() == "button_particlesystem_remove") {
			view->getPrototype()->removeParticleSystemAt(view->getParticleSystemIndex());
			view->setParticleSystemIndex(view->getParticleSystemIndex() - 1);
		}
	}
}

void ParticleSystemScreenController::getViewPort(int& left, int& top, int& width, int& height) {
	auto& constraints = viewPort->getComputedConstraints();
	left = constraints.left + constraints.alignmentLeft + constraints.contentAlignmentLeft;
	top = constraints.top + constraints.alignmentTop + constraints.contentAlignmentTop;
	width = constraints.width;
	height = constraints.height;
}
