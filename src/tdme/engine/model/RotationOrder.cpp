#include <tdme/engine/model/RotationOrder.h>

#include <tdme/math/Vector3.h>
#include <tdme/utilities/Enum.h>

using tdme::engine::model::RotationOrder;
using tdme::math::Vector3;
using tdme::utilities::Enum;

RotationOrder::RotationOrder(const string& name, int ordinal, const Vector3& axis0, const Vector3& axis1, const Vector3& axis2, int32_t axis0VectorIndex, int32_t axis1VectorIndex, int32_t axis2VectorIndex, int32_t axisXIndex, int32_t axisYIndex, int32_t axisZIndex) : Enum(name, ordinal)
{
	this->axis0 = axis0;
	this->axis1 = axis1;
	this->axis2 = axis2;
	this->axis0VectorIndex = axis0VectorIndex;
	this->axis1VectorIndex = axis1VectorIndex;
	this->axis2VectorIndex = axis2VectorIndex;
	this->axisXIndex = axisXIndex;
	this->axisYIndex = axisYIndex;
	this->axisZIndex = axisZIndex;
}

RotationOrder* tdme::engine::model::RotationOrder::XYZ = new RotationOrder("XYZ", 0, Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f), 0, 1, 2, 0, 1, 2);
RotationOrder* tdme::engine::model::RotationOrder::YZX = new RotationOrder("YZX", 1, Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f), Vector3(1.0f, 0.0f, 0.0f), 1, 2, 0, 2, 0, 1);
RotationOrder* tdme::engine::model::RotationOrder::ZYX = new RotationOrder("ZYX", 2, Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 1.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), 2, 1, 0, 2, 1, 0);

RotationOrder* RotationOrder::valueOf(const string& a0)
{
	if (XYZ->getName() == a0) return XYZ;
	if (YZX->getName() == a0) return YZX;
	if (ZYX->getName() == a0) return ZYX;
	// TODO: throw exception here maybe
	return nullptr;
}
