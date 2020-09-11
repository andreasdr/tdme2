#include <tdme/engine/subsystems/lighting/LightingShaderBaseImplementation.h>

#include <string>

#include <tdme/engine/Engine.h>
#include <tdme/engine/Timing.h>
#include <tdme/engine/subsystems/lighting/LightingShaderConstants.h>
#include <tdme/engine/subsystems/renderer/Renderer_Light.h>
#include <tdme/engine/subsystems/renderer/Renderer_SpecularMaterial.h>
#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/utilities/Console.h>

using std::to_string;
using std::string;

using tdme::engine::Engine;
using tdme::engine::Timing;
using tdme::engine::subsystems::lighting::LightingShaderConstants;
using tdme::engine::subsystems::lighting::LightingShaderBaseImplementation;
using tdme::engine::subsystems::renderer::Renderer_Light;
using tdme::engine::subsystems::renderer::Renderer_SpecularMaterial;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::math::Matrix4x4;
using tdme::utilities::Console;

LightingShaderBaseImplementation::LightingShaderBaseImplementation(Renderer* renderer)
{
	this->renderer = renderer;
	initialized = false;
}

bool LightingShaderBaseImplementation::isInitialized()
{
	return initialized;
}

void LightingShaderBaseImplementation::initialize()
{
	// map inputs to attributes
	if (renderer->isUsingProgramAttributeLocation() == true) {
		renderer->setProgramAttributeLocation(renderLightingProgramId, 0, "inVertex");
		renderer->setProgramAttributeLocation(renderLightingProgramId, 1, "inNormal");
		renderer->setProgramAttributeLocation(renderLightingProgramId, 2, "inTextureUV");
		renderer->setProgramAttributeLocation(renderLightingProgramId, 4, "inOrigin");
	}

	// link program
	if (renderer->linkProgram(renderLightingProgramId) == false) return;

	// get uniforms
	//	globals
	uniformTextureAtlasSize = renderer->getProgramUniformLocation(renderLightingProgramId, "textureAtlasSize");
	uniformTextureAtlasPixelDimension = renderer->getProgramUniformLocation(renderLightingProgramId, "textureAtlasPixelDimension");
	uniformDiffuseTextureUnit = renderer->getProgramUniformLocation(renderLightingProgramId, "diffuseTextureUnit");
	uniformDiffuseTextureAvailable = renderer->getProgramUniformLocation(renderLightingProgramId, "diffuseTextureAvailable");
	uniformDiffuseTextureMaskedTransparency = renderer->getProgramUniformLocation(renderLightingProgramId, "diffuseTextureMaskedTransparency");
	uniformDiffuseTextureMaskedTransparencyThreshold = renderer->getProgramUniformLocation(renderLightingProgramId, "diffuseTextureMaskedTransparencyThreshold");

	// additional mapping
	if (renderer->isSpecularMappingAvailable() == true) {
		uniformSpecularTextureUnit = renderer->getProgramUniformLocation(renderLightingProgramId, "specularTextureUnit");
		uniformSpecularTextureAvailable = renderer->getProgramUniformLocation(renderLightingProgramId, "specularTextureAvailable");

	}
	if (renderer->isNormalMappingAvailable() == true) {
		uniformNormalTextureUnit = renderer->getProgramUniformLocation(renderLightingProgramId, "normalTextureUnit");
		uniformNormalTextureAvailable = renderer->getProgramUniformLocation(renderLightingProgramId, "normalTextureAvailable");
	}

	// texture matrix
	uniformTextureMatrix = renderer->getProgramUniformLocation(renderLightingProgramId, "textureMatrix");
	if (uniformTextureMatrix == -1) return;

	// matrices as uniform only if not using instanced rendering
	if (renderer->isInstancedRenderingAvailable() == false) {
		uniformMVPMatrix = renderer->getProgramUniformLocation(renderLightingProgramId, "mvpMatrix");
		if (uniformMVPMatrix == -1) return;

		uniformMVMatrix = renderer->getProgramUniformLocation(renderLightingProgramId, "mvMatrix");
		if (uniformMVMatrix == -1) return;

		uniformNormalMatrix = renderer->getProgramUniformLocation(renderLightingProgramId, "normalMatrix");
		if (uniformNormalMatrix == -1) return;

		uniformModelMatrix = renderer->getProgramUniformLocation(renderLightingProgramId, "modelMatrix");
		if (uniformModelMatrix == -1) return;

		uniformEffectColorMul = renderer->getProgramUniformLocation(renderLightingProgramId, "effectColorMul");
		uniformEffectColorAdd = renderer->getProgramUniformLocation(renderLightingProgramId, "effectColorAdd");
	} else {
		uniformCameraMatrix = renderer->getProgramUniformLocation(renderLightingProgramId, "cameraMatrix");
		if (uniformCameraMatrix == -1) return;
		uniformProjectionMatrix = renderer->getProgramUniformLocation(renderLightingProgramId, "projectionMatrix");
		if (uniformProjectionMatrix == -1) return;
	}

	//	material
	uniformMaterialAmbient = renderer->getProgramUniformLocation(renderLightingProgramId, "material.ambient");
	uniformMaterialDiffuse = renderer->getProgramUniformLocation(renderLightingProgramId, "material.diffuse");
	uniformMaterialSpecular = renderer->getProgramUniformLocation(renderLightingProgramId, "material.specular");
	uniformMaterialEmission = renderer->getProgramUniformLocation(renderLightingProgramId, "material.emission");
	uniformMaterialShininess = renderer->getProgramUniformLocation(renderLightingProgramId, "material.shininess");

	//	lights
	for (auto i = 0; i < Engine::LIGHTS_MAX; i++) {
		uniformLightEnabled[i] = renderer->getProgramUniformLocation(renderLightingProgramId, "lights[" + to_string(i) +"].enabled");
		uniformLightAmbient[i] = renderer->getProgramUniformLocation(renderLightingProgramId,"lights[" + to_string(i) + "].ambient");
		uniformLightDiffuse[i] = renderer->getProgramUniformLocation(renderLightingProgramId, "lights[" + to_string(i) + "].diffuse");
		uniformLightSpecular[i] = renderer->getProgramUniformLocation(renderLightingProgramId, "lights[" + to_string(i) + "].specular");
		uniformLightPosition[i] = renderer->getProgramUniformLocation(renderLightingProgramId, "lights[" + to_string(i) + "].position");
		uniformLightSpotDirection[i] = renderer->getProgramUniformLocation(renderLightingProgramId, "lights[" + to_string(i) + "].spotDirection");
		uniformLightSpotExponent[i] = renderer->getProgramUniformLocation(renderLightingProgramId, "lights[" + to_string(i) + "].spotExponent");
		uniformLightSpotCosCutoff[i] = renderer->getProgramUniformLocation(renderLightingProgramId, "lights[" + to_string(i) + "].spotCosCutoff");
		uniformLightConstantAttenuation[i] = renderer->getProgramUniformLocation(renderLightingProgramId, "lights[" + to_string(i) + "].constantAttenuation");
		uniformLightLinearAttenuation[i] = renderer->getProgramUniformLocation(renderLightingProgramId, "lights[" + to_string(i) + "].linearAttenuation");
		uniformLightQuadraticAttenuation[i] = renderer->getProgramUniformLocation(renderLightingProgramId, "lights[" + to_string(i) + "].quadraticAttenuation");
	}

	// use foliage animation
	uniformTime = renderer->getProgramUniformLocation(renderLightingProgramId, "time");

	//
	initialized = true;
}

