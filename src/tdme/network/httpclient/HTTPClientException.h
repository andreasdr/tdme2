#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/network/httpclient/fwd-tdme.h>
#include <tdme/utilities/ExceptionBase.h>

using tdme::utilities::ExceptionBase;

/**
 * HTTP client exception
 * @author Andreas Drewke
 */
class tdme::network::httpclient::HTTPClientException: public ExceptionBase {
public:
	/**
	 * @brief public constructor
	 * @param &message message exception message
	 */
	HTTPClientException(const string &message) throw();
};
