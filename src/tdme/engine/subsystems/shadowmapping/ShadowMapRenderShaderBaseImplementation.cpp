#include <tdme/engine/subsystems/shadowmapping/ShadowMapRenderShaderBaseImplementation.h>

#include <string>

#include <tdme/engine/subsystems/lighting/LightingShader.h>
#include <tdme/engine/subsystems/lighting/LightingShaderConstants.h>
#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/engine/subsystems/shadowmapping/ShadowMap.h>
#include <tdme/engine/subsystems/shadowmapping/ShadowMapping.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Timing.h>
#include <tdme/math/Math.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector3.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>

using std::to_string;

using tdme::engine::subsystems::lighting::LightingShader;
using tdme::engine::subsystems::lighting::LightingShaderConstants;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::subsystems::shadowmapping::ShadowMap;
using tdme::engine::subsystems::shadowmapping::ShadowMapping;
using tdme::engine::subsystems::shadowmapping::ShadowMapRenderShaderBaseImplementation;
using tdme::engine::Engine;
using tdme::engine::Timing;
using tdme::math::Math;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;

ShadowMapRenderShaderBaseImplementation::ShadowMapRenderShaderBaseImplementation(Renderer* renderer)
{
	this->renderer = renderer;
	initialized = false;
}

ShadowMapRenderShaderBaseImplementation::~ShadowMapRenderShaderBaseImplementation()
{
}

bool ShadowMapRenderShaderBaseImplementation::isInitialized()
{
	return initialized;
}

void ShadowMapRenderShaderBaseImplementation::initialize()
{
	auto shaderVersion = renderer->getShaderVersion();

	// map inputs to attributes
	if (renderer->isUsingProgramAttributeLocation() == true) {
		renderer->setProgramAttributeLocation(renderProgramId, 0, "inVertex");
		renderer->setProgramAttributeLocation(renderProgramId, 1, "inNormal");
		renderer->setProgramAttributeLocation(renderProgramId, 2, "inTextureUV");
		renderer->setProgramAttributeLocation(renderProgramId, 4, "inOrigin");
	}
	// link
	if (renderer->linkProgram(renderProgramId) == false)
		return;

	//	uniforms
	renderUniformShadowMapLookUps = renderer->getProgramUniformLocation(renderProgramId, "shadowMapLookUps");
	uniformTextureAtlasSize = renderer->getProgramUniformLocation(renderProgramId, "textureAtlasSize");
	if (uniformTextureAtlasSize == -1) return;
	uniformTextureAtlasPixelDimension = renderer->getProgramUniformLocation(renderProgramId, "textureAtlasPixelDimension");
	if (uniformTextureAtlasPixelDimension == -1) return;
	renderUniformTextureUnit = renderer->getProgramUniformLocation(renderProgramId, "textureUnit");
	if (renderUniformTextureUnit == -1) return;
	renderUniformTexturePixelWidth = renderer->getProgramUniformLocation(renderProgramId, "texturePixelWidth");
	if (renderUniformTexturePixelWidth == -1) return;
	renderUniformTexturePixelHeight = renderer->getProgramUniformLocation(renderProgramId, "texturePixelHeight");
	if (renderUniformTexturePixelHeight == -1) return;
	renderUniformDepthBiasMVPMatrix = renderer->getProgramUniformLocation(renderProgramId, "depthBiasMVPMatrix");
	if (renderUniformDepthBiasMVPMatrix == -1) return;
	if (renderer->isInstancedRenderingAvailable() == true) {
		renderUniformProjectionMatrix = renderer->getProgramUniformLocation(renderProgramId, "projectionMatrix");
		if (renderUniformProjectionMatrix == -1) return;
		renderUniformCameraMatrix = renderer->getProgramUniformLocation(renderProgramId, "cameraMatrix");
		if (renderUniformCameraMatrix == -1) return;
	} else {
		renderUniformMVPMatrix = renderer->getProgramUniformLocation(renderProgramId, "mvpMatrix");
		if (renderUniformMVPMatrix == -1) return;
		renderUniformNormalMatrix = renderer->getProgramUniformLocation(renderProgramId, "normalMatrix");
		if (renderUniformNormalMatrix == -1) return;
		renderUniformModelTranslation = renderer->getProgramUniformLocation(renderProgramId, "modelTranslation");
	}
	uniformTextureMatrix = renderer->getProgramUniformLocation(renderProgramId, "textureMatrix");
	if (uniformTextureMatrix == -1) return;
	uniformDiffuseTextureUnit = renderer->getProgramUniformLocation(renderProgramId, "diffuseTextureUnit");
	if (uniformDiffuseTextureUnit == -1) return;
	uniformDiffuseTextureAvailable = renderer->getProgramUniformLocation(renderProgramId, "diffuseTextureAvailable");
	if (uniformDiffuseTextureAvailable == -1) return;
	uniformDiffuseTextureMaskedTransparency = renderer->getProgramUniformLocation(renderProgramId, "diffuseTextureMaskedTransparency");
	if (uniformDiffuseTextureMaskedTransparency == -1) return;
	uniformDiffuseTextureMaskedTransparencyThreshold = renderer->getProgramUniformLocation(renderProgramId, "diffuseTextureMaskedTransparencyThreshold");
	if (uniformDiffuseTextureMaskedTransparencyThreshold == -1) return;
	renderUniformLightDirection = renderer->getProgramUniformLocation(renderProgramId, "lightDirection");
	if (renderUniformLightDirection == -1) return;
	if (shaderVersion != "gl2") {
		if (renderer->isInstancedRenderingAvailable() == false) {
			renderUniformMVMatrix = renderer->getProgramUniformLocation(renderProgramId, "mvMatrix");
			if (renderUniformMVMatrix == -1) return;
		}
		renderUniformLightPosition = renderer->getProgramUniformLocation(renderProgramId, "lightPosition");
		if (renderUniformLightPosition == -1) return;
		renderUniformLightSpotExponent = renderer->getProgramUniformLocation(renderProgramId, "lightSpotExponent");
		if (renderUniformLightSpotExponent == -1) return;
		renderUniformLightSpotCosCutoff = renderer->getProgramUniformLocation(renderProgramId, "lightSpotCosCutoff");
		if (renderUniformLightSpotCosCutoff == -1) return;
		renderUniformLightConstantAttenuation = renderer->getProgramUniformLocation(renderProgramId, "lightConstantAttenuation");
		if (renderUniformLightConstantAttenuation == -1) return;
		renderUniformLightLinearAttenuation = renderer->getProgramUniformLocation(renderProgramId, "lightLinearAttenuation");
		if (renderUniformLightLinearAttenuation == -1) return;
		renderUniformLightQuadraticAttenuation = renderer->getProgramUniformLocation(renderProgramId, "lightQuadraticAttenuation");
		if (renderUniformLightQuadraticAttenuation == -1) return;
	}

	//
	renderUniformTime = renderer->getProgramUniformLocation(renderProgramId, "time");

	//
	initialized = true;
}

