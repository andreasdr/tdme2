#pragma once

#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/prototype/fwd-tdme.h>

using std::vector;

using tdme::engine::prototype::Prototype;

/**
 * Prototype terrain definition
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::prototype::PrototypeTerrain final
{
private:
	float width { -1 };
	float depth { -1 };
	vector<float> terrainHeightVector;

public:

	/**
	 * Public constructor
	 */
	inline PrototypeTerrain() {
	}

	/**
	 * Destructor
	 */
	~PrototypeTerrain() {
	}

	/**
	 * @return width
	 */
	inline float getWidth() const {
		return width;
	}

	/**
	 * Set width
	 * @param width width
	 */
	inline void setWidth(float width) {
		this->width = width;
	}

	/**
	 * @return depth
	 */
	inline float getDepth() const {
		return depth;
	}

	/**
	 * Set depth
	 * @param depth depth
	 */
	inline void setDepth(float depth) {
		this->depth = depth;
	}

	/**
	 * @return terrain height vector
	 */
	inline vector<float>& getHeightVector() {
		return terrainHeightVector;
	}

};
