#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/model/fwd-tdme.h>

using std::string;

using tdme::engine::model::Model;

/**
 * Animation setup
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::model::AnimationSetup final
{

private:
	Model* model { nullptr };
	string id;
	int32_t startFrame;
	int32_t endFrame;
	int32_t frames;
	bool loop;
	string overlayFromNodeId;
	float speed;

public:

	/**
	 * Public constructor
	 * @param model model
	 * @param id id
	 * @param startFrame start frame
	 * @param endFrame end frame
	 * @param loop loop
	 * @param overlayFromNodeId overlay from node id
	 * @param speed speed whereas 1.0 is default speed
	 */
	AnimationSetup(Model* model, const string& id, int32_t startFrame, int32_t endFrame, bool loop, const string& overlayFromNodeId, float speed = 1.0);

	/**
	 * @return model this animation belongs to
	 */
	inline Model* getModel() {
		return model;
	}

	/**
	 * @return id
	 */
	inline const string& getId() {
		return id;
	}

	/**
	 * @return if animation set up is a overlay animation setup
	 */
	inline bool isOverlayAnimationSetup() {
		return overlayFromNodeId.empty() == false;
	}

	/**
	 * @return start frame
	 */
	inline int32_t getStartFrame() {
		return startFrame;
	}

	/**
	 * Set start frame
	 * @param startFrame start frame
	 */
	void setStartFrame(int32_t startFrame);

	/**
	 * @return end frame
	 */
	inline int32_t getEndFrame() {
		return endFrame;
	}

	/**
	 * Set end frame
	 * @param endFrame end frame
	 */
	void setEndFrame(int32_t endFrame);

	/**
	 * @return frames
	 */
	inline int32_t getFrames() {
		return frames;
	}

	/**
	 * @return looping enabled
	 */
	inline bool isLoop() {
		return loop;
	}

	/**
	 * Set loop
	 * @param loop loop
	 */
	inline void setLoop(bool loop) {
		this->loop = loop;
	}

	/**
	 * If this is a overlay animation this returns a node id from which node the animation will start in the hierarchy
	 * @return node id from which the animation will start in the hierarchy
	 */
	inline const string& getOverlayFromNodeId() {
		return overlayFromNodeId;
	}

	/**
	 * Set overlay from node id
	 * @param overlayFromNodeId overlay from node id
	 */
	inline void setOverlayFromNodeId(const string& overlayFromNodeId) {
		this->overlayFromNodeId = overlayFromNodeId;
	}

	/**
	 * @return animation duration in milliseconds
	 */
	int64_t computeDuration();

	/**
	 * Compute animation duration
	 * @param startFrame start frame
	 * @param endFrame end frame
	 * @return animation duration in milliseconds
	 */
	int64_t computeDuration(int32_t startFrame, int32_t endFrame);

	/**
	 * @return speed whereas 1.0 is default speed
	 */
	inline float getSpeed() {
		return speed;
	}

	/**
	 * Set up animation speed
	 * @param speed speed whereas 1.0 is default speed
	 */
	inline void setSpeed(float speed) {
		this->speed = speed;
	}

};
