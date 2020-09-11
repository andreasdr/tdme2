#include <string.h>

#include <string>

#include <tdme/tdme.h>

#if defined(_WIN32)
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#define SHUT_RDWR SD_BOTH	
#else
	#include <errno.h>
	#include <fcntl.h>
	#include <unistd.h>
	#include <netinet/in.h>
	#include <sys/socket.h>
	#include <arpa/inet.h>
#endif

#include <tdme/os/network/NetworkSocket.h>

using std::to_string;

using tdme::os::network::NetworkSocket;

NetworkSocket& NetworkSocket::operator=(NetworkSocket& socket) {
	descriptor = socket.descriptor;
	ip = socket.ip;
	ipVersion = socket.ipVersion;
	return *this;
}

NetworkSocket::NetworkSocket() : descriptor(-1), ip("0.0.0.0"), port(0), ipVersion(IpVersion::IPV4) {
}

NetworkSocket::~NetworkSocket() {
}

const std::string& NetworkSocket::getAddress() {
	return ip;
}

const unsigned int NetworkSocket::getPort() {
	return port;
}

void NetworkSocket::shutdown() {
	if (descriptor != -1) ::shutdown(descriptor, SHUT_RDWR);
}

void NetworkSocket::bind(const std::string& ip, const unsigned int port) {
	// determine IP version
	ipVersion = determineIpVersion(ip);

	// socket address in setup
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
				sinIPV4.sin_addr.s_addr = inet_addr(ip.c_str());
				sin = &sinIPV4;
			}
			break;
		case IPV6:
			{
				sinLen = sizeof(sinIPV6);
				memset(&sinIPV6, 0, sinLen);
				sinIPV6.sin6_family = AF_INET6;
				sinIPV6.sin6_port = htons(port);
				inet_pton(AF_INET6, ip.c_str(), &sinIPV6.sin6_addr);
				sin = &sinIPV6;
			}
			break;
	}

	// bind
	if (::bind(descriptor, (const struct sockaddr*)sin, sinLen) == -1) {
		std::string msg = "Could not bind socket: ";
		#if defined(_WIN32)
			msg+= to_string(WSAGetLastError());
		#else
			msg+= strerror(errno);
		#endif
		throw NetworkSocketException(msg);
	}

	// set address
	this->ip = ip;
	this->port = port;
}

void NetworkSocket::setNonBlocked() {
	#if defined(_WIN32)
		long unsigned int mode = 1;
		if (ioctlsocket(descriptor, FIONBIO, &mode) != 0) {
			std::string msg = "Could not set socket non blocked: ";
			msg+= to_string(WSAGetLastError());
			throw NetworkSocketException(msg);
		}
	#else
		// get the server socket file descriptor control settings
		int fdc = fcntl(descriptor, F_GETFL, 0);
		if (fdc == -1) {
			std::string msg = "Could not get socket file descriptor settings: ";
			msg+= strerror(errno);
			throw NetworkSocketException(msg);
		}

		// make the socket non blocked
		if (fcntl(descriptor, F_SETFL, fdc | O_NONBLOCK) == -1) {
			std::string msg = "Could not set socket non blocked: ";
			msg+= strerror(errno);
			throw NetworkSocketException(msg);
		}
	#endif
}

void NetworkSocket::close() {
	#if defined(_WIN32)
		::closesocket(descriptor);
	#else
		::close(descriptor);
	#endif
	descriptor = -1;
}

NetworkSocket::IpVersion NetworkSocket::determineIpVersion(const string& ip) {
	return ip.find(':') != std::string::npos?IpVersion::IPV6:IpVersion::IPV4;
}

