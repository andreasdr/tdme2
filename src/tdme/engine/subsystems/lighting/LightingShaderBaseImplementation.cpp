#include <tdme/engine/subsystems/lighting/LightingShaderBaseImplementation.h>

#include <string>

#include <tdme/engine/subsystems/lighting/LightingShaderConstants.h>
#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/engine/Camera.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Timing.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/utilities/Console.h>

using std::string;
using std::to_string;

using tdme::engine::subsystems::lighting::LightingShaderBaseImplementation;
using tdme::engine::subsystems::lighting::LightingShaderConstants;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::Camera;
using tdme::engine::Engine;
using tdme::engine::Timing;
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
		renderer->setProgramAttributeLocation(programId, 0, "inVertex");
		renderer->setProgramAttributeLocation(programId, 1, "inNormal");
		renderer->setProgramAttributeLocation(programId, 2, "inTextureUV");
		renderer->setProgramAttributeLocation(programId, 4, "inOrigin");
	}

	// link program
	if (renderer->linkProgram(programId) == false) return;

	// get uniforms
	//	globals
	uniformTextureAtlasSize = renderer->getProgramUniformLocation(programId, "textureAtlasSize");
	uniformTextureAtlasPixelDimension = renderer->getProgramUniformLocation(programId, "textureAtlasPixelDimension");
	uniformDiffuseTextureUnit = renderer->getProgramUniformLocation(programId, "diffuseTextureUnit");
	uniformDiffuseTextureAvailable = renderer->getProgramUniformLocation(programId, "diffuseTextureAvailable");
	uniformDiffuseTextureMaskedTransparency = renderer->getProgramUniformLocation(programId, "diffuseTextureMaskedTransparency");
	uniformDiffuseTextureMaskedTransparencyThreshold = renderer->getProgramUniformLocation(programId, "diffuseTextureMaskedTransparencyThreshold");

	// additional mapping
	if (renderer->isSpecularMappingAvailable() == true) {
		uniformSpecularTextureUnit = renderer->getProgramUniformLocation(programId, "specularTextureUnit");
		uniformSpecularTextureAvailable = renderer->getProgramUniformLocation(programId, "specularTextureAvailable");

	}
	if (renderer->isNormalMappingAvailable() == true) {
		uniformNormalTextureUnit = renderer->getProgramUniformLocation(programId, "normalTextureUnit");
		uniformNormalTextureAvailable = renderer->getProgramUniformLocation(programId, "normalTextureAvailable");
	}

	// environment mapping
	uniformEnvironmentMappingTextureUnit = renderer->getProgramUniformLocation(programId, "environmentMappingTextureUnit");
	uniformEnvironmentMappingTextureAvailable = renderer->getProgramUniformLocation(programId, "environmentMappingTextureAvailable");
	uniformEnvironmentMappingPosition = renderer->getProgramUniformLocation(programId, "environmentMappingPosition");

	// texture matrix
	uniformTextureMatrix = renderer->getProgramUniformLocation(programId, "textureMatrix");
	if (uniformTextureMatrix == -1) return;

	// matrices as uniform only if not using instanced rendering
	if (renderer->isInstancedRenderingAvailable() == false) {
		uniformNormalMatrix = renderer->getProgramUniformLocation(programId, "normalMatrix");
		if (uniformNormalMatrix == -1) return;

		uniformModelMatrix = renderer->getProgramUniformLocation(programId, "modelMatrix");
		if (uniformModelMatrix == -1) return;

		uniformEffectColorMul = renderer->getProgramUniformLocation(programId, "effectColorMul");
		uniformEffectColorAdd = renderer->getProgramUniformLocation(programId, "effectColorAdd");
	}

	// camera, projection matrix
	uniformCameraMatrix = renderer->getProgramUniformLocation(programId, "cameraMatrix");
	if (uniformCameraMatrix == -1) return;
	uniformProjectionMatrix = renderer->getProgramUniformLocation(programId, "projectionMatrix");
	if (uniformProjectionMatrix == -1) return;

	//	material
	uniformMaterialAmbient = renderer->getProgramUniformLocation(programId, "material.ambient");
	uniformMaterialDiffuse = renderer->getProgramUniformLocation(programId, "material.diffuse");
	uniformMaterialSpecular = renderer->getProgramUniformLocation(programId, "material.specular");
	uniformMaterialEmission = renderer->getProgramUniformLocation(programId, "material.emission");
	uniformMaterialShininess = renderer->getProgramUniformLocation(programId, "material.shininess");
	uniformMaterialReflection = renderer->getProgramUniformLocation(programId, "material.reflection");

	//	lights
	for (auto i = 0; i < Engine::LIGHTS_MAX; i++) {
		uniformLightEnabled[i] = renderer->getProgramUniformLocation(programId, "lights[" + to_string(i) +"].enabled");
		uniformLightAmbient[i] = renderer->getProgramUniformLocation(programId,"lights[" + to_string(i) + "].ambient");
		uniformLightDiffuse[i] = renderer->getProgramUniformLocation(programId, "lights[" + to_string(i) + "].diffuse");
		uniformLightSpecular[i] = renderer->getProgramUniformLocation(programId, "lights[" + to_string(i) + "].specular");
		uniformLightPosition[i] = renderer->getProgramUniformLocation(programId, "lights[" + to_string(i) + "].position");
		uniformLightSpotDirection[i] = renderer->getProgramUniformLocation(programId, "lights[" + to_string(i) + "].spotDirection");
		uniformLightSpotExponent[i] = renderer->getProgramUniformLocation(programId, "lights[" + to_string(i) + "].spotExponent");
		uniformLightSpotCosCutoff[i] = renderer->getProgramUniformLocation(programId, "lights[" + to_string(i) + "].spotCosCutoff");
		uniformLightConstantAttenuation[i] = renderer->getProgramUniformLocation(programId, "lights[" + to_string(i) + "].constantAttenuation");
		uniformLightLinearAttenuation[i] = renderer->getProgramUniformLocation(programId, "lights[" + to_string(i) + "].linearAttenuation");
		uniformLightQuadraticAttenuation[i] = renderer->getProgramUniformLocation(programId, "lights[" + to_string(i) + "].quadraticAttenuation");
	}

	uniformMaterialReflectionFragmentShader = renderer->getProgramUniformLocation(programId, "materialReflection");

	// use foliage animation
	uniformTime = renderer->getProgramUniformLocation(programId, "time");

	//
	initialized = true;
}

