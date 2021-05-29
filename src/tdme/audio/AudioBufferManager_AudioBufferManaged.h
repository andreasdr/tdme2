#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/audio/fwd-tdme.h>

using std::string;

using tdme::audio::AudioBufferManager;

/**
 * Managed audio buffer entity
 * @author Andreas Drewke
 */
class tdme::audio::AudioBufferManager_AudioBufferManaged
{
	friend class AudioBufferManager;
	friend class Sound;

private:
	AudioBufferManager* audioBufferManager;
	string id;
	int32_t alId;
	int32_t referenceCounter;

	/**
	 * Protected constructor
	 * @param audioBufferManager audio buffer manager
	 * @param id id
	 * @param alId Open AL id
	 */
	AudioBufferManager_AudioBufferManaged(AudioBufferManager* audioBufferManager, const string& id, int32_t alId);

	/**
	 * @return audio buffer id
	 */
	inline const string& getId() {
		return id;
	}

	/**
	 * @return Open AL id
	 */
	inline int32_t getAlId() {
		return alId;
	}

	/**
	 * Set up Open AL audio buffer id
	 * @param alId al Id
	 */
	inline void setAlId(int32_t alId) {
		this->alId = alId;
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

};
