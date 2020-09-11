#include <tdme/network/udpclient/NetworkClientException.h>

using tdme::network::udpclient::NetworkClientException;

NetworkClientException::NetworkClientException(const string &message) throw() : NetworkException(message) {
}