void LightingShaderBaseImplementation::useProgram(Engine* engine, void* context)
{
	renderer->useProgram(context, renderLightingProgramId);
	renderer->setLighting(context, renderer->LIGHTING_SPECULAR);
	// initialize static uniforms
	if (renderer->isInstancedRenderingAvailable() == true) {
		renderer->setProgramUniformFloatMatrix4x4(context, uniformProjectionMatrix, renderer->getProjectionMatrix().getArray());
		renderer->setProgramUniformFloatMatrix4x4(context, uniformCameraMatrix, renderer->getCameraMatrix().getArray());
	}
	if (uniformDiffuseTextureUnit != -1) {
		renderer->setProgramUniformInteger(context, uniformDiffuseTextureUnit, LightingShaderConstants::SPECULAR_TEXTUREUNIT_DIFFUSE);
	}
	if (renderer->isSpecularMappingAvailable() == true && uniformSpecularTextureUnit != -1) {
		renderer->setProgramUniformInteger(context, uniformSpecularTextureUnit, LightingShaderConstants::SPECULAR_TEXTUREUNIT_SPECULAR);
	}
	if (renderer->isNormalMappingAvailable() == true && uniformNormalTextureUnit != -1) {
		renderer->setProgramUniformInteger(context, uniformNormalTextureUnit, LightingShaderConstants::SPECULAR_TEXTUREUNIT_NORMAL);
	}
	// initialize dynamic uniforms
	updateEffect(renderer, context);
	updateMaterial(renderer, context);
	for (auto i = 0; i < Engine::LIGHTS_MAX; i++) {
		updateLight(renderer, context, i);
	}
	// frame
	if (uniformTime != -1) renderer->setProgramUniformFloat(context, uniformTime, static_cast<float>(engine->getTiming()->getTotalTime()) / 1000.0f);
}

