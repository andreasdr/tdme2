#pragma once

#include <vector>
#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/subsystems/manager/fwd-tdme.h>

using std::string;
using std::vector;

using tdme::engine::subsystems::manager::VBOManager;

/**
 * Managed VBO entity
 * @author Andreas Drewke
 */
class tdme::engine::subsystems::manager::VBOManager_VBOManaged
{
	friend class VBOManager;

private:
	string id;
	vector<int32_t> vboIds;
	int32_t referenceCounter { 0 };
	volatile bool uploaded { false };

	/**
	 * Private constructor
	 * @param id id
	 * @param vboIds VBO ids
	 */
	VBOManager_VBOManaged(const string& id, vector<int32_t>& vboIds);

public:

	/**
	 * @return vbo id
	 */
	inline const string& getId() {
		return id;
	}

	/**
	 * @return vbo gl ids
	 */
	inline vector<int32_t>* getVBOIds() {
		return &vboIds;
	}

	/**
	 * @return reference counter
	 */
	inline int32_t getReferenceCounter() {
		return referenceCounter;
	}

private:
	/**
	 * decrement reference counter
	 * @return if reference counter = 0
	 */
	inline bool decrementReferenceCounter() {
		referenceCounter--;
		return referenceCounter == 0;
	}

	/**
	 * increment reference counter
	 */
	inline void incrementReferenceCounter() {
		referenceCounter++;
	}

public:

	/**
	 * Set uploaded
	 * @param uploaded uploaded
	 */
	inline void setUploaded(bool uploaded) {
		this->uploaded = uploaded;
	}

	/**
	 * @return if vbo's have been uploaded
	 */
	inline bool isUploaded() {
		return uploaded;
	}

};
