#include <algorithm>
#include <string>
#include <vector>

#include <tdme/application/Application.h>
#include <tdme/engine/Version.h>
#include <tdme/os/filesystem/FileNameFilter.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>
#include <tdme/utilities/StringTools.h>

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
using tdme::utilities::StringTools;

void scanDir(const string& folder, vector<string>& sourceFiles, vector<string>& mainSourceFiles) {
	class SourceFilesFilter : public virtual FileNameFilter {
		public:
			virtual ~SourceFilesFilter() {}

			bool accept(const string& pathName, const string& fileName) override {
				if (fileName == ".") return false;
				if (fileName == "..") return false;
				if (FileSystem::getInstance()->isPath(pathName + "/" + fileName) == true) return true;
				if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".cpp") == true) return true;
				return false;
			}
	};

	SourceFilesFilter sourceFilesFilter;
	vector<string> files;

	FileSystem::getInstance()->list(folder, files, &sourceFilesFilter);

	for (auto fileName: files) {
		if (StringTools::endsWith(fileName, "-main.cpp") == true) {
			mainSourceFiles.push_back(folder + "/" + fileName);
		} else
		if (StringTools::endsWith(fileName, ".cpp") == true) {
			sourceFiles.push_back(folder + "/" + fileName);
		} else {
			scanDir(folder + "/" + fileName, sourceFiles, mainSourceFiles);
		}
	}
}

int main(int argc, char** argv)
{
	Console::println(string("makefilegenerator ") + Version::getVersion());
	Console::println(Version::getCopyright());
	Console::println();

	if (argc != 2) {
		Console::println("Usage: makefilegenerator path_to_source");
		Application::exit(1);
	}

	auto pathToSource = string(argv[1]);

	try {
		Console::println("Scanning source files");
		vector<string> sourceFiles;
		vector<string> mainSourceFiles;
		scanDir(pathToSource, sourceFiles, mainSourceFiles);

		string sourceFilesVariable = "\\\n";
		for (auto& file: sourceFiles) sourceFilesVariable+= "\t" + file + "\\\n";
		sourceFilesVariable+= "\n";

		string mainSourceFilesVariable = "\\\n";
		for (auto& file: mainSourceFiles) mainSourceFilesVariable+= "\t" + file + "\\\n";
		mainSourceFilesVariable+= "\n";

		Console::println("Generating Makefile");

		auto executableFolder = string(argv[0]);
		auto tdme2Folder = StringTools::substring(executableFolder, 0, executableFolder.rfind("/tdme2/") + string("/tdme2/").length());

		auto makefileSource = FileSystem::getInstance()->getContentAsString(tdme2Folder + "/resources/engine/templates/makefiles", "Makefile");
		makefileSource = StringTools::replace(makefileSource, "{$source-files}", sourceFilesVariable);
		makefileSource = StringTools::replace(makefileSource, "{$main-source-files}", mainSourceFilesVariable);
		FileSystem::getInstance()->setContentFromString(".", "Makefile", makefileSource);
	} catch (Exception& exception) {
		Console::println("An error occurred: " + string(exception.what()));
	}
}
