#include <tdme/engine/subsystems/earlyzrejection/EZRShaderDefaultImplementation.h>

#include <string>

#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>

using std::string;

using tdme::engine::subsystems::earlyzrejection::EZRShaderDefaultImplementation;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;

bool EZRShaderDefaultImplementation::isSupported(Renderer* renderer) {
	return true;
}

EZRShaderDefaultImplementation::EZRShaderDefaultImplementation(Renderer* renderer): EZRShaderBaseImplementation(renderer)
{
}

EZRShaderDefaultImplementation::~EZRShaderDefaultImplementation() {
}

const string EZRShaderDefaultImplementation::getId() {
	return "default";
}

void EZRShaderDefaultImplementation::initialize()
{
	auto shaderVersion = renderer->getShaderVersion();

	// load EZR shaders
	vertexShaderId = renderer->loadShader(
		renderer->SHADER_VERTEX_SHADER,
		"shader/" + shaderVersion + "/earlyzrejection",
		"vertexshader.vert"
	);
	if (vertexShaderId == 0) return;
	fragmentShaderId = renderer->loadShader(
		renderer->SHADER_FRAGMENT_SHADER,
		"shader/" + shaderVersion + "/earlyzrejection",
		"fragmentshader.frag"
	);
	if (fragmentShaderId == 0) return;

	// create EZR render program
	programId = renderer->createProgram(renderer->PROGRAM_OBJECTS);
	renderer->attachShaderToProgram(programId, vertexShaderId);
	renderer->attachShaderToProgram(programId, fragmentShaderId);

	//
	EZRShaderBaseImplementation::initialize();
}

