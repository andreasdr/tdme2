#include <tdme/gui/effects/GUIEffect.h>

#include <tdme/engine/Engine.h>
#include <tdme/engine/Timing.h>
#include <tdme/gui/events/Action.h>

using tdme::engine::Engine;
using tdme::engine::Timing;
using tdme::gui::effects::GUIEffect;
using tdme::gui::events::Action;

GUIEffect::GUIEffect()
{
	active = false;
	timeTotal = 0.0f;
	timeLeft = timeTotal;
	timePassed = 0.0f;
}

GUIEffect::~GUIEffect() {
	if (action != nullptr) delete action;
}

void GUIEffect::start()
{
	active = true;
	timeLeft = timeTotal;
	timePassed = 0.0f;
}

void GUIEffect::stop()
{
	active = false;
	timeLeft = timeTotal;
	timePassed = 0.0f;
}

bool GUIEffect::update(GUIRenderer* guiRenderer)
{
	if (active == false) return false;
	// TODO: Maybe do not use timing from main engine
	timePassed = static_cast<float>((Engine::getInstance()->getTiming()->getDeltaTime())) / 1000.0f;
	timeLeft -= timePassed;
	if (timeLeft < 0.0f) {
		timeLeft = 0.0f;
		active = false;
		return true;
	}
	return false;
}
