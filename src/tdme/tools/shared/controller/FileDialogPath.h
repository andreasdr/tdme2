
#pragma once

#include <string>

#include <tdme/tools/shared/controller/fwd-tdme.h>

using std::string;


/**
 * File dialog path
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::shared::controller::FileDialogPath final
{
private:
	string path;

public:
	/**
	 * Public constructor
	 */
	FileDialogPath();

	/**
	 * @return path
	 */
	const string& getPath();

	/**
	 * Set path
	 * @param path path
	 */
	void setPath(const string& path);

};
