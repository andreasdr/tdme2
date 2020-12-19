#pragma once

#include <tdme/tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector3.h>

using tdme::math::Matrix4x4;
using tdme::math::Vector3;

/**
 * Simple class to determine if a transform is negative
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::math::Matrix4x4Negative
{
private:
	Vector3 xAxis;
	Vector3 yAxis;
	Vector3 zAxis;
	Vector3 tmpAxis;

public:
	/**
	 * Public constructor
	 */
	inline Matrix4x4Negative() {
	}

	/**
	 * Check if matrix is negative
	 * @param matrix matrix
	 * @return negative
	 */
	inline bool isNegative(Matrix4x4& matrix) {
		// check if negative scale and rotation
		auto& transformationsMatrixData = matrix.getArray();
		// copy into x,y,z axes
		xAxis.set(transformationsMatrixData[0], transformationsMatrixData[1], transformationsMatrixData[2]);
		yAxis.set(transformationsMatrixData[4], transformationsMatrixData[5], transformationsMatrixData[6]);
		zAxis.set(transformationsMatrixData[8], transformationsMatrixData[9], transformationsMatrixData[10]);
		// check if inverted/negative transformation
		return Vector3::computeDotProduct(Vector3::computeCrossProduct(xAxis, yAxis, tmpAxis), zAxis) < 0.0f;
	}

};
