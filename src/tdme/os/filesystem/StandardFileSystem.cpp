#include <tdme/os/filesystem/StandardFileSystem.h>

#if defined(_WIN32) && defined(_MSC_VER)
	#include <direct.h>
# else
	#include <unistd.h>
#endif

#include <algorithm>
#include <limits.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <tdme/os/filesystem/FileNameFilter.h>
#include <tdme/utilities/fwd-tdme.h>
#include <tdme/utilities/StringTools.h>
#include <tdme/utilities/StringTokenizer.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>

using std::getline;
using std::ifstream;
using std::ios;
using std::ofstream;
using std::sort;
using std::string;
using std::stringstream;
using std::vector;
using std::to_string;

using tdme::os::filesystem::StandardFileSystem;

using tdme::os::filesystem::FileNameFilter;
using tdme::utilities::StringTools;
using tdme::utilities::StringTokenizer;
using tdme::utilities::Console;
using tdme::utilities::Exception;

StandardFileSystem::StandardFileSystem()
{
}

StandardFileSystem::~StandardFileSystem()
{
}

const string StandardFileSystem::getFileName(const string& pathName, const string& fileName) {
	return pathName + "/" + fileName;
}

void StandardFileSystem::list(const string& pathName, vector<string>& files, FileNameFilter* filter, bool addDrives)
{
	auto _pathName = pathName;
	if (StringTools::endsWith(pathName, "/") == false) _pathName+= "/";

	DIR *dir;
	struct dirent *dirent;
	if ((dir = opendir(_pathName.c_str())) == NULL) {
		throw FileSystemException("Unable to list path(" + to_string(errno) + "): " + _pathName);
	}
	while ((dirent = readdir(dir)) != NULL) {
		string fileName = (dirent->d_name);
		if (fileName == ".") continue;
		try {
			if (filter != nullptr && filter->accept(pathName, fileName) == false) continue;
		} catch (Exception& exception) {
			Console::println("StandardFileSystem::list(): Filter::accept(): " + pathName + "/" + fileName + ": " + exception.what());
			continue;
		}
		files.push_back(fileName);
	}
	sort(files.begin(), files.end());

	#if defined(_WIN32)
		if (addDrives == true) {
			for (char drive = 'A'; drive <= 'Z'; drive++) {
				string fileName;
				fileName+= drive;
				fileName+= ":";
				try {
					if (fileExists(fileName + "/") == true) files.insert(files.begin() + (drive - 'C'), fileName);
				} catch (Exception& exception) {
					Console::println("StandardFileSystem::list(): fileExists(): " + pathName + "/" + fileName + ": " + exception.what());
				}
			}
		}
	#endif

	closedir(dir);
}

bool StandardFileSystem::isPath(const string& pathName) {
	struct stat s;
	if (stat(pathName.c_str(), &s) == 0) {
		return (s.st_mode & S_IFDIR) == S_IFDIR;
	} else {
		throw FileSystemException("Unable to check if path(" + to_string(errno) + "): " + pathName);
	}
}

bool StandardFileSystem::isDrive(const string& pathName) {
	return StringTools::regexMatch(pathName, "^[a-zA-Z]\\:[\\/\\\\]?$");
}

bool StandardFileSystem::fileExists(const string& fileName) {
	struct stat s;
	return stat(fileName.c_str(), &s) == 0;
}

bool StandardFileSystem::isExecutable(const string& pathName, const string& fileName) {
	struct stat s;
	if (stat((pathName + "/" + fileName).c_str(), &s) != 0) return false;
	return (s.st_mode & S_IXUSR) == S_IXUSR;
}

void StandardFileSystem::setExecutable(const string& pathName, const string& fileName) {
	#if !defined(_MSC_VER)
		struct stat s;
		if (stat((pathName + "/" + fileName).c_str(), &s) != 0) {
			throw FileSystemException("Unable to set file to executable(" + to_string(errno) + "): " + pathName + "/" + fileName);
		}
		auto newMode = s.st_mode;
		if ((s.st_mode & S_IRUSR) == S_IRUSR && (s.st_mode & S_IXUSR) == 0) newMode|= S_IXUSR;
		if ((s.st_mode & S_IRGRP) == S_IRGRP && (s.st_mode & S_IXGRP) == 0) newMode|= S_IXGRP;
		if ((s.st_mode & S_IROTH) == S_IROTH && (s.st_mode & S_IXOTH) == 0) newMode|= S_IXOTH;
		if (chmod((pathName + "/" + fileName).c_str(), newMode) < 0) {
			throw FileSystemException("Unable to set file to executable(" + to_string(errno) + "): " + pathName + "/" + fileName);
		}
	#endif
}

