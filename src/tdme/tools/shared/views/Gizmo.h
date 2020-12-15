#pragma once

#include <vector>

#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/Transformations.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/math/Vector3.h>
#include <tdme/tools/shared/views/fwd-tdme.h>

using std::vector;

using tdme::engine::Engine;
using tdme::engine::Entity;
using tdme::engine::Object3D;
using tdme::engine::Transformations;
using tdme::engine::model::Node;
using tdme::math::Vector3;

/**
 * Gizmo tool for views
 * @author Andreas Drewke
 */
class tdme::tools::shared::views::Gizmo
{
public:
	enum GizmoType {
		GIZMOTYPE_NONE = 0,
		GIZMOTYPE_TRANSLATE = 1,
		GIZMOTYPE_ROTATE = 2,
		GIZMOTYPE_SCALE = 4,
		GIZMOTYPE_ALL = 8,
	};
	enum GizmoMode {
		GIZMOMODE_NONE,
		GIZMOMODE_TRANSLATE_X,
		GIZMOMODE_TRANSLATE_Y,
		GIZMOMODE_TRANSLATE_Z,
		GIZMOMODE_TRANSLATEPLANE_X,
		GIZMOMODE_TRANSLATEPLANE_Y,
		GIZMOMODE_TRANSLATEPLANE_Z,
		GIZMOMODE_SCALE_X,
		GIZMOMODE_SCALE_Y,
		GIZMOMODE_SCALE_Z,
		GIZMOMODE_SCALEPLANE_X,
		GIZMOMODE_SCALEPLANE_Y,
		GIZMOMODE_SCALEPLANE_Z,
		GIZMOMODE_ROTATE_X,
		GIZMOMODE_ROTATE_Y,
		GIZMOMODE_ROTATE_Z
	};

private:
	Engine* engine { nullptr };
	string id;
	GizmoType gizmoType;
	GizmoMode gizmoMode;
	Vector3 gizmoLastResult;
	bool gizmoLastResultAvailable;
	int32_t gizmoTypeMask;

public:
	/**
	 * Public constructor
	 * @param engine engine
	 * @param id id
	 * @param gizmoTypeMask gizmo type mask
	 */
	Gizmo(Engine* engine, const string& id, int32_t gizmoTypeMask = GIZMOTYPE_TRANSLATE | GIZMOTYPE_ROTATE | GIZMOTYPE_SCALE);

	/**
	 * Destructor
	 */
	virtual ~Gizmo();

	/**
	 * @return GIZMO type mask
	 */
	inline int32_t getGizmoTypeMask() const {
		return gizmoTypeMask;
	}

	/**
	 * Set GIZMO type mask
	 * @param gizmoTypeMask GIZMO type mask
	 */
	inline void setGizmoTypeMask(int gizmoTypeMask) {
		this->gizmoTypeMask = gizmoTypeMask;
		if ((gizmoType & gizmoTypeMask) == 0) gizmoType = GIZMOTYPE_ALL;
	}

	/**
	 * @return GIZMO type
	 */
	inline GizmoType getGizmoType() const {
		return gizmoType;
	}

	/**
	 * Set GIZMO type
	 * @param gizmoType GIZMO type
	 */
	inline void setGizmoType(GizmoType gizmoType) {
		if (gizmoType != GIZMOTYPE_ALL && (gizmoType & gizmoTypeMask) == 0) return;
		this->gizmoType = gizmoType;
	}

	/**
	 * @return GIZMO mode
	 */
	inline GizmoMode getGizmoMode() const {
		return gizmoMode;
	}

	/**
	 * Set GIZMO mode
	 * @param gizmoMode gizmo mode
	 */
	inline void setGizmoMode(GizmoMode gizmoMode) {
		this->gizmoMode = gizmoMode;
		if (this->gizmoMode == GIZMOMODE_NONE) gizmoLastResultAvailable = false;
	}

	/**
	 * Update gizmo
	 * @param gizmoCenter GIZMO center
	 * @param transformations transformations used for rotation
	 */
	void updateGizmo(const Vector3& gizmoCenter, const Transformations& transformations);

	/**
	 * @return GIZMO object
	 */
	Object3D* getGizmoObject3D();

	/**
	 * Remove gizmo
	 */
	void removeGizmo();

	/**
	 * Determine movement on a plane given by 4 vertices
	 * @param mouseX current mouse X position
	 * @param mouseY current mouse Y position
	 * @param vertices 4 vertices that span a plane
	 * @param deltaMovement delta movement result
	 * @return success
	 */
	bool determineGizmoMovement(int mouseX, int mouseY, vector<Vector3> vertices, Vector3& deltaMovement);

	/**
	 * Determine GIZMO delta transformations
	 * @param mouseLastX last mouse X position
	 * @param mouseLastY last mouse Y position
	 * @param mouseX mouse X position
	 * @param mouseY mouse Y position
	 * @param deltaTranslation determined delta translation
	 * @param deltaRotation determined delta rotations
	 * @param deltaScale determined delta scale
	 */
	bool determineGizmoDeltaTransformations(int mouseLastX, int mouseLastY, int mouseX, int mouseY, Vector3& deltaTranslation, Vector3& deltaRotation, Vector3& deltaScale);

	/**
	 * Select GIZMO mode
	 * @param selectedEntity selected entity
	 * @param selectedEntityNode selected entity node
	 */
	bool determineGizmoMode(Entity* selectedEntity, Node* selectedEntityNode);

	/**
	 * Set gizmo rotation
	 * @param transformations transformations containing rotations
	 */
	void setGizmoRotation(const Transformations& transformations);

};
