#include <tdme/utilities/Integer.h>

#include <algorithm>
#include <string>

#include <tdme/utilities/Console.h>

using tdme::utilities::Console;

using std::find_if;
using std::isdigit;
using std::stoi;
using std::string;
using std::to_string;

using tdme::utilities::Integer;

int Integer::parseInt(const string& str) {
	if (str.length() == 0) return 0;
	return stoi(str);
}

bool Integer::isInt(const string& str) {
    return
		str.empty() == false &&
		find_if(str.begin() + (str[0] == '-'?1:0), str.end(), [](char c) { return isdigit(c) == false; }) == str.end();
}
