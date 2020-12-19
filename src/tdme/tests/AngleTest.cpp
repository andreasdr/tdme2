#include <tdme/tests/AngleTest.h>

#include <array>
#include <string>

#include <tdme/engine/primitives/LineSegment.h>
#include <tdme/math/Vector3.h>
#include <tdme/utilities/Console.h>

using std::array;
using std::string;
using std::to_string;

using tdme::engine::primitives::LineSegment;
using tdme::math::Vector3;
using tdme::tests::AngleTest;
using tdme::utilities::Console;

void AngleTest::main()
{
	// test around x axis
	{
		array<Vector3, 4> aArray {
			Vector3(0.0f, 1.0f, 0.0f),
			Vector3(0.0f, 0.0f, -1.0f),
			Vector3(0.0f, -1.0f, 0.0f),
			Vector3(0.0f, 0.0f, 1.0f)
		};
		auto b = Vector3(0.0f, 1.0f, 0.0f);
		auto n = Vector3(1.0f, 0.0f, 0.0f);
		for (auto i = 0; i < aArray.size(); i++) {
			auto& a = aArray[i];
			Console::println(
				"Testing: " +
				to_string(a.getX()) + ", " +
				to_string(a.getY()) + ", " +
				to_string(a.getZ()) + " against " +
				to_string(b.getX()) + ", " +
				to_string(b.getY()) + ", " +
				to_string(b.getZ()) + " on " +
				to_string(n.getX()) + ", " +
				to_string(n.getY()) + ", " +
				to_string(n.getZ()) + ": " +
				to_string(Vector3::computeAngle(a, b, n))
			);
		}
	}
	// test around y axis
	{
		array<Vector3, 4> aArray {
			Vector3(0.0f, 0.0f, -1.0f),
			Vector3(1.0f, 0.0f, 0.0f),
			Vector3(0.0f, 0.0f, 1.0f),
			Vector3(-1.0f, 0.0f, 0.0f)
		};
		auto b = Vector3(0.0f, 0.0f, -1.0f);
		auto n = Vector3(0.0f, 1.0f, 0.0f);
		for (auto i = 0; i < aArray.size(); i++) {
			auto& a = aArray[i];
			Console::println(
				"Testing: " +
				to_string(a.getX()) + ", " +
				to_string(a.getY()) + ", " +
				to_string(a.getZ()) + " against " +
				to_string(b.getX()) + ", " +
				to_string(b.getY()) + ", " +
				to_string(b.getZ()) + " on " +
				to_string(n.getX()) + ", " +
				to_string(n.getY()) + ", " +
				to_string(n.getZ()) + ": " +
				to_string(Vector3::computeAngle(a, b, n))
			);
		}
	}
	// test around z axis
	{
		array<Vector3, 4> aArray {
			Vector3(0.0f, 1.0f, 0.0f),
			Vector3(1.0f, 0.0f, 0.0f),
			Vector3(0.0f, -1.0f, 0.0f),
			Vector3(-1.0f, 0.0f, 0.0f)
		};
		auto b = Vector3(0.0f, 1.0f, 0.0f);
		auto n = Vector3(0.0f, 0.0f, 1.0f);
		for (auto i = 0; i < aArray.size(); i++) {
			auto& a = aArray[i];
			Console::println(
				"Testing: " +
				to_string(a.getX()) + ", " +
				to_string(a.getY()) + ", " +
				to_string(a.getZ()) + " against " +
				to_string(b.getX()) + ", " +
				to_string(b.getY()) + ", " +
				to_string(b.getZ()) + " on " +
				to_string(n.getX()) + ", " +
				to_string(n.getY()) + ", " +
				to_string(n.getZ()) + ": " +
				to_string(Vector3::computeAngle(a, b, n))
			);
		}
	}
}
