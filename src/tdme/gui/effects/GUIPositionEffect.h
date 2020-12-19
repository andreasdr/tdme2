#pragma once

#include <tdme/tdme.h>
#include <tdme/gui/effects/fwd-tdme.h>
#include <tdme/gui/effects/GUIEffect.h>
#include <tdme/gui/renderer/fwd-tdme.h>

using tdme::gui::effects::GUIEffect;
using tdme::gui::renderer::GUIRenderer;

/**
 * GUI position effect
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::gui::effects::GUIPositionEffect final: public GUIEffect
{

private:
	float positionXStart { 0.0f };
	float positionXEnd { 0.0f };
	float positionYStart { 0.0f };
	float positionYEnd { 0.0f };
	float positionX { 0.0f };
	float positionY { 0.0f };

public:
	/**
	 *  Public constructor
	 */
	GUIPositionEffect();

	/**
	 * @return position X start
	 */
	inline virtual float getPositionXStart() const {
		return positionXStart;
	}

	/**
	 * Set position X start
	 * @param positionXStart position X start
	 */
	inline virtual void setPositionXStart(float positionXStart) {
		this->positionXStart = positionXStart;
	}

	/**
	 * @return position X end
	 */
	inline virtual float getPositionXEnd() const {
		return positionXEnd;
	}

	/**
	 * Set position X end
	 * @param positionXEnd position X end
	 */
	inline virtual void setPositionXEnd(float positionXEnd) {
		this->positionXEnd = positionXEnd;
	}

	/**
	 * @return position Y start
	 */
	inline virtual float getPositionYStart() const {
		return positionYStart;
	}

	/**
	 * Set position Y start
	 * @param positionYStart position Y start
	 */
	inline virtual void setPositionYStart(float positionYStart) {
		this->positionYStart = positionYStart;
	}

	/**
	 * @return get position Y end
	 */
	inline virtual float getPositionYEnd() const {
		return positionYEnd;
	}

	/**
	 * Set position Y end
	 * @param positionYEnd position Y end
	 */
	inline virtual void setPositionYEnd(float positionYEnd) {
		this->positionYEnd = positionYEnd;
	}

	// override methods
	void apply(GUIRenderer* guiRenderer) override;

};
