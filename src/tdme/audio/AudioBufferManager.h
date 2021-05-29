#pragma once

#include <string>
#include <unordered_map>

#include <tdme/tdme.h>
#include <tdme/audio/fwd-tdme.h>

using std::string;
using std::unordered_map;

using tdme::audio::AudioBufferManager_AudioBufferManaged;

/**
 * Audio buffer manager
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::audio::AudioBufferManager final
{
	friend class AudioBufferManager_AudioBufferManaged;
	friend class Audio;
	friend class Sound;

private:
	unordered_map<string, AudioBufferManager_AudioBufferManaged*> audioBuffers;

	/**
	 * Private constructor
	 */
	AudioBufferManager();

	/**
	 * Adds a audio buffer to manager / open al stack
	 * @param id id
	 * @return audio buffer managed
	 */
	AudioBufferManager_AudioBufferManaged* addAudioBuffer(const string& id);

	/**
	 * Removes a texture from manager / open gl stack
	 * @param id id
	 * @return true if caller has to remove the audio buffer from open AL
	 */
	bool removeAudioBuffer(const string& id);

};
