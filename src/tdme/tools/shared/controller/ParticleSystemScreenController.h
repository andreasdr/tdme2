
#pragma once

#include <vector>
#include <string>

#include <tdme/tdme.h>
#include <tdme/gui/events/fwd-tdme.h>
#include <tdme/gui/events/GUIActionListener.h>
#include <tdme/gui/events/GUIChangeListener.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/math/Vector3.h>
#include <tdme/tools/shared/controller/fwd-tdme.h>
#include <tdme/tools/shared/model/fwd-tdme.h>
#include <tdme/tools/shared/views/fwd-tdme.h>
#include <tdme/tools/shared/controller/fwd-tdme.h>
#include <tdme/tools/shared/controller/ScreenController.h>

using std::vector;
using std::string;

using tdme::tools::shared::controller::ScreenController;
using tdme::gui::events::GUIActionListener;
using tdme::gui::events::GUIChangeListener;
using tdme::gui::events::GUIActionListenerType;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::nodes::GUITextNode;
using tdme::math::Vector3;
using tdme::tools::shared::controller::EntityBaseSubScreenController;
using tdme::tools::shared::controller::EntityPhysicsSubScreenController;
using tdme::tools::shared::controller::EntityDisplaySubScreenController;
using tdme::tools::shared::controller::EntitySoundsSubScreenController;
using tdme::tools::shared::controller::FileDialogPath;
using tdme::tools::shared::model::LevelEditorEntity;
using tdme::tools::shared::views::SharedParticleSystemView;

