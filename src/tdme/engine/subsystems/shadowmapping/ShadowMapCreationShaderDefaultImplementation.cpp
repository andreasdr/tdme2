#include <tdme/engine/subsystems/shadowmapping/ShadowMapCreationShaderDefaultImplementation.h>

#include <string>

#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>

using std::to_string;

using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::subsystems::shadowmapping::ShadowMapCreationShaderDefaultImplementation;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;

bool ShadowMapCreationShaderDefaultImplementation::isSupported(Renderer* renderer) {
	return true;
}

ShadowMapCreationShaderDefaultImplementation::ShadowMapCreationShaderDefaultImplementation(Renderer* renderer): ShadowMapCreationShaderBaseImplementation(renderer)
{
}

ShadowMapCreationShaderDefaultImplementation::~ShadowMapCreationShaderDefaultImplementation() {
}

const string ShadowMapCreationShaderDefaultImplementation::getId() {
	return "default";
}

void ShadowMapCreationShaderDefaultImplementation::initialize()
{
	auto shaderVersion = renderer->getShaderVersion();

	// load shadow mapping creation shaders
	vertexShaderId = renderer->loadShader(
		renderer->SHADER_VERTEX_SHADER,
		"shader/" + shaderVersion + "/shadowmapping",
		"creation_vertexshader.vert"
	);
	if (vertexShaderId == 0) return;
	fragmentShaderId = renderer->loadShader(
		renderer->SHADER_FRAGMENT_SHADER,
		"shader/" + shaderVersion + "/shadowmapping",
		"creation_fragmentshader.frag"
	);
	if (fragmentShaderId == 0) return;

	// create shadow map creation program
	programId = renderer->createProgram(renderer->PROGRAM_OBJECTS);
	renderer->attachShaderToProgram(programId, vertexShaderId);
	renderer->attachShaderToProgram(programId, fragmentShaderId);

	//
	ShadowMapCreationShaderBaseImplementation::initialize();
}

