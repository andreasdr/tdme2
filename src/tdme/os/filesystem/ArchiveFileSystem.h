#pragma once

#include <fstream>
#include <map>
#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/os/filesystem/fwd-tdme.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/os/threading/Mutex.h>
#include <tdme/utilities/fwd-tdme.h>

using std::ifstream;
using std::map;
using std::string;
using std::vector;

using tdme::os::filesystem::FileNameFilter;
using tdme::os::filesystem::FileSystemInterface;
using tdme::os::threading::Mutex;

/**
 * Archive file system implementation
 * @author Andreas Drewke
 */
class tdme::os::filesystem::ArchiveFileSystem final: public FileSystemInterface
{
private:
	struct FileInformation {
		string name;
		uint64_t bytes;
		uint8_t compressed;
		uint64_t bytesCompressed;
		uint64_t offset;
		bool executable;
	};
	string fileName;
	Mutex ifsMutex;
	ifstream ifs;
	map<string, FileInformation> fileInformations;

	/**
	 * Decompress from archive
	 * @param inContent compressed content
	 * @param outContent uncompressed out content
	 * @throws tdme::os::filesystem::FileSystemException
	 */
	void decompress(vector<uint8_t>& inContent, vector<uint8_t>& outContent);

public:
	/**
	 * @return Returns underlying TDME2 archive file name
	 */
	const string& getArchiveFileName();

	// overriden methods
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
	const string computeSHA256Hash();
	ArchiveFileSystem(const string& fileName = "archive.ta");
	virtual ~ArchiveFileSystem();
};
