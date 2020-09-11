
#pragma once

#include <string>
#include <vector>

#include <tdme/utilities/fwd-tdme.h>

using std::vector;
using std::string;

/**
 * String tokenizer class
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::utilities::StringTokenizer
{

private:
	vector<string> elements;
	int idx { 0 };

public:
	/**
	 * Tokenize
	 * @param str string to tokenize
	 * @param delimiters delimiters
	 */
	void tokenize(const string& str, const string& delimiters);

	/**
	 * @return number of tokens
	 */
	int32_t countTokens();

	/**
	 * @return has more tokens
	 */
	bool hasMoreTokens();

	/**
	 * @return next token
	 */
	const string& nextToken();

	/**
	 * Public constructor
	 */
	StringTokenizer();
};
