#pragma once

#include <array>

#include <tdme/tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Math.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Vector4.h>

using std::array;

using tdme::math::Math;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;
using tdme::math::Vector4;

/**
 * 4x4 3D Matrix class
 * @author Andreas Drewke, Song Ho Ahn <song.ahn@gmail.com>
 * @version $Id$
 */
class tdme::math::Matrix4x4 final
{
private:
	array<float, 16> data;

public:
	/**
	 * Public constructor
	 */
	inline Matrix4x4() {
		data.fill(0.0f);
	}

	/**
	 * Public constructor
	 * @param m matrix as float values
	 */
	inline Matrix4x4(const array<float, 16>& m) {
		data = m;
	}

	/**
	 * Public constructor
	 * @param matrix matrix
	 */
	inline Matrix4x4(const Matrix4x4& matrix) {
		data = matrix.data;
	}

	/**
	 * Public constructor
	 * @param r0c0 r0c0
	 * @param r1c0 r1c0
	 * @param r2c0 r2c0
	 * @param r3c0 r3c0
	 * @param r0c1 r0c1
	 * @param r1c1 r1c1
	 * @param r2c1 r2c1
	 * @param r3c1 r3c1
	 * @param r0c2 r0c2
	 * @param r1c2 r1c2
	 * @param r2c2 r2c2
	 * @param r3c2 r3c2
	 * @param r0c3 r0c3
	 * @param r1c3 r1c3
	 * @param r2c3 r2c3
	 * @param r3c3 r3c3
	 */
	inline Matrix4x4(float r0c0, float r1c0, float r2c0, float r3c0, float r0c1, float r1c1, float r2c1, float r3c1, float r0c2, float r1c2, float r2c2, float r3c2, float r0c3, float r1c3, float r2c3, float r3c3) {
		set(r0c0, r1c0, r2c0, r3c0, r0c1, r1c1, r2c1, r3c1, r0c2, r1c2, r2c2, r3c2, r0c3, r1c3, r2c3, r3c3);
	}

	/**
	 * Set up matrix by values
	 * @param r0c0 row 0, column 0
	 * @param r1c0 row 1, column 0
	 * @param r2c0 row 2, column 0
	 * @param r3c0 row 3, column 0
	 * @param r0c1 row 0, column 1
	 * @param r1c1 row 1, column 1
	 * @param r2c1 row 2, column 1
	 * @param r3c1 row 3, column 1
	 * @param r0c2 row 0, column 2
	 * @param r1c2 row 1, column 2
	 * @param r2c2 row 2, column 2
	 * @param r3c2 row 3, column 2
	 * @param r0c3 row 0, column 3
	 * @param r1c3 row 1, column 3
	 * @param r2c3 row 2, column 3
	 * @param r3c3 row 3, column 3
	 * @return this matrix
	 */
	inline Matrix4x4& set(float r0c0, float r1c0, float r2c0, float r3c0, float r0c1, float r1c1, float r2c1, float r3c1, float r0c2, float r1c2, float r2c2, float r3c2, float r0c3, float r1c3, float r2c3, float r3c3) {
		data[0] = r0c0;
		data[1] = r1c0;
		data[2] = r2c0;
		data[3] = r3c0;
		data[4] = r0c1;
		data[5] = r1c1;
		data[6] = r2c1;
		data[7] = r3c1;
		data[8] = r0c2;
		data[9] = r1c2;
		data[10] = r2c2;
		data[11] = r3c2;
		data[12] = r0c3;
		data[13] = r1c3;
		data[14] = r2c3;
		data[15] = r3c3;
		return *this;
	}

	/**
	 * Sets up this matrix by matrix m
	 * @param m m
	 * @return this matrix
	 */
	inline Matrix4x4& set(const array<float, 16>& m) {
		data = m;
		return *this;
	}

