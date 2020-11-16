#pragma once

#include <array>

#include <tdme/tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Math.h>

using std::array;

using tdme::math::Math;

/**
 * 2D vector 2 class
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::math::Vector2 final
{
	friend class Matrix2D3x3;

private:
	array<float, 2> data {  };

public:
	/**
	 * Public constructor
	 */
	inline Vector2() {
		data.fill(0.0f);
	}

	/**
	 * Public constructor
	 * @param x x
	 * @param y y
	 */
	inline Vector2(float x, float y) {
		data[0] = x;
		data[1] = y;
	}

	/**
	 * Public constructor
	 * @param v float values
	 */
	inline Vector2(const array<float, 2>& v) {
		data = v;
	}

	/**
	 * Public constructor
	 * @param v v
	 */
	inline Vector2(const Vector2& v) {
		data = v.data;
	}

	/**
	 * Set up vector
	 * @param x x
	 * @param y y
	 * @return this vector
	 */
	inline Vector2& set(float x, float y) {
		data[0] = x;
		data[1] = y;
		return *this;
	}

	/**
	 * Set up vector
	 * @param v float array containing x,y values
	 * @return this vector
	 */
	inline Vector2& set(const array<float, 2>& v) {
		data = v;
		return *this;
	}

	/**
	 * Set up vector
	 * @param v v
	 * @return this vector
	 */
	inline Vector2& set(const Vector2& v) {
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
	 * set X
	 * @param x x
	 * @return this vector
	 */
	inline Vector2& setX(float x) {
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
	 */
	inline Vector2& setY(float y) {
		data[1] = y;
		return *this;
	}

	/**
	 * Adds a vector
	 * @param v v
	 * @return this vector
	 */
	inline Vector2& add(const Vector2& v) {
		data[0] += v.data[0];
		data[1] += v.data[1];
		return *this;
	}

	/**
	 * Subtracts a vector
	 * @param v v
	 * @return this vector
	 */
	inline Vector2& sub(const Vector2& v) {
		data[0] -= v.data[0];
		data[1] -= v.data[1];
		return *this;
	}

	/**
	 * Scale this vector
	 * @param scale scale
	 * @return this vector
	 */
	inline Vector2& scale(const float scale) {
		data[0] *= scale;
		data[1] *= scale;
		return *this;
	}

	/**
	 * Scale this vector
	 * @param scale scale
	 * @return this vector
	 */
	inline Vector2& scale(const Vector2& scale) {
		data[0] *= scale.data[0];
		data[1] *= scale.data[1];
		return *this;
	}

	/**
	 * @return the vectors length
	 */
	inline float computeLength() const {
		return Math::sqrt((data[0] * data[0]) + (data[1] * data[1]));
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
	inline Vector2 operator +(const Vector2& v) const {
		auto r = this->clone().add(v);
		return r;
	}

	/**
	 * Operator -
	 * @param v vector to subtract
	 * @return new vector (this - v)
	 */
	inline Vector2 operator -(const Vector2& v) const {
		auto r = this->clone().sub(v);
		return r;
	}

	/**
	 * Operator * (float)
	 * @param f value to multiply by
	 * @return new vector (this * f)
	 */
	inline Vector2 operator *(const float f) const {
		auto r = this->clone().scale(f);
		return r;
	}

	/**
	 * Operator * (Vector2&)
	 * @param v vector to multiply by
	 * @return new vector (this * v)
	 */
	inline Vector2 operator *(const Vector2& v) const {
		auto r = this->clone().scale(v);
		return r;
	}

	/**
	 * Operator / (f)
	 * @param v value to divide by
	 * @return new vector (this / f)
	 */
	inline Vector2 operator /(const float f) const {
		auto r = this->clone().scale(1.0f / f);
		return r;
	}

	/**
	 * Operator / (Vector2&)
	 * @param v vector to divide by
	 * @return new vector (this / v)
	 */
	inline Vector2 operator /(const Vector2& v) const {
		auto vInverted = Vector2(1.0f / v[0], 1.0f / v[1]);
		auto r = this->clone().scale(vInverted);
		return r;
	}

	/**
	 * Operator +=
	 * @param v vector to add
	 * @return this vector added by v
	 */
	inline Vector2& operator +=(const Vector2& v) {
		return this->add(v);
	}

	/**
	 * Operator -=
	 * @param v vector to substract
	 * @return this vector substracted by v
	 */
	inline Vector2& operator -=(Vector2& v) {
		return this->sub(v);
	}

	/**
	 * Operator *=
	 * @param v vector to multiply by
	 * @return this vector multiplied by v
	 */
	inline Vector2& operator *=(Vector2& v) {
		return this->scale(v);
	}

	/**
	 * Operator /=
	 * @param v vector to devide by
	 * @return this vector devided by v
	 */
	inline Vector2& operator /=(Vector2& v) {
		auto vInverted = Vector2(1.0f / v[0], 1.0f / v[1]);
		return this->scale(vInverted);
	}

	/**
	 * Operator *=
	 * @param f float to multiply by
	 * @return this vector multiplied by f
	 */
	inline Vector2& operator *=(const float f) {
		return this->scale(f);
	}

	/**
	 * Operator /=
	 * @param f float to divide by
	 * @return this vector divided by f
	 */
	inline Vector2& operator /=(const float f) {
		return this->scale(1.0f / f);
	}

	/**
	 * Equality comparison operator
	 * @param v vector to compare to
	 * @return equality
	 */
	inline bool operator ==(const Vector2& v) const {
		return this->equals(v);
	}

	/**
	 * Non equality comparison operator
	 * @param v vector to compare to
	 * @return non equality
	 */
	inline bool operator !=(const Vector2& v) const {
		return this->equals(v) == false;
	}

	/**
	 * @return vector as array
	 */
	inline array<float, 2>& getArray() const {
		return (array<float, 2>&)data;
	}

	/**
	 * Clones the vector
	 * @return new cloned vector
	 */
	inline Vector2 clone() const {
		return Vector2(data);
	}

	/**
	 * Compares this vector with given vector
	 * @param v vector v
	 * @return equality
	 */
	inline bool equals(const Vector2& v) const {
		return equals(v, Math::EPSILON);
	}

	/**
	 * Compares this vector with given vector
	 * @param v vector v
	 * @param tolerance tolerance per component(x, y, z)
	 * @return equality
	 */
	inline bool equals(const Vector2& v, float tolerance) const {
		return (this == &v) ||
			(
				Math::abs(data[0] - v.data[0]) < tolerance &&
				Math::abs(data[1] - v.data[1]) < tolerance
			);
	}

};
