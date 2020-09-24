#include <string>
#include <cstdlib>

#include <tdme/application/Application.h>
#include <tdme/engine/fileio/models/ModelReader.h>
#include <tdme/engine/fileio/models/TMWriter.h>
#include <tdme/engine/model/Model.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>
#include <tdme/utilities/ModelTools.h>
#include <tdme/utilities/StringTools.h>

using tdme::application::Application;
using tdme::engine::fileio::models::ModelReader;
using tdme::engine::fileio::models::TMWriter;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;
using tdme::utilities::Console;
using tdme::utilities::Exception;
using tdme::utilities::ModelTools;
using tdme::utilities::StringTools;

int main(int argc, char** argv)
{
	Console::println(string("optimizemodel 1.9.9"));
	Console::println(string("Programmed 2020 by Andreas Drewke, drewke.net."));
	Console::println();
	if (argc < 2) {
		Console::println("Usage: optimizemodel file.tm [exclude_materials_with_specific_filename1] [exclude_materials_with_specific_filenameN]");
		Application::exit(1);
	}
	string fileName = string(argv[1]);
	if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".tm") == false) {
		Console::println("optimizemodel only accepts .tm model files");
		Application::exit(1);
	}
	vector<string> excludeDiffuseTextureFileNamePatterns;
	for (auto i = 2; i < argc; i++) excludeDiffuseTextureFileNamePatterns.push_back(argv[i]);
	try {
		Console::println("Loading model: " + fileName);
		auto model = ModelReader::read(
			FileSystem::getInstance()->getPathName(fileName),
			FileSystem::getInstance()->getFileName(fileName)
		);
		auto optimizedModel = model;
		if (ModelTools::isOptimizedModel(model) == true) {
			Console::println("Already optimized. Skipping.");
		} else {
			Console::println("Optimizing model: " + fileName);
			optimizedModel = ModelTools::optimizeModel(model, FileSystem::getInstance()->getPathName(fileName), excludeDiffuseTextureFileNamePatterns);
			if (optimizedModel == model) {
				Console::println("No optimization was required. Skipping.");
			} else {
				Console::println("Exporting model: " + fileName);
				TMWriter::write(
					optimizedModel,
					FileSystem::getInstance()->getPathName(fileName),
					FileSystem::getInstance()->getFileName(fileName)
				);
			}
		}
		delete optimizedModel;
	} catch (Exception& exception) {
		Console::println("An error occurred: " + string(exception.what()));
	}
}
