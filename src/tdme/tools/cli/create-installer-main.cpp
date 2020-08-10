#include <cassert>
#include <fstream>
#include <string>
#include <vector>

#include <ext/zlib/zlib.h>

#include <tdme/application/Application.h>
#include <tdme/os/filesystem/FileNameFilter.h>
#include <tdme/os/filesystem/ArchiveFileSystem.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/utils/Console.h>
#include <tdme/utils/Exception.h>
#include <tdme/utils/Properties.h>
#include <tdme/utils/StringTokenizer.h>
#include <tdme/utils/StringUtils.h>
#include <tdme/utils/Time.h>

using std::ofstream;
using std::string;
using std::to_string;
using std::vector;

using tdme::application::Application;
using tdme::os::filesystem::ArchiveFileSystem;
using tdme::os::filesystem::FileNameFilter;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;
using tdme::utils::Console;
using tdme::utils::Exception;
using tdme::utils::Properties;
using tdme::utils::StringTokenizer;
using tdme::utils::StringUtils;
using tdme::utils::Time;

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

static void scanDirResources(const string& folder, vector<string>& totalFiles) {
	class ListFilter : public virtual FileNameFilter {
		public:
			virtual ~ListFilter() {}

			bool accept(const string& pathName, const string& fileName) override {
				if (fileName == ".") return false;
				if (fileName == "..") return false;
				if (FileSystem::getInstance()->isPath(pathName + "/" + fileName) == true) return true;
				// shader
				if (StringUtils::endsWith(StringUtils::toLowerCase(fileName), ".glsl") == true) return true;
				if (StringUtils::endsWith(StringUtils::toLowerCase(fileName), ".vert") == true) return true;
				if (StringUtils::endsWith(StringUtils::toLowerCase(fileName), ".frag") == true) return true;
				if (StringUtils::endsWith(StringUtils::toLowerCase(fileName), ".cl") == true) return true;
				// audio
				if (StringUtils::endsWith(StringUtils::toLowerCase(fileName), ".ogg") == true) return true;
				// models
				if (StringUtils::endsWith(StringUtils::toLowerCase(fileName), ".tm") == true) return true;
				if (StringUtils::endsWith(StringUtils::toLowerCase(fileName), ".dae") == true) return true;
				if (StringUtils::endsWith(StringUtils::toLowerCase(fileName), ".fbx") == true) return true;
				if (StringUtils::endsWith(StringUtils::toLowerCase(fileName), ".glb") == true) return true;
				// meta
				if (StringUtils::endsWith(StringUtils::toLowerCase(fileName), ".tmm") == true) return true;
				// level
				if (StringUtils::endsWith(StringUtils::toLowerCase(fileName), ".tl") == true) return true;
				// particle system
				if (StringUtils::endsWith(StringUtils::toLowerCase(fileName), ".tps") == true) return true;
				// xml
				if (StringUtils::endsWith(StringUtils::toLowerCase(fileName), ".xml") == true) return true;
				// images
				if (StringUtils::endsWith(StringUtils::toLowerCase(fileName), ".ico") == true) return true;
				if (StringUtils::endsWith(StringUtils::toLowerCase(fileName), ".png") == true) return true;
				// fonts
				if (StringUtils::endsWith(StringUtils::toLowerCase(fileName), ".fnt") == true) return true;
				// fonts
				if (StringUtils::endsWith(StringUtils::toLowerCase(fileName), ".fnt") == true) return true;
				// files without ending
				//	TODO: fix me, paths get submitted here too as filename
				if (fileName.rfind(".") == string::npos ||
					(fileName.rfind("/") != string::npos &&
					fileName.rfind(".") < fileName.rfind("/"))) {
					return true;
				}
				// properties
				if (StringUtils::endsWith(StringUtils::toLowerCase(fileName), ".properties") == true) return true;
				// tdme archive
				if (StringUtils::endsWith(StringUtils::toLowerCase(fileName), ".ta") == true) return true;
				//
				return false;
			}
	};

	ListFilter listFilter;
	vector<string> files;

	if (FileSystem::getInstance()->fileExists(folder) == false) {
		Console::println("Error: scanDirResources: file does not exist: " + folder);
	} else
	if (FileSystem::getInstance()->isPath(folder) == false) {
		if (listFilter.accept(".", folder) == true) {
			totalFiles.push_back(folder);
		} else {
			Console::println("Error: scanDirResources: file exist, but does not match filter: " + folder);
		}
	} else {
		FileSystem::getInstance()->list(folder, files, &listFilter);
		for (auto fileName: files) {
			if (FileSystem::getInstance()->isPath(folder + "/" + fileName) == false) {
				totalFiles.push_back(folder + "/" + fileName);
			} else {
				scanDirResources(folder + "/" + fileName, totalFiles);
			}
		}
	}
}

