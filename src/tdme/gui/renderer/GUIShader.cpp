#include <tdme/gui/renderer/GUIShader.h>

#include <tdme/engine/subsystems/renderer/Renderer.h>

using tdme::gui::renderer::GUIShader;
using tdme::engine::subsystems::renderer::Renderer;

GUIShader::GUIShader(Renderer* renderer)
{
	this->renderer = renderer;
	initialized = false;
	isRunning = false;
}

bool GUIShader::isInitialized()
{
	return initialized;
}

void GUIShader::initialize()
{
	auto shaderVersion = renderer->getShaderVersion();
	vertexShaderId = renderer->loadShader(
		renderer->SHADER_VERTEX_SHADER,
		"shader/" + shaderVersion + "/gui",
		"render_vertexshader.vert"
	);
	if (vertexShaderId == 0) return;

	fragmentShaderId = renderer->loadShader(
		renderer->SHADER_FRAGMENT_SHADER,
		"shader/" + shaderVersion + "/gui",
		"render_fragmentshader.frag"
	);
	if (fragmentShaderId == 0) return;

	programId = renderer->createProgram(renderer->PROGRAM_OBJECTS);
	renderer->attachShaderToProgram(programId, vertexShaderId);
	renderer->attachShaderToProgram(programId, fragmentShaderId);
	if (renderer->isUsingProgramAttributeLocation() == true) {
		renderer->setProgramAttributeLocation(programId, 0, "inVertex");
		renderer->setProgramAttributeLocation(programId, 2, "inTextureUV");
		renderer->setProgramAttributeLocation(programId, 3, "inColor");
	}
	if (renderer->linkProgram(programId) == false) return;

	uniformDiffuseTextureUnit = renderer->getProgramUniformLocation(programId, "diffuseTextureUnit");
	if (uniformDiffuseTextureUnit == -1) return;

	uniformDiffuseTextureAvailable = renderer->getProgramUniformLocation(programId, "diffuseTextureAvailable");
	if (uniformDiffuseTextureAvailable == -1) return;

	uniformMaskTextureUnit = renderer->getProgramUniformLocation(programId, "maskTextureUnit");
	if (uniformMaskTextureUnit == -1) return;

	uniformMaskTextureAvailable = renderer->getProgramUniformLocation(programId, "maskTextureAvailable");
	if (uniformMaskTextureAvailable == -1) return;

	uniformMaskMaxValue = renderer->getProgramUniformLocation(programId, "maskMaxValue");
	if (uniformMaskMaxValue == -1) return;

	uniformEffectColorMul = renderer->getProgramUniformLocation(programId, "effectColorMul");
	if (uniformEffectColorMul == -1) return;

	uniformEffectColorAdd = renderer->getProgramUniformLocation(programId, "effectColorAdd");
	if (uniformEffectColorAdd == -1) return;

	// texture matrix
	uniformTextureMatrix = renderer->getProgramUniformLocation(programId, "textureMatrix");
	if (uniformTextureMatrix == -1) return;

	initialized = true;
}

void GUIShader::useProgram()
{
	auto defaultContext = renderer->getDefaultContext();
	renderer->useProgram(defaultContext, programId);
	renderer->setLighting(defaultContext, renderer->LIGHTING_NONE);
	renderer->setProgramUniformInteger(defaultContext, uniformDiffuseTextureUnit, 0);
	renderer->setProgramUniformInteger(defaultContext, uniformMaskTextureUnit, 1);
	renderer->setProgramUniformInteger(defaultContext, uniformMaskTextureAvailable, 0);
	renderer->setProgramUniformFloat(defaultContext, uniformMaskMaxValue, 1.0);
	isRunning = true;
}

void GUIShader::unUseProgram()
{
	isRunning = false;
}

void GUIShader::bindTexture(int32_t textureId)
{
	if (isRunning == false) return;
	switch(renderer->getTextureUnit(renderer->getDefaultContext())) {
		case 0:
			renderer->setProgramUniformInteger(renderer->getDefaultContext(), uniformDiffuseTextureAvailable, textureId == 0 ? 0 : 1);
			break;
		case 1:
			renderer->setProgramUniformFloat(renderer->getDefaultContext(), uniformMaskMaxValue, renderer->getMaskMaxValue(renderer->getDefaultContext()));
			renderer->setProgramUniformInteger(renderer->getDefaultContext(), uniformMaskTextureAvailable, textureId == 0 ? 0 : 1);
			break;
	}
}

void GUIShader::updateEffect()
{
	if (isRunning == false) return;

	// use default context
	auto context = renderer->getDefaultContext();

	//
	renderer->setProgramUniformFloatVec4(context, uniformEffectColorMul, renderer->getEffectColorMul(context));
	renderer->setProgramUniformFloatVec4(context, uniformEffectColorAdd, renderer->getEffectColorAdd(context));
}


void GUIShader::updateTextureMatrix() {
	if (isRunning == false) return;

	// use default context
	auto context = renderer->getDefaultContext();

	//
	renderer->setProgramUniformFloatMatrix3x3(context, uniformTextureMatrix, renderer->getTextureMatrix(context).getArray());
}
