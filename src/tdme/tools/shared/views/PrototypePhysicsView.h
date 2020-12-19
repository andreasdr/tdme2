
#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/prototype/fwd-tdme.h>
#include <tdme/engine/Transformations.h>
#include <tdme/math/Vector3.h>
#include <tdme/tools/shared/controller/fwd-tdme.h>
#include <tdme/tools/shared/controller/PrototypePhysicsSubScreenController.h>
#include <tdme/tools/shared/views/fwd-tdme.h>
#include <tdme/tools/shared/views/Gizmo.h>

using std::string;

using tdme::engine::prototype::Prototype;
using tdme::engine::Engine;
using tdme::engine::Transformations;
using tdme::math::Vector3;
using tdme::tools::shared::controller::PrototypePhysicsSubScreenController;
using tdme::tools::shared::views::Gizmo;
using tdme::tools::shared::views::PopUps;

/**
 * Prototype physics view
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::shared::views::PrototypePhysicsView final: public Gizmo
{
public:
	static constexpr int DISPLAY_BOUNDINGVOLUMEIDX_ALL { -1 };

private:
	static constexpr int MOUSE_DOWN_LAST_POSITION_NONE { -1 };

	Engine* engine { nullptr };
	PrototypePhysicsSubScreenController* prototypePhysicsSubScreenController { nullptr };
	PopUps* popUps { nullptr };

	int maxBoundingVolumeCount;

	int displayBoundingVolumeIdxLast;
	int mouseDownLastX;
	int mouseDownLastY;
	Vector3 totalDeltaScale;

	int displayBoundingVolumeIdx;
	bool displayBoundingVolume;

	int32_t boundingVolumeTypeMask;

public:
	/**
	 * Public constructor
	 * @param prototypePhysicsSubScreenController pop ups
	 * @param popUps model editor screen controller
	 * @param maxBoundingVolumeCount maximum number of editable bounding volumes or -1 for default
	 * @param boundingVolumeTypeMask bounding volume type mask
	 */
	PrototypePhysicsView(PrototypePhysicsSubScreenController* prototypePhysicsSubScreenController, PopUps* popUps, int maxBoundingVolumeCount = -1, int32_t boundingVolumeTypeMask = PrototypePhysicsSubScreenController::BOUNDINGVOLUMETYPE_ALL);

	/**
	 * Destructor
	 */
	~PrototypePhysicsView();

	/**
	 * @return display bounding volume
	 */
	inline  bool isDisplayBoundingVolume() {
		return displayBoundingVolume;
	}

	/**
	 * Set up bounding volume visibility
	 * @param displayBoundingVolume bounding volume
	 */
	inline  void setDisplayBoundingVolume(bool displayBoundingVolume) {
		this->displayBoundingVolume = displayBoundingVolume;
	}

	/**
	 * @return bounding volume index to display or DISPLAY_BOUNDINGVOLUMEIDX_ALL
	 */
	inline int getDisplayBoundingVolumeIdx() const {
		return displayBoundingVolumeIdx;
	}

	/**
	 * Set display bounding volume idx
	 * @param displayBoundingVolumeIdx display bounding volume index or DISPLAY_BOUNDINGVOLUMEIDX_ALL
	 */
	inline void setDisplayBoundingVolumeIdx(int displayBoundingVolumeIdx) {
		this->displayBoundingVolumeIdx = displayBoundingVolumeIdx;
	}

	/**
	 * @return pop up views
	 */
	PopUps* getPopUpsViews();

	/**
	 * Init
	 */
	void initialize();

	/**
	 * Reset bounding volume
	 * @param prototype prototype
	 * @param idx idx
	 * @param type type
	 */
	void resetBoundingVolume(Prototype* prototype, int idx, int type);

	/**
	 * Set bounding volumes
	 * @param prototype prototype
	 */
	void setBoundingVolumes(Prototype* prototype);

	/**
	 * Unset bounding volumes
	 */
	void unsetBoundingVolumes();

	/**
	 * Select bounding volume type
	 * @param idx idx
	 * @param bvTypeId bounding volume type
	 */
	void selectBoundingVolumeType(int idx, int bvTypeId);

	/**
	 * Set terrain mesh
	 * @param prototype prototype
	 */
	void setTerrainMesh(Prototype* prototype);

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

