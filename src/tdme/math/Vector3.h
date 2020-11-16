#pragma once

#include <array>

#include <tdme/tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Math.h>
#include <tdme/utilities/Float.h>

using std::array;

using tdme::math::Math;
using tdme::math::Vector3;
using tdme::utilities::Float;

/**
 * 3D vector 3 class
 * @author andreas.drewke
 * @version $Id$
 */
class tdme::math::Vector3 final
{
	friend class Matrix4x4;
	friend class Quaternion;
	friend class Vector4;

private:
	array<float, 3> data {  };

public:
	/**
	 * Public constructor
	 */
	inline Vector3() {
		data.fill(0.0f);
	}

	/**
	 * Public constructor
	 * @param x x
	 * @param y y
	 * @param z z
	 */
	inline Vector3(float x, float y, float z) {
		data[0] = x;
		data[1] = y;
		data[2] = z;
	}

	/**
	 * Public constructor
	 * @param v values
	 */
	inline Vector3(const array<float,3>& v) {
		data = v;
	}

	/**
	 * Public constructor
	 * @param v vector
	 */
	inline Vector3(const Vector3& v) {
		data = v.data;
	}

	/**
	 * Set up vector
	 * @param x x
	 * @param y y
	 * @param z z
	 * @return this vector
	 */
	inline Vector3& set(float x, float y, float z) {
		data[0] = x;
		data[1] = y;
		data[2] = z;
		return *this;
	}

	/**
	 * Set up vector
	 * @param v float array containing x,y,z values
	 * @return this vector
	 */
	inline Vector3& set(const array<float, 3>& v) {
		data = v;
		return *this;
	}

