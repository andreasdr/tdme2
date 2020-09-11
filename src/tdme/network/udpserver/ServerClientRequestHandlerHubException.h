#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/utilities/ExceptionBase.h>

using tdme::utilities::ExceptionBase;

namespace tdme {
namespace network {
namespace udpserver {

/**
 * Network server client request handler hub exception
 * @author Andreas Drewke
 */
class ServerClientRequestHandlerHubException: public ExceptionBase {
public:
	/**
	 * @brief public constructor
	 * @param &message message exception message
	 */
	ServerClientRequestHandlerHubException(const string &message) throw();
};

};
};
};

