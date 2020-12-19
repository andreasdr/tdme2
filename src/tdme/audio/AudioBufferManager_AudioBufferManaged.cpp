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

const string& AudioBufferManager_AudioBufferManaged::getId()
{
	return id;
}

int32_t AudioBufferManager_AudioBufferManaged::getAlId()
{
	return alId;
}

void AudioBufferManager_AudioBufferManaged::setAlId(int32_t alId)
{
	this->alId = alId;
}

int32_t AudioBufferManager_AudioBufferManaged::getReferenceCounter()
{
	return referenceCounter;
}

bool AudioBufferManager_AudioBufferManaged::decrementReferenceCounter()
{
	referenceCounter--;
	return referenceCounter == 0;
}

void AudioBufferManager_AudioBufferManaged::incrementReferenceCounter()
{
	referenceCounter++;
}

