#include <string>

#include "PostProcessingShaderDesaturationImplementation.h"
#include <tdme/engine/subsystems/postprocessing/PostProcessingShaderBaseImplementation.h>
#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/ShaderParameter.h>

using std::string;

using tdme::engine::subsystems::postprocessing::PostProcessingShaderDesaturationImplementation;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::Engine;
using tdme::engine::ShaderParameter;

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

	//
	if (initialized == false) return;

	// uniforms
	uniformIntensity = renderer->getProgramUniformLocation(programId, "intensity");

	// register shader
	Engine::registerShader(
		Engine::ShaderType::SHADERTYPE_POSTPROCESSING,
		"desaturation",
		{{ "intensity", ShaderParameter(0.0f) }}
	);
}

void PostProcessingShaderDesaturationImplementation::setShaderParameters(void* context, Engine* engine) {
	if (uniformIntensity != -1) renderer->setProgramUniformFloat(context, uniformIntensity, engine->getShaderParameter("desaturation", "intensity").getFloatValue());
}

