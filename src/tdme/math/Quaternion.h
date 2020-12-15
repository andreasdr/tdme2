#pragma once

#include <array>

#include <tdme/tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Math.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Matrix4x4.h>

using std::array;

using tdme::math::Math;
using tdme::math::Vector3;
using tdme::math::Matrix4x4;

/**
 * Quaternion class
 * @author Andreas Drewke
 */
class tdme::math::Quaternion final
{
// see http://db-in.com/blog/2011/04/cameras-on-opengl-es-2-x/
private:
	array<float, 4> data {  };

public:
	/**
	 * Public constructor
	 * @param q quaternion
	 */
	inline Quaternion(const Quaternion& q) {
		data = q.data;
	}

	/**
	 * Public constructor
	 * @param x x
	 * @param y y
	 * @param z z
	 * @param w w
	 */
	inline Quaternion(float x, float y, float z, float w) {
		data[0] = x;
		data[1] = y;
		data[2] = z;
		data[3] = w;
	}

	/**
	 * Public constructor
	 * @param v vector
	 * @param w w
	 */
	inline Quaternion(const Vector3& v, float w) {
		data[0] = v.data[0];
		data[1] = v.data[1];
		data[2] = v.data[2];
		data[3] = w;
	}
	/**
	 * Set up this quaternion by components
	 * @param x x
	 * @param y y
	 * @param z z
	 * @param w w
	 */
	inline Quaternion& set(float x, float y, float z, float w) {
		data[0] = x;
		data[1] = y;
		data[2] = z;
		data[3] = w;
		return *this;
	}

	/**
	 * Sets up this quaternion by quaternion q
	 * @param q q
	 * @return
	 */
	inline Quaternion& set(const Quaternion& q) {
		data = q.data;
		return *this;
	}

	/**
	 * Set quaternion
	 * @param v vector
	 * @param w w
	 */
	inline Quaternion& set(const Vector3& v, float w) {
		data[0] = v.data[0];
		data[1] = v.data[1];
		data[2] = v.data[2];
		data[3] = w;
		return *this;
	}

	/**
	 * @return x
	 */
	inline float getX() const {
		return data[0];
	}

	/**
	 * Set X
	 * @param x x
	 * @return this quaternion
	 */
	inline Quaternion& setX(float x) {
		data[0] = x;
		return *this;
	}

	/**
	 * @return y
	 */
	inline float getY() const {
		return data[1];
	}

	/**
	 * Set Y
	 * @param y y
	 * @return this quaternion
	 */
	inline Quaternion& setY(float y) {
		data[1] = y;
		return *this;
	}

	/**
	 * @return z
	 */
	inline float getZ() const {
		return data[2];
	}

	/**
	 * Set Z
	 * @param z z
	 * @return this quaternion
	 */
	inline Quaternion& setZ(float z) {
		data[2] = z;
		return *this;
	}

	/**
	 * @return w
	 */
	inline float getW() const {
		return data[3];
	}

	/**
	 * Set W
	 * @param w w
	 * @return this quaternion
	 */
	inline Quaternion& setW(float w) {
		data[3] = w;
		return *this;
	}

	/**
	 * Set up quaternion identity
	 * @return this quaternion
	 */
	inline Quaternion& identity() {
		data[0] = 0.0f;
		data[1] = 0.0f;
		data[2] = 0.0f;
		data[3] = 1.0f;
		return *this;
	}

	/**
	 * Inverts this quaternion
	 * @return this quaternion
	 */
	inline Quaternion& invert() {
		data[0] *= -1.0f;
		data[1] *= -1.0f;
		data[2] *= -1.0f;
		return *this;
	}

	/**
	 * Creates a rotation quaternion
	 * @param axis axis
	 * @param angle angle
	 * @return this quaternion
	 */
	inline Quaternion& rotate(const Vector3& axis, float angle) {
		// converts the angle in degrees to radians
		auto radians = angle * 3.1415927f / 180.0f;
		// finds the sin and cosin for the half angle
		auto sin = Math::sin(radians * 0.5);
		auto cos = Math::cos(radians * 0.5);
		// formula to construct a new quaternion based on direction and angle
		data[0] = axis.data[0] * sin;
		data[1] = axis.data[1] * sin;
		data[2] = axis.data[2] * sin;
		data[3] = cos;
		return *this;
	}

