#include <tdme/engine/subsystems/shadowmapping/ShadowMapping.h>

#include <string>
#include <vector>

#include <tdme/engine/Engine.h>
#include <tdme/engine/Light.h>
#include <tdme/engine/subsystems/rendering/EntityRenderer.h>
#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/engine/subsystems/shadowmapping/ShadowMap.h>
#include <tdme/engine/subsystems/shadowmapping/ShadowMappingShaderPreImplementation.h>
#include <tdme/engine/subsystems/shadowmapping/ShadowMappingShaderPre.h>
#include <tdme/engine/subsystems/shadowmapping/ShadowMappingShaderRender.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Vector4.h>
#include <tdme/utils/Console.h>

using std::vector;
using std::string;
using std::to_string;

using tdme::engine::subsystems::shadowmapping::ShadowMapping;
using tdme::engine::Engine;
using tdme::engine::Light;
using tdme::engine::subsystems::rendering::EntityRenderer;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::subsystems::shadowmapping::ShadowMap;
using tdme::engine::subsystems::shadowmapping::ShadowMapping_RunState;
using tdme::engine::subsystems::shadowmapping::ShadowMappingShaderPre;
using tdme::engine::subsystems::shadowmapping::ShadowMappingShaderRender;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;
using tdme::math::Vector4;
using tdme::utils::Console;

ShadowMapping::ShadowMapping(Engine* engine, Renderer* renderer, EntityRenderer* object3DRenderer)
{
	this->engine = engine;
	this->renderer = renderer;
	this->object3DRenderer = object3DRenderer;
	shadowMaps.resize(engine->getLightCount());
	for (auto i = 0; i < shadowMaps.size(); i++) {
		shadowMaps[i] = nullptr;
	}
	depthBiasMVPMatrix.identity();
	runState = ShadowMapping_RunState::NONE;
}

ShadowMapping::~ShadowMapping() {
	for (auto i = 0; i < shadowMaps.size(); i++) {
		if (shadowMaps[i] != nullptr) delete shadowMaps[i];
	}
}

Engine* ShadowMapping::getEngine()
{
	return engine;
}

void ShadowMapping::reshape(int32_t width, int32_t height)
{
}

void ShadowMapping::createShadowMaps()
{
	runState = ShadowMapping_RunState::PRE;
	// disable color rendering, we only want to write to the Z-Buffer
	renderer->setColorMask(false, false, false, false);
	// render backfaces only, avoid self-shadowing
	renderer->setCullFace(renderer->CULLFACE_FRONT);
	// render to shadow maps
	for (auto i = 0; i < engine->getLightCount(); i++) {
		auto light = engine->getLightAt(i);
		if (light->isEnabled() == true) {
			// create shadow map for light, if required
			if (shadowMaps[i] == nullptr) {
				auto shadowMap = new ShadowMap(this, Engine::getShadowMapWidth(), Engine::getShadowMapHeight());
				shadowMap->initialize();
				shadowMaps[i] = shadowMap;
			}
			// render
			Engine::getShadowMappingShaderPre()->useProgram(engine);
			shadowMaps[i]->render(light);
			Engine::getShadowMappingShaderPre()->unUseProgram();
		} else {
			if (shadowMaps[i] != nullptr) {
				// dispose shadow map
				shadowMaps[i]->dispose();
				delete shadowMaps[i];
				shadowMaps[i] = nullptr;
			}
		}
	}
	// restore disable color rendering
	renderer->setColorMask(true, true, true, true);
	// restore render backfaces only
	renderer->setCullFace(renderer->CULLFACE_BACK);
	//
	runState = ShadowMapping_RunState::NONE;
}

ShadowMap* ShadowMapping::getShadowMap(int idx) {
	return shadowMaps[idx];
}

