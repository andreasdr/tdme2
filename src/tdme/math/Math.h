#pragma once

#include <cmath>
#include <cstdlib>

#include <tdme/tdme.h>
#include <tdme/math/fwd-tdme.h>

#if defined(_WIN32) && defined(_MSC_VER)
	#define NOMINMAX
	#undef max
	#undef min
#endif

/** 
 * Standard math functions
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::math::Math final
{
public:
	static constexpr float E { 2.718281828459045f };
	static constexpr float PI { 3.141592653589793f };
	static constexpr float EPSILON { 0.0000001f };
	static constexpr float DEG2RAD { 0.017453294f };
	static constexpr float g { 9.80665f };

	/**
	 * Clamps a int value to min or max value
	 * @param value value
	 * @param min min value
	 * @param max max value
	 * @return clamped value
	 */
	inline static int clamp(int value, int min, int max) {
		if (value < min) return min;
		if (value > max) return max;
		return value;
	}

	/**
	 * Clamps a 64 bit int value to min or max value
	 * @param value value
	 * @param min min value
	 * @param max max value
	 * @return clamped value
	 */
	inline static int64_t clamp(int64_t value, int64_t min, int64_t max) {
		if (value < min) return min;
		if (value > max) return max;
		return value;
	}

	/**
	 * Clamps a float value to min or max value
	 * @param value value
	 * @param min min value
	 * @param max max value
	 * @return clamped value
	 */
	inline static float clamp(float value, float min, float max) {
		if (value < min) return min;
		if (value > max) return max;
		return value;
	}

	/**
	 * Returns sign of value
	 * @param value value
	 * @return -1 if value is negative or +1 if positive
	 */
	inline static float sign(float value) {
		return value / Math::abs(value);
	}

	/**
	 * Do the square product
	 * @param value value
	 * @return
	 */
	inline static float square(float value) {
		return value * value;
	}

	/**
	 * Returns absolute value
	 * @param value value
	 * @return absolute value
	 */
	inline static int32_t abs(int32_t value) {
		return std::abs(value);
	}

	/**
	 * Returns absolute value
	 * @param value value
	 * @return absolute value
	 */
	inline static int64_t abs(int64_t value) {
		return std::abs(value);
	}

	/**
	 * Returns absolute value
	 * @param value value
	 * @return absolute value
	 */
	inline static float abs(float value) {
		return std::fabs(value);
	}

	/**
	 * Returns the arc cosine of a value
	 * @param value value
	 * @return arc cosine
	 */
	inline static float acos(float value) {
		return std::acos(value);
	}

	/**
	 * Returns the arc sine of a value
	 * @param value value
	 * @return arc sine
	 */
	inline static float asin(float value) {
		return std::asin(value);
	}

	/**
	 * Returns the arc tangent of a value
	 * @param value value
	 * @return arc tangent
	 */
	inline static float atan(float value) {
		return std::atan(value);
	}

	/**
	 * Returns the angle from the conversion of rectangular coordinates to polar coordinates.
	 * @param y y
	 * @param x x
	 * @return angle
	 *
	 */
	inline static float atan2(float y, float x) {
		return std::atan2(y, x);
	}

	/**
	 * Returns the higher integer value of given value
	 * @param value value
	 * @return higher integer
	 */
	inline static float ceil(float value) {
		return std::ceil(value);
	}

	/**
	 * Returns the cosine of an angle
	 * @param value value
	 * @return cosine
	 */
	inline static float cos(float value) {
		return std::cos(value);
	}

	/**
	 * Returns the lower integer value of given value
	 * @param value value
	 * @return lower integer
	 */
	inline static float floor(float value) {
		return std::floor(value);
	}

	/**
	 * Returns the higher value of given values
	 * @param value1 value 1
	 * @param value2 value 2
	 * @return higher value
	 */
	inline static int32_t max(int32_t value1, int32_t value2) {
		return value1 > value2?value1:value2;
	}

	/**
	 * Returns the higher value of given values
	 * @param value1 value 1
	 * @param value2 value 2
	 * @return higher value
	 */
	inline static int64_t max(int64_t value1, int64_t value2) {
		return value1 > value2?value1:value2;
	}

	/**
	 * Returns the higher value of given values
	 * @param value1 value 1
	 * @param value2 value 2
	 * @return higher value
	 */
	inline static float max(float value1, float value2) {
		return value1 > value2?value1:value2;
	}

	/**
	 * Returns the lesser value of given values
	 * @param value1 value 1
	 * @param value2 value 2
	 * @return lesser value
	 */
	inline static int32_t min(int32_t value1, int32_t value2) {
		return value1 < value2?value1:value2;
	}

	/**
	 * Returns the lesser value of given values
	 * @param value1 value 1
	 * @param value2 value 2
	 * @return lesser value
	 */
	inline static int32_t min(int64_t value1, int64_t value2) {
		return value1 < value2?value1:value2;
	}

	/**
	 * Returns the lesser value of given values
	 * @param value1 value 1
	 * @param value2 value 2
	 * @return lesser value
	 */
	inline static float min(float value1, float value2) {
		return value1 < value2?value1:value2;
	}

	/**
	 * Returns the rounded value of given float value
	 * @param value value
	 * @return rounded value
	 */
	inline static float round(float value) {
		return std::round(value);
	}

	/**
	 * Returns the value of the value 1 raised to the power of value2
	 * @param value1 value 1
	 * @param value2 value 2
	 * @return pow
	 */
	inline static double pow(double value1, double value2) {
		return std::pow(value1, value2);
	}

	/**
	 * Returns a random value between 0.0 .. 1.0
	 * @return random value
	 */
	inline static float random() {
		return (float)rand() / (float)RAND_MAX;
	}

	/**
	 * Returns the sine of an angle
	 * @param value value
	 * @return sine
	 */
	inline static float sin(float value) {
		return std::sin(value);
	}

	/**
	 * Returns the square of given value
	 * @param value value
	 * @return squaere
	 */
	inline static float sqrt(float value) {
		return std::sqrt(value);
	}

	/**
	 * Returns the tangent of an angle
	 * @param value value
	 * @return tangent
	 */
	inline static float tan(float value) {
		return std::tan(value);
	}

	/**
	 * Returns e raised to the given power
	 * @param value value
	 * @return e
	 */
	inline static float exp(float value) {
		return std::exp(value);
	}

	/**
	 * Returns the natural (base e) logarithm of value
	 * @param value value
	 * @return log
	 */
	inline static float log(float value) {
		return std::log(value);
	}

	/**
	 * Returns modulo of value, so that return value is 0.0f <= value < range
	 * @param value value
	 * @param range range
	 * @return modulo of value
	 */
	inline static float mod(float value, float range) {
		return std::fmod(value, range);
	}

};
