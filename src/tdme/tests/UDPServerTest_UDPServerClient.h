#pragma once

#include <sstream>

#include <tdme/tdme.h>
#include <tdme/network/udpserver/UDPServer.h>
#include <tdme/network/udpserver/UDPServerClient.h>
#include <tdme/utilities/Exception.h>

#include "UDPServerTest_UDPServer.h"

using std::stringstream;

using tdme::network::udpserver::UDPServer;
using tdme::network::udpserver::UDPServerClient;
using tdme::utilities::Exception;

class EchoUDPServerClient : public UDPServerClient {
	friend class EchoUDPServer;

public:
	EchoUDPServerClient(const uint32_t clientId, const std::string& ip, const unsigned int port);

protected:
	virtual ~EchoUDPServerClient();

	virtual void onRequest(stringstream* frame, const uint32_t messageId, const uint8_t retries);

	void onInit();
	void onClose();
	void onCustom(const string& type);
};
