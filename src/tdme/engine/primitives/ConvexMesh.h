#pragma once

#include <vector>

#include <ext/reactphysics3d/src/collision/PolyhedronMesh.h>
#include <ext/reactphysics3d/src/collision/PolygonVertexArray.h>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/primitives/BoundingVolume.h>
#include <tdme/engine/primitives/Triangle.h>
#include <tdme/math/Vector3.h>
#include <tdme/utils/fwd-tdme.h>


using std::vector;

using tdme::engine::primitives::BoundingVolume;
using tdme::engine::primitives::Triangle;
using tdme::engine::Object3DModel;
using tdme::math::Vector3;
using tdme::utils::ByteBuffer;
using tdme::utils::FloatBuffer;
using tdme::utils::IntBuffer;

/** 
 * Convex mesh physics primitive
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::primitives::ConvexMesh final
	: public BoundingVolume
{
private:
	vector<Vector3> vertices;
	vector<int> facesVerticesCount;
	vector<int> indices;

	vector<reactphysics3d::PolygonVertexArray::PolygonFace> faces;
	reactphysics3d::PolygonVertexArray* polygonVertexArray { nullptr };
	reactphysics3d::PolyhedronMesh* polyhedronMesh { nullptr };
	ByteBuffer* verticesByteBuffer { nullptr };
	ByteBuffer* indicesByteBuffer { nullptr };


	/**
	 * Public constructor
	 * @param vertices vertices
	 * @param facesVerticesCount faces vertices count
	 * @param indices indices
	 * @param scale scale
	 */
	ConvexMesh(const vector<Vector3>& vertices, const vector<int>& facesVerticesCount, const vector<int>& indices, const Vector3& scale = Vector3(1.0f, 1.0f, 1.0f));

	/**
	 * Checks if vertex lives on triangle plane
	 * @param triangle triangle
	 * @param vertex vertex
	 * @return if vertex lives on triangle plane
	 */
	bool isVertexOnTrianglePlane(Triangle& triangle, const Vector3& vertex);

	/**
	 * Checks if 2 triangles are adjacent
	 * @param triangle1 triangle 1
	 * @param triangle2 triangle 2
	 * @return if triangles are adjacent
	 */
	bool areTrianglesAdjacent(Triangle& triangle1, Triangle& triangle2);

	/**
	 * Create convex mesh
	 * 	Note: it also translates center into origin
	 * @param vertices vertices
	 * @param facesVerticesCount faces vertices count
	 * @param indices indices
	 * @param scale scale
	 */
	void createConvexMesh(const vector<Vector3>& vertices, const vector<int>& facesVerticesCount, const vector<int>& indices, const Vector3& scale);

public:
	// overriden methods
	void setScale(const Vector3& scale) override;
	BoundingVolume* clone() const override;

	/**
	 * @return vertices
	 */
	const vector<Vector3>& getVertices();

	/**
	 * Public constructor
	 */
	ConvexMesh();

	/**
	 * Public constructor
	 * @param model model
	 * @param scale scale
	 */
	ConvexMesh(Object3DModel* model, const Vector3& scale = Vector3(1.0f, 1.0f, 1.0f));

	/**
	 * Public denstructor
	 */
	~ConvexMesh();

};
