#include <tdme/engine/primitives/HeightMap.h>

#include <ext/reactphysics3d/src/collision/shapes/HeightFieldShape.h>
#include <ext/reactphysics3d/src/mathematics/Vector3.h>

#include <tdme/math/Vector3.h>
#include <tdme/utilities/Console.h>

using std::to_string;

using tdme::engine::primitives::HeightMap;
using tdme::math::Vector3;
using tdme::utilities::Console;

HeightMap::HeightMap(
	int columns,
	int rows,
	float minHeight,
	float maxHeight,
	float* heightValues,
	const Vector3& scale
) {
	this->scale.set(scale);
	this->columns = columns;
	this->rows = rows;
	this->minHeight = minHeight;
	this->maxHeight = maxHeight;
	this->heightValues = heightValues;
	collisionShape = new reactphysics3d::HeightFieldShape(
		columns,
		rows,
		minHeight,
		maxHeight,
		heightValues,
		reactphysics3d::HeightFieldShape::HeightDataType::HEIGHT_FLOAT_TYPE,
		1,
		1.0f,
		reactphysics3d::Vector3(scale.getX(), scale.getY(), scale.getZ())
	);
	computeBoundingBox();
}

void HeightMap::setScale(const Vector3& scale) {
	Console::println("HeightMap::setScale(): not supported!");
}

BoundingVolume* HeightMap::clone() const
{
	return new HeightMap(columns, rows, minHeight, maxHeight, heightValues);
}