	/**
	 * Sets up this matrix by matrix m
	 * @param m m
	 * @return
	 */
	inline Matrix4x4& set(const Matrix4x4& m) {
		data = m.data;
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
	 * Operator * (float)
	 * @param f value to multiply by
	 * @return new matrix (this * f)
	 */
	inline Matrix4x4 operator *(const float f) const {
		auto r = this->clone().scale(f);
		return r;
	}

	/**
	 * Operator * (Matrix4x4&)
	 * @param m matrix to multiply by
	 * @return new matrix (this * m)
	 */
	inline Matrix4x4 operator *(const Matrix4x4& m) const {
		auto r = this->clone().multiply(m);
		return r;
	}

	/**
	 * Operator * (Vector3&)
	 * @param v vector to multiply by
	 * @return new vector (this * v)
	 */
	inline Vector3 operator *(const Vector3& v) const {
		auto r = Vector3();
		return this->multiply(v, r);
	}

	/**
	 * Operator * (Vector4&)
	 * @param v vector to multiply by
	 * @return new vector (this * v)
	 */
	inline Vector4 operator *(const Vector4& v) const {
		auto r = Vector4();
		return this->multiply(v, r);
	}

	/*
	 * Operator *=
	 * @param m matrix to multiply by
	 * @return this matrix multiplied by m
	 */
	inline Matrix4x4& operator *=(const Matrix4x4 m) {
		return this->multiply(m);
	}

	/**
	 * Equality comparison operator
	 * @param m matrix to compare to
	 * @return equality
	 */

	inline bool operator ==(const Matrix4x4& m) const {
		return this->equals(m);
	}

	/**
	 * Non equality comparison operator
	 * @param m matrix to compare to
	 * @return non equality
	 */

	inline bool operator !=(const Matrix4x4& m) const {
		return this->equals(m) == false;
	}

	/**
	 * Get coordinate system axes
	 * @param xAxis x axis
	 * @param yAxis y axis
	 * @param zAxis z axis
	 * @return this matrix
	 */
	inline void getAxes(Vector3& xAxis, Vector3& yAxis, Vector3& zAxis) const {
		xAxis.set(data[0], data[1], data[2]);
		yAxis.set(data[4], data[5], data[6]);
		zAxis.set(data[8], data[9], data[10]);
	}

	/**
	 * Set coordinate system axes
	 * @param xAxis x axis
	 * @param yAxis y axis
	 * @param zAxis z axis
	 * @return this matrix
	 */
	inline Matrix4x4& setAxes(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis) {
		data[0] = xAxis.data[0];
		data[1] = xAxis.data[1];
		data[2] = xAxis.data[2];
		data[3] = 0.0f;
		data[4] = yAxis.data[0];
		data[5] = yAxis.data[1];
		data[6] = yAxis.data[2];
		data[7] = 0.0f;
		data[8] = zAxis.data[0];
		data[9] = zAxis.data[1];
		data[10] = zAxis.data[2];
		data[11] = 0.0f;
		return *this;
	}

	/**
	 * Get translation
	 * @param translation translation
	 * @return this matrix
	 */
	inline void getTranslation(Vector3& translation) const {
		translation.set(data[12], data[13], data[14]);
	}

	/**
	 * Set translation
	 * @param translation translation
	 * @return this matrix
	 */
	inline Matrix4x4& setTranslation(const Vector3& translation) {
		data[12] = translation.data[0];
		data[13] = translation.data[1];
		data[14] = translation.data[2];
		return *this;
	}

	/**
	 * Get scale
	 * @param scale scale
	 * @return this matrix
	 */
	inline void getScale(Vector3& scale) const {
		Vector3 tmpVector3;
		// x axis
		tmpVector3.set(data[0], data[1], data[2]);
		scale.data[0] = tmpVector3.computeLength();
		// y axis
		tmpVector3.set(data[4], data[5], data[6]);
		scale.data[1] = tmpVector3.computeLength();
		// z axis
		tmpVector3.set(data[8], data[9], data[10]);
		scale.data[2] = tmpVector3.computeLength();
	}

	/**
	 * Get scale
	 * @param scale scale
	 * @return this matrix
	 */
	inline Matrix4x4& setScale(const Vector3& scale) {
		Vector3 tmpVector3;
		// x axis
		tmpVector3.set(data[0], data[1], data[2]);
		tmpVector3.normalize();
		tmpVector3.scale(scale.data[0]);
		data[0] = tmpVector3.data[0];
		data[1] = tmpVector3.data[1];
		data[2] = tmpVector3.data[2];
		// y axis
		tmpVector3.set(data[4], data[5], data[6]);
		tmpVector3.normalize();
		tmpVector3.scale(scale.data[1]);
		data[4] = tmpVector3.data[0];
		data[5] = tmpVector3.data[1];
		data[6] = tmpVector3.data[2];
		// z axis
		tmpVector3.set(data[8], data[9], data[10]);
		tmpVector3.normalize();
		tmpVector3.scale(scale.data[2]);
		data[8] = tmpVector3.data[0];
		data[9] = tmpVector3.data[1];
		data[10] = tmpVector3.data[2];
		return *this;
	}

	/**
	 * Setup identity matrix
	 * @return this matrix
	 */
	inline Matrix4x4& identity() {
		data[0] = 1.0f;
		data[1] = 0.0f;
		data[2] = 0.0f;
		data[3] = 0.0f;
		data[4] = 0.0f;
		data[5] = 1.0f;
		data[6] = 0.0f;
		data[7] = 0.0f;
		data[8] = 0.0f;
		data[9] = 0.0f;
		data[10] = 1.0f;
		data[11] = 0.0f;
		data[12] = 0.0f;
		data[13] = 0.0f;
		data[14] = 0.0f;
		data[15] = 1.0f;
		return *this;
	}

	/**
	 * Multiplies a vector3 with this matrix into destination vector
	 * @param v vector 3
	 * @param dest destination vector 3
	 * @return vector 3
	 */
	inline Vector3& multiply(const Vector3& v, Vector3& dest) const {
		return dest.set(
			v.data[0] * data[0] + v.data[1] * data[4] + v.data[2] * data[8] + data[12],
			v.data[0] * data[1] + v.data[1] * data[5] + v.data[2] * data[9] + data[13],
			v.data[0] * data[2] + v.data[1] * data[6] + v.data[2] * data[10] + data[14]
		);
	}

	/**
	 * Multiplies a vector3 with this matrix ignoring translation
	 * @param v vector 3
	 * @param dest destination vector 3
	 * @return vector 3 dest
	 */
	inline Vector3& multiplyNoTranslation(const Vector3& v, Vector3& dest) const {
		return dest.set(
			v.data[0] * data[0] + v.data[1] * data[4] + v.data[2] * data[8],
			v.data[0] * data[1] + v.data[1] * data[5] + v.data[2] * data[9],
			v.data[0] * data[2] + v.data[1] * data[6] + v.data[2] * data[10]
		);
	}

	/**
	 * Multiplies a vector4 with this matrix into destination vector
	 * @param v vector 4
	 * @param dest destination vector4
	 * @return destination vector 4
	 */
	inline Vector4& multiply(const Vector4& v, Vector4& dest) const {
		dest.set(
			v.data[0] * data[0] + v.data[1] * data[4] + v.data[2] * data[8] + v.data[3] * data[12],
			v.data[0] * data[1] + v.data[1] * data[5] + v.data[2] * data[9] + v.data[3] * data[13],
			v.data[0] * data[2] + v.data[1] * data[6] + v.data[2] * data[10] + v.data[3] * data[14],
			v.data[0] * data[3] + v.data[1] * data[7] + v.data[2] * data[11] + v.data[3] * data[15]
		);
		return dest;
	}

	/**
	 * Multiplies this matrix with another matrix
	 * @param m m
	 * @return this matrix
	 */
	inline Matrix4x4& multiply(const Matrix4x4& m) {
		array<float, 16> _data;
		_data[0] = data[0] * m.data[0] + data[1] * m.data[4] + data[2] * m.data[8] + data[3] * m.data[12];
		_data[1] = data[0] * m.data[1] + data[1] * m.data[5] + data[2] * m.data[9] + data[3] * m.data[13];
		_data[2] = data[0] * m.data[2] + data[1] * m.data[6] + data[2] * m.data[10] + data[3] * m.data[14];
		_data[3] = data[0] * m.data[3] + data[1] * m.data[7] + data[2] * m.data[11] + data[3] * m.data[15];
		_data[4] = data[4] * m.data[0] + data[5] * m.data[4] + data[6] * m.data[8] + data[7] * m.data[12];
		_data[5] = data[4] * m.data[1] + data[5] * m.data[5] + data[6] * m.data[9] + data[7] * m.data[13];
		_data[6] = data[4] * m.data[2] + data[5] * m.data[6] + data[6] * m.data[10] + data[7] * m.data[14];
		_data[7] = data[4] * m.data[3] + data[5] * m.data[7] + data[6] * m.data[11] + data[7] * m.data[15];
		_data[8] = data[8] * m.data[0] + data[9] * m.data[4] + data[10] * m.data[8] + data[11] * m.data[12];
		_data[9] = data[8] * m.data[1] + data[9] * m.data[5] + data[10] * m.data[9] + data[11] * m.data[13];
		_data[10] = data[8] * m.data[2] + data[9] * m.data[6] + data[10] * m.data[10] + data[11] * m.data[14];
		_data[11] = data[8] * m.data[3] + data[9] * m.data[7] + data[10] * m.data[11] + data[11] * m.data[15];
		_data[12] = data[12] * m.data[0] + data[13] * m.data[4] + data[14] * m.data[8] + data[15] * m.data[12];
		_data[13] = data[12] * m.data[1] + data[13] * m.data[5] + data[14] * m.data[9] + data[15] * m.data[13];
		_data[14] = data[12] * m.data[2] + data[13] * m.data[6] + data[14] * m.data[10] + data[15] * m.data[14];
		_data[15] = data[12] * m.data[3] + data[13] * m.data[7] + data[14] * m.data[11] + data[15] * m.data[15];
		data = _data;
		return *this;
	}

	/**
	 * Scales this matrix
	 * @param s s
	 * @returns this matrix
	 */
	inline Matrix4x4& scale(float s) {
		data[0] *= s;
		data[1] *= s;
		data[2] *= s;
		data[3] *= s;
		data[4] *= s;
		data[5] *= s;
		data[6] *= s;
		data[7] *= s;
		data[8] *= s;
		data[9] *= s;
		data[10] *= s;
		data[11] *= s;
		return *this;
	}

	/**
	 * Scales this matrix by given vector
	 * @param v v
	 * @return this matrix
	 */
	inline Matrix4x4& scale(const Vector3& v) {
		data[0] *= v.data[0];
		data[1] *= v.data[0];
		data[2] *= v.data[0];
		data[3] *= v.data[0];
		data[4] *= v.data[1];
		data[5] *= v.data[1];
		data[6] *= v.data[1];
		data[7] *= v.data[1];
		data[8] *= v.data[2];
		data[9] *= v.data[2];
		data[10] *= v.data[2];
		data[11] *= v.data[2];
		return *this;
	}

	/**
	 * Sets up a translation matrix
	 * @param v v
	 * @return this matrix
	 */
	inline Matrix4x4& translate(const Vector3& v) {
		data[12] += v.data[0] * data[0] + v.data[1] * data[4] + v.data[2] * data[8];
		data[13] += v.data[0] * data[1] + v.data[1] * data[5] + v.data[2] * data[9];
		data[14] += v.data[0] * data[2] + v.data[1] * data[6] + v.data[2] * data[10];
		return *this;
	}

	/**
	 * Creates a rotation matrix
	 * @param axis axis
	 * @param angle angle
	 * @return this matrix
	 */
	inline Matrix4x4& rotate(const Vector3& axis, float angle) {
		// see: http://www.songho.ca/opengl/gl_matrix.html
		auto& vXYZ = axis.getArray();
		float c = Math::cos(angle * Math::DEG2RAD);    // cosine
		float s = Math::sin(angle * Math::DEG2RAD);    // sine
		float c1 = 1.0f - c;                // 1 - c
		float m0 = data[0],  m4 = data[4],  m8 = data[8],  m12= data[12],
			  m1 = data[1],  m5 = data[5],  m9 = data[9],  m13= data[13],
			  m2 = data[2],  m6 = data[6],  m10= data[10], m14= data[14];
		float r0 = vXYZ[0] * vXYZ[0] * c1 + c;
		float r1 = vXYZ[0] * vXYZ[1] * c1 + vXYZ[2] * s;
		float r2 = vXYZ[0] * vXYZ[2] * c1 - vXYZ[1] * s;
		float r4 = vXYZ[0] * vXYZ[1] * c1 - vXYZ[2] * s;
		float r5 = vXYZ[1] * vXYZ[1] * c1 + c;
		float r6 = vXYZ[1] * vXYZ[2] * c1 + vXYZ[0] * s;
		float r8 = vXYZ[0] * vXYZ[2] * c1 + vXYZ[1] * s;
		float r9 = vXYZ[1] * vXYZ[2] * c1 - vXYZ[0] * s;
		float r10= vXYZ[2] * vXYZ[2] * c1 + c;
		data[0] = r0 * m0 + r4 * m1 + r8 * m2;
		data[1] = r1 * m0 + r5 * m1 + r9 * m2;
		data[2] = r2 * m0 + r6 * m1 + r10* m2;
		data[4] = r0 * m4 + r4 * m5 + r8 * m6;
		data[5] = r1 * m4 + r5 * m5 + r9 * m6;
		data[6] = r2 * m4 + r6 * m5 + r10* m6;
		data[8] = r0 * m8 + r4 * m9 + r8 * m10;
		data[9] = r1 * m8 + r5 * m9 + r9 * m10;
		data[10]= r2 * m8 + r6 * m9 + r10* m10;
		data[12]= r0 * m12+ r4 * m13+ r8 * m14;
		data[13]= r1 * m12+ r5 * m13+ r9 * m14;
		data[14]= r2 * m12+ r6 * m13+ r10* m14;
		return *this;
	}

	/**
	 * Transposes this matrix
	 * @return this matrix
	 */
	inline Matrix4x4& transpose() {
		array <float, 16> _data;
		_data[0] = data[0];
		_data[1] = data[4];
		_data[2] = data[8];
		_data[3] = data[12];
		_data[4] = data[1];
		_data[5] = data[5];
		_data[6] = data[9];
		_data[7] = data[13];
		_data[8] = data[2];
		_data[9] = data[6];
		_data[10] = data[10];
		_data[11] = data[14];
		_data[12] = data[3];
		_data[13] = data[7];
		_data[14] = data[11];
		_data[15] = data[15];
		data = _data;
		return *this;
	}

	/**
	 * Inverts the matrix
	 * @return this matrix
	 */
	inline Matrix4x4& invert() {
		/*
		 Taken from MESA GLU implementation
		 Copyright (C) 1999-2007  Brian Paul   All Rights Reserved.
		 Permission is hereby granted, free of charge, to any person obtaining a
		 copy of this software and associated documentation files (the "Software"),
		 to 	deal in the Software without restriction, including without limitation
		 the rights to use, copy, modify, merge, publish, distribute, sublicense,
		 and/or sell copies of the Software, and to permit persons to whom the
		 Software is furnished to do so, subject to the following conditions:
		 The above copyright notice and this permission notice shall be included
		 in all copies or substantial portions of the Software.
		 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
		 OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
		 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
		 BRIAN PAUL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
		 AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
		 CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
		*/
		array <float, 16> _data;
		_data[0] = data[5] * data[10] * data[15] - data[5] * data[11] * data[14] - data[9] * data[6] * data[15] + data[9] * data[7] * data[14] + data[13] * data[6] * data[11] - data[13] * data[7] * data[10];
		_data[4] = -data[4] * data[10] * data[15] + data[4] * data[11] * data[14] + data[8] * data[6] * data[15] - data[8] * data[7] * data[14] - data[12] * data[6] * data[11] + data[12] * data[7] * data[10];
		_data[8] = data[4] * data[9] * data[15] - data[4] * data[11] * data[13] - data[8] * data[5] * data[15] + data[8] * data[7] * data[13] + data[12] * data[5] * data[11] - data[12] * data[7] * data[9];
		_data[12] = -data[4] * data[9] * data[14] + data[4] * data[10] * data[13] + data[8] * data[5] * data[14] - data[8] * data[6] * data[13] - data[12] * data[5] * data[10] + data[12] * data[6] * data[9];
		_data[1] = -data[1] * data[10] * data[15] + data[1] * data[11] * data[14] + data[9] * data[2] * data[15] - data[9] * data[3] * data[14] - data[13] * data[2] * data[11] + data[13] * data[3] * data[10];
		_data[5] = data[0] * data[10] * data[15] - data[0] * data[11] * data[14] - data[8] * data[2] * data[15] + data[8] * data[3] * data[14] + data[12] * data[2] * data[11] - data[12] * data[3] * data[10];
		_data[9] = -data[0] * data[9] * data[15] + data[0] * data[11] * data[13] + data[8] * data[1] * data[15] - data[8] * data[3] * data[13] - data[12] * data[1] * data[11] + data[12] * data[3] * data[9];
		_data[13] = data[0] * data[9] * data[14] - data[0] * data[10] * data[13] - data[8] * data[1] * data[14] + data[8] * data[2] * data[13] + data[12] * data[1] * data[10] - data[12] * data[2] * data[9];
		_data[2] = data[1] * data[6] * data[15] - data[1] * data[7] * data[14] - data[5] * data[2] * data[15] + data[5] * data[3] * data[14] + data[13] * data[2] * data[7] - data[13] * data[3] * data[6];
		_data[6] = -data[0] * data[6] * data[15] + data[0] * data[7] * data[14] + data[4] * data[2] * data[15] - data[4] * data[3] * data[14] - data[12] * data[2] * data[7] + data[12] * data[3] * data[6];
		_data[10] = data[0] * data[5] * data[15] - data[0] * data[7] * data[13] - data[4] * data[1] * data[15] + data[4] * data[3] * data[13] + data[12] * data[1] * data[7] - data[12] * data[3] * data[5];
		_data[14] = -data[0] * data[5] * data[14] + data[0] * data[6] * data[13] + data[4] * data[1] * data[14] - data[4] * data[2] * data[13] - data[12] * data[1] * data[6] + data[12] * data[2] * data[5];
		_data[3] = -data[1] * data[6] * data[11] + data[1] * data[7] * data[10] + data[5] * data[2] * data[11] - data[5] * data[3] * data[10] - data[9] * data[2] * data[7] + data[9] * data[3] * data[6];
		_data[7] = data[0] * data[6] * data[11] - data[0] * data[7] * data[10] - data[4] * data[2] * data[11] + data[4] * data[3] * data[10] + data[8] * data[2] * data[7] - data[8] * data[3] * data[6];
		_data[11] = -data[0] * data[5] * data[11] + data[0] * data[7] * data[9] + data[4] * data[1] * data[11] - data[4] * data[3] * data[9] - data[8] * data[1] * data[7] + data[8] * data[3] * data[5];
		_data[15] = data[0] * data[5] * data[10] - data[0] * data[6] * data[9] - data[4] * data[1] * data[10] + data[4] * data[2] * data[9] + data[8] * data[1] * data[6] - data[8] * data[2] * data[5];
		auto determinant = data[0] * _data[0] + data[1] * _data[4] + data[2] * _data[8] + data[3] * _data[12];
		if (determinant == 0.0f) {
			identity();
			return *this;
		}
		determinant = 1.0f / determinant;
		for (auto i = 0; i < _data.size(); i++)
			_data[i] = _data[i] * determinant;
		data = _data;
		return *this;
	}

	/**
	 * Returns if this matrix equals m
	 * @param m m
	 * @return equals
	 */
	inline bool equals(const Matrix4x4& m) const {
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
				Math::abs(data[8] - m.data[8]) < Math::EPSILON &&
				Math::abs(data[9] - m.data[9]) < Math::EPSILON &&
				Math::abs(data[10] - m.data[10]) < Math::EPSILON &&
				Math::abs(data[11] - m.data[11]) < Math::EPSILON &&
				Math::abs(data[12] - m.data[12]) < Math::EPSILON &&
				Math::abs(data[13] - m.data[13]) < Math::EPSILON &&
				Math::abs(data[14] - m.data[14]) < Math::EPSILON &&
				Math::abs(data[15] - m.data[15]) < Math::EPSILON
			);
	}

