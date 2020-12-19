#include <tdme/engine/prototype/Prototype_Type.h>

#include <string>

#include <tdme/math/Vector3.h>
#include <tdme/tools/shared/views/Gizmo.h>
#include <tdme/utilities/Enum.h>

using std::string;

using tdme::engine::prototype::Prototype_Type;
using tdme::math::Vector3;
using tdme::tools::shared::views::Gizmo;
using tdme::utilities::Enum;

Prototype_Type::Prototype_Type(const string& name, int ordinal, int boundingVolumeCount, int32_t gizmoTypeMask, bool nonEditScaleDownMode, const Vector3& nonEditScaleDownModeDimension):
	Enum(name, ordinal), boundingVolumeCount(boundingVolumeCount), gizmoTypeMask(gizmoTypeMask), nonEditScaleDownMode(nonEditScaleDownMode), nonEditScaleDownModeDimension(nonEditScaleDownModeDimension)
{
}

Prototype_Type* tdme::engine::prototype::Prototype_Type::EMPTY = new Prototype_Type("EMPTY", 0, 0, Gizmo::GIZMOTYPE_TRANSLATE | Gizmo::GIZMOTYPE_ROTATE | Gizmo::GIZMOTYPE_SCALE, false, Vector3());
Prototype_Type* tdme::engine::prototype::Prototype_Type::MODEL = new Prototype_Type("MODEL", 1, -1, Gizmo::GIZMOTYPE_TRANSLATE | Gizmo::GIZMOTYPE_ROTATE | Gizmo::GIZMOTYPE_SCALE, false, Vector3());
Prototype_Type* tdme::engine::prototype::Prototype_Type::PARTICLESYSTEM = new Prototype_Type("PARTICLESYSTEM", 2, -1, Gizmo::GIZMOTYPE_TRANSLATE | Gizmo::GIZMOTYPE_ROTATE | Gizmo::GIZMOTYPE_SCALE, false, Vector3());
Prototype_Type* tdme::engine::prototype::Prototype_Type::TRIGGER = new Prototype_Type("TRIGGER", 3, -1, Gizmo::GIZMOTYPE_TRANSLATE | Gizmo::GIZMOTYPE_ROTATE | Gizmo::GIZMOTYPE_SCALE, true, Vector3(1.0f, 1.0f, 1.0f));
Prototype_Type* tdme::engine::prototype::Prototype_Type::ENVIRONMENTMAPPING = new Prototype_Type("ENVIRONMENTMAPPING", 4, 1, Gizmo::GIZMOTYPE_TRANSLATE | Gizmo::GIZMOTYPE_SCALE, true, Vector3(1.0f, 1.0f, 1.0f));

Prototype_Type* Prototype_Type::valueOf(const string& name)
{
	if (EMPTY->getName() == name) return EMPTY;
	if (MODEL->getName() == name) return MODEL;
	if (PARTICLESYSTEM->getName() == name) return PARTICLESYSTEM;
	if (TRIGGER->getName() == name) return TRIGGER;
	if (ENVIRONMENTMAPPING->getName() == name) return ENVIRONMENTMAPPING;
	return nullptr;
}
