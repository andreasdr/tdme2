#include <tdme/engine/primitives/ConvexMesh.h>

#include <algorithm>
#include <array>
#include <map>
#include <string>
#include <unordered_set>
#include <vector>

#include <ext/reactphysics3d/src/collision/shapes/ConvexMeshShape.h>

#include <tdme/engine/Object3DModel.h>
#include <tdme/engine/Transformations.h>
#include <tdme/engine/model/ModelHelper.h>
#include <tdme/engine/model/ModelHelper_VertexOrder.h>
#include <tdme/engine/primitives/BoundingVolume.h>
#include <tdme/engine/primitives/LineSegment.h>
#include <tdme/engine/primitives/Triangle.h>
#include <tdme/math/Math.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector3.h>
#include <tdme/utils/ByteBuffer.h>
#include <tdme/utils/Float.h>
#include <tdme/utils/FloatBuffer.h>
#include <tdme/utils/IntBuffer.h>

using std::array;
using std::find;
using std::reverse;
using std::map;
using std::sort;
using std::string;
using std::to_string;
using std::unique;
using std::unordered_set;
using std::vector;

using tdme::engine::primitives::ConvexMesh;
using tdme::engine::Object3DModel;
using tdme::engine::Transformations;
using tdme::engine::model::ModelHelper;
using tdme::engine::model::ModelHelper_VertexOrder;
using tdme::engine::primitives::BoundingVolume;
using tdme::engine::primitives::LineSegment;
using tdme::engine::primitives::Triangle;
using tdme::math::Math;
using tdme::math::Vector3;
using tdme::utils::Float;
using tdme::utils::FloatBuffer;
using tdme::utils::IntBuffer;

ConvexMesh::ConvexMesh()
{
}

ConvexMesh::~ConvexMesh()
{
	if (polyhedronMesh != nullptr) delete polyhedronMesh;
	if (polygonVertexArray != nullptr) delete polygonVertexArray;
	if (verticesByteBuffer != nullptr) delete verticesByteBuffer;
	if (indicesByteBuffer != nullptr) delete indicesByteBuffer;
}

inline bool ConvexMesh::isVertexOnTrianglePlane(Triangle& triangle, const Vector3& vertex) {
	for (auto& triangleVertex: triangle.getVertices()) {
		if (triangleVertex.equals(vertex) == true) return true;
	}
	// see: http://www.ambrsoft.com/TrigoCalc/Plan3D/PointsCoplanar.htm
	Vector3 v1;
	Vector3 v2;
	Vector3 v3;
	Vector3 v2v3Cross;
	v1.set(triangle.getVertices()[1]).sub(triangle.getVertices()[0]).normalize();
	v2.set(triangle.getVertices()[2]).sub(triangle.getVertices()[0]).normalize();
	v3.set(vertex).sub(triangle.getVertices()[0]);
	auto v1Dotv2v3Cross = Vector3::computeDotProduct(v1, Vector3::computeCrossProduct(v2, v3, v2v3Cross).normalize());
	return Math::abs(v1Dotv2v3Cross) < Math::EPSILON;
}

inline bool ConvexMesh::areTrianglesAdjacent(Triangle& triangle1, Triangle& triangle2) {
	auto equalVertices = 0;
	for (auto& triangle1Vertex: triangle1.getVertices()) {
		for (auto& triangle2Vertex: triangle2.getVertices()) {
			if (triangle1Vertex.equals(triangle2Vertex)) equalVertices++;
		}
	}
	return equalVertices == 2;
}

