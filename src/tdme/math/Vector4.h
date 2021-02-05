#pragma once

#include <array>

#include <tdme/tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Vector3.h>

using std::array;

using tdme::math::Vector3;

/**
 * 3D vector 4 class
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::math::Vector4 final
{
	friend class Matrix4x4;
	friend class Vector3;

private:
	array<float, 4> data;

public:
	/**
	 * Public constructor
	 */
	inline Vector4() {
		data.fill(0.0f);
	}

	/**
	 * Public constructor
	 * @param v vector
	 */
	inline Vector4(const Vector4& v) {
		data = v.data;
	}

	/**
	 * Public constructor
	 * @param v vector
	 * @param w w
	 */
	inline Vector4(const Vector3& v, float w) {
		data[0] = v.data[0];
		data[1] = v.data[1];
		data[2] = v.data[2];
		data[3] = w;
	}

	/**
	 * Public constructor
	 * @param x x
	 * @param y y
	 * @param z z
	 * @param w w
	 */
	inline Vector4(float x, float y, float z, float w) {
		data[0] = x;
		data[1] = y;
		data[2] = z;
		data[3] = w;
	}

	/**
	 * Public constructor
	 * @param v v
	 */
	inline Vector4(const array<float, 4>& v) {
		data = v;
	}

	/**
	 * Set up vector
	 * @param x x
	 * @param y y
	 * @param z z
	 * @param w w
	 * @return this vector
	 */
	inline Vector4& set(float x, float y, float z, float w) {
		data[0] = x;
		data[1] = y;
		data[2] = z;
		data[3] = w;
		return *this;
	}

	/**
	 * Set up vector
	 * @param v float array containing x,y,z,w values
	 * @return this vector
	 */
	inline Vector4& set(const array<float, 4>& v) {
		data = v;
		return *this;
	}

	/**
	 * Set up vector
	 * @param v v
	 * @return this vector
	 */
	inline Vector4& set(const Vector4& v) {
		data[1] = v.data[1];
		data[0] = v.data[0];
		data[2] = v.data[2];
		data[3] = v.data[3];
		return *this;
	}

	/**
	 * Set up vector
	 * @param v vector 3
	 * @param w w
	 * @return this vector
	 */
	inline Vector4& set(const Vector3& v, float w) {
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
	 * set X
	 * @param x x
	 * @return this vector
	 */
	inline Vector4& setX(float x) {
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
	 * set Y
	 * @param y y
	 * @return this vector
	 */
	inline Vector4& setY(float y) {
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
	inline Vector4& setZ(float z) {
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
	 * @return this vector
	 */
	inline Vector4& setW(float w) {
		data[3] = w;
		return *this;
	}

	/**
	 * Adds a vector
	 * @param v v
	 * @return this vector
	 */
	inline Vector4& add(const Vector4& v) {
		data[0] += v.data[0];
		data[1] += v.data[1];
		data[2] += v.data[2];
		data[3] += v.data[3];
		return *this;
	}

	/**
	 * Subtracts a vector
	 * @param v v
	 * @return this vector
	 */
	inline Vector4& sub(const Vector4& v) {
		data[0] -= v.data[0];
		data[1] -= v.data[1];
		data[2] -= v.data[2];
		data[3] -= v.data[3];
		return *this;
	}


	/**
	 * Scale this vector
	 * @param scale scale
	 * @return this vector
	 */
	inline Vector4& scale(float scale) {
		data[0] *= scale;
		data[1] *= scale;
		data[2] *= scale;
		data[3] *= scale;
		return *this;
	}

	/**
	 * Scale this vector
	 * @param scale scale
	 * @return this vector
	 */
	inline Vector4& scale(const Vector4& scale) {
		data[0] *= scale.data[0];
		data[1] *= scale.data[1];
		data[2] *= scale.data[2];
		data[3] *= scale.data[3];
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
	 * Operator +
	 * @param v vector to add
	 * @return new vector (this + v)
	 */
	inline Vector4 operator +(const Vector4& v) const {
		auto r = this->clone().add(v);
		return r;
	}

	/**
	 * Operator -
	 * @param v vector to subtract
	 * @return new vector (this - v)
	 */
	inline Vector4 operator -(const Vector4& v) const {
		auto r = this->clone().sub(v);
		return r;
	}

	/**
	 * Operator * (float)
	 * @param f value to multiply by
	 * @return new vector (this * f)
	 */
	inline Vector4 operator *(const float f) const {
		auto r = this->clone().scale(f);
		return r;
	}

	/**
	 * Operator * (Vector4&)
	 * @param v vector to multiply by
	 * @return new vector (this * v)
	 */
	inline Vector4 operator *(const Vector4& v) const {
		auto r = this->clone().scale(v);
		return r;
	}

	/**
	 * Operator / (f)
	 * @param v value to divide by
	 * @return new vector (this / f)
	 */
	inline Vector4 operator /(const float f) const {
		auto r = this->clone().scale(1.0f / f);
		return r;
	}

	/**
	 * Operator / (Vector4&)
	 * @param v vector to divide by
	 * @return new vector (this / v)
	 */
	inline Vector4 operator /(const Vector4& v) const {
		auto vInverted = Vector4(1.0f / v[0], 1.0f / v[1], 1.0f / v[2], 1.0f / v[3]);
		auto r = this->clone().scale(vInverted);
		return r;
	}

	/**
	 * Operator +=
	 * @param v vector to add
	 * @return this vector added by v
	 */
	inline Vector4& operator +=(const Vector4& v) {
		return this->add(v);
	}

	/**
	 * Operator -=
	 * @param v vector to substract
	 * @return this vector substracted by v
	 */
	inline Vector4& operator -=(Vector4& v) {
		return this->sub(v);
	}

	/**
	 * Operator *=
	 * @param v vector to multiply by
	 * @return this vector multiplied by v
	 */
	inline Vector4& operator *=(Vector4& v) {
		return this->scale(v);
	}

	/**
	 * Operator /=
	 * @param v vector to devide by
	 * @return this vector devided by v
	 */
	inline Vector4& operator /=(Vector4& v) {
		auto vInverted = Vector4(1.0f / v[0], 1.0f / v[1], 1.0f / v[2], 1.0f / v[3]);
		return this->scale(vInverted);
	}

	/**
	 * Operator *=
	 * @param f float to multiply by
	 * @return this vector multiplied by f
	 */
	inline Vector4& operator *=(const float f) {
		return this->scale(f);
	}

	/**
	 * Operator /=
	 * @param f float to divide by
	 * @return this vector divided by f
	 */
	inline Vector4& operator /=(const float f) {
		return this->scale(1.0f / f);
	}

	/**
	 * Equality comparison operator
	 * @param v vector to compare to
	 * @return equality
	 */

	inline bool operator ==(const Vector4& v) const {
		return this->equals(v);
	}

	/**
	 * Non equality comparison operator
	 * @param v vector to compare to
	 * @return non equality
	 */

	inline bool operator !=(const Vector4& v) const {
		return this->equals(v) == false;
	}

	/**
	 * @return vector as array
	 */
	inline array<float, 4>& getArray() const {
		return (array<float, 4>&)data;
	}

	/**
	 * Clones the vector
	 * @return new cloned vector
	 */
	inline Vector4 clone() const {
		return Vector4(*this);
	}

	/**
	 * Compares this vector with given vector
	 * @param v vector v
	 * @return equality
	 */
	inline bool equals(const Vector4& v) const {
		return equals(v, Math::EPSILON);
	}

	/**
	 * Compares this vector with given vector
	 * @param v vector v
	 * @param tolerance tolerance per component(x, y, z)
	 * @return equality
	 */
	inline bool equals(const Vector4& v, float tolerance) const {
		return (this == &v) ||
			(
				Math::abs(data[0] - v.data[0]) < tolerance &&
				Math::abs(data[1] - v.data[1]) < tolerance &&
				Math::abs(data[2] - v.data[2]) < tolerance &&
				Math::abs(data[3] - v.data[3]) < tolerance
			);
	}


};
