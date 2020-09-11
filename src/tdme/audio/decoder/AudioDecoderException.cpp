#include <tdme/audio/decoder/AudioDecoderException.h>

#include <tdme/utilities/ExceptionBase.h>

using tdme::audio::decoder::AudioDecoderException;

using tdme::utilities::ExceptionBase;

AudioDecoderException::AudioDecoderException(const string& message) throw() : ExceptionBase(message) {
}