void LightingShaderBaseImplementation::unUseProgram(void* context)
{
}

void LightingShaderBaseImplementation::updateEffect(Renderer* renderer, void* context)
{
	// skip if using instanced rendering
	if (renderer->isInstancedRenderingAvailable() == false) {
		//
		renderer->setProgramUniformFloatVec4(context, uniformEffectColorMul, renderer->getEffectColorMul(context));
		renderer->setProgramUniformFloatVec4(context, uniformEffectColorAdd, renderer->getEffectColorAdd(context));
	}
}

void LightingShaderBaseImplementation::updateMaterial(Renderer* renderer, void* context)
{
	//
	array<float, 4> tmpColor4 {{ 0.0f, 0.0f, 0.0f, 0.0f }};

	auto material = renderer->getSpecularMaterial(context);

	// ambient without alpha, as we only use alpha from diffuse color
	tmpColor4 = material.ambient;
	tmpColor4[3] = 0.0f;
	if (uniformMaterialAmbient != -1) renderer->setProgramUniformFloatVec4(context, uniformMaterialAmbient, tmpColor4);
	// diffuse
	if (uniformMaterialDiffuse != -1) renderer->setProgramUniformFloatVec4(context, uniformMaterialDiffuse, material.diffuse);
	// specular without alpha, as we only use alpha from diffuse color
	tmpColor4 = material.specular;
	tmpColor4[3] = 0.0f;
	if (uniformMaterialSpecular != -1) renderer->setProgramUniformFloatVec4(context, uniformMaterialSpecular, tmpColor4);
	// emission without alpha, as we only use alpha from diffuse color
	tmpColor4 = material.emission;
	tmpColor4[3] = 0.0f;
	if (uniformMaterialEmission != -1) renderer->setProgramUniformFloatVec4(context, uniformMaterialEmission, tmpColor4);
	// shininess
	if (uniformMaterialShininess != -1) renderer->setProgramUniformFloat(context, uniformMaterialShininess, material.shininess);
	// diffuse texture masked transparency
	if (uniformDiffuseTextureMaskedTransparency != -1) {
		renderer->setProgramUniformInteger(context, uniformDiffuseTextureMaskedTransparency, material.diffuseTextureMaskedTransparency);
	}
	// diffuse texture masked transparency threshold
	if (uniformDiffuseTextureMaskedTransparencyThreshold != -1) {
		renderer->setProgramUniformFloat(context, uniformDiffuseTextureMaskedTransparencyThreshold, material.diffuseTextureMaskedTransparencyThreshold);
	}
	// texture atlas size
	if (uniformTextureAtlasSize != -1) {
		renderer->setProgramUniformInteger(context, uniformTextureAtlasSize, material.textureAtlasSize);
	}
	// texture atlas pixel dimension
	if (uniformTextureAtlasPixelDimension != -1) {
		renderer->setProgramUniformFloatVec2(context, uniformTextureAtlasPixelDimension, material.textureAtlasPixelDimension);
	}
}

