#include <tdme/engine/primitives/TerrainMesh.h>

#include <vector>

#include <ext/reactphysics3d/src/collision/shapes/ConcaveMeshShape.h>
#include <ext/reactphysics3d/src/collision/TriangleMesh.h>
#include <ext/reactphysics3d/src/collision/TriangleVertexArray.h>

#include <tdme/engine/primitives/BoundingVolume.h>
#include <tdme/engine/primitives/Triangle.h>
#include <tdme/engine/Object3DModel.h>
#include <tdme/utilities/Console.h>

using std::to_string;
using std::vector;

using tdme::engine::primitives::BoundingVolume;
using tdme::engine::primitives::TerrainMesh;
using tdme::engine::primitives::Triangle;
using tdme::engine::Object3DModel;
using tdme::utilities::Console;

TerrainMesh::TerrainMesh()
{
}

TerrainMesh::TerrainMesh(Object3DModel* model, const Transformations& transformations)
{
	// fetch vertices and indices
	vector<Triangle> triangles;
	vector<Vector3> indexedVertices;
	model->getTriangles(triangles);
	Vector3 vertexTransformed;
	for (auto& triangle: triangles) {
		for (const auto& vertex: triangle.getVertices()) {
			vertexTransformed = transformations.getTransformationsMatrix().multiply(vertex);
			auto i = 0;
			for (; i < indexedVertices.size(); i++) {
				if (indexedVertices[i].equals(vertexTransformed) == true) break;
			}
			if (i == indexedVertices.size()) {
				indexedVertices.push_back(vertexTransformed);
				vertices.push_back(vertexTransformed[0]);
				vertices.push_back(vertexTransformed[1]);
				vertices.push_back(vertexTransformed[2]);
			}
			indices.push_back(i);
		}
	}
	vertices.shrink_to_fit();
	indices.shrink_to_fit();
	setScale(Vector3(1.0f, 1.0f, 1.0f));
}

TerrainMesh::~TerrainMesh() {
	if (triangleMesh != nullptr) delete triangleMesh;
	if (triangleVertexArray != nullptr) delete triangleVertexArray;
}

void TerrainMesh::setScale(const Vector3& scale) {
	if (scale.equals(Vector3(1.0f, 1.0f, 1.0f)) == false) {
		Console::println("TerrainMesh::setScale(): != 1.0f: Not supported!");
	}
	// store new scale
	this->scale.set(scale);

	// delete old collision shape
	if (collisionShape != nullptr) delete collisionShape;
	if (triangleMesh != nullptr) delete triangleMesh;
	if (triangleVertexArray != nullptr) delete triangleVertexArray;

	// RP3D triangle vertex array
	triangleVertexArray = new reactphysics3d::TriangleVertexArray(
		vertices.size() / 3,
		vertices.data(),
		3 * sizeof(float),
		indices.size() / 3,
		indices.data(),
		3 * sizeof(int),
		reactphysics3d::TriangleVertexArray::VertexDataType::VERTEX_FLOAT_TYPE,
		reactphysics3d::TriangleVertexArray::IndexDataType::INDEX_INTEGER_TYPE
	);

	// add the triangle vertex array to the triangle mesh
	triangleMesh = new reactphysics3d::TriangleMesh();
	triangleMesh->addSubpart(triangleVertexArray);

	// create the concave mesh shape
	collisionShape = new reactphysics3d::ConcaveMeshShape(triangleMesh);

	// compute bounding box
	computeBoundingBox();

	//
	vertices.clear();
	indices.clear();;
}

void TerrainMesh::fromTransformations(const Transformations& transformations) {
	Console::println("TerrainMesh::fromTransformations(): Not supported!");
}

TerrainMesh::BoundingVolume* TerrainMesh::clone() const {
	Console::println("TerrainMesh::clone(): Not supported!");
	return nullptr;
}
