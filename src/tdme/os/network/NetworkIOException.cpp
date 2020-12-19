#include <tdme/os/network/NetworkIOException.h>

#include <string>

#include <tdme/os/network/NetworkException.h>

using std::string;
using tdme::os::network::NetworkException;
using tdme::os::network::NetworkIOException;

NetworkIOException::NetworkIOException(const string &message) throw() : NetworkException(message) {
}

