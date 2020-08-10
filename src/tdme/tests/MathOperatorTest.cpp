#include <tdme/tests/MathOperatorTest.h>

#include <tdme/math/Matrix2D3x3.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Quaternion.h>
#include <tdme/math/Vector2.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Vector4.h>
#include <tdme/utils/Console.h>

using tdme::tests::MathOperatorTest;
using tdme::math::Matrix2D3x3;
using tdme::math::Matrix4x4;
using tdme::math::Quaternion;
using tdme::math::Vector2;
using tdme::math::Vector3;
using tdme::math::Vector4;
using tdme::utils::Console;

MathOperatorTest::MathOperatorTest()
{
}

void MathOperatorTest::main()
{
	auto mt = new MathOperatorTest();
	Console::println(string("Math operator tests:"));
	mt->testVector2Operators();
	mt->testVector3Operators();
	mt->testVector4Operators();
	mt->testQuaternionOperators();
	mt->testMatrix2D3x3Operators();
	mt->testMatrix4x4Operators();
}

void MathOperatorTest::testVector2Operators()
{
	Vector2 v1 = Vector2(14.0, 45.3);
	Vector2 v2, v3, v4 = Vector2(0.48, 19.33);
	float f = 3.4;

	Console::println(string("\nVector2 operators\n-----------------"));

	// operator ==
	Console::print(string("operator ==: "));
	if ((v1 == v1) == (v1.equals(v1) )) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	// operator !=
	Console::print(string("operator !=: "));
	if ((v1 != v2) == (v1.equals(v2) == false)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	// operator +
	Console::print(string("operator +: "));
	if (v2 + v1 == v3.add(v1)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	// operator -
	v3 = v2;
	Console::print(string("operator -: "));
	if (v2 - v1 == v3.sub(v1)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	// operator *
	v3 = v2;
	Console::print(string("operator *(float): "));
	if (v3 * f == v3.scale(f)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}
	v3 = v2;
	Console::print(string("operator *(Vector2&): "));
	if (v3 * v2 == v3.scale(v2)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	// operator /
	v2 = v4;
	v3 = v4;
	Console::print(string("operator /(float): "));
	if (v2 / f == v3.scale(1.0f / f)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	v2 = v4;
	v3 = v4;
	auto vInverted = Vector2(1.0f / v1[0], 1.0f / v1[1]);
	Console::print(string("operator /(Vector2&): "));
	if ((v2 / v1) == v3.scale(vInverted)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	// operator +=
	v3 = v2;
	Console::print(string("operator +=: "));
	if ((v2 += v1) == v3.add(v1)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	// operator -=
	v2 = v4;
	v3 = v4;
	Console::print(string("operator -=: "));
	if ((v2 -= v1) == v3.sub(v1)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	// operator *=
	v2 = v4;
	v3 = v4;
	Console::print(string("operator *=: "));
	if ((v2 *= v1) == v3.scale(v1)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	// operator /=
	v2 = v4;
	v3 = v4;
	vInverted = Vector2(1.0f / v1[0], 1.0f / v1[1]);
	Console::print(string("operator /=: "));
	if ((v2 /= v1) == v3.scale(vInverted)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}
}

void MathOperatorTest::testVector3Operators()
{
	Vector3 v1 = Vector3(14.0, 45.3, 0.342);
	Vector3 v2, v3, v4 = Vector3(0.48, 19.33, 7.209);
	float f = 3.4;

	Console::println(string("\nVector3 operators\n-----------------"));

	// operator ==
	Console::print(string("operator ==: "));
	if ((v1 == v1) == (v1.equals(v1) )) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	// operator !=
	Console::print(string("operator !=: "));
	if ((v1 != v2) == (v1.equals(v2) == false)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	// operator +
	Console::print(string("operator +: "));
	if (v2 + v1 == v3.add(v1)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	// operator -
	v3 = v2;
	Console::print(string("operator -: "));
	if (v2 - v1 == v3.sub(v1)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	// operator *
	v3 = v2;
	Console::print(string("operator *(float): "));
	if (v3 * f == v3.scale(f)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}
	v3 = v2;
	Console::print(string("operator *(Vector3&): "));
	if (v3 * v2 == v3.scale(v2)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	// operator /
	v2 = v4;
	v3 = v4;
	Console::print(string("operator /(float): "));
	if (v2 / f == v3.scale(1.0f / f)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	v2 = v4;
	v3 = v4;
	auto vInverted = Vector3(1.0f / v1[0], 1.0f / v1[1], 1.0f / v1[2]);
	Console::print(string("operator /(Vector3&): "));
	if ((v2 / v1) == v3.scale(vInverted)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	// operator +=
	v3 = v2;
	Console::print(string("operator +=: "));
	if ((v2 += v1) == v3.add(v1)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	// operator -=
	v2 = v4;
	v3 = v4;
	Console::print(string("operator -=: "));
	if ((v2 -= v1) == v3.sub(v1)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	// operator *=
	v2 = v4;
	v3 = v4;
	Console::print(string("operator *=: "));
	if ((v2 *= v1) == v3.scale(v1)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	// operator /=
	v2 = v4;
	v3 = v4;
	vInverted = Vector3(1.0f / v1[0], 1.0f / v1[1], 1.0f / v1[2]);
	Console::print(string("operator /=: "));
	if ((v2 /= v1) == v3.scale(vInverted)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}
}

void MathOperatorTest::testVector4Operators() {
	Vector4 v1 = Vector4(14.0, 45.3, 0.342, 2.43);
	Vector4 v2, v3, v4 = Vector4(0.48, 19.33, 7.209, 5.905);
	float f = 3.4;

	Console::println(string("\nVector4 operators\n-----------------"));

	// operator ==
	Console::print(string("operator ==: "));
	if ((v1 == v1) == (v1.equals(v1) )) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	// operator !=
	Console::print(string("operator !=: "));
	if ((v1 != v2) == (v1.equals(v2) == false)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	// operator +
	Console::print(string("operator +: "));
	if (v2 + v1 == v3.add(v1)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	// operator -
	v3 = v2;
	Console::print(string("operator -: "));
	if (v2 - v1 == v3.sub(v1)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	// operator *
	v3 = v2;
	Console::print(string("operator *(float): "));
	if (v3 * f == v3.scale(f)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}
	v3 = v2;
	Console::print(string("operator *(Vector3&): "));
	if (v3 * v2 == v3.scale(v2)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	// operator /
	v2 = v4;
	v3 = v4;
	Console::print(string("operator /(float): "));
	if (v2 / f == v3.scale(1.0f / f)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	v2 = v4;
	v3 = v4;
	auto vInverted = Vector4(1.0f / v1[0], 1.0f / v1[1], 1.0f / v1[2], 1.0f / v1[3]);
	Console::print(string("operator /(Vector3&): "));
	if ((v2 / v1) == v3.scale(vInverted)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	// operator +=
	v3 = v2;
	Console::print(string("operator +=: "));
	if ((v2 += v1) == v3.add(v1)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	// operator -=
	v2 = v4;
	v3 = v4;
	Console::print(string("operator -=: "));
	if ((v2 -= v1) == v3.sub(v1)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	// operator *=
	v2 = v4;
	v3 = v4;
	Console::print(string("operator *=: "));
	if ((v2 *= v1) == v3.scale(v1)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	// operator /=
	v2 = v4;
	v3 = v4;
	vInverted = Vector4(1.0f / v1[0], 1.0f / v1[1], 1.0f / v1[2], 1.0f / v1[3]);
	Console::print(string("operator /=: "));
	if ((v2 /= v1) == v3.scale(vInverted)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}
}

void MathOperatorTest::testQuaternionOperators() {
	Quaternion q1 = Quaternion(14.0, 45.3, 0.342, 2.43);
	Quaternion q2, q3, q4 = Quaternion(0.48, 19.33, 7.209, 5.905);
	float f = 3.4;

	Console::println(string("\nQuaternion operators\n-----------------"));

	// operator ==
	Console::print(string("operator ==: "));
	if ((q1 == q1) == (q1.equals(q1) )) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	// operator !=
	Console::print(string("operator !=: "));
	if ((q1 != q2) == (q1.equals(q2) == false)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	// operator +
	Console::print(string("operator +: "));
	if (q2 + q1 == q3.add(q1)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	// operator -
	q3 = q2;
	Console::print(string("operator -: "));
	if (q2 - q1 == q3.sub(q1)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	// operator *
	q3 = q2;
	Console::print(string("operator *(float): "));
	if (q3 * f == q3.scale(f)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	q3 = q2;
	Console::print(string("operator *(Quaternion&): "));
	if (q3 * q2 == q3.multiply(q2)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	q3 = q2;
	auto v1 = Vector3(4.75, 0.98, 7.0228);
	auto v2 = Vector3();
	Console::print(string("operator *(Vector3&): "));
	q3.multiply(v1, v2);
	if (q3 * v1 == v2) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	// operator /
	q2 = q4;
	q3 = q4;
	Console::print(string("operator /(float): "));
	if (q2 / f == q3.scale(1.0f / f)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	q2 = q4;
	q3 = q4;
	auto qInverted = Quaternion(1.0f / q1[0], 1.0f / q1[1], 1.0f / q1[2], 1.0f / q1[3]);
	Console::print(string("operator /(Quaternion&): "));
	if ((q2 / q1) == q3.multiply(qInverted)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	// operator +=
	q3 = q2;
	Console::print(string("operator +=: "));
	if ((q2 += q1) == q3.add(q1)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	// operator -=
	q2 = q4;
	q3 = q4;
	Console::print(string("operator -=: "));
	if ((q2 -= q1) == q3.sub(q1)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	// operator *=
	q2 = q4;
	q3 = q4;
	Console::print(string("operator *=: "));
	if ((q2 *= q1) == q3.multiply(q1)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	// operator /=
	q2 = q4;
	q3 = q4;
	qInverted = Quaternion(1.0f / q1[0], 1.0f / q1[1], 1.0f / q1[2], 1.0f / q1[3]);
	Console::print(string("operator /=: "));
	if ((q2 /= q1) == q3.multiply(qInverted)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}
}

void MathOperatorTest::testMatrix2D3x3Operators() {
	Matrix2D3x3 m1 = Matrix2D3x3(14.0, 45.3, 0.342, 2.43, 14.0, 45.3, 0.342, 2.43, 4.5);
	Matrix2D3x3 m2, m3, m4 = Matrix2D3x3(0.48, 19.33, 7.209, 5.905, 9.0, 14.0, 45.3, 0.342, 2.43);
	float f = 3.4;


	Console::println(string("\nMatrix2D3x3 operators\n-----------------"));

	// operator ==
	Console::print(string("operator ==: "));
	if ((m1 == m1) == (m1.equals(m1) )) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	// operator !=
	Console::print(string("operator !=: "));
	if ((m1 != m2) == (m1.equals(m2) == false)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	// operator *
	Console::print(string("operator *(float): "));
	if (m2 * f == m3.scale(f)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	Console::print(string("operator *(Matrix2D3x3&): "));
	if (m2 * m3 == m2.multiply(m3)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	auto v1 = Vector2(4.75, 0.98);
	auto v2 = Vector2();
	Console::print(string("operator *(Vector2&): "));
	m2.multiply(v1, v2);
	if (m3 * v1 == v2) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	// operator *=
	m2 = m3;
	Console::print(string("operator *=: "));
	if ((m2 *= m1) == m3.multiply(m1)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}
}

void MathOperatorTest::testMatrix4x4Operators() {
	Matrix4x4 m1 = Matrix4x4(14.0, 45.3, 0.342, 2.43, 14.0, 45.3, 0.342, 2.43, 4.5, 0.342, 2.43, 14.0, 45.3, 0.342, 2.43, 4.5);
	Matrix4x4 m2, m3, m4 = Matrix4x4(0.48, 19.33, 7.209, 5.905, 9.0, 0.342, 2.43, 14.0, 45.3, 0.342, 2.43, 4.5, 14.0, 45.3, 0.342, 2.43);
	Vector3 v3 = Vector3(9.0, 0.342, 2.43);
	Vector4 v4 = Vector4(9.0, 0.342, 2.43, 7.082);
	float f = 3.4;


	Console::println(string("\nMatrix4x4 operators\n-----------------"));

	// operator ==
	Console::print(string("operator ==: "));
	if ((m1 == m1) == (m1.equals(m1) )) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	// operator !=
	Console::print(string("operator !=: "));
	if ((m1 != m2) == (m1.equals(m2) == false)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	// operator *
	Console::print(string("operator *(float): "));
	if (m2 * f == m3.scale(f)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	Console::print(string("operator *(Matrix4x4&): "));
	if (m2 * m3 == m2.multiply(m3)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	Vector3 v03 = Vector3();
	Console::print(string("operator *(Vector3&): "));
	m2.multiply(v3, v03);
	if (m3 * v3 == v03) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	Vector4 v04 = Vector4();
	Console::print(string("operator *(Vector4&): "));
	m2.multiply(v4, v04);
	if (m3 * v4 == v04) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}

	// operator *=
	m2 = m3;
	Console::print(string("operator *=: "));
	if ((m2 *= m1) == m3.multiply(m1)) {
		Console::println(string(this->success));
	} else {
		Console::println(string(this->fail));
	}
}
