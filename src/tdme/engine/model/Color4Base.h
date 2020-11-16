#pragma once

#include <array>

#include <tdme/tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/math/Math.h>

using std::array;

using tdme::math::Math;

/**
 * Color 4 base definition class
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::model::Color4Base
{
protected:
	array<float, 4> data;

public:
	/**
	 * Public constructor
	 */
	inline Color4Base() {
		data[0] = 0.0f;
		data[1] = 0.0f;
		data[2] = 0.0f;
		data[3] = 1.0f;
	}

	/**
	 * Public constructor
	 * @param color color
	 */
	inline Color4Base(const Color4Base& color) {
		data = color.data;
	}

	/**
	 * Public constructor
	 * @param color color
	 */
	inline Color4Base(const array<float, 4>& color) {
		this->data = color;
	}

	/**
	 * Public constructor
	 * @param r red
	 * @param g green
	 * @param b blue
	 * @param a alpha
	 */
	inline Color4Base(float r, float g, float b, float a) {
		data[0] = r;
		data[1] = g;
		data[2] = b;
		data[3] = a;
	}

	/**
	 * Set up color
	 * @param color color float array
	 */
	inline void set(const array<float, 4>& color) {
		this->data = color;
	}

	/**
	 * Set up color
	 * @param r red
	 * @param g green
	 * @param b blue
	 * @param a alpha
	 */
	inline void set(float r, float g, float b, float a) {
		data[0] = r;
		data[1] = g;
		data[2] = b;
		data[3] = a;
	}

	/**
	 * Sets up this color with given color
	 * @param color color
	 */
	inline void set(const Color4Base& color) {
		data = color.data;
	}

	/**
	 * Add to color
	 * @param r red
	 * @param g green
	 * @param b blue
	 * @param a alpha
	 */
	inline void add(float r, float g, float b, float a) {
		data[0] += r;
		data[1] += g;
		data[2] += b;
		data[3] += a;
	}

	/**
	 * @return red component
	 */
	inline float getRed() const {
		return data[0];
	}

	/**
	 * @param red red component
	 */
	inline void setRed(float red) {
		data[0] = red;
	}

	/**
	 * @return green component
	 */
	inline float getGreen() const {
		return data[1];
	}

	/**
	 * @param green green component
	 */
	inline void setGreen(float green) {
		data[1] = green;
	}

	/**
	 * @return blue component
	 */
	inline float getBlue() const {
		return data[2];
	}

	/**
	 * @param blue blue component
	 */
	inline void setBlue(float blue) {
		data[2] = blue;
	}

	/**
	 * @return alpha component
	 */
	inline float getAlpha() const {
		return data[3];
	}

	/**
	 * @param alpha alpha component
	 */
	inline void setAlpha(float alpha) {
		data[3] = alpha;
	}

	/**
	 * Scale this color
	 * @param scale scale
	 * @return this color
	 */
	inline Color4Base& scale(float scale) {
		data[0] *= scale;
		data[1] *= scale;
		data[2] *= scale;
		data[3] *= scale;
		return *this;
	}

	/**
	 * Scale this color
	 * @param scale scale
	 * @return this color
	 */
	inline Color4Base& scale(const Color4Base& scale) {
		data[0] *= scale.data[0];
		data[1] *= scale.data[1];
		data[2] *= scale.data[2];
		data[3] *= scale.data[3];
		return *this;
	}

	/**
	 * Add color to this color
	 * @param color color
	 * @return this color
	 */
	inline Color4Base& add(const Color4Base& color) {
		data[0] += color.data[0];
		data[1] += color.data[1];
		data[2] += color.data[2];
		data[3] += color.data[3];
		return *this;
	}

	/**
	 * Subtracts color from this color
	 * @param color color
	 * @return this color
	 */
	inline Color4Base& sub(const Color4Base& color) {
		data[0] -= color.data[0];
		data[1] -= color.data[1];
		data[2] -= color.data[2];
		data[3] -= color.data[3];
		return *this;
	}

	/**
	 * @return rgba as float array
	 */
	inline array<float, 4>& getArray() const {
		return (array<float, 4>&)data;
	}

	/**
	 * Compares this color with given color
	 * @return equality
	 */
	inline bool equals(const Color4Base& c) const {
		return (this == &c) || (
			Math::abs(data[0] - c.data[0]) < Math::EPSILON &&
			Math::abs(data[1] - c.data[1]) < Math::EPSILON &&
			Math::abs(data[2] - c.data[2]) < Math::EPSILON &&
			Math::abs(data[3] - c.data[3]) < Math::EPSILON
		);
	}

};