void ShadowMapRenderShaderBaseImplementation::useProgram(Engine* engine, void* context)
{
	renderer->useProgram(context, renderProgramId);
	renderer->setLighting(context, renderer->LIGHTING_SPECULAR);
	renderer->setProgramUniformInteger(context, renderUniformTextureUnit, ShadowMap::TEXTUREUNIT);
	if (renderUniformTime != -1) renderer->setProgramUniformFloat(context, renderUniformTime, static_cast<float>(engine->getTiming()->getTotalTime()) / 1000.0f);
	if (renderUniformShadowMapLookUps != -1) renderer->setProgramUniformInteger(context, renderUniformShadowMapLookUps, Engine::getShadowMapRenderLookUps());
}

void ShadowMapRenderShaderBaseImplementation::unUseProgram(void* context)
{
}

void ShadowMapRenderShaderBaseImplementation::updateMatrices(void* context) {
	if (renderer->isInstancedRenderingAvailable() == true) {
		renderer->setProgramUniformFloatMatrix4x4(context, renderUniformProjectionMatrix, renderer->getProjectionMatrix().getArray());
		renderer->setProgramUniformFloatMatrix4x4(context, renderUniformCameraMatrix, renderer->getCameraMatrix().getArray());
	} else {
		Matrix4x4 mvMatrix;
		Matrix4x4 mvpMatrix;
		Matrix4x4 normalMatrix;
		Vector3 modelTranslation;
		// model view matrix
		mvMatrix.set(renderer->getModelViewMatrix()).multiply(renderer->getCameraMatrix());
		// object to screen matrix
		mvpMatrix.set(mvMatrix).multiply(renderer->getProjectionMatrix());
		// normal matrix
		normalMatrix.set(renderer->getModelViewMatrix()).invert().transpose();
		// model translation
		renderer->getModelViewMatrix().getTranslation(modelTranslation);
		// upload
		if (renderUniformMVMatrix != -1) renderer->setProgramUniformFloatMatrix4x4(context, renderUniformMVMatrix, mvMatrix.getArray());
		renderer->setProgramUniformFloatMatrix4x4(context, renderUniformMVPMatrix, mvpMatrix.getArray());
		renderer->setProgramUniformFloatMatrix4x4(context, renderUniformNormalMatrix, normalMatrix.getArray());
		if (renderUniformModelTranslation != -1) renderer->setProgramUniformFloatVec3(context, renderUniformModelTranslation, modelTranslation.getArray());
	}
}

