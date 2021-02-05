#include <array>
#include <string>

#include <tdme/engine/fileio/textures/Texture.h>
#include <tdme/engine/fileio/textures/TextureReader.h>
#include <tdme/engine/subsystems/manager/TextureManager.h>
#include <tdme/engine/subsystems/postprocessing/PostProcessingShaderBaseImplementation.h>
#include <tdme/engine/subsystems/postprocessing/PostProcessingShaderSSAOMapImplementation.h>
#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/engine/Engine.h>
#include <tdme/math/Vector3.h>

using std::array;
using std::string;
using std::to_string;

using tdme::engine::fileio::textures::Texture;
using tdme::engine::fileio::textures::TextureReader;
using tdme::engine::subsystems::manager::TextureManager;
using tdme::engine::subsystems::postprocessing::PostProcessingShaderSSAOMapImplementation;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::Engine;
using tdme::engine::Engine;
using tdme::math::Vector3;

bool PostProcessingShaderSSAOMapImplementation::isSupported(Renderer* renderer) {
	return renderer->getShaderVersion() == "gl3";
}

PostProcessingShaderSSAOMapImplementation::PostProcessingShaderSSAOMapImplementation(Renderer* renderer): PostProcessingShaderBaseImplementation(renderer)
{
}

void PostProcessingShaderSSAOMapImplementation::initialize()
{
	auto shaderVersion = renderer->getShaderVersion();

	//	fragment shader
	fragmentShaderId = renderer->loadShader(
		renderer->SHADER_FRAGMENT_SHADER,
		"shader/" + shaderVersion + "/postprocessing",
		"ssao_map_fragmentshader.frag"
	);
	if (fragmentShaderId == 0) return;

	//	vertex shader
	vertexShaderId = renderer->loadShader(
		renderer->SHADER_VERTEX_SHADER,
		"shader/" + shaderVersion + "/postprocessing",
		"ssao_map_vertexshader.vert"
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

	// custom initialization
	for (auto i = 0; i < uniformSphere.size(); i++) {
		uniformSphere[i] = renderer->getProgramUniformLocation(programId, "sphere[" + to_string(i) + "]");
		if (uniformSphere[i] == -1) {
			initialized = false;
			return;
		}
	}
	uniformRandomTextureUnit = renderer->getProgramUniformLocation(programId, "randomTextureUnit");
	if (uniformRandomTextureUnit == -1) {
		initialized = false;
		return;
	}
	auto randomTexture = TextureReader::read("resources/engine/textures", "random.png");
	randomTexture->setUseMipMap(false);
	randomTextureId = Engine::getInstance()->getTextureManager()->addTexture(randomTexture, renderer->getDefaultContext());

	// register shader
	Engine::registerShader(Engine::ShaderType::SHADERTYPE_POSTPROCESSING, "ssaomap");
}

void PostProcessingShaderSSAOMapImplementation::useProgram(void* context) {
	PostProcessingShaderBaseImplementation::useProgram(context);
	renderer->setProgramUniformFloatVec3(context, uniformSphere[0], Vector3( 0.5381f, 0.1856f,-0.4319f).getArray());
	renderer->setProgramUniformFloatVec3(context, uniformSphere[1], Vector3( 0.1379f, 0.2486f, 0.4430f).getArray());
	renderer->setProgramUniformFloatVec3(context, uniformSphere[2], Vector3( 0.3371f, 0.5679f,-0.0057f).getArray());
	renderer->setProgramUniformFloatVec3(context, uniformSphere[3], Vector3(-0.6999f,-0.0451f,-0.0019f).getArray());
	renderer->setProgramUniformFloatVec3(context, uniformSphere[4], Vector3( 0.0689f,-0.1598f,-0.8547f).getArray());
	renderer->setProgramUniformFloatVec3(context, uniformSphere[5], Vector3( 0.0560f, 0.0069f,-0.1843f).getArray());
	renderer->setProgramUniformFloatVec3(context, uniformSphere[6], Vector3(-0.0146f, 0.1402f, 0.0762f).getArray());
	renderer->setProgramUniformFloatVec3(context, uniformSphere[7], Vector3( 0.0100f,-0.1924f,-0.0344f).getArray());
	renderer->setProgramUniformFloatVec3(context, uniformSphere[8], Vector3(-0.3577f,-0.5301f,-0.4358f).getArray());
	renderer->setProgramUniformFloatVec3(context, uniformSphere[9], Vector3(-0.3169f, 0.1063f, 0.0158f).getArray());
	renderer->setProgramUniformFloatVec3(context, uniformSphere[10], Vector3( 0.0103f,-0.5869f, 0.0046f).getArray());
	renderer->setProgramUniformFloatVec3(context, uniformSphere[11], Vector3(-0.0897f,-0.4940f, 0.3287f).getArray());
	renderer->setProgramUniformFloatVec3(context, uniformSphere[12], Vector3( 0.7119f,-0.0154f,-0.0918f).getArray());
	renderer->setProgramUniformFloatVec3(context, uniformSphere[13], Vector3(-0.0533f, 0.0596f,-0.5411f).getArray());
	renderer->setProgramUniformFloatVec3(context, uniformSphere[14], Vector3( 0.0352f,-0.0631f, 0.5460f).getArray());
	renderer->setProgramUniformFloatVec3(context, uniformSphere[15], Vector3(-0.4776f, 0.2847f,-0.0271f).getArray());
	renderer->setTextureUnit(context, 4);
	renderer->setProgramUniformInteger(context, uniformRandomTextureUnit, 4);
	renderer->bindTexture(context, randomTextureId);
}

void PostProcessingShaderSSAOMapImplementation::setShaderParameters(void* context, Engine* engine) {
}
