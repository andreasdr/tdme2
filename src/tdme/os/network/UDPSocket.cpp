#include <errno.h>
#include <string.h>
#if defined(_WIN32)
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#define socklen_t int
	#define BUF_CAST(buf) ((char*)buf)
#else
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#define BUF_CAST(buf) ((void*)buf)
#endif

#include <string>

#include <tdme/os/network/UDPSocket.h>

using std::string;
using std::to_string;

using tdme::os::network::UDPSocket;

// determine which SO_REUSE option to use
#if defined(_WIN32)
	#define SO_REUSEOPTION	SO_REUSEADDR
#elif defined(__linux__)
	// on linux < 3.9 we need to to use "addr". it behaves like "port" on BSD for UDP sockets
	#define SO_REUSEOPTION	SO_REUSEADDR
#else
	// standard is "port" as it exactly does what we want
	#define SO_REUSEOPTION	SO_REUSEPORT
#endif

UDPSocket::~UDPSocket() {
}

ssize_t UDPSocket::read(string& from, unsigned int& port, void* buf, const size_t bytes) {
	// socket address in setup
	socklen_t sinLen = 0;
	void* sin;
	sockaddr_in sinIPV4;
	sockaddr_in6 sinIPV6;
	switch(ipVersion) {
		case IPV4:
			{
				sin = &sinIPV4;
				sinLen = sizeof(sinIPV4);
			}
			break;
		case IPV6:
			{

				sin = &sinIPV6;
				sinLen = sizeof(sinIPV6);
			}
	}

	ssize_t bytesRead = ::recvfrom(descriptor, BUF_CAST(buf), bytes, 0, (struct sockaddr *)sin, &sinLen);
	if (bytesRead == -1) {
		#if defined(_WIN32)
			auto wsaError = WSAGetLastError();
			if (wsaError == WSAEWOULDBLOCK ||
				wsaError == WSAECONNRESET) {
				return -1;
			} else {
				string msg = "error while reading from socket: ";
				msg+= to_string(wsaError);
				throw NetworkIOException(msg);
			}
		#else
			// nope throw an exception
			if (errno == EAGAIN) {
				return -1;
			} else {
				string msg = "error while reading from socket: ";
				msg+= strerror(errno);
				throw NetworkIOException(msg);
			}
		#endif
	}

	// set up senders ip + port
	switch(ipVersion) {
		case IPV4:
			{
				char ipv4AddressString[INET_ADDRSTRLEN];
				from = inet_ntop(AF_INET, &sinIPV4.sin_addr, ipv4AddressString, INET_ADDRSTRLEN) == NULL?"127.0.0.1":ipv4AddressString;
				port = ntohs(sinIPV4.sin_port);
			}
			break;
		case IPV6:
			{
				char ipv6AddressString[INET6_ADDRSTRLEN];
				from = inet_ntop(AF_INET6, &sinIPV6.sin6_addr, ipv6AddressString, INET6_ADDRSTRLEN) == NULL?"::1":ipv6AddressString;
				port = ntohs(sinIPV6.sin6_port);
			}
	}

	// return bytes read
	return bytesRead;
}

ssize_t UDPSocket::write(const string& to, const unsigned int port, void* buf, const size_t bytes) {
	// receiver address in setup
	socklen_t sinLen = 0;
	void* sin;
	sockaddr_in sinIPV4;
	sockaddr_in6 sinIPV6;
	switch(ipVersion) {
		case IPV4:
			{
				sinLen = sizeof(sinIPV4);
				memset(&sinIPV4, 0, sinLen);
				sinIPV4.sin_family = AF_INET;
				sinIPV4.sin_port = htons(port);
				sinIPV4.sin_addr.s_addr = inet_addr(to.c_str());
				sin = &sinIPV4;
			}
			break;
		case IPV6:
			{
				sinLen = sizeof(sinIPV6);
				memset(&sinIPV6, 0, sinLen);
				sinIPV6.sin6_family = AF_INET6;
				sinIPV6.sin6_port = htons(port);
				inet_pton(AF_INET6, to.c_str(), &sinIPV6.sin6_addr);
				sin = &sinIPV6;
			}
			break;
	}

	// go
	#if defined(__APPLE__) || defined(_WIN32)
		ssize_t bytesWritten = ::sendto(descriptor, BUF_CAST(buf), bytes, 0, (const struct sockaddr*)sin, sinLen);
	#else
		ssize_t bytesWritten = ::sendto(descriptor, BUF_CAST(buf), bytes, MSG_NOSIGNAL, (const struct sockaddr*)sin, sinLen);
	#endif

	// send successful?
	if (bytesWritten == -1) {
		#if defined(_WIN32)
			auto wsaError = WSAGetLastError();
			if (wsaError == WSAEWOULDBLOCK) {
				return -1;
			} else {
				string msg = "error while writing to socket: ";
				msg+= to_string(wsaError);
				throw NetworkIOException(msg);
			}
		#else
			// nope throw an exception
			if (errno == EAGAIN) {
				return -1;
			} else {
				string msg = "error while writing to socket: ";
				msg+= strerror(errno);
				throw NetworkIOException(msg);
			}
		#endif
	}

	// return bytes written
	return bytesWritten;
}

void UDPSocket::create(UDPSocket& socket, IpVersion ipVersion) {
	socket.ipVersion = ipVersion;
	socket.descriptor = ::socket(ipVersion == IPV6?AF_INET6:AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (socket.descriptor == -1) {
		string msg = "Could not create socket: ";
		msg+= strerror(errno);
		throw NetworkSocketException(msg);
	}
	#ifdef __APPLE__
		int flag = 1;
		if (setsockopt(socket.descriptor, SOL_SOCKET, SO_NOSIGPIPE, (void*)&flag, sizeof(flag)) == -1) {
			string msg = "Could not set no sig pipe on socket: ";
			msg+= strerror(errno);
			throw NetworkSocketException(msg);
		}
	#endif
}

void UDPSocket::createServerSocket(UDPSocket& socket, const string& ip, const unsigned int port) {
	// create socket
	UDPSocket::create(socket, determineIpVersion(ip));

	try {
		// set non blocked
		socket.setNonBlocked();

		// enable socket reuse port
		int flag = 1;
		if (setsockopt(socket.descriptor, SOL_SOCKET, SO_REUSEOPTION, BUF_CAST(&flag), sizeof(flag)) == -1) {
			string msg = "Could not set reuse port on socket: ";
			msg+= strerror(errno);
			throw NetworkSocketException(msg);
		}

		// bind socket to host
		socket.bind(ip, port);

	} catch (NetworkSocketException &exception) {
		socket.close();
		throw exception;
	}
}

void UDPSocket::createClientSocket(UDPSocket& socket, IpVersion ipVersion) {
	// create socket
	UDPSocket::create(socket, ipVersion);

	try {
		// set non blocked
		socket.setNonBlocked();
	} catch (NetworkSocketException &exception) {
		socket.close();
		throw exception;
	}
}
