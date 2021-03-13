#include <tdme/engine/subsystems/lighting/LightingShaderLightScatteringFoliageImplementation.h>

#include <string>

#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/engine/Engine.h>

#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>

using std::string;
using std::to_string;

using tdme::engine::subsystems::lighting::LightingShaderBaseImplementation;
using tdme::engine::subsystems::lighting::LightingShaderLightScatteringFoliageImplementation;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::Engine;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;

bool LightingShaderLightScatteringFoliageImplementation::isSupported(Renderer* renderer) {
	return renderer->getShaderVersion() == "gl3";
}

LightingShaderLightScatteringFoliageImplementation::LightingShaderLightScatteringFoliageImplementation(Renderer* renderer): LightingShaderBaseImplementation(renderer)
{
}

const string LightingShaderLightScatteringFoliageImplementation::getId() {
	return "ls_foliage";
}

void LightingShaderLightScatteringFoliageImplementation::initialize()
{
	auto shaderVersion = renderer->getShaderVersion();

	// lighting
	//	fragment shader
	renderLightingFragmentShaderId = renderer->loadShader(
		renderer->SHADER_FRAGMENT_SHADER,
		"shader/" + shaderVersion + "/lighting/light_scattering",
		"render_fragmentshader.frag"
	);
	if (renderLightingFragmentShaderId == 0) return;

	//	vertex shader
	renderLightingVertexShaderId = renderer->loadShader(
		renderer->SHADER_VERTEX_SHADER,
		"shader/" + shaderVersion + "/lighting/light_scattering",
		"render_vertexshader.vert",
		"#define HAVE_FOLIAGE",
		FileSystem::getInstance()->getContentAsString(
			"shader/" + shaderVersion + "/functions",
			"create_rotation_matrix.inc.glsl"
		) +
		"\n\n" +
		FileSystem::getInstance()->getContentAsString(
			"shader/" + shaderVersion + "/functions",
			"create_translation_matrix.inc.glsl"
		) +
		"\n\n" +
		FileSystem::getInstance()->getContentAsString(
			"shader/" + shaderVersion + "/functions",
			"create_foliage_transform_matrix.inc.glsl"
		)
	);
	if (renderLightingVertexShaderId == 0) return;

	// create, attach and link program
	programId = renderer->createProgram(renderer->PROGRAM_OBJECTS);
	renderer->attachShaderToProgram(programId, renderLightingVertexShaderId);
	renderer->attachShaderToProgram(programId, renderLightingFragmentShaderId);

	//
	LightingShaderBaseImplementation::initialize();

	//
	if (initialized == false) return;

	// uniforms
	uniformSpeed = renderer->getProgramUniformLocation(programId, "speed");
	uniformAmplitudeDefault = renderer->getProgramUniformLocation(programId, "amplitudeDefault");
	uniformAmplitudeMax = renderer->getProgramUniformLocation(programId, "amplitudeMax");
}

void LightingShaderLightScatteringFoliageImplementation::registerShader() {
}

void LightingShaderLightScatteringFoliageImplementation::updateShaderParameters(Renderer* renderer, void* context) {
	auto& shaderParameters = renderer->getShaderParameters(context);
	if (uniformSpeed != -1) renderer->setProgramUniformFloat(context, uniformSpeed, shaderParameters.getShaderParameter("speed").getFloatValue());
	if (uniformAmplitudeDefault != -1) renderer->setProgramUniformFloat(context, uniformAmplitudeDefault, shaderParameters.getShaderParameter("amplitudeDefault").getFloatValue());
	if (uniformAmplitudeMax != -1) renderer->setProgramUniformFloat(context, uniformAmplitudeMax, shaderParameters.getShaderParameter("amplitudeMax").getFloatValue());
}
