#include <tdme/os/filesystem/ArchiveFileSystem.h>

#include <string.h>

#include <algorithm>
#include <cassert>
#include <fstream>
#include <string>
#include <vector>

#include <ext/sha256/sha256.h>
#include <ext/zlib/zlib.h>

#include <tdme/math/Math.h>
#include <tdme/os/filesystem/FileNameFilter.h>
#include <tdme/os/threading/Mutex.h>
#include <tdme/utilities/fwd-tdme.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>
#include <tdme/utilities/StringTokenizer.h>
#include <tdme/utilities/StringTools.h>

using std::ifstream;
using std::ios;
using std::sort;
using std::string;
using std::to_string;
using std::vector;

using tdme::math::Math;
using tdme::os::filesystem::ArchiveFileSystem;
using tdme::os::threading::Mutex;
using tdme::utilities::Console;
using tdme::utilities::Exception;
using tdme::utilities::StringTokenizer;
using tdme::utilities::StringTools;

ArchiveFileSystem::ArchiveFileSystem(const string& fileName): fileName(fileName), ifsMutex("afs-ifs-mutex")
{
	// open
	ifs.open(fileName.c_str(), ifstream::binary);
	if (ifs.is_open() == false) {
		throw FileSystemException("Unable to open file for reading(" + to_string(errno) + "): " + fileName);
	}

	// read toc offset
	uint64_t fileInformationOffset;
	ifs.seekg(-static_cast<int64_t>(sizeof(fileInformationOffset)), ios::end);
	ifs.read((char*)&fileInformationOffset, sizeof(fileInformationOffset));
	ifs.seekg(fileInformationOffset, ios::beg);

	// read toc
	while (true == true) {
		uint32_t nameSize;
		ifs.read((char*)&nameSize, sizeof(nameSize));
		if (nameSize == 0) break;

		FileInformation fileInformation;
		auto buffer = new char[nameSize];
		ifs.read(buffer, nameSize);
		fileInformation.name.append(buffer, nameSize);
		delete [] buffer;
		ifs.read((char*)&fileInformation.bytes, sizeof(fileInformation.bytes));
		ifs.read((char*)&fileInformation.compressed, sizeof(fileInformation.compressed));
		ifs.read((char*)&fileInformation.bytesCompressed, sizeof(fileInformation.bytesCompressed));
		ifs.read((char*)&fileInformation.offset, sizeof(fileInformation.offset));
		ifs.read((char*)&fileInformation.executable, sizeof(fileInformation.executable));
		fileInformations[fileInformation.name] = fileInformation;
	}
}

ArchiveFileSystem::~ArchiveFileSystem()
{
	ifs.close();
}

const string& ArchiveFileSystem::getArchiveFileName() {
	return fileName;
}

const string ArchiveFileSystem::getFileName(const string& pathName, const string& fileName) {
	return pathName + "/" + fileName;
}

void ArchiveFileSystem::list(const string& pathName, vector<string>& files, FileNameFilter* filter, bool addDrives)
{
	// TODO: this currently lists all files beginning from given path, also files in sub folders
	auto _pathName = pathName;
	if (_pathName.empty() == false && StringTools::endsWith(pathName, "/") == false) _pathName+= "/";
	for (auto& fileInformationIt: fileInformations) {
		auto fileName = fileInformationIt.second.name;
		if (StringTools::startsWith(fileName, _pathName) == true) {
			try {
				if (filter != nullptr && filter->accept(
					getPathName(fileName),
					getFileName(fileName)
				) == false) continue;
			} catch (Exception& exception) {
				Console::println("StandardFileSystem::list(): Filter::accept(): " + pathName + "/" + fileName + ": " + exception.what());
				continue;
			}
			files.push_back(StringTools::substring(fileName, pathName.size()));
		}
	}
	sort(files.begin(), files.end());
}

bool ArchiveFileSystem::isPath(const string& pathName) {
	return false;
}

bool ArchiveFileSystem::isDrive(const string& pathName) {
	return false;
}

bool ArchiveFileSystem::fileExists(const string& fileName) {
	// compose relative file name and remove ./
	auto relativeFileName = fileName;
	if (StringTools::startsWith(relativeFileName, "./")  == true) relativeFileName = StringTools::substring(relativeFileName, 2);

	//
	return fileInformations.find(relativeFileName) != fileInformations.end();
}

bool ArchiveFileSystem::isExecutable(const string& pathName, const string& fileName) {
	// compose relative file name and remove ./
	auto relativeFileName = pathName + "/" + fileName;
	if (StringTools::startsWith(relativeFileName, "./")  == true) relativeFileName = StringTools::substring(relativeFileName, 2);

	// determine file
	auto fileInformationIt = fileInformations.find(relativeFileName);
	if (fileInformationIt == fileInformations.end()) {
		throw FileSystemException("Unable to open file for reading: " + relativeFileName + ": " + pathName + "/" + fileName);
	}
	auto& fileInformation = fileInformationIt->second;
	//
	return fileInformation.executable;
}

