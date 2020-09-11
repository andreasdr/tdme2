#include <exception>
#include <typeinfo>
#include <string>

#include <tdme/utilities/fwd-tdme.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>
#include <tdme/utilities/RTTI.h>
#include <tdme/network/udpserver/fwd-tdme.h>
#include <tdme/network/udpserver/ServerClient.h>
#include <tdme/network/udpserver/ServerGroup.h>
#include <tdme/network/udpserver/ServerWorkerThread.h>
#include <tdme/network/udpserver/ServerWorkerThreadPool.h>

using std::string;
using std::to_string;

using tdme::utilities::Console;
using tdme::utilities::Exception;
using tdme::utilities::RTTI;
using tdme::network::udpserver::ServerClient;
using tdme::network::udpserver::ServerGroup;
using tdme::network::udpserver::ServerWorkerThread;
using tdme::network::udpserver::ServerWorkerThreadPool;

ServerWorkerThread::ServerWorkerThread(const unsigned int id, ServerWorkerThreadPool* threadPool) :
	Thread("nioworkerthread"),
	id(id),
	threadPool(threadPool) {
	//
}

ServerWorkerThread::~ServerWorkerThread() {
}

void ServerWorkerThread::run() {
	Console::println("ServerWorkerThread[" + to_string(id) + "]::run(): start");

	// wait on startup barrier
	threadPool->startUpBarrier->wait();

	ServerRequest* request;
	while((request = threadPool->getElement()) != NULL) {
		// get request parameter
		ServerRequest::RequestType requestType = request->getRequestType();
		ServerClient* client = NULL;
		ServerGroupBase* group = NULL;

		// handle request types
		switch(requestType) {
			case(ServerRequest::REQUESTTYPE_CLIENT_REQUEST): {
				client = (ServerClient*)request->getObject();
				stringstream* frame = request->getMessageFrame();
				uint32_t messageId = request->getMessageId();
				uint8_t retries = request->getMessageRetries();

				// handle request
				try {
					client->onRequest(frame, messageId, retries);
				} catch(Exception& exception) {
					Console::println(
						"ServerWorkerThread[" +
						to_string(id) +
						"]::run(): client: request: " +
						(RTTI::demangle(typeid(exception).name())) +
						": " +
						(exception.what())
					);

					// unhandled exception, so shutdown the client
					client->shutdown();
				}

				// delete stream
				delete frame;

				//
				break;
			}
			case(ServerRequest::REQUESTTYPE_CLIENT_INIT): {
				client = (ServerClient*)request->getObject();
				// handle close
				try {
					client->onInit();
				} catch(Exception& exception) {
					Console::println(
						"ServerWorkerThread[" +
						to_string(id) +
						"]::run(): client: init: " +
						(RTTI::demangle(typeid(exception).name())) +
						": " +
						(exception.what())
					);
				}
				break;
			}
			case(ServerRequest::REQUESTTYPE_CLIENT_CLOSE): {
				client = (ServerClient*)request->getObject();
				// handle close
				try {
					client->onClose();
				} catch(Exception& exception) {
					Console::println(
						"ServerWorkerThread[" +
						to_string(id) +
						"]::run(): client: close: " +
						(RTTI::demangle(typeid(exception).name())) +
						": " +
						(exception.what())
					);
				}
				break;
			}
			case(ServerRequest::REQUESTTYPE_CLIENT_CUSTOM): {
				client = (ServerClient*)request->getObject();
				// handle close
				try {
					client->onCustom(request->getCustomEvent());
				} catch(Exception& exception) {
					Console::println(
						"ServerWorkerThread[" +
						to_string(id) +
						"]::run(): client: custom: " +
						(RTTI::demangle(typeid(exception).name())) +
						": " +
						(exception.what())
					);
				}
				break;
			}
			case(ServerRequest::REQUESTTYPE_GROUP_INIT): {
				group = (ServerGroupBase*)request->getObject();
				// handle close
				try {
					group->onInit();
				} catch(Exception& exception) {
					Console::println(
						"ServerWorkerThread[" +
						to_string(id) +
						"]::run(): group: init: " +
						(RTTI::demangle(typeid(exception).name())) +
						": " +
						(exception.what())
					);
				}
				break;
			}
			case(ServerRequest::REQUESTTYPE_GROUP_CLOSE): {
				group = (ServerGroupBase*)request->getObject();
				// handle close
				try {
					group->onClose();
				} catch(Exception& exception) {
					Console::println(
						"ServerWorkerThread[" +
						to_string(id) +
						"]::run(): group: close: " +
						(RTTI::demangle(typeid(exception).name())) +
						": " +
						(exception.what())
					);
				}
				break;
			}
			case(ServerRequest::REQUESTTYPE_GROUP_CUSTOM): {
				group = (ServerGroupBase*)request->getObject();
				// handle close
				try {
					group->onCustomEvent(request->getCustomEvent());
				} catch(Exception& exception) {
					Console::println(
						"ServerWorkerThread[" +
						to_string(id) +
						"]::run(): group: custom: " +
						(RTTI::demangle(typeid(exception).name())) +
						": " +
						(exception.what())
					);
				}
				break;
			}
		}

		// release reference
		if (client != NULL) client->releaseReference();
		if (group != NULL) group->releaseReference();

		// delete request
		delete(request);
	}

	//
	Console::println("ServerWorkerThread[" + to_string(id) + "]::run(): done");
}