uint64_t StandardFileSystem::getFileSize(const string& pathName, const string& fileName) {
	ifstream ifs(getFileName(pathName, fileName).c_str(), ifstream::binary);
	if (ifs.is_open() == false) {
		throw FileSystemException("Unable to open file for reading(" + to_string(errno) + "): " + pathName + "/" + fileName);
	}
	ifs.seekg( 0, ios::end );
	size_t size = ifs.tellg();
	ifs.close();
	return size;
}

const string StandardFileSystem::getContentAsString(const string& pathName, const string& fileName) {
	ifstream ifs(getFileName(pathName, fileName).c_str());
	if (ifs.is_open() == false) {
		throw FileSystemException("Unable to open file for reading(" + to_string(errno) + "): " + pathName + "/" + fileName);
	}
	stringstream stringStream;
	stringStream << ifs.rdbuf();
	ifs.close();
	return (stringStream.str());
}

void StandardFileSystem::setContentFromString(const string& pathName, const string& fileName, const string& content) {
	ofstream ofs(getFileName(pathName, fileName).c_str());
	if (ofs.is_open() == false) {
		throw FileSystemException("Unable to open file for writing(" + to_string(errno) + "): " + pathName + "/" + fileName);
	}
	ofs << (content);
	ofs.close();
	return;
}

void StandardFileSystem::getContent(const string& pathName, const string& fileName, vector<uint8_t>& content)
{
	ifstream ifs(getFileName(pathName, fileName).c_str(), ifstream::binary);
	if (ifs.is_open() == false) {
		throw FileSystemException("Unable to open file for reading(" + to_string(errno) + "): " + pathName + "/" + fileName);
	}
	ifs.seekg( 0, ios::end );
	size_t size = ifs.tellg();
	content.resize(size);
	ifs.seekg(0, ios::beg);
	ifs.read((char*)content.data(), size);
	ifs.close();
}

void StandardFileSystem::setContent(const string& pathName, const string& fileName, const vector<uint8_t>& content) {
	ofstream ofs(getFileName(pathName, fileName).c_str(), ofstream::binary);
	if (ofs.is_open() == false) {
		throw FileSystemException("Unable to open file for writing(" + to_string(errno) + "): " + pathName + "/" + fileName);
	}
	ofs.write((char*)content.data(), content.size());
	ofs.close();
}

void StandardFileSystem::getContentAsStringArray(const string& pathName, const string& fileName, vector<string>& content)
{
	ifstream ifs(getFileName(pathName, fileName).c_str());
	if(ifs.is_open() == false) {
		throw FileSystemException("Unable to open file for reading(" + to_string(errno) + "): " + pathName + "/" + fileName);
	}

	string line;
	while (getline(ifs, line)) {
		content.push_back((line));
	}

	ifs.close();
}

void StandardFileSystem::setContentFromStringArray(const string& pathName, const string& fileName, const vector<string>& content)
{
	ofstream ofs(getFileName(pathName, fileName).c_str());
	if(ofs.is_open() == false) {
		throw FileSystemException("Unable to open file for writing(" + to_string(errno) + "): " + pathName + "/" + fileName);
	}

	for (auto i = 0; i < content.size(); i++) {
		ofs << (content.at(i)) << "\n";
	}

	ofs.close();
	return;
}