private:

	/**
	 * Clear model bounding volume
	 * @param idx idx
	 */
	void clearModelBoundingVolume(int idx);

	/**
	 * Setup model bounding volume
	 * @param prototype prototype
	 * @param idx idx
	 */
	void setupModelBoundingVolume(Prototype* prototype, int idx);

public:

	/**
	 * On bounding volume none apply
	 * @param prototype prototype
	 * @param idx bounding volume index
	 */
	void applyBoundingVolumeNone(Prototype* prototype, int idx);

	/**
	 * On bounding volume sphere apply
	 * @param prototype prototype
	 * @param idx bounding volume index
	 * @param center sphere center
	 * @param radius radius
	 */
	void applyBoundingVolumeSphere(Prototype* prototype, int idx, const Vector3& center, float radius);

	/**
	 * On bounding volume capsule apply
	 * @param prototype prototype
	 * @param idx bounding volume index
	 * @param a point a
	 * @param b point b
	 * @param radius radius
	 */
	void applyBoundingVolumeCapsule(Prototype* prototype, int idx, const Vector3& a, const Vector3& b, float radius);

	/**
	 * On bounding volume AABB apply
	 * @param prototype prototype
	 * @param idx bounding volume index
	 * @param min AABB min vector
	 * @param max AABB max vector
	 */
	void applyBoundingVolumeAabb(Prototype* prototype, int idx, const Vector3& min, const Vector3& max);

	/**
	 * On bounding volume OBB apply
	 * @param prototype prototype
	 * @param idx bounding volume index
	 * @param center OBB center
	 * @param axis0 OBB axis 0
	 * @param axis1 OBB axis 1
	 * @param axis2 OBB axis 2
	 * @param halfExtension OBB half extension
	 */
	void applyBoundingVolumeObb(Prototype* prototype, int idx, const Vector3& center, const Vector3& axis0, const Vector3& axis1, const Vector3& axis2, const Vector3& halfExtension);

	/**
	 * On bounding volume convex mesh apply
	 * @param prototype prototype
	 * @param idx bounding volume index
	 * @param fileName file name
	 */
	void applyBoundingVolumeConvexMesh(Prototype* prototype, int idx, const string& fileName);

	/**
	 * Apply bounding volume transformations
	 * @param prototype prototype
	 * @param idx bounding volume index
	 * @param transformations transformations
	 * @param objectScale object scale
	 * @param guiOnly only update GUI not the BV it self
	 */
	void applyBoundingVolumeTransformations(Prototype* prototype, int idx, const Transformations& transformations, const Vector3& objectScale, bool guiOnly);

	/**
	 * Handle input events
	 * @param prototype prototype
	 * @param objectScale object scale
	 */
	void handleInputEvents(Prototype* prototype, const Vector3& objectScale);

	/**
	 * Display
	 * @param prototype prototype
	 */
	void display(Prototype* prototype);

	/**
	 * Update GIZMO
	 * @param prototype prototype
	 */
	void updateGizmo(Prototype* prototype);

	/**
	 * Set GIZMO rotation
	 * @param prototype prototype
	 * @param transformations transformations
	 */
	void setGizmoRotation(Prototype* prototype, const Transformations& transformations);

	/**
	 * Start editing bounding volume
	 * @param prototype prototype
	 */
	void startEditingBoundingVolume(Prototype* prototype);

	/**
	 * End editing bounding volume
	 * @param prototype prototype
	 */
	void endEditingBoundingVolume(Prototype* prototype);

	/**
	 * @return is editing bounding volume
	 */
	bool isEditingBoundingVolume(Prototype* prototype);

};
