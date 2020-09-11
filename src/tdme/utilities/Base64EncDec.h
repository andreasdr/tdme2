#pragma once

#include <stdint.h>

#include <string>

#include <tdme/utilities/fwd-tdme.h>

using std::string;

/**
 * Base64 encoding/decoding class
 * @author Andreas Drewke
 */
class tdme::utilities::Base64EncDec {
public:
	/**
	 * Encodes an string to base 64
	 * @param decodedString string to encode
	 * @param encodedString encoded string
	 */
	static void encode(const string& decodedString, string& encodedString);

	/**
	 * Decodes an base64 encoded string
	 * @param encodedString encoded string
	 * @param decodedString decoded string
	 */
	static void decode(const string& encodedString, string& decodedString);
};
