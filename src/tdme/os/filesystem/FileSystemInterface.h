#pragma once

#include <vector>
#include <string>

#include <tdme/tdme.h>
#include <tdme/os/filesystem/fwd-tdme.h>
#include <tdme/utilities/fwd-tdme.h>

#include <tdme/os/filesystem/FileSystemException.h>

using std::vector;
using std::string;

using tdme::os::filesystem::FileNameFilter;
using tdme::os::filesystem::FileSystemException;

/** 
 * File system interface
 * @author Andreas Drewke
 * @version $Id$
 */
struct tdme::os::filesystem::FileSystemInterface
{
	/**
	 * Destructor
	 */
	virtual ~FileSystemInterface() {}

	/**
	 * Get file name
	 * @param pathName path name
	 * @param fileName file name
	 * @return complete filename with path and file
	 * @throws tdme::os::filesystem::FileSystemException
	 */
	virtual const string getFileName(const string& pathName, const string& fileName) = 0;

	/**
	 * Return file size of given file
	 * @param pathName path name
	 * @param fileName file name
	 * @return file size
	 */
	virtual uint64_t getFileSize(const string& pathName, const string& fileName) = 0;

	/**
	 * Get content as string
	 * @param pathName path name
	 * @param fileName file name
	 * @return string
	 * @throws tdme::os::filesystem::FileSystemException
	 */
	virtual const string getContentAsString(const string& pathName, const string& fileName) = 0;

	/**
	 * Set content from string
	 * @param pathName path name
	 * @param fileName file name
	 * @param content content string
	 * @throws tdme::os::filesystem::FileSystemException
	 */
	virtual void setContentFromString(const string& pathName, const string& fileName, const string& content) = 0;

	/** 
	 * Get file content
	 * @param pathName path name
	 * @param fileName file name
	 * @param content content vector
	 * @throws tdme::os::filesystem::FileSystemException
	 */
	virtual void getContent(const string& pathName, const string& fileName, vector<uint8_t>& content) = 0;

	/** 
	 * Set file content
	 * @param pathName path name
	 * @param fileName file name
	 * @param content content vector
	 * @throws tdme::os::filesystem::FileSystemException
	 */
	virtual void setContent(const string& pathName, const string& fileName, const vector<uint8_t>& content) = 0;

	/**
	 * Get file content as string array
	 * @param pathName path name
	 * @param fileName file name
	 * @param content content vector
	 * @throws tdme::os::filesystem::FileSystemException
	 */
	virtual void getContentAsStringArray(const string& pathName, const string& fileName, vector<string>& content) = 0;

	/**
	 * Set file content as string array
	 * @param pathName path name
	 * @param fileName file name
	 * @param content string array
	 * @return byte array
	 * @throws tdme::os::filesystem::FileSystemException
	 */
	virtual void setContentFromStringArray(const string& pathName, const string& fileName, const vector<string>& content) = 0;

	/**
	 * List files for given path and filter by a file name filter if not null 
	 * @param pathName path name
	 * @param files files
	 * @param filter filter or null, this filter can be created on stack as ownership will not be taken over
	 * @param addDrives add drives to list(applies to Microsoft Windows only)
	 * @return file names 
	 */
	virtual void list(const string& pathName, vector<string>& files, FileNameFilter* filter = nullptr, bool addDrives = false) = 0;

	/**
	 * Check if file is a path
	 * @param pathName path name
	 * @return if file is a path
	 */
	virtual bool isPath(const string& pathName) = 0;

	/**
	 * Check if file is a drive (applies to Microsoft Windows only)
	 * @param pathName path name
	 * @return if file is a drive
	 */
	virtual bool isDrive(const string& pathName) = 0;

	/**
	 * Check if file exists
	 * @param fileName file name
	 * @return bool if file exists
	 */
	virtual bool fileExists(const string& fileName) = 0;

	/**
	 * Returns if file is a executable file
	 * @param pathName path name
	 * @param fileName file name
	 * @return is executable
	 */
	virtual bool isExecutable(const string& pathName, const string& fileName) = 0;

	/**
	 * Set up file to be an executable file
	 * @param pathName path name
	 * @param fileName file name
	 * @return success
	 * @throws tdme::os::filesystem::FileSystemException
	 */
	virtual void setExecutable(const string& pathName, const string& fileName) = 0;

	/**
	 * Get canonical path name
	 * @param pathName path name
	 * @param fileName file name
	 * @return canonical path
	 */
	virtual const string getCanonicalPath(const string& pathName, const string& fileName) = 0;

	/**
	 * Get current working path name
	 * @return current working path
	 */
	virtual const string getCurrentWorkingPathName() = 0;

	/**
	 * Get path name
	 * @param fileName file name
	 * @return canonical path
	 */
	virtual const string getPathName(const string& fileName) = 0;

	/**
	 * Get file name
	 * @param fileName file name
	 * @return canonical path
	 */
	virtual const string getFileName(const string& fileName) = 0;

	/**
	 * Create path
	 * @param pathName path name
	 */
	virtual void createPath(const string& pathName) = 0;

	/**
	 * Remove path
	 * @param pathName path name
	 * @param recursive remove recursive
	 * @return success
	 */
	virtual void removePath(const string& pathName, bool recursive) = 0;

	/**
	 * Remove file
	 * @param pathName path name
	 * @param fileName file name
	 * @return success
	 */
	virtual void removeFile(const string& pathName, const string& fileName) = 0;
};
