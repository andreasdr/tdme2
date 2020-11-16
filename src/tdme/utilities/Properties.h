#pragma once

#include <map>
#include <string>

#include <tdme/tdme.h>

#include <tdme/tdme.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/os/filesystem/FileSystemException.h>

using std::map;
using std::string;

using tdme::os::filesystem::FileSystemException;
using tdme::os::filesystem::FileSystemInterface;

/**
 * Properties class, which helps out with storeing or loading key value pairs from/to property files
 * @author Andreas Drewke
 */
class tdme::utilities::Properties
{
private:
	map<string, string> properties;

public:
	/**
	 * Public constructor
	 */
	Properties();

	/**
	 * Get property value by key
	 * @param key key
	 * @param defaultValue default value
	 * @return value if found or default value
	 */
	const string& get(const string& key, const string& defaultValue);

	/**
	 * Add property
	 * @param key key
	 * @param value value
	 *
	 */
	void put(const string& key, const string& value);

	/**
	 * Load property file
	 * @param pathName path name
	 * @param fileName file name
	 * @param fileSystem file system to use
	 * @throws tdme::os::filesystem::FileSystemException
	 */
	void load(const string& pathName, const string& fileName, FileSystemInterface* fileSystem = nullptr);

	/**
	 * Store property file
	 * @param pathName path name
	 * @param fileName file name
	 * @param fileSystem file system to use
	 * @throws tdme::os::filesystem::FileSystemException
	 */
	void store(const string& pathName, const string& fileName, FileSystemInterface* fileSystem = nullptr);

};
