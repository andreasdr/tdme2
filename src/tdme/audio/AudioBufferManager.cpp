#include <tdme/audio/AudioBufferManager.h>

#include <string>
#include <unordered_map>

#include <tdme/audio/Audio.h>
#include <tdme/audio/AudioBufferManager_AudioBufferManaged.h>
#include <tdme/utilities/Console.h>

using std::string;
using std::unordered_map;

using tdme::audio::Audio;
using tdme::audio::AudioBufferManager;
using tdme::audio::AudioBufferManager_AudioBufferManaged;
using tdme::utilities::Console;

AudioBufferManager::AudioBufferManager()
{
}

AudioBufferManager_AudioBufferManaged* AudioBufferManager::addAudioBuffer(const string& id)
{
	// check if we already manage this audio buffer
	auto audioBufferManagedIt = audioBuffers.find(id);
	AudioBufferManager_AudioBufferManaged* audioBufferManaged = audioBufferManagedIt != audioBuffers.end()?audioBufferManagedIt->second:nullptr;
	if (audioBufferManaged != nullptr) {
		audioBufferManaged->incrementReferenceCounter();
		// yep, return buffer
		return audioBufferManaged;
	}
	// not yet, create managed audio buffer with no AL id attached yet
	audioBufferManaged = new AudioBufferManager_AudioBufferManaged(this, id, Audio::ALBUFFERID_NONE);
	audioBufferManaged->incrementReferenceCounter();
	// add it to our audioBuffers
	audioBuffers[audioBufferManaged->getId()] = audioBufferManaged;
	// return audio buffer
	return audioBufferManaged;
}

bool AudioBufferManager::removeAudioBuffer(const string& id)
{
	auto audioBufferManagedIt = audioBuffers.find(id);
	AudioBufferManager_AudioBufferManaged* audioBufferManaged = audioBufferManagedIt != audioBuffers.end()?audioBufferManagedIt->second:nullptr;
	if (audioBufferManaged != nullptr) {
		if (audioBufferManaged->decrementReferenceCounter()) {
			// remove from our list
			audioBuffers.erase(audioBufferManagedIt);
			delete audioBufferManaged;
			// report to called that this audio buffer can be removed
			return true;
		} else {
			return false;
		}
	}
	// should never happen
	Console::println(string("Warning: audio buffer not loaded by audio buffer manager"));
	return false;
}
