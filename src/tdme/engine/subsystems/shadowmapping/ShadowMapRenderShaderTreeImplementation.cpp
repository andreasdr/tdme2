#include <tdme/engine/subsystems/shadowmapping/ShadowMapRenderShaderTreeImplementation.h>

#include <string>

#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/engine/EntityShaderParameters.h>
#include <tdme/engine/ShaderParameter.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>

using std::to_string;

using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::subsystems::shadowmapping::ShadowMapRenderShaderTreeImplementation;
using tdme::engine::EntityShaderParameters;
using tdme::engine::ShaderParameter;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;

bool ShadowMapRenderShaderTreeImplementation::isSupported(Renderer* renderer) {
	return true;
}

ShadowMapRenderShaderTreeImplementation::ShadowMapRenderShaderTreeImplementation(Renderer* renderer): ShadowMapRenderShaderBaseImplementation(renderer)
{
}

ShadowMapRenderShaderTreeImplementation::~ShadowMapRenderShaderTreeImplementation()
{
}

const string ShadowMapRenderShaderTreeImplementation::getId() {
	return "tree";
}

void ShadowMapRenderShaderTreeImplementation::initialize()
{
	auto shaderVersion = renderer->getShaderVersion();

	// load shadow mapping shaders
	vertexShaderId = renderer->loadShader(
		renderer->SHADER_VERTEX_SHADER,
		"shader/" + shaderVersion + "/shadowmapping",
		"render_vertexshader.vert",
		"#define HAVE_TREE",
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
			"create_tree_transform_matrix.inc.glsl"
		)
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

	//
	if (initialized == false) return;

	// uniforms
	uniformSpeed = renderer->getProgramUniformLocation(programId, "speed");
}

void ShadowMapRenderShaderTreeImplementation::updateShaderParameters(Renderer* renderer, void* context) {
	auto& shaderParameters = renderer->getShaderParameters(context);
	if (uniformSpeed != -1) renderer->setProgramUniformFloat(context, uniformSpeed, shaderParameters.getShaderParameter("speed").getFloatValue());
}
