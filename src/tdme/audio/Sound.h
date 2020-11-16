#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/audio/fwd-tdme.h>
#include <tdme/audio/AudioEntity.h>

using std::string;

using tdme::audio::AudioEntity;

/**
 * Sound audio entity implementation
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::audio::Sound final
	: public AudioEntity
{
	friend class Audio;

private:
	bool initiated;
	string pathName;
	string fileName;
	string bufferId;
	uint32_t alBufferId;
	uint32_t alSourceId;

public:
	/**
	 * Protected constructor
	 * @param id id
	 * @param pathName path name
	 * @param fileName file name
	 */
	Sound(const string& id, const string& pathName, const string& fileName);

	// override methods
	bool isPlaying() override;
	void rewind() override;
	void play() override;
	void pause() override;
	void stop() override;

protected:
	// overridden methods
	bool initialize() override;
	void update() override;
	void dispose() override;
};
