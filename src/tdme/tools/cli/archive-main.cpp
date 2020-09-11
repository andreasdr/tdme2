#include <cassert>
#include <fstream>
#include <string>
#include <vector>

#include <ext/zlib/zlib.h>

#include <tdme/application/Application.h>
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

void scanDir(const string& folder, vector<string>& totalFiles) {
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
				// audio
				if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".ogg") == true) return true;
				// models
				if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".tm") == true) return true;
				if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".dae") == true) return true;
				// meta
				if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".tmm") == true) return true;
				// level
				if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".tl") == true) return true;
				// particle system
				if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".tps") == true) return true;
				// xml
				if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".xml") == true) return true;
				// images
				if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".png") == true) return true;
				// fonts
				if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".fnt") == true) return true;
				//
				if (fileName.find(".") == string::npos) return true;
				//
				if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".properties") == true) return true;
				//
				return false;
			}
	};

	ListFilter listFilter;
	vector<string> files;

	FileSystem::getInstance()->list(folder, files, &listFilter);

	for (auto fileName: files) {
		if (FileSystem::getInstance()->isPath(folder + "/" + fileName) == false) {
			totalFiles.push_back(folder + "/" + fileName);
		} else {
			scanDir(folder + "/" + fileName, totalFiles);
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
	ofstream ofs("archive.ta", ofstream::binary | ofstream::app);
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
	fileInformation.name = fileName;
	fileInformation.bytes = content.size();
	fileInformation.compressed = compressed;
	fileInformation.bytesCompressed = bytesCompressed;
	fileInformation.offset = fileOffset;
	fileInformation.executable = false;
	fileInformations.push_back(fileInformation);

	// done
	Console::println(", processed " + to_string(content.size()) + " bytes" + (compressed == 1?", " + to_string(bytesCompressed) + " bytes compressed":""));
}

int main(int argc, char** argv)
{
	Console::println(string("archive 1.9.9"));
	Console::println(string("Programmed 2018 by Andreas Drewke, drewke.net."));
	Console::println();

	if (argc != 1) {
		Console::println("Usage: archive");
		Application::exit(1);
	}

	// scan files
	Console::println("Scanning files");
	vector<string> files;
	scanDir("resources", files);
	scanDir("shader", files);

	// processing
	Console::println("Processing files");

	// reset archive
	{
		ofstream ofs("archive.ta", ofstream::binary | ofstream::trunc);
		ofs.close();
	}

	// add files to archive
	vector<FileInformation> fileInformations;
	for (auto fileName: files) {
		processFile(fileName, fileInformations);
	}

	// add file informations
	{
		ofstream ofs("archive.ta", ofstream::binary | ofstream::app);
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
}
