#include <tdme/engine/primitives/LineSegment.h>

#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/primitives/OrientedBoundingBox.h>
#include <tdme/math/Math.h>
#include <tdme/math/Vector3.h>

using tdme::engine::primitives::LineSegment;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::primitives::OrientedBoundingBox;
using tdme::math::Math;
using tdme::math::Vector3;

void LineSegment::computeClosestPointOnLineSegment(const Vector3& p1, const Vector3& q1, const Vector3& p, Vector3& c) {
	// see https://forum.unity.com/threads/how-do-i-find-the-closest-point-on-a-line.340058/
	Vector3 lineDirection = q1.clone().sub(p1);
	float lineLength = lineDirection.computeLength();
	lineDirection.normalize();
	Vector3 pSubP1 = p.clone().sub(p1);
	float t = Vector3::computeDotProduct(pSubP1, lineDirection);
	if (t < 0.0f) t = 0.0f;
	if (t > lineLength) t = lineLength;
	c.set(p1).add(lineDirection.scale(t));
}

bool LineSegment::doesLineSegmentsCollide(const Vector3& p1, const Vector3& q1, const Vector3& p2, const Vector3& q2, Vector3& p)
{
	Vector3 c1;
	Vector3 c2;
	computeClosestPointsOnLineSegments(p1, q1, p2, q2, c1, c2);
	if (c1.sub(c2).computeLength() < Math::EPSILON) {
		p.set(c2);
		return true;
	} else {
		return false;
	}
}

void LineSegment::computeClosestPointsOnLineSegments(const Vector3& p1, const Vector3& q1, const Vector3& p2, const Vector3& q2, Vector3& c1, Vector3& c2)
{
	Vector3 d1;
	Vector3 d2;
	Vector3 r;
	d1.set(q1).sub(p1);
	d2.set(q2).sub(p2);
	r.set(p1).sub(p2);
	auto a = Vector3::computeDotProduct(d1, d1);
	auto e = Vector3::computeDotProduct(d2, d2);
	auto f = Vector3::computeDotProduct(d2, r);
	// both line segments degenerate into points?
	if (a <= Math::EPSILON && e <= Math::EPSILON) {
		c1 = p1;
		c2 = p2;
		return;
	}
	// first line segment degenerates into point?
	float s;
	float t;
	if (a <= Math::EPSILON) {
		s = 0.0f;
		t = f / e;
		t = Math::clamp(t, 0.0f, 1.0f);
	} else {
		auto c = Vector3::computeDotProduct(d1, r);
		// second line segment degenerates into point?
		if (e <= Math::EPSILON) {
			t = 0.0f;
			s = Math::clamp(-c / a, 0.0f, 1.0f);
		} else {
			// nope, use general case
			auto b = Vector3::computeDotProduct(d1, d2);
			auto denom = a * e - b * b;
			if (denom != 0.0f) {
				s = Math::clamp((b * f - c * e) / denom, 0.0f, 1.0f);
			} else {
				s = 0.0f;
			}
			t = (b * s + f) / e;
			if (t < 0.0f) {
				t = 0.0f;
				s = Math::clamp(-c / a, 0.0f, 1.0f);
			} else if (t > 1.0f) {
				t = 1.0f;
				s = Math::clamp((b - c) / a, 0.0f, 1.0f);
			}
		}
	}
	c1.set(p1).add(d1.scale(s));
	c2.set(p2).add(d2.scale(t));
}

bool LineSegment::doesBoundingBoxCollideWithLineSegment(BoundingBox* boundingBox, const Vector3& p, const Vector3& q, Vector3& contactMin, Vector3& contactMax)
{
	Vector3 d;
	Vector3 d1;
	Vector3 d2;
	auto tmin = 0.0f;
	auto tmax = 1.0f;
	auto minXYZ = boundingBox->getMin().getArray();
	auto maxXYZ = boundingBox->getMax().getArray();
	d.set(q).sub(p);
	auto& directionXYZ = d.getArray();
	auto& pointXYZ = p.getArray();
	for (auto i = 0; i < 3; i++) {
		if (Math::abs(directionXYZ[i]) < Math::EPSILON &&
			(pointXYZ[i] <= minXYZ[i] || pointXYZ[i] >= maxXYZ[i])) {
			return false;
		} else {
			auto odd = 1.0f / directionXYZ[i];
			auto t1 = (minXYZ[i] - pointXYZ[i]) * odd;
			auto t2 = (maxXYZ[i] - pointXYZ[i]) * odd;
			if (t1 > t2) {
				auto tmp = t1;
				t1 = t2;
				t2 = tmp;
			}
			tmin = Math::max(tmin, t1);
			tmax = Math::min(tmax, t2);
			if (tmin > tmax)
				return false;

		}
	}
	if (tmin > 1.0) return false;
	// compute contact points
	contactMin.set(p).add(d1.set(d).scale(tmin));
	contactMax.set(p).add(d2.set(d).scale(tmax));
	// we have a collision
	return true;
}

