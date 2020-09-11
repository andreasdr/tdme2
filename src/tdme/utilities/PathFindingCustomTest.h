#pragma once

#include <tdme/engine/primitives/fwd-tdme.h>
#include <tdme/utilities/fwd-tdme.h>

using tdme::engine::primitives::BoundingVolume;

/**
 * Path finding custom test interface
 * @author Andreas Drewke
 * @version $Id$
 */
struct tdme::utilities::PathFindingCustomTest
{

	/**
	 * Initialize custom test for a sequence of isWalkable calls for the current path finding execution
	 */
	virtual void initialize() = 0;

	/**
	 * Disposes custom test after a sequence of isWalkable calls for the last path finding execution
	 */
	virtual void dispose() = 0;

	/**
	 * Is walkable user test
	 * @param pathFinding current path finding instance
	 * @param x x
	 * @param y y
	 * @param z z
	 * @return is walkable
	 */
	virtual bool isWalkable(PathFinding* pathFinding, float x, float y, float z) = 0;

	/**
	 * Destructor
	 */
	virtual ~PathFindingCustomTest() {}

};
