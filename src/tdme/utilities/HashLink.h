#pragma once

#include <string>

#include <tdme/utilities/fwd-tdme.h>

using std::string;

/**
 * Hash link class
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::utilities::HashLink final
{
public:

	/**
	 * Execute hash link script
	 * @param pathName path name
	 * @return fileName file name
	 */
	static bool execute(const string& pathName, const string& fileName);

};
