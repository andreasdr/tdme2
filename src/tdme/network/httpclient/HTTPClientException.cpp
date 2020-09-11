#include <tdme/network/httpclient/HTTPClientException.h>

#include <string>

#include <tdme/network/httpclient/HTTPClientException.h>

using tdme::network::httpclient::HTTPClientException;

using std::string;

using tdme::utilities::ExceptionBase;

HTTPClientException::HTTPClientException(const string &message) throw() : ExceptionBase(message) {
}

