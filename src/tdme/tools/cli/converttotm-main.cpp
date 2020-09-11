#include <string>
#include <cstdlib>

#include <tdme/application/Application.h>
#include <tdme/engine/fileio/models/ModelReader.h>
#include <tdme/engine/fileio/models/TMWriter.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>

using tdme::application::Application;
using tdme::engine::fileio::models::ModelReader;
using tdme::engine::fileio::models::TMWriter;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;
using tdme::utilities::Console;
using tdme::utilities::Exception;

int main(int argc, char** argv)
{
	Console::println(string("converttotm 1.9.9"));
	Console::println(string("Programmed 2018 by Andreas Drewke, drewke.net."));
	Console::println();
	if (argc != 3) {
		Console::println("Usage: converttotm inputfile outputfile");
		Application::exit(1);
	}
	string inputFileName = string(argv[1]);
	string outputFileName = string(argv[2]);
	try {
		Console::println("Loading model: " + inputFileName);
		auto model = ModelReader::read(
			FileSystem::getInstance()->getPathName(inputFileName),
			FileSystem::getInstance()->getFileName(inputFileName)
		);
		Console::println("Exporting model: " + outputFileName);
		TMWriter::write(
			model,
			FileSystem::getInstance()->getPathName(outputFileName),
			FileSystem::getInstance()->getFileName(outputFileName)
		);
	} catch (Exception& exception) {
		Console::println("An error occurred: " + string(exception.what()));
	}
}
