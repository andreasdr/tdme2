#include <cassert>
#include <fstream>
#include <string>
#include <vector>

#include <ext/zlib/zlib.h>

#include <tdme/application/Application.h>
#include <tdme/os/filesystem/ArchiveFileSystem.h>
#include <tdme/os/filesystem/FileNameFilter.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>
#include <tdme/utilities/Properties.h>
#include <tdme/utilities/StringTokenizer.h>
#include <tdme/utilities/StringTools.h>
#include <tdme/utilities/Time.h>

using std::ofstream;
using std::string;
using std::to_string;
using std::vector;

using tdme::application::Application;
using tdme::os::filesystem::ArchiveFileSystem;
using tdme::os::filesystem::FileNameFilter;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;
using tdme::utilities::Console;
using tdme::utilities::Exception;
using tdme::utilities::Properties;
using tdme::utilities::StringTokenizer;
using tdme::utilities::StringTools;
using tdme::utilities::Time;

namespace tdme {
namespace tools {
namespace cli {
namespace installer {
	struct FileInformation {
		string name;
		uint64_t bytes;
		uint8_t compressed;
		uint64_t bytesCompressed;
		uint64_t offset;
		bool executable;
	};
};
};
};
};

using tdme::tools::cli::installer::FileInformation;

static void scanPathResources(const string& path, vector<string>& totalFiles) {
	class ListFilter : public virtual FileNameFilter {
		public:
			virtual ~ListFilter() {}

			bool accept(const string& pathName, const string& fileName) override {
				if (fileName == ".") return false;
				if (fileName == "..") return false;
				if (FileSystem::getInstance()->isPath(pathName + "/" + fileName) == true) return true;
				// shader
				if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".glsl") == true) return true;
				if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".vert") == true) return true;
				if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".frag") == true) return true;
				if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".cl") == true) return true;
				// audio
				if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".ogg") == true) return true;
				// models
				if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".tm") == true) return true;
				if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".dae") == true) return true;
				if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".fbx") == true) return true;
				if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".glb") == true) return true;
				// meta
				if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".tmm") == true) return true;
				// scene
				if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".tl") == true) return true;
				// particle system
				if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".tps") == true) return true;
				// xml
				if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".xml") == true) return true;
				// images
				if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".ico") == true) return true;
				if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".png") == true) return true;
				// fonts
				if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".fnt") == true) return true;
				// wss
				if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".wss") == true) return true;
				// files without ending
				//	TODO: fix me, paths get submitted here too as filename
				if (fileName.rfind(".") == string::npos ||
					(fileName.rfind("/") != string::npos &&
					fileName.rfind(".") < fileName.rfind("/"))) {
					return true;
				}
				// properties
				if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".properties") == true) return true;
				// tdme archive
				if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".ta") == true) return true;
				//
				return false;
			}
	};

	ListFilter listFilter;
	vector<string> files;

	if (FileSystem::getInstance()->fileExists(path) == false) {
		Console::println("Error: scanPathResources: file does not exist: " + path);
	} else
	if (FileSystem::getInstance()->isPath(path) == false) {
		if (listFilter.accept(".", path) == true) {
			totalFiles.push_back(path);
		} else {
			Console::println("Error: scanPathResources: file exist, but does not match filter: " + path);
		}
	} else {
		FileSystem::getInstance()->list(path, files, &listFilter);
		for (auto fileName: files) {
			if (FileSystem::getInstance()->isPath(path + "/" + fileName) == false) {
				totalFiles.push_back(path + "/" + fileName);
			} else {
				scanPathResources(path + "/" + fileName, totalFiles);
			}
		}
	}
}

static void scanPathLibraries(const string& path, vector<string>& totalFiles) {
	class ListFilter : public virtual FileNameFilter {
		public:
			virtual ~ListFilter() {}

			bool accept(const string& pathName, const string& fileName) override {
				if (fileName == ".") return false;
				if (fileName == "..") return false;
				if (FileSystem::getInstance()->isPath(pathName + "/" + fileName) == true) return true;
				// static libraries
				if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".a") == true) return true;
				// dynamic libraries
				#if defined(_WIN32)
					if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".dll") == true) return true;
					if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".lib") == true) return true;
				#elif defined(__APPLE__)
					if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".dylib") == true) return true;
					if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".so") == true) return true;
				#else
					if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".so") == true) return true;
				#endif
				//
				return false;
			}
	};

	ListFilter listFilter;
	vector<string> files;

	if (FileSystem::getInstance()->fileExists(path) == false) {
		Console::println("Error: scanPathLibraries: file does not exist: " + path);
	} else
	if (FileSystem::getInstance()->isPath(path) == false) {
		if (listFilter.accept(".", path) == true) {
			totalFiles.push_back(path);
		} else {
			Console::println("Error: scanPathLibraries: file exist, but does not match filter: " + path);
		}
	} else {
		FileSystem::getInstance()->list(path, files, &listFilter);
		for (auto fileName: files) {
			if (FileSystem::getInstance()->isPath(path + "/" + fileName) == false) {
				totalFiles.push_back(path + "/" + fileName);
			} else {
				scanPathLibraries(path + "/" + fileName, totalFiles);
			}
		}
	}
}

