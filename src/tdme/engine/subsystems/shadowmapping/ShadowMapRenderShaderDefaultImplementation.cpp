#include <tdme/engine/subsystems/shadowmapping/ShadowMapRenderShaderDefaultImplementation.h>

#include <string>

#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>

using std::to_string;

using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::subsystems::shadowmapping::ShadowMapRenderShaderDefaultImplementation;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;

bool ShadowMapRenderShaderDefaultImplementation::isSupported(Renderer* renderer) {
	return true;
}

ShadowMapRenderShaderDefaultImplementation::ShadowMapRenderShaderDefaultImplementation(Renderer* renderer): ShadowMapRenderShaderBaseImplementation(renderer)
{
}

ShadowMapRenderShaderDefaultImplementation::~ShadowMapRenderShaderDefaultImplementation()
{
}

const string ShadowMapRenderShaderDefaultImplementation::getId() {
	return "default";
}

void ShadowMapRenderShaderDefaultImplementation::initialize()
{
	auto shaderVersion = renderer->getShaderVersion();

	// load shadow mapping shaders
	vertexShaderId = renderer->loadShader(
		renderer->SHADER_VERTEX_SHADER,
		"shader/" + shaderVersion + "/shadowmapping",
		"render_vertexshader.vert"
	);
	if (vertexShaderId == 0) return;

	fragmentShaderId = renderer->loadShader(
		renderer->SHADER_FRAGMENT_SHADER,
		"shader/" + shaderVersion + "/shadowmapping",
		"render_fragmentshader.frag"
	);
	if (fragmentShaderId == 0) return;

	// create shadow mapping render program
	programId = renderer->createProgram(renderer->PROGRAM_OBJECTS);
	renderer->attachShaderToProgram(programId, vertexShaderId);
	renderer->attachShaderToProgram(programId, fragmentShaderId);

	ShadowMapRenderShaderBaseImplementation::initialize();
}

void ShadowMapRenderShaderDefaultImplementation::updateShaderParameters(Renderer* renderer, void* context) {
}
