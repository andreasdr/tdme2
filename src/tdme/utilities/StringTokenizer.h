
#pragma once

#include <string>
#include <vector>

#include <tdme/utilities/fwd-tdme.h>

using std::string;
using std::vector;

/**
 * String tokenizer class
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::utilities::StringTokenizer
{

private:
	vector<string> tokens;
	int idx { 0 };

public:
	/**
	 * Public constructor
	 */
	StringTokenizer();

	/**
	 * Tokenize
	 * @param str string to tokenize
	 * @param delimiters delimiters
	 */
	void tokenize(const string& str, const string& delimiters);

	/**
	 * @return number of tokens
	 */
	inline int32_t countTokens() {
		return tokens.size();
	}

	/**
	 * @return has more tokens
	 */
	inline bool hasMoreTokens() {
		return idx != tokens.size();
	}

	/**
	 * @return next token
	 */
	inline const string& nextToken() {
		return tokens[idx++];
	}

	/**
	 * @return tokens
	 */
	inline const vector<string>& getTokens() {
		return tokens;
	}

};
