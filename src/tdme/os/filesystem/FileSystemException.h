#pragma once

#include <tdme/tdme.h>
#include <tdme/os/filesystem/fwd-tdme.h>
#include <tdme/utilities/fwd-tdme.h>
#include <tdme/utilities/ExceptionBase.h>

using tdme::utilities::ExceptionBase;

/**
 * File system exception
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::os::filesystem::FileSystemException: public ExceptionBase
{
public:

	/**
	 * @brief Public constructor
	 * @param message message Exception message
	 */
	FileSystemException(const string& message) throw();

};