void ArchiveFileSystem::setExecutable(const string& pathName, const string& fileName) {
	throw FileSystemException("ArchiveFileSystem::createPath(): Not implemented yet");
}

uint64_t ArchiveFileSystem::getFileSize(const string& pathName, const string& fileName) {
	// compose relative file name and remove ./
	auto relativeFileName = pathName + "/" + fileName;
	if (StringTools::startsWith(relativeFileName, "./")  == true) relativeFileName = StringTools::substring(relativeFileName, 2);

	// determine file
	auto fileInformationIt = fileInformations.find(relativeFileName);
	if (fileInformationIt == fileInformations.end()) {
		throw FileSystemException("Unable to open file for reading: " + relativeFileName + ": " + pathName + "/" + fileName);
	}
	auto& fileInformation = fileInformationIt->second;
	//
	return fileInformation.bytes;
}

void ArchiveFileSystem::decompress(vector<uint8_t>& inContent, vector<uint8_t>& outContent) {
	// see: https://www.zlib.net/zpipe.c

	#define CHUNK	16384

	int ret;
	size_t have;
	z_stream strm;
	unsigned char in[CHUNK];
	unsigned char out[CHUNK];

	// allocate inflate state
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;
	ret = inflateInit(&strm);
	if (ret != Z_OK) {
		throw FileSystemException("ArchiveFileSystem::decompress(): Error while decompressing: inflate init");
	}

	// decompress until deflate stream ends or end of file */
	size_t outPosition = 0;
	size_t inPosition = 0;
	size_t inBytes = inContent.size();
	do {
		// see: https://www.zlib.net/zpipe.c
		auto inStartPosition = inPosition;
		for (size_t i = 0; i < CHUNK; i++) {
			if (inPosition == inBytes) break;
			in[i] = inContent[inPosition];
			inPosition++;
		}
		strm.avail_in = inPosition - inStartPosition;
		if (strm.avail_in == 0) break;
		strm.next_in = in;

		// run inflate() on input until output buffer not full
		do {
			strm.avail_out = CHUNK;
			strm.next_out = out;
			ret = inflate(&strm, Z_NO_FLUSH);
			assert(ret != Z_STREAM_ERROR); // state not clobbered
			switch (ret) {
				case Z_NEED_DICT:
					throw FileSystemException("ArchiveFileSystem::decompress(): Error while decompressing: Z_NEED_DICT");
				case Z_DATA_ERROR:
				case Z_MEM_ERROR:
					(void)inflateEnd(&strm);
					throw FileSystemException("ArchiveFileSystem::decompress(): Error while decompressing: Z_DATA_ERROR | Z_MEM_ERROR");
			}
			have = CHUNK - strm.avail_out;
			for (size_t i = 0; i < have; i++) {
				outContent[outPosition++] = out[i];
			}
		} while (strm.avail_out == 0);

		// done when inflate() says it's done
	} while (ret != Z_STREAM_END);

	// clean up and return
	(void) inflateEnd(&strm);

	// check if eof
	if (ret != Z_STREAM_END) {
		throw FileSystemException("ArchiveFileSystem::decompress(): Error while decompressing: missing eof");
	}
}

const string ArchiveFileSystem::getContentAsString(const string& pathName, const string& fileName) {
	// compose relative file name and remove ./
	auto relativeFileName = pathName + "/" + fileName;
	if (StringTools::startsWith(relativeFileName, "./")  == true) relativeFileName = StringTools::substring(relativeFileName, 2);

	// determine file
	auto fileInformationIt = fileInformations.find(relativeFileName);
	if (fileInformationIt == fileInformations.end()) {
		throw FileSystemException("Unable to open file for reading: " + relativeFileName + ": " + pathName + "/" + fileName);
	}
	auto& fileInformation = fileInformationIt->second;

	//
	ifsMutex.lock();

	// seek
	ifs.seekg(fileInformation.offset, ios::beg);

	// result
	string result;
	if (fileInformation.compressed == 1) {
		vector<uint8_t> compressedBuffer;
		compressedBuffer.resize(fileInformation.bytesCompressed);
		ifs.read((char*)compressedBuffer.data(), fileInformation.bytesCompressed);
		ifsMutex.unlock();
		vector<uint8_t> decompressedBuffer;
		decompressedBuffer.resize(fileInformation.bytes);
		decompress(compressedBuffer, decompressedBuffer);
		result.append((char*)decompressedBuffer.data(), fileInformation.bytes);
	} else {
		vector<uint8_t> buffer;
		buffer.resize(fileInformation.bytes);
		ifs.read((char*)buffer.data(), fileInformation.bytes);
		ifsMutex.unlock();
		result.append((char*)buffer.data(), fileInformation.bytes);
	}

	// done
	return result;
}

