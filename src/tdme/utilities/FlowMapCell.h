#pragma once

#include <tdme/math/Vector3.h>

using tdme::math::Vector3;

/**
 * Flow map cell
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::utilities::FlowMapCell final {
public:
	/**
	 * Default constructor
	 */
	FlowMapCell(): walkable(false) {
	}

	/**
	 * Private constructor
	 * @param position position
	 * @param walkable walkable
	 * @param direction direction
	 */
	FlowMapCell(const Vector3& position, bool walkable, const Vector3& direction): position(position), walkable(walkable), direction(direction) {
	}

	/**
	 * @return cell position
	 */
	inline const Vector3& getPosition() const {
		return position;
	}


	/**
	 * @return if cell is walkable
	 */
	inline bool isWalkable() const {
		return walkable;
	}

	/**
	 * @return cell movement direction
	 */
	inline const Vector3& getDirection() const {
		return direction;
	}

	/**
	 * Set movement direction
	 * @param cell movement direction
	 */
	inline void setDirection(const Vector3& direction) {
		this->direction = direction;
	}

private:
	Vector3 position;
	bool walkable;
	Vector3 direction;
};
