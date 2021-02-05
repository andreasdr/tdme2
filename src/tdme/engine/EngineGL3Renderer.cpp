#include <tdme/engine/EngineGL3Renderer.h>

#include <tdme/engine/subsystems/earlyzrejection/EZRShader.h>
#include <tdme/engine/subsystems/lighting/LightingShader.h>
#include <tdme/engine/subsystems/lines/LinesShader.h>
#include <tdme/engine/subsystems/particlesystem/ParticlesShader.h>
#include <tdme/engine/subsystems/shadowmapping/ShadowMapping.h>
#include <tdme/engine/Engine.h>
#include <tdme/gui/renderer/GUIShader.h>

using tdme::engine::subsystems::earlyzrejection::EZRShader;
using tdme::engine::subsystems::lighting::LightingShader;
using tdme::engine::subsystems::lines::LinesShader;
using tdme::engine::subsystems::particlesystem::ParticlesShader;
using tdme::engine::subsystems::shadowmapping::ShadowMapping;
using tdme::engine::Engine;
using tdme::engine::EngineGL3Renderer;
using tdme::gui::renderer::GUIShader;

EngineGL3Renderer::EngineGL3Renderer(Engine* engine) :
	engine(engine)
{
}

void EngineGL3Renderer::onUpdateProjectionMatrix(void* context)
{
	if (Engine::lightingShader != nullptr)
		Engine::lightingShader->updateMatrices(context);

	if (Engine::particlesShader != nullptr)
		Engine::particlesShader->updateMatrices(context);

	if (Engine::linesShader != nullptr)
		Engine::linesShader->updateMatrices(context);

	if (Engine::currentEngine->shadowMapping != nullptr)
		Engine::currentEngine->shadowMapping->updateMatrices(context);

	if (Engine::ezrShader != nullptr)
		Engine::ezrShader->updateMatrices(context);
}

void EngineGL3Renderer::onUpdateCameraMatrix(void* context)
{
	if (Engine::lightingShader != nullptr)
		Engine::lightingShader->updateMatrices(context);

	if (Engine::particlesShader != nullptr)
		Engine::particlesShader->updateMatrices(context);

	if (Engine::linesShader != nullptr)
		Engine::linesShader->updateMatrices(context);

	if (Engine::currentEngine->shadowMapping != nullptr)
		Engine::currentEngine->shadowMapping->updateMatrices(context);

	if (Engine::ezrShader != nullptr)
		Engine::ezrShader->updateMatrices(context);
}

void EngineGL3Renderer::onUpdateModelViewMatrix(void* context)
{
	if (Engine::lightingShader != nullptr)
		Engine::lightingShader->updateMatrices(context);

	if (Engine::particlesShader != nullptr)
		Engine::particlesShader->updateMatrices(context);

	if (Engine::linesShader != nullptr)
		Engine::linesShader->updateMatrices(context);

	if (Engine::currentEngine->shadowMapping != nullptr)
		Engine::currentEngine->shadowMapping->updateMatrices(context);

	if (Engine::ezrShader != nullptr)
		Engine::ezrShader->updateMatrices(context);
}

void EngineGL3Renderer::onBindTexture(void* context, int32_t textureId)
{
	if (Engine::lightingShader != nullptr)
		Engine::lightingShader->bindTexture(context, textureId);

	if (Engine::guiShader != nullptr)
		Engine::guiShader->bindTexture(textureId);

	if (Engine::currentEngine->shadowMapping != nullptr)
		Engine::currentEngine->shadowMapping->bindTexture(context, textureId);

	if (Engine::ezrShader != nullptr)
		Engine::ezrShader->bindTexture(context, textureId);
}

void EngineGL3Renderer::onUpdateTextureMatrix(void* context)
{
	if (Engine::lightingShader != nullptr)
		Engine::lightingShader->updateTextureMatrix(context);

	if (Engine::currentEngine->shadowMapping != nullptr)
		Engine::currentEngine->shadowMapping->updateTextureMatrix(context);

	if (Engine::guiShader != nullptr)
		Engine::guiShader->updateTextureMatrix();

	if (Engine::ezrShader != nullptr)
		Engine::ezrShader->updateTextureMatrix(context);
}

void EngineGL3Renderer::onUpdateEffect(void* context)
{
	if (Engine::lightingShader != nullptr)
		Engine::lightingShader->updateEffect(context);

	if (Engine::particlesShader != nullptr)
		Engine::particlesShader->updateEffect(context);

	if (Engine::linesShader != nullptr)
		Engine::linesShader->updateEffect(context);

	if (Engine::guiShader != nullptr)
		Engine::guiShader->updateEffect();

}

void EngineGL3Renderer::onUpdateLight(void* context, int32_t lightId)
{
	if (Engine::lightingShader != nullptr)
		Engine::lightingShader->updateLight(context, lightId);

	if (Engine::currentEngine->shadowMapping != nullptr)
		Engine::currentEngine->shadowMapping->updateLight(context, lightId);
}

void EngineGL3Renderer::onUpdateMaterial(void* context)
{
	if (Engine::lightingShader != nullptr)
		Engine::lightingShader->updateMaterial(context);

	if (Engine::currentEngine->shadowMapping != nullptr)
		Engine::currentEngine->shadowMapping->updateMaterial(context);

	if (Engine::ezrShader != nullptr)
		Engine::ezrShader->updateMaterial(context);
}

void EngineGL3Renderer::onUpdateShader(void* context) {
	if (Engine::lightingShader != nullptr)
		Engine::lightingShader->setShader(context, shader);

	if (Engine::currentEngine->shadowMapping != nullptr)
		Engine::currentEngine->shadowMapping->setShader(context, shader);

	if (Engine::ezrShader != nullptr)
		Engine::ezrShader->setShader(context, shader);
}

void EngineGL3Renderer::onUpdateShaderParameters(void* context) {
	if (Engine::lightingShader != nullptr)
		Engine::lightingShader->updateShaderParameters(context);

	if (Engine::currentEngine->shadowMapping != nullptr)
		Engine::currentEngine->shadowMapping->updateShaderParameters(context);

	if (Engine::ezrShader != nullptr)
		Engine::ezrShader->updateShaderParameters(context);
}
