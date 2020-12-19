#include <tdme/engine/subsystems/particlesystem/ParticlesShader.h>

#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/engine/Engine.h>
#include <tdme/math/Matrix4x4.h>

using tdme::engine::fileio::textures::TextureReader;
using tdme::engine::subsystems::manager::TextureManager;
using tdme::engine::subsystems::particlesystem::ParticlesShader;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::Engine;
using tdme::math::Matrix4x4;

ParticlesShader::ParticlesShader(Engine* engine, Renderer* renderer)
{
	this->engine = engine;
	this->renderer = renderer;
	isRunning = false;
	initialized = false;
	boundTextureIds.fill(renderer->ID_NONE);
}

bool ParticlesShader::isInitialized()
{
	return initialized;
}

void ParticlesShader::initialize()
{
	auto shaderVersion = renderer->getShaderVersion();
	// particles
	//	fragment shader
	renderFragmentShaderId = renderer->loadShader(
		renderer->SHADER_FRAGMENT_SHADER,
		"shader/" + shaderVersion + "/particles",
		"render_fragmentshader.frag",
		"#define HAVE_DEPTH_FOG"
	);
	if (renderFragmentShaderId == 0) return;
	//	vertex shader
	renderVertexShaderId = renderer->loadShader(
		renderer->SHADER_VERTEX_SHADER,
		"shader/" + shaderVersion + "/particles",
		"render_vertexshader.vert",
		"#define HAVE_DEPTH_FOG"
	);
	if (renderVertexShaderId == 0) return;
	// create, attach and link program
	renderProgramId = renderer->createProgram(renderer->PROGRAM_POINTS);
	renderer->attachShaderToProgram(renderProgramId, renderVertexShaderId);
	renderer->attachShaderToProgram(renderProgramId, renderFragmentShaderId);
	// map inputs to attributes
	if (renderer->isUsingProgramAttributeLocation() == true) {
		renderer->setProgramAttributeLocation(renderProgramId, 0, "inVertex");
		renderer->setProgramAttributeLocation(renderProgramId, 1, "inTextureSpriteIndex");
		renderer->setProgramAttributeLocation(renderProgramId, 3, "inColor");
		renderer->setProgramAttributeLocation(renderProgramId, 5, "inPointSize");
		renderer->setProgramAttributeLocation(renderProgramId, 6, "inSpriteSheetDimensions");
		renderer->setProgramAttributeLocation(renderProgramId, 10, "inEffectColorMul");
		renderer->setProgramAttributeLocation(renderProgramId, 11, "inEffectColorAdd");
	}
	// link program
	if (renderer->linkProgram(renderProgramId) == false) return;

	// get uniforms
	uniformMVPMatrix = renderer->getProgramUniformLocation(renderProgramId, "mvpMatrix");
	if (uniformMVPMatrix == -1) return;
	for (auto i = 0; i < uniformDiffuseTextureUnits.size(); i++) {
		uniformDiffuseTextureUnits[i] = renderer->getProgramUniformLocation(renderProgramId, "diffuseTextureUnits[" + to_string(i) + "]");
		if (i == 0 && uniformDiffuseTextureUnits[i] == -1) return;
	}
	// TODO: use ivec2 and vec2
	uniformViewPortWidth = renderer->getProgramUniformLocation(renderProgramId, "viewPortWidth");
	if (uniformViewPortWidth == -1) return;
	uniformViewPortHeight = renderer->getProgramUniformLocation(renderProgramId, "viewPortHeight");
	if (uniformViewPortHeight == -1) return;
	uniformProjectionMatrixXx = renderer->getProgramUniformLocation(renderProgramId, "projectionMatrixXx");
	if (uniformProjectionMatrixXx == -1) return;
	uniformProjectionMatrixYy = renderer->getProgramUniformLocation(renderProgramId, "projectionMatrixYy");
	if (uniformProjectionMatrixYy == -1) return;
	initialized = true;
}

void ParticlesShader::useProgram(void* context)
{
	isRunning = true;
	renderer->useProgram(context, renderProgramId);
	renderer->setLighting(context, renderer->LIGHTING_NONE);
	for (auto i = 0; i < uniformDiffuseTextureUnits.size(); i++) {
		if (uniformDiffuseTextureUnits[i] == -1) break;
		renderer->setProgramUniformInteger(context, uniformDiffuseTextureUnits[i], i);
	}
}

void ParticlesShader::updateEffect(void* context)
{
	// skip if not running
	if (isRunning == false) return;
}

void ParticlesShader::unUseProgram(void* context)
{
	isRunning = false;
	for (auto i = 0; i < boundTextureIds.size(); i++) {
		if (uniformDiffuseTextureUnits[i] == -1) break;
		auto textureId = boundTextureIds[i];
		if (textureId == 0) continue;
		renderer->setTextureUnit(context, i);
		renderer->bindTexture(context, renderer->ID_NONE);
	}
	renderer->setTextureUnit(context, 0);
	boundTextureIds.fill(renderer->ID_NONE);
}

void ParticlesShader::updateMatrices(void* context)
{
	// skip if not running
	if (isRunning == false) return;
	// object to screen matrix
	mvpMatrix.set(renderer->getModelViewMatrix()).multiply(renderer->getProjectionMatrix());
	renderer->setProgramUniformFloatMatrix4x4(context, uniformMVPMatrix, mvpMatrix.getArray());
	renderer->setProgramUniformFloat(context, uniformProjectionMatrixXx, renderer->getProjectionMatrix().getArray()[0]);
	renderer->setProgramUniformFloat(context, uniformProjectionMatrixYy, renderer->getProjectionMatrix().getArray()[5]);
	renderer->setProgramUniformInteger(context, uniformViewPortWidth, renderer->getViewPortWidth());
	renderer->setProgramUniformInteger(context, uniformViewPortHeight, renderer->getViewPortHeight());
}

void ParticlesShader::setParameters(void* context, const array<int32_t, 16>& textureIds) {
	for (auto i = 0; i < boundTextureIds.size(); i++) {
		if (uniformDiffuseTextureUnits[i] == -1) break;
		auto textureId = boundTextureIds[i];
		if (textureId == renderer->ID_NONE) continue;
		renderer->setTextureUnit(context, i);
		renderer->bindTexture(context, renderer->ID_NONE);
	}
	for (auto i = 0; i < textureIds.size(); i++) {
		if (uniformDiffuseTextureUnits[i] == -1) break;
		auto textureId = textureIds[i];
		if (textureId == renderer->ID_NONE) continue;
		renderer->setTextureUnit(context, i);
		renderer->bindTexture(context, textureId);
	}
	renderer->setTextureUnit(context, 0);
	boundTextureIds = textureIds;
}
