/**
 * @author Andreas Drewke
 * @version $Id: 40a4a8159e048a3026211e19a8dcb2f6a226ed81 $
 */
#pragma once

#include <stdexcept>
#include <string>

#include <tdme/tdme.h>
#include <tdme/utilities/fwd-tdme.h>

using std::runtime_error;
using std::string;

/**
 * Exception base class
 * @author Andreas Drewke
 */
class tdme::utilities::ExceptionBase: public runtime_error {
	public:
		/**
		 * @brief Public constructor
		 * @param message message Exception message
		 */
		ExceptionBase(const string& message) throw();
		~ExceptionBase() throw();
};
