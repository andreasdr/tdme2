#include <algorithm>
#include <string>
#include <vector>

#include <tdme/application/Application.h>
#include <tdme/engine/Version.h>
#include <tdme/math/Math.h>
#include <tdme/os/filesystem/FileNameFilter.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/utilities/Character.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>
#include <tdme/utilities/StringTokenizer.h>
#include <tdme/utilities/StringTools.h>

using std::sort;
using std::string;
using std::to_string;
using std::vector;

using tdme::application::Application;
using tdme::engine::Version;
using tdme::math::Math;
using tdme::os::filesystem::FileNameFilter;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;
using tdme::utilities::Character;
using tdme::utilities::Console;
using tdme::utilities::Exception;
using tdme::utilities::StringTokenizer;
using tdme::utilities::StringTools;

void scanDir(const string& folder, vector<string>& totalFiles) {
	class ListFilter : public virtual FileNameFilter {
		public:
			virtual ~ListFilter() {}

			bool accept(const string& pathName, const string& fileName) override {
				if (fileName == ".") return false;
				if (fileName == "..") return false;
				if (FileSystem::getInstance()->isPath(pathName + "/" + fileName) == true) return true;
				if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".h") == true) return true;
				if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".cpp") == true) return true;
				return false;
			}
	};

	ListFilter listFilter;
	vector<string> files;

	FileSystem::getInstance()->list(folder, files, &listFilter);

	for (auto fileName: files) {
		if (StringTools::endsWith(fileName, ".h") == true ||
			StringTools::endsWith(fileName, ".cpp") == true) {
			totalFiles.push_back(folder + "/" + fileName);
		} else {
			scanDir(folder + "/" + fileName, totalFiles);
		}
	}
}

static int string_compare(const string& lhs, const string& rhs) {
	if (StringTools::startsWith(lhs, "#include <tdme/tdme.h>") == true) return true; else
	if (StringTools::startsWith(rhs, "#include <tdme/tdme.h>") == true) return false;
	auto charCount = Math::min((int32_t)lhs.size(), (int32_t)rhs.size());
	for (auto i = 0; i < charCount; i++) {
		if (lhs[i] == rhs[i]) {
			// no op
		} else {
			auto charLHS = lhs[i];
			auto charLCLHS = Character::toLowerCase(lhs[i]);
			auto charLHSLowerCase = charLHS == charLCLHS;
			auto charRHS = rhs[i];
			auto charLCRHS = Character::toLowerCase(rhs[i]);
			auto charRHSLowerCase = charRHS == charLCRHS;
			if (charLHSLowerCase == true && charRHSLowerCase == false) {
				return true;
			} else
			if (charLHSLowerCase == false && charRHSLowerCase == true) {
				return false;
			} else {
				return lhs < rhs;
			}
		}
	}
	return lhs.size() < rhs.size();
}

void processFile(const string& fileName) {
	Console::println("Processing file: " + fileName);
	vector<string> fileContent;
	FileSystem::getInstance()->getContentAsStringArray(".", fileName, fileContent);
	vector<string> newFileContent;
	string method = "";
	{
		auto startLineIdx = -1;
		auto endLineIdx = -1;
		auto lineIdx = 0;
		for (auto line: fileContent) {
			newFileContent.push_back(line);
			if (StringTools::startsWith(line, "#include ") == true) {
				if (startLineIdx == -1) {
					startLineIdx = lineIdx;
					endLineIdx = lineIdx;
				} else {
					endLineIdx = lineIdx;
				}
			} else
			if (startLineIdx != -1 && endLineIdx != -1) {
				sort(newFileContent.begin() + startLineIdx, newFileContent.begin() + endLineIdx + 1, string_compare);
				startLineIdx = -1;
				endLineIdx = -1;
			}
			lineIdx++;
		}
	}
	fileContent = newFileContent;
	newFileContent.clear();
	{
		auto startLineIdx = -1;
		auto endLineIdx = -1;
		auto lineIdx = 0;
		for (auto line: fileContent) {
			newFileContent.push_back(line);
			if (StringTools::startsWith(line, "using ") == true) {
				if (startLineIdx == -1) {
					startLineIdx = lineIdx;
					endLineIdx = lineIdx;
				} else {
					endLineIdx = lineIdx;
				}
			} else
			if (startLineIdx != -1 && endLineIdx != -1) {
				sort(newFileContent.begin() + startLineIdx, newFileContent.begin() + endLineIdx + 1, string_compare);
				startLineIdx = -1;
				endLineIdx = -1;
			}
			lineIdx++;
		}
	}
	FileSystem::getInstance()->setContentFromStringArray(".", fileName, newFileContent);
}

int main(int argc, char** argv)
{
	Console::println(string("sortincludes ") + Version::getVersion());
	Console::println(Version::getCopyright());
	Console::println();

	if (argc != 2) {
		Console::println("Usage: sortincludes path_to_source");
		Application::exit(1);
	}

	auto pathToSource = string(argv[1]);

	Console::println("Scanning files");
	vector<string> files;
	scanDir(pathToSource, files);

	Console::println("Processing files");
	for (auto fileName: files) {
		processFile(fileName);
	}
}
