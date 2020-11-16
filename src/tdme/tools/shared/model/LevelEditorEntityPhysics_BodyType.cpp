#include <tdme/tools/shared/model/LevelEditorEntityPhysics_BodyType.h>

#include <string>

#include <tdme/utilities/Enum.h>

using std::string;

using tdme::tools::shared::model::LevelEditorEntityPhysics_BodyType;
using tdme::utilities::Enum;

LevelEditorEntityPhysics_BodyType::LevelEditorEntityPhysics_BodyType(const string& name, int ordinal): Enum(name, ordinal)
{
}

LevelEditorEntityPhysics_BodyType* tdme::tools::shared::model::LevelEditorEntityPhysics_BodyType::NONE = new LevelEditorEntityPhysics_BodyType("NONE", 0);
LevelEditorEntityPhysics_BodyType* tdme::tools::shared::model::LevelEditorEntityPhysics_BodyType::COLLISION_BODY = new LevelEditorEntityPhysics_BodyType("COLLISION_BODY", 1);
LevelEditorEntityPhysics_BodyType* tdme::tools::shared::model::LevelEditorEntityPhysics_BodyType::DYNAMIC_RIGIDBODY = new LevelEditorEntityPhysics_BodyType("DYNAMIC_RIGIDBODY", 2);
LevelEditorEntityPhysics_BodyType* tdme::tools::shared::model::LevelEditorEntityPhysics_BodyType::STATIC_RIGIDBODY = new LevelEditorEntityPhysics_BodyType("STATIC_RIGIDBODY", 3);

LevelEditorEntityPhysics_BodyType* LevelEditorEntityPhysics_BodyType::valueOf(const string& name)
{
	if (NONE->getName() == name) return NONE;
	if (COLLISION_BODY->getName() == name) return COLLISION_BODY;
	if (DYNAMIC_RIGIDBODY->getName() == name) return DYNAMIC_RIGIDBODY;
	if (STATIC_RIGIDBODY->getName() == name) return STATIC_RIGIDBODY;
	// TODO: throw exception here maybe
	return nullptr;
}
