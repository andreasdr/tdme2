#include <tdme/os/network/NetworkKEMException.h>

#include <string>

#include <tdme/os/network/NetworkException.h>

using std::string;
using tdme::os::network::NetworkException;
using tdme::os::network::NetworkKEMException;

NetworkKEMException::NetworkKEMException(const string &message) throw() : NetworkException(message) {
}

