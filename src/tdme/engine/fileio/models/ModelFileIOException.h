#pragma once

#include <tdme/tdme.h>
#include <tdme/engine/fileio/models/fwd-tdme.h>
#include <tdme/utilities/fwd-tdme.h>
#include <tdme/utilities/ExceptionBase.h>

using tdme::utilities::ExceptionBase;

/**
 * Model file IO exception
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::fileio::models::ModelFileIOException: public ExceptionBase
{
public:

	/**
	 * @brief Public constructor
	 * @param message message Exception message
	 */
	ModelFileIOException(const string& message) throw();

};