static void scanPathHeaders(const string& path, vector<string>& totalFiles) {
	class ListFilter : public virtual FileNameFilter {
		public:
			virtual ~ListFilter() {}

			bool accept(const string& pathName, const string& fileName) override {
				if (fileName == ".") return false;
				if (fileName == "..") return false;
				if (FileSystem::getInstance()->isPath(pathName + "/" + fileName) == true) return true;
				// headers
				if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".h") == true) return true;
				//
				return false;
			}
	};

	ListFilter listFilter;
	vector<string> files;

	if (FileSystem::getInstance()->fileExists(path) == false) {
		Console::println("Error: scanPathHeaders: file does not exist: " + path);
	} else
	if (FileSystem::getInstance()->isPath(path) == false) {
		if (listFilter.accept(".", path) == true) {
			totalFiles.push_back(path);
		} else {
			Console::println("Error: scanPathHeaders: file exist, but does not match filter: " + path);
		}
	} else {
		FileSystem::getInstance()->list(path, files, &listFilter);
		for (auto fileName: files) {
			if (FileSystem::getInstance()->isPath(path + "/" + fileName) == false) {
				totalFiles.push_back(path + "/" + fileName);
			} else {
				scanPathHeaders(path + "/" + fileName, totalFiles);
			}
		}
	}
}

static void scanPathExecutables(const string& path, vector<string>& totalFiles) {
	class ListFilter : public virtual FileNameFilter {
		public:
			virtual ~ListFilter() {}

			bool accept(const string& pathName, const string& fileName) override {
				if (fileName == ".") return false;
				if (fileName == "..") return false;
				if (FileSystem::getInstance()->isPath(pathName + "/" + fileName) == true) return true;
				#if defined(_WIN32)
					if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".exe") == true) return true;
					if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".dll") == true) return true;
					if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".bat") == true) return true;
				#elif defined(__APPLE__)
					// TODO: fix me, paths get submitted here too as filename
					if (fileName.rfind(".") == string::npos ||
						(fileName.rfind("/") != string::npos &&
						fileName.rfind(".") < fileName.rfind("/"))) {
						return true;
					}
					if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".dylib") == true) return true;
					if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".so") == true) return true;
					if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".sh") == true) return true;
				#else
					// TODO: fix me, paths get submitted here too as filename
					if (fileName.rfind(".") == string::npos ||
						(fileName.rfind("/") != string::npos &&
						fileName.rfind(".") < fileName.rfind("/"))) {
						return true;
					}
					if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".so") == true) return true;
					if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".sh") == true) return true;
				#endif
				//
				return false;
			}
	};

	ListFilter listFilter;
	vector<string> files;

	if (FileSystem::getInstance()->fileExists(path) == false) {
		Console::println("Error: scanPathExecutables: file does not exist: " + path);
	} else
	if (FileSystem::getInstance()->isPath(path) == false) {
		if (listFilter.accept(".", path) == true) {
			totalFiles.push_back(path);
		} else {
			Console::println("Error: scanPathExecutables: file exist, but does not match filter: " + path);
		}
	} else {
		FileSystem::getInstance()->list(path, files, &listFilter);
		for (auto fileName: files) {
			if (FileSystem::getInstance()->isPath(path + "/" + fileName) == false) {
				totalFiles.push_back(path + "/" + fileName);
			} else {
				scanPathExecutables(path + "/" + fileName, totalFiles);
			}
		}
	}
}