	/**
	 * Normalize quaternion
	 */
	inline Quaternion& normalize() {
		auto magnitude = Math::sqrt(data[0] * data[0] + data[1] * data[1] + data[2] * data[2] + data[3] * data[3]);
		data[0] = data[0] / magnitude;
		data[1] = data[1] / magnitude;
		data[2] = data[2] / magnitude;
		data[3] = data[3] / magnitude;
		return *this;
	}

	/**
	 * Multiplies this quaternion with quaternion q
	 * @param q quaterion q
	 * @return this quaternion
	 */
	inline Quaternion& multiply(const Quaternion q) {
		array<float, 4> _data;
		_data[0] = data[3] * q.data[0] + data[0] * q.data[3] + data[1] * q.data[2] - data[2] * q.data[1];
		_data[1] = data[3] * q.data[1] - data[0] * q.data[2] + data[1] * q.data[3] + data[2] * q.data[0];
		_data[2] = data[3] * q.data[2] + data[0] * q.data[1] - data[1] * q.data[0] + data[2] * q.data[3];
		_data[3] = data[3] * q.data[3] - data[0] * q.data[0] - data[1] * q.data[1] - data[2] * q.data[2];
		data = _data;
		return *this;
	}

	/**
	 * Adds given quaternion q to this quaternion
	 * @param q quaterion q
	 * @return this quaternion
	 */
	inline Quaternion& add(const Quaternion& q) {
		data[0] += q.data[0];
		data[1] += q.data[1];
		data[2] += q.data[2];
		data[3] += q.data[3];
		return *this;
	}

	/**
	 * Subtracts given quaternion q from this quaternion
	 * @param q quaterion q
	 * @return this quaternion
	 */
	inline Quaternion& sub(const Quaternion& q) {
		data[0] -= q.data[0];
		data[1] -= q.data[1];
		data[2] -= q.data[2];
		data[3] -= q.data[3];
		return *this;
	}

	/**
	 * Scales this quaternion with given value
	 * @param value value
	 * @return this quaternion
	 */
	inline Quaternion& scale(float value) {
		data[0] *= value;
		data[1] *= value;
		data[2] *= value;
		data[3] *= value;
		return *this;
	}

	/**
	 * Multiplies a quaternion with given vector v
	 * @param v vector v
	 * @param dest destination vector
	 * @return dest
	 */
	inline Vector3& multiply(const Vector3& v, Vector3& dest) const {
		Vector3 q;
		Vector3 t;
		Vector3 qxt;
		// t = 2 * cross(q.xyz, v)
		q.set(data[0], data[1], data[2]);
		Vector3::computeCrossProduct(q, v, t).scale(2.0f);
		// v' = v + q.w * t + cross(q.xyz, t)
		Vector3::computeCrossProduct(q, t, qxt);
		dest.set(v);
		dest.add(qxt);
		dest.add(t.scale(data[3]));
		return dest;
	}

	/**
	 * Computes a matrix from given
	 * @param matrix destination matrix
	 * @return destination matrix
	 */
	inline Matrix4x4& computeMatrix(Matrix4x4& matrix) const {
		matrix.set(
			1.0f - 2.0f * (data[1] * data[1] + data[2] * data[2]),
			2.0f * (data[0] * data[1] + data[2] * data[3]),
			2.0f * (data[0] * data[2] - data[1] * data[3]),
			0.0f,
			2.0f * (data[0] * data[1] - data[2] * data[3]),
			1.0f - 2.0f * (data[0] * data[0] + data[2] * data[2]),
			2.0f * (data[2] * data[1] + data[0] * data[3]),
			0.0f,
			2.0f * (data[0] * data[2] + data[1] * data[3]),
			2.0f * (data[1] * data[2] - data[0] * data[3]),
			1.0f - 2.0f * (data[0] * data[0] + data[1] * data[1]),
			0.0f,
			0.0f,
			0.0f,
			0.0f,
			1.0f
		);
		return matrix;
	}

	/**
	 * Returns array data
	 * @return array data
	 */
	inline array<float, 4>& getArray() const {
		return (array<float, 4>&)data;
	}

	/**
	 * Array access operator
	 * @param i index
	 * @return quaternion component
	 */
	inline float& operator[](int i) {
		return data[i];
	}

	/**
	 * Const array access operator
	 * @param i index
	 * @return quaternion component
	 */
	inline const float& operator[](int i) const {
		return data[i];
	}

