#include <tdme/tools/shared/model/LevelEditorEntity_EntityType.h>

#include <string>

#include <tdme/math/Vector3.h>
#include <tdme/tools/shared/views/Gizmo.h>
#include <tdme/utilities/Enum.h>

using std::string;

using tdme::math::Vector3;
using tdme::utilities::Enum;
using tdme::tools::shared::model::LevelEditorEntity_EntityType;
using tdme::tools::shared::views::Gizmo;

LevelEditorEntity_EntityType::LevelEditorEntity_EntityType(const string& name, int ordinal, int boundingVolumeCount, int32_t gizmoTypeMask, bool nonEditScaleDownMode, const Vector3& nonEditScaleDownModeDimension):
	Enum(name, ordinal), boundingVolumeCount(boundingVolumeCount), gizmoTypeMask(gizmoTypeMask), nonEditScaleDownMode(nonEditScaleDownMode), nonEditScaleDownModeDimension(nonEditScaleDownModeDimension)
{
}

LevelEditorEntity_EntityType* tdme::tools::shared::model::LevelEditorEntity_EntityType::EMPTY = new LevelEditorEntity_EntityType("EMPTY", 0, 0, Gizmo::GIZMOTYPE_TRANSLATE | Gizmo::GIZMOTYPE_ROTATE | Gizmo::GIZMOTYPE_SCALE, false, Vector3());
LevelEditorEntity_EntityType* tdme::tools::shared::model::LevelEditorEntity_EntityType::MODEL = new LevelEditorEntity_EntityType("MODEL", 1, -1, Gizmo::GIZMOTYPE_TRANSLATE | Gizmo::GIZMOTYPE_ROTATE | Gizmo::GIZMOTYPE_SCALE, false, Vector3());
LevelEditorEntity_EntityType* tdme::tools::shared::model::LevelEditorEntity_EntityType::PARTICLESYSTEM = new LevelEditorEntity_EntityType("PARTICLESYSTEM", 2, -1, Gizmo::GIZMOTYPE_TRANSLATE | Gizmo::GIZMOTYPE_ROTATE | Gizmo::GIZMOTYPE_SCALE, false, Vector3());
LevelEditorEntity_EntityType* tdme::tools::shared::model::LevelEditorEntity_EntityType::TRIGGER = new LevelEditorEntity_EntityType("TRIGGER", 3, -1, Gizmo::GIZMOTYPE_TRANSLATE | Gizmo::GIZMOTYPE_ROTATE | Gizmo::GIZMOTYPE_SCALE, true, Vector3(1.0f, 1.0f, 1.0f));
LevelEditorEntity_EntityType* tdme::tools::shared::model::LevelEditorEntity_EntityType::ENVIRONMENTMAPPING = new LevelEditorEntity_EntityType("ENVIRONMENTMAPPING", 4, 1, Gizmo::GIZMOTYPE_TRANSLATE | Gizmo::GIZMOTYPE_SCALE, true, Vector3(1.0f, 1.0f, 1.0f));

LevelEditorEntity_EntityType* LevelEditorEntity_EntityType::valueOf(const string& name)
{
	if (EMPTY->getName() == name) return EMPTY;
	if (MODEL->getName() == name) return MODEL;
	if (PARTICLESYSTEM->getName() == name) return PARTICLESYSTEM;
	if (TRIGGER->getName() == name) return TRIGGER;
	if (ENVIRONMENTMAPPING->getName() == name) return ENVIRONMENTMAPPING;
	return nullptr;
}
