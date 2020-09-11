#pragma once

#include <tdme/os/network/NetworkIOException.h>

#include <tdme/tdme.h>

/**
 * Network socket closed exception
 * @author Andreas Drewke
 */
class tdme::os::network::NetworkSocketClosedException: public NetworkIOException {
	public:
		/**
		 * @brief public constructor
		 * @param message exception message
		 */
		NetworkSocketClosedException(const string &message) throw();
};