void ShadowMapping::renderShadowMaps(const vector<Object3D*>& visibleObjects)
{
	// only render for objects that receives shadows
	for (auto object: visibleObjects) {
		if (object->isReceivesShadows() == false) continue;
		visibleObjectsReceivingShadows.push_back(object);
	}

	//
	auto contextCount = renderer->isSupportingMultithreadedRendering() == true?Engine::getThreadCount():1;
	runState = ShadowMapping_RunState::RENDER;
	// render using shadow mapping program
	auto shader = Engine::getShadowMappingShaderRender();
	//	do not allow writing to depth buffer
	renderer->disableDepthBufferWriting();
	// user shader program
	shader->useProgram(engine);
	// render each shadow map
	for (auto i = 0; i < shadowMaps.size(); i++) {
		// skip on unused shadow mapping
		if (shadowMaps[i] == nullptr) continue;

		//
		auto shadowMap = shadowMaps[i];
		// set light to render
		shader->setRenderLightId(i);

		// setup depth textures to contexts
		for (auto j = 0; j < contextCount; j++) {
			// use default context
			auto context = renderer->getContext(j);
			// set up light shader uniforms
			shadowMap->updateDepthBiasMVPMatrix(context);
			// bind shadow map texture on shadow map texture unit
			auto textureUnit = renderer->getTextureUnit(context);
			renderer->setTextureUnit(context, ShadowMap::TEXTUREUNIT);
			shadowMap->bindDepthBufferTexture(context);
			// switch back to texture last unit
			renderer->setTextureUnit(context, textureUnit);
		}

		// render objects, enable blending
		//	will be disabled after rendering transparent faces
		renderer->enableBlending();
		// 	only opaque face entities as shadows will not be produced on transparent faces
		object3DRenderer->render(
			visibleObjectsReceivingShadows,
			false,
			EntityRenderer::RENDERTYPE_NORMALS |
			EntityRenderer::RENDERTYPE_TEXTUREARRAYS_DIFFUSEMASKEDTRANSPARENCY |
			EntityRenderer::RENDERTYPE_TEXTURES_DIFFUSEMASKEDTRANSPARENCY |
			EntityRenderer::RENDERTYPE_SHADOWMAPPING
		);
		//	disable blending
		renderer->disableBlending();
	}

	// unuse shader program
	shader->unUseProgram();

	// restore depth textures of contexts
	for (auto j = 0; j < contextCount; j++) {
		// use default context
		auto context = renderer->getContext(j);
		auto textureUnit = renderer->getTextureUnit(context);
		renderer->setTextureUnit(context, ShadowMap::TEXTUREUNIT);
		renderer->bindTexture(context, renderer->ID_NONE);
		renderer->setTextureUnit(context, textureUnit);
	}

	// restore render defaults
	renderer->disableBlending();
	renderer->enableDepthBufferWriting();

	//
	visibleObjectsReceivingShadows.clear();

	//
	runState = ShadowMapping_RunState::NONE;
}

void ShadowMapping::dispose()
{
	// dispose shadow mappings
	for (auto i = 0; i < shadowMaps.size(); i++) {
		if (shadowMaps[i] != nullptr) {
			shadowMaps[i]->dispose();
			delete shadowMaps[i];
			shadowMaps[i] = nullptr;
		}
	}
}

void ShadowMapping::startObjectTransformations(void* context, Matrix4x4& transformationsMatrix)
{
	if (runState != ShadowMapping_RunState::RENDER)
		return;

    // retrieve current model view matrix and put it on stack
	Matrix4x4 tmpMatrix;
	shadowTransformationsMatrix.set(depthBiasMVPMatrix);
	// set up new model view matrix
	tmpMatrix.set(depthBiasMVPMatrix);
	depthBiasMVPMatrix.set(transformationsMatrix).multiply(tmpMatrix);

	//
	updateDepthBiasMVPMatrix(context);
}

void ShadowMapping::endObjectTransformations()
{
	if (runState != ShadowMapping_RunState::RENDER)
		return;
	// set up new model view matrix
	depthBiasMVPMatrix.set(shadowTransformationsMatrix);
}