	/**
	 * Operator +
	 * @param q quaternion to add
	 * @return new quaternion (this + q)
	 */
	inline Quaternion operator +(const Quaternion& q) const {
		auto r = this->clone().add(q);
		return r;
	}

	/**
	 * Operator -
	 * @param q quaternion to subtrct
	 * @return new quaternion (this - q)
	 */
	inline Quaternion operator -(const Quaternion& q) const {
		auto r = this->clone().sub(q);
		return r;
	}

	/**
	 * Operator * (float)
	 * @param f value to multiply by
	 * @return new quaternion (this * f)
	 */
	inline Quaternion operator *(const float f) const {
		auto r = this->clone().scale(f);
		return r;
	}

	/**
	 * Operator * (Quaternion&)
	 * @param q quaternion to multiply by
	 * @return new quaternion (this * q)
	 */
	inline Quaternion operator *(const Quaternion& q) const {
		auto r = this->clone().multiply(q);
		return r;
	}

	/**
	 * Operattor * (Vector&)
	 * @param v vector to multiply by
	 * @return new Vector ()thic * v
	 */
	inline Vector3 operator *(const Vector3& v) const {
		auto r = Vector3();
		return this->multiply(v, r);
	}

	/**
	 * Operator / (f)
	 * @param q value to divide by
	 * @return new quaternion (this / f)
	 */
	inline Quaternion operator /(const float f) const {
		auto r = this->clone().scale(1.0f / f);
		return r;
	}

	/**
	 * Operator / (Quaternion&)
	 * @param q quaternion to divide by
	 * @return new quaternion (this / q)
	 */
	inline Quaternion operator /(const Quaternion& q) const {
		auto qInverted = Quaternion(1.0f / q[0], 1.0f / q[1], 1.0f / q[2], 1.0f / q[3]);
		auto r = this->clone().multiply(qInverted);
		return r;
	}

	/**
	 * Operator +=
	 * @param q quaternion to add
	 * @return this quaternion added by q
	 */
	inline Quaternion& operator +=(const Quaternion& q) {
		return this->add(q);
	}

	/**
	 * Operator -=
	 * @param q quaternion to substract
	 * @return this quaternion substracted by q
	 */
	inline Quaternion& operator -=(Quaternion& q) {
		return this->sub(q);
	}

	/**
	 * Operator *=
	 * @param q quaternion to multiply by
	 * @return this quaternion multiplied by q
	 */
	inline Quaternion& operator *=(Quaternion& q) {
		return this->multiply(q);
	}

	/**
	 * Operator /=
	 * @param q quaternion to devide by
	 * @return this quaternion devided by q
	 */
	inline Quaternion& operator /=(Quaternion& q) {
		auto qInverted = Quaternion(1.0f / q[0], 1.0f / q[1], 1.0f / q[2], 1.0f / q[3]);
		return this->multiply(qInverted);
	}

	/**
	 * Equality comparison operator
	 * @param q quaternion to compare to
	 * @return equality
	 */

	inline bool operator ==(const Quaternion& q) const {
		return this->equals(q);
	}

	/**
	 * Non equality comparison operator
	 * @param q quaternion to compare to
	 * @return non equality
	 */

	inline bool operator !=(const Quaternion& q) const {
		return this->equals(q) == false;
	}

	/**
	 * Clones the quaternion
	 * @return new cloned vector
	 */
	inline Quaternion clone() const {
		return Quaternion(*this);
	}

	/**
	 * Compares this quaternion with given quaternion
	 * @param q quaternion q
	 * @return equality
	 */
	inline bool equals(const Quaternion& q) const {
		return equals(q, Math::EPSILON);
	}

	/**
	 * Compares this quaternion with given quaternion
	 * @param q quaternion q
	 * @param tolerance tolerance per component(x, y, z)
	 * @return equality
	 */
	inline bool equals(const Quaternion& q, float tolerance) const {
		return (this == &q) ||
			(
				Math::abs(data[0] - q.data[0]) < tolerance &&
				Math::abs(data[1] - q.data[1]) < tolerance &&
				Math::abs(data[2] - q.data[2]) < tolerance &&
				Math::abs(data[3] - q.data[3]) < tolerance
			);
	}

	/**
	 * Public constructor
	 */
	inline Quaternion() {
		data.fill(0.0f);
	}

};
