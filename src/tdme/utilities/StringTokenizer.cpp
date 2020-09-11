#include <string>
#include <vector>

#include <tdme/utilities/StringTokenizer.h>

using std::string;

using tdme::utilities::StringTokenizer;

StringTokenizer::StringTokenizer() {
}

void StringTokenizer::tokenize(const string& str, const string& delimiters)
{
	idx = 0;
	elements.clear();
	string token;
	for (int i = 0; i < str.length(); i++) {
		// got a delimiter?
		if (delimiters.find(str[i]) != -1) {
			// yep, add token to elements if we have any
			if (token.length() > 0) {
				elements.push_back(token);
				token = "";
			}
		} else {
			// no delimiter, add char to token
			token+= str[i];
		}
	}
	// do we have a token still? add it to elements
	if (token.length() > 0) {
		elements.push_back(token);
		token = "";
	}
}

int32_t StringTokenizer::countTokens()
{
	return elements.size();
}

bool StringTokenizer::hasMoreTokens()
{
	return idx != elements.size();;
}

const string& StringTokenizer::nextToken()
{
	return elements[idx++];
}
