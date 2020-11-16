#pragma once

#include <array>

#include <tdme/tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/model/Color4Base.h>

using std::array;

using tdme::engine::model::Color4Base;

/**
 * Color 4 definition
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::model::Color4 final
	: public Color4Base
{
public:
	/**
	 * Public constructor
	 */
	inline Color4() : Color4Base() {
	}

	/**
	 * Public constructor
	 * @param color color
	 */
	inline Color4(const Color4& color) : Color4Base(color) {
	}

	/**
	 * Public constructor
	 * @param r red
	 * @param g green
	 * @param b blue
	 * @param a alpha
	 */
	inline Color4(float r, float g, float b, float a) : Color4Base(r,g,b,a) {
	}

	/**
	 * Public constructor
	 * @param color color array
	 */
	inline Color4(const array<float, 4>& color) : Color4Base(color) {
	}

};