void processFile(const string& fileName, vector<FileInformation>& fileInformations, const string& archiveFileName, bool executableFile, const string& basePath = string()) {
	// read content
	vector<uint8_t> content;
	FileSystem::getInstance()->getContent(
		FileSystem::getInstance()->getPathName(fileName),
		FileSystem::getInstance()->getFileName(fileName),
		content
	);

	auto fileNameToUse = StringTools::startsWith(fileName, basePath + "/") == true?StringTools::substring(fileName, (basePath + "/").size(), fileName.size()):fileName;
	// remove prefix if requested
	if (executableFile == true && fileName.find_last_of('/') != string::npos) {
		fileNameToUse = StringTools::substring(fileNameToUse, fileNameToUse.find_last_of('/') + 1);
	}

	Console::print(archiveFileName + ": Processing file: " + fileNameToUse);

	// append to archive
	ofstream ofs(archiveFileName.c_str(), ofstream::binary | ofstream::app);
	ofs.seekp(0, ofstream::end);
	uint64_t fileOffset = ofs.tellp();

	//
	uint64_t bytesCompressed = 0;
	uint8_t compressed = 1;

	// always use compression for now
	if (compressed == 1) {
		// see: https://www.zlib.net/zpipe.c

		#define CHUNK 	16384

		int ret;
		int flush;
		size_t have;
		z_stream strm;
		unsigned char in[CHUNK];
		unsigned char out[CHUNK];

		/* allocate deflate state */
		strm.zalloc = Z_NULL;
		strm.zfree = Z_NULL;
		strm.opaque = Z_NULL;
		ret = deflateInit(&strm, Z_DEFAULT_COMPRESSION);
		if (ret != Z_OK) {
			Console::println("processFile(): Error compressing file: Aborting");
			return;
		}

		// compress until end of file
		size_t inPosition = 0;
		size_t inBytes = content.size();
		do {
			auto inStartPosition = inPosition;
			for (size_t i = 0; i < CHUNK; i++) {
				if (inPosition == inBytes) break;
				in[i] = content[inPosition];
				inPosition++;
			}
			strm.avail_in = inPosition - inStartPosition;
			flush = inPosition == inBytes?Z_FINISH:Z_NO_FLUSH;
			strm.next_in = in;

			// run deflate() on input until output buffer not full, finish compression if all of source has been read in
			do {
				strm.avail_out = CHUNK;
				strm.next_out = out;
				ret = deflate(&strm, flush); // no bad return value
				assert(ret != Z_STREAM_ERROR); // state not clobbered
				have = CHUNK - strm.avail_out;
				ofs.write((char*)out, have);
				bytesCompressed+= have;
			} while (strm.avail_out == 0);
			assert(strm.avail_in == 0); // all input will be used

			// done when last data in file processed
		} while (flush != Z_FINISH);
		assert(ret == Z_STREAM_END); // stream will be complete

		// clean up and return
		(void) deflateEnd(&strm);
	} else {
		ofs.write((char*)content.data(), content.size());
	}
	ofs.close();

	// store file information
	FileInformation fileInformation;
	fileInformation.name = fileNameToUse;
	fileInformation.bytes = content.size();
	fileInformation.compressed = compressed;
	fileInformation.bytesCompressed = bytesCompressed;
	fileInformation.offset = fileOffset;
	fileInformation.executable = executableFile;
	fileInformations.push_back(fileInformation);

	// done
	Console::println(", processed " + to_string(content.size()) + " bytes" + (compressed == 1?", " + to_string(bytesCompressed) + " bytes compressed":""));
}