#if defined(__MINGW32__) && !defined(__MINGW64__)
	// see: https://stackoverflow.com/questions/15370033/how-to-use-inet-pton-with-the-mingw-compiler
	#define NS_INADDRSZ  4
	#define NS_IN6ADDRSZ 16
	#define NS_INT16SZ   2

	// author: Paul Vixie, 1996.
	int inet_pton4(const char* src, void* dst) {
		uint8_t tmp[NS_INADDRSZ], *tp;

		int saw_digit = 0;
		int octets = 0;
		*(tp = tmp) = 0;

		int ch;
		while ((ch = *src++) != '\0') {
			if (ch >= '0' && ch <= '9') {
				uint32_t n = *tp * 10 + (ch - '0');

				if (saw_digit && *tp == 0)
					return 0;

				if (n > 255)
					return 0;

				*tp = n;
				if (!saw_digit) {
					if (++octets > 4)
						return 0;
					saw_digit = 1;
				}
			} else if (ch == '.' && saw_digit) {
				if (octets == 4)
					return 0;
				*++tp = 0;
				saw_digit = 0;
			} else
				return 0;
		}
		if (octets < 4)
			return 0;

		memcpy(dst, tmp, NS_INADDRSZ);

		return 1;
	}

	// author: Paul Vixie, 1996.
	int inet_pton6(int af, const char* src, void* dst) {
		static const char xdigits[] = "0123456789abcdef";
		uint8_t tmp[NS_IN6ADDRSZ];

		uint8_t *tp = (uint8_t*) memset(tmp, '\0', NS_IN6ADDRSZ);
		uint8_t *endp = tp + NS_IN6ADDRSZ;
		uint8_t *colonp = NULL;

		/* Leading :: requires some special handling. */
		if (*src == ':') {
			if (*++src != ':')
				return 0;
		}

		const char *curtok = src;
		int saw_xdigit = 0;
		uint32_t val = 0;
		int ch;
		while ((ch = tolower(*src++)) != '\0') {
			const char *pch = strchr(xdigits, ch);
			if (pch != NULL) {
				val <<= 4;
				val |= (pch - xdigits);
				if (val > 0xffff)
					return 0;
				saw_xdigit = 1;
				continue;
			}
			if (ch == ':') {
				curtok = src;
				if (!saw_xdigit) {
					if (colonp)
						return 0;
					colonp = tp;
					continue;
				} else if (*src == '\0') {
					return 0;
				}
				if (tp + NS_INT16SZ > endp)
					return 0;
				*tp++ = (uint8_t) (val >> 8) & 0xff;
				*tp++ = (uint8_t) val & 0xff;
				saw_xdigit = 0;
				val = 0;
				continue;
			}
			if (ch == '.' && ((tp + NS_INADDRSZ) <= endp)
					&& inet_pton4(curtok, (char*) tp) > 0) {
				tp += NS_INADDRSZ;
				saw_xdigit = 0;
				break; /* '\0' was seen by inet_pton4(). */
			}
			return 0;
		}
		if (saw_xdigit) {
			if (tp + NS_INT16SZ > endp)
				return 0;
			*tp++ = (uint8_t) (val >> 8) & 0xff;
			*tp++ = (uint8_t) val & 0xff;
		}
		if (colonp != NULL) {
			/*
			 * Since some memmove()'s erroneously fail to handle
			 * overlapping regions, we'll do the shift by hand.
			 */
			const int n = tp - colonp;

			if (tp == endp)
				return 0;

			for (int i = 1; i <= n; i++) {
				endp[-i] = colonp[n - i];
				colonp[n - i] = 0;
			}
			tp = endp;
		}
		if (tp != endp)
			return 0;

		memcpy(dst, tmp, NS_IN6ADDRSZ);

		return 1;
	}

	/*
	 * $OpenBSD: strlcpy.c,v 1.11 2006/05/05 15:27:38 millert Exp $
	 * Copyright (c) 1998 Todd C. Miller <Todd.Miller@courtesan.com>
	 *
	 * Permission to use, copy, modify, and distribute this software for any
	 * purpose with or without fee is hereby granted, provided that the above
	 * copyright notice and this permission notice appear in all copies.
	 *
	 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
	 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
	 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
	 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
	 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
	 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
	 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
	 */
	size_t strlcpy(char* __restrict dst, const char* __restrict src, size_t siz)
	{
		char *d = dst;
		const char *s = src;
		size_t n = siz;

		/* Copy as many bytes as will fit */
		if (n != 0) {
			while (--n != 0) {
				if ((*d++ = *s++) == '\0')
					break;
			}
		}

		/* Not enough room in dst, add NUL and traverse rest of src */
		if (n == 0) {
			if (siz != 0)
				*d = '\0'; /* NUL-terminate dst */
			while (*s++)
				;
		}

		//
		return (s - src - 1); /* count does not include NUL */
	}

	// author: Paul Vixie, 1996.
	char* inet_ntop4(const void* src, char* dst, size_t size) {
		static const char fmt[128] = "%u.%u.%u.%u";
		char tmp[sizeof "255.255.255.255"];
		int l;

		// l = snprintf(tmp, sizeof(tmp), fmt, src[0], src[1], src[2], src[3]); // ****
		std::sprintf(tmp, fmt, ((uint8_t*)src)[0], ((uint8_t*)src)[1], ((uint8_t*)src)[2], ((uint8_t*)src)[3]); // **** vc++ does not have snprintf
		if (l <= 0 || (socklen_t) l >= size) {
			return (NULL);
		}
		strlcpy(dst, tmp, size);
		return (dst);
	}

	// author: Paul Vixie, 1996.
	char* inet_ntop6(int af, const void* src, char* dst, size_t size)
	{
		/*
		 * Note that int32_t and int16_t need only be "at least" large enough
		 * to contain a value of the specified size.  On some systems, like
		 * Crays, there is no such thing as an integer variable with 16 bits.
		 * Keep this in mind if you think this function should have been coded
		 * to use pointer overlays.  All the world's not a VAX.
		 */
		char tmp[sizeof "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255"], *tp;
		struct {
			int base, len;
		} best, cur;
		u_int words[NS_IN6ADDRSZ / NS_INT16SZ];
		int i;

		/*
		 * Preprocess:
		 *	Copy the input (bytewise) array into a wordwise array.
		 *	Find the longest run of 0x00's in src[] for :: shorthanding.
		 */
		memset(words, '\0', sizeof words);
		for (i = 0; i < NS_IN6ADDRSZ; i++)
			words[i / 2] |= (((uint8_t*)src)[i] << ((1 - (i % 2)) << 3));
		best.base = -1;
		best.len = 0;
		cur.base = -1;
		cur.len = 0;
		for (i = 0; i < (NS_IN6ADDRSZ / NS_INT16SZ); i++) {
			if (words[i] == 0) {
				if (cur.base == -1)
					cur.base = i, cur.len = 1;
				else
					cur.len++;
			} else {
				if (cur.base != -1) {
					if (best.base == -1 || cur.len > best.len)
						best = cur;
					cur.base = -1;
				}
			}
		}
		if (cur.base != -1) {
			if (best.base == -1 || cur.len > best.len)
				best = cur;
		}
		if (best.base != -1 && best.len < 2)
			best.base = -1;

		/*
		 * Format the result.
		 */
		tp = tmp;
		for (i = 0; i < (NS_IN6ADDRSZ / NS_INT16SZ); i++) {
			/* Are we inside the best run of 0x00's? */
			if (best.base != -1 && i >= best.base && i < (best.base + best.len)) {
				if (i == best.base)
					*tp++ = ':';
				continue;
			}
			/* Are we following an initial run of 0x00s or any real hex? */
			if (i != 0)
				*tp++ = ':';
			/* Is this address an encapsulated IPv4? */
			if (i == 6 && best.base == 0
					&& (best.len == 6 || (best.len == 7 && words[7] != 0x0001)
							|| (best.len == 5 && words[5] == 0xffff))) {
				if (!inet_ntop4(src + 12, tp, sizeof tmp - (tp - tmp)))
					return (NULL);
				tp += strlen(tp);
				break;
			}
			tp += std::sprintf(tp, "%x", words[i]); // ****
		}
		/* Was it a trailing run of 0x00's? */
		if (best.base != -1
				&& (best.base + best.len) == (NS_IN6ADDRSZ / NS_INT16SZ))
			*tp++ = ':';
		*tp++ = '\0';

		/*
		 * Check for overflow, copy, and we're done.
		 */
		if ((socklen_t) (tp - tmp) > size) {
			return (NULL);
		}
		strcpy(dst, tmp);
		return (dst);
	}

#endif