void ArchiveFileSystem::setContentFromString(const string& pathName, const string& fileName, const string& content) {
	throw FileSystemException("ArchiveFileSystem::setContentFromString(): Not implemented yet");
}

void ArchiveFileSystem::getContent(const string& pathName, const string& fileName, vector<uint8_t>& content)
{
	// compose relative file name and remove ./
	auto relativeFileName = pathName + "/" + fileName;
	if (StringTools::startsWith(relativeFileName, "./")  == true) relativeFileName = StringTools::substring(relativeFileName, 2);

	// determine file
	auto fileInformationIt = fileInformations.find(relativeFileName);
	if (fileInformationIt == fileInformations.end()) {
		throw FileSystemException("Unable to open file for reading: " + relativeFileName + ": " + pathName + "/" + fileName);
	}
	auto& fileInformation = fileInformationIt->second;

	//
	ifsMutex.lock();

	// seek
	ifs.seekg(fileInformation.offset, ios::beg);

	// result
	if (fileInformation.compressed == 1) {
		vector<uint8_t> compressedContent;
		compressedContent.resize(fileInformation.bytesCompressed);
		ifs.read((char*)compressedContent.data(), fileInformation.bytesCompressed);
		ifsMutex.unlock();
		content.resize(fileInformation.bytes);
		decompress(compressedContent, content);
	} else {
		content.resize(fileInformation.bytes);
		ifs.read((char*)content.data(), fileInformation.bytes);
		ifsMutex.unlock();
	}
}

void ArchiveFileSystem::setContent(const string& pathName, const string& fileName, const vector<uint8_t>& content) {
	throw FileSystemException("ArchiveFileSystem::setContent(): Not implemented yet");
}

void ArchiveFileSystem::getContentAsStringArray(const string& pathName, const string& fileName, vector<string>& content)
{
	auto contentAsString = getContentAsString(pathName, fileName);
	contentAsString = StringTools::replace(contentAsString, "\r", "");
	StringTokenizer t;
	t.tokenize(contentAsString, "\n");
	while (t.hasMoreTokens() == true) {
		content.push_back(t.nextToken());
	}
}

void ArchiveFileSystem::setContentFromStringArray(const string& pathName, const string& fileName, const vector<string>& content)
{
	throw FileSystemException("ArchiveFileSystem::setContentFromStringArray(): Not implemented yet");
}

const string ArchiveFileSystem::getCanonicalPath(const string& pathName, const string& fileName) {
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

const string ArchiveFileSystem::getCurrentWorkingPathName() {
	return ".";
}

const string ArchiveFileSystem::getPathName(const string& fileName) {
	string unixFileName = StringTools::replace(fileName, L'\\', L'/');
	int32_t lastPathSeparator = StringTools::lastIndexOf(unixFileName, L'/');
	if (lastPathSeparator == -1) return ".";
	return StringTools::substring(unixFileName, 0, lastPathSeparator);
}

const string ArchiveFileSystem::getFileName(const string& fileName) {
	string unixFileName = StringTools::replace(fileName, L'\\', L'/');
	int32_t lastPathSeparator = StringTools::lastIndexOf(unixFileName, L'/');
	if (lastPathSeparator == -1) return fileName;
	return StringTools::substring(unixFileName, lastPathSeparator + 1, unixFileName.length());
}

void ArchiveFileSystem::createPath(const string& pathName) {
	throw FileSystemException("ArchiveFileSystem::createPath(): Not implemented yet");
}

void ArchiveFileSystem::removePath(const string& pathName, bool recursive) {
	throw FileSystemException("ArchiveFileSystem::removePath(): Not implemented yet");
}

void ArchiveFileSystem::removeFile(const string& pathName, const string& fileName) {
	throw FileSystemException("ArchiveFileSystem::removeFile(): Not implemented yet");
}

const string ArchiveFileSystem::computeSHA256Hash() {
	ifs.seekg(0, ios::end);
	auto bytesTotal = ifs.tellg();
	ifs.seekg(0, ios::beg);

	uint8_t input[16384];
	unsigned char digest[SHA256::DIGEST_SIZE];
	memset(digest, 0, SHA256::DIGEST_SIZE);

	SHA256 ctx = SHA256();
	ctx.init();
	auto bytesRead = 0LL;
	while (bytesRead < bytesTotal) {
		auto bytesToRead = Math::min(static_cast<int64_t>(bytesTotal - bytesRead), sizeof(input));
		ifs.read((char*)input, bytesToRead);
		ctx.update((const uint8_t*)input, bytesToRead);
		bytesRead+= bytesToRead;
	}
	ctx.final(digest);

	char buf[2 * SHA256::DIGEST_SIZE + 1];
	buf[2 * SHA256::DIGEST_SIZE] = 0;
	for (int i = 0; i < SHA256::DIGEST_SIZE; i++) sprintf(buf + i * 2, "%02x", digest[i]);
	return std::string(buf);
}
