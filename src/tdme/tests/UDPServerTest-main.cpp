#include <signal.h>

#include <sstream>
#include <string>

#include "UDPServerTest_UDPServer.h"
#include "UDPServerTest_UDPServerClient.h"

#include <tdme/os/network/Network.h>
#include <tdme/os/threading/Thread.h>
#include <tdme/os/threading/Queue.h>
#include <tdme/utilities/Console.h>

using std::string;
using std::stringstream;

using tdme::os::network::Network;
using tdme::os::threading::Thread;
using tdme::os::threading::Queue;
using tdme::utilities::Console;

class ServerBroadcaster : public Thread {
public:
	ServerBroadcaster(EchoUDPServer *server) : Thread("broadcaster"), server(server), time(0) {}

	virtual ~ServerBroadcaster() {
	}

	virtual void run() {
		while (isStopRequested() == false) {
			Thread::sleep(1000);
			if (++time == 5) {
				// get client key list
				EchoUDPServer::ClientKeySet clientKeySet = server->getClientKeySet();
				for (EchoUDPServer::ClientKeySet::iterator i = clientKeySet.begin(); i != clientKeySet.end(); ++i) {
					EchoUDPServerClient* client = static_cast<EchoUDPServerClient*>(server->getClientByKey(*i));
					if (client != NULL) {
						stringstream* frame = client->createFrame();
						*frame << "broadcast test";
						client->send(frame, true);
						client->releaseReference();
					}
				}
				time = 0;
			}
		}
	}
private:
	EchoUDPServer *server;
	unsigned int time;
};

EchoUDPServer* server = NULL;
ServerBroadcaster* bc = NULL;

void sigHandlerINT(int signal) {
	Console::println("Interrupt signal catched");
	if (server != NULL) {
		// stop broad caster
		server->stop();
	}
}

int main(int argc, char *argv[]) {
	// install SIGNINT handler
	if (signal(SIGINT, sigHandlerINT) == SIG_ERR) {
		Console::println("Can't install signal handler for SIGINT");
	}

	// initialize network module
	Network::initialize();

	// start echo server
	server = new EchoUDPServer("127.0.0.1", 10000, 100);
	bc = new ServerBroadcaster(server);
	bc->start();
	server->start();

	// wait until server has been stopped
	server->join();

	// stop and wait for broad caster
	bc->stop();
	bc->join();

	// delete server and broadcaster
	delete server;
	delete bc;

	return 0;
}
