#include <tdme/tools/shared/controller/EntityPhysicsSubScreenController_BoundingVolumeType.h>

#include <string>

#include <tdme/utilities/Enum.h>

using std::string;

using tdme::tools::shared::controller::EntityPhysicsSubScreenController_BoundingVolumeType;
using tdme::utilities::Enum;

EntityPhysicsSubScreenController_BoundingVolumeType::EntityPhysicsSubScreenController_BoundingVolumeType(const string& name, int ordinal)
	: Enum(name, ordinal)
{
}

EntityPhysicsSubScreenController_BoundingVolumeType* tdme::tools::shared::controller::EntityPhysicsSubScreenController_BoundingVolumeType::NONE = new EntityPhysicsSubScreenController_BoundingVolumeType("NONE", 0);
EntityPhysicsSubScreenController_BoundingVolumeType* tdme::tools::shared::controller::EntityPhysicsSubScreenController_BoundingVolumeType::SPHERE = new EntityPhysicsSubScreenController_BoundingVolumeType("SPHERE", 1);
EntityPhysicsSubScreenController_BoundingVolumeType* tdme::tools::shared::controller::EntityPhysicsSubScreenController_BoundingVolumeType::CAPSULE = new EntityPhysicsSubScreenController_BoundingVolumeType("CAPSULE", 2);
EntityPhysicsSubScreenController_BoundingVolumeType* tdme::tools::shared::controller::EntityPhysicsSubScreenController_BoundingVolumeType::BOUNDINGBOX = new EntityPhysicsSubScreenController_BoundingVolumeType("BOUNDINGBOX", 3);
EntityPhysicsSubScreenController_BoundingVolumeType* tdme::tools::shared::controller::EntityPhysicsSubScreenController_BoundingVolumeType::ORIENTEDBOUNDINGBOX = new EntityPhysicsSubScreenController_BoundingVolumeType("ORIENTEDBOUNDINGBOX", 4);
EntityPhysicsSubScreenController_BoundingVolumeType* tdme::tools::shared::controller::EntityPhysicsSubScreenController_BoundingVolumeType::CONVEXMESH = new EntityPhysicsSubScreenController_BoundingVolumeType("CONVEXMESH", 5);

EntityPhysicsSubScreenController_BoundingVolumeType* EntityPhysicsSubScreenController_BoundingVolumeType::valueOf(const string& name)
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
