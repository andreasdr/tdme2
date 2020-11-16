#include <tdme/engine/Frustum.h>

#include <array>

#include <tdme/math/Math.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/primitives/Plane.h>
#include <tdme/engine/primitives/Sphere.h>
#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector3.h>

using std::array;

using tdme::engine::Frustum;
using tdme::math::Math;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::primitives::Plane;
using tdme::engine::primitives::Sphere;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;

Frustum::Frustum(Renderer* renderer)
{
	this->renderer = renderer;
}

constexpr int32_t Frustum::PLANE_RIGHT;
constexpr int32_t Frustum::PLANE_LEFT;
constexpr int32_t Frustum::PLANE_BOTTOM;
constexpr int32_t Frustum::PLANE_TOP;
constexpr int32_t Frustum::PLANE_FAR;
constexpr int32_t Frustum::PLANE_NEAR;

void Frustum::updateFrustum()
{
	// see: http://www.crownandcutlass.com/features/technicaldetails/frustum.html
	projectionMatrixTransposed.set(renderer->getProjectionMatrix()).transpose();
	modelViewMatrixTransposed.set(renderer->getModelViewMatrix()).transpose();
	frustumMatrix.set(projectionMatrixTransposed).multiply(modelViewMatrixTransposed);
	auto& data = frustumMatrix.getArray();
	float x, y, z, d, t;

	// right plane
	x = data[12] - data[0];
	y = data[13] - data[1];
	z = data[14] - data[2];
	d = data[15] - data[3];

	// 	normalize
	t = Math::sqrt((x * x) + (y * y) + (z * z));
	x /= t;
	y /= t;
	z /= t;
	d /= t;

	//	setup plane
	planes[0].getNormal().set(x, y, z);
	planes[0].setDistance(d);

	// left plane
	x = data[12] + data[0];
	y = data[13] + data[1];
	z = data[14] + data[2];
	d = data[15] + data[3];

	// 	normalize
	t = Math::sqrt((x * x) + (y * y) + (z * z));
	x /= t;
	y /= t;
	z /= t;
	d /= t;

	//	setup plane
	planes[1].getNormal().set(x, y, z);
	planes[1].setDistance(d);

	// bottom plane
	x = data[12] + data[4];
	y = data[13] + data[5];
	z = data[14] + data[6];
	d = data[15] + data[7];

	// 	normalize
	t = Math::sqrt((x * x) + (y * y) + (z * z));
	x /= t;
	y /= t;
	z /= t;
	d /= t;

	//	setup plane
	planes[2].getNormal().set(x, y, z);
	planes[2].setDistance(d);

	// top plane
	x = data[12] - data[4];
	y = data[13] - data[5];
	z = data[14] - data[6];
	d = data[15] - data[7];

	// 	normalize
	t = Math::sqrt((x * x) + (y * y) + (z * z));
	x /= t;
	y /= t;
	z /= t;
	d /= t;

	//	setup plane
	planes[3].getNormal().set(x, y, z);
	planes[3].setDistance(d);

	// far plane
	x = data[12] - data[8];
	y = data[13] - data[9];
	z = data[14] - data[10];
	d = data[15] - data[11];

	// 	normalize
	t = Math::sqrt((x * x) + (y * y) + (z * z));
	x /= t;
	y /= t;
	z /= t;
	d /= t;

	//	setup plane
	planes[4].getNormal().set(x, y, z);
	planes[4].setDistance(d);

	// near plane
	x = data[12] + data[8];
	y = data[13] + data[9];
	z = data[14] + data[10];
	d = data[15] + data[11];

	// 	normalize
	t = Math::sqrt((x * x) + (y * y) + (z * z));
	x /= t;
	y /= t;
	z /= t;
	d /= t;

	//	setup plane
	planes[5].getNormal().set(x, y, z);
	planes[5].setDistance(d);
}

