#include <tdme/audio/Sound.h>

#if defined(__APPLE__)
	#include <OpenAL/al.h>
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__linux__) || defined(_WIN32) || defined(__HAIKU__)
	#include <AL/al.h>
#endif

#include <string>

#include <tdme/audio/decoder/fwd-tdme.h>
#include <tdme/audio/decoder/AudioDecoderException.h>
#include <tdme/audio/decoder/VorbisDecoder.h>
#include <tdme/audio/Audio.h>
#include <tdme/audio/AudioBufferManager.h>
#include <tdme/audio/AudioBufferManager_AudioBufferManaged.h>
#include <tdme/math/Vector3.h>
#include <tdme/os/filesystem/fwd-tdme.h>
#include <tdme/os/filesystem/FileSystemException.h>
#include <tdme/utilities/ByteBuffer.h>
#include <tdme/utilities/Console.h>

using std::string;
using std::string;
using std::to_string;

using tdme::audio::decoder::AudioDecoderException;
using tdme::audio::decoder::VorbisDecoder;
using tdme::audio::Audio;
using tdme::audio::AudioBufferManager;
using tdme::audio::AudioBufferManager_AudioBufferManaged;
using tdme::audio::Sound;
using tdme::math::Vector3;
using tdme::os::filesystem::FileSystemException;
using tdme::utilities::ByteBuffer;
using tdme::utilities::Console;

Sound::Sound(const string& id, const string& pathName, const string& fileName) : AudioEntity(id)
{
	initiated = false;
	this->bufferId = pathName + "/" + fileName;
	this->pathName = pathName;
	this->fileName = fileName;
	alBufferId = Audio::ALBUFFERID_NONE;
	alSourceId = Audio::ALSOURCEID_NONE;
}

bool Sound::isPlaying()
{
	ALint state;
	alGetSourcei(alSourceId, AL_SOURCE_STATE, &state);
	return (state == AL_PLAYING);
}

void Sound::rewind()
{
	if (initiated == false)
		return;

	alSourceRewind(alSourceId);
	if (alGetError() != AL_NO_ERROR) {
		Console::println(string("Audio sound: '" + id + "': Could not rewind"));
	}
}

void Sound::play()
{
	if (initiated == false)
		return;
	// update
	update();
	// play
	alSourcePlay(alSourceId);
	if (alGetError() != AL_NO_ERROR) {
		Console::println(string("Audio sound: '" + id + "': Could not play"));
	}
}

void Sound::pause()
{
	if (initiated == false)
		return;

	alSourcePause(alSourceId);
	if (alGetError() != AL_NO_ERROR) {
		Console::println(string("Audio sound: '" + id+ "': Could not pause"));
	}
}

void Sound::stop()
{
	if (initiated == false)
		return;

	alSourceStop(alSourceId);
	if (alGetError() != AL_NO_ERROR) {
		Console::println(string("Audio sound: '" + id + "': Could not stop"));
	}
}