static void scanDirLibraries(const string& folder, vector<string>& totalFiles) {
	class ListFilter : public virtual FileNameFilter {
		public:
			virtual ~ListFilter() {}

			bool accept(const string& pathName, const string& fileName) override {
				if (fileName == ".") return false;
				if (fileName == "..") return false;
				if (FileSystem::getInstance()->isPath(pathName + "/" + fileName) == true) return true;
				// static libraries
				if (StringUtils::endsWith(StringUtils::toLowerCase(fileName), ".a") == true) return true;
				// dynamic libraries
				#if defined(_WIN32)
					if (StringUtils::endsWith(StringUtils::toLowerCase(fileName), ".dll") == true) return true;
					if (StringUtils::endsWith(StringUtils::toLowerCase(fileName), ".lib") == true) return true;
				#elif defined(__APPLE__)
					if (StringUtils::endsWith(StringUtils::toLowerCase(fileName), ".dylib") == true) return true;
					if (StringUtils::endsWith(StringUtils::toLowerCase(fileName), ".so") == true) return true;
				#else
					if (StringUtils::endsWith(StringUtils::toLowerCase(fileName), ".so") == true) return true;
				#endif
				//
				return false;
			}
	};

	ListFilter listFilter;
	vector<string> files;

	if (FileSystem::getInstance()->fileExists(folder) == false) {
		Console::println("Error: scanDirLibraries: file does not exist: " + folder);
	} else
	if (FileSystem::getInstance()->isPath(folder) == false) {
		if (listFilter.accept(".", folder) == true) {
			totalFiles.push_back(folder);
		} else {
			Console::println("Error: scanDirLibraries: file exist, but does not match filter: " + folder);
		}
	} else {
		FileSystem::getInstance()->list(folder, files, &listFilter);
		for (auto fileName: files) {
			if (FileSystem::getInstance()->isPath(folder + "/" + fileName) == false) {
				totalFiles.push_back(folder + "/" + fileName);
			} else {
				scanDirLibraries(folder + "/" + fileName, totalFiles);
			}
		}
	}
}

static void scanDirHeaders(const string& folder, vector<string>& totalFiles) {
	class ListFilter : public virtual FileNameFilter {
		public:
			virtual ~ListFilter() {}

			bool accept(const string& pathName, const string& fileName) override {
				if (fileName == ".") return false;
				if (fileName == "..") return false;
				if (FileSystem::getInstance()->isPath(pathName + "/" + fileName) == true) return true;
				// headers
				if (StringUtils::endsWith(StringUtils::toLowerCase(fileName), ".h") == true) return true;
				//
				return false;
			}
	};

	ListFilter listFilter;
	vector<string> files;

	if (FileSystem::getInstance()->fileExists(folder) == false) {
		Console::println("Error: scanDirHeaders: file does not exist: " + folder);
	} else
	if (FileSystem::getInstance()->isPath(folder) == false) {
		if (listFilter.accept(".", folder) == true) {
			totalFiles.push_back(folder);
		} else {
			Console::println("Error: scanDirHeaders: file exist, but does not match filter: " + folder);
		}
	} else {
		FileSystem::getInstance()->list(folder, files, &listFilter);
		for (auto fileName: files) {
			if (FileSystem::getInstance()->isPath(folder + "/" + fileName) == false) {
				totalFiles.push_back(folder + "/" + fileName);
			} else {
				scanDirHeaders(folder + "/" + fileName, totalFiles);
			}
		}
	}
}

