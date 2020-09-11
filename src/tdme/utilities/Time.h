#pragma once

#include <tdme/utilities/fwd-tdme.h>

#include <chrono>
#include <ctime>
#include <string>

using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;
using std::string;

/**
 * Time utility class
 * @author mahula
 * @version $Id$
 */
class tdme::utilities::Time
{
public:

	/**
	 * Retrieve current time in milliseconds
	 * @return int64_t
	 */
	inline static int64_t getCurrentMillis() {
		// TODO: check clock_gettime from time.h
		//	struct timespec now;
		//	clock_gettime(CLOCK_REALTIME, &now);
		//	return (now.tv_sec * 1000L) + (now.tv_nsec / 1000000L);
		return high_resolution_clock::now().time_since_epoch() / milliseconds(1);
	}

	/**
	 * Get date/time as string
	 * @param format format, see strftime
	 * @return date/time as string
	 */
	inline static string getAsString(const string& format = "%Y-%m-%d %H:%M:%S") {
		// see: https://stackoverflow.com/questions/34963738/c11-get-current-date-and-time-as-string
		std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		char timeString[256] = {0};
		std::strftime(timeString, sizeof(timeString), format.c_str(), std::localtime(&now));
		return string(timeString);
	}
};
