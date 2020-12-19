#include <tdme/engine/subsystems/lines/LinesShader.h>

#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/engine/Engine.h>
#include <tdme/math/Matrix4x4.h>

using tdme::engine::fileio::textures::TextureReader;
using tdme::engine::subsystems::lines::LinesShader;
using tdme::engine::subsystems::manager::TextureManager;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::Engine;
using tdme::math::Matrix4x4;

LinesShader::LinesShader(Engine* engine, Renderer* renderer)
{
	this->engine = engine;
	this->renderer = renderer;
	isRunning = false;
	initialized = false;
}

bool LinesShader::isInitialized()
{
	return initialized;
}

void LinesShader::initialize()
{
	auto shaderVersion = renderer->getShaderVersion();
	// particles
	//	fragment shader
	renderFragmentShaderId = renderer->loadShader(
		renderer->SHADER_FRAGMENT_SHADER,
		"shader/" + shaderVersion + "/lines",
		"render_fragmentshader.frag"
	);
	if (renderFragmentShaderId == 0)
		return;
	//	vertex shader
	renderVertexShaderId = renderer->loadShader(
		renderer->SHADER_VERTEX_SHADER,
		"shader/" + shaderVersion + "/lines",
		"render_vertexshader.vert"
	);
	if (renderVertexShaderId == 0)
		return;
	// create, attach and link program
	renderProgramId = renderer->createProgram(renderer->PROGRAM_LINES);
	renderer->attachShaderToProgram(renderProgramId, renderVertexShaderId);
	renderer->attachShaderToProgram(renderProgramId, renderFragmentShaderId);
	// map inputs to attributes
	if (renderer->isUsingProgramAttributeLocation() == true) {
		renderer->setProgramAttributeLocation(renderProgramId, 0, "inVertex");
		renderer->setProgramAttributeLocation(renderProgramId, 3, "inColor");
	}
	// link program
	if (renderer->linkProgram(renderProgramId) == false)
		return;

	// get uniforms
	uniformMVPMatrix = renderer->getProgramUniformLocation(renderProgramId, "mvpMatrix");
	if (uniformMVPMatrix == -1)
		return;
	uniformDiffuseTextureUnit = renderer->getProgramUniformLocation(renderProgramId, "diffuseTextureUnit");
	if (uniformDiffuseTextureUnit == -1)
		return;
	uniformEffectColorMul = renderer->getProgramUniformLocation(renderProgramId, "effectColorMul");
	if (uniformEffectColorMul == -1)
		return;
	uniformEffectColorAdd = renderer->getProgramUniformLocation(renderProgramId, "effectColorAdd");
	if (uniformEffectColorAdd == -1)
		return;
	initialized = true;
}

void LinesShader::useProgram(void* context)
{
	isRunning = true;
	renderer->useProgram(context, renderProgramId);
	renderer->setLighting(context, renderer->LIGHTING_NONE);
	renderer->setProgramUniformInteger(context, uniformDiffuseTextureUnit, 0);
}

void LinesShader::updateEffect(void* context)
{
	// skip if not running
	if (isRunning == false)
		return;
	// effect color
	renderer->setProgramUniformFloatVec4(context, uniformEffectColorMul, renderer->getEffectColorMul(context));
	renderer->setProgramUniformFloatVec4(context, uniformEffectColorAdd, renderer->getEffectColorAdd(context));
}

void LinesShader::unUseProgram(void* context)
{
	isRunning = false;
	renderer->setLineWidth(1.0f);
	renderer->bindTexture(context, renderer->ID_NONE);
}

void LinesShader::updateMatrices(void* context)
{
	// skip if not running
	if (isRunning == false)
		return;
	// object to screen matrix
	mvpMatrix.set(renderer->getModelViewMatrix()).multiply(renderer->getProjectionMatrix());
	// upload matrices
	renderer->setProgramUniformFloatMatrix4x4(context, uniformMVPMatrix, mvpMatrix.getArray());
}

void LinesShader::setParameters(void* context, int32_t textureId, float lineWidth) {
	renderer->setLineWidth(lineWidth);
	renderer->bindTexture(context, textureId);
}
