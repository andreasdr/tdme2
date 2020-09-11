#pragma once

#include <string>

#include <tdme/network/udpserver/fwd-tdme.h>

#include <tdme/tdme.h>
#include <tdme/os/network/fwd-tdme.h>
#include <tdme/os/network/NetworkException.h>

using tdme::os::network::NetworkException;

/**
 * Base exception class for network server exceptions
 * @author Andreas Drewke
 */
class tdme::network::udpserver::NetworkServerException: public NetworkException {
public:
	/**
	 * @brief public constructor
	 * @param &message message exception message
	 */
	NetworkServerException(const string &message) throw();
};
