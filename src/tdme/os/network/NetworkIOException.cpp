#include <tdme/os/network/NetworkIOException.h>

#include <string>

#include <tdme/os/network/NetworkException.h>

using tdme::os::network::NetworkException;
using tdme::os::network::NetworkIOException;
using std::string;

NetworkIOException::NetworkIOException(const string &message) throw() : NetworkException(message) {
}

