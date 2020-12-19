#include <tdme/tools/shared/controller/PrototypePhysicsSubScreenController_BoundingVolumeType.h>

#include <string>

#include <tdme/utilities/Enum.h>

using std::string;

using tdme::tools::shared::controller::PrototypePhysicsSubScreenController_BoundingVolumeType;
using tdme::utilities::Enum;

PrototypePhysicsSubScreenController_BoundingVolumeType::PrototypePhysicsSubScreenController_BoundingVolumeType(const string& name, int ordinal)
	: Enum(name, ordinal)
{
}

PrototypePhysicsSubScreenController_BoundingVolumeType* tdme::tools::shared::controller::PrototypePhysicsSubScreenController_BoundingVolumeType::NONE = new PrototypePhysicsSubScreenController_BoundingVolumeType("NONE", 0);
PrototypePhysicsSubScreenController_BoundingVolumeType* tdme::tools::shared::controller::PrototypePhysicsSubScreenController_BoundingVolumeType::SPHERE = new PrototypePhysicsSubScreenController_BoundingVolumeType("SPHERE", 1);
PrototypePhysicsSubScreenController_BoundingVolumeType* tdme::tools::shared::controller::PrototypePhysicsSubScreenController_BoundingVolumeType::CAPSULE = new PrototypePhysicsSubScreenController_BoundingVolumeType("CAPSULE", 2);
PrototypePhysicsSubScreenController_BoundingVolumeType* tdme::tools::shared::controller::PrototypePhysicsSubScreenController_BoundingVolumeType::BOUNDINGBOX = new PrototypePhysicsSubScreenController_BoundingVolumeType("BOUNDINGBOX", 3);
PrototypePhysicsSubScreenController_BoundingVolumeType* tdme::tools::shared::controller::PrototypePhysicsSubScreenController_BoundingVolumeType::ORIENTEDBOUNDINGBOX = new PrototypePhysicsSubScreenController_BoundingVolumeType("ORIENTEDBOUNDINGBOX", 4);
PrototypePhysicsSubScreenController_BoundingVolumeType* tdme::tools::shared::controller::PrototypePhysicsSubScreenController_BoundingVolumeType::CONVEXMESH = new PrototypePhysicsSubScreenController_BoundingVolumeType("CONVEXMESH", 5);

PrototypePhysicsSubScreenController_BoundingVolumeType* PrototypePhysicsSubScreenController_BoundingVolumeType::valueOf(const string& name)
{
	if (BOUNDINGBOX->getName() == name) return BOUNDINGBOX;
	if (CAPSULE->getName() == name) return CAPSULE;
	if (CONVEXMESH->getName() == name) return CONVEXMESH;
	if (NONE->getName() == name) return NONE;
	if (ORIENTEDBOUNDINGBOX->getName() == name) return ORIENTEDBOUNDINGBOX;
	if (SPHERE->getName() == name) return SPHERE;
	// TODO: throw exception here maybe
	return nullptr;
}
