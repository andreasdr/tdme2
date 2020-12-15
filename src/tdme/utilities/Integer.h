#pragma once

#if defined(_WIN32) && defined(_MSC_VER)
	#define NOMINMAX
	#undef max
	#undef min
#endif

#include <tdme/utilities/fwd-tdme.h>

#include <limits>
#include <string>

using std::numeric_limits;
using std::string;

/**
 * Integer class
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::utilities::Integer final
{
public:
	static constexpr int MAX_VALUE { numeric_limits<int>::max() };
	static constexpr int MIN_VALUE { -numeric_limits<int>::max() };

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
