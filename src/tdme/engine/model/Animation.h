#pragma once

#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/math/Matrix4x4.h>

using std::vector;

using tdme::math::Matrix4x4;

/**
 * Animation entity
 * @author andreas.drewke
 * @version $Id$
 */
class tdme::engine::model::Animation final
{
private:
	vector<Matrix4x4> transformationsMatrices;

public:
	/**
	 * Public constructor
	 * @param frames frames
	 */
	Animation();

	/**
	 * @return number of frames
	 */
	inline int getFrames() const {
		return transformationsMatrices.size();
	}

	/**
	 * Returns transformation matrices
	 * @return transformation matrices
	 */
	inline const vector<Matrix4x4>& getTransformationsMatrices() const {
		return transformationsMatrices;
	}

	/**
	 * Set transformation matrices
	 * @return transformationMatrices transformation matrices
	 */
	inline void setTransformationsMatrices(const vector<Matrix4x4>& transformationsMatrices) {
		this->transformationsMatrices = transformationsMatrices;
	}

};