void ShadowMapRenderShaderBaseImplementation::updateTextureMatrix(Renderer* renderer, void* context) {
	renderer->setProgramUniformFloatMatrix3x3(context, uniformTextureMatrix, renderer->getTextureMatrix(context).getArray());
}

void ShadowMapRenderShaderBaseImplementation::updateMaterial(Renderer* renderer, void* context)
{
	auto material = renderer->getSpecularMaterial(context);
	renderer->setProgramUniformInteger(context, uniformDiffuseTextureMaskedTransparency, material.diffuseTextureMaskedTransparency);
	renderer->setProgramUniformFloat(context, uniformDiffuseTextureMaskedTransparencyThreshold, material.diffuseTextureMaskedTransparencyThreshold);
	renderer->setProgramUniformInteger(context, uniformTextureAtlasSize, material.textureAtlasSize);
	renderer->setProgramUniformFloatVec2(context, uniformTextureAtlasPixelDimension, material.textureAtlasPixelDimension);
}

void ShadowMapRenderShaderBaseImplementation::updateLight(Renderer* renderer, void* context, int32_t lightId) {
	if (lightId != this->lightId) {
		return;
	}

	auto& light = renderer->getLight(context, lightId);
	auto lightPosition = Vector3(light.position[0], light.position[1], light.position[2]);
	auto lightSpotDirection = Vector3(light.spotDirection[0], light.spotDirection[1], light.spotDirection[2]);
	if (renderUniformLightPosition != -1) renderer->setProgramUniformFloatVec3(context, renderUniformLightPosition, lightPosition.getArray());
	if (renderUniformLightDirection != -1) renderer->setProgramUniformFloatVec3(context, renderUniformLightDirection, lightSpotDirection.getArray());
	if (renderUniformLightSpotExponent != -1) renderer->setProgramUniformFloat(context, renderUniformLightSpotExponent, light.spotExponent);
	if (renderUniformLightSpotCosCutoff != -1) renderer->setProgramUniformFloat(context, renderUniformLightSpotCosCutoff, light.spotCosCutoff);
	if (renderUniformLightConstantAttenuation != -1) renderer->setProgramUniformFloat(context, renderUniformLightConstantAttenuation, light.constantAttenuation);
	if (renderUniformLightLinearAttenuation != -1) renderer->setProgramUniformFloat(context, renderUniformLightLinearAttenuation, light.linearAttenuation);
	if (renderUniformLightQuadraticAttenuation != -1) renderer->setProgramUniformFloat(context, renderUniformLightQuadraticAttenuation, light.quadraticAttenuation);
	if (renderer->isInstancedRenderingAvailable() == true) {
		renderer->setProgramUniformFloatMatrix4x4(context, renderUniformProjectionMatrix, renderer->getProjectionMatrix().getArray());
		renderer->setProgramUniformFloatMatrix4x4(context, renderUniformCameraMatrix, renderer->getCameraMatrix().getArray());
	}
	renderer->setProgramUniformFloat(context, renderUniformTexturePixelWidth, 1.0f / static_cast<float>(Engine::getShadowMapWidth()));
	renderer->setProgramUniformFloat(context, renderUniformTexturePixelHeight, 1.0f / static_cast<float>(Engine::getShadowMapHeight()));
}

void ShadowMapRenderShaderBaseImplementation::bindTexture(Renderer* renderer, void* context, int32_t textureId)
{
	switch (renderer->getTextureUnit(context)) {
		case LightingShaderConstants::SPECULAR_TEXTUREUNIT_DIFFUSE:
			renderer->setProgramUniformInteger(context, uniformDiffuseTextureAvailable, textureId == 0 ? 0 : 1);
			break;
	}
}

void ShadowMapRenderShaderBaseImplementation::setDepthBiasMVPMatrix(void* context, const Matrix4x4& depthBiasMVPMatrix)
{
	renderer->setProgramUniformFloatMatrix4x4(context, renderUniformDepthBiasMVPMatrix, depthBiasMVPMatrix.getArray());
}

void ShadowMapRenderShaderBaseImplementation::setRenderLightId(int32_t lightId) {
	this->lightId = lightId;
}
