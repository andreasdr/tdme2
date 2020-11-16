
#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/Transformations.h>
#include <tdme/math/Vector3.h>
#include <tdme/tools/shared/controller/fwd-tdme.h>
#include <tdme/tools/shared/model/fwd-tdme.h>
#include <tdme/tools/shared/views/fwd-tdme.h>
#include <tdme/tools/shared/views/Gizmo.h>

using std::string;

using tdme::engine::Engine;
using tdme::engine::Transformations;
using tdme::math::Vector3;
using tdme::tools::shared::controller::EntityPhysicsSubScreenController;
using tdme::tools::shared::model::LevelEditorEntity;
using tdme::tools::shared::views::Gizmo;
using tdme::tools::shared::views::PopUps;

/**
 * Entity physics view
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::shared::views::EntityPhysicsView final: protected Gizmo
{
public:
	static constexpr int DISPLAY_BOUNDINGVOLUMEIDX_ALL { -1 };

private:
	static constexpr int32_t MOUSE_DOWN_LAST_POSITION_NONE { -1 };

	Engine* engine { nullptr };
	EntityPhysicsSubScreenController* entityPhysicsSubScreenController { nullptr };
	PopUps* popUps { nullptr };

	int displayBoundingVolumeIdxLast;
	int32_t mouseDownLastX;
	int32_t mouseDownLastY;
	Vector3 totalDeltaScale;

	int displayBoundingVolumeIdx;
	bool displayBoundingVolume;

public:
	/**
	 * Public constructor
	 * @param entityPhysicsSubScreenController pop ups
	 * @param popUps model editor screen controller
	 */
	EntityPhysicsView(EntityPhysicsSubScreenController* entityPhysicsSubScreenController, PopUps* popUps);

	/**
	 * Destructor
	 */
	~EntityPhysicsView();

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
	 * @param entity entity
	 * @param idx idx
	 * @param type type
	 */
	void resetBoundingVolume(LevelEditorEntity* entity, int32_t idx, int32_t type);

	/**
	 * Set bounding volumes
	 * @param entity entity
	 */
	void setBoundingVolumes(LevelEditorEntity* entity);

	/**
	 * Unset bounding volumes
	 */
	void unsetBoundingVolumes();

	/**
	 * Select bounding volume type
	 * @param idx idx
	 * @param bvTypeId bounding volume type
	 */
	void selectBoundingVolumeType(int32_t idx, int32_t bvTypeId);

	/**
	 * Set terrain mesh
	 * @param entity entity
	 */
	void setTerrainMesh(LevelEditorEntity* entity);

	/**
	 * Unset terrain mesh
	 */
	void unsetTerrainMesh();

	/**
	 * Set convex meshes
	 * @param entity entity
	 */
	void setConvexMeshes(LevelEditorEntity* entity);

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
	 * @param entity entity
	 */
	void setPhysics(LevelEditorEntity* entity);

private:

	/**
	 * Clear model bounding volume
	 * @param idx idx
	 */
	void clearModelBoundingVolume(int32_t idx);

	/**
	 * Setup model bounding volume
	 * @param entity entity
	 * @param idx idx
	 */
	void setupModelBoundingVolume(LevelEditorEntity* entity, int32_t idx);

public:

	/**
	 * On bounding volume none apply
	 * @param entity entity
	 * @param idx bounding volume index
	 */
	void applyBoundingVolumeNone(LevelEditorEntity* entity, int32_t idx);

	/**
	 * On bounding volume sphere apply
	 * @param entity entity
	 * @param idx bounding volume index
	 * @param center sphere center
	 * @param radius radius
	 */
	void applyBoundingVolumeSphere(LevelEditorEntity* entity, int32_t idx, const Vector3& center, float radius);

	/**
	 * On bounding volume capsule apply
	 * @param entity entity
	 * @param idx bounding volume index
	 * @param a point a
	 * @param b point b
	 * @param radius radius
	 */
	void applyBoundingVolumeCapsule(LevelEditorEntity* entity, int32_t idx, const Vector3& a, const Vector3& b, float radius);

	/**
	 * On bounding volume AABB apply
	 * @param entity entity
	 * @param idx bounding volume index
	 * @param min AABB min vector
	 * @param max AABB max vector
	 */
	void applyBoundingVolumeAabb(LevelEditorEntity* entity, int32_t idx, const Vector3& min, const Vector3& max);

	/**
	 * On bounding volume OBB apply
	 * @param entity entity
	 * @param idx bounding volume index
	 * @param center OBB center
	 * @param axis0 OBB axis 0
	 * @param axis1 OBB axis 1
	 * @param axis2 OBB axis 2
	 * @param halfExtension OBB half extension
	 */
	void applyBoundingVolumeObb(LevelEditorEntity* entity, int32_t idx, const Vector3& center, const Vector3& axis0, const Vector3& axis1, const Vector3& axis2, const Vector3& halfExtension);

	/**
	 * On bounding volume convex mesh apply
	 * @param entity entity
	 * @param idx bounding volume index
	 * @param fileName file name
	 */
	void applyBoundingVolumeConvexMesh(LevelEditorEntity* entity, int32_t idx, const string& fileName);

	/**
	 * Apply bounding volume transformations
	 * @param entity entity
	 * @param idx bounding volume index
	 * @param transformations transformations
	 * @param objectScale object scale
	 * @param guiOnly only update GUI not the BV it self
	 */
	void applyBoundingVolumeTransformations(LevelEditorEntity* entity, int32_t idx, const Transformations& transformations, const Vector3& objectScale, bool guiOnly);

	/**
	 * Handle input events
	 * @param entity entity
	 * @param objectScale object scale
	 */
	void handleInputEvents(LevelEditorEntity* entity, const Vector3& objectScale);

	/**
	 * Display
	 * @param entity entity
	 */
	void display(LevelEditorEntity* entity);

	/**
	 * Update GIZMO
	 * @param entity level editor entity
	 */
	void updateGizmo(LevelEditorEntity* entity);

	/**
	 * Set GIZMO rotation
	 * @param entity level editor entity
	 * @param transformations transformations
	 */
	void setGizmoRotation(LevelEditorEntity* entity, const Transformations& transformations);

	/**
	 * Start editing bounding volume
	 * @param entity entity
	 */
	void startEditingBoundingVolume(LevelEditorEntity* entity);

	/**
	 * End editing bounding volume
	 * @param entity entity
	 */
	void endEditingBoundingVolume(LevelEditorEntity* entity);

	/**
	 * @return is editing bounding volume
	 */
	bool isEditingBoundingVolume(LevelEditorEntity* entity);

};
