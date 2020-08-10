
#pragma once

#include <tdme/tdme.h>
#include <tdme/engine/primitives/fwd-tdme.h>
#include <tdme/math/fwd-tdme.h>

using tdme::engine::primitives::BoundingBox;
using tdme::engine::primitives::OrientedBoundingBox;
using tdme::math::Vector3;

/**
 * Line segment helper functions
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::primitives::LineSegment final
{
public:

	/**
	 * Compute closest point on line segment
	 * @param p1 p1 line 1 point 1
	 * @param q1 q1 line 1 point 2
	 * @param p p point
	 * @param c c closest point
	 * @return if collides or not
	 */
	static void computeClosestPointOnLineSegment(const Vector3& p1, const Vector3& q1, const Vector3& p, Vector3& c);

	/**
	 * Does line segments collide
	 * @param p1 p1 line 1 point 1
	 * @param q1 q1 line 1 point 2
	 * @param p2 p2 line 2 point 1
	 * @param q2 q2 line 2 point 2
	 * @param p p intersection point
	 * @return if collides or not
	 */
	static bool doesLineSegmentsCollide(const Vector3& p1, const Vector3& q1, const Vector3& p2, const Vector3& q2, Vector3& p);

	/**
	 * Computes closest points c1, c2 on line segment p1->q1, p2->q2
	 * based on an algorithm from "Real-Time Collision Detection" / Ericson"
	 * Credit:
	 * "From Real-Time Collision Detection by Christer Ericson
	 * published by Morgan Kaufman Publishers, (c) 2005 Elsevier Inc"
	 * @param p1 point p1 on line segment 1
	 * @param q1 point q1 on line segment 1
	 * @param p2 point p2 on line segment 2
	 * @param q2 point q2 on line segment 2
	 * @param c1 closest point on line segment 1 c1
	 * @param c2 closest point on line segment 2 c2
	 */
	static void computeClosestPointsOnLineSegments(const Vector3& p1, const Vector3& q1, const Vector3& p2, const Vector3& q2, Vector3& c1, Vector3& c2);

	/**
	 * Check if segment collides with bounding box
	 * based on an algorithm from "Real-Time Collision Detection" / Ericson
	 * Credit:
	 * "From Real-Time Collision Detection by Christer Ericson
	 * published by Morgan Kaufman Publishers, (c) 2005 Elsevier Inc"
	 * @param boundingBox bounding box
	 * @param p point p on line segment
	 * @param q point q on line segment
	 * @param contactMin contact point min
	 * @param contactMax contact point max
	 * @return true if collides or false if not
	 */
	static bool doesBoundingBoxCollideWithLineSegment(BoundingBox* boundingBox, const Vector3& p, const Vector3& q, Vector3& contactMin, Vector3& contactMax);

	/**
	 * Check if segment collides with oriented bounding box
	 * based on an algorithm from "Real-Time Collision Detection" / Ericson
	 * Credit:
	 * "From Real-Time Collision Detection by Christer Ericson
	 * published by Morgan Kaufman Publishers, (c) 2005 Elsevier Inc"
	 * @param orientedBoundingBox oriented bounding box
	 * @param p point p on line segment
	 * @param q point q on line segment
	 * @param contactMin contact point min
	 * @param contactMax contact point max
	 * @return true if collides or false if not
	 */
	static bool doesOrientedBoundingBoxCollideWithLineSegment(OrientedBoundingBox* orientedBoundingBox, const Vector3& p, const Vector3& q, Vector3& contactMin, Vector3& contactMax);

	/**
	 * Does line segment collides with triangle
	 * @param p1 p1 triangle point 1
	 * @param p2 p2 triangle point 2
	 * @param p3 p3 triangle point 3
	 * @param r1 r1 line segment point 1
	 * @param r2 r2 line segment point 2
	 * @param contact point of intersection
	 * @return line segment collides with triangle
	 */
	static bool doesLineSegmentCollideWithTriangle(const Vector3& p1, const Vector3& p2, const Vector3& p3, const Vector3& r1, const Vector3& r2, Vector3& contact);
};
