#include <tdme/os/network/Network.h>

#include <string.h>

#if defined(_WIN32)
	#include <winsock2.h>
	#include <ws2tcpip.h>
#else
	#include <netdb.h>
	#include <arpa/inet.h>
	#include <netinet/in.h>
	#include <sys/socket.h>
#endif

#include <string>

#include <tdme/os/network/NetworkException.h>
#include <tdme/os/network/NetworkSocket.h>
#include <tdme/utilities/Console.h>

using tdme::os::network::Network;

using std::to_string;

using tdme::os::network::NetworkException;
using tdme::os::network::NetworkSocket;
using tdme::utilities::Console;

bool Network::initialize() {
	#if defined(_WIN32)
		WSADATA wsaData;
		int result;
		result = WSAStartup(MAKEWORD(2,2), &wsaData);
		if (result != 0) {
			Console::println("WinSock2 initialization failed: " + to_string(result));
			return false;
		}
	#endif
	return true;
}

const string Network::getIpByHostName(const string& hostName) {
	struct hostent* hostEnt = gethostbyname(hostName.c_str());

	if (hostEnt == nullptr) {
		throw NetworkException("Could not resolve hostname");
	}

	switch (hostEnt->h_addrtype) {
	case AF_INET:
		for (auto i = 0; hostEnt->h_addr_list[i] != nullptr; i++) {
			struct in_addr addr;
			addr.s_addr = *(uint32_t*)hostEnt->h_addr_list[i++];
			return string(inet_ntoa(addr));
		}
		break;
	case AF_INET6:
		for (auto i = 0; hostEnt->h_addr_list[i] != nullptr; i++) {
			char ipv6AddressString[46];
			return string(inet_ntop(AF_INET6, (in6_addr*)hostEnt->h_addr_list[i++], ipv6AddressString, INET6_ADDRSTRLEN) == NULL?"":ipv6AddressString);
		}
		break;
	}

	throw NetworkException("Could not resolve hostname");
}
