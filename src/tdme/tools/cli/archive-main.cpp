#include <cassert>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include <ext/zlib/zlib.h>

#include <tdme/tdme.h>
#include <tdme/application/Application.h>
#include <tdme/engine/Version.h>
#include <tdme/os/filesystem/FileNameFilter.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>
#include <tdme/utilities/StringTokenizer.h>
#include <tdme/utilities/StringTools.h>

using std::ofstream;
using std::string;
using std::to_string;
using std::vector;

using tdme::application::Application;
using tdme::engine::Version;
using tdme::os::filesystem::FileNameFilter;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;
using tdme::utilities::Console;
using tdme::utilities::Exception;
using tdme::utilities::StringTokenizer;
using tdme::utilities::StringTools;

namespace tdme {
namespace tools {
namespace cli {
namespace archive {
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

using tdme::tools::cli::archive::FileInformation;

void scanPath(const string& path, vector<string>& totalFiles) {
	class ListFilter : public FileNameFilter {
		public:
			virtual ~ListFilter() {}

			bool accept(const string& pathName, const string& fileName) override {
				if (fileName == ".") return false;
				if (fileName == "..") return false;
				if (FileSystem::getInstance()->isPath(pathName + "/" + fileName) == true) return true;
				auto fileNameLowerCase = StringTools::toLowerCase(fileName);
				// audio
				if (StringTools::endsWith(fileNameLowerCase, ".ogg") == true) return true;
				// fonts
				if (StringTools::endsWith(fileNameLowerCase, ".ttf") == true) return true;
				// images
				if (StringTools::endsWith(fileNameLowerCase, ".icns") == true) return true;
				if (StringTools::endsWith(fileNameLowerCase, ".ico") == true) return true;
				if (StringTools::endsWith(fileNameLowerCase, ".png") == true) return true;
				// videos
				if (StringTools::endsWith(fileNameLowerCase, ".mpg") == true) return true;
				// models
				if (StringTools::endsWith(fileNameLowerCase, ".dae") == true) return true;
				if (StringTools::endsWith(fileNameLowerCase, ".fbx") == true) return true;
				if (StringTools::endsWith(fileNameLowerCase, ".glb") == true) return true;
				if (StringTools::endsWith(fileNameLowerCase, ".tm") == true) return true;
				// property files
				if (StringTools::endsWith(fileNameLowerCase, ".properties") == true) return true;
				// shader
				if (StringTools::endsWith(fileNameLowerCase, ".cl") == true) return true;
				if (StringTools::endsWith(fileNameLowerCase, ".frag") == true) return true;
				if (StringTools::endsWith(fileNameLowerCase, ".glsl") == true) return true;
				if (StringTools::endsWith(fileNameLowerCase, ".vert") == true) return true;
				// tdme model
				if (StringTools::endsWith(fileNameLowerCase, ".tmodel") == true) return true;
				// tdme scene
				if (StringTools::endsWith(fileNameLowerCase, ".tscene") == true) return true;
				// tdme particle system
				if (StringTools::endsWith(fileNameLowerCase, ".tparticle") == true) return true;
				// tdme terrain
				if (StringTools::endsWith(fileNameLowerCase, ".tterrain") == true) return true;
				// tdme script
				if (StringTools::endsWith(fileNameLowerCase, ".tscript") == true) return true;
				// xml
				if (StringTools::endsWith(fileNameLowerCase, ".xml") == true) return true;
				// files without ending
				if (fileName.rfind(".") == string::npos ||
					(fileName.rfind("/") != string::npos &&
					fileName.rfind(".") < fileName.rfind("/"))) {
					return true;
				}
				//
				return false;
			}
	};

	ListFilter listFilter;
	vector<string> files;

	FileSystem::getInstance()->list(path, files, &listFilter);

	for (const auto& fileName: files) {
		if (FileSystem::getInstance()->isPath(path + "/" + fileName) == false) {
			totalFiles.push_back(path + "/" + fileName);
		} else {
			scanPath(path + "/" + fileName, totalFiles);
		}
	}
}

void processFile(const string& fileName, vector<FileInformation>& fileInformations) {
	Console::print("Processing file: " + fileName);

	// read content
	vector<uint8_t> content;
	FileSystem::getInstance()->getContent(
		FileSystem::getInstance()->getPathName(fileName),
		FileSystem::getInstance()->getFileName(fileName),
		content
	);

	// append to archive
	ofstream ofs(std::filesystem::u8path("archive.ta"), ofstream::binary | ofstream::app);
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
			Console::printLine("processFile(): Error compressing file: Aborting");
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
	fileInformation.name = fileName;
	fileInformation.bytes = content.size();
	fileInformation.compressed = compressed;
	fileInformation.bytesCompressed = bytesCompressed;
	fileInformation.offset = fileOffset;
	fileInformation.executable = false;
	fileInformations.push_back(fileInformation);

	// done
	Console::printLine(", processed " + to_string(content.size()) + " bytes" + (compressed == 1?", " + to_string(bytesCompressed) + " bytes compressed":""));
}

int main(int argc, char** argv)
{
	Console::printLine(string("archive ") + Version::getVersion());
	Console::printLine(Version::getCopyright());
	Console::printLine();

	if (argc != 1) {
		Console::printLine("Usage: archive");
		Application::exit(Application::EXITCODE_FAILURE);
	}

	// scan files
	Console::printLine("Scanning files");
	vector<string> files;
	scanPath("resources", files);
	scanPath("shader", files);

	// processing
	Console::printLine("Processing files");

	// reset archive
	{
		ofstream ofs(std::filesystem::u8path("archive.ta"), ofstream::binary | ofstream::trunc);
		ofs.close();
	}

	// add files to archive
	vector<FileInformation> fileInformations;
	for (const auto& fileName: files) {
		processFile(fileName, fileInformations);
	}

	// add file informations
	{
		ofstream ofs(std::filesystem::u8path("archive.ta"), ofstream::binary | ofstream::app);
		ofs.seekp(0, ofstream::end);
		uint32_t fileInformationOffsetEnd = 0LL;
		uint64_t fileInformationOffset = ofs.tellp();
		for (const auto& fileInformation: fileInformations) {
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

	//
	Application::exit(Application::EXITCODE_SUCCESS);
}
