#include <tdme/engine/subsystems/lighting/LightingShaderTerrainImplementation.h>

#include <string>

#include <tdme/engine/fileio/textures/TextureReader.h>
#include <tdme/engine/subsystems/lighting/LightingShaderBaseImplementation.h>
#include <tdme/engine/subsystems/lighting/LightingShaderConstants.h>
#include <tdme/engine/subsystems/manager/TextureManager.h>
#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/engine/Engine.h>

using std::string;
using std::to_string;

using tdme::engine::fileio::textures::TextureReader;
using tdme::engine::subsystems::lighting::LightingShaderBaseImplementation;
using tdme::engine::subsystems::lighting::LightingShaderConstants;
using tdme::engine::subsystems::lighting::LightingShaderTerrainImplementation;
using tdme::engine::subsystems::lighting::LightingShaderTerrainImplementation;
using tdme::engine::subsystems::manager::TextureManager;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::Engine;

bool LightingShaderTerrainImplementation::isSupported(Renderer* renderer) {
	return true;
}

LightingShaderTerrainImplementation::LightingShaderTerrainImplementation(Renderer* renderer): LightingShaderBaseImplementation(renderer)
{
}

const string LightingShaderTerrainImplementation::getId() {
	return "terrain";
}

void LightingShaderTerrainImplementation::initialize()
{
	auto shaderVersion = renderer->getShaderVersion();

	// lighting
	//	fragment shader
	renderLightingFragmentShaderId = renderer->loadShader(
		renderer->SHADER_FRAGMENT_SHADER,
		"shader/" + shaderVersion + "/lighting/specular",
		"render_fragmentshader.frag",
		"#define HAVE_TERRAIN_SHADER\n#define HAVE_DEPTH_FOG" + additionalDefinitions
	);
	if (renderLightingFragmentShaderId == 0) return;

	//	vertex shader
	renderLightingVertexShaderId = renderer->loadShader(
		renderer->SHADER_VERTEX_SHADER,
		"shader/" + shaderVersion + "/lighting/specular",
		"render_vertexshader.vert",
		"#define HAVE_TERRAIN_SHADER\n#define HAVE_DEPTH_FOG" + additionalDefinitions
	);
	if (renderLightingVertexShaderId == 0) return;

	// create, attach and link program
	programId = renderer->createProgram(renderer->PROGRAM_OBJECTS);
	renderer->attachShaderToProgram(programId, renderLightingVertexShaderId);
	renderer->attachShaderToProgram(programId, renderLightingFragmentShaderId);

	//
	LightingShaderBaseImplementation::initialize();
	if (initialized == false) return;

	//
	initialized = false;

	uniformModelMatrix = renderer->getProgramUniformLocation(programId, "modelMatrix");

	uniformGrasTextureUnit = renderer->getProgramUniformLocation(programId, "grasTextureUnit");
	if (uniformGrasTextureUnit == -1) return;

	uniformDirtTextureUnit = renderer->getProgramUniformLocation(programId, "dirtTextureUnit");
	if (uniformDirtTextureUnit == -1) return;

	uniformSnowTextureUnit = renderer->getProgramUniformLocation(programId, "snowTextureUnit");
	if (uniformSnowTextureUnit == -1) return;

	uniformStoneTextureUnit = renderer->getProgramUniformLocation(programId, "stoneTextureUnit");
	if (uniformStoneTextureUnit == -1) return;

	//
	grasTextureId = Engine::getInstance()->getTextureManager()->addTexture(TextureReader::read("resources/engine/textures", "terrain_gras.png"), renderer->getDefaultContext());
	dirtTextureId = Engine::getInstance()->getTextureManager()->addTexture(TextureReader::read("resources/engine/textures", "terrain_dirt.png"), renderer->getDefaultContext());
	snowTextureId = Engine::getInstance()->getTextureManager()->addTexture(TextureReader::read("resources/engine/textures", "terrain_snow.png"), renderer->getDefaultContext());
	stoneTextureId = Engine::getInstance()->getTextureManager()->addTexture(TextureReader::read("resources/engine/textures", "terrain_stone.png"), renderer->getDefaultContext());

	//
	initialized = true;
}

