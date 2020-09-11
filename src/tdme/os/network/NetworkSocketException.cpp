#include <tdme/os/network/NetworkSocketException.h>

#include <string>

#include <tdme/os/network/NetworkException.h>

using tdme::os::network::NetworkSocketException;

using std::string;

using tdme::os::network::NetworkException;

NetworkSocketException::NetworkSocketException(const string &message) throw() : NetworkException(message) {
}

