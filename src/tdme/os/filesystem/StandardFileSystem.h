#pragma once

#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/os/filesystem/fwd-tdme.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/utilities/fwd-tdme.h>

using std::string;
using std::vector;

using tdme::os::filesystem::FileSystemInterface;
using tdme::os::filesystem::FileNameFilter;

/**
 * Standard file system implementation
 * @author Andreas Drewke
 */
class tdme::os::filesystem::StandardFileSystem final: public FileSystemInterface
{

public:
	/**
	 * Public constructor
	 */
	StandardFileSystem();

	/**
	 * Public destructor
	 */
	virtual ~StandardFileSystem();

	// overridden methods
	const string getFileName(const string& path, const string& fileName) override;
	void list(const string& pathName, vector<string>& files, FileNameFilter* filter = nullptr, bool addDrives = false) override;
	bool isPath(const string& pathName) override;
	bool isDrive(const string& pathName) override;
	bool fileExists(const string& fileName) override;
	bool isExecutable(const string& pathName, const string& fileName) override;
	void setExecutable(const string& pathName, const string& fileName) override;
	uint64_t getFileSize(const string& pathName, const string& fileName) override;
	const string getContentAsString(const string& pathName, const string& fileName) override;
	void setContentFromString(const string& pathName, const string& fileName, const string& content) override;
	void getContent(const string& pathName, const string& fileName, vector<uint8_t>& content) override;
	void setContent(const string& pathName, const string& fileName, const vector<uint8_t>& content) override;
	void getContentAsStringArray(const string& pathName, const string& fileName, vector<string>& content) override;
	void setContentFromStringArray(const string& pathName, const string& fileName, const vector<string>& content) override;
	const string getCanonicalPath(const string& pathName, const string& fileName) override;
	const string getCurrentWorkingPathName() override;
	const string getPathName(const string& fileName) override;
	const string getFileName(const string& fileName) override;
	void createPath(const string& pathName) override;
	void removePath(const string& pathName, bool recursive) override;
	void removeFile(const string& pathName, const string& fileName) override;
};
