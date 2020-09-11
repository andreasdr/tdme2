#pragma once

#include <string>

#include <tdme/tools/shared/model/fwd-tdme.h>
#include <tdme/utilities/Enum.h>

using std::string;

using tdme::utilities::Enum;

using tdme::tools::shared::model::LevelEditorEntityPhysics_BodyType;

/**
 * Level editor entity physics body type
 * @author Andreas Drewke
 */
class tdme::tools::shared::model::LevelEditorEntityPhysics_BodyType final: public Enum
{
public:
	static LevelEditorEntityPhysics_BodyType* NONE;
	static LevelEditorEntityPhysics_BodyType* COLLISION_BODY;
	static LevelEditorEntityPhysics_BodyType* DYNAMIC_RIGIDBODY;
	static LevelEditorEntityPhysics_BodyType* STATIC_RIGIDBODY;
	LevelEditorEntityPhysics_BodyType(const string& name, int ordinal);
	static LevelEditorEntityPhysics_BodyType* valueOf(const string& a0);

};
