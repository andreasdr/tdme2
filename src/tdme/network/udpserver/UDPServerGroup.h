#pragma once

#include <tdme/network/udpserver/fwd-tdme.h>
#include <tdme/network/udpserver/ServerGroup.h>
#include <tdme/network/udpserver/UDPServer.h>
#include <tdme/network/udpserver/UDPServerClient.h>

using tdme::network::udpserver::ServerGroup;
using tdme::network::udpserver::UDPServer;
using tdme::network::udpserver::UDPServerClient;

/**
 * UDP server group class
 * @author Andreas Drewke
 */
class tdme::network::udpserver::UDPServerGroup : public ServerGroup<UDPServer, UDPServerClient, UDPServerGroup> {
public:
	/**
	 * @brief Creates a frame to be used with send
	 * @return frame to be send
	 */
	// stringstream* createFrame();

	/**
	 * @brief Sends a frame to client, takes over ownership of frame
	 * @param frame frame data
	 * @param safe safe, requires ack and retransmission
	 */
	// void send(stringstream* frame, bool safe = true);
};

