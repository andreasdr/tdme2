#include <tdme/network/httpclient/HTTPClient.h>

#include <iomanip>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <tdme/network/httpclient/HTTPClientException.h>
#include <tdme/os/network/Network.h>
#include <tdme/os/network/TCPSocket.h>
#include <tdme/os/network/NetworkSocketClosedException.h>
#include <tdme/utilities/Base64EncDec.h>
#include <tdme/utilities/Character.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>
#include <tdme/utilities/Integer.h>
#include <tdme/utilities/StringTokenizer.h>
#include <tdme/utilities/StringTools.h>

using std::hex;
using std::nouppercase;
using std::ostringstream;
using std::setw;
using std::string;
using std::stringstream;
using std::to_string;
using std::unordered_map;
using std::uppercase;
using std::vector;

using tdme::network::httpclient::HTTPClientException;
using tdme::os::network::Network;
using tdme::os::network::NetworkSocketClosedException;
using tdme::os::network::TCPSocket;
using tdme::utilities::Base64EncDec;
using tdme::utilities::Character;
using tdme::utilities::Console;
using tdme::utilities::Exception;
using tdme::utilities::Integer;
using tdme::utilities::StringTokenizer;
using tdme::utilities::StringTools;

using tdme::network::httpclient::HTTPClient;

const string HTTPClient::HTTP_METHOD_GET = "GET";
const string HTTPClient::HTTP_METHOD_HEAD = "HEAD";
const string HTTPClient::HTTP_METHOD_POST = "POST";
const string HTTPClient::HTTP_METHOD_PUT = "PUT";
const string HTTPClient::HTTP_METHOD_DELETE = "DELETE";

string HTTPClient::urlEncode(const string &value) {
	// see: https://stackoverflow.com/questions/154536/encode-decode-urls-in-c
	ostringstream escaped;
	escaped.fill('0');
	escaped << hex;

	for (string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
		string::value_type c = (*i);

		// Keep alphanumeric and other accepted characters intact
		if (Character::isAlphaNumeric(c) == true || c == '-' || c == '_' || c == '.' || c == '~') {
			escaped << c;
			continue;
		}

		// Any other characters are percent-encoded
		escaped << uppercase;
		escaped << '%' << setw(2) << int((unsigned char) c);
		escaped << nouppercase;
	}

	return escaped.str();
}


string HTTPClient::createHTTPRequestHeaders(const string& hostName, const string& method, const string& relativeUrl, const unordered_map<string, string>& getParameters, const unordered_map<string, string>& postParameters, const string& body) {
	string query;
	for (auto getParameterIt: getParameters) {
		if (query.size() == 0) query+= "?"; else query+="&";
		query+= urlEncode(getParameterIt.first) + "=" + urlEncode(getParameterIt.second);
	}
	auto request =
		string(method + " " + relativeUrl + query + " HTTP/1.1\r\n") +
		string("User-Agent: tdme2-httpclient\r\n") +
		string("Host: " + hostName + "\r\n") +
		string("Connection: close\r\n");
	if (username.empty() == false || password.empty() == false) {
		string base64Pass;
		Base64EncDec::encode(username + ":" + password, base64Pass);
		request+= "Authorization: Basic " + base64Pass + "\r\n";
	}
	if (contentType.size() > 0) {
		request+=
			string("Content-Type: " + contentType + "\r\n");
	}
	if (method == HTTP_METHOD_POST || method == HTTP_METHOD_PUT) {
		string _body;
		if (postParameters.size() > 0) {
			for (auto postParameterIt: postParameters) {
				if (_body.size() >= 0) _body+="&";
				_body+= urlEncode(postParameterIt.first) + "=" + urlEncode(postParameterIt.second);
			}
		} else {
			_body = body;
		}
		request+= "Content-Length: " + to_string(_body.size()) + "\r\n\r\n";
		request+= _body;
	} else {
		request+= "\r\n";
	}
	return request;
}

void HTTPClient::parseHTTPResponseHeaders(stringstream& rawResponse, int16_t& httpStatusCode, vector<string>& httpHeader) {
	string line;
	char lastChar = -1;
	char currentChar;
	while (rawResponse.eof() == false) {
		rawResponse.get(currentChar);
		if (lastChar == '\r' && currentChar == '\n') {
			if (line.size() != 0) {
				httpHeader.push_back(line);
			}
			if (line.size() == 0) break;
			line.clear();
		} else
		if (currentChar != '\r' && currentChar != '\n') {
			line+= currentChar;
		}
		lastChar = currentChar;
	}
	if (httpHeader.size() > 0) {
		StringTokenizer t;
		t.tokenize(httpHeader[0], " ");
		for (auto i = 0; i < 3 && t.hasMoreTokens(); i++) {
			auto token = t.nextToken();
			if (i == 1) {
				httpStatusCode = Integer::parseInt(token);
			}
		}
	}
}

void HTTPClient::reset() {
	url.clear();
	method.clear();
	getParameters.clear();
	postParameters.clear();
	body.clear();
	contentType.clear();

	rawResponse.clear();
	httpStatusCode = -1;
	httpHeader.clear();
}

void HTTPClient::execute() {
	TCPSocket socket;
	try {
		if (StringTools::startsWith(url, "http://") == false) throw HTTPClientException("Invalid protocol");
		auto relativeUrl = StringTools::substring(url, string("http://").size());
		if (relativeUrl.size() == 0) throw HTTPClientException("No URL given");
		auto slashIdx = relativeUrl.find('/');
		auto hostName = relativeUrl;
		if (slashIdx != -1) hostName = StringTools::substring(relativeUrl, 0, slashIdx);
		relativeUrl = StringTools::substring(relativeUrl, hostName.size());

		Console::println("HTTPClient::execute(): Hostname: " + hostName);
		Console::println("HTTPClient::execute(): RelativeUrl: " + relativeUrl);

		Console::print("HTTPClient::execute(): Resolving name to IP: " + hostName + ": ");
		auto ip = Network::getIpByHostName(hostName);
		if (ip.size() == 0) {
			Console::println("HTTPClient::execute(): Failed");
			throw HTTPClientException("Could not resolve host IP by host name");
		}
		Console::println(ip);

		TCPSocket::create(socket, TCPSocket::determineIpVersion(ip));
		socket.connect(ip, 80);
		auto request = createHTTPRequestHeaders(hostName, method, relativeUrl, getParameters, postParameters, body);
		socket.write((void*)request.data(), request.length());

		char rawResponseBuf[16384];
		auto rawResponseBytesRead = 0;
		try {
			for (;true;) {
				auto rawResponseBytesRead = socket.read(rawResponseBuf, sizeof(rawResponseBuf));
				rawResponse.write(rawResponseBuf, rawResponseBytesRead);
			};
		} catch (NetworkSocketClosedException& sce) {
			// end of stream
		}

		parseHTTPResponseHeaders(rawResponse, httpStatusCode, httpHeader);

		Console::println("HTTPClient::execute(): performed HTTP request: HTTP status code: " + to_string(httpStatusCode));

		socket.shutdown();
	} catch (Exception& exception) {
		socket.shutdown();
		Console::println(string("HTTPClient::execute(): performed HTTP request: FAILED: ") + exception.what());
		// rethrow
		throw;
	}

}
