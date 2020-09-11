#pragma once

#include <string>

#include <tdme/tools/shared/controller/fwd-tdme.h>
#include <tdme/utilities/Enum.h>

using std::string;

using tdme::utilities::Enum;
using tdme::tools::shared::controller::EntityPhysicsSubScreenController;
using tdme::tools::shared::controller::EntityPhysicsSubScreenController_BoundingVolumeType;

class tdme::tools::shared::controller::EntityPhysicsSubScreenController_BoundingVolumeType final
	: public Enum
{
	friend class EntityPhysicsSubScreenController;
	friend class EntityPhysicsSubScreenController_onBoundingVolumeConvexMeshFile;

public: /* package */
	static EntityPhysicsSubScreenController_BoundingVolumeType *NONE;
	static EntityPhysicsSubScreenController_BoundingVolumeType *SPHERE;
	static EntityPhysicsSubScreenController_BoundingVolumeType *CAPSULE;
	static EntityPhysicsSubScreenController_BoundingVolumeType *BOUNDINGBOX;
	static EntityPhysicsSubScreenController_BoundingVolumeType *ORIENTEDBOUNDINGBOX;
	static EntityPhysicsSubScreenController_BoundingVolumeType *CONVEXMESH;
	EntityPhysicsSubScreenController_BoundingVolumeType(const string& name, int ordinal);

public:
	static EntityPhysicsSubScreenController_BoundingVolumeType* valueOf(const string& a0);

};