/**
 * Particle system screen controller
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::shared::controller::ParticleSystemScreenController final
	: public ScreenController
	, public GUIActionListener
	, public GUIChangeListener
{

private:
	static string TYPE_NONE;
	static string TYPE_OBJECTPARTICLESYSTEM;
	static string TYPE_POINTSPARTICLESYSTEM;
	static string TYPE_FOGPARTICLESYSTEM;
	static string EMITTER_NONE;
	static string EMITTER_POINTPARTICLEEMITTER;
	static string EMITTER_BOUNDINGBOXPARTICLEEMITTER;
	static string EMITTER_CIRCLEPARTICLEEMITTER;
	static string EMITTER_CIRCLEPARTICLEEMITTERPLANEVELOCITY;
	static string EMITTER_SPHEREPARTICLEEMITTER;
	EntityBaseSubScreenController* entityBaseSubScreenController { nullptr };
	EntityDisplaySubScreenController* entityDisplaySubScreenController { nullptr };
	EntityPhysicsSubScreenController* entityPhysicsSubScreenController { nullptr };
	EntitySoundsSubScreenController* entitySoundsSubScreenController { nullptr };
	SharedParticleSystemView* view { nullptr };
	GUIScreenNode* screenNode { nullptr };
	GUITextNode* screenCaption { nullptr };
	GUIElementNode* particleSystemReload { nullptr };
	GUIElementNode* particleSystemSave { nullptr };
	GUIElementNode* particleSystemTypes { nullptr };
	GUIElementNode* particleSystemTypeApply { nullptr };
	GUIElementNode* particleSystemType { nullptr };
	GUIElementNode* particleSystemEmitters { nullptr };
	GUIElementNode* particleSystemEmitter { nullptr };
	GUIElementNode* particleSystemEmitterApply { nullptr };
	GUIElementNode* opsScale { nullptr };
	GUIElementNode* opsMaxCount { nullptr };
	GUIElementNode* opsModel { nullptr };
	GUIElementNode* opsAutoEmit { nullptr };
	GUIElementNode* ppsAutoEmit { nullptr };
	GUIElementNode* ppsMaxPoints { nullptr };
	GUIElementNode* ppsPointSize { nullptr };
	GUIElementNode* ppsTexture { nullptr };
	GUIElementNode* ppsTransparencyTexture { nullptr };
	GUIElementNode* ppsHorizontalSprites { nullptr };
	GUIElementNode* ppsVerticalSprites { nullptr };
	GUIElementNode* ppsFPS { nullptr };
	GUIElementNode* fpsMaxPoints { nullptr };
	GUIElementNode* fpsPointSize { nullptr };
	GUIElementNode* fpsTexture { nullptr };
	GUIElementNode* fpsTransparencyTexture { nullptr };
	GUIElementNode* fpsHorizontalSprites { nullptr };
	GUIElementNode* fpsVerticalSprites { nullptr };
	GUIElementNode* fpsFPS { nullptr };
	GUIElementNode* ppeCount { nullptr };
	GUIElementNode* ppeLifeTime { nullptr };
	GUIElementNode* ppeLifeTimeRnd { nullptr };
	GUIElementNode* ppeMass { nullptr };
	GUIElementNode* ppeMassRnd { nullptr };
	GUIElementNode* ppePosition { nullptr };
	GUIElementNode* ppeVelocity { nullptr };
	GUIElementNode* ppeVelocityRnd { nullptr };
	GUIElementNode* ppeColorStart { nullptr };
	GUIElementNode* ppeColorEnd { nullptr };
	GUIElementNode* bbpeCount { nullptr };
	GUIElementNode* bbpeLifeTime { nullptr };
	GUIElementNode* bbpeLifeTimeRnd { nullptr };
	GUIElementNode* bbpeMass { nullptr };
	GUIElementNode* bbpeMassRnd { nullptr };
	GUIElementNode* bbpeVelocity { nullptr };
	GUIElementNode* bbpeVelocityRnd { nullptr };
	GUIElementNode* bbpeColorStart { nullptr };
	GUIElementNode* bbpeColorEnd { nullptr };
	GUIElementNode* bbpeObbCenter { nullptr };
	GUIElementNode* bbpeObbHalfextension { nullptr };
	GUIElementNode* bbpeObbRotationX { nullptr };
	GUIElementNode* bbpeObbRotationY { nullptr };
	GUIElementNode* bbpeObbRotationZ { nullptr };
	GUIElementNode* cpeCount { nullptr };
	GUIElementNode* cpeLifeTime { nullptr };
	GUIElementNode* cpeLifeTimeRnd { nullptr };
	GUIElementNode* cpeMass { nullptr };
	GUIElementNode* cpeMassRnd { nullptr };
	GUIElementNode* cpeVelocity { nullptr };
	GUIElementNode* cpeVelocityRnd { nullptr };
	GUIElementNode* cpeColorStart { nullptr };
	GUIElementNode* cpeColorEnd { nullptr };
	GUIElementNode* cpeCenter { nullptr };
	GUIElementNode* cpeRadius { nullptr };
	GUIElementNode* cpeRotationX { nullptr };
	GUIElementNode* cpeRotationY { nullptr };
	GUIElementNode* cpeRotationZ { nullptr };
	GUIElementNode* cpepvCount { nullptr };
	GUIElementNode* cpepvLifeTime { nullptr };
	GUIElementNode* cpepvLifeTimeRnd { nullptr };
	GUIElementNode* cpepvMass { nullptr };
	GUIElementNode* cpepvMassRnd { nullptr };
	GUIElementNode* cpepvVelocity { nullptr };
	GUIElementNode* cpepvVelocityRnd { nullptr };
	GUIElementNode* cpepvColorStart { nullptr };
	GUIElementNode* cpepvColorEnd { nullptr };
	GUIElementNode* cpepvCenter { nullptr };
	GUIElementNode* cpepvRadius { nullptr };
	GUIElementNode* cpepvRotationX { nullptr };
	GUIElementNode* cpepvRotationY { nullptr };
	GUIElementNode* cpepvRotationZ { nullptr };
	GUIElementNode* speCount { nullptr };
	GUIElementNode* speLifeTime { nullptr };
	GUIElementNode* speLifeTimeRnd { nullptr };
	GUIElementNode* speMass { nullptr };
	GUIElementNode* speMassRnd { nullptr };
	GUIElementNode* speVelocity { nullptr };
	GUIElementNode* speVelocityRnd { nullptr };
	GUIElementNode* speColorStart { nullptr };
	GUIElementNode* speColorEnd { nullptr };
	GUIElementNode* speCenter { nullptr };
	GUIElementNode* speRadius { nullptr };
	GUIElementNode* particleSystemsListbox { nullptr };
	GUIElementNode* particleSystemAddButton { nullptr };
	GUIElementNode* particleSystemRemoveButton { nullptr };
	GUIElementNode* viewPort { nullptr };

	FileDialogPath* particleSystemPath { nullptr };
	FileDialogPath* modelPath { nullptr };
	FileDialogPath* audioPath { nullptr };

public:
	/**
	 * Public constructor
	 * @param view view
	 */
	ParticleSystemScreenController(SharedParticleSystemView* view);

	/**
	 * Destructor
	 */
	virtual ~ParticleSystemScreenController();

	/**
	 * @return entity display sub screen controller
	 */
	EntityDisplaySubScreenController* getEntityDisplaySubScreenController();

	/**
	 * @return entity physics sub screen controller
	 */
	EntityPhysicsSubScreenController* getEntityPhysicsSubScreenController();


	/**
	 * @return entity sounds sub screen controller
	 */
	EntitySoundsSubScreenController* getEntitySoundsSubScreenController();

	// overridden methods
	GUIScreenNode* getScreenNode() override;

	/**
	 * @return particle system path
	 */
	FileDialogPath* getParticleSystemPath();

	/**
	 * @return model path
	 */
	FileDialogPath* getModelPath();
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
	 * Set up particle system types
	 * @param particleSystemTypesCollection particle system types
	 */
	void setParticleSystemTypes(const vector<string>& particleSystemTypesCollection);

	/**
	 * Update point particle system emitter
	 * @param position position
	 */
	void updatePointParticleSystemEmitter(const Vector3& position);

	/**
	 * Update bounding box particle system emitter
	 * @param center center
	 * @param axis0 axis 0
	 * @param axis1 axis 1
	 * @param axis2 axis 2
	 * @param halfExtension half extension
	 */
	void updateBoundingBoxParticleSystemEmitter(const Vector3& center, const Vector3& axis0, const Vector3& axis1, const Vector3& axis2, const Vector3& halfExtension);

	/**
	 * Update circle particle system emitter
	 * @param center center
	 * @param axis0 axis 0
	 * @param axis1 axis 1
	 * @param radius radius
	 */
	void updateCircleParticleSystemEmitter(const Vector3& center, const Vector3& axis0, const Vector3& axis1, float radius);

	/**
	 * Update circle particle plane velocity system emitter
	 * @param center center
	 * @param axis0 axis 0
	 * @param axis1 axis 1
	 * @param radius radius
	 */
	void updateCirclePlaneVelocityParticleSystemEmitter(const Vector3& center, const Vector3& axis0, const Vector3& axis1, float radius);

	/**
	 * Update sphere particle system emitter
	 * @param center center
	 * @param radius radius
	 */
	void updateSphereParticleSystemEmitter(const Vector3& center, float radius);

	/**
	 * Set up emitters
	 * @param emittersCollection emitters
	 */
	void setParticleSystemEmitters(const vector<string>& emittersCollection);

	/**
	 * On quit
	 */
	void onQuit();

	/**
	 * Unset particle system type
	 */
	void unsetParticleSystemType();

	/**
	 * Set particle system type
	 */
	void setParticleSystemType();

	/**
	 * Set particle system type
	 */
	void onParticleSystemTypeDataApply();

	/**
	 * On particle system type apply
	 */
	void onParticleSystemTypeApply();

	/**
	 * On particle system emitter apply
	 */
	void onParticleSystemEmitterApply();

	/**
	 * Set particle system emitter
	 */
	void onParticleSystemEmitterDataApply();

	/**
	 * Unset particle system emitter
	 */
	void unsetParticleSystemEmitter();

	/**
	 * Set particle system emitter
	 */
	void setParticleSystemEmitter();

	/**
	 * On particle system load
	 */
	void onParticleSystemLoad();

	/**
	 * On model save
	 */
	void onEntitySave();

	/**
	 * Unset particle system list box
	 */
	void unsetParticleSystemListBox();

	/**
	 * Set particle system list box
	 * @param count count
	 * @param selectionIdx selection index
	 */
	void setParticleSystemListBox(int count, int selectionIdx);

	/**
	 * On particle system reload
	 */
	void onParticleSystemReload();
	void saveFile(const string& pathName, const string& fileName) /* throws(Exception) */;
	void loadFile(const string& pathName, const string& fileName) /* throws(Exception) */;

	/**
	 * Shows the error pop up
	 */
	void showErrorPopUp(const string& caption, const string& message);
	void onValueChanged(GUIElementNode* node) override;
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