int main(int argc, char** argv)
{
	Console::println(string("create-installer 1.9.9"));
	Console::println(string("Programmed 2019 by Andreas Drewke, drewke.net."));
	Console::println();

	//
	string tdmePath = "../tdme2";
	auto cpu = Application::getCPUName();
	auto os = Application::getOSName();
	auto fileNameTime = StringTools::replace(StringTools::replace(StringTools::replace(Time::getAsString(), " ", "-" ), ":", ""), "-", "");

	//
	Properties installerProperties;
	installerProperties.load("resources/installer", "installer.properties");
	for (auto componentIdx = 0; true; componentIdx++) {
		auto componentId = componentIdx == 0?"installer":"component" + to_string(componentIdx);
		auto componentName = installerProperties.get(componentId, "");
		if (componentName.empty() == true) break;
		Console::println("Having component: " + to_string(componentIdx) + ": " + componentName);
		auto componentInclude = installerProperties.get(componentId + "_include", "");
		if (componentInclude.empty() == true) {
			Console::println("component: " + to_string(componentIdx) + ": missing includes. Skipping.");
			continue;
		}

		//
		auto componentFileName = os + "-" + cpu + "-" + StringTools::replace(StringTools::replace(componentName, " - ", "-"), " ", "-") + "-" + fileNameTime + ".ta";
		//
		Console::println("Component: " + to_string(componentIdx) + ": component file name: " + componentFileName);

		if (FileSystem::getInstance()->fileExists("installer") == false) {
			FileSystem::getInstance()->createPath("installer");
		}

		// reset archive
		{
			ofstream ofs("installer/" + componentFileName, ofstream::binary | ofstream::trunc);
			ofs.close();
		}

		//
		vector<FileInformation> fileInformations;
		vector<string> filesData;
		vector<string> filesBin;

		// parse includes definitions
		StringTokenizer t;
		StringTokenizer t2;
		t.tokenize(componentInclude, ",");
		while (t.hasMoreTokens() == true) {
			auto componentIncludeDefinition = t.nextToken();
			t2.tokenize(componentIncludeDefinition, ":");
			string type = t2.hasMoreTokens() == true?t2.nextToken():"";
			string file = t2.hasMoreTokens() == true?t2.nextToken():"";
			if (type.empty() == true || file.empty() == true) {
				Console::println("Component: " + to_string(componentIdx) + ": type or file empty. Skipping");
				continue;
			}
			Console::println("Component: " + to_string(componentIdx) + ": type = " + type + "; file = " + file);

			// scan files
			if (type == "exe") {
				StringTokenizer t;
				t.tokenize(installerProperties.get("exe_path", ""), ",");
				while (t.hasMoreTokens() == true) {
					scanPathExecutables(t.nextToken() + "/" + file, filesBin);
				}
			} else
			if (type == "!exe") {
				StringTokenizer t;
				t.tokenize(installerProperties.get("exe_path", ""), ",");
				while (t.hasMoreTokens() == true) {
					scanPathExecutables(tdmePath + "/" + t.nextToken() + "/" + file, filesBin);
				}
			} else
			if (type == "res") {
				scanPathResources(file, filesData);
			} else
			if (type == "!res") {
				scanPathResources(tdmePath + "/" + file, filesData);
			} else
			if (type == "lib") {
				scanPathLibraries(file, filesData);
			} else
			if (type == "api") {
				scanPathHeaders(file, filesData);
			} else {
				Console::println("Component: " + to_string(componentIdx) + ": type = " + type + " unsupported!");
			}

			// process files
			Console::println("Component: " + to_string(componentIdx) + ": Processing files");
		}

		// add files to archive
		for (auto fileName: filesData) {
			processFile(fileName, fileInformations, "installer/" + componentFileName, false, tdmePath);
		}

		// add files to archive
		for (auto fileName: filesBin) {
			processFile(fileName, fileInformations, "installer/" + componentFileName, true, tdmePath);
		}

		// add file informations
		{
			ofstream ofs("installer/" + componentFileName, ofstream::binary | ofstream::app);
			ofs.seekp(0, ofstream::end);
			uint32_t fileInformationOffsetEnd = 0LL;
			uint64_t fileInformationOffset = ofs.tellp();
			for (auto& fileInformation: fileInformations) {
				uint32_t nameSize = fileInformation.name.size();
				ofs.write((char*)&nameSize, sizeof(nameSize));
				for (auto i = 0; i < nameSize; i++) ofs.write(&fileInformation.name[i], 1);
				ofs.write((char*)&fileInformation.bytes, sizeof(fileInformation.bytes));
				ofs.write((char*)&fileInformation.compressed, sizeof(fileInformation.compressed));
				ofs.write((char*)&fileInformation.bytesCompressed, sizeof(fileInformation.bytesCompressed));
				ofs.write((char*)&fileInformation.offset, sizeof(fileInformation.offset));
				ofs.write((char*)&fileInformation.executable, sizeof(fileInformation.executable));
			}
			ofs.write((char*)&fileInformationOffsetEnd, sizeof(fileInformationOffsetEnd));
			ofs.write((char*)&fileInformationOffset, sizeof(fileInformationOffset));
			ofs.close();
		}

		// sha256 hash
		auto archiveFileSystem = new ArchiveFileSystem("installer/" + componentFileName);
		auto archiveHash = archiveFileSystem->computeSHA256Hash();
		delete archiveFileSystem;
		FileSystem::getStandardFileSystem()->setContentFromString("installer", componentFileName + ".sha256", archiveHash);
		Console::println("Component: " + to_string(componentIdx) + ": component file name: " + componentFileName + ": hash: " + archiveHash);
	}

	// add completion file
	auto completionFileName = os + "-" + cpu + "-upload-" + fileNameTime;
	// reset archive
	{
		ofstream ofs("installer/" + completionFileName, ofstream::binary | ofstream::trunc);
		ofs.close();
	}
}