	/**
	 * Returns array data
	 * @return array data
	 */
	inline array<float, 16>& getArray() const {
		return (array<float, 16>&)data;
	}

	/**
	 * Clones this matrix
	 * @return new cloned matrix
	 */
	inline Matrix4x4 clone() {
		Matrix4x4 clonedMatrix(*this);
		return clonedMatrix;
	}

	/**
	 * Interpolates between matrix 1 and matrix 2 by 0f<=t<=1f linearly
	 * @param m1 matrix 1
	 * @param m2 matrix 2
	 * @param t t
	 * @param dest destination matrix
	 * @return interpolated matrix
	 */
	inline static Matrix4x4& interpolateLinear(const Matrix4x4& m1, const Matrix4x4& m2, float t, Matrix4x4& dest) {
		return dest.set(
			(m2.data[0] * t) + ((1.0f - t) * m1.data[0]),
			(m2.data[1] * t) + ((1.0f - t) * m1.data[1]),
			(m2.data[2] * t) + ((1.0f - t) * m1.data[2]),
			(m2.data[3] * t) + ((1.0f - t) * m1.data[3]),
			(m2.data[4] * t) + ((1.0f - t) * m1.data[4]),
			(m2.data[5] * t) + ((1.0f - t) * m1.data[5]),
			(m2.data[6] * t) + ((1.0f - t) * m1.data[6]),
			(m2.data[7] * t) + ((1.0f - t) * m1.data[7]),
			(m2.data[8] * t) + ((1.0f - t) * m1.data[8]),
			(m2.data[9] * t) + ((1.0f - t) * m1.data[9]),
			(m2.data[10] * t) + ((1.0f - t) * m1.data[10]),
			(m2.data[11] * t) + ((1.0f - t) * m1.data[11]),
			(m2.data[12] * t) + ((1.0f - t) * m1.data[12]),
			(m2.data[13] * t) + ((1.0f - t) * m1.data[13]),
			(m2.data[14] * t) + ((1.0f - t) * m1.data[14]),
			(m2.data[15] * t) + ((1.0f - t) * m1.data[15])
		);
	}

