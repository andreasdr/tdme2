#include <tdme/engine/Frustum.h>

#include <array>

#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/primitives/Plane.h>
#include <tdme/engine/primitives/Sphere.h>
#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/math/Math.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector3.h>

using std::array;

using tdme::engine::primitives::BoundingBox;
using tdme::engine::primitives::Plane;
using tdme::engine::primitives::Sphere;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::Frustum;
using tdme::math::Math;
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
	float x, y, z, d, t;

	// right plane
	x = frustumMatrix[12] - frustumMatrix[0];
	y = frustumMatrix[13] - frustumMatrix[1];
	z = frustumMatrix[14] - frustumMatrix[2];
	d = frustumMatrix[15] - frustumMatrix[3];

	// 	normalize
	t = Math::sqrt((x * x) + (y * y) + (z * z));
	x /= t;
	y /= t;
	z /= t;
	d /= t;

	//	setup plane
	planes[0].setNormal(Vector3(x, y, z));
	planes[0].setDistance(d);

	// left plane
	x = frustumMatrix[12] + frustumMatrix[0];
	y = frustumMatrix[13] + frustumMatrix[1];
	z = frustumMatrix[14] + frustumMatrix[2];
	d = frustumMatrix[15] + frustumMatrix[3];

	// 	normalize
	t = Math::sqrt((x * x) + (y * y) + (z * z));
	x /= t;
	y /= t;
	z /= t;
	d /= t;

	//	setup plane
	planes[1].setNormal(Vector3(x, y, z));
	planes[1].setDistance(d);

	// bottom plane
	x = frustumMatrix[12] + frustumMatrix[4];
	y = frustumMatrix[13] + frustumMatrix[5];
	z = frustumMatrix[14] + frustumMatrix[6];
	d = frustumMatrix[15] + frustumMatrix[7];

	// 	normalize
	t = Math::sqrt((x * x) + (y * y) + (z * z));
	x /= t;
	y /= t;
	z /= t;
	d /= t;

	//	setup plane
	planes[2].setNormal(Vector3(x, y, z));
	planes[2].setDistance(d);

	// top plane
	x = frustumMatrix[12] - frustumMatrix[4];
	y = frustumMatrix[13] - frustumMatrix[5];
	z = frustumMatrix[14] - frustumMatrix[6];
	d = frustumMatrix[15] - frustumMatrix[7];

	// 	normalize
	t = Math::sqrt((x * x) + (y * y) + (z * z));
	x /= t;
	y /= t;
	z /= t;
	d /= t;

	//	setup plane
	planes[3].setNormal(Vector3(x, y, z));
	planes[3].setDistance(d);

	// far plane
	x = frustumMatrix[12] - frustumMatrix[8];
	y = frustumMatrix[13] - frustumMatrix[9];
	z = frustumMatrix[14] - frustumMatrix[10];
	d = frustumMatrix[15] - frustumMatrix[11];

	// 	normalize
	t = Math::sqrt((x * x) + (y * y) + (z * z));
	x /= t;
	y /= t;
	z /= t;
	d /= t;

	//	setup plane
	planes[4].setNormal(Vector3(x, y, z));
	planes[4].setDistance(d);

	// near plane
	x = frustumMatrix[12] + frustumMatrix[8];
	y = frustumMatrix[13] + frustumMatrix[9];
	z = frustumMatrix[14] + frustumMatrix[10];
	d = frustumMatrix[15] + frustumMatrix[11];

	// 	normalize
	t = Math::sqrt((x * x) + (y * y) + (z * z));
	x /= t;
	y /= t;
	z /= t;
	d /= t;

	//	setup plane
	planes[5].setNormal(Vector3(x, y, z));
	planes[5].setDistance(d);
}

