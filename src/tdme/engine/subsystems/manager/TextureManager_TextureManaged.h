#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/subsystems/manager/fwd-tdme.h>

using tdme::engine::subsystems::manager::TextureManager;

using std::string;

/**
 * Managed texture entity
 * @author Andreas Drewke
 */
class tdme::engine::subsystems::manager::TextureManager_TextureManaged
{
	friend class TextureManager;

private:
	string id;
	int32_t rendererId;
	int32_t referenceCounter { 0 };
	volatile bool uploaded { false };
private:

	/**
	 * Protected constructor
	 * @param id id
	 * @param rendererId renderer id
	 */
	TextureManager_TextureManaged(const string& id, int32_t rendererId);

	/**
	 * @return texture id
	 */
	inline const string& getId() {
		return id;
	}

	/**
	 * @return reference counter
	 */
	inline int32_t getReferenceCounter() {
		return referenceCounter;
	}

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
	 * @return texture renderer id
	 */
	inline int32_t getRendererId() {
		return rendererId;
	}

	/**
	 * Set uploaded
	 * @param uploaded uploaded
	 */
	inline void setUploaded(bool uploaded) {
		this->uploaded = uploaded;
	}

	/**
	 * @return if texture has been uploaded
	 */
	inline bool isUploaded() {
		return uploaded;
	}

};
