#pragma once

#include <array>
#include <string>

#include <tdme/tdme.h>
#include <tdme/utilities/fwd-tdme.h>
#include <tdme/audio/fwd-tdme.h>
#include <tdme/audio/AudioEntity.h>

using std::array;
using std::string;

using tdme::audio::AudioEntity;
using tdme::utilities::ByteBuffer;

/** 
 * Audio stream
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::audio::AudioStream
	: public AudioEntity
{
	friend class Audio;

private:
	bool initiated { false };
	array<uint32_t, 2> alBufferIds;
	uint32_t alSourceId;
	uint32_t sampleRate;
	uint8_t channels;
	ByteBuffer* data { nullptr };
	int32_t format;
	bool playing { false };

public:
	/**
	 * Set audio initialization parameters
	 * @param sampleRate sample rate
	 * @param channels channels
	 * @param bufferSize buffer size
	 */
	virtual void setParameters(uint32_t sampleRate, uint8_t channels, const uint32_t bufferSize = 32768);

	// overriden methods
	virtual bool isPlaying() override;
	virtual void rewind() override;
	virtual void play() override;
	virtual void pause() override;
	virtual void stop() override;

private:
	/**
	 * @return having buffers that are currently played
	 */
	bool isPlayingBuffers();

	/** 
	 * Updates properties to Open AL
	 */
	void updateProperties();

protected:
	/**
	 * Protected constructor
	 * @param id id
	 */
	AudioStream(const string& id);

	// overriden methods
	virtual bool initialize() override;
	virtual void update() override;
	virtual void dispose() override;

	/**
	 * Fill buffer
	 * @param data data
	 */
	virtual void fillBuffer(ByteBuffer* data) = 0;

	/**
	 * Destructor
	 */
	virtual ~AudioStream();
};