void ShadowMapping::updateTextureMatrix(void* context)
{
	if (runState == ShadowMapping_RunState::NONE) return;

	// upload
	{
		auto v = runState;
		if (v == ShadowMapping_RunState::PRE) {
			{
				Engine::getShadowMappingShaderPre()->updateTextureMatrix(context);
				goto end_switch0;
			}
		} else
		if (v == ShadowMapping_RunState::RENDER) {
			{
				Engine::getShadowMappingShaderRender()->updateTextureMatrix(context);
				goto end_switch0;
			}
		} else {
			{
				Console::println(string("ShadowMapping::updateTextureMatrix(): unsupported run state '" + to_string(runState)));
				goto end_switch0;;
			}
		}
		end_switch0:;
	}
}

void ShadowMapping::updateMatrices(void* context)
{
	if (runState == ShadowMapping_RunState::NONE) return;
	// delegate to shader
	{
		if (runState == ShadowMapping_RunState::PRE) {
			Engine::getShadowMappingShaderPre()->updateMatrices(context);
		} else
		if (runState == ShadowMapping_RunState::RENDER) {
			Engine::getShadowMappingShaderRender()->updateMatrices(context);
		} else {
			Console::println(string("ShadowMapping::updateMatrices(): unsupported run state '" + to_string(runState)));
		}
	}
}

void ShadowMapping::updateMaterial(void* context) {
	if (runState == ShadowMapping_RunState::NONE)
		return;
	{
		auto v = runState;
		if (v == ShadowMapping_RunState::PRE) {
			{
				Engine::getShadowMappingShaderPre()->updateMaterial(context);
				goto end_switch0;;
			}
		} else
		if (v == ShadowMapping_RunState::RENDER) {
			{
				Engine::getShadowMappingShaderRender()->updateMaterial(context);
				goto end_switch0;;
			}
		}
		end_switch0:;
	}
}

void ShadowMapping::updateLight(void* context, int32_t lightId) {
	if (runState == ShadowMapping_RunState::RENDER) {
		Engine::getShadowMappingShaderRender()->updateLight(context, lightId);
	}
}

void ShadowMapping::bindTexture(void* context, int32_t textureId) {
	if (runState == ShadowMapping_RunState::NONE)
		return;
	{
		auto v = runState;
		if (v == ShadowMapping_RunState::PRE) {
			{
				Engine::getShadowMappingShaderPre()->bindTexture(context, textureId);
				goto end_switch0;;
			}
		} else
		if (v == ShadowMapping_RunState::RENDER) {
			{
				Engine::getShadowMappingShaderRender()->bindTexture(context, textureId);
				goto end_switch0;;
			}
		}
		end_switch0:;
	}
}

void ShadowMapping::updateDepthBiasMVPMatrix(void* context, Matrix4x4& depthBiasMVPMatrix)
{
	if (runState != ShadowMapping_RunState::RENDER)
		return;
	// copy matrix
	this->depthBiasMVPMatrix.set(depthBiasMVPMatrix);
	// upload
	Engine::getShadowMappingShaderRender()->setProgramDepthBiasMVPMatrix(context, depthBiasMVPMatrix);
}

void ShadowMapping::updateDepthBiasMVPMatrix(void* context)
{
	if (runState != ShadowMapping_RunState::RENDER)
		return;

	// upload
	Engine::getShadowMappingShaderRender()->setProgramDepthBiasMVPMatrix(context, depthBiasMVPMatrix);
}

void ShadowMapping::setShader(void* context, const string& id) {
	{
		if (runState == ShadowMapping_RunState::NONE) {
			// no op
		} else
		if (runState == ShadowMapping_RunState::PRE) {
			Engine::getShadowMappingShaderPre()->setShader(context, id);
		} else
		if (runState == ShadowMapping_RunState::RENDER) {
			Engine::getShadowMappingShaderRender()->setShader(context, id);
		} else {
			Console::println(string("ShadowMapping::setShader(): unsupported run state '" + to_string(runState)));
		}
	}
}
