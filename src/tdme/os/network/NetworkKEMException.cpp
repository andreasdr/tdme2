#include <tdme/os/network/NetworkKEMException.h>

#include <string>

#include <tdme/os/network/NetworkException.h>

using tdme::os::network::NetworkException;
using tdme::os::network::NetworkKEMException;
using std::string;

NetworkKEMException::NetworkKEMException(const string &message) throw() : NetworkException(message) {
}

