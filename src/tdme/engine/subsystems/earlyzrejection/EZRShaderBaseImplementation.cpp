#include <tdme/engine/subsystems/earlyzrejection/EZRShaderBaseImplementation.h>

#include <tdme/engine/subsystems/lighting/LightingShader.h>
#include <tdme/engine/subsystems/lighting/LightingShaderConstants.h>
#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Timing.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/utilities/Console.h>

using tdme::engine::subsystems::earlyzrejection::EZRShaderBaseImplementation;
using tdme::engine::subsystems::lighting::LightingShader;
using tdme::engine::subsystems::lighting::LightingShaderConstants;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::Engine;
using tdme::engine::Timing;
using tdme::math::Matrix4x4;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;
using tdme::utilities::Console;

EZRShaderBaseImplementation::EZRShaderBaseImplementation(Renderer* renderer)
{
	this->renderer = renderer;
	initialized = false;
}

EZRShaderBaseImplementation::~EZRShaderBaseImplementation() {
}

bool EZRShaderBaseImplementation::isInitialized()
{
	return initialized;
}

void EZRShaderBaseImplementation::initialize()
{
	// map inputs to attributes
	if (renderer->isUsingProgramAttributeLocation() == true) {
		renderer->setProgramAttributeLocation(programId, 0, "inVertex");
		renderer->setProgramAttributeLocation(programId, 2, "inTextureUV");
	}
	// link
	if (renderer->linkProgram(programId) == false) return;

	// uniforms
	if (renderer->isInstancedRenderingAvailable() == true) {
		//	uniforms
		uniformProjectionMatrix = renderer->getProgramUniformLocation(programId, "projectionMatrix");
		if (uniformProjectionMatrix == -1) return;
		uniformCameraMatrix = renderer->getProgramUniformLocation(programId, "cameraMatrix");
		if (uniformCameraMatrix == -1) return;
	} else {
		//	uniforms
		uniformMVPMatrix = renderer->getProgramUniformLocation(programId, "mvpMatrix");
		if (uniformMVPMatrix == -1) return;
	}
	uniformTextureMatrix = renderer->getProgramUniformLocation(programId, "textureMatrix");
	if (uniformTextureMatrix == -1) return;
	uniformTextureAtlasSize = renderer->getProgramUniformLocation(programId, "textureAtlasSize");
	if (uniformTextureAtlasSize == -1) return;
	uniformTextureAtlasPixelDimension = renderer->getProgramUniformLocation(programId, "textureAtlasPixelDimension");
	if (uniformTextureAtlasPixelDimension == -1) return;
	uniformDiffuseTextureUnit = renderer->getProgramUniformLocation(programId, "diffuseTextureUnit");
	if (uniformDiffuseTextureUnit == -1) return;
	uniformDiffuseTextureAvailable = renderer->getProgramUniformLocation(programId, "diffuseTextureAvailable");
	if (uniformDiffuseTextureAvailable == -1) return;
	uniformDiffuseTextureMaskedTransparency = renderer->getProgramUniformLocation(programId, "diffuseTextureMaskedTransparency");
	if (uniformDiffuseTextureMaskedTransparency == -1) return;
	uniformDiffuseTextureMaskedTransparencyThreshold = renderer->getProgramUniformLocation(programId, "diffuseTextureMaskedTransparencyThreshold");
	if (uniformDiffuseTextureMaskedTransparencyThreshold == -1) return;

	//
	uniformFrame = renderer->getProgramUniformLocation(programId, "frame");

	//
	initialized = true;
}

void EZRShaderBaseImplementation::useProgram(Engine* engine, void* context)
{
	renderer->useProgram(context, programId);
	renderer->setLighting(context, renderer->LIGHTING_SPECULAR);
	renderer->setProgramUniformInteger(context, uniformDiffuseTextureUnit, LightingShaderConstants::SPECULAR_TEXTUREUNIT_DIFFUSE);
	if (uniformFrame != -1) renderer->setProgramUniformInteger(context, uniformFrame, engine->getTiming()->getFrame());
}

void EZRShaderBaseImplementation::unUseProgram(void* context)
{
}

void EZRShaderBaseImplementation::updateMatrices(Renderer* renderer, void* context)
{
	if (renderer->isInstancedRenderingAvailable() == true) {
		renderer->setProgramUniformFloatMatrix4x4(context, uniformProjectionMatrix, renderer->getProjectionMatrix().getArray());
		renderer->setProgramUniformFloatMatrix4x4(context, uniformCameraMatrix, renderer->getCameraMatrix().getArray());
	} else {
		// matrices
		Matrix4x4 mvpMatrix;
		// mvp matrix
		mvpMatrix.set(renderer->getModelViewMatrix()).multiply(renderer->getProjectionMatrix());
		// upload
		renderer->setProgramUniformFloatMatrix4x4(context, uniformMVPMatrix, mvpMatrix.getArray());
	}
}

void EZRShaderBaseImplementation::updateTextureMatrix(Renderer* renderer, void* context) {
	renderer->setProgramUniformFloatMatrix3x3(context, uniformTextureMatrix, renderer->getTextureMatrix(context).getArray());
}

void EZRShaderBaseImplementation::updateMaterial(Renderer* renderer, void* context)
{
	auto material = renderer->getSpecularMaterial(context);
	renderer->setProgramUniformInteger(context, uniformDiffuseTextureMaskedTransparency, material.diffuseTextureMaskedTransparency);
	renderer->setProgramUniformFloat(context, uniformDiffuseTextureMaskedTransparencyThreshold, material.diffuseTextureMaskedTransparencyThreshold);
	renderer->setProgramUniformInteger(context, uniformTextureAtlasSize, material.textureAtlasSize);
	renderer->setProgramUniformFloatVec2(context, uniformTextureAtlasPixelDimension, material.textureAtlasPixelDimension);
}

void EZRShaderBaseImplementation::bindTexture(Renderer* renderer, void* context, int32_t textureId)
{
	switch (renderer->getTextureUnit(context)) {
		case LightingShaderConstants::SPECULAR_TEXTUREUNIT_DIFFUSE:
			renderer->setProgramUniformInteger(context, uniformDiffuseTextureAvailable, textureId == 0 ? 0 : 1);
			break;
	}
}
