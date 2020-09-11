#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/os/network/fwd-tdme.h>
#include <tdme/os/network/NetworkSocketException.h>

using std::string;

using tdme::os::network::NetworkSocketException;

/**
 * Base class of network sockets
 * @author Andreas Drewke
 */
class tdme::os::network::NetworkSocket {
	friend class KernelEventMechanism;

public:
	enum IpVersion {IPV4, IPV6};

	/**
	 * @brief copy operator
	 */
	NetworkSocket& operator=(NetworkSocket& socket);

	/**
	 * Protected constructor
	 */
	NetworkSocket();

	/**
	 * @brief public destructor
	 */
	virtual ~NetworkSocket();

	/**
	 * @brief returns the end points ip address
	 * @return end point ip address
	 */
	const string& getAddress();

	/**
	 * @brief returns the end points port
	 * @return end point port
	 */
	const unsigned int getPort();

	/**
	 * @brief shuts socket down for reading and writing
	 */
	void shutdown();

	/**
	 * Binds a socket to local ip and port
	 * @param ip ip
	 * @param port port
	 * @throws tdme::os::network::NetworkSocketException
	 */
	void bind(const string& ip, const unsigned int port);

	/**
	 * @brief sets the socket non blocked
	 * @throws tdme::os::network::NetworkSocketException
	 */
	void setNonBlocked();

	/**
	 * Closes the socket
	 */
	void close();

	/**
	 * Determine IP version
	 * @param ip ip
	 * @returns ip version
	 */
	static IpVersion determineIpVersion(const string& ip);

protected:
	IpVersion ipVersion;
	int descriptor;
	string ip;
	unsigned int port;
};

// MINGW: Have some missing posix functions
#if defined(__MINGW32__) && !defined(__MINGW64__)
	// TODO: move those somewhere else as they are not only socket specific
	#define inet_pton inet_pton6
	#define inet_ntop inet_ntop6
	size_t strlcpy(char* __restrict dst, const char* __restrict src, size_t siz);
	int inet_pton4(const char* src, void* dst);
	int inet_pton6(int af, const char* src, void* dst);
	char* inet_ntop4(const void* src, char* dst, size_t size);
	char* inet_ntop6(int af, const void* src, char* dst, size_t size);
#endif