	/**
	 * Compute Euler angles (rotation around x, y, z axes)
	 * @param euler euler
	 */
	inline void computeEulerAngles(Vector3& euler) const {
		/*
		see https://github.com/erich666/GraphicsGems/tree/master/gemsiv/euler_angle

			This code repository predates the concept of Open Source, and predates most licenses along such lines.
			As such, the official license truly is:
			EULA: The Graphics Gems code is copyright-protected.
			In other words, you cannot claim the text of the code as your own and resell it.
			Using the code is permitted in any program, product, or library, non-commercial or commercial.
			Giving credit is not required, though is a nice gesture.
			The code comes as-is, and if there are any flaws or problems with any Gems code,
			nobody involved with Gems - authors, editors, publishers, or webmasters - are to be held responsible.
			Basically, don't be a jerk, and remember that anything free comes with no guarantee.
		*/
		auto axis0 = 0;
		auto axis1 = 1;
		auto axis2 = 2;
		auto cy = static_cast< float >(Math::sqrt(data[axis0 + 4 * axis0] * data[axis0 + 4 * axis0] + data[axis1 + 4 * axis0] * data[axis1 + 4 * axis0]));
		if (cy > 16.0f * Math::EPSILON) {
			euler[0] = static_cast< float >((Math::atan2(data[axis2 + 4 * axis1], data[axis2 + 4 * axis2])));
			euler[1] = static_cast< float >((Math::atan2(-data[axis2 + 4 * axis0], cy)));
			euler[2] = static_cast< float >((Math::atan2(data[axis1 + 4 * axis0], data[axis0 + 4 * axis0])));
		} else {
			euler[0] = static_cast< float >((Math::atan2(-data[axis1 + 4 * axis2], data[axis1 + 4 * axis1])));
			euler[1] = static_cast< float >((Math::atan2(-data[axis2 + 4 * axis0], cy)));
			euler[2] = 0.0f;
		}
		euler.scale(static_cast< float >((180.0 / Math::PI)));
	}

	/**
	 * Clones this matrix
	 * @return new cloned matrix
	 */
	inline Matrix4x4 clone() const {
		return Matrix4x4(data);
	}

};