bool Sound::initialize()
{
	// check if we already have this buffer
	auto audioBufferManaged = Audio::instance->audioBufferManager.addAudioBuffer(bufferId);
	if (audioBufferManaged->alId == Audio::ALBUFFERID_NONE) {
		// nope, generate al buffer
		int alError;
		alGenBuffers(1, &alBufferId);
		if (alGetError() != AL_NO_ERROR) {
			Console::println(string("Audio sound: '" + id + "': Could not generate buffer"));
			return false;
		}
		// set up al id in audio buffer managed
		audioBufferManaged->setAlId(alBufferId);
		auto format = -1;
		auto frequency = -1;

		// decode audio sound
		VorbisDecoder decoder;
		ByteBuffer* data = nullptr;
		try {
			// decode ogg vorbis
			decoder.openFile(pathName, fileName);
			Console::println(
				string(
					"Audio sound: '" +
					id +
					"' with " +
					to_string(decoder.getBitsPerSample()) +
					" bits per sample, " +
					to_string(decoder.getChannels()) +
					" channels, " +
					to_string(decoder.getSampleRate()) +
					" samplerate"
				)
			);
			frequency = decoder.getSampleRate();
			switch (decoder.getChannels()) {
				case(1): format = AL_FORMAT_MONO16; break;
				case(2): format = AL_FORMAT_STEREO16; break;
				default:
					Console::println(string("Audio sound: '" + id + "': Unsupported number of channels"));
			}
			data = ByteBuffer::allocate(2 * 2 * decoder.getSamples());
			if (decoder.readFromStream(data) == 0) throw new AudioDecoderException("no audio data was decoded");
			Console::println(
				string(
					"Audio sound: '" +
					id +
					"' with length " +
					to_string((float)data->getPosition() / 2.0f / (float)decoder.getChannels() / (float)decoder.getSampleRate()) +
					" seconds" +
					"(" +
					to_string(data->getPosition()) +
					" bytes)"
				)
			);
		} catch (FileSystemException& fse) {
			Console::println(string("Audio sound: '" + (id) + "': " + fse.what()));
			decoder.close();
			dispose();
			return false;
		} catch (AudioDecoderException& ade) {
			Console::println(string("Audio sound: '" + (id) + "': " + ade.what()));
			decoder.close();
			dispose();
			return false;
		}
		decoder.close();
		// check for valid format and frequency
		if (format == -1 || frequency == -1) {
			Console::println(string("Audio sound: '" + id + "': Format or frequency invalid"));
			dispose();
			return false;
		}
		// upload to al
		alBufferData(alBufferId, format, data->getBuffer(), data->getPosition(), frequency);
		if (alGetError() != AL_NO_ERROR) {
			Console::println(string("Audio sound: '" + id + "': Could not upload buffer data"));
			dispose();
			return false;
		}
		delete data;
	} else {
		alBufferId = audioBufferManaged->alId;
	}

	// create source
	alGenSources(1, &alSourceId);
	if (alGetError() != AL_NO_ERROR) {
		Console::println(string("Audio sound: '" + id + "': Could not generate source"));
		dispose();
		return false;
	}

	// initiate sound properties
	alSourcei(alSourceId, AL_BUFFER, alBufferId);
	update();

	//
	initiated = true;
	return true;
}

void Sound::update()
{
	// update sound properties
	alSourcef(alSourceId, AL_PITCH, pitch);
	alSourcef(alSourceId, AL_GAIN, gain);
	alSourcefv(alSourceId, AL_POSITION, sourcePosition.getArray().data());
	alSourcefv(alSourceId, AL_DIRECTION, sourceDirection.getArray().data());
	alSourcefv(alSourceId, AL_VELOCITY, sourceVelocity.getArray().data());
	alSourcei(alSourceId, AL_LOOPING, looping ? AL_TRUE : AL_FALSE);
	if (fixed == true) {
		alSourcef(alSourceId, AL_ROLLOFF_FACTOR, 0.0f);
		alSourcei(alSourceId, AL_SOURCE_RELATIVE, AL_TRUE);
	} else {
		alSourcef(alSourceId, AL_ROLLOFF_FACTOR, 1.0f);
		alSourcei(alSourceId, AL_SOURCE_RELATIVE, AL_FALSE);
	}
}

void Sound::dispose()
{
	if (alSourceId != Audio::ALSOURCEID_NONE) {
		alDeleteSources(1, &alSourceId);
		if (alGetError() != AL_NO_ERROR) {
			Console::println(string("Audio sound: '" + id + "': Could not delete source"));
		}
		alSourceId = Audio::ALSOURCEID_NONE;
	}
	if (alBufferId != Audio::ALBUFFERID_NONE && Audio::instance->audioBufferManager.removeAudioBuffer(bufferId) == true) {
		alDeleteBuffers(1, &alBufferId);
		if (alGetError() != AL_NO_ERROR) {
			Console::println(string("Audio sound: '" + id + "': Could not delete buffers"));
		}
		alBufferId = Audio::ALBUFFERID_NONE;
	}
	initiated = false;
}
