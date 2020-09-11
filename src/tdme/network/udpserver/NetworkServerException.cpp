#include <tdme/network/udpserver/NetworkServerException.h>

using tdme::network::udpserver::NetworkServerException;

NetworkServerException::NetworkServerException(const string &message) throw() : NetworkException(message) {
}

