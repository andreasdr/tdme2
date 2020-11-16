
#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/utilities/fwd-tdme.h>

using std::string;

/**
 * Mutable string class
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::utilities::MutableString final
{
public:
	/**
	 * Public default constructor
	 */
	MutableString();

	/**
	 * Public constructor
	 * @param s string
	 */
	MutableString(const string& s);

	/**
	 * Public constructor
	 * @param i integer
	 */
	MutableString(int i);

	/**
	 * Public constructor
	 * @param f f
	 * @param decimals decimals
	 */
	MutableString(float f, int32_t decimals);

	/**
	 * @return length
	 */
	int32_t length() const;

	/**
	 * Get char at index
	 * @param idx idx
	 * @return char
	 */
	char charAt(int32_t idx) const;

	/**
	 * Reset
	 */
	MutableString& reset();

	/**
	 * Set character
	 * @param c char
	 * @return this mutable string
	 */
	MutableString& set(char c);

	/**
	 * Append character
	 * @param c char
	 * @return this mutable string
	 */
	MutableString& append(char c);

	/**
	 * Insert character c at idx
	 * @param idx index
	 * @param c char
	 * @return this mutable string
	 */
	MutableString& insert(int32_t idx, char c);

	/**
	 * Set string
	 * @param s s
	 * @return this mutable string
	 */
	MutableString& set(const string& s);

	/**
	 * Append string
	 * @param s s
	 * @return this mutable string
	 */
	MutableString& append(const string& s);

	/**
	 * Insert string at idx
	 * @param idx index
	 * @param s string
	 * @return this mutable string
	 */
	MutableString& insert(int32_t idx, const string& s);

	/**
	 * Set mutable string
	 * @param s s
	 * @return this mutable string
	 */
	MutableString& set(const MutableString& s);

	/**
	 * Append mutable string
	 * @param s s
	 * @return this mutable string
	 */
	MutableString& append(const MutableString& s);

	/**
	 * Insert mutable string at idx
	 * @param idx index
	 * @param s string
	 * @return this mutable string
	 */
	MutableString& insert(int32_t idx, const MutableString& s);

	/**
	 * Set integer
	 * @param i i
	 * @return this mutable string
	 */
	MutableString& set(int32_t i);

	/**
	 * Append integer
	 * @param i i
	 * @return this mutable string
	 */
	MutableString& append(int32_t i);

	/**
	 * Insert integer at idx
	 * @param idx index
	 * @param i i
	 * @return this mutable string
	 */
	MutableString& insert(int32_t idx, int32_t i);

	/**
	 * Set float
	 * @param f f
	 * @param decimals decimals
	 * @return this mutable string
	 */
	MutableString& set(float f, int32_t decimals);

	/**
	 * Append float with given decimals
	 * @param f f
	 * @param decimals decimals
	 * @return this mutable string
	 */
	MutableString& append(float f, int32_t decimals);

	/**
	 * Insert float at idx
	 * @param idx index
	 * @param f float
	 * @param decimals decimals
	 * @return this mutable string
	 */
	MutableString& insert(int32_t idx, float f, int32_t decimals);

	/**
	 * Delete characters at idx with given length
	 * @param idx idx
	 * @param count length
	 * @return this mutable string
	 */
	MutableString& delete_(int32_t idx, int32_t count);

	/**
	 * Returns the character index where string s have been found or -1 if not found
	 * @param s string
	 * @param idx index
	 * @return index where string has been found or -1
	 */
	int32_t indexOf(const MutableString& s, int32_t idx) const;

	/**
	 * Returns the character index where string s have been found or -1 if not found
	 * @param s string
	 * @return index where string has been found or -1
	 */
	int32_t indexOf(const MutableString& s) const;

	/**
	 * Equals
	 * @param s2 string 2
	 * @return string 2 equals this string
	 */
	bool equals(const string& s2) const;

	/**
	 * Equals
	 * @param s2 string 2
	 * @return string 2 equals this string
	 */
	bool equals(const MutableString& s2) const;

	/**
	 * @return string
	 */
	const string& getString() const;

	/**
	 * Clone
	 */
	MutableString clone();

private:
	string data;
};