bool LineSegment::doesOrientedBoundingBoxCollideWithLineSegment(OrientedBoundingBox* orientedBoundingBox, const Vector3& p, const Vector3& q, Vector3& contactMin, Vector3& contactMax)
{
	Vector3 d;
	Vector3 d1;
	Vector3 d2;
	auto tmin = 0.0f;
	auto tmax = 1.0f;
	auto obbAxes = orientedBoundingBox->getAxes();
	auto obbCenter = orientedBoundingBox->getCenter();
	auto obbHalfExtension = orientedBoundingBox->getHalfExtension();
	auto& obbHalfExtensionXYZ = obbHalfExtension.getArray();
	d.set(q).sub(p);
	for (auto i = 0; i < 3; i++) {
		auto directionLengthOnAxis = Vector3::computeDotProduct(d, obbAxes[i]);
		auto obbExtensionLengthOnAxis = obbHalfExtensionXYZ[i];
		auto obbCenterLengthOnAxis = Vector3::computeDotProduct(obbCenter, obbAxes[i]);
		auto pointLengthOnAxis = Vector3::computeDotProduct(p, obbAxes[i]);
		if (Math::abs(directionLengthOnAxis) < Math::EPSILON &&
			(pointLengthOnAxis <= obbCenterLengthOnAxis - obbExtensionLengthOnAxis ||
			pointLengthOnAxis >= obbCenterLengthOnAxis + obbExtensionLengthOnAxis)) {
			return false;
		} else {
			auto odd = 1.0f / directionLengthOnAxis;
			auto t1 = (obbCenterLengthOnAxis - obbExtensionLengthOnAxis - pointLengthOnAxis) * odd;
			auto t2 = (obbCenterLengthOnAxis + obbExtensionLengthOnAxis - pointLengthOnAxis) * odd;
			if (t1 > t2) {
				auto tmp = t1;
				t1 = t2;
				t2 = tmp;
			}
			tmin = Math::max(tmin, t1);
			tmax = Math::min(tmax, t2);
			if (tmin > tmax)
				return false;

		}
	}
	if (tmin > 1.0) return false;
	// compute contact points
	contactMin.set(p).add(d1.set(d).scale(tmin));
	contactMax.set(p).add(d2.set(d).scale(tmax));
	// we have a collision
	return true;
}

bool LineSegment::doesLineSegmentCollideWithTriangle(const Vector3& p1, const Vector3& p2, const Vector3& p3, const Vector3& r1, const Vector3& r2, Vector3& contact)
{
	// see: https://gamedev.stackexchange.com/questions/5585/line-triangle-intersection-last-bits
	Vector3 d1;
	Vector3 d2;
	Vector3 n;
	Vector3 t;
	// find triangle normal
	Vector3::computeCrossProduct(d1.set(p2).sub(p1), d2.set(p3).sub(p1), n).normalize();
	// find distance from LP1 and LP2 to the plane defined by the triangle
	auto dist1 = Vector3::computeDotProduct(d1.set(r1).sub(p1), n);
	auto dist2 = Vector3::computeDotProduct(d2.set(r2).sub(p1), n);
	// check if line doesn't cross the triangle.
	if (dist1 * dist2 >= 0.0f) {
		return false;
	}
	// line and plane are parallel
	if (Math::abs(dist1 - dist2) < Math::EPSILON) {
		return false;
	}
	// Find point on the line that intersects with the plane
	contact.set(r2).sub(r1).scale(-dist1 / (dist2 - dist1));
	contact.add(r1);
	// check intersection p2-p1
	Vector3::computeCrossProduct(n, d1.set(p2).sub(p1), t);
	if (Vector3::computeDotProduct(t, d2.set(contact).sub(p1)) < 0.0f) {
		return false;
	}
	// check intersection p3-p2
	Vector3::computeCrossProduct(n, d1.set(p3).sub(p2), t);
	if (Vector3::computeDotProduct(t, d2.set(contact).sub(p2)) < 0.0f) {
		return false;
	}
	// check intersection p1-p3
	Vector3::computeCrossProduct(n, d1.set(p1).sub(p3), t);
	if (Vector3::computeDotProduct(t, d2.set(contact).sub(p1)) < 0.0f) {
		return false;
	}
	// intersection
	return true;
}
