#pragma once

#include <tdme/tdme.h>
#include <tdme/engine/model/fwd-tdme.h>

/**
 * Joint/bone weight
 * @author andreas.drewke
 * @version $Id$
 */
class tdme::engine::model::JointWeight final
{
private:
	int32_t jointIndex;
	int32_t weightIndex;
public:
	/**
	 * Public constructor
	 */
	JointWeight();

	/**
	 * Public constructor
	 * @param jointIndex joint index
	 * @param weightIndex weight index
	 */
	JointWeight(int32_t jointIndex, int32_t weightIndex);

	/**
	 * @returns joint index
	 */
	inline int32_t getJointIndex() const {
		return jointIndex;
	}

	/**
	 * @returns weight index
	 */
	inline int32_t getWeightIndex() const {
		return weightIndex;
	}

};