void LightingShaderBaseImplementation::useProgram(Engine* engine, void* context)
{
	renderer->useProgram(context, programId);
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

	// environment mapping texture unit
	if (uniformEnvironmentMappingTextureUnit != -1) {
		renderer->setProgramUniformInteger(context, uniformEnvironmentMappingTextureUnit, LightingShaderConstants::SPECULAR_TEXTUREUNIT_ENVIRONMENT);
	}

	// frame
	if (uniformTime != -1) renderer->setProgramUniformFloat(context, uniformTime, static_cast<float>(engine->getTiming()->getTotalTime()) / 1000.0f);
}

void LightingShaderBaseImplementation::unUseProgram(void* context)
{
	renderer->setTextureUnit(context, LightingShaderConstants::SPECULAR_TEXTUREUNIT_DIFFUSE);
	renderer->bindTexture(context, renderer->ID_NONE);
	renderer->setTextureUnit(context, LightingShaderConstants::SPECULAR_TEXTUREUNIT_SPECULAR);
	renderer->bindTexture(context, renderer->ID_NONE);
	renderer->setTextureUnit(context, LightingShaderConstants::SPECULAR_TEXTUREUNIT_NORMAL);
	renderer->bindTexture(context, renderer->ID_NONE);
	renderer->setTextureUnit(context, 0);
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
	// reflection
	if (uniformMaterialReflection != -1) renderer->setProgramUniformFloat(context, uniformMaterialReflection, material.reflection);
	if (uniformMaterialReflectionFragmentShader != -1) renderer->setProgramUniformFloat(context, uniformMaterialReflectionFragmentShader, material.reflection);
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
	renderer->setProgramUniformFloatMatrix4x4(context, uniformProjectionMatrix, renderer->getProjectionMatrix().getArray());
	renderer->setProgramUniformFloatMatrix4x4(context, uniformCameraMatrix, renderer->getCameraMatrix().getArray());
	if (renderer->isInstancedRenderingAvailable() == false) {
		// matrices
		Matrix4x4 normalMatrix;
		// normal matrix
		normalMatrix.set(renderer->getModelViewMatrix()).invert().transpose();
		// upload matrices
		renderer->setProgramUniformFloatMatrix4x4(context, uniformNormalMatrix, normalMatrix.getArray());
		renderer->setProgramUniformFloatMatrix4x4(context, uniformModelMatrix, renderer->getModelViewMatrix().getArray());
	}
}

// TODO: shader parameters

void LightingShaderBaseImplementation::updateTextureMatrix(Renderer* renderer, void* context) {
	//
	renderer->setProgramUniformFloatMatrix3x3(context, uniformTextureMatrix, renderer->getTextureMatrix(context).getArray());
}

void LightingShaderBaseImplementation::updateShaderParameters(Renderer* renderer, void* context) {
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
		case LightingShaderConstants::SPECULAR_TEXTUREUNIT_ENVIRONMENT:
			if (uniformEnvironmentMappingTextureUnit != -1 && textureId != 0) {
				if (uniformEnvironmentMappingTextureAvailable != -1) {
					renderer->setProgramUniformInteger(context, uniformEnvironmentMappingTextureAvailable, 1);
				}
				if (uniformEnvironmentMappingPosition != -1) {
					renderer->setProgramUniformFloatVec3(context, uniformEnvironmentMappingPosition, renderer->getEnvironmentMappingCubeMapPosition(context));
				}
			} else {
				if (uniformEnvironmentMappingTextureAvailable != -1) renderer->setProgramUniformInteger(context, uniformEnvironmentMappingTextureAvailable, 0);
			}
	}
}
