#include <tdme/engine/prototype/PrototypePhysics_BodyType.h>

#include <string>

#include <tdme/utilities/Enum.h>

using std::string;

using tdme::engine::prototype::PrototypePhysics_BodyType;
using tdme::utilities::Enum;

PrototypePhysics_BodyType::PrototypePhysics_BodyType(const string& name, int ordinal): Enum(name, ordinal)
{
}

PrototypePhysics_BodyType* tdme::engine::prototype::PrototypePhysics_BodyType::NONE = new PrototypePhysics_BodyType("NONE", 0);
PrototypePhysics_BodyType* tdme::engine::prototype::PrototypePhysics_BodyType::COLLISION_BODY = new PrototypePhysics_BodyType("COLLISION_BODY", 1);
PrototypePhysics_BodyType* tdme::engine::prototype::PrototypePhysics_BodyType::DYNAMIC_RIGIDBODY = new PrototypePhysics_BodyType("DYNAMIC_RIGIDBODY", 2);
PrototypePhysics_BodyType* tdme::engine::prototype::PrototypePhysics_BodyType::STATIC_RIGIDBODY = new PrototypePhysics_BodyType("STATIC_RIGIDBODY", 3);

PrototypePhysics_BodyType* PrototypePhysics_BodyType::valueOf(const string& name)
{
	if (NONE->getName() == name) return NONE;
	if (COLLISION_BODY->getName() == name) return COLLISION_BODY;
	if (DYNAMIC_RIGIDBODY->getName() == name) return DYNAMIC_RIGIDBODY;
	if (STATIC_RIGIDBODY->getName() == name) return STATIC_RIGIDBODY;
	// TODO: throw exception here maybe
	return nullptr;
}
