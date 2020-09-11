#include <tdme/tests/AudioTest.h>

#include <tdme/os/threading/Thread.h>
#include <tdme/audio/Audio.h>
#include <tdme/audio/AudioEntity.h>
#include <tdme/audio/Sound.h>
#include <tdme/audio/VorbisAudioStream.h>
#include <tdme/utilities/Console.h>

using tdme::tests::AudioTest;
using tdme::audio::Audio;
using tdme::audio::AudioEntity;
using tdme::audio::Sound;
using tdme::audio::VorbisAudioStream;
using tdme::os::threading::Thread;
using tdme::utilities::Console;

void AudioTest::main()
{
	AudioEntity* sound;
	AudioEntity* stream;
	Console::println(string("Starting playing sound"));
	Audio::getInstance()->addEntity(sound = new Sound("sound", "resources/tests/sounds", "water.ogg"));
	sound->setLooping(false);
	sound->setFixed(true);
	sound->play();
	while(sound->isPlaying() == true) { Audio::getInstance()->update(); Thread::sleep(100L); };
	Console::println(string("Finished playing sound"));
	Console::println(string("Starting playing music"));
	Audio::getInstance()->addEntity(stream = new VorbisAudioStream("music", "resources/tests/music", "memento-sanchez_monate_spaeter.ogg"));
	stream->setLooping(false);
	stream->setFixed(true);
	stream->play();
	while(stream->isPlaying() == true) { Audio::getInstance()->update(); Thread::sleep(100L); };
	Console::println(string("Finished playing music"));
	Audio::getInstance()->shutdown();
}