void ConvexMesh::createConvexMesh(const vector<Vector3>& vertices, const vector<int>& facesVerticesCount, const vector<int>& indices, const Vector3& scale) {
	// delete old collision shape if we have any
	if (collisionShape != nullptr) delete collisionShape;
	if (polyhedronMesh != nullptr) delete polyhedronMesh;
	if (polygonVertexArray != nullptr) delete polygonVertexArray;
	if (verticesByteBuffer != nullptr) delete verticesByteBuffer;
	if (indicesByteBuffer != nullptr) delete indicesByteBuffer;

	// check if local translation is given
	// determine center/position transformed
	collisionShapeLocalTranslation.set(0.0f, 0.0f, 0.0f);
	for (auto vertexIdx: indices) {
		auto& vertex = vertices[vertexIdx];
		collisionShapeLocalTranslation.add(vertex);
	}
	collisionShapeLocalTranslation.scale(1.0f / indices.size());

	// center
	center.set(collisionShapeLocalTranslation).scale(scale);

	// scale collision shape local translation
	collisionShapeLocalTranslation.scale(scale);

	// local transformations
	collisionShapeLocalTransform.setPosition(reactphysics3d::Vector3(collisionShapeLocalTranslation.getX(), collisionShapeLocalTranslation.getY(), collisionShapeLocalTranslation.getZ()));

	// generate vertices and indices buffers
	verticesByteBuffer = ByteBuffer::allocate(vertices.size() * 3 * sizeof(float));
	indicesByteBuffer = ByteBuffer::allocate(indices.size() * sizeof(int));
	auto verticesBuffer = verticesByteBuffer->asFloatBuffer();
	auto indicesBuffer = indicesByteBuffer->asIntBuffer();
	Vector3 vertexTransformed;
	for (auto& vertex: vertices) {
		vertexTransformed.set(vertex);
		vertexTransformed.sub(center);
		vertexTransformed.scale(scale);
		verticesBuffer.put(vertexTransformed.getArray());
	}
	for (auto& index: indices) {
		indicesBuffer.put(index);
	}
	faces.clear();
	int indexIdx = 0;
	for (auto faceVerticesCount: facesVerticesCount) {
		reactphysics3d::PolygonVertexArray::PolygonFace face;
		face.nbVertices = faceVerticesCount;
		face.indexBase = indexIdx;
		faces.push_back(face);
		indexIdx+= faceVerticesCount;
	}

	//
	polygonVertexArray = new reactphysics3d::PolygonVertexArray(
		vertices.size(),
		verticesByteBuffer->getBuffer(),
		3 * sizeof(float),
		indicesByteBuffer->getBuffer(),
		sizeof(int),
		faces.size(),
		faces.data(),
		reactphysics3d::PolygonVertexArray::VertexDataType::VERTEX_FLOAT_TYPE,
		reactphysics3d::PolygonVertexArray::IndexDataType::INDEX_INTEGER_TYPE
	);
	polyhedronMesh = new reactphysics3d::PolyhedronMesh(polygonVertexArray);
	// create convex mesh shape
	auto convexMeshShape = new reactphysics3d::ConvexMeshShape(polyhedronMesh);
	// set up new collision shape
	collisionShape = convexMeshShape;
	// transform
	collisionShapeTransform = reactphysics3d::Transform();
	// compute bounding box
	computeBoundingBox();
}

