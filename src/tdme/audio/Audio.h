#pragma once

#if defined(__APPLE__)
	#include <OpenAL/alc.h>
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__linux__) || defined(_WIN32) || defined(__HAIKU__)
	#include <AL/alc.h>
#endif

#include <map>
#include <string>

#include <tdme/tdme.h>
#include <tdme/audio/fwd-tdme.h>
#include <tdme/audio/AudioBufferManager.h>
#include <tdme/math/Vector3.h>

using std::map;
using std::string;

using tdme::audio::AudioBufferManager;
using tdme::audio::AudioEntity;
using tdme::math::Vector3;

/**
 * Interface to audio module
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::audio::Audio final
{
	friend class AudioBufferManager;
	friend class AudioStream;
	friend class Sound;

private:
	static constexpr int32_t ALBUFFERID_NONE { -1 };
	static constexpr int32_t ALSOURCEID_NONE { -1 };
	static Audio* instance;

	ALCdevice* device { nullptr };
	ALCcontext* context { nullptr };

	map<string, AudioEntity*> audioEntities;

	AudioBufferManager audioBufferManager;
	Vector3 listenerPosition;
	Vector3 listenerVelocity;
	Vector3 listenerOrientationAt;
	Vector3 listenerOrientationUp;

	/**
	 * Private constructor
	 */
	Audio();

public:
	/**
	 * @return audio singleton instance
	 */
	static Audio* getInstance();

	/**
	 * @return listener position
	 */
	inline const Vector3& getListenerPosition() const {
		return listenerPosition;
	}

	/**
	 * Set listener position
	 * @param listenerPosition listener position
	 */
	inline void setListenerPosition(const Vector3& listenerPosition) {
		this->listenerPosition = listenerPosition;
	}

	/**
	 * @return listener velocity
	 */
	inline const Vector3& getListenerVelocity() const {
		return listenerVelocity;
	}

	/**
	 * Set listener velocity
	 * @param listenerVelocity listener velocity
	 */
	inline void setListenerVelocity(const Vector3& listenerVelocity) {
		this->listenerVelocity = listenerVelocity;
	}

	/**
	 * @return listener orientation at
	 */
	inline const Vector3& getListenerOrientationAt() const {
		return listenerOrientationAt;
	}

	/**
	 * Set listener orientation at
	 * @param listenerOrientationAt listener orientation at
	 */
	inline void setListenerOrientationAt(const Vector3& listenerOrientationAt) {
		this->listenerOrientationAt = listenerOrientationAt;
	}

	/**
	 * @return listener orientation up
	 */
	inline const Vector3& getListenerOrientationUp() const {
		return listenerOrientationUp;
	}

	/**
	 * Set listener orientation up
	 * @return listener orientation up
	 */
	inline void setListenerOrientationUp(const Vector3& listenerOrientationUp) {
		this->listenerOrientationUp = listenerOrientationUp;
	}

	/**
	 * Returns an audio entity identified by given id
	 * @param id id
	 * @return audio entity
	 */
	AudioEntity* getEntity(const string& id);

	/**
	 * Adds a audio entity
	 * @param entity audio entity
	 */
	void addEntity(AudioEntity* entity);

	/**
	 * Removes an audio entity
	 * @param id id
	 */
	void removeEntity(const string& id);

	/**
	 * Clears all audio entities
	 */
	void reset();

	/**
	 * Shuts the audio down
	 */
	void shutdown();

	/**
	 * Update and transfer audio entity states to open AL
	 */
	void update();

};
