#include <tdme/engine/Timing.h>

#include <tdme/utilities/Time.h>

using tdme::engine::Timing;
using tdme::utilities::Time;

Timing::Timing()
{
	frame = 0;
	startTime = Time::getCurrentMillis();
	lastFrameAtTime = UNDEFINED;
	currentFrameAtTime = UNDEFINED;
	currentFPS = 60.0f;
	lastFrameAtTime = Timing::UNDEFINED;
	currentFrameAtTime = Timing::UNDEFINED;
	currentFPS = Timing::UNDEFINED;
}

constexpr int64_t Timing::UNDEFINED;

void Timing::updateTiming()
{
	lastFrameAtTime = currentFrameAtTime;
	currentFrameAtTime = Time::getCurrentMillis();
	currentFPS = 0;
	if (lastFrameAtTime != UNDEFINED) {
		currentFPS = 1000.0f / ((currentFrameAtTime - lastFrameAtTime));
	}
	frame++;
}