static void scanDirExecutables(const string& folder, vector<string>& totalFiles) {
	class ListFilter : public virtual FileNameFilter {
		public:
			virtual ~ListFilter() {}

			bool accept(const string& pathName, const string& fileName) override {
				if (fileName == ".") return false;
				if (fileName == "..") return false;
				if (FileSystem::getInstance()->isPath(pathName + "/" + fileName) == true) return true;
				#if defined(_WIN32)
					if (StringUtils::endsWith(StringUtils::toLowerCase(fileName), ".exe") == true) return true;
					if (StringUtils::endsWith(StringUtils::toLowerCase(fileName), ".dll") == true) return true;
					if (StringUtils::endsWith(StringUtils::toLowerCase(fileName), ".bat") == true) return true;
				#elif defined(__APPLE__)
					// TODO: fix me, paths get submitted here too as filename
					if (fileName.rfind(".") == string::npos ||
						(fileName.rfind("/") != string::npos &&
						fileName.rfind(".") < fileName.rfind("/"))) {
						return true;
					}
					if (StringUtils::endsWith(StringUtils::toLowerCase(fileName), ".dylib") == true) return true;
					if (StringUtils::endsWith(StringUtils::toLowerCase(fileName), ".so") == true) return true;
					if (StringUtils::endsWith(StringUtils::toLowerCase(fileName), ".sh") == true) return true;
				#else
					// TODO: fix me, paths get submitted here too as filename
					if (fileName.rfind(".") == string::npos ||
						(fileName.rfind("/") != string::npos &&
						fileName.rfind(".") < fileName.rfind("/"))) {
						return true;
					}
					if (StringUtils::endsWith(StringUtils::toLowerCase(fileName), ".so") == true) return true;
					if (StringUtils::endsWith(StringUtils::toLowerCase(fileName), ".sh") == true) return true;
				#endif
				//
				return false;
			}
	};

	ListFilter listFilter;
	vector<string> files;

	if (FileSystem::getInstance()->fileExists(folder) == false) {
		Console::println("Error: scanDirExecutables: file does not exist: " + folder);
	} else
	if (FileSystem::getInstance()->isPath(folder) == false) {
		if (listFilter.accept(".", folder) == true) {
			totalFiles.push_back(folder);
		} else {
			Console::println("Error: scanDirExecutables: file exist, but does not match filter: " + folder);
		}
	} else {
		FileSystem::getInstance()->list(folder, files, &listFilter);
		for (auto fileName: files) {
			if (FileSystem::getInstance()->isPath(folder + "/" + fileName) == false) {
				totalFiles.push_back(folder + "/" + fileName);
			} else {
				scanDirExecutables(folder + "/" + fileName, totalFiles);
			}
		}
	}
}

void processFile(const string& fileName, vector<FileInformation>& fileInformations, const string& archiveFileName, bool executableFile, const string& baseFolder = string()) {
	// read content
	vector<uint8_t> content;
	FileSystem::getInstance()->getContent(
		FileSystem::getInstance()->getPathName(fileName),
		FileSystem::getInstance()->getFileName(fileName),
		content
	);

	auto fileNameToUse = StringUtils::startsWith(fileName, baseFolder + "/") == true?StringUtils::substring(fileName, (baseFolder + "/").size(), fileName.size()):fileName;
	// remove prefix if requested
	if (executableFile == true && fileName.find_last_of('/') != string::npos) {
		fileNameToUse = StringUtils::substring(fileNameToUse, fileNameToUse.find_last_of('/') + 1);
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
	string tdmeFolder = "../tdme2";
	auto cpu = Application::getCPUName();
	auto os = Application::getOSName();
	auto fileNameTime = StringUtils::replace(StringUtils::replace(StringUtils::replace(Time::getAsString(), " ", "-" ), ":", ""), "-", "");

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
		auto componentFileName = os + "-" + cpu + "-" + StringUtils::replace(StringUtils::replace(componentName, " - ", "-"), " ", "-") + "-" + fileNameTime + ".ta";
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
				t.tokenize(installerProperties.get("exe_folder", ""), ",");
				while (t.hasMoreTokens() == true) {
					scanDirExecutables(t.nextToken() + "/" + file, filesBin);
				}
			} else
			if (type == "!exe") {
				StringTokenizer t;
				t.tokenize(installerProperties.get("exe_folder", ""), ",");
				while (t.hasMoreTokens() == true) {
					scanDirExecutables(tdmeFolder + "/" + t.nextToken() + "/" + file, filesBin);
				}
			} else
			if (type == "res") {
				scanDirResources(file, filesData);
			} else
			if (type == "!res") {
				scanDirResources(tdmeFolder + "/" + file, filesData);
			} else
			if (type == "lib") {
				scanDirLibraries(file, filesData);
			} else
			if (type == "api") {
				scanDirHeaders(file, filesData);
			} else {
				Console::println("Component: " + to_string(componentIdx) + ": type = " + type + " unsupported!");
			}

			// process files
			Console::println("Component: " + to_string(componentIdx) + ": Processing files");
		}

		// add files to archive
		for (auto fileName: filesData) {
			processFile(fileName, fileInformations, "installer/" + componentFileName, false, tdmeFolder);
		}

		// add files to archive
		for (auto fileName: filesBin) {
			processFile(fileName, fileInformations, "installer/" + componentFileName, true, tdmeFolder);
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
