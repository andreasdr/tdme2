#include <tdme/engine/model/AnimationSetup.h>

#include <tdme/engine/model/Model.h>

using tdme::engine::model::AnimationSetup;
using tdme::engine::model::Model;

AnimationSetup::AnimationSetup(Model* model, const string& id, int32_t startFrame, int32_t endFrame, bool loop, const string& overlayFromGroupId, float speed)
{
	this->model = model;
	this->id = id;
	this->startFrame = startFrame;
	this->endFrame = endFrame;
	this->frames = (endFrame - startFrame) + 1;
	this->loop = loop;
	this->overlayFromGroupId = overlayFromGroupId;
	this->speed = speed;
}

void AnimationSetup::setStartFrame(int32_t startFrame) {
	this->startFrame = startFrame;
	this->frames = (endFrame - startFrame) + 1;
	if (this->frames < 0) this->frames = 0;
}

void AnimationSetup::setEndFrame(int32_t endFrame) {
	this->endFrame = endFrame;
	this->frames = (endFrame - startFrame) + 1;
	if (this->frames < 0) this->frames = 0;
}

int64_t AnimationSetup::computeDuration()
{
	return computeDuration(startFrame, endFrame);
}

int64_t AnimationSetup::computeDuration(int32_t startFrame, int32_t endFrame)
{
	return static_cast<int64_t>(
		static_cast<float>(endFrame - startFrame + 1) / model->getFPS() * 1000.0f
	);
}
