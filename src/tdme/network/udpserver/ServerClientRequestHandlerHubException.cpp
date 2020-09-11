#include <tdme/network/udpserver/ServerClientRequestHandlerHubException.h>

#include <string>

#include <tdme/utilities/ExceptionBase.h>

using std::string;

using tdme::network::udpserver::ServerClientRequestHandlerHubException;
using tdme::utilities::ExceptionBase;

ServerClientRequestHandlerHubException::ServerClientRequestHandlerHubException(const string &message) throw() : ExceptionBase(message) {
}
