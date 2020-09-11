#pragma once

#include <stdint.h>

#include <string>

#include <tdme/utilities/fwd-tdme.h>

using std::string;

/**
 * Integer to string conversion and vice versa utility class
 * @author Andreas Drewke
 */
class tdme::utilities::IntEncDec {
public:
	/**
	 * @brief Encodes an 32 bit unsigned integer to a 6 char string representation
	 * @param decodedInt int value to encode
	 * @param encodedString string
	 */
	static void encodeInt(const uint32_t decodedInt, string& encodedString);

	/**
	 * @brief Decodes an 6 char string representation to a unsigned 32 bit integer
	 * @param encodedInt encoded string
	 * @param decodedInt integer
	 */
	static bool decodeInt(const string& encodedInt, uint32_t& decodedInt);
};