const string StandardFileSystem::getCanonicalPath(const string& pathName, const string& fileName) {
	string unixPathName = StringTools::replace(pathName, "\\", "/");
	string unixFileName = StringTools::replace(fileName, "\\", "/");

	auto pathString = getFileName(unixPathName, unixFileName);

	// separate into path components
	vector<string> pathComponents;
	StringTokenizer t;
	t.tokenize(pathString, "/");
	while (t.hasMoreTokens()) {
		pathComponents.push_back(t.nextToken());
	}

	// process path components
	for (auto i = 0; i < pathComponents.size(); i++) {
		auto pathComponent = pathComponents[i];
		if (pathComponent == ".") {
			pathComponents[i] = "";
		} else
		if (pathComponent == "..") {
			pathComponents[i]= "";
			int j = i - 1;
			for (int pathComponentReplaced = 0; pathComponentReplaced < 1 && j >= 0; ) {
				if (pathComponents[j] != "") {
					pathComponents[j] = "";
					pathComponentReplaced++;
				}
				j--;
			}
		}
	}

	// process path components
	string canonicalPath = "";
	bool slash = StringTools::startsWith(pathString, "/");
	for (auto i = 0; i < pathComponents.size(); i++) {
		auto pathComponent = pathComponents[i];
		if (pathComponent == "") {
			// no op
		} else {
			canonicalPath = canonicalPath + (slash == true?"/":"") + pathComponent;
			slash = true;
		}
	}

	// add cwd if required
	auto canonicalPathString = canonicalPath;
	if (canonicalPathString.length() == 0 ||
		(StringTools::startsWith(canonicalPathString, "/") == false &&
		StringTools::regexMatch(canonicalPathString, "^[a-zA-Z]\\:.*$") == false)) {
		canonicalPathString = getCurrentWorkingPathName() + "/" + canonicalPathString;
	}

	//
	return canonicalPathString;
}

const string StandardFileSystem::getCurrentWorkingPathName() {
	// cwd
	char cwdBuffer[PATH_MAX + 1];
	char* cwdPtr = getcwd(cwdBuffer, sizeof(cwdBuffer));
	if (cwdPtr == nullptr) {
		throw FileSystemException("Unable to get current working path(" + to_string(errno) + ")");
	}
	return (cwdPtr);
}

const string StandardFileSystem::getPathName(const string& fileName) {
	string unixFileName = StringTools::replace(fileName, L'\\', L'/');
	int32_t lastPathSeparator = StringTools::lastIndexOf(unixFileName, L'/');
	if (lastPathSeparator == -1) return ".";
	return StringTools::substring(unixFileName, 0, lastPathSeparator);
}

const string StandardFileSystem::getFileName(const string& fileName) {
	string unixFileName = StringTools::replace(fileName, L'\\', L'/');
	int32_t lastPathSeparator = StringTools::lastIndexOf(unixFileName, L'/');
	if (lastPathSeparator == -1) return fileName;
	return StringTools::substring(unixFileName, lastPathSeparator + 1, unixFileName.length());
}

void StandardFileSystem::createPath(const string& pathName) {
	#if defined(_WIN32)
		int32_t status = mkdir(pathName.c_str());
	#else
		int32_t status = mkdir(pathName.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	#endif
	if (status == -1) {
		throw FileSystemException("Unable to create path(" + to_string(errno) + "): " + pathName);
	}
}

void StandardFileSystem::removePath(const string& pathName, bool recursive) {
	if (recursive == true) {
		vector<string> files;
		list(pathName, files, nullptr);
		for (auto i = 0; i < files.size(); i++) {
			auto file = files[i];
			if (file == "." || file == "..") {
				continue;
			}
			auto completeFileName = getFileName(pathName, file);
			if (isPath(completeFileName)) {
				removePath(completeFileName, true);
			} else {
				removeFile(pathName, file);
			}
		}
	}
	Console::println(string("StandardFileSystem::removePath(): Removing ") + pathName);
	int32_t status = rmdir(pathName.c_str());
	if (status == -1) {
		throw FileSystemException("Unable to delete folder(" + to_string(errno) + "): " + pathName);
	}
}

void StandardFileSystem::removeFile(const string& pathName, const string& fileName) {
	Console::println(string("StandardFileSystem::removeFile(): Removing ") + getFileName(pathName, fileName));
	int32_t status = unlink(getFileName(pathName, fileName).c_str());
	if (status == -1) {
		throw FileSystemException("Unable to delete file(" + to_string(errno) + "): " + pathName + "/" + fileName);
	}
}
