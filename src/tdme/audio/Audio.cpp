#include <tdme/audio/Audio.h>

#if defined(__APPLE__)
	#include <OpenAL/al.h>
	#include <OpenAL/alc.h>
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__linux__) || defined(_WIN32) || defined(_WIN64) || defined(__HAIKU__)
	#include <AL/al.h>
	#include <AL/alc.h>
#endif

#include <array>
#include <map>
#include <string>
#include <vector>

#include <tdme/audio/AudioEntity.h>
#include <tdme/math/Vector3.h>
#include <tdme/utilities/Console.h>

using std::array;
using std::map;
using std::vector;
using std::string;

using tdme::audio::Audio;
using tdme::audio::AudioEntity;
using tdme::math::Vector3;
using tdme::utilities::Console;

Audio::Audio()
{
	// TODO: error handling
	device = alcOpenDevice(NULL);
	context = alcCreateContext(device, NULL);
	alcMakeContextCurrent(context);

	//
	listenerPosition.set(0.0f, 0.0f, 0.0);
	listenerVelocity.set(0.0f, 0.0f, 0.0);
	listenerOrientationAt.set(0.0f, 0.0f, 1.0f);
	listenerOrientationUp.set(0.0f, 1.0f, 0.0f);

	// init listener position
	update();
}

constexpr int32_t Audio::ALBUFFERID_NONE;

constexpr int32_t Audio::ALSOURCEID_NONE;

Audio* Audio::instance = nullptr;

Audio* Audio::getInstance()
{
	if (Audio::instance == nullptr) {
		Audio::instance = new Audio();
	}
	return Audio::instance;
}

AudioEntity* Audio::getEntity(const string& id)
{
	auto audioEntityIt = audioEntities.find(id);
	if (audioEntityIt == audioEntities.end()) return nullptr;
	return audioEntityIt->second;
}

void Audio::addEntity(AudioEntity* entity)
{
	if (entity->initialize() == true) {
		auto audioEntityIt = audioEntities.find(entity->getId());
		if (audioEntityIt != audioEntities.end()) {
			// check if we want to add this entity a second time
			if (entity == audioEntityIt->second) {
				Console::println("Audio::addEntity(): " + entity->getId() + ": entity already added!");
				return;
			}
			// remove old other entity
			removeEntity(entity->getId());
		}
		audioEntities[entity->getId()] = entity;
	} else {
		Console::println(string("Audio::addEntity(): adding '" + entity->getId() + "' failed"));
	}
}

void Audio::removeEntity(const string& id)
{
	auto audioEntityIt = audioEntities.find(id);
	if (audioEntityIt != audioEntities.end()) {
		auto audioEntity = audioEntityIt->second;
		audioEntity->stop();
		audioEntity->dispose();
		audioEntities.erase(audioEntityIt);
		delete audioEntity;
	}
}

void Audio::reset()
{
	// determine keys to remove
	vector<string> keys;
	for (auto it = audioEntities.begin(); it != audioEntities.end(); ++it) {
		keys.push_back(it->first);
	}

	// remove entities
	for (auto& key: keys) {
		removeEntity(key);
	}
}

void Audio::shutdown()
{
	reset();
	alcCloseDevice(device);
}

void Audio::update()
{
	// update audio entities
	for (auto it = audioEntities.begin(); it != audioEntities.end(); ++it) {
		it->second->update();
	}

	// update listener position
	alListenerfv(AL_POSITION, listenerPosition.getArray().data());
	alListenerfv(AL_VELOCITY, listenerVelocity.getArray().data());
	auto& listenerOrientationAtArray = listenerOrientationAt.getArray();
	auto& listenerOrientationUpArray = listenerOrientationUp.getArray();
	array<float, 6> listenerOrientation = {
		listenerOrientationAtArray[0],
		listenerOrientationAtArray[1],
		listenerOrientationAtArray[2],
		listenerOrientationUpArray[0],
		listenerOrientationUpArray[1],
		listenerOrientationUpArray[2]
	};
	alListenerfv(AL_ORIENTATION, listenerOrientation.data());
}
