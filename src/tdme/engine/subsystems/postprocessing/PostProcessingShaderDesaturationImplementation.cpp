#include <string>

#include <tdme/engine/Engine.h>
#include <tdme/engine/subsystems/postprocessing/PostProcessingShaderBaseImplementation.h>
#include <tdme/engine/subsystems/renderer/Renderer.h>
#include "PostProcessingShaderDesaturationImplementation.h"

using std::string;

using tdme::engine::subsystems::postprocessing::PostProcessingShaderDesaturationImplementation;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::Engine;

bool PostProcessingShaderDesaturationImplementation::isSupported(Renderer* renderer) {
	return renderer->getShaderVersion() == "gl3";
}

PostProcessingShaderDesaturationImplementation::PostProcessingShaderDesaturationImplementation(Renderer* renderer): PostProcessingShaderBaseImplementation(renderer)
{
}

void PostProcessingShaderDesaturationImplementation::initialize()
{
	auto shaderVersion = renderer->getShaderVersion();

	//	fragment shader
	fragmentShaderId = renderer->loadShader(
		renderer->SHADER_FRAGMENT_SHADER,
		"shader/" + shaderVersion + "/postprocessing",
		"desaturation_fragmentshader.frag",
		Engine::is4K() == true?"#define HAVE_4K":""
	);
	if (fragmentShaderId == 0) return;

	//	vertex shader
	vertexShaderId = renderer->loadShader(
		renderer->SHADER_VERTEX_SHADER,
		"shader/" + shaderVersion + "/postprocessing",
		"desaturation_vertexshader.vert"
	);
	if (vertexShaderId == 0) return;

	// create, attach and link program
	programId = renderer->createProgram(renderer->PROGRAM_OBJECTS);
	renderer->attachShaderToProgram(programId, vertexShaderId);
	renderer->attachShaderToProgram(programId, fragmentShaderId);

	//
	PostProcessingShaderBaseImplementation::initialize();
}