ConvexMesh::ConvexMesh(Object3DModel* model, const Vector3& scale)
{
	vector<Triangle> triangles;
	model->getTriangles(triangles);

	// determine coplanar faces of model
	map<int, vector<Triangle*>> trianglesCoplanar;
	{
		auto triangle1Idx = 0;
		unordered_set<int> trianglesProcessed;

		for (auto& triangle1: triangles) {
			if (trianglesProcessed.find(triangle1Idx) != trianglesProcessed.end()) {
				triangle1Idx++;
				continue;
			}
			trianglesCoplanar[triangle1Idx].push_back(&triangle1);
			trianglesProcessed.insert(triangle1Idx);
			auto triangle2Added = 0;
			do {
				auto triangle2Idx = 0;
				triangle2Added = 0;
				for (auto& triangle2: triangles) {
					if (trianglesProcessed.find(triangle2Idx) != trianglesProcessed.end()) {
						triangle2Idx++;
						continue;
					}
					auto adjacent = areTrianglesAdjacent(triangle1, triangle2);
					auto triangle2OnTriangle1Plane =
						isVertexOnTrianglePlane(triangle1, triangle2.getVertices()[0]) &&
						isVertexOnTrianglePlane(triangle1, triangle2.getVertices()[1]) &&
						isVertexOnTrianglePlane(triangle1, triangle2.getVertices()[2]);
					if (adjacent == true && triangle2OnTriangle1Plane == true) {
						trianglesCoplanar[triangle1Idx].push_back(&triangle2);
						trianglesProcessed.insert(triangle2Idx);
						triangle2Added++;
					}
					triangle2Idx++;
				}
			} while (triangle2Added > 0);
			triangle1Idx++;
		}
	}

	// iterate triangles that are coplanar and build a polygon
	for (auto& trianglesCoplanarIt: trianglesCoplanar) {
		auto& trianglesCoplanarVector = trianglesCoplanarIt.second;

		// collect polygon vertices
		vector<Vector3> polygonVertices;

		// determine polygon vertices
		for (auto& triangle: trianglesCoplanarVector) {
			for (auto& triangleVertex: triangle->getVertices()) {
				bool foundVertex = false;
				for (auto& polygonVertex: polygonVertices) {
					if (polygonVertex.equals(triangleVertex) == true) {
						foundVertex = true;
						break;
					}
				}
				if (foundVertex == false) {
					polygonVertices.push_back(triangleVertex);
				}
			}
		}

		// remove vertices that live on the line 2 other 2 vertices span
		/*
		{
			vector<int> polygonVerticesToRemove;
			Vector3 c;
			for (auto i = 0; i < polygonVertices.size(); i++) {
				for (auto j = 0; j < polygonVertices.size(); j++) {
					if (i == j) continue;
					for (auto k = 0; k < polygonVertices.size(); k++) {
						if (i == k || j == k) continue;
						LineSegment::computeClosestPointOnLineSegment(
							polygonVertices[i],
							polygonVertices[j],
							polygonVertices[k],
							c
						);
						if (polygonVertices[k].equals(c) == true) polygonVerticesToRemove.push_back(k);
					}
				}
			}
			sort(polygonVerticesToRemove.begin(), polygonVerticesToRemove.end());
			polygonVerticesToRemove.erase(unique(polygonVerticesToRemove.begin(), polygonVerticesToRemove.end()), polygonVerticesToRemove.end());
			auto polygonVerticesToRemoved = 0;
			for (auto i: polygonVerticesToRemove) {
				polygonVertices.erase(polygonVertices.begin() + i - polygonVerticesToRemoved);
				polygonVerticesToRemoved++;
			}
		}
		*/

		// check if to skip as combined polygons could already have the current polygon
		if (polygonVertices.size() > 2) {
			auto skip = false;
			auto idx = 0;
			for (auto faceVertexCount: facesVerticesCount) {
				unordered_set<int> foundIndices;
				for (auto i = 0; i < faceVertexCount; i++) {
					auto foundVertex = false;
					for (auto& polygonVertex: polygonVertices) {
						if (polygonVertex.equals(vertices[indices[idx]]) == true) {
							foundIndices.insert(indices[idx]);
							break;
						}
					}
					idx++;
				}
				if (foundIndices.size() == polygonVertices.size()) {
					skip = true;
					break;
				}
			}
			if (skip == true) {
				continue;
			}
		}

		//
		if (polygonVertices.size() < 3) continue;

		// determine polygon center, a point outside of mesh viewing the polygon
		Vector3 polygonCenter;
		for (auto& polygonVertex: polygonVertices) {
			polygonCenter.add(polygonVertex);
		}
		polygonCenter.scale(1.0f / polygonVertices.size());

		// plane normal
		Vector3 triangle1Edge1;
		Vector3 triangle1Edge2;
		Vector3 polygonNormal;
		triangle1Edge1.set(trianglesCoplanarVector[0]->getVertices()[1]).sub(trianglesCoplanarVector[0]->getVertices()[0]).normalize();
		triangle1Edge2.set(trianglesCoplanarVector[0]->getVertices()[2]).sub(trianglesCoplanarVector[0]->getVertices()[0]).normalize();
		Vector3::computeCrossProduct(triangle1Edge1, triangle1Edge2, polygonNormal).normalize();

		// determine polygon vertices order
		vector<int> polygonVerticesOrdered;
		// add first vertex
		polygonVerticesOrdered.push_back(0);

		// then check vertex order if it matches
		// if it matches we have the next vertex
		Vector3 distanceVector;
		// as long as we have vertices left
		while (polygonVerticesOrdered.size() != polygonVertices.size()) {
			// find next vertex with most little
			auto hitVertexAngle = 0.0f;
			auto hitVertexIdx = -1;
			for (int i = 0; i < polygonVertices.size(); i++) {
				// check if already add to ordered vertices list
				if (find(polygonVerticesOrdered.begin(), polygonVerticesOrdered.end(), i) != polygonVerticesOrdered.end()) continue;

				// otherwise check if angle is smaller
				auto angleCurrent = Vector3::computeAngle(
					polygonVertices[0].clone().sub(polygonCenter).normalize(),
					polygonVertices[i].clone().sub(polygonCenter).normalize(),
					polygonNormal
				);
				if (hitVertexIdx == -1 || angleCurrent < hitVertexAngle) {
					hitVertexAngle = angleCurrent;
					hitVertexIdx = i;
				}
			}
			// yep
			polygonVerticesOrdered.push_back(hitVertexIdx);
		}

		/*
		{
			// vertex order
			// 	see: https://stackoverflow.com/questions/14370636/sorting-a-list-of-3d-coplanar-points-to-be-clockwise-or-counterclockwise
			auto& polygonVertexOrderedFirst = polygonVertices[polygonVerticesOrdered[0]];
			auto& polygonVertexOrderedLast = polygonVertices[polygonVerticesOrdered[1]];
			Vector3 ac;
			Vector3 bc;
			Vector3 acbcCross;
			ac.set(polygonVertexOrderedFirst).sub(polygonCenter);
			bc.set(polygonVertexOrderedLast).sub(polygonCenter);
			Vector3::computeCrossProduct(ac, bc, acbcCross);
			// counter clockwise???
			if ((Vector3::computeDotProduct(polygonNormal, acbcCross) > 0.0f) == false) {
				// yep, reverse
				reverse(begin(polygonVerticesOrdered), end(polygonVerticesOrdered));
			}
		}
		*/

		// add face
		facesVerticesCount.push_back(polygonVerticesOrdered.size());
		for (auto polygonVerticesOrderedIdx: polygonVerticesOrdered) {
			// polygon vertex
			auto& polygonVertex = polygonVertices[polygonVerticesOrderedIdx];

			// check if to insert vertex
			int vertexIdx = 0;
			for (auto& vertexExisting: vertices) {
				if (vertexExisting.equals(polygonVertex) == true) {
					break;
				}
				vertexIdx++;
			}
			if (vertexIdx == vertices.size()) {
				vertices.push_back(polygonVertex);
			}

			// add index
			indices.push_back(vertexIdx);
		}
	}

	// create convex mesh
	setScale(scale);
}

ConvexMesh::ConvexMesh(const vector<Vector3>& vertices, const vector<int>& facesVerticesCount, const vector<int>& indices, const Vector3& scale) {
	this->vertices = vertices;
	this->facesVerticesCount = facesVerticesCount;
	this->indices = indices;
	setScale(scale);
}

void ConvexMesh::setScale(const Vector3& scale) {
	// store new scale
	this->scale.set(scale);
	// recreate convex mesh
	createConvexMesh(vertices, facesVerticesCount, indices, scale);
}

BoundingVolume* ConvexMesh::clone() const
{
	return new ConvexMesh(vertices, facesVerticesCount, indices, scale);
}

const vector<Vector3>& ConvexMesh::getVertices() {
	return vertices;
}
