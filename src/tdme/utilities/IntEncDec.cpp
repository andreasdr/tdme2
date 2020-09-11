#include <string.h>

#include <tdme/utilities/IntEncDec.h>

using std::string;

using tdme::utilities::IntEncDec;

void IntEncDec::encodeInt(const uint32_t decodedInt, string& encodedString) {
	encodedString = "";
	char encodingCharSet[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVW-+/*.";
	for (auto i = 0; i < 6; i++) {
		auto charIdx = (decodedInt >> (i * 6)) & 63;
		encodedString = encodingCharSet[charIdx] + encodedString;
	}
}

bool IntEncDec::decodeInt(const string& encodedInt, uint32_t& decodedInt) {
	char encodingCharSet[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVW-+/*.";
	decodedInt = 0;
	for (auto i = 0; i < encodedInt.length(); i++) {
		auto codeIdx = -1;
		char c = encodedInt[encodedInt.length() - i - 1];
		char* codePtr = strchr(encodingCharSet, c);
		if (codePtr == NULL) {
			return false;
		} else {
			codeIdx = codePtr - encodingCharSet;
		}
		decodedInt+= codeIdx << (i * 6);
	}
	return true;
}
