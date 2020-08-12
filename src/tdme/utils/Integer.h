
#pragma once

#include <tdme/utils/fwd-tdme.h>

#include <string>

using std::string;

/**
 * Integer class
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::utils::Integer final
{
public:

	/**
	 * Parse integer
	 * @param str string
	 * @return integer
	 */
	static int parseInt(const string& str);

	/**
	 * Check if given string is a integer string
	 * @param str string
	 * @return given string is integer
	 */
	static bool isInt(const string& str);

};
