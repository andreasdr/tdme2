#include <tdme/utilities/Float.h>

#include <algorithm>
#include <string>

using std::find_if;
using std::isnan;
using std::stof;
using std::string;
using std::to_string;

using tdme::utilities::Float;

float Float::parseFloat(const string& str) {
	if (str.length() == 0) return 0.0f;
	return stof(str);
}

bool Float::isFloat(const string& str) {
	int dotCount = 0;
	return
		str.empty() == false &&
		find_if(str.begin() + (str[0] == '-'?1:0), str.end(), [&dotCount](char c) { return isdigit(c) == false && (c != '.' || ++dotCount > 1); }) == str.end();
}
