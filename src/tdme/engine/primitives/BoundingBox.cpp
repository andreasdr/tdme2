#include <tdme/engine/primitives/BoundingBox.h>

#include <vector>

#include <tdme/engine/primitives/OrientedBoundingBox.h>
#include <tdme/engine/Transformations.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector3.h>

using std::vector;

using tdme::engine::primitives::BoundingBox;
using tdme::engine::primitives::OrientedBoundingBox;
using tdme::engine::Transformations;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;

const array<int32_t, 3> BoundingBox::FACE0_INDICES = {{ 0, 4, 7 }};
const array<int32_t, 3> BoundingBox::FACE1_INDICES = {{ 7, 3, 0 }};
const array<int32_t, 3> BoundingBox::FACE2_INDICES = {{ 6, 5, 1 }};
const array<int32_t, 3> BoundingBox::FACE3_INDICES = {{ 1, 2, 6 }};
const array<int32_t, 3> BoundingBox::FACE4_INDICES = {{ 5, 4, 0 }};
const array<int32_t, 3> BoundingBox::FACE5_INDICES = {{ 0, 1, 5 }};
const array<int32_t, 3> BoundingBox::FACE6_INDICES = {{ 3, 7, 6 }};
const array<int32_t, 3> BoundingBox::FACE7_INDICES = {{ 6, 2, 3 }};
const array<int32_t, 3> BoundingBox::FACE8_INDICES = {{ 2, 1, 0 }};
const array<int32_t, 3> BoundingBox::FACE9_INDICES = {{ 0, 3, 2 }};
const array<int32_t, 3> BoundingBox::FACE10_INDICES = {{ 4, 5, 6 }};
const array<int32_t, 3> BoundingBox::FACE11_INDICES = {{ 6, 7, 4 }};
const array<array<int32_t,3>,12> BoundingBox::facesVerticesIndexes =
{{
	FACE0_INDICES, FACE1_INDICES, FACE2_INDICES, FACE3_INDICES,
	FACE4_INDICES, FACE5_INDICES, FACE6_INDICES, FACE7_INDICES,
	FACE8_INDICES, FACE9_INDICES, FACE10_INDICES, FACE11_INDICES
}};

BoundingBox::BoundingBox()
{
	vertices.resize(8);
	update();
}

BoundingBox::BoundingBox(BoundingBox* boundingBox)
{
	vertices.resize(8);
	this->min.set(boundingBox->min);
	this->max.set(boundingBox->max);
	update();
}

BoundingBox::BoundingBox(OrientedBoundingBox* obb)
{
	vertices.resize(8);
	min = obb->getCenter();
	max = obb->getCenter();
	for (auto& vertex: obb->getVertices()) {
		extend(vertex);
	}
}

BoundingBox::BoundingBox(const Vector3& min, const Vector3& max)
{
	vertices.resize(8);
	this->min.set(min);
	this->max.set(max);
	update();
}

void BoundingBox::fromBoundingVolume(BoundingBox* boundingBox)
{
	min = boundingBox->min;
	max = boundingBox->max;
	vertices = boundingBox->vertices;
	center = boundingBox->center;
	dimensions = boundingBox->dimensions;
}

void BoundingBox::fromBoundingVolumeWithTransformations(BoundingBox* boundingBox, const Transformations& transformations)
{
	// apply transformations from original vertices to local vertices
	auto& transformationsMatrix = transformations.getTransformationsMatrix();
	auto _vertices = boundingBox->getVertices();
	for (auto i = 0; i < vertices.size(); i++) {
		vertices[i] = transformationsMatrix.multiply(_vertices[i]);
	}
	// determine axis aligned bounding box constraints based on local vertices
	auto& vertexXYZ = vertices[0].getArray();
	float minX = vertexXYZ[0], minY = vertexXYZ[1], minZ = vertexXYZ[2];
	float maxX = vertexXYZ[0], maxY = vertexXYZ[1], maxZ = vertexXYZ[2];
	for (auto vertexIndex = 1; vertexIndex < vertices.size(); vertexIndex++) {
		auto& vertex = vertices[vertexIndex];
		vertexXYZ = vertex.getArray();
		if (vertexXYZ[0] < minX) minX = vertexXYZ[0];
		if (vertexXYZ[1] < minY) minY = vertexXYZ[1];
		if (vertexXYZ[2] < minZ) minZ = vertexXYZ[2];
		if (vertexXYZ[0] > maxX) maxX = vertexXYZ[0];
		if (vertexXYZ[1] > maxY) maxY = vertexXYZ[1];
		if (vertexXYZ[2] > maxZ) maxZ = vertexXYZ[2];
	}
	// set up new aabb
	min.set(minX, minY, minZ);
	max.set(maxX, maxY, maxZ);
	// compute new vertices based on aabb constraints
	update();
}

void BoundingBox::update() {
	auto& minXYZ = min.getArray();
	auto& maxXYZ = max.getArray();
	// near, left, top
	vertices[0].set(minXYZ[0], minXYZ[1], minXYZ[2]);
	// near, right, top
	vertices[1].set(maxXYZ[0], minXYZ[1], minXYZ[2]);
	// near, right, bottom
	vertices[2].set(maxXYZ[0], maxXYZ[1], minXYZ[2]);
	// near, left, bottom
	vertices[3].set(minXYZ[0], maxXYZ[1], minXYZ[2]);
	// far, left, top
	vertices[4].set(minXYZ[0], minXYZ[1], maxXYZ[2]);
	// far, right, top
	vertices[5].set(maxXYZ[0], minXYZ[1], maxXYZ[2]);
	// far, right, bottom
	vertices[6].set(maxXYZ[0], maxXYZ[1], maxXYZ[2]);
	// far, left, bottom
	vertices[7].set(minXYZ[0], maxXYZ[1], maxXYZ[2]);
	center.set(min).add(max).scale(0.5f);
	dimensions.set(max).sub(min);
}


