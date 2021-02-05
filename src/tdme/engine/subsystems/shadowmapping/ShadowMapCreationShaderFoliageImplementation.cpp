#include <tdme/engine/subsystems/shadowmapping/ShadowMapCreationShaderFoliageImplementation.h>

#include <string>

#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/engine/EntityShaderParameters.h>
#include <tdme/engine/ShaderParameter.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>

using std::to_string;

using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::subsystems::shadowmapping::ShadowMapCreationShaderFoliageImplementation;
using tdme::engine::EntityShaderParameters;
using tdme::engine::ShaderParameter;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;

bool ShadowMapCreationShaderFoliageImplementation::isSupported(Renderer* renderer) {
	return true;
}

ShadowMapCreationShaderFoliageImplementation::ShadowMapCreationShaderFoliageImplementation(Renderer* renderer): ShadowMapCreationShaderBaseImplementation(renderer)
{
}

ShadowMapCreationShaderFoliageImplementation::~ShadowMapCreationShaderFoliageImplementation() {
}

const string ShadowMapCreationShaderFoliageImplementation::getId() {
	return "foliage";
}

void ShadowMapCreationShaderFoliageImplementation::initialize()
{
	auto shaderVersion = renderer->getShaderVersion();

	// load shadow mapping creation shaders
	vertexShaderId = renderer->loadShader(
		renderer->SHADER_VERTEX_SHADER,
		"shader/" + shaderVersion + "/shadowmapping",
		"creation_vertexshader.vert",
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

	//
	if (initialized == false) return;

	// uniforms
	uniformSpeed = renderer->getProgramUniformLocation(programId, "speed");
	uniformAmplitudeDefault = renderer->getProgramUniformLocation(programId, "amplitudeDefault");
	uniformAmplitudeMax = renderer->getProgramUniformLocation(programId, "amplitudeMax");
}

void ShadowMapCreationShaderFoliageImplementation::updateShaderParameters(Renderer* renderer, void* context) {
	auto& shaderParameters = renderer->getShaderParameters(context);
	if (uniformSpeed != -1) renderer->setProgramUniformFloat(context, uniformSpeed, shaderParameters.getShaderParameter("speed").getFloatValue());
	if (uniformAmplitudeDefault != -1) renderer->setProgramUniformFloat(context, uniformAmplitudeDefault, shaderParameters.getShaderParameter("amplitudeDefault").getFloatValue());
	if (uniformAmplitudeMax != -1) renderer->setProgramUniformFloat(context, uniformAmplitudeMax, shaderParameters.getShaderParameter("amplitudeMax").getFloatValue());
}
