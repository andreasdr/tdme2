#pragma once

#include <array>
#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/gui/events/fwd-tdme.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/tools/shared/controller/fwd-tdme.h>
#include <tdme/tools/shared/model/fwd-tdme.h>
#include <tdme/tools/shared/model/LevelEditorEntity.h>
#include <tdme/tools/shared/views/fwd-tdme.h>
#include <tdme/utilities/fwd-tdme.h>

using std::array;
using std::vector;
using std::string;

using tdme::gui::events::GUIActionListener_Type;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUIParentNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::math::Vector3;
using tdme::tools::shared::controller::EntityPhysicsSubScreenController_BoundingVolumeType;
using tdme::tools::shared::controller::FileDialogPath;
using tdme::tools::shared::model::LevelEditorEntity;
using tdme::tools::shared::views::EntityPhysicsView;
using tdme::tools::shared::views::PopUps;
using tdme::utilities::MutableString;

/** 
 * Entity physics sub screen controller
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::shared::controller::EntityPhysicsSubScreenController
{
	friend class EntityPhysicsSubScreenController_BoundingVolumeType;
	friend class EntityPhysicsSubScreenController_GenerateConvexMeshes;
	friend class EntityPhysicsSubScreenController_onBoundingVolumeConvexMeshesFile;
	friend class EntityPhysicsSubScreenController_onBoundingVolumeConvexMeshFile;

private:
	GUIScreenNode* screenNode { nullptr };
	FileDialogPath* modelPath { nullptr };
	EntityPhysicsView* view { nullptr };
	array<GUIElementNode*, LevelEditorEntity::MODEL_BOUNDINGVOLUME_COUNT> boundingVolumeTypeDropDown;
	array<GUIElementNode*, LevelEditorEntity::MODEL_BOUNDINGVOLUME_COUNT> boundingVolumeApply;
	array<GUIElementNode*, LevelEditorEntity::MODEL_BOUNDINGVOLUME_COUNT> boundingVolume;
	array<GUIElementNode*, LevelEditorEntity::MODEL_BOUNDINGVOLUME_COUNT> boundingvolumeSphereCenter;
	array<GUIElementNode*, LevelEditorEntity::MODEL_BOUNDINGVOLUME_COUNT> boundingvolumeSphereRadius;
	array<GUIElementNode*, LevelEditorEntity::MODEL_BOUNDINGVOLUME_COUNT> boundingvolumeCapsuleA;
	array<GUIElementNode*, LevelEditorEntity::MODEL_BOUNDINGVOLUME_COUNT> boundingvolumeCapsuleB;
	array<GUIElementNode*, LevelEditorEntity::MODEL_BOUNDINGVOLUME_COUNT> boundingvolumeCapsuleRadius;
	array<GUIElementNode*, LevelEditorEntity::MODEL_BOUNDINGVOLUME_COUNT> boundingvolumeBoundingBoxMin;
	array<GUIElementNode*, LevelEditorEntity::MODEL_BOUNDINGVOLUME_COUNT> boundingvolumeBoundingBoxMax;
	array<GUIElementNode*, LevelEditorEntity::MODEL_BOUNDINGVOLUME_COUNT> boundingvolumeObbCenter;
	array<GUIElementNode*, LevelEditorEntity::MODEL_BOUNDINGVOLUME_COUNT> boundingvolumeObbHalfextension;
	array<GUIElementNode*, LevelEditorEntity::MODEL_BOUNDINGVOLUME_COUNT> boundingvolumeObbRotationX;
	array<GUIElementNode*, LevelEditorEntity::MODEL_BOUNDINGVOLUME_COUNT> boundingvolumeObbRotationY;
	array<GUIElementNode*, LevelEditorEntity::MODEL_BOUNDINGVOLUME_COUNT> boundingvolumeObbRotationZ;
	array<GUIElementNode*, LevelEditorEntity::MODEL_BOUNDINGVOLUME_COUNT> boundingvolumeConvexMeshFile;
	bool isModelBoundingVolumes;
	GUIElementNode* terrainMesh { nullptr };
	GUIElementNode* terrainMeshApply { nullptr };
	GUIElementNode* convexmeshesModeGenerate { nullptr };
	GUIElementNode* convexmeshesModeModel { nullptr };
	GUIElementNode* convexMeshesFile { nullptr };
	GUIElementNode* convexMeshesLoad { nullptr };
	GUIElementNode* convexMeshesResolution { nullptr };
	GUIElementNode* convexMeshesDepth { nullptr };
	GUIElementNode* convexMeshesConcavity { nullptr };
	GUIElementNode* convexMeshesPlaneDownSampling { nullptr };
	GUIElementNode* convexMeshesConvexHullDownSampling { nullptr };
	GUIElementNode* convexMeshesAlpha { nullptr };
	GUIElementNode* convexMeshesBeta { nullptr };
	GUIElementNode* convexMeshesMaxVerticesPerConvexHull { nullptr };
	GUIElementNode* convexMeshesMinVolumePerConvexHull { nullptr };
	GUIElementNode* convexMeshesPCA { nullptr };
	GUIElementNode* convexMeshesRemove { nullptr };
	GUIElementNode* convexMeshesGenerate { nullptr };
	GUIElementNode* bodyTypeDropdown { nullptr };
	GUIElementNode* bodyTypeDropdownApply { nullptr };
	GUIElementNode* bodyMass { nullptr };
	GUIElementNode* bodyBounciness { nullptr };
	GUIElementNode* bodyFriction { nullptr };
	GUIElementNode* bodyInertiaTensor { nullptr };
	GUIElementNode* bodyApply { nullptr };
	bool boundingVolumeTabActivated;
	int boundingVolumeIdxActivated;

public:

	/** 
	 * @return view
	 */
	virtual EntityPhysicsView* getView();

	/**
	 * @return screen node
	 */
	virtual GUIScreenNode* getScreenNode();

	/** 
	 * Init
	 * @param screenNode screen node
	 */
	virtual void initialize(GUIScreenNode* screenNode);

	/** 
	 * Disable bounding volume
	 */
	virtual void disableBoundingVolume(int32_t idx);

	/** 
	 * Enable bounding volume
	 * @param idx idx
	 */
	virtual void enableBoundingVolume(int32_t idx);

	/** 
	 * Set up model bounding volume type
	 * @param entity entity
	 * @param idx idx
	 */
	virtual void setupModelBoundingVolumeType(LevelEditorEntity* entity, int32_t idx);

	/** 
	 * Set up bounding volume types
	 * @param idx idx
	 */
	virtual void setupBoundingVolumeTypes(int32_t idx);

	/** 
	 * Display given bounding volume GUI elements
	 * @param idx idx
	 * @param bvType bounding volume type
	 */
	virtual void selectBoundingVolume(int32_t idx, EntityPhysicsSubScreenController_BoundingVolumeType* bvType);

	/** 
	 * Setup sphere bounding volume
	 * @param idx idx
	 * @param center center
	 * @param radius radius
	 */
	virtual void setupSphere(int32_t idx, const Vector3& center, float radius);

	/** 
	 * Setup capsule bounding volume
	 * @param idx idx
	 * @param a a
	 * @param b b
	 * @param radius radius
	 */
	virtual void setupCapsule(int32_t idx, const Vector3& a, const Vector3& b, float radius);

	/** 
	 * Setup AABB bounding volume
	 * @param idx idx
	 * @param min min
	 * @param max max
	 */
	virtual void setupBoundingBox(int32_t idx, const Vector3& min, const Vector3& max);

	/** 
	 * Setup oriented bounding box
	 * @param idx idx
	 * @param center center
	 * @param axis0 axis 0
	 * @param axis1 axis 1
	 * @param axis2 axis 2
	 * @param halfExtension half extension
	 */
	virtual void setupOrientedBoundingBox(int32_t idx, const Vector3& center, const Vector3& axis0, const Vector3& axis1, const Vector3& axis2, const Vector3& halfExtension);

	/** 
	 * Setup convex mesh bounding volume
	 * @param idx idx
	 * @param file file
	 */
	virtual void setupConvexMesh(int32_t idx, const string& file);

	/** 
	 * On pivot apply
	 * @param entity entity
	 * @param idx idx
	 */
	virtual void onBoundingVolumeTypeApply(LevelEditorEntity* entity, int32_t idx);

	/** 
	 * On bounding volume none apply
	 * @param entity entity
	 * @param idx idx
	 */
	virtual void onBoundingVolumeNoneApply(LevelEditorEntity* entity, int32_t idx);

	/** 
	 * On bounding volume sphere apply
	 * @param entity entity
	 * @param idx idx
	 */
	virtual void onBoundingVolumeSphereApply(LevelEditorEntity* entity, int32_t idx);

	/** 
	 * On bounding volume capsule apply
	 * @param entity entity
	 * @param idx idx
	 */
	virtual void onBoundingVolumeCapsuleApply(LevelEditorEntity* entity, int32_t idx);

	/** 
	 * On bounding volume AABB apply
	 * @param entity entity
	 * @param idx idx
	 */
	virtual void onBoundingVolumeAabbApply(LevelEditorEntity* entity, int32_t idx);

	/** 
	 * On bounding volume OBB apply
	 * @param entity entity
	 * @param idx idx
	 */
	virtual void onBoundingVolumeObbApply(LevelEditorEntity* entity, int32_t idx);

	/** 
	 * On bounding volume convex mesh apply
	 * @param entity entity
	 * @param idx idx
	 */
	virtual void onBoundingVolumeConvexMeshApply(LevelEditorEntity* entity, int32_t idx);

	/** 
	 * On bounding volume convex mesh file clicked
	 * @param entity entity
	 * @param idx idx
	 */
	virtual void onBoundingVolumeConvexMeshFile(LevelEditorEntity* entity, int32_t idx);

	/** 
	 * On bounding volume convex meshes file
	 * @param entity entity
	 */
	virtual void onBoundingVolumeConvexMeshesFile(LevelEditorEntity* entity);

	/**
	 * On bounding volume convex meshes remove
	 * @param entity entity
	 */
	virtual void onBoundingVolumeConvexMeshesRemove(LevelEditorEntity* entity);

	/**
	 * On bounding volume convex meshes generate
	 * @param entity entity
	 */
	virtual void onBoundingVolumeConvexMeshesGenerate(LevelEditorEntity* entity);

	/**
	 * Set terrain mesh
	 * @param entity entity
	 */
	virtual void setTerrainMesh(LevelEditorEntity* entity);

	/**
	 * Shows the error pop up
	 * @param entity entity
	 */
	virtual void onSetTerrainMesh(LevelEditorEntity* entity);

	/**
	 * Unset terrain mesh
	 */
	virtual void unsetTerrainMesh();

	/**
	 * Set convex meshes
	 * @param entity entity
	 */
	virtual void setConvexMeshes(LevelEditorEntity* entity);

	/**
	 * Unset convex meshes
	 */
	virtual void unsetConvexMeshes();

	/**
	 * Unset physics
	 */
	virtual void unsetPhysics();

	/**
	 * Set physics
	 * @param entity entity
	 */
	virtual void setPhysics(LevelEditorEntity* entity);

	/**
	 * On physics body type apply
	 * @param entity entity
	 */
	virtual void onPhysicsBodyTypeApply(LevelEditorEntity* entity);

	/**
	 * On physics body apply
	 * @param entity entity
	 */
	virtual void onPhysicsBodyApply(LevelEditorEntity* entity);

	/**
	 * On convex mesh mode changed
	 * @param disabled disabled
	 */
	virtual void onConvexMeshModeChanged(bool disabled);

	/**
	 * Shows the error pop up
	 */
	virtual void showErrorPopUp(const string& caption, const string& message);

	/**
	 * On value changed
	 * @param node node
	 * @param entity entity
	 */
	virtual void onValueChanged(GUIElementNode* node, LevelEditorEntity* entity);

	/** 
	 * On action performed
	 * @param type type
	 * @param node node
	 * @param entity entity
	 */
	virtual void onActionPerformed(GUIActionListener_Type* type, GUIElementNode* node, LevelEditorEntity* entity);

	/**
	 * Public constructor
	 * @param popUps pop ups
	 * @param modelPath model editor screen controller
	 * @param isModelBoundingVolumes is model bounding volumes
	 */
	EntityPhysicsSubScreenController(PopUps* popUps, FileDialogPath* modelPath, bool isModelBoundingVolumes);

	/**
	 * Destructor
	 */
	virtual ~EntityPhysicsSubScreenController();
};
