#pragma once

#include <array>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/primitives/fwd-tdme.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/primitives/Plane.h>
#include <tdme/engine/primitives/Sphere.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Matrix4x4.h>

using std::array;

using tdme::engine::primitives::BoundingBox;
using tdme::engine::primitives::Plane;
using tdme::engine::primitives::Sphere;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;

/**
 * Frustum class
 * @author Mark Morley, Andreas Drewke
 * @version $Id$
 */
class tdme::engine::Frustum final
{
private:
	Renderer* renderer {  };

	// right, left, bottom, top, far, near
	static constexpr int32_t PLANE_RIGHT { 0 };
	static constexpr int32_t PLANE_LEFT { 1 };
	static constexpr int32_t PLANE_BOTTOM { 2 };
	static constexpr int32_t PLANE_TOP { 3 };
	static constexpr int32_t PLANE_FAR { 4 };
	static constexpr int32_t PLANE_NEAR { 5 };

	Matrix4x4 projectionMatrixTransposed;
	Matrix4x4 modelViewMatrixTransposed;
	Matrix4x4 frustumMatrix;

	array<Plane, 6> planes;

public:
	/**
	 * Public constructor
	 */
	Frustum(Renderer* renderer);

	/**
	 * Setups frustum, should be called if frustum did change
	 */
	void updateFrustum();

	/**
	 * Checks if given vector is in frustum
	 * @param vector vecto
	 * @return visibility
	 */
	inline bool isVisible(const Vector3& vector) {
		for (auto& p : planes) {
			auto& normal = p.getNormal();
			auto distance = p.getDistance();
			if (Vector3::computeDotProduct(normal, vector) + distance <= 0) {
				return false;
			}
		}
		return true;
	}

	/**
	 * Checks if sphere is in frustum
	 * @param s s
	 * @return visibility
	 */
	inline bool isVisible(Sphere* s) {
		auto& center = s->getCenter();
		auto radius = s->getRadius();
		for (auto& p : planes) {
			auto& normal = p.getNormal();
			if (Vector3::computeDotProduct(normal, center) + p.getDistance() <= -radius) {
				return false;
			}
		}
		return true;
	}

	/**
	 * Checks if bounding box is in frustum
	 * @param b s
	 * @return visibility
	 */
	inline bool isVisible(BoundingBox* b) {
		auto minX = b->getMin()[0];
		auto minY = b->getMin()[1];
		auto minZ = b->getMin()[2];
		auto maxX = b->getMax()[0];
		auto maxY = b->getMax()[1];
		auto maxZ = b->getMax()[2];
		Vector3 point;
		for (auto& p : planes) {
			auto& normal = p.getNormal();
			auto distance = p.getDistance();
			if (Vector3::computeDotProduct(normal, point.set(minX, minY, minZ)) + distance > 0) continue;
			if (Vector3::computeDotProduct(normal, point.set(maxX, minY, minZ)) + distance > 0) continue;
			if (Vector3::computeDotProduct(normal, point.set(minX, maxY, minZ)) + distance > 0) continue;
			if (Vector3::computeDotProduct(normal, point.set(maxX, maxY, minZ)) + distance > 0) continue;
			if (Vector3::computeDotProduct(normal, point.set(minX, minY, maxZ)) + distance > 0) continue;
			if (Vector3::computeDotProduct(normal, point.set(maxX, minY, maxZ)) + distance > 0) continue;
			if (Vector3::computeDotProduct(normal, point.set(minX, maxY, maxZ)) + distance > 0) continue;
			if (Vector3::computeDotProduct(normal, point.set(maxX, maxY, maxZ)) + distance > 0) continue;
			return false;
		}
		return true;
	}

};
