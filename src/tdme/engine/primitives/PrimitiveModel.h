#pragma once

#include <map>
#include <string>

#include <ext/reactphysics3d/src/mathematics/Vector3.h>

#include <tdme/tdme.h>
#include <tdme/math/Vector3.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/primitives/fwd-tdme.h>
#include <tdme/engine/primitives/BoundingVolume.h>
#include <tdme/utilities/fwd-tdme.h>

using std::map;
using std::string;

using tdme::engine::model::Material;
using tdme::engine::model::Model;
using tdme::engine::model::Node;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::primitives::BoundingVolume;
using tdme::engine::primitives::Capsule;
using tdme::engine::primitives::OrientedBoundingBox;
using tdme::engine::primitives::Sphere;

/**
 * Helper class to create models from physics primitive bounding volumes
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::primitives::PrimitiveModel final
{
private:
	static constexpr int32_t SPHERE_SEGMENTS_X { 20 };
	static constexpr int32_t SPHERE_SEGMENTS_Y { 20 };
	static constexpr int32_t CAPSULE_SEGMENTS_X { 20 };
	static constexpr int32_t CAPSULE_SEGMENTS_Y { 20 };

	/**
	 * Converts a TDME2 vector to ReactPhysics3D vector
	 * @param vector tdme vector
	 * @returns reactphysics3d vector
	 */
	inline static reactphysics3d::Vector3 toRP3DVector3(const Vector3& vector) {
		return reactphysics3d::Vector3(vector.getX(), vector.getY(), vector.getZ());
	}

	/**
	 * Transforms a given ReactPhysics3D vector with bounding volume transform
	 * @param boundingVolume bounding volume
	 * @param vector vector
	 * @return transformed vector
	 */
	inline static Vector3 transformVector3(BoundingVolume* boundingVolume, const reactphysics3d::Vector3& vector) {
		//	Note: there is no hurry as LE and ME do not use scale for level editor entity bounding volumes
		auto vectorTransformed = boundingVolume->collisionShapeTransform * boundingVolume->collisionShapeLocalTransform * (vector * toRP3DVector3(boundingVolume->scale));
		return Vector3(vectorTransformed.x, vectorTransformed.y, vectorTransformed.z);
	}

	/**
	 * Transforms a given ReactPhysics3D vector with bounding volume transform
	 * @param boundingVolume bounding volume
	 * @param normal vector
	 * @return transformed vector
	 */
	inline static Vector3 transformVector3Normal(BoundingVolume* boundingVolume, const reactphysics3d::Vector3& normal) {
		//	Note: there is no hurry as LE and ME do not use scale for level editor entity bounding volumes
		auto normalTransformed = boundingVolume->collisionShapeTransform.getOrientation() * boundingVolume->collisionShapeLocalTransform.getOrientation() * (normal * toRP3DVector3(boundingVolume->scale));
		normalTransformed.normalize();
		return Vector3(normalTransformed.x, normalTransformed.y, normalTransformed.z);
	}

	/**
	 * Set up convex mesh material
	 * @param nodes nodes
	 * @param material material
	 */
	static void setupConvexMeshMaterial(const map<string, Node*>& nodes, Material* material);

public:
	/**
	 * Creates a model from bounding box
	 * @param boundingBox bounding box
	 * @param id id
	 * @return model
	 */
	static Model* createBoundingBoxModel(BoundingBox* boundingBox, const string& id);

	/**
	 * Creates a model from oriented bounding box
	 * @param orientedBoundingBox bounding box
	 * @param id id
	 * @return model
	 */
	static Model* createOrientedBoundingBoxModel(OrientedBoundingBox* orientedBoundingBox, const string& id);

	/**
	 * Creates a model from oriented bounding box
	 * @param sphere sphere
	 * @param id id
	 * @param segmentsX number of x segments
	 * @param segmentsY number of y segments
	 * @return model
	 */
	static Model* createSphereModel(Sphere* sphere, const string& id, int32_t segmentsX, int32_t segmentsY);

	/**
	 * Creates a model from capsule
	 * @param capsule sphere
	 * @param id id
	 * @param segmentsX number of x segments
	 * @param segmentsY number of y segments
	 * @return model
	 */
	static Model* createCapsuleModel(Capsule* capsule, const string& id, int32_t segmentsX, int32_t segmentsY);

	/**
	 * Creates a model from convex mesh
	 * @param mesh convex mesh
	 * @param id id
	 * @return model
	 */
	static Model* createConvexMeshModel(ConvexMesh* mesh, const string& id);

	/**
	 * Set up a convex mesh model
	 * @param model model
	 */
	static void setupConvexMeshModel(Model* model);

public:
	/**
	 * Creates a model from bounding volume
	 * @param boundingVolume bounding volume
	 * @param id id
	 * @return model
	 */
	static Model* createModel(BoundingBox* boundingVolume, const string& id);

	/**
	 * Creates a model from bounding volume
	 * @param boundingVolume bounding box
	 * @param id id
	 * @return model
	 */
	static Model* createModel(BoundingVolume* boundingVolume, const string& id);
};
