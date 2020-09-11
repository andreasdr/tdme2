#pragma once

#include <fstream>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/network/httpclient/fwd-tdme.h>
#include <tdme/network/httpclient/HTTPClientException.h>
#include <tdme/os/network/NetworkException.h>
#include <tdme/os/threading/Mutex.h>
#include <tdme/os/threading/Thread.h>

using std::ifstream;
using std::string;
using std::vector;

using tdme::network::httpclient::HTTPClientException;
using tdme::os::network::NetworkException;
using tdme::os::threading::Mutex;
using tdme::os::threading::Thread;

/**
 * HTTP download client
 * @author Andreas Drewke
 */
class tdme::network::httpclient::HTTPDownloadClient {

private:
	string url;
	string file;
	string username;
	string password;

	int16_t httpStatusCode { -1 };
	vector<string> httpHeader;

	Thread* downloadThread { nullptr };
	Mutex downloadThreadMutex;
	bool haveHeaders { false };
	bool haveContentSize { false };
	uint64_t headerSize { 0LL };
	uint64_t contentSize { 0LL };
	volatile bool finished { true };
	volatile float progress { 0.0f };

	/**
	 * Create HTTP request headers
	 * @param hostName host name
	 * @param relativeUrl url relative to server root
	 */
	string createHTTPRequestHeaders(const string& hostName, const string& relativeUrl);

	/**
	 * Parse HTTP response headers
	 * @param rawResponse raw response
	 * @param httpStatusCode HTTP status code
	 * @param httpHeader HTTP header
	 * @return http header size or 0 if not yet completely submitted
	 */
	uint64_t parseHTTPResponseHeaders(ifstream& rawResponse, int16_t& httpStatusCode, vector<string>& httpHeader);

public:

	static const constexpr int16_t HTTP_STATUSCODE_OK { 200 };

	/**
	 * Public constructor
	 */
	HTTPDownloadClient();

	/**
	 * Get username
	 * @return username
	 */
	inline const string& getUsername() {
		return username;
	}

	/**
	 * Set username
	 * @param username user name
	 */
	inline void setUsername(const string& username) {
		this->username = username;
	}

	/**
	 * Get password
	 * @return password
	 */
	inline const string& getPassword() {
		return password;
	}

	/**
	 * Set password
	 * @param password password
	 */
	inline void setPassword(const string& password) {
		this->password = password;
	}

	/**
	 * Get URL
	 * @return URL
	 */
	inline const string& getURL() {
		return url;
	}

	/**
	 * Set URL
	 * @param url URL
	 */
	inline void setURL(const string& url) {
		this->url = url;
	}

	/**
	 * Get file to download to
	 * @return file
	 */
	inline const string& getFile() {
		return file;
	}

	/**
	 * Set file to download to
	 * @param file file
	 */
	inline void setFile(const string& file) {
		this->file = file;
	}

	/**
	 * Reset this HTTP client
	 */
	void reset();

	/**
	 * Starts the HTTP download to file
	 */
	void start();

	/**
	 * @return HTTP status code
	 */
	inline int16_t getStatusCode() {
		return httpStatusCode;
	}

	/**
	 * @return HTTP response headers
	 */
	inline const vector<string>& getResponseHeaders() {
		return httpHeader;
	}

	inline bool isFinished() {
		return finished;
	}

	/**
	 * @return progress
	 */
	inline float getProgress() {
		return progress;
	}

	/**
	 * Cancel a started download
	 */
	void cancel();

	/**
	 * Wait until underlying thread has finished
	 */
	void join();

};
