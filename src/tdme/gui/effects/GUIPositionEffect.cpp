#include <tdme/gui/effects/GUIPositionEffect.h>

#include <tdme/gui/renderer/GUIRenderer.h>
#include <tdme/gui/GUI.h>

using tdme::gui::effects::GUIPositionEffect;
using tdme::gui::renderer::GUIRenderer;
using tdme::gui::GUI;

GUIPositionEffect::GUIPositionEffect(): GUIEffect()
{
}

void GUIPositionEffect::apply(GUIRenderer* guiRenderer)
{
	float screenWidth = guiRenderer->getGUI()->getWidth();
	float screenHeight = guiRenderer->getGUI()->getHeight();
	positionX = positionXStart + ((positionXEnd - positionXStart) / timeTotal * (timeTotal - timeLeft));
	positionY = positionYStart + ((positionYEnd - positionYStart) / timeTotal * (timeTotal - timeLeft));
	guiRenderer->setGUIEffectOffsetX(-positionX / screenWidth * 2.0f);
	guiRenderer->setGUIEffectOffsetY(-positionY / screenHeight * 2.0f);
}

