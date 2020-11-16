#pragma once

#include <tdme/os/filesystem/fwd-tdme.h>

using tdme::os::filesystem::FileSystemInterface;

/**
 * File system singleton class
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::os::filesystem::FileSystem
{
private:
	static FileSystemInterface* standardFileSystem;
	static FileSystemInterface* fileSystem;

public:

	/**
	 * Singleton instance to retrieve file system
	 * will use standard file system by default if not set up different explicitly
	 * @return file system
	 */
	static FileSystemInterface* getInstance();

	/**
	 * Retrieve standard file system
	 * @return standard file system
	 */
	static FileSystemInterface* getStandardFileSystem();

	/**
	 * Set up file system
	 * @param fileSystem file system
	 */
	static void setupFileSystem(FileSystemInterface* fileSystem);

	/**
	 * Unset file system, which means disposing the old one and unset internal file system to use
	 */
	static void unsetFileSystem();

};
