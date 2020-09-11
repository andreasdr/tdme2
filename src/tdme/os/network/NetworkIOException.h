#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/os/network/NetworkException.h>

using std::string;

using tdme::os::network::NetworkException;

/**
 * Base exception class for network IO exceptions
 * @author Andreas Drewke
 */
class tdme::os::network::NetworkIOException: public NetworkException {
public:
	/**
	 * @brief public constructor
	 * @param &message message exception message
	 */
	NetworkIOException(const string &message) throw();
};
