#pragma once

#include <string>

#include <tdme/engine/prototype/fwd-tdme.h>
#include <tdme/math/Vector3.h>
#include <tdme/utilities/Enum.h>

using std::string;

using tdme::engine::prototype::Prototype_Type;
using tdme::math::Vector3;
using tdme::utilities::Enum;

/**
 * Prototype type enum
 * @author Andreas Drewke
 */
class tdme::engine::prototype::Prototype_Type final: public Enum {
private:
	int boundingVolumeCount;
	int32_t gizmoTypeMask;
	bool nonEditScaleDownMode;
	Vector3 nonEditScaleDownModeDimension;

public:
	static Prototype_Type* EMPTY;
	static Prototype_Type* MODEL;
	static Prototype_Type* PARTICLESYSTEM;
	static Prototype_Type* TRIGGER;
	static Prototype_Type* ENVIRONMENTMAPPING;

	/**
	 * Public constructor
	 * @param name name
	 * @param ordinal ordinal
	 * @param boundingVolumeCount bounding volume count or -1 for default
	 * @param gizmoTypeMask gizmo type mask, see Gizmo::GIZMO_TYPE
	 * @param nonEditScaleDownMode non edit scale down mode
	 * @param nonEditScaleDownModeDimension non edit scale down mode dimension
	 */
	Prototype_Type(const string& name, int ordinal, int boundingVolumeCount, int gizmoTypeMask, bool nonEditScaleDownMode, const Vector3& nonEditScaleDownModeDimension);

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
	static Prototype_Type* valueOf(const string& name);

};
