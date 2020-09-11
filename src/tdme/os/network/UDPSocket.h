#pragma once

#include <string>

#include <stddef.h>

#if defined(_WIN32) && defined(_MSC_VER)
	#define ssize_t int
#endif

#include <tdme/os/network/fwd-tdme.h>
#include <tdme/os/network/NetworkIOException.h>
#include <tdme/os/network/NetworkSocket.h>
#include <tdme/os/network/NetworkSocketException.h>

using std::string;

using tdme::os::network::NetworkIOException;
using tdme::os::network::NetworkSocket;
using tdme::os::network::NetworkSocketException;

/**
 * Class representing a UDP socket
 * @author Andreas Drewke
 */
class tdme::os::network::UDPSocket : public NetworkSocket {
public:
	/**
	 * @brief public destructor
	 */
	virtual ~UDPSocket();

	/**
	 * @brief reads a datagram from socket
	 * @param from from host
	 * @param port from port
	 * @param buf buf
	 * @param bytes buf size
	 * @throws tdme::os::network::NetworkIOException
	 * @return datagram size or -1 if read would block
	 */
	ssize_t read(string& from, unsigned int& port, void* buf, const size_t bytes);

	/**
	 * @brief writes up to "bytes" bytes to socket
	 * @throws tdme::os::network::NetworkIOException
	 * @param to to host
	 * @param port to port
	 * @param buf buf
	 * @param bytes buf size
	 * @throws tdme::os::network::NetworkIOException
	 * @return datagram bytes written or -1 if write would block
	 */
	ssize_t write(const string& to, const unsigned int port, void* buf, const size_t bytes);

	/**
	 * @brief creates a udp socket
	 * @param socket socket
	 * @param ipVersion IP version
	 * @throws tdme::os::network::NetworkSocketException
	 */
	static void create(UDPSocket& socket, IpVersion ipVersion);

	/**
	 * @brief creates a udp server socket
	 * @param socket socket
	 * @param ip ip
	 * @param port port
	 * @throws tdme::os::network::NetworkSocketException
	 */
	static void createServerSocket(UDPSocket& socket, const std::string& ip, const unsigned int port);

	/**
	 * @brief creates a udp client socket
	 * @param socket socket
	 * @param ipVersion IP version
	 * @throws tdme::os::network::NetworkSocketException
	 */
	static void createClientSocket(UDPSocket& socket, IpVersion ipVersion);
};

