#include <tdme/engine/subsystems/shadowmapping/ShadowMapRenderShaderTreeImplementation.h>

#include <string>

#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>

using std::to_string;

using tdme::engine::subsystems::shadowmapping::ShadowMapRenderShaderTreeImplementation;
using tdme::engine::subsystems::renderer::Renderer;
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
	renderVertexShaderId = renderer->loadShader(
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
	if (renderVertexShaderId == 0) return;

	renderFragmentShaderId = renderer->loadShader(
		renderer->SHADER_FRAGMENT_SHADER,
		"shader/" + shaderVersion + "/shadowmapping",
		"render_fragmentshader.frag"
	);
	if (renderFragmentShaderId == 0) return;

	// create shadow mapping render program
	renderProgramId = renderer->createProgram(renderer->PROGRAM_OBJECTS);
	renderer->attachShaderToProgram(renderProgramId, renderVertexShaderId);
	renderer->attachShaderToProgram(renderProgramId, renderFragmentShaderId);

	ShadowMapRenderShaderBaseImplementation::initialize();
}