	/**
	 * Set up vector
	 * @param v v
	 * @return this vector
	 */
	inline Vector3& set(const Vector3& v) {
		data = v.data;
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
	 */
	inline Vector3& setX(float x) {
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
	 * @return this vector
	 */
	inline Vector3& setY(float y) {
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
	 * @return this vector
	 */
	inline Vector3& setZ(float z) {
		data[2] = z;
		return *this;
	}

	/**
	 * Array access operator
	 * @param i index
	 * @return vector3 component
	 */
	inline float& operator[](int i) {
		return data[i];
	}

	/**
	 * Const array access operator
	 * @param i index
	 * @return vector3 component
	 */
	inline const float& operator[](int i) const {
		return data[i];
	}

	/**
	 * @return vector as array
	 */
	inline array<float,3>& getArray() const {
		return (array<float,3>&)data;
	}

	/**
	 * Compute the cross product of vector v1 and v2
	 * @param v1 v1
	 * @param v2 v2
	 * @param dest destination vector
	 * @return destination vector
	 */
	inline static Vector3& computeCrossProduct(const Vector3& v1, const Vector3& v2, Vector3& dest) {
		dest.set(
			(v1.data[1] * v2.data[2]) - (v1.data[2] * v2.data[1]),
			(v1.data[2] * v2.data[0]) - (v1.data[0] * v2.data[2]),
			(v1.data[0] * v2.data[1]) - (v1.data[1] * v2.data[0]));
		return dest;
	}

	/**
	 * Compute the dot product of vector v1 and v2
	 * @param v1 v1
	 * @param v2 v2
	 * @return Vector3
	 */
	inline static float computeDotProduct(const Vector3& v1, const Vector3& v2) {
		return (v1.data[0] * v2.data[0]) + (v1.data[1] * v2.data[1]) + (v1.data[2] * v2.data[2]);
	}

	/**
	 * @return the vectors length
	 */
	inline float computeLength() const {
		return Math::sqrt((data[0] * data[0]) + (data[1] * data[1]) + (data[2] * data[2]));
	}

	/**
	 * @return the vectors length squared
	 */
	inline float computeLengthSquared() const {
		return (data[0] * data[0]) + (data[1] * data[1]) + (data[2] * data[2]);
	}

	/**
	 * Compute Euler angles (rotation around x, y, z axes)
	 * @param euler euler angles
	 * @return if euler angles have been generated
	 */
	inline bool computeEulerAngles(Vector3& euler) const {
		if (computeLength() < Math::EPSILON) {
			return false;
		}
		Vector3 a(*this);
		a.normalize();
		// test around x axis
		{
			auto b = Vector3(0.0f, 1.0f, 0.0f);
			auto n = Vector3(1.0f, 0.0f, 0.0f);
			auto angle = Vector3::computeAngle(a, b, n);
			euler.setX(angle);
		}
		// test around y axis
		{
			auto b = Vector3(0.0f, 0.0f, -1.0f);
			auto n = Vector3(0.0f, 1.0f, 0.0f);
			auto angle = Vector3::computeAngle(a, b, n);
			euler.setY(angle);
		}
		// test around z axis
		{
			auto b = Vector3(0.0f, 1.0f, 0.0f);
			auto n = Vector3(0.0f, 0.0f, 1.0f);
			auto angle = Vector3::computeAngle(a, b, n);
			euler.setZ(angle);
		}
		return true;
	}

	/**
	 * Computes angle between a and b from 0..180
	 * @param a vector a, vector to test, must be normalized
	 * @param b vector b, vector to test against, must be normalized
	 * @return
	 */
	inline static float computeAngle(const Vector3& a, const Vector3& b) {
		auto result = 180.0 / Math::PI * Math::acos(Math::clamp(Vector3::computeDotProduct(a, b), -1.0f, 1.0f));
		return result;
	}

	/**
	 * Computes angle between a and b
	 * @param a vector a, vector to test, must be normalized
	 * @param b vector b, vector to test against, must be normalized
	 * @param n plane normal n where a and b live in, must be normalized
	 * @return
	 */
	inline static float computeAngle(const Vector3& a, const Vector3& b, const Vector3& n) {
		Vector3 c;
		auto angle = Vector3::computeAngle(a, b);
		auto sign = Math::sign(Vector3::computeDotProduct(n, Vector3::computeCrossProduct(a, b, c)));
		if (Float::isNaN(sign) == true) sign = 1.0f;
		return std::fmod(((angle * sign) + 360.0f), 360.0f);
	}

	/**
	 * Convert this vector to vector with positive vector components
	 * @return this vector
	 */
	inline Vector3& abs() {
		data[0] = Math::abs(data[0]);
		data[1] = Math::abs(data[1]);
		data[2] = Math::abs(data[2]);
		return *this;
	}

	/**
	 * Normalize the vector
	 * @return this vector
	 */
	inline Vector3& normalize() {
		auto length = computeLength();
		data[0] /= length;
		data[1] /= length;
		data[2] /= length;
		return *this;
	}

	/**
	 * Adds a vector
	 * @param v v
	 * @return this vector
	 */
	inline Vector3& add(const Vector3& v) {
		data[0] += v.data[0];
		data[1] += v.data[1];
		data[2] += v.data[2];
		return *this;
	}

	/**
	 * Adds a float to each vector component
	 * @param value v
	 * @return this vector
	 */
	inline Vector3& add(float value) {
		data[0] += value;
		data[1] += value;
		data[2] += value;
		return *this;
	}

	/**
	 * Subtracts a vector
	 * @param v v
	 * @return this vector
	 */
	inline Vector3& sub(const Vector3& v) {
		data[0] -= v.data[0];
		data[1] -= v.data[1];
		data[2] -= v.data[2];
		return *this;
	}

	/**
	 * Subtracts a float from each vector component
	 * @param value v
	 * @return this vector
	 */
	inline Vector3& sub(float value) {
		data[0] -= value;
		data[1] -= value;
		data[2] -= value;
		return *this;
	}

	/**
	 * Scale this vector
	 * @param scale scale
	 * @return this vector
	 */
	inline Vector3& scale(float scale) {
		data[0] *= scale;
		data[1] *= scale;
		data[2] *= scale;
		return *this;
	}

	/**
	 * Scale this vector
	 * @param scale scale
	 * @return this vector
	 */
	inline Vector3& scale(const Vector3& scale) {
		data[0] *= scale.data[0];
		data[1] *= scale.data[1];
		data[2] *= scale.data[2];
		return *this;
	}

	/**
	 * Clones the vector
	 * @return new cloned vector
	 */
	inline Vector3 clone() const {
		return Vector3(data);
	}

	/**
	 * Compares this vector with given vector
	 * @param v vector v
	 * @return equality
	 */
	inline bool equals(const Vector3& v) const {
		return equals(v, Math::EPSILON);
	}

	/**
	 * Interpolates between vector 1 and vector2 by 0f<=t<=1f linearly
	 * @param v1 vector 1
	 * @param v2 vector 2
	 * @param t t
	 * @param dest destination vector
	 * @return destination vector
	 */
	inline static Vector3& interpolateLinear(const Vector3& v1, const Vector3& v2, float t, Vector3& dest) {
		return dest.set(
			(v2.data[0] * t) + ((1.0f - t) * v1.data[0]),
			(v2.data[1] * t) + ((1.0f - t) * v1.data[1]),
			(v2.data[2] * t) + ((1.0f - t) * v1.data[2])
		);
	}

	/**
	 * Compares this vector with given vector
	 * @param v vector v
	 * @param tolerance tolerance per component(x, y, z)
	 * @return equality
	 */
	inline bool equals(const Vector3& v, float tolerance) const {
		return (this == &v) ||
			(
				Math::abs(data[0] - v.data[0]) < tolerance &&
				Math::abs(data[1] - v.data[1]) < tolerance &&
				Math::abs(data[2] - v.data[2]) < tolerance
			);
	}

	/**
	 * Operator +
	 * @param v vector to add
	 * @return new vector (this + v)
	 */
	inline Vector3 operator +(const Vector3& v) const {
		auto r = this->clone().add(v);
		return r;
	}

	/**
	 * Operator +
	 * @param v vector to subtract
	 * @return new vector (this - v)
	 */
	inline Vector3 operator -(const Vector3& v) const {
		auto r = this->clone().sub(v);
		return r;
	}

	/**
	 * Operator * (float)
	 * @param f value to multiply by
	 * @return new vector (this * f)
	 */
	inline Vector3 operator *(const float f) const {
		auto r = this->clone().scale(f);
		return r;
	}

	/**
	 * Operator * (Vector3&)
	 * @param v vector to multiply by
	 * @return new vector (this * v)
	 */
	inline Vector3 operator *(const Vector3& v) const {
		auto r = this->clone().scale(v);
		return r;
	}

	/**
	 * Operator / (f)
	 * @param v value to divide by
	 * @return new vector (this / f)
	 */
	inline Vector3 operator /(const float f) const {
		auto r = this->clone().scale(1.0f / f);
		return r;
	}

	/**
	 * Operator / (Vector3&)
	 * @param v vector to divide by
	 * @return new vector (this / v)
	 */
	inline Vector3 operator /(const Vector3& v) const {
		auto vInverted = Vector3(1.0f / v[0], 1.0f / v[1], 1.0f / v[2]);
		auto r = this->clone().scale(vInverted);
		return r;
	}

	/**
	 * Operator +=
	 * @param v vector to add
	 * @return this vector added by v
	 */
	inline Vector3& operator +=(const Vector3& v) {
		return this->add(v);
	}

	/**
	 * Operator -=
	 * @param v vector to substract
	 * @return this vector substracted by v
	 */
	inline Vector3& operator -=(const Vector3& v) {
		return this->sub(v);
	}

	/**
	 * Operator *=
	 * @param v vector to multiply by
	 * @return this vector multiplied by v
	 */
	inline Vector3& operator *=(const Vector3& v) {
		return this->scale(v);
	}

	/**
	 * Operator /=
	 * @param v vector to divide by
	 * @return this vector divided by v
	 */
	inline Vector3& operator /=(const Vector3& v) {
		auto vInverted = Vector3(1.0f / v[0], 1.0f / v[1], 1.0f / v[2]);
		return this->scale(vInverted);
	}

	/**
	 * Operator *=
	 * @param f float to multiply by
	 * @return this vector multiplied by f
	 */
	inline Vector3& operator *=(const float f) {
		return this->scale(f);
	}

	/**
	 * Operator /=
	 * @param f float to divide by
	 * @return this vector divided by f
	 */
	inline Vector3& operator /=(const float f) {
		return this->scale(1.0f / f);
	}

	/**
	 * Equality comparison operator
	 * @param v vector to compare to
	 * @return equality
	 */
	inline bool operator ==(const Vector3& v) const {
		return this->equals(v);
	}

	/**
	 * Non equality comparison operator
	 * @param v vector to compare to
	 * @return non equality
	 */
	inline bool operator !=(const Vector3& v) const {
		return this->equals(v) == false;
	}

};
