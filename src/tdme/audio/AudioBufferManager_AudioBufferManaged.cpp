#include <tdme/audio/AudioBufferManager_AudioBufferManaged.h>

#include <string>

#include <tdme/audio/AudioBufferManager.h>

using std::string;

using tdme::audio::AudioBufferManager;
using tdme::audio::AudioBufferManager_AudioBufferManaged;

AudioBufferManager_AudioBufferManaged::AudioBufferManager_AudioBufferManaged(AudioBufferManager* audioBufferManager, const string& id, int32_t alId)
{
	this->audioBufferManager = audioBufferManager;
	this->id = id;
	this->alId = alId;
	this->referenceCounter = 0;
}
