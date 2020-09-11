#include <sstream>

#include <tdme/network/udpserver/ServerClient.h>

using std::stringstream;

using tdme::network::udpserver::ServerClient;

const char* ServerClient::KEY_PREFIX_UNNAMED = "unnamed.";

stringstream* ServerClient::createFrame() {
	return new stringstream();
}
