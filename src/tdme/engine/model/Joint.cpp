#include <tdme/engine/model/Joint.h>

#include <tdme/math/Matrix4x4.h>

using tdme::engine::model::Joint;
using tdme::math::Matrix4x4;

Joint::Joint()
{
	this->nodeId.clear();
	this->bindMatrix.identity();
}

Joint::Joint(const string& nodeId)
{
	this->nodeId = nodeId;
	this->bindMatrix.identity();
}
