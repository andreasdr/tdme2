#include <tdme/os/network/Network.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>
#include <tdme/network/httpclient/HTTPDownloadClient.h>
#include <tdme/os/threading/Thread.h>

using std::to_string;

using tdme::network::httpclient::HTTPDownloadClient;
using tdme::os::network::Network;
using tdme::os::threading::Thread;
using tdme::utilities::Console;
using tdme::utilities::Exception;

int main(int argc, char *argv[]) {
	Network::initialize();

	try {
		HTTPDownloadClient httpDownloadClient;
		httpDownloadClient.setFile("tdme2-freebsd-x64_2018-07-07-03-29.tgz");
		httpDownloadClient.setURL("http://drewke.net/tdme2/tdme2-freebsd-x64_2018-07-07-03-29.tgz");
		httpDownloadClient.start();
		Console::println("Download started");
		while (httpDownloadClient.isFinished() == false) {
			Console::println("Download progress: " + to_string(httpDownloadClient.getProgress() * 100.0f));
			Thread::sleep(1000LL);
		}
		httpDownloadClient.join();
		Console::println("Download finished");
	} catch (Exception& exception) {
		Console::println(string("Fail: ") + exception.what());
	}
}

