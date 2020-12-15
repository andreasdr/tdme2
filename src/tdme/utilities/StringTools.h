#pragma once

#include <string>
#include <vector>

#include <tdme/utilities/fwd-tdme.h>

using std::string;
using std::vector;

/**
 * String tools class
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::utilities::StringTools final
{
public:

	/**
	 * Checks if string starts with prefix
	 * @param src source string
     * @param prefix prefix string
	 * @return bool
	 */
	static const bool startsWith(const string& src, const string& prefix);

	/**
	 * Checks if string ends with suffix
	 * @param src source string
     * @param suffix suffix string
	 * @return bool
	 */
	static const bool endsWith(const string& src, const string& suffix);

	/**
	 * Replace char with another char
	 * @param src source string to be processed
	 * @param what what to replace
	 * @param by to replace by
	 * @param beginIndex index to begin with
	 * @return new string
	 */
	static const string replace(const string& src, const char what, const char by, int beginIndex = 0);

	/**
	 * Replace string with another string
	 * @param src source string to be processed
	 * @param what what to replace
	 * @param by to replace by
	 * @param beginIndex index to begin with
	 * @return new string
	 */
	static const string replace(const string& src, const string& what, const string& by, int beginIndex = 0);

	/**
	 * Finds first index of given character
	 * @param src source string
	 * @param what what
	 * @return index or -1 if not found
	 */
	static int32_t firstIndexOf(const string& src, char what);

	/**
	 * Finds first index of characters provided within given string
	 * @param src source string
	 * @param what what
	 * @return index or -1 if not found
	 */
	static int32_t firstIndexOf(const string& src, const string& what);

	/**
	 * Finds last index of given character
	 * @param src source string
	 * @param what what
	 * @return index or -1 if not found
	 */
	static int32_t lastIndexOf(const string& src, char what);

	/**
	 * Finds last index of characters provided within given string
	 * @param src source string
	 * @param what what
	 * @return index or -1 if not found
	 */
	static int32_t lastIndexOf(const string& src, const string& what);

	/**
	 * Returns substring of given string from begin index
	 * @param src source string
	 * @param beginIndex begin index
	 * @return new string
	 */
	static const string substring(const string& src, int32_t beginIndex);

	/**
	 * Returns substring of given string from begin index to end index
	 * @param src source string
	 * @param beginIndex begin index
	 * @param endIndex end index
	 * @return new string
	 */
	static const string substring(const string& src, int32_t beginIndex, int32_t endIndex);

	/**
	 * Checks if string equals ignoring case
	 * @param string1 string 1
	 * @param string2 string 2
	 * @return equals
	 */
	static bool equalsIgnoreCase(const string& string1, const string& string2);

	/**
	 * Trim string
	 * @param src source string
	 * @return trimmed string
	 */
	static const string trim(const string& src);

	/**
	 * Transform string to lower case
	 * @param src source string
	 * @return transformed string
	 */
	static const string toLowerCase(const string& src);

	/**
	 * Transform string to upper case
	 * @param src source string
	 * @return transformed string
	 */
	static const string toUpperCase(const string& src);

	/**
	 * Do regex pattern matching
	 * @param src source string to test
	 * @param pattern pattern
	 * @return if patter matches
	 */
	static bool regexMatch(const string& src, const string& pattern);

	/**
	 * Replace regex pattern with given string
	 * @param src source string to operate on
	 * @param pattern pattern to search
	 * @param by string that will replace pattern occurrances
	 */
	static const string regexReplace(const string& src, const string& pattern, const string& by);

	/**
	 * Tokenize
	 * @param str string to tokenize
	 * @param delimiters delimiters
	 * @return tokens
	 */
	static const vector<string> tokenize(const string& str, const string& delimiters);

};

