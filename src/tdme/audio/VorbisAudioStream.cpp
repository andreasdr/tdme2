#include <tdme/audio/VorbisAudioStream.h>

#include <string>

#include <tdme/audio/decoder/AudioDecoderException.h>
#include <tdme/audio/decoder/VorbisDecoder.h>
#include <tdme/os/filesystem/FileSystemException.h>
#include <tdme/utilities/ByteBuffer.h>
#include <tdme/utilities/Console.h>

using std::string;
using std::to_string;

using tdme::audio::decoder::AudioDecoderException;
using tdme::audio::decoder::VorbisDecoder;
using tdme::audio::VorbisAudioStream;
using tdme::os::filesystem::FileSystemException;
using tdme::utilities::ByteBuffer;
using tdme::utilities::Console;

VorbisAudioStream::VorbisAudioStream(const string& id, const string& pathName, const string& fileName) : AudioStream(id)
{
	this->pathName = pathName;
	this->fileName = fileName;
	this->initiated = false;
}

VorbisAudioStream::~VorbisAudioStream() {
}

void VorbisAudioStream::rewind()
{
	if (initiated == false) return;

	try {
		decoder.reset();
	} catch (FileSystemException &fse) {
		Console::println(string("VorbisAudioStream::rewind(): '"+ (id) + "': " + fse.what()));
	} catch (AudioDecoderException &ade) {
		Console::println(string("VorbisAudioStream::rewind(): '" + (id) + "': " + ade.what()));
	}
}

bool VorbisAudioStream::initialize()
{
	if (initiated == true) return true;

	uint32_t sampleRate = 0;
	uint8_t channels = 0;

	// decode audio stream
	try {
		// decode ogg vorbis
		decoder.openFile(pathName, fileName);
		Console::println(
			string(
				"VorbisAudioStream::initialize(): '" +
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
		sampleRate = decoder.getSampleRate();
		channels = decoder.getChannels();
	} catch (FileSystemException& fse) {
		Console::println(string("VorbisAudioStream::initialize(): '" + (id) + "': " + fse.what()));
		decoder.close();
		dispose();
		return false;
	} catch (AudioDecoderException& ade) {
		Console::println(string("VorbisAudioStream::initialize(): '" + (id) + "': " + ade.what()));
		decoder.close();
		dispose();
		return false;
	}

	// set stream parameters
	setParameters(sampleRate, channels, 32768);

	// check if underlying audio stream did initialize
	if (AudioStream::initialize() == false) {
		// nope, dispose and such
		decoder.close();
		dispose();
		return false;
	}

	// success
	initiated = true;
	return true;
}

void VorbisAudioStream::fillBuffer(ByteBuffer* data) {
	auto bytesDecoded = 0;
	try {
		bytesDecoded = decoder.readFromStream(data);
		if (looping == true && bytesDecoded < data->getCapacity()) {
			decoder.reset();
		}
	} catch (FileSystemException& fse) {
		Console::println(string("Audio stream: '" + (id) + "': " + fse.what()));
	} catch (AudioDecoderException& ade) {
		Console::println(string("Audio stream: '" + (id) + "': " + ade.what()));
	}
}

void VorbisAudioStream::dispose()
{
	if (initiated == false) return;

	// close decoder
	decoder.close();

	// dispose audio stream
	AudioStream::dispose();

	//
	initiated = false;
}
