#include <string.h>

#include <tdme/utilities/HexEncDec.h>

using std::string;

using tdme::utilities::HexEncDec;

void HexEncDec::encodeInt(const uint64_t decodedInt, string& encodedString) {
	encodedString = "";
	char encodingCharSet[] = "0123456789abcdef";
	auto _decodedInt = decodedInt;
	for (auto i = 0; i < 32; i++) {
		auto charIdx = _decodedInt & 15;
		encodedString = encodingCharSet[charIdx] + encodedString;
		_decodedInt>>= 4;
		if (_decodedInt == 0) break;
	}
}