void LightingShaderBaseImplementation::updateLight(Renderer* renderer, void* context, int32_t lightId)
{
	// lights
	auto& light = renderer->getLight(context, lightId);
	if (uniformLightEnabled[lightId] != -1) renderer->setProgramUniformInteger(context, uniformLightEnabled[lightId], light.enabled);
	if (light.enabled == 1) {
		if (uniformLightAmbient[lightId] != -1) renderer->setProgramUniformFloatVec4(context, uniformLightAmbient[lightId], light.ambient);
		if (uniformLightDiffuse[lightId] != -1) renderer->setProgramUniformFloatVec4(context, uniformLightDiffuse[lightId], light.diffuse);
		if (uniformLightSpecular[lightId] != -1) renderer->setProgramUniformFloatVec4(context, uniformLightSpecular[lightId], light.specular);
		if (uniformLightPosition[lightId] != -1) renderer->setProgramUniformFloatVec4(context, uniformLightPosition[lightId], light.position);
		if (uniformLightSpotDirection[lightId] != -1) renderer->setProgramUniformFloatVec3(context, uniformLightSpotDirection[lightId], light.spotDirection);
		if (uniformLightSpotExponent[lightId] != -1) renderer->setProgramUniformFloat(context, uniformLightSpotExponent[lightId], light.spotExponent);
		if (uniformLightSpotCosCutoff[lightId] != -1) renderer->setProgramUniformFloat(context, uniformLightSpotCosCutoff[lightId], light.spotCosCutoff);
		if (uniformLightConstantAttenuation[lightId] != -1) renderer->setProgramUniformFloat(context, uniformLightConstantAttenuation[lightId], light.constantAttenuation);
		if (uniformLightLinearAttenuation[lightId] != -1) renderer->setProgramUniformFloat(context, uniformLightLinearAttenuation[lightId], light.linearAttenuation);
		if (uniformLightQuadraticAttenuation[lightId] != -1) renderer->setProgramUniformFloat(context, uniformLightQuadraticAttenuation[lightId], light.quadraticAttenuation);
	}
}

void LightingShaderBaseImplementation::updateMatrices(Renderer* renderer, void* context)
{
	// set up camera and projection matrices if using instanced rendering
	if (renderer->isInstancedRenderingAvailable() == true) {
		renderer->setProgramUniformFloatMatrix4x4(context, uniformProjectionMatrix, renderer->getProjectionMatrix().getArray());
		renderer->setProgramUniformFloatMatrix4x4(context, uniformCameraMatrix, renderer->getCameraMatrix().getArray());
	} else
	if (renderer->isInstancedRenderingAvailable() == false) {
		// matrices
		Matrix4x4 mvMatrix;
		Matrix4x4 mvpMatrix;
		Matrix4x4 normalMatrix;
		// model view matrix
		mvMatrix.set(renderer->getModelViewMatrix()).multiply(renderer->getCameraMatrix());
		// object to screen matrix
		mvpMatrix.set(mvMatrix).multiply(renderer->getProjectionMatrix());
		// normal matrix
		normalMatrix.set(renderer->getModelViewMatrix()).invert().transpose();
		// upload matrices
		renderer->setProgramUniformFloatMatrix4x4(context, uniformMVPMatrix, mvpMatrix.getArray());
		renderer->setProgramUniformFloatMatrix4x4(context, uniformMVMatrix, mvMatrix.getArray());
		renderer->setProgramUniformFloatMatrix4x4(context, uniformNormalMatrix, normalMatrix.getArray());
		renderer->setProgramUniformFloatMatrix4x4(context, uniformModelMatrix, renderer->getModelViewMatrix().getArray());
	}
}

// TODO: shader parameters

void LightingShaderBaseImplementation::updateTextureMatrix(Renderer* renderer, void* context) {
	//
	renderer->setProgramUniformFloatMatrix3x3(context, uniformTextureMatrix, renderer->getTextureMatrix(context).getArray());
}

void LightingShaderBaseImplementation::bindTexture(Renderer* renderer, void* context, int32_t textureId)
{
	switch (renderer->getTextureUnit(context)) {
		case LightingShaderConstants::SPECULAR_TEXTUREUNIT_DIFFUSE:
			if (uniformDiffuseTextureAvailable != -1) {
				renderer->setProgramUniformInteger(context, uniformDiffuseTextureAvailable, textureId == 0 ? 0 : 1);
			}
			break;
		case LightingShaderConstants::SPECULAR_TEXTUREUNIT_SPECULAR:
			if (renderer->isSpecularMappingAvailable() == false)
				break;

			if (uniformSpecularTextureAvailable != -1) renderer->setProgramUniformInteger(context, uniformSpecularTextureAvailable, textureId == 0 ? 0 : 1);
			break;
		case LightingShaderConstants::SPECULAR_TEXTUREUNIT_NORMAL:
			if (renderer->isNormalMappingAvailable() == false)
				break;

			if (uniformNormalTextureAvailable != -1) renderer->setProgramUniformInteger(context, uniformNormalTextureAvailable, textureId == 0 ? 0 : 1);
			break;
	}
}
