#pragma once

#include <array>

#include <tdme/tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Math.h>
#include <tdme/math/Vector2.h>

using std::array;

using tdme::math::Math;
using tdme::math::Vector2;

/**
 * 3x3 2D Matrix class
 * @see http://learnwebgl.brown37.net/10_surface_properties/texture_mapping_transforms.html
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::math::Matrix2D3x3 final
{
private:
	array<float, 9> data;

public:
	/**
	 * Public constructor
	 */
	inline Matrix2D3x3() {
		data.fill(0.0f);
	}

	/**
	 * Public constructor
	 * @param m matrix as float values
	 */
	inline Matrix2D3x3(const array<float, 9>& m) {
		data = m;
	}

	/**
	 * Public constructor
	 * @param matrix matrix
	 */
	inline Matrix2D3x3(const Matrix2D3x3& matrix) {
		data = matrix.data;
	}

	/**
	 * Public constructor
	 * @param r0c0 r0c0
	 * @param r1c0 r1c0
	 * @param r2c0 r2c0
	 * @param r0c1 r0c1
	 * @param r1c1 r1c1
	 * @param r2c1 r2c1
	 * @param r0c2 r0c2
	 * @param r1c2 r1c2
	 * @param r2c2 r2c2
	 */
	inline Matrix2D3x3(float r0c0, float r1c0, float r2c0, float r0c1, float r1c1, float r2c1, float r0c2, float r1c2, float r2c2) {
		set(r0c0, r1c0, r2c0, r0c1, r1c1, r2c1, r0c2, r1c2, r2c2);
	}

	/**
	 * Set up matrix by values
	 * @param r0c0 row 0, column 0
	 * @param r1c0 row 1, column 0
	 * @param r2c0 row 2, column 0
	 * @param r0c1 row 0, column 1
	 * @param r1c1 row 1, column 1
	 * @param r2c1 row 2, column 1
	 * @param r0c2 row 0, column 2
	 * @param r1c2 row 1, column 2
	 * @param r2c2 row 2, column 2
	 * @return this matrix
	 */
	inline Matrix2D3x3& set(float r0c0, float r1c0, float r2c0, float r0c1, float r1c1, float r2c1, float r0c2, float r1c2, float r2c2) {
		data[0] = r0c0;
		data[1] = r1c0;
		data[2] = r2c0;
		data[3] = r0c1;
		data[4] = r1c1;
		data[5] = r2c1;
		data[6] = r0c2;
		data[7] = r1c2;
		data[8] = r2c2;
		return *this;
	}

	/**
	 * Sets up this matrix by matrix m
	 * @param m m
	 * @return this matrix
	 */
	inline Matrix2D3x3& set(const array<float, 9>& m) {
		data = m;
		return *this;
	}

	/**
	 * Sets up this matrix by matrix m
	 * @param m m
	 * @return
	 */
	inline Matrix2D3x3& set(const Matrix2D3x3& m) {
		data = m.data;
		return *this;
	}

	/**
	 * Setup identity matrix
	 * @return this matrix
	 */
	inline Matrix2D3x3& identity() {
		data[0] = 1.0f;
		data[1] = 0.0f;
		data[2] = 0.0f;
		data[3] = 0.0f;
		data[4] = 1.0f;
		data[5] = 0.0f;
		data[6] = 0.0f;
		data[7] = 0.0f;
		data[8] = 1.0f;
		return *this;
	}

	/**
	 * Scales this matrix
	 * @param s s
	 * @returns this matrix
	 */
	inline Matrix2D3x3& scale(float s) {
		data[0] *= s;
		data[1] *= s;
		data[2] *= s;
		data[3] *= s;
		data[4] *= s;
		data[5] *= s;
		return *this;
	}

	/**
	 * Scales this matrix by given vector
	 * @param v v
	 * @return this matrix
	 */
	inline Matrix2D3x3& scale(const Vector2& v) {
		data[0] *= v.data[0];
		data[1] *= v.data[0];
		data[2] *= v.data[0];
		data[3] *= v.data[1];
		data[4] *= v.data[1];
		data[5] *= v.data[1];
		return *this;
	}

	/**
	 * Sets up a translation matrix
	 * @param v v
	 * @return this matrix
	 */
	inline Matrix2D3x3& translate(const Vector2& v) {
		data[6] = v.data[0];
		data[7] = v.data[1];
		return *this;
	}

	/**
	 * Creates a rotation matrix
	 * @param angle angle
	 * @return this matrix
	 */
	inline Matrix2D3x3& rotate(float angle) {
		auto r = angle * 3.1415927f / 180.0f;
		float c = Math::cos(r);
		float s = Math::sin(r);
		data[0] = c;
		data[1] = s;
		data[2] = 0.0f;
		data[3] = -s;
		data[4] = c;
		data[5] = 0.0f;
		data[6] = 0.0f;
		data[7] = 0.0f;
		data[8] = 1.0f;
		return *this;
	}

	/**
	 * Creates a rotation matrix that rotates around texture center by given angle
	 * @param angle angle
	 * @return rotation matrix
	 */
	static inline Matrix2D3x3 rotateAroundTextureCenter(float angle) {
		return rotateAroundPoint(Vector2(0.5f, 0.5f), angle);
	}

	/**
	 * Creates a rotation matrix that rotates around given point by given angle
	 * @param point point
	 * @param angle angle
	 * @return rotation matrix
	 */
	static inline Matrix2D3x3 rotateAroundPoint(const Vector2& point, float angle) {
		Matrix2D3x3 matrix;
		matrix.identity();
		matrix.translate(point);
		matrix.multiply((Matrix2D3x3()).identity().rotate(angle));
		matrix.multiply((Matrix2D3x3()).identity().translate(point.clone().scale(-1.0f)));
		return matrix;
	}

	/**
	 * Multiplies this matrix with another matrix
	 * @param m m
	 * @return this matrix
	 */
	inline Matrix2D3x3& multiply(const Matrix2D3x3& m) {
		array<float, 9> _data;
		_data[0] = data[0] * m.data[0]  + data[3] * m.data[1]  + data[6] * m.data[2];
		_data[1] = data[1] * m.data[0]  + data[4] * m.data[1]  + data[7] * m.data[2];
		_data[2] = data[2] * m.data[0]  + data[5] * m.data[1]  + data[8] * m.data[2];
		_data[3] = data[0] * m.data[3]  + data[3] * m.data[4]  + data[6] * m.data[5];
		_data[4] = data[1] * m.data[3]  + data[4] * m.data[4]  + data[7] * m.data[5];
		_data[5] = data[2] * m.data[3]  + data[5] * m.data[4]  + data[8] * m.data[5];
		_data[6] = data[0] * m.data[6]  + data[3] * m.data[7]  + data[6] * m.data[8];
		_data[7] = data[1] * m.data[6]  + data[4] * m.data[7]  + data[7] * m.data[8];
		_data[8] = data[2] * m.data[6]  + data[5] * m.data[7]  + data[8] * m.data[8];
		data = _data;
		return *this;
	}

	/**
	 * Multiplies a vector with this matrix into destination vector
	 * @param v vector 2
	 * @return resulting vector 2
	 */
	inline Vector2 multiply(const Vector2& v) const {
		return Vector2(
			data[0] * v.data[0] + data[3] * v.data[1] + data[6],
			data[1] * v.data[0] + data[4] * v.data[1] + data[7]
		);
	}

	/**
	 * Returns if this matrix equals m
	 * @param m m
	 * @return equals
	 */
	inline bool equals(const Matrix2D3x3& m) const {
		return
			(this == &m) ||
			(
				Math::abs(data[0] - m.data[0]) < Math::EPSILON &&
				Math::abs(data[1] - m.data[1]) < Math::EPSILON &&
				Math::abs(data[2] - m.data[2]) < Math::EPSILON &&
				Math::abs(data[3] - m.data[3]) < Math::EPSILON &&
				Math::abs(data[4] - m.data[4]) < Math::EPSILON &&
				Math::abs(data[5] - m.data[5]) < Math::EPSILON &&
				Math::abs(data[6] - m.data[6]) < Math::EPSILON &&
				Math::abs(data[7] - m.data[7]) < Math::EPSILON &&
				Math::abs(data[8] - m.data[8]) < Math::EPSILON
			);
	}

	/**
	 * Operator * (float)
	 * @param f value to multiply by
	 * @return new matrix (this * f)
	 */
	inline Matrix2D3x3 operator *(const float f) const {
		auto r = this->clone().scale(f);
		return r;
	}

	/**
	 * Operator * (Matrix2D3x3&)
	 * @param m matrix to multiply by
	 * @return new matrix (this * m)
	 */
	inline Matrix2D3x3 operator *(const Matrix2D3x3& m) const {
		auto r = this->clone().multiply(m);
		return r;
	}

	/*
	 * Operator * (Vector2&)
	 * @param v vector to multiply by
	 * @return new vector (this * v)
	 */
	inline Vector2 operator *(const Vector2& v) const {
		return this->multiply(v);
	}

	/**
	 * Operator *=
	 * @param m matrix to multiply by
	 * @return this matrix multiplied by m
	 */
	inline Matrix2D3x3& operator *=(const Matrix2D3x3& m) {
		return this->multiply(m);
	}

	/**
	 * Equality comparison operator
	 * @param m matrix to compare to
	 * @return equality
	 */

	inline bool operator ==(const Matrix2D3x3& m) const {
		return this->equals(m);
	}

	/**
	 * Non equality comparison operator
	 * @param m matrix to compare to
	 * @return non equality
	 */

	inline bool operator !=(const Matrix2D3x3& m) const {
		return this->equals(m) == false;
	}

	/**
	 * Returns array data
	 * @return array data
	 */
	inline array<float, 9>& getArray() const {
		return (array<float, 9>&)data;
	}

	/**
	 * Interpolates between matrix 1 and matrix 2 by 0f<=t<=1f linearly
	 * @param m1 matrix 1
	 * @param m2 matrix 2
	 * @param t t
	 * @return interpolated matrix
	 */
	inline static Matrix2D3x3 interpolateLinear(const Matrix2D3x3& m1, const Matrix2D3x3& m2, float t) {
		return Matrix2D3x3(
			(m2.data[0] * t) + ((1.0f - t) * m1.data[0]),
			(m2.data[1] * t) + ((1.0f - t) * m1.data[1]),
			(m2.data[2] * t) + ((1.0f - t) * m1.data[2]),
			(m2.data[3] * t) + ((1.0f - t) * m1.data[3]),
			(m2.data[4] * t) + ((1.0f - t) * m1.data[4]),
			(m2.data[5] * t) + ((1.0f - t) * m1.data[5]),
			(m2.data[6] * t) + ((1.0f - t) * m1.data[6]),
			(m2.data[7] * t) + ((1.0f - t) * m1.data[7]),
			(m2.data[8] * t) + ((1.0f - t) * m1.data[8])
		);
	}

	/**
	 * Clones this matrix
	 * @return new cloned matrix
	 */
	inline Matrix2D3x3 clone() const {
		return Matrix2D3x3(data);
	}

};
