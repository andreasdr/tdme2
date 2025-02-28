#pragma once

#include <array>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/subsystems/lighting/fwd-tdme.h>
#include <tdme/engine/subsystems/lighting/LightingShaderConstants.h>
#include <tdme/engine/subsystems/lighting/LightingShaderImplementation.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/engine/Engine.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Matrix4x4.h>

using std::array;

using tdme::engine::subsystems::lighting::LightingShaderConstants;
using tdme::engine::subsystems::lighting::LightingShaderImplementation;
using tdme::engine::subsystems::renderer::RendererBackend;
using tdme::engine::Engine;
using tdme::math::Matrix4x4;

/**
 * Lighting shader implementation
 * @author Andreas Drewke
 */
class tdme::engine::subsystems::lighting::LightingShaderBaseImplementation: public LightingShaderImplementation
{
protected:
	int32_t programId { -1 };
	int32_t fragmentShaderId { -1 };
	int32_t vertexShaderId { -1 };
	int32_t uniformTextureAtlasSize { -1 };
	int32_t uniformTextureAtlasPixelDimension { -1 };
	int32_t uniformDiffuseTextureUnit { -1 };
	int32_t uniformDiffuseTextureAvailable { -1 };
	int32_t uniformDiffuseTextureMaskedTransparency { -1 };
	int32_t uniformDiffuseTextureMaskedTransparencyThreshold { -1 };
	int32_t uniformSpecularTextureUnit { -1 };
	int32_t uniformSpecularTextureAvailable { -1 };
	int32_t uniformNormalTextureUnit { -1 };
	int32_t uniformNormalTextureAvailable { -1 };
	int32_t uniformProjectionMatrix { -1 };
	int32_t uniformCameraMatrix { -1 };
	int32_t uniformCameraPosition { -1 };
	int32_t uniformMVPMatrix { -1 };
	int32_t uniformMVMatrix { -1 };
	int32_t uniformNormalMatrix { -1 };
	int32_t uniformModelMatrix { -1 };
	int32_t uniformTextureMatrix { -1 };
	int32_t uniformEffectColorMul { -1 };
	int32_t uniformEffectColorAdd { -1 };
	int32_t uniformMaterialAmbient { -1 };
	int32_t uniformMaterialDiffuse { -1 };
	int32_t uniformMaterialSpecular { -1 };
	int32_t uniformMaterialEmission { -1 };
	int32_t uniformMaterialShininess { -1 };
	int32_t uniformMaterialReflection { -1 };
	int32_t uniformMaterialReflectionFragmentShader { -1 };
	int32_t uniformTime { -1 };
	int32_t uniformApplyFoliageAnimation { -1 };
	int32_t uniformEnvironmentMappingTextureUnit { -1 };
	int32_t uniformEnvironmentMappingTextureAvailable { -1 };
	int32_t uniformEnvironmentMappingPosition { -1 };
	array<int32_t, Engine::LIGHTS_MAX> uniformLightEnabled;
	array<int32_t, Engine::LIGHTS_MAX> uniformLightAmbient;
	array<int32_t, Engine::LIGHTS_MAX> uniformLightDiffuse;
	array<int32_t, Engine::LIGHTS_MAX> uniformLightSpecular;
	array<int32_t, Engine::LIGHTS_MAX> uniformLightPosition;
	array<int32_t, Engine::LIGHTS_MAX> uniformLightSpotDirection;
	array<int32_t, Engine::LIGHTS_MAX> uniformLightSpotExponent;
	array<int32_t, Engine::LIGHTS_MAX> uniformLightSpotCosCutoff;
	array<int32_t, Engine::LIGHTS_MAX> uniformLightConstantAttenuation;
	array<int32_t, Engine::LIGHTS_MAX> uniformLightLinearAttenuation;
	array<int32_t, Engine::LIGHTS_MAX> uniformLightQuadraticAttenuation;
	array<int32_t, Engine::LIGHTS_MAX> uniformLightRadius;
	array<float, 4> defaultSceneColor {{ 0.0f, 0.0f, 0.0f, 0.0f }};
	bool initialized { false };
	RendererBackend* rendererBackend { nullptr };

public:
	// forbid class copy
	FORBID_CLASS_COPY(LightingShaderBaseImplementation)

	/**
	 * Public constructor
	 * @param rendererBackend renderer backend
	 */
	LightingShaderBaseImplementation(RendererBackend* rendererBackend);

	// overridden methods
	virtual bool isInitialized() override;
	virtual void initialize() override;
	virtual void useProgram(Engine* engine, int contextIdx) override;
	virtual void unUseProgram(int contextIdx) override;
	virtual void updateEffect(RendererBackend* rendererBackend, int contextIdx) override;
	virtual void updateMaterial(RendererBackend* rendererBackend, int contextIdx) override;
	virtual void updateLight(RendererBackend* rendererBackend, int contextIdx, int32_t lightId) override;
	virtual void updateMatrices(RendererBackend* rendererBackend, int contextIdx) override;
	virtual void updateTextureMatrix(RendererBackend* rendererBackend, int contextIdx) override;
	virtual void bindTexture(RendererBackend* rendererBackend, int contextIdx, int32_t textureId) override;
	virtual void updateShaderParameters(RendererBackend* rendererBackend, int contextIdx) override = 0;
	virtual void unloadTextures() override;
	virtual void loadTextures(const string& pathName) override;

};
