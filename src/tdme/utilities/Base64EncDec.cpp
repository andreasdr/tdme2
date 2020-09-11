#include <string>
#include <vector>

#include <tdme/utilities/Base64EncDec.h>

using std::string;
using std::vector;

using tdme::utilities::Base64EncDec;

void Base64EncDec::encode(const string& decodedString, string& encodedString) {
	// see: https://stackoverflow.com/questions/180947/base64-decode-snippet-in-c/34571089#34571089
	string dictionary = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	int val = 0, valb = -6;
	for (uint8_t c: decodedString) {
		val = (val << 8) + c;
		valb+= 8;
		while (valb >= 0) {
			encodedString.push_back(dictionary[(val >> valb) & 0x3F]);
			valb-= 6;
		}
	}
	if (valb > -6) encodedString.push_back(dictionary[((val << 8) >> (valb + 8)) & 0x3F]);
	while (encodedString.size() % 4) encodedString.push_back('=');
}

void Base64EncDec::decode(const string& encodedString, string& decodedString) {
	// see: https://stackoverflow.com/questions/180947/base64-decode-snippet-in-c/34571089#34571089
	string dictionary = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	std::vector<int> T(256, -1);
	for (int i = 0; i < 64; i++) T[dictionary[i]] = i; 
		int val = 0, valb = -8;
		for (uint8_t c: encodedString) {
			if (T[c] == -1) break;
			val = (val << 6) + T[c];
			valb+= 6;
			if (valb >= 0) {
			decodedString.push_back(char((val >> valb) & 0xFF));
			valb-= 8;
		}
	}
}
