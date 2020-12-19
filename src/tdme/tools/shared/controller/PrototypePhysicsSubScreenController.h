#pragma once

#include <array>
#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/prototype/fwd-tdme.h>
#include <tdme/engine/prototype/Prototype.h>
#include <tdme/gui/events/fwd-tdme.h>
#include <tdme/gui/events/GUIActionListener.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/tools/shared/controller/fwd-tdme.h>
#include <tdme/tools/shared/views/fwd-tdme.h>
#include <tdme/utilities/fwd-tdme.h>

using std::array;
using std::string;
using std::vector;

using tdme::engine::prototype::Prototype;
using tdme::gui::events::GUIActionListenerType;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUIParentNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::math::Vector3;
using tdme::tools::shared::controller::FileDialogPath;
using tdme::tools::shared::controller::PrototypePhysicsSubScreenController_BoundingVolumeType;
using tdme::tools::shared::views::PopUps;
using tdme::tools::shared::views::PrototypePhysicsView;
using tdme::utilities::MutableString;

/**
 * Prototype physics sub screen controller
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::shared::controller::PrototypePhysicsSubScreenController final
{
	friend class PrototypePhysicsSubScreenController_BoundingVolumeType;
	friend class PrototypePhysicsSubScreenController_GenerateConvexMeshes;
	friend class PrototypePhysicsSubScreenController_onBoundingVolumeConvexMeshesFile;
	friend class PrototypePhysicsSubScreenController_onBoundingVolumeConvexMeshFile;

private:
	GUIScreenNode* screenNode { nullptr };
	FileDialogPath* modelPath { nullptr };
	PrototypePhysicsView* view { nullptr };
	array<GUIElementNode*, Prototype::MODEL_BOUNDINGVOLUME_COUNT> boundingVolumeTypeDropDown;
	array<GUIElementNode*, Prototype::MODEL_BOUNDINGVOLUME_COUNT> boundingVolumeApply;
	array<GUIElementNode*, Prototype::MODEL_BOUNDINGVOLUME_COUNT> boundingVolume;
	array<GUIElementNode*, Prototype::MODEL_BOUNDINGVOLUME_COUNT> boundingvolumeSphereCenter;
	array<GUIElementNode*, Prototype::MODEL_BOUNDINGVOLUME_COUNT> boundingvolumeSphereRadius;
	array<GUIElementNode*, Prototype::MODEL_BOUNDINGVOLUME_COUNT> boundingvolumeCapsuleA;
	array<GUIElementNode*, Prototype::MODEL_BOUNDINGVOLUME_COUNT> boundingvolumeCapsuleB;
	array<GUIElementNode*, Prototype::MODEL_BOUNDINGVOLUME_COUNT> boundingvolumeCapsuleRadius;
	array<GUIElementNode*, Prototype::MODEL_BOUNDINGVOLUME_COUNT> boundingvolumeBoundingBoxMin;
	array<GUIElementNode*, Prototype::MODEL_BOUNDINGVOLUME_COUNT> boundingvolumeBoundingBoxMax;
	array<GUIElementNode*, Prototype::MODEL_BOUNDINGVOLUME_COUNT> boundingvolumeObbCenter;
	array<GUIElementNode*, Prototype::MODEL_BOUNDINGVOLUME_COUNT> boundingvolumeObbHalfextension;
	array<GUIElementNode*, Prototype::MODEL_BOUNDINGVOLUME_COUNT> boundingvolumeObbRotationX;
	array<GUIElementNode*, Prototype::MODEL_BOUNDINGVOLUME_COUNT> boundingvolumeObbRotationY;
	array<GUIElementNode*, Prototype::MODEL_BOUNDINGVOLUME_COUNT> boundingvolumeObbRotationZ;
	array<GUIElementNode*, Prototype::MODEL_BOUNDINGVOLUME_COUNT> boundingvolumeConvexMeshFile;
	bool isModelBoundingVolumes;
	int maxBoundingVolumeCount;
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
	int boundingVolumeTypeCount;

public:
	enum BoundingVolumeType {
		BOUNDINGVOLUMETYPE_NONE = 1,
		BOUNDINGVOLUMETYPE_SPHERE = 2,
		BOUNDINGVOLUMETYPE_CAPSULE = 4,
		BOUNDINGVOLUMETYPE_BOUNDINGBOX = 8,
		BOUNDINGVOLUMETYPE_ORIENTEDBOUNDINGBOX = 16,
		BOUNDINGVOLUMETYPE_CONVEXMESH = 32,
		BOUNDINGVOLUMETYPE_ALL = 1 + 2 + 4 +8 + 16 + 32
	};

	/**
	 * Public constructor
	 * @param popUps pop ups
	 * @param modelPath model editor screen controller
	 * @param isModelBoundingVolumes is model bounding volumes
	 * @param maxBoundingVolumeCount maximum number of editable bounding volumes or -1 for default
	 * @param boundingVolumeTypeMask bounding volume type mask
	 */
	PrototypePhysicsSubScreenController(PopUps* popUps, FileDialogPath* modelPath, bool isModelBoundingVolumes, int maxBoundingVolumeCount = -1, int32_t boundingVolumeTypeMask = BOUNDINGVOLUMETYPE_ALL);

	/**
	 * Destructor
	 */
	~PrototypePhysicsSubScreenController();

	/**
	 * @return view
	 */
	PrototypePhysicsView* getView();

	/**
	 * @return screen node
	 */
	GUIScreenNode* getScreenNode();

	/**
	 * Init
	 * @param screenNode screen node
	 */
	void initialize(GUIScreenNode* screenNode);

	/**
	 * Disable bounding volume
	 */
	void disableBoundingVolume(int idx);

	/**
	 * Enable bounding volume
	 * @param idx idx
	 */
	void enableBoundingVolume(int idx);

	/**
	 * Set up model bounding volume type
	 * @param prototype prototype
	 * @param idx idx
	 */
	void setupModelBoundingVolumeType(Prototype* prototype, int idx);

	/**
	 * Set up bounding volume types
	 * @param idx idx
	 * @param boundingVolumeTypeMask bounding volume type mask
	 */
	void setupBoundingVolumeTypes(int idx, int boundingVolumeTypeMask);

	/**
	 * Display given bounding volume GUI elements
	 * @param idx idx
	 * @param bvType bounding volume type
	 */
	void selectBoundingVolume(int idx, PrototypePhysicsSubScreenController_BoundingVolumeType* bvType);

	/**
	 * Setup sphere bounding volume
	 * @param idx idx
	 * @param center center
	 * @param radius radius
	 */
	void setupSphere(int idx, const Vector3& center, float radius);

	/**
	 * Setup capsule bounding volume
	 * @param idx idx
	 * @param a a
	 * @param b b
	 * @param radius radius
	 */
	void setupCapsule(int idx, const Vector3& a, const Vector3& b, float radius);

	/**
	 * Setup AABB bounding volume
	 * @param idx idx
	 * @param min min
	 * @param max max
	 */
	void setupBoundingBox(int idx, const Vector3& min, const Vector3& max);

	/**
	 * Setup oriented bounding box
	 * @param idx idx
	 * @param center center
	 * @param axis0 axis 0
	 * @param axis1 axis 1
	 * @param axis2 axis 2
	 * @param halfExtension half extension
	 */
	void setupOrientedBoundingBox(int idx, const Vector3& center, const Vector3& axis0, const Vector3& axis1, const Vector3& axis2, const Vector3& halfExtension);

	/**
	 * Setup convex mesh bounding volume
	 * @param idx idx
	 * @param file file
	 */
	void setupConvexMesh(int idx, const string& file);

	/**
	 * On pivot apply
	 * @param prototype prototype
	 * @param idx idx
	 */
	void onBoundingVolumeTypeApply(Prototype* prototype, int idx);

	/**
	 * On bounding volume none apply
	 * @param prototype prototype
	 * @param idx idx
	 */
	void onBoundingVolumeNoneApply(Prototype* prototype, int idx);

	/**
	 * On bounding volume sphere apply
	 * @param prototype prototype
	 * @param idx idx
	 */
	void onBoundingVolumeSphereApply(Prototype* prototype, int idx);

	/**
	 * On bounding volume capsule apply
	 * @param prototype prototype
	 * @param idx idx
	 */
	void onBoundingVolumeCapsuleApply(Prototype* prototype, int idx);

	/**
	 * On bounding volume AABB apply
	 * @param prototype prototype
	 * @param idx idx
	 */
	void onBoundingVolumeAabbApply(Prototype* prototype, int idx);

	/**
	 * On bounding volume OBB apply
	 * @param prototype prototype
	 * @param idx idx
	 */
	void onBoundingVolumeObbApply(Prototype* prototype, int idx);

	/**
	 * On bounding volume convex mesh apply
	 * @param prototype prototype
	 * @param idx idx
	 */
	void onBoundingVolumeConvexMeshApply(Prototype* prototype, int idx);

	/**
	 * On bounding volume convex mesh file clicked
	 * @param prototype prototype
	 * @param idx idx
	 */
	void onBoundingVolumeConvexMeshFile(Prototype* prototype, int idx);

	/**
	 * On bounding volume convex meshes file
	 * @param prototype prototype
	 */
	void onBoundingVolumeConvexMeshesFile(Prototype* prototype);

	/**
	 * On bounding volume convex meshes remove
	 * @param prototype prototype
	 */
	void onBoundingVolumeConvexMeshesRemove(Prototype* prototype);

	/**
	 * On bounding volume convex meshes generate
	 * @param prototype prototype
	 */
	void onBoundingVolumeConvexMeshesGenerate(Prototype* prototype);

	/**
	 * Set terrain mesh
	 * @param prototype prototype
	 */
	void setTerrainMesh(Prototype* prototype);

	/**
	 * Shows the error pop up
	 * @param prototype prototype
	 */
	void onSetTerrainMesh(Prototype* prototype);

	/**
	 * Unset terrain mesh
	 */
	void unsetTerrainMesh();

	/**
	 * Set convex meshes
	 * @param prototype prototype
	 */
	void setConvexMeshes(Prototype* prototype);

	/**
	 * Unset convex meshes
	 */
	void unsetConvexMeshes();

	/**
	 * Unset physics
	 */
	void unsetPhysics();

	/**
	 * Set physics
	 * @param prototype prototype
	 */
	void setPhysics(Prototype* prototype);

	/**
	 * On physics body type apply
	 * @param prototype prototype
	 */
	void onPhysicsBodyTypeApply(Prototype* prototype);

	/**
	 * On physics body apply
	 * @param prototype prototype
	 */
	void onPhysicsBodyApply(Prototype* prototype);

	/**
	 * On convex mesh mode changed
	 * @param disabled disabled
	 */
	void onConvexMeshModeChanged(bool disabled);

	/**
	 * Shows the error pop up
	 * @param caption caption
	 * @param message message
	 */
	void showErrorPopUp(const string& caption, const string& message);

	/**
	 * On value changed
	 * @param node node
	 * @param prototype prototype
	 */
	void onValueChanged(GUIElementNode* node, Prototype* prototype);

	/**
	 * On action performed
	 * @param type type
	 * @param node node
	 * @param prototype prototype
	 */
	void onActionPerformed(GUIActionListenerType type, GUIElementNode* node, Prototype* prototype);

};
