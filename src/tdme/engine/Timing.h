
#pragma once

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>

/**
 * Timing class
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::Timing final
{
	friend class Engine;

public:
	static constexpr int64_t UNDEFINED { -1LL };

private:
	int64_t frame;
	int64_t startTime;
	int64_t lastFrameAtTime;
	int64_t currentFrameAtTime;
	float currentFPS;

	/**
	 * Updates timing
	 */
	void updateTiming();

	/**
	 * Public constructor
	 */
	Timing();

public:
	/**
	 * @return frames that have been rendered
	 */
	inline int64_t getFrame() {
		return frame;
	}

	/**
	 * @return start time
	 */
	inline int64_t getStartTime() {
		return startTime;
	}

	/**
	 * @return total time the engine is running
	 */
	inline int64_t getTotalTime() {
		return currentFrameAtTime - startTime;
	}

	/**
	 * @return time last frame has been rendered in ms
	 */
	inline int64_t getLastFrameAtTime() {
		return lastFrameAtTime;
	}

	/**
	 * @return time current frame has been rendered in ms
	 */
	inline int64_t getCurrentFrameAtTime() {
		return currentFrameAtTime;
	}

	/**
	 * Gets the time passed between last and current frame
	 * @return delta time
	 */
	inline int64_t getDeltaTime() {
		if (currentFrameAtTime == UNDEFINED || lastFrameAtTime == UNDEFINED) {
			return 0LL;
		}
		return currentFrameAtTime - lastFrameAtTime;
	}

	/**
	 * @return current fps
	 */
	inline float getCurrentFPS() {
		return currentFPS;
	}

};
