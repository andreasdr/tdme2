#pragma once

#include <string>

#include <tdme/tools/shared/controller/fwd-tdme.h>
#include <tdme/utilities/Enum.h>

using std::string;

using tdme::tools::shared::controller::PrototypePhysicsSubScreenController_BoundingVolumeType;
using tdme::utilities::Enum;

/**
 * Prototype physics sub screen controller bounding volume type enum
 */
class tdme::tools::shared::controller::PrototypePhysicsSubScreenController_BoundingVolumeType final: public Enum
{
public:
	static PrototypePhysicsSubScreenController_BoundingVolumeType* NONE;
	static PrototypePhysicsSubScreenController_BoundingVolumeType* SPHERE;
	static PrototypePhysicsSubScreenController_BoundingVolumeType* CAPSULE;
	static PrototypePhysicsSubScreenController_BoundingVolumeType* BOUNDINGBOX;
	static PrototypePhysicsSubScreenController_BoundingVolumeType* ORIENTEDBOUNDINGBOX;
	static PrototypePhysicsSubScreenController_BoundingVolumeType* CONVEXMESH;

	/**
	 * Public constructor
	 * @param name name
	 * @param ordinal ordinal
	 */
	PrototypePhysicsSubScreenController_BoundingVolumeType(const string& name, int ordinal);


	/**
	 * Returns enum object given by name
	 * @param name name
	 * @return enum object
	 */
	static PrototypePhysicsSubScreenController_BoundingVolumeType* valueOf(const string& name);

};
