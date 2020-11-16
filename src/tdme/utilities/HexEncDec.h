#pragma once

#include <stdint.h>

#include <string>

#include <tdme/utilities/fwd-tdme.h>

using std::string;

/**
 * Integer to hex string conversion utility class
 * @author Andreas Drewke
 */
class tdme::utilities::HexEncDec {
public:

	/**
	 * @brief Encodes an 64 bit unsigned integer to a hex string representation
	 * @param decodedInt int value to encode
	 * @returns encodedString
	 */
	inline static const string encodeInt(const uint64_t decodedInt) {
		string encodedString;
		encodeInt(decodedInt, encodedString);
		return encodedString;
	}

	/**
	 * @brief Encodes an 64 bit unsigned integer to a hex string representation
	 * @param decodedInt int value to encode
	 * @param encodedString string
	 */
	static void encodeInt(const uint64_t decodedInt, string& encodedString);

};
