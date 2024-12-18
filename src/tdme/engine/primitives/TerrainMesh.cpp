#include <tdme/engine/primitives/TerrainMesh.h>

#include <memory>
#include <vector>

#include <reactphysics3d/collision/shapes/ConcaveMeshShape.h>
#include <reactphysics3d/collision/TriangleMesh.h>
#include <reactphysics3d/collision/TriangleVertexArray.h>
#include <reactphysics3d/utils/Message.h>

#include <tdme/tdme.h>
#include <tdme/engine/physics/World.h>
#include <tdme/engine/primitives/BoundingVolume.h>
#include <tdme/engine/primitives/Triangle.h>
#include <tdme/engine/ObjectModel.h>
#include <tdme/utilities/Console.h>

using std::make_unique;
using std::to_string;
using std::unique_ptr;
using std::vector;

using tdme::engine::physics::World;
using tdme::engine::primitives::BoundingVolume;
using tdme::engine::primitives::TerrainMesh;
using tdme::engine::primitives::Triangle;
using tdme::engine::ObjectModel;
using tdme::utilities::Console;

TerrainMesh::TerrainMesh()
{
}

TerrainMesh::TerrainMesh(ObjectModel* model, const Transform& transform)
{
	// fetch vertices and indices
	vector<Triangle> triangles;
	vector<Vector3> indexedVertices;
	model->getTriangles(triangles);
	Vector3 vertexTransformed;
	for (const auto& triangle: triangles) {
		for (const auto& vertex: triangle.getVertices()) {
			vertexTransformed = transform.getTransformMatrix().multiply(vertex);
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
	destroyCollisionShape();
}

void TerrainMesh::setScale(const Vector3& scale) {
	//
	if (scale.equals(Vector3(1.0f, 1.0f, 1.0f)) == false) {
		Console::printLine("TerrainMesh::setScale(): != 1.0f: Not supported!");
	}
	// store new scale
	this->scale.set(scale);
}

void TerrainMesh::setTransform(const Transform& transform) {
	Console::printLine("TerrainMesh::setTransform(): Not supported!");
}

void TerrainMesh::destroyCollisionShape() {
	if (collisionShape == nullptr) return;
	this->world->physicsCommon.destroyConcaveMeshShape(static_cast<reactphysics3d::ConcaveMeshShape*>(collisionShape));
	this->world->physicsCommon.destroyTriangleMesh(triangleMesh);
	triangleVertexArray = nullptr;
	collisionShape = nullptr;
	triangleMesh = nullptr;
	triangleVertexArray = nullptr;
}

void TerrainMesh::createCollisionShape(World* world) {
	if (this->world != nullptr && this->world != world) {
		Console::printLine("TerrainMesh::createCollisionShape(): already attached to a world.");
	}
	this->world = world;

	// RP3D triangle vertex array
	triangleVertexArray = make_unique<reactphysics3d::TriangleVertexArray>(
		static_cast<uint32_t>(vertices.size() / 3),
		vertices.data(),
		3 * sizeof(float),
		static_cast<uint32_t>(indices.size() / 3),
		indices.data(),
		3 * sizeof(int),
		reactphysics3d::TriangleVertexArray::VertexDataType::VERTEX_FLOAT_TYPE,
		reactphysics3d::TriangleVertexArray::IndexDataType::INDEX_INTEGER_TYPE
	);

	// add the triangle vertex array to the triangle mesh
	vector<reactphysics3d::Message> messages;
	triangleMesh = world->physicsCommon.createTriangleMesh(*triangleVertexArray.get(), messages);

	// dump messages
	for (const auto& message: messages) {
		auto getMessageTypeText = [](const reactphysics3d::Message& message) -> const string {
			switch (message.type) {
				case reactphysics3d::Message::Type::Error:
					return "ERROR";
				case reactphysics3d::Message::Type::Warning:
					return "WARNING";
				case reactphysics3d::Message::Type::Information:
					return "INFORMATION";
			}
		};
		Console::printLine("TerrainMesh::createCollisionShape(): " + getMessageTypeText(message) + ": " + message.text);
	}

	// create the concave mesh shape
	collisionShape = world->physicsCommon.createConcaveMeshShape(triangleMesh);
}

TerrainMesh::BoundingVolume* TerrainMesh::clone() const {
	auto terrainMesh = new TerrainMesh();
	terrainMesh->vertices = vertices;
	terrainMesh->indices = indices;
	return terrainMesh;
}
