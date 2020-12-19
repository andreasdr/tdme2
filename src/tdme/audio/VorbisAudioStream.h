#pragma once

#include <array>
#include <string>

#include <tdme/tdme.h>
#include <tdme/audio/decoder/VorbisDecoder.h>
#include <tdme/audio/fwd-tdme.h>
#include <tdme/audio/AudioStream.h>

using std::array;
using std::string;

using tdme::audio::decoder::VorbisDecoder;
using tdme::audio::AudioStream;
using tdme::utilities::ByteBuffer;

/**
 * Vorbis audio stream
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::audio::VorbisAudioStream final
	: public AudioStream
{
	friend class Audio;

private:
	string pathName;
	string fileName;
	VorbisDecoder decoder;
	bool initiated { false };

public:
	/**
	 * Protected constructor
	 * @param id id
	 * @param pathName path name
	 * @param fileName file name
	 */
	VorbisAudioStream(const string& id, const string& pathName, const string& fileName);

	// overridden methods
	void rewind() override;

protected:
	// overridden methods
	bool initialize() override;
	void dispose() override;

	/**
	 * Destructor
	 */
	virtual ~VorbisAudioStream();

	// overridden methods
	void fillBuffer(ByteBuffer* data) override;
};
