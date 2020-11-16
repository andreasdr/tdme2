#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/audio/fwd-tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Vector3.h>

using std::string;

using tdme::math::Vector3;

/**
 * Audio entity base class
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::audio::AudioEntity
{
	friend class Audio;

protected:
	string id;
	bool looping;
	bool fixed;
	float pitch;
	float gain;
	Vector3 sourcePosition;
	Vector3 sourceDirection;
	Vector3 sourceVelocity;

protected:
	/**
	 * Constructor
	 */
	AudioEntity(const string& id);

	/**
	 * Constructor
	 */
	virtual ~AudioEntity();

	/**
	 * Initiates this OpenAL entity to OpenAl
	 */
	virtual bool initialize() = 0;

	/**
	 * Commits properties to OpenAl
	 */
	virtual void update() = 0;

	/**
	 * Dispose this entity from OpenAL
	 */
	virtual void dispose() = 0;

public:

	/**
	 * @return id
	 */
	inline virtual const string& getId() const {
		return id;
	}

	/**
	 * @return if sound will be looped
	 */
	inline virtual const bool isLooping() const {
		return looping;
	}

	/**
	 * Set looping
	 * @param looping if sound will be looped
	 */
	inline virtual void setLooping(bool looping) {
		this->looping = looping;
	}

	/**
	 * @return fixed, means the sound will always played no matter where the position and listener is located
	 */
	inline virtual const bool isFixed() const {
		return fixed;
	}

	/**
	 * Set this entity fixed, means the sound will always played no matter where the position and listener is located
	 * @param fixed fixed
	 */
	inline virtual void setFixed(bool fixed) {
		this->fixed = fixed;
	}

	/**
	 * @return pitch
	 */
	inline virtual const float getPitch() const {
		return pitch;
	}

	/**
	 * Set up pitch
	 * @param pitch pitch
	 */
	inline virtual void setPitch(float pitch) {
		this->pitch = pitch;
	}

	/**
	 * @return gain
	 */
	inline virtual const float getGain() const {
		return gain;
	}

	/**
	 * Set up gain
	 * @param gain gain
	 */
	inline virtual void setGain(float gain) {
		this->gain = gain;
	}

	/**
	 * @return source position
	 */
	inline virtual const Vector3& getSourcePosition() const {
		return sourcePosition;
	}

	/**
	 * Set source position
	 * @return source position
	 */
	inline virtual void setSourcePosition(const Vector3& sourcePosition) {
		this->sourcePosition = sourcePosition;
	}

	/**
	 * @return source direction
	 */
	inline virtual const Vector3& getSourceDirection() const {
		return sourceDirection;
	}

	/**
	 * Set source direction
	 * @param sourceDirection source direction
	 */
	inline virtual void setSourceDirection(const Vector3& sourceDirection) {
		this->sourceDirection = sourceDirection;
	}

	/**
	 * @return source velocity
	 */
	inline virtual const Vector3& getSourceVelocity() const {
		return sourceVelocity;
	}

	/**
	 * Set source velocity
	 * @param sourceVelocity source velocity
	 */
	inline virtual void getSourceVelocity(const Vector3& sourceVelocity) {
		this->sourceVelocity = sourceVelocity;
	}

	/**
	 * @return if stream is playing
	 */
	virtual bool isPlaying() = 0;

	/**
	 * Rewinds this audio entity
	 */
	virtual void rewind() = 0;

	/**
	 * Plays this audio entity
	 */
	virtual void play() = 0;

	/**
	 * Pauses this audio entity
	 */
	virtual void pause() = 0;

	/**
	 * Stops this audio entity
	 */
	virtual void stop() = 0;

};