void LightingShaderTerrainImplementation::registerShader() {
	// register shader
	Engine::registerShader(
		Engine::ShaderType::SHADERTYPE_OBJECT3D,
		getId()
	);
}

void LightingShaderTerrainImplementation::useProgram(Engine* engine, void* context) {
	LightingShaderBaseImplementation::useProgram(engine, context);

	//
	auto currentTextureUnit = renderer->getTextureUnit(context);
	renderer->setTextureUnit(context, LightingShaderConstants::SPECULAR_TEXTUREUNIT_TERRAIN_GRAS);
	renderer->bindTexture(context, grasTextureId);
	renderer->setTextureUnit(context, LightingShaderConstants::SPECULAR_TEXTUREUNIT_TERRAIN_DIRT);
	renderer->bindTexture(context, dirtTextureId);
	renderer->setTextureUnit(context, LightingShaderConstants::SPECULAR_TEXTUREUNIT_TERRAIN_STONE);
	renderer->bindTexture(context, stoneTextureId);
	renderer->setTextureUnit(context, LightingShaderConstants::SPECULAR_TEXTUREUNIT_TERRAIN_SNOW);
	renderer->bindTexture(context, snowTextureId);
	renderer->setTextureUnit(context, LightingShaderConstants::SPECULAR_TEXTUREUNIT_TERRAIN_BRUSH);
	renderer->setTextureUnit(context, currentTextureUnit);

	//
	renderer->setProgramUniformInteger(context, uniformGrasTextureUnit, LightingShaderConstants::SPECULAR_TEXTUREUNIT_TERRAIN_GRAS);
	renderer->setProgramUniformInteger(context, uniformDirtTextureUnit, LightingShaderConstants::SPECULAR_TEXTUREUNIT_TERRAIN_DIRT);
	renderer->setProgramUniformInteger(context, uniformSnowTextureUnit, LightingShaderConstants::SPECULAR_TEXTUREUNIT_TERRAIN_SNOW);
	renderer->setProgramUniformInteger(context, uniformStoneTextureUnit, LightingShaderConstants::SPECULAR_TEXTUREUNIT_TERRAIN_STONE);
}

void LightingShaderTerrainImplementation::unUseProgram(void* context) {
	//
	auto currentTextureUnit = renderer->getTextureUnit(context);
	renderer->setTextureUnit(context, LightingShaderConstants::SPECULAR_TEXTUREUNIT_TERRAIN_GRAS);
	renderer->bindTexture(context, renderer->ID_NONE);
	renderer->setTextureUnit(context, LightingShaderConstants::SPECULAR_TEXTUREUNIT_TERRAIN_DIRT);
	renderer->bindTexture(context, renderer->ID_NONE);
	renderer->setTextureUnit(context, LightingShaderConstants::SPECULAR_TEXTUREUNIT_TERRAIN_STONE);
	renderer->bindTexture(context, renderer->ID_NONE);
	renderer->setTextureUnit(context, LightingShaderConstants::SPECULAR_TEXTUREUNIT_TERRAIN_SNOW);
	renderer->bindTexture(context, renderer->ID_NONE);
	renderer->setTextureUnit(context, currentTextureUnit);

	//
	LightingShaderBaseImplementation::unUseProgram(context);
}

void LightingShaderTerrainImplementation::updateMatrices(Renderer* renderer, void* context) {
	LightingShaderBaseImplementation::updateMatrices(renderer, context);
	if (uniformModelMatrix != -1) renderer->setProgramUniformFloatMatrix4x4(context, uniformModelMatrix, renderer->getModelViewMatrix().getArray());
}

void LightingShaderTerrainImplementation::updateShaderParameters(Renderer* renderer, void* context) {
}
