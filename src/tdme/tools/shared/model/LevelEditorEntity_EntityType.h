#pragma once

#include <string>

#include <tdme/math/Vector3.h>
#include <tdme/tools/shared/model/fwd-tdme.h>
#include <tdme/utilities/Enum.h>

using std::string;

using tdme::math::Vector3;
using tdme::tools::shared::model::LevelEditorEntity_EntityType;
using tdme::utilities::Enum;

class tdme::tools::shared::model::LevelEditorEntity_EntityType final: public Enum
{

private:
	int boundingVolumeCount;
	int32_t gizmoTypeMask;
	bool nonEditScaleDownMode;
	Vector3 nonEditScaleDownModeDimension;

public:
	static LevelEditorEntity_EntityType* EMPTY;
	static LevelEditorEntity_EntityType* MODEL;
	static LevelEditorEntity_EntityType* PARTICLESYSTEM;
	static LevelEditorEntity_EntityType* TRIGGER;
	static LevelEditorEntity_EntityType* ENVIRONMENTMAPPING;

	/**
	 * Public constructor
	 * @param name name
	 * @param ordinal ordinal
	 * @param boundingVolumeCount bounding volume count or -1 for default
	 * @param gizmoTypeMask gizmo type mask, see Gizmo::GIZMO_TYPE
	 * @param nonEditScaleDownMode non edit scale down mode
	 * @param nonEditScaleDownModeDimension non edit scale down mode dimension
	 */
	LevelEditorEntity_EntityType(const string& name, int ordinal, int boundingVolumeCount, int gizmoTypeMask, bool nonEditScaleDownMode, const Vector3& nonEditScaleDownModeDimension);

	/**
	 * @return bounding volume count
	 */
	inline int getBoundingVolumeCount() {
		return boundingVolumeCount;
	}

	/**
	 * @return gizmo types
	 */
	inline int32_t getGizmoTypeMask() {
		return gizmoTypeMask;
	}

	/**
	 * @return has non edit scale down mode
	 */
	inline bool hasNonEditScaleDownMode() {
		return nonEditScaleDownMode;
	}

	/**
	 * @return has non edit scale down mode
	 */
	inline const Vector3& getNonEditScaleDownModeDimension() {
		return nonEditScaleDownModeDimension;
	}

	/**
	 * Returns enum object given by name
	 * @param name name
	 * @return enum object
	 */
	static LevelEditorEntity_EntityType* valueOf(const string& name);

};
