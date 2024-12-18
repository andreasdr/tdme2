#pragma once

#include <memory>
#include <vector>

#include <reactphysics3d/collision/TriangleMesh.h>
#include <reactphysics3d/collision/TriangleVertexArray.h>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/physics/fwd-tdme.h>
#include <tdme/engine/primitives/BoundingVolume.h>
#include <tdme/engine/primitives/Triangle.h>
#include <tdme/engine/Transform.h>

using std::unique_ptr;
using std::vector;

using tdme::engine::physics::World;
using tdme::engine::primitives::TerrainMesh;
using tdme::engine::primitives::Triangle;
using tdme::engine::ObjectModel;
using tdme::engine::Transform;

/**
 * Terrain mesh physics primitive
 * @author Andreas Drewke
 */
class tdme::engine::primitives::TerrainMesh final
	: public BoundingVolume
{
private:
	vector<float> vertices;
	vector<int32_t> indices;
	unique_ptr<reactphysics3d::TriangleVertexArray> triangleVertexArray;
	reactphysics3d::TriangleMesh* triangleMesh { nullptr };

	// overridden methods
	void destroyCollisionShape() override;
	void createCollisionShape(World* world) override;

public:
	// forbid class copy
	FORBID_CLASS_COPY(TerrainMesh)

	/**
	 * Public constructor
	 */
	TerrainMesh();

	/**
	 * Public constructor
	 * @param model model
	 * @param transform transform
	 */
	TerrainMesh(ObjectModel* model, const Transform& transform = Transform());

	/**
	 * Destructor
	 */
	~TerrainMesh();

	// overrides
	void setScale(const Vector3& scale) override;
	void setTransform(const Transform& transform) override;
	BoundingVolume* clone() const override;
};
