#pragma once

#include <tdme/tdme.h>
#include <tdme/engine/primitives/fwd-tdme.h>
#include <tdme/engine/primitives/BoundingVolume.h>
#include <tdme/math/Vector3.h>

using tdme::engine::primitives::BoundingVolume;
using tdme::math::Vector3;

/**
 * Height map physics primitive
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::primitives::HeightMap final
	: public BoundingVolume
{
private:
	int columns;
	int rows;
	float minHeight;
	float maxHeight;
	float* heightValues;

public:
	/**
	 * Public constructor
	 * @param colums columns
	 * @param rows rows
	 * @param minHeight min height
	 * @param maxHeight max height
	 * @param heightValues height values which will not be copied
	 * @param scale scale
	 */
	HeightMap(
		int columns,
		int rows,
		float minHeight,
		float maxHeight,
		float* heightValues,
		const Vector3& scale = Vector3(1.0f, 1.0f, 1.0f)
	);

	// overrides
	void setScale(const Vector3& scale) override;
	BoundingVolume* clone() const override;

};